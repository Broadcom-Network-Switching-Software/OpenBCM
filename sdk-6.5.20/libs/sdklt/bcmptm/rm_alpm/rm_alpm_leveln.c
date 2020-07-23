/*! \file rm_alpm_leveln.c
 *
 * Resource Manager for ALPM Level-N
 *
 * This file contains the resource manager for ALPM Level-N
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_util.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmltd/bcmltd_lt_types.h>

#include "rm_alpm_common.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_device.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_level1.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

#define LN_INFO(u, m)              (ln_info[u][m])
#define LN_SID(u, m, l)            (LN_INFO(u, m)->ln_sid[l])
#define LN_HIT_SID(u, m, l)        (LN_INFO(u, m)->ln_hit_sid[l])
#define LINFOS(u, m)               (LN_INFO(u, m)->lvl_hdl)
#define LVL_HDL(u, m, d, l)        (LN_INFO(u, m)->lvl_hdl[d][l])
#define LN_INFO_FD(u, m, _f)       (LN_INFO(u, m)->_f)
#define LN_INFO_FDM(u, m, _f, _i)  ((LN_INFO(u, m)->_f)[(_i)])
#define LINFO_INDEX(u, m, d, l)    (LVL_HDL(u, m, d, l)->level_idx)
#define LINFO_STATS(u, m, d, l)    (LVL_HDL(u, m, d, l)->level_stats)
#define MAX_SPLIT_ATTEMPTS      10
#define MAX_DELAY_ATTEMPTS      10

#define LN_HIT_DT_BITMAP        0
/*******************************************************************************
 * Typedefs
 */

/* Propagation data structure */
typedef struct propa_data_s {
    key_tuple_t t;
    alpm_route_t *bpm;
    int db;
    int u;
    int m;
    trie_propagate_op_t propa_op;
    alpm_arg_t *arg;
    int8_t level;
} propa_data_t;

typedef struct pivot_traverse_data_s {
    int u;
    int m;
    int db;
    rm_alpm_trie_t *route_trie;
} pivot_traverse_data_t;

typedef struct pivot_trie_sanitize_ctrl_s {
    int unit;
    int db;
    int m;
    int ln;
    rm_alpm_trie_t *pivot_trie[MAX_LEVELS];
} pivot_trie_sanitize_ctrl_t;
/*******************************************************************************
 * Private variables
 */
static alpm_ln_info_t *ln_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_internals_t
    bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_rm_alpm_ln_ha_t *ln_ha[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
/*******************************************************************************
 * Private Functions (declaration)
 */
static int alpm_insert(int u, int m, int8_t ln, alpm_arg_t *arg,
                       bkt_split_aux_t *split_aux);
static int alpm_delete(int u, int m, int8_t ln, alpm_arg_t *arg);
/*******************************************************************************
 * Private Functions (definition)
 */


static inline void
format_get(int u, int m, alpm_arg_t *arg)
{
    if (LN_INFO_FD(u, m, alpm_ver) != ALPM_VERSION_0) {
        return;
    }
    if (arg->key.t.ipv == IP_VER_6) {
        if (bcmptm_rm_alpm_pkm128_is_set(u, m)) {
            arg->fmt = VER0_FORMAT_5;
        } else {
            arg->fmt = VER0_FORMAT_2;
        }
    } else {
        arg->fmt = VER0_FORMAT_1;
    }
}


/*!
 * \brief Parameters check for insert
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
insert_validate(int u, int m, alpm_arg_t *arg)
{
    if (LN_INFO_FD(u, m, alpm_ver) == ALPM_VERSION_0) {
        if (arg->key.vt != VRF_GLOBAL && arg->key.t.app != APP_COMP0) {
            /* Parallel search mode protection */
            if (bcmptm_rm_alpm_is_parallel(u, m)) {
                /* Cannot have VRF=0 in parallel mode */
                if (arg->key.t.w_vrf == 0) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(u,
                                          "VRF=0 cannot be added in parallel mode\n")));
                    return SHR_E_PARAM;
                }
            }
        }
        if (!bcmptm_rm_alpm_pkm128_is_set(u, m)) {
            if (arg->key.t.len > 64) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(u,
                                      "Ip_len %d: must configure Key-Input-128 "
                                      "for ip_len > 64 \n"),
                           arg->key.t.len));
                return SHR_E_PARAM;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Parameters check for delete
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
delete_validate(int u, int m, alpm_arg_t *arg)
{
    return SHR_E_NONE;
}



/*!
 * \brief Init propagation data
 *
 * \param [in] u Device unit
 * \param [in] arg ALPM arg
 * \param [in] bpm Best prefix match
 * \param [in] op Propagation op code
 * \param [out] propa_data Propagation data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static void
propa_data_init(int u, int m, alpm_arg_t *arg,
                alpm_route_t *bpm,
                trie_propagate_op_t op,
                propa_data_t *propa_data)
{
    propa_data->t = arg->key.t;
    propa_data->bpm = bpm;
    propa_data->db = arg->db;
    propa_data->m = m;
    propa_data->u = u;
    propa_data->propa_op = op;
    if (op == PROPOGATE_HIT) {
        propa_data->arg = arg;
    }
    return;
}

/*!
 * \brief Callback for insert propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
insert_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* INSERT propagation targets pivot whose bpm_len <= new ip_len */

    /*
     * pivot->bkt->bpm->key.t.len == pfx_len is possible only when
     * this pivot is just created during split, and it happens to
     * be same as the new prefix.
     */
    if (pivot->bkt->bpm->key.t.len > pfx_len) {
        return SHR_E_NONE;
    }
    pivot->bkt->bpm = propa_data->bpm;
    ALPM_ASSERT(pfx_len == pivot->bkt->bpm->key.t.len);
    return bcmptm_rm_alpm_pivot_update(propa_data->u, propa_data->m, propa_data->db,
                                       propa_data->level, pivot, NULL,
                                       UPDATE_DATA);
}

/*!
 * \brief Callback for delete propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
delete_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* DELETE propagation targets pivot whose bpm_len == deleting ip_len */
    if (pivot->bkt->bpm->key.t.len != pfx_len) {
        return SHR_E_NONE;
    }
    pivot->bkt->bpm = propa_data->bpm;
    ALPM_ASSERT(pfx_len > pivot->bkt->bpm->key.t.len);
    return bcmptm_rm_alpm_pivot_update(propa_data->u, propa_data->m, propa_data->db,
                                       propa_data->level, pivot, NULL,
                                       UPDATE_DATA);
}

/*!
 * \brief Callback for update propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
update_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* UPDATE propagation targets pivot whose bpm_len == updating ip_len */
    if (pivot->bkt->bpm->key.t.len != pfx_len) {
        return SHR_E_NONE;
    }
    /* BPM does not change, thus skip updating it. */
    return bcmptm_rm_alpm_pivot_update(propa_data->u, propa_data->m, propa_data->db,
                                       propa_data->level, pivot, NULL,
                                       UPDATE_DATA);
}

/*!
 * \brief Callback for hit propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
hit_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int rv, pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;
    alpm_arg_t *arg;
    int8_t ln;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* HIT propagation targets pivot whose bpm_len == target ip_len */
    if (pivot->bkt->bpm->key.t.len != pfx_len) {
        return SHR_E_NONE;
    }

    ln = propa_data->level;
    arg = propa_data->arg;
    arg->pivot[ln] = pivot;
    arg->index[ln] = arg->pivot[ln]->index;
    arg->decode_data_only = 1;
    if (ln == LEVEL1) {
        rv = bcmptm_rm_alpm_l1_read(propa_data->u, propa_data->m, arg);
    } else {
        rv = bcmptm_rm_alpm_bucket_entry_read(propa_data->u, propa_data->m, ln, arg);
    }

    if (arg->hit && SHR_SUCCESS(rv)) {
        return SHR_E_LIMIT; /* Break early */
    }

    return rv;
}

/*!
 * \brief Software propagation
 *
 * \param [in] u Device u.
 * \param [in] propa_op Type of propagation
 * \param [in] arg ALPM arg for adding/deleting/updating entry
 * \param [in] bpm Best prefix match
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_propa(int u, int m, trie_propagate_op_t propa_op,
           alpm_arg_t *arg, alpm_route_t *bpm)
{
    int level;
    rm_alpm_trie_t *pivot_trie;
    propa_data_t propa_data;
    int (*cb) (rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info);

    SHR_FUNC_ENTER(u);

    propa_data_init(u, m, arg, bpm, propa_op, &propa_data);

    switch (propa_op) {
    case PROPOGATE_INSERT: cb = insert_propa_cb; break;
    case PROPOGATE_DELETE: cb = delete_propa_cb; break;
    case PROPOGATE_UPDATE: cb = update_propa_cb; break;
    case PROPOGATE_HIT: cb = hit_propa_cb; break;
    default: cb = NULL; break;
    }

    for (level = 0; level < DB_LAST_LEVEL(u, m, arg->db); level++) {
        rm_alpm_trie_bpm_cb_info_t cb_info;

        propa_data.level = level;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, m, arg->db, level,
                                        arg->key.t.w_vrf,
                                        arg->key.t.ipv, &pivot_trie));
        ALPM_ASSERT(pivot_trie);

        cb_info.user_data = &propa_data;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_propagate(pivot_trie,
                                          &propa_data.t.trie_ip.key[0],
                                          propa_data.t.len,
                                          propa_op,
                                          cb,
                                          &cb_info,
                                          false));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete an existing entry on a given level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for deleting entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_delete(int u, int m, int8_t ln, alpm_arg_t *arg)
{
    int rv = SHR_E_NONE;
    ln_index_t ent_idx;
    alpm_pivot_t *pivot = NULL;
    alpm_bkt_elem_t *bkt_elem = NULL;
    alpm_pivot_t *pivot_elem = NULL;
    alpm_bkt_elem_t *bpm_elem;
    int8_t prev_ln = ln - 1;
    uint8_t db = arg->db;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(prev_ln >= LEVEL1);
    if (arg->pivot[prev_ln] == NULL) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, m, db, prev_ln, &arg->key.t, &pivot));
        arg->pivot[prev_ln] = pivot;
        if (prev_ln == LEVEL2) {
            /* For table_op_pt_info only. */
            arg->pivot[LEVEL1] =
                bcmptm_rm_alpm_bucket_pivot_get(u, m, db, prev_ln, pivot->index);
        }
    } else {
        pivot = arg->pivot[prev_ln];
    }

#ifdef ALPM_DEBUG
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_delete(u, m, ln, &arg->key.t, pivot->bkt, &bkt_elem));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_delete(u, m, db, ln, arg->a1.direct_route,
                                  &arg->key.t, &pivot_elem));
    ALPM_ASSERT(bkt_elem == pivot_elem);
    /* Save the group so we can free later */
    if (bkt_elem->bkt) {
        arg->log_bkt[ln] = bkt_elem->bkt->log_bkt;
    }
    ent_idx = bkt_elem->index;

    /* Find the bpm_elem after bucket entry got deleted */
    rv = bcmptm_rm_alpm_pvt_find(u, m, db,
                                 DB_LAST_LEVEL(u, m, db),
                                 &arg->key.t,
                                 &bpm_elem);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    if (rv == SHR_E_NOT_FOUND) {
        /* For parallel mode, now also for combined mode. */
        bpm_elem = VRF_HDL_DFT(u, m, db, arg->key.t.ipv, arg->key.t.w_vrf);
    }

    if (ln == DB_LAST_LEVEL(u, m, db)) {
        SHR_IF_ERR_EXIT(
            alpm_propa(u, m, PROPOGATE_DELETE, arg, bpm_elem));
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_free(u, m, db, ln, false, bkt_elem));

    /*
     * Delete bucket entry after propagation done.
     * Then it is fine to delete bucket entry before deleting pivot.
     */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_delete(u, m, db, ln, pivot, ent_idx));
    arg->orig_index = ent_idx; /* For table_op_pt_info. */

#ifdef ALPM_DEBUG
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
#endif

    if (pivot->bkt->trie->trie == NULL || pivot->bkt->trie->trie->count == 1) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, m, db, ln, &pivot));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert a delayed pivot to prevent mis-forwarding.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] pre_pivot Level1 pivot
 * \param [out] pivot Level2 pivot
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_delayed_pivot_insert(int u, int m, int db, alpm_pivot_t *pre_pivot,
                          alpm_pivot_t **pivot)
{
    int rv;
    alpm_arg_t *split_arg = NULL;
    bkt_split_aux_t split_aux = {0};

    SHR_FUNC_ENTER(u);
    split_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    SHR_NULL_CHECK(split_arg, SHR_E_MEMORY);
    bcmptm_rm_alpm_split_arg_init(u, m, db, LEVEL2,
                                  INVALID_INDEX,
                                  &pre_pivot->key,
                                  &pre_pivot->key.t.trie_ip,
                                  pre_pivot->key.t.len,
                                  INVALID_LOG_BKT,
                                  split_arg);

    rv = bcmptm_rm_alpm_split_pivot_add(u, m, db, LEVEL2, split_arg,
                                        pivot, &split_aux);
    if (rv == SHR_E_PARTIAL) {
        ALPM_ASSERT(split_aux.epartial_reason == EPARTIAL_REASON_L3BKTAC_SPLIT);
        ALPM_ASSERT(split_aux.l3_pivot);
        sal_memset(&split_aux, 0, sizeof(split_aux));
        /* Just retry with payload split */
        split_aux.delayed_payload_spilt = 1;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_split_pivot_add(u, m, db, LEVEL2, split_arg,
                                           pivot, &split_aux));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    ALPM_ASSERT(pre_pivot->key.t.len == (*pivot)->key.t.len);

exit:
    if (split_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], split_arg);
    }
    SHR_FUNC_EXIT();
}


static int
alpm_delayed_pivot_handle(int u, int m, int db, int prev_ln, alpm_arg_t *arg,
                          bool resource_check)
{
    int delay_attempt;
    alpm_pivot_t *pre_pivot;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);

    pivot = arg->pivot[prev_ln];
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_find(u, m, db, LEVEL1, &arg->key.t, &pre_pivot));
    delay_attempt = 0;

    /*
     * The delayed insert could cause a bucket split generating a new
     * L1 pivot becoming the new pivot for the inserting entry.
     * We need recompare the L2 pivot to ensure the pivot len
     * requirement, if it does not repeat the process.
     */
    if (pre_pivot->key.t.len > pivot->key.t.len && resource_check) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_alpm_bucket_resource_check(u, m, arg, EXTRA_NEW_INSERT));
    }
    while (pre_pivot->key.t.len > pivot->key.t.len) {
        SHR_IF_ERR_EXIT(
            alpm_delayed_pivot_insert(u, m, db, pre_pivot, &pivot));
        arg->pivot[LEVEL2] = pivot;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, m, db, LEVEL1, &arg->key.t, &pre_pivot));
        delay_attempt++;
        if (delay_attempt > MAX_DELAY_ATTEMPTS) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u,
                              "%d delay attempts fail\n"),
                   delay_attempt));
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    arg->pivot[LEVEL1] = pre_pivot;
    ALPM_ASSERT(pre_pivot ==
        bcmptm_rm_alpm_bucket_pivot_get(u, m, db, prev_ln, pivot->index));


exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Insert a new entry to bucket
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for new entry
 * \param [in] split_aux Aux info for split.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_bucket_insert(int u, int m, int ln, alpm_arg_t *arg, bkt_split_aux_t *split_aux)
{
    int rv;
    int attempt;
    alpm_pivot_t *pivot;
    alpm_bkt_elem_t *bkt_elem;
    int bkt_inserted = !arg->dt_update;
    int8_t prev_ln;

    SHR_FUNC_ENTER(u);
    pivot = arg->pivot[ln - 1];
    arg->state_changed = 1;
    rv = bcmptm_rm_alpm_bucket_insert(u, m, ln, arg, pivot);
    if (rv == SHR_E_RESOURCE && arg->resouce_full) {
        arg->state_changed = 0;
        SHR_ERR_EXIT(rv);
    }

    if (rv == SHR_E_RESOURCE && pivot->bkt->log_bkt != INVALID_LOG_BKT) {
        for (attempt = 0; attempt < MAX_SPLIT_ATTEMPTS; attempt++) {
            rv = bcmptm_rm_alpm_bucket_split_insert(u, m, ln, arg, pivot, split_aux);
            if (SHR_SUCCESS(rv)) {
                if (bkt_inserted == 0) {
                    prev_ln = PREV_LEVEL(ln);
                    /* It was a non-lazy split, thus we need insert entry again */
                    /* But before inserting, update pivot as it might be changed during split. */
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_pvt_find(u, m, arg->db, prev_ln,
                                                &arg->key.t, &pivot));
                    arg->pivot[prev_ln] = pivot;
                    if (prev_ln == LEVEL2) {
                        SHR_IF_ERR_EXIT(
                            alpm_delayed_pivot_handle(u, m, arg->db, prev_ln, arg, false));
                        pivot = arg->pivot[prev_ln];
                    }

                    if (!arg->dt_update) {
                        SHR_IF_ERR_EXIT(
                            bcmptm_rm_alpm_bkt_insert(u, m, ln, &arg->key, pivot->bkt,
                                                      bkt_elem));
                        bkt_inserted = 1;
                    }
                    sal_memset(split_aux, 0, sizeof(*split_aux));
                    rv = bcmptm_rm_alpm_bucket_insert(u, m, ln, arg, pivot);
                    if (rv != SHR_E_RESOURCE) {
                        SHR_ERR_EXIT(rv);
                    }
                } else {
                    ALPM_ASSERT(arg->index[ln] >= 0);
                    break;
                }
            } else if (rv == SHR_E_RESOURCE && split_aux) {
                if (split_aux->retry == SPLIT_RETRY_ONE) {
                    /* Imply new in parent, we are just fine to continue split */
                } else if (split_aux->retry == SPLIT_RETRY_TWO) {
                    if (bkt_inserted == 1) {
                        SHR_IF_ERR_EXIT(
                            bcmptm_rm_alpm_bkt_delete(u, m, ln, &arg->key.t,
                                                      pivot->bkt, &bkt_elem));
                    }
                    bkt_inserted = 0;
                } else {
                    /* Must be an error. */
                    SHR_ERR_EXIT(rv);
                }
                /* Reset aux to start a fresh new split. */
                sal_memset(split_aux, 0, sizeof(*split_aux));
            } else if (rv == SHR_E_PARTIAL && ln == DB_LAST_LEVEL(u, m, arg->db)) {

                if (split_aux->epartial_reason == EPARTIAL_REASON_L3BKTAC_SPLIT) {
                    if (split_aux->l3_pivot &&
                        split_aux->l3_pivot != pivot) {
                        /* Must be a different bucket (c) than target bucket (a) */
                        rv = bcmptm_rm_alpm_bucket_split_insert(u, m, ln, arg,
                                split_aux->l3_pivot, split_aux);
                        if (rv == SHR_E_RESOURCE &&
                            split_aux->delayed_payload_spilt == 0) {
                            /* Retry with payload split */
                            sal_memset(split_aux, 0, sizeof(*split_aux));
                            split_aux->delayed_payload_spilt = 1;
                        } else {
                            SHR_IF_ERR_EXIT(rv);
                            sal_memset(split_aux, 0, sizeof(*split_aux));
                        }

                    } else {
                        /*
                         * Implies the affected entries are in the same bucket
                         * as the target split bucket.
                         */
                    }
                } else {
                    /* EPARTIAL_REASON_L3BKT_DETACHED */
                    /*
                     * Must be the same bucket (a), re-split to bucket a,
                     * such that this time, the split point (l2 pivot) will be
                     * exactly same as the one of previous level (l1 pivot).
                     */
                    ALPM_ASSERT(split_aux->l3_pivot == pivot);
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_bucket_split_insert(u, m, ln, arg,
                                                           pivot, split_aux));
                }
            } else {
                /* Must be an error. */
                SHR_ERR_EXIT(rv);
            }
        }
        if (attempt == MAX_SPLIT_ATTEMPTS) {
            SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
        }
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a new entry on a given level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for new entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_insert(int u, int m, int8_t ln, alpm_arg_t *arg, bkt_split_aux_t *split_aux)
{
    alpm_pivot_t *elem;
    alpm_pivot_t *pivot;
    int8_t prev_ln = ln - 1;
    uint8_t db = arg->db;
    enum {
        INSERT_NONE,
        INSERT_ELEM_ALLOC,
        INSERT_ELEM_BKT_INSERT,
        INSERT_ELEM_PVT_INSERT,
    } done = INSERT_NONE;

    SHR_FUNC_ENTER(u);
    if (arg->pivot[prev_ln] == NULL) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, m, db, prev_ln, &arg->key.t, &pivot));
        arg->pivot[prev_ln] = pivot;
        if (prev_ln == LEVEL2) {
            SHR_IF_ERR_EXIT(
                alpm_delayed_pivot_handle(u, m, db, prev_ln, arg, true));
            pivot = arg->pivot[prev_ln];
        }
    } else {
#ifdef ALPM_DEBUG
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, m, db, prev_ln, &arg->key.t, &pivot));
        ALPM_ASSERT(pivot == arg->pivot[prev_ln]);
#endif
        pivot = arg->pivot[prev_ln];
    }

#ifdef ALPM_DEBUG
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, m, db, ln, false,
                                  &arg->key, &elem));
    done = INSERT_ELEM_ALLOC;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_insert(u, m, ln, &arg->key, pivot->bkt, elem));
    done = INSERT_ELEM_BKT_INSERT;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_insert(u, m, db, ln, arg->a1.direct_route,
                                  &arg->key, elem));
    done = INSERT_ELEM_PVT_INSERT;
    if (elem->ad) {
        elem->ad->user_data_type = arg->ad.user_data_type;
    }

    arg->pivot[ln] = elem;

    SHR_IF_ERR_VERBOSE_EXIT(
        alpm_bucket_insert(u, m, ln, arg, split_aux));

    /* The pivot might be changed in insert */
    pivot = arg->pivot[prev_ln];

#ifdef ALPM_DEBUG
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
#endif

    /* Do insert propagation to update existing default datas */
    if (ln == DB_LAST_LEVEL(u, m, db)) {
        /*
         * Set actual data implicitly.
         *  elem->ad->actual_data_type = arg->ad.actual_data_type;
         */

        if (elem->ad) {
            sal_memcpy(elem->ad, &arg->ad, sizeof(arg->ad));
        }
        SHR_IF_ERR_EXIT(
            alpm_propa(u, m, PROPOGATE_INSERT, arg, elem));
    } else {
        ALPM_ASSERT(elem->bkt->bpm == NULL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case INSERT_ELEM_PVT_INSERT:
            /* The pivot might be changed in insert */
            pivot = arg->pivot[prev_ln];
            bcmptm_rm_alpm_bkt_delete(u, m, ln, &arg->key.t, pivot->bkt, &elem);
        case INSERT_ELEM_BKT_INSERT:
            bcmptm_rm_alpm_pvt_delete(u, m, db, ln, arg->a1.direct_route,
                                      &arg->key.t, &elem);
        case INSERT_ELEM_ALLOC:
            bcmptm_rm_alpm_elem_free(u, m, db, ln, false, elem);
        case INSERT_NONE:
        default:
            /* nothing to roll back */
            break;
        }
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Update a route's assoc data for data type modification
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg with new assoc data and index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_update_dt(int u, int m, alpm_arg_t *arg)
{
    alpm_pivot_t *pivot;
    int8_t ln = DB_LAST_LEVEL(u, m, arg->db);
    int8_t prev_ln = ln - 1;
    uint8_t db = arg->db;
    bkt_split_aux_t split_aux = {0};
    bcmptm_rm_alpm_ln_stat_t *stat;
#ifdef ALPM_DEBUG
    log_bkt_t old_bkt;
#endif

    SHR_FUNC_ENTER(u);

#ifdef ALPM_DEBUG
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_find(u, m, db, prev_ln, &arg->key.t, &pivot));
    ALPM_ASSERT(pivot == arg->pivot[prev_ln]);
#endif
    pivot = arg->pivot[prev_ln];

#ifdef ALPM_DEBUG
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
    old_bkt = pivot->bkt->log_bkt;
#endif

    arg->dt_update = 1;
    SHR_IF_ERR_VERBOSE_EXIT(
        alpm_bucket_insert(u, m, ln, arg, &split_aux));
    /* ALPM_ASSERT(ent_idx != arg->pivot[ln]->index); Can be same in some rare cases. */
    /* Update pivot as it might be changed during insert due to split. */
    pivot = arg->pivot[prev_ln];
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_delete(u, m, db, ln, pivot, arg->orig_index));

    /* Update assoc-data, then do propagation. */
    *(arg->pivot[ln]->ad) = arg->ad;
    SHR_IF_ERR_EXIT(
        alpm_propa(u, m, PROPOGATE_UPDATE, arg, NULL));

#ifdef ALPM_DEBUG
    if (pivot->bkt->log_bkt != old_bkt) {
        bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, old_bkt);
    }
    bcmptm_rm_alpm_bucket_sanity(u, m, db, ln, pivot->bkt->log_bkt);
#endif
    stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
    if (stat) {
        stat->c_update_dt++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a route's assoc data
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg with new assoc data and index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_update(int u, int m, alpm_arg_t *arg)
{
    int8_t db, ln;
    bcmptm_rm_alpm_ln_stat_t *stat;
    SHR_FUNC_ENTER(u);

    /* Pivot should be set in position during search */
/*    ALPM_ASSERT(arg->pivot[PREV_LEVEL(ln)]); */

    /*
     * When update, we always update the info in the last level, therefore
     * we need to make sure all info for the last level are prepared in arg,
     * Currently, the only info that beyonds key and assoc data is sub_bkt
     * on ver0 device.
     *
     * Sub_bkt is supposed to be fetched from last level entry and then encode
     * to tcam entry during lookup. When performing update, it should be
     * decoded into arg already, thus no worry figuring it out either.
     */

    db = arg->db;
    ln = DB_LAST_LEVEL(u, m, db);
    arg->index[ln] = arg->pivot[ln]->index;
    arg->encode_data_only = 1;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_entry_write(u, m, ln, arg));

    /* Update assoc-data, then do propagation. */
    *(arg->pivot[ln]->ad) = arg->ad;
    SHR_IF_ERR_EXIT(
        alpm_propa(u, m, PROPOGATE_UPDATE, arg, NULL));

    stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
    if (stat) {
        stat->c_update++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup pivot(s) for default route for a new VRF.
 *
 * Basically, this is to add a default (0/0) route pivot(s).
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] key Arg key
 * \param [in] vrf_hdl Pointer to vrf handle
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_vrf_add(int u, int m, alpm_arg_t *arg, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t *root_arg = NULL;
    int8_t ln = -1; /* for proper roll back */
    alpm_route_t *bpm_elem;
    alpm_pivot_t *pivot;
    arg_key_t *key;
    int db;
    SHR_FUNC_ENTER(u);

    db = arg->db;
    root_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (root_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, m, root_arg);
    key = &arg->key;
    root_arg->db = db;
    root_arg->key.t.ipv = key->t.ipv;
    root_arg->key.t.w_vrf = key->t.w_vrf;
    root_arg->key.t.app = key->t.app;
    root_arg->key.t.max_bits = key->t.max_bits;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    root_arg->key.vt = key->vt;
    root_arg->default_route = 1;
    root_arg->valid = 1;
    root_arg->default_miss = 1;
    root_arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, db, root_arg->key.t.app);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, m, db, DB_LAST_LEVEL(u, m, db), false,
                                  &root_arg->key, &bpm_elem));
    bpm_elem->key.t.len = -1; /* Indicating this is a fake one */

    ln = LEVEL1;
    while (ln < DB_LAST_LEVEL(u, m, db)) {
        bcmptm_rm_alpm_bucket_alloc(u, m, NEXT_LEVEL(ln), arg, &root_arg->log_bkt[ln]);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_insert(u, m, ln, root_arg, NULL));
        pivot = root_arg->pivot[ln];
        pivot->bkt->bpm = bpm_elem;
        pivot->bkt->log_bkt = root_arg->log_bkt[ln];
        arg->pivot[ln] = pivot;
        ln++;
        /* Next Level */
        bcmptm_rm_alpm_bucket_pivot_set(u, m, db, ln, pivot->bkt->log_bkt, pivot);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_create(u, m, db, ln, key->t.ipv, key->t.w_vrf,
                                       key->t.app, key->vt));

    /*
     * Note that the prefix is not inserted in this routine,
     * only the pivots are inserted.
     */
    vrf_hdl->ipv = key->t.ipv;
    vrf_hdl->vt = key->vt;
    vrf_hdl->app = key->t.app;
    vrf_hdl->w_vrf = key->t.w_vrf;
    vrf_hdl->in_use = TRUE;
    vrf_hdl->vrf_data_type = VRF_DATA_DYNAMIC;
    vrf_hdl->virtual_default = bpm_elem;
    if (vrf_hdl->in_dq == FALSE) {
        SHR_DQ_INSERT_HEAD(&DB_VRF_HDL_LIST(u, m, db, key->t.ipv), &vrf_hdl->dqnode);
    }

exit:
    if (SHR_FUNC_ERR()) {
        while (ln >= LEVEL1) {
            if (ln < DB_LAST_LEVEL(u, m, db)) {
                if (root_arg->pivot[ln]) {
                    (void)bcmptm_rm_alpm_pivot_delete(u, m, ln, root_arg);
                }
                if (root_arg->log_bkt[ln] != INVALID_GROUP) {
                    (void)bcmptm_rm_alpm_bucket_free(u, m, db, NEXT_LEVEL(ln),
                                                     root_arg->log_bkt[ln]);
                }
            }
            ln--;
        }
        if (bpm_elem) {
            bcmptm_rm_alpm_elem_free(u, m, db, DB_LAST_LEVEL(u, m, db), false, bpm_elem);
        }
    }
    if (root_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], root_arg);
    }

    SHR_FUNC_EXIT();
}

static int
pivot_trie_sanitize_cb(rm_alpm_trie_node_t *trie, void *info)
{
    int u, m, db, ln;
    alpm_pivot_t *pivot;
    alpm_arg_t *arg = NULL;
    pivot_trie_sanitize_ctrl_t *ctrl = (pivot_trie_sanitize_ctrl_t *) info;

    SHR_FUNC_ENTER(ctrl->unit);
    u = ctrl->unit;
    db = ctrl->db;
    ln = ctrl->ln;
    m = ctrl->m;
    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, trie, pvtnode);
    arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    bcmptm_rm_alpm_pivot_arg_init(u, m, db, ln, pivot, UPDATE_KEY, arg);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_delete(u, m, ln, arg));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_free(u, m, db, NEXT_LEVEL(ln), arg->log_bkt[ln]));

exit:
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();

}


/*!
 * \brief Remove pivot(s) for default route for an existing VRF.
 *
 * Basically, this is to delete a default (0/0) route pivots.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] key Arg key
 * \param [in] vrf_hdl Pointer to vrf handle
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_vrf_delete(int u, int m, int db, arg_key_t *key, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t *arg = NULL;
    int8_t ln;
    pivot_trie_sanitize_ctrl_t ctrl;
    rm_alpm_trie_t *pivot_trie;

    SHR_FUNC_ENTER(u);

    arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, m, arg);
    arg->db = db;
    arg->key.t.ipv = key->t.ipv;
    arg->key.t.w_vrf = key->t.w_vrf;
    arg->key.t.app = key->t.app;
    arg->key.t.max_bits = key->t.max_bits;
    arg->key.vt = key->vt;
    arg->default_route = 1;

    ln = DB_LAST_LEVEL(u, m, db) - 1;
    while (ln >= LEVEL1) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_delete(u, m, ln, arg));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_free(u, m, db, NEXT_LEVEL(ln), arg->log_bkt[ln]));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln,
                                        key->t.w_vrf,
                                        key->t.ipv, &pivot_trie));
        if (pivot_trie) {
            ctrl.unit = u;
            ctrl.db = db;
            ctrl.m = m;
            ctrl.ln = ln;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_traverse(u, m,
                                              pivot_trie->trie,
                                              pivot_trie_sanitize_cb,
                                              &ctrl,
                                              TRAVERSE_BFS));
        }
        ln--;
    }
    ln = DB_LAST_LEVEL(u, m, db);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_destroy(u, m, db, ln, key->t.ipv, key->t.w_vrf));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_free(u, m, db, ln, false, vrf_hdl->virtual_default));
    vrf_hdl->in_use = FALSE;
    vrf_hdl->virtual_default = NULL;
    if (vrf_hdl->in_dq == FALSE) {
        SHR_DQ_REMOVE(&vrf_hdl->dqnode);
    }
exit:
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert new route.
 *
 * \param [in] u Device u.
 * \param [in] arg Alpm arg
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_new_insert(int u, int m, alpm_arg_t *arg)
{
    alpm_vrf_hdl_t *vrf_hdl = NULL;
    int8_t ln, db = arg->db;
    uint8_t vrf_add_flag = 0;
    bkt_split_aux_t split_aux = {0};
    bcmptm_rm_alpm_ln_stat_t *stat;
    int rv;

    SHR_FUNC_ENTER(u);

    stat = bcmptm_rm_alpm_ln_stats_get(u, m, db);
    SHR_IF_ERR_EXIT(
        insert_validate(u, m, arg));
    format_get(u, m, arg);


    /* From now on, new add operation */
    vrf_hdl = &VRF_HDL(u, m, db, arg->key.t.ipv, arg->key.t.w_vrf);
    if (vrf_hdl->in_use == FALSE) {
        arg->vrf_init = 1;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_alpm_bucket_resource_check(u, m, arg, EXTRA_NEW_VRF));
        arg->state_changed = 1;
        SHR_IF_ERR_EXIT(
            alpm_vrf_add(u, m, arg, vrf_hdl));
        vrf_add_flag = 1;
    }

    ln = DB_LAST_LEVEL(u, m, db);
    split_aux.epartial_reason = EPARTIAL_REASON_NONE;
    rv = alpm_insert(u, m, ln, arg, &split_aux);
    if (rv == SHR_E_RESOURCE) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    stat->c_insert++;
    stat->c_curr++;

exit:
    if (SHR_FUNC_ERR()) {
        if (SHR_FUNC_VAL_IS(SHR_E_RESOURCE)) {
            stat->c_last_full = stat->c_curr;
        }
        if (vrf_add_flag) {
            alpm_vrf_delete(u, m, db, &arg->key, vrf_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

static int
alpm_get(int u, int m, int ln, alpm_arg_t *arg)
{
    int cur_ln;
    alpm_info_t *info;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    /* Get last level index */
    pivot = arg->pivot[ln];
    arg->index[ln] = pivot->index;

    /* Get previous level (until Level-1) pivot & index */
    cur_ln = PREV_LEVEL(ln);
    while(cur_ln >= LEVEL1) {
        arg->pivot[cur_ln] =
            bcmptm_rm_alpm_bucket_pivot_get(u, m, arg->db,
                                            NEXT_LEVEL(cur_ln),
                                            arg->index[NEXT_LEVEL(cur_ln)]);

        arg->index[cur_ln] = arg->pivot[cur_ln]->index;
        cur_ln--;
    }

    /* Map trie format ip & mask to api format */
    ALPM_ASSERT(arg->index[ln] != INVALID_INDEX);

    info = bcmptm_rm_alpm_info_get(u, m);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (info->cmd_ctrl.req_flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_entry_read(u, m, ln, arg));
        if ((bcmptm_rm_alpm_hit_support(u, m) != HIT_MODE_DISABLE) &&
            !arg->hit && LN_INFO_FD(u, m, alpm_ver) == ALPM_VERSION_1) {
            SHR_IF_ERR_EXIT(
                alpm_propa(u, m, PROPOGATE_HIT, arg, NULL));
        }
        /* For LT encode. */
        arg->ad.user_data_type = pivot->ad->user_data_type;
    } else {
        if (!arg->decode_data_only) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_alpm_api_key_create(u, m, pivot->key.t.max_bits,
                                               &pivot->key.t.trie_ip,
                                               pivot->key.t.len,
                                               arg->key.ip_addr));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_len_to_mask(u, m, pivot->key.t.max_bits,
                                           pivot->key.t.len,
                                           &arg->key.ip_mask[0]));
        }
        arg->ad = *(pivot->ad);
    }

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_pivot_t *pre_pivot = NULL;
        alpm_bkt_elem_t *bkt_elem = NULL;
        cur_ln = PREV_LEVEL(ln);
        while (cur_ln >= LEVEL1) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_pvt_find(u, m, arg->db, cur_ln, &arg->key.t,
                                        &pre_pivot));
            ALPM_ASSERT(arg->pivot[cur_ln] == pre_pivot);
            cur_ln--;
        }
        pre_pivot = arg->pivot[PREV_LEVEL(ln)];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bkt_lookup(u, m, &arg->key.t, pre_pivot->bkt,
                                      &bkt_elem));
        ALPM_ASSERT(bkt_elem == pivot);
    }
#endif
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Callback to group bkt elems based on IP Length.
 *
 * \param [in] node Trie node
 * \param [in] user_data User data
 *
 * \return SHR_E_NONE only.
 */
static int
pivot_bpm_find_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    pivot_traverse_data_t *trvs_data = (pivot_traverse_data_t *) user_data;
    alpm_pivot_t *pivot;
    alpm_route_t *lpm;
    rm_alpm_trie_node_t *lpm_node;
    int rv;

    SHR_FUNC_ENTER(trvs_data->u);
    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    rv = bcmptm_rm_alpm_trie_find_lpm(trvs_data->route_trie,
                                      &pivot->key.t.trie_ip.key[0],
                                      pivot->key.t.len,
                                      &lpm_node,
                                      NULL,
                                      NULL);
    if (rv == SHR_E_NOT_FOUND) {
        /* For parallel mode, now also for combined mode. */
        lpm = VRF_HDL_DFT(trvs_data->u, trvs_data->m, trvs_data->db,
                          pivot->key.t.ipv, pivot->key.t.w_vrf);
        ALPM_ASSERT(lpm->key.t.len == VIRTUAL_DEFAULT_LEN);
    } else {
        SHR_IF_ERR_EXIT(rv);
        lpm = TRIE_ELEMENT_GET(alpm_pivot_t*, lpm_node, pvtnode);
    }
    ALPM_ASSERT(pivot->bkt);
    pivot->bkt->bpm = lpm;

exit:
    SHR_FUNC_EXIT();
}

static int
alpm_vrf_recover(int u, int m, alpm_arg_t *arg, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t *root_arg = NULL;
    int8_t ln;
    alpm_route_t *bpm_elem = NULL;
    arg_key_t *key;
    int db;
    SHR_FUNC_ENTER(u);

    db = arg->db;
    root_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (root_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, m, root_arg);
    key = &arg->key;
    root_arg->db = db;
    root_arg->key.t.ipv = key->t.ipv;
    root_arg->key.t.w_vrf = key->t.w_vrf;
    root_arg->key.t.app = key->t.app;
    root_arg->key.t.max_bits = key->t.max_bits;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    root_arg->key.vt = key->vt;
    root_arg->default_route = 1;
    root_arg->valid = 1;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, m, db, DB_LAST_LEVEL(u, m, db), false,
                                  &root_arg->key, &bpm_elem));
    bpm_elem->key.t.len = -1; /* Indicating this is a fake one */

    ln = DB_LAST_LEVEL(u, m, db);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_create(u, m, db, ln, key->t.ipv, key->t.w_vrf,
                                       key->t.app, key->vt));

    vrf_hdl->ipv = key->t.ipv;
    vrf_hdl->vt = key->vt;
    vrf_hdl->app = key->t.app;
    vrf_hdl->w_vrf = key->t.w_vrf;
    vrf_hdl->in_use = TRUE;
    vrf_hdl->vrf_data_type = VRF_DATA_DYNAMIC;
    vrf_hdl->virtual_default = bpm_elem;
    SHR_DQ_INSERT_HEAD(&DB_VRF_HDL_LIST(u, m, db, key->t.ipv), &vrf_hdl->dqnode);

exit:
    if (SHR_FUNC_ERR()) {
        if (bpm_elem) {
            bcmptm_rm_alpm_elem_free(u, m, db, DB_LAST_LEVEL(u, m, db), false, bpm_elem);
        }
    }
    if (root_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], root_arg);
    }
    SHR_FUNC_EXIT();
}


static int
alpm_bpm_recover_done(int u, int m, int db)
{
    uint8_t lvl, last_lvl;
    shr_dq_t *elem;
    alpm_vrf_hdl_t *vrf_hdl;
    rm_alpm_trie_t *pivot_trie;
    pivot_traverse_data_t trvs_data;
    alpm_ip_ver_t ipv;

    SHR_FUNC_ENTER(u);
    trvs_data.u = u;
    trvs_data.db = db;
    trvs_data.m = m;
    last_lvl = DB_LAST_LEVEL(u, m, db);

    for (ipv = IP_VER_4; ipv < IP_VER_COUNT; ipv++) {
        if (DB_VRF_HDL_CNT(u, m, db) == 0) {
            continue;
        }
        SHR_DQ_TRAVERSE(&DB_VRF_HDL_LIST(u, m, db, ipv), elem)
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t *, elem, dqnode);
            trvs_data.route_trie = vrf_hdl->pivot_trie[last_lvl];
            if (trvs_data.route_trie == NULL) {
                continue;
            }
            for (lvl = LEVEL1; lvl < last_lvl; lvl++) {
                pivot_trie = vrf_hdl->pivot_trie[lvl];
                if (pivot_trie) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_trie_traverse(u, m,
                                                     pivot_trie->trie,
                                                     pivot_bpm_find_cb,
                                                     &trvs_data,
                                                     TRAVERSE_BFS));
                }
            }
        SHR_DQ_TRAVERSE_END(&DB_VRF_HDL_LIST(u, m, db, ipv), elem);
    }
exit:
    SHR_FUNC_EXIT();
}

static void
ln_info_cleanup(int u, int m)
{
    int i, j;
    int db_count;
    lvl_hdl_t *lvl_hdl = NULL;

    db_count = bcmptm_rm_alpm_database_count(u, m);
    for (i = 0; i < db_count; i++) {
        alpm_db_t *database_ptr;
        database_ptr = bcmptm_rm_alpm_database_get(u, m, i);

        /* Start from Level2 */
        for (j = LEVEL2; j < database_ptr->max_levels; j++) {
            lvl_hdl = LVL_HDL(u, m, i, j);
            if (lvl_hdl) {
                SHR_FREE(lvl_hdl);
            }
        }
    }
    SHR_FREE(LN_INFO(u, m));
    bcmptm_rm_alpm_bkt_hdl_cleanup(u, m);
}

/*!
 * \brief Initiliaze ALPM level-1 info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
ln_info_init(int u, int m, bool recover)
{
    int sz;
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    alpm_info_t *alpm_info;

    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u, m);
    SHR_NULL_CHECK(alpm_info, SHR_E_INIT);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    sz = sizeof(alpm_ln_info_t);
    ALPM_ALLOC(LN_INFO(u, m), sz, "bcmptmRmalpmLninfo");
    sal_memset(LN_INFO(u, m), 0, sz);

    LN_SID(u, m, LEVEL1) = INVALID_SID;
    LN_HIT_SID(u, m, LEVEL1) = dev_info->l1_hit_sid;
    LN_SID(u, m, LEVEL2) = dev_info->l2_sid;
    LN_HIT_SID(u, m, LEVEL2) = dev_info->l2_hit_sid;
    LN_SID(u, m, LEVEL3) = dev_info->l3_sid;
    LN_HIT_SID(u, m, LEVEL3) = dev_info->l3_hit_sid;
    LN_INFO_FD(u, m, alpm_ver) = dev_info->alpm_ver;

    /* Level info initialization */
    db_count = bcmptm_rm_alpm_database_count(u, m);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, m, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_bkt_hdl_init(u, m, recover));

    for (db_idx = 0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        for (level = LEVEL2; level < db_max_levels[db_idx]; level++) {
            lvl_hdl_t *lvl_hdl = NULL;
            bkt_hdl_t *bkt_hdl;

            ALPM_ALLOC(lvl_hdl, sizeof(lvl_hdl_t), "bcmptmRmalpmLvlhdl");
            sal_memset(lvl_hdl, 0, sizeof(lvl_hdl_t));
            LVL_HDL(u, m, db_idx, level) = lvl_hdl;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_hdl_get(u, m,
                                           db_idx,
                                           level,
                                           &bkt_hdl));
            lvl_hdl->bkt_hdl = bkt_hdl;
            lvl_hdl->lvl_idx = level;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (LN_INFO(u, m)) {
            ln_info_cleanup(u, m);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initiliaze ALPM level-1 HA info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 * \param [in] boot_type Boot type.
 *
 * \return SHR_E_XXX.
 */
static int
ln_ha_init(int u, int m, bool recover, uint8_t boot_type)
{
    shr_ha_sub_id sub_id;
    uint32_t req_size = sizeof(bcmptm_rm_alpm_ln_ha_t);
    uint32_t alloc_size = sizeof(bcmptm_rm_alpm_ln_ha_t);

    SHR_FUNC_ENTER(u);

    sub_id = m ? BCMPTM_HA_SUBID_M_RM_ALPM_LN : BCMPTM_HA_SUBID_RM_ALPM_LN;
    ln_ha[u][m] = shr_ha_mem_alloc(u,
                                BCMMGMT_RM_ALPM_COMP_ID,
                                sub_id,
                                "bcmptmRmAlpmLevelnHa",
                                &alloc_size);
    SHR_NULL_CHECK(ln_ha[u][m], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (recover) {
        ALPM_ASSERT(ln_ha[u][m]);
    } else {
        sal_memset(ln_ha[u][m], 0, alloc_size);
        if (boot_type == ALPM_WARM_BOOT || boot_type == ALPM_COLD_BOOT) {
            SHR_IF_ERR_EXIT(
                bcmissu_struct_info_report(u, BCMMGMT_RM_ALPM_COMP_ID,
                                           sub_id,
                                           0, /* offset */
                                           req_size, 1,
                                           BCMPTM_RM_ALPM_LN_HA_T_ID));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
ln_ha_cleanup(int u, int m, bool graceful)
{
    if (graceful) {
    }
}

/*******************************************************************************
 * Public Functions
 */

/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_find(int u, int m, alpm_arg_t *arg)
{
    int rv;
    int8_t ln = DB_LAST_LEVEL(u, m, arg->db);

    rv = bcmptm_rm_alpm_pvt_find(u, m, arg->db, ln, &arg->key.t, &arg->pivot[ln]);
    if (SHR_SUCCESS(rv)) {
        return alpm_get(u, m, ln, arg);
    } else {
        return rv;
    }
}

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_match(int u, int m, alpm_arg_t *arg)
{
    int rv;
    int8_t ln = DB_LAST_LEVEL(u, m, arg->db);

    if (!arg->match_ready) {
        rv = bcmptm_rm_alpm_pvt_lookup(u, m, arg->db, ln, &arg->key.t, &arg->pivot[ln]);
    } else {
        rv = SHR_E_NONE;
    }
    if (SHR_SUCCESS(rv)) {
        return alpm_get(u, m, ln, arg);
    } else {
        return rv;
    }
}


/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_insert(int u, int m, alpm_arg_t *arg)
{
    int ln = DB_LAST_LEVEL(u, m, arg->db);
    int rv;
    SHR_FUNC_ENTER(u);

    /* Assume ltm will always do lookup ahead for insert/update */
    if (arg->pivot[ln] && arg->pivot[ln]->index != INVALID_INDEX) {

        if (DATA_TYPE_LOOSE_FIT(arg->ad.user_data_type,
                                arg->pivot[ln]->ad->actual_data_type)) {
            SHR_IF_ERR_EXIT(alpm_update(u, m, arg));
            arg->req_update = 1;
        } else {
            /*
             * There can be a case where user keeps updating data of a given
             * route from FULL to REDUCED repeatedly. But this shrink attempt
             * might not work, as internally we will assign loose_fit format,
             * which means the new format can be still FULL.
             * Therfore if we do insert and delete,
             * the result may be exactly same as a simple update, yet
             * the performance won't be as good as a simple update.
             * We choose to do update on loose_fit because if
             * the user requested dt was REDUCED (but actually it is saved as
             * FULL, due to loose_fit in insert logic), and user updates
             * the entry with still REDUCED format, the strict_fit logic
             * might throw E_RESOURCE during insert+delete process, which is
             * usually not something one would expect.
             */
            SHR_IF_ERR_EXIT(alpm_update_dt(u, m, arg));
        }
    } else {
        rv = alpm_new_insert(u, m, arg);
        if (rv == SHR_E_RESOURCE) {
            SHR_ERR_EXIT(rv);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
        bcmptm_rm_alpm_vrf_route_inc(u, m, arg->db, arg->key.t.w_vrf,
                                     arg->key.t.ipv, &ln);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_delete(int u, int m, alpm_arg_t *arg)
{
    int8_t ln;
    int cnt;
    bcmptm_rm_alpm_ln_stat_t *stats;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(delete_validate(u, m, arg));

    ln = DB_LAST_LEVEL(u, m, arg->db);
    /*
       SHR_IF_ERR_EXIT(
        LEVEL_ELEM_DELETE(alpm_pivot_t *, u, ln, arg, &route));*/
    SHR_IF_ERR_EXIT(
        alpm_delete(u, m, ln, arg));
    bcmptm_rm_alpm_vrf_route_dec(u, m, arg->db, arg->key.t.w_vrf,
                                 arg->key.t.ipv, &cnt);

    if (cnt == 0) {
        alpm_vrf_hdl_t *vrf_hdl;
        vrf_hdl = &VRF_HDL(u, m, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
        ALPM_ASSERT(vrf_hdl->in_use == TRUE);
        SHR_IF_ERR_EXIT(
            alpm_vrf_delete(u, m, arg->db, &arg->key, vrf_hdl));
    }
    stats = bcmptm_rm_alpm_ln_stats_get(u, m, arg->db);

    if (stats->c_curr > stats->c_peak) {
        stats->c_peak = stats->c_curr; /* All time peak */
    }
    stats->c_delete++;
    stats->c_curr--;
    /*    alpm_sanity(u, m, arg);*/
exit:
    SHR_FUNC_EXIT();
}


void
bcmptm_rm_alpm_ln_cleanup(int u, int m, bool graceful)
{
    if (ln_ha[u][m]) {
        ln_ha_cleanup(u, m, graceful);
    }
    if (!LN_INFO(u, m)) {
        return;
    }
    ln_info_cleanup(u, m);
    bcmptm_rm_alpm_bucket_cleanup(u, m);
}

int
bcmptm_rm_alpm_ln_init(int u, int m, bool recover, uint8_t boot_type)
{
    uint8_t num_banks;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_uft_banks(u, m, DBS, LEVEL2, &num_banks, NULL, NULL, NULL));
    if (num_banks == 0) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_init(u, m));
    SHR_IF_ERR_EXIT(
        ln_info_init(u, m, recover));

    SHR_IF_ERR_EXIT(ln_ha_init(u, m, recover, boot_type));

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_ln_info_get(int u, int m, alpm_ln_info_t *linfo)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(LN_INFO(u, m), SHR_E_NOT_FOUND);
    sal_memcpy(linfo, LN_INFO(u, m), sizeof(alpm_ln_info_t));
exit:
    SHR_FUNC_EXIT();
}

bcmdrd_sid_t
bcmptm_rm_alpm_ln_sid_get(int u, int m, int8_t ln)
{
    if (LN_INFO(u, m)) {
        return LN_SID(u, m, ln);
    } else {
        return INVALIDm;
    }
}

int
bcmptm_rm_alpm_lvl_hdl_get(int u, int m, int db, int8_t ln, lvl_hdl_t **lvl_hdl)
{
    ALPM_ASSERT(lvl_hdl);
    ALPM_ASSERT(db < DBS);
    ALPM_ASSERT(ln < LEVELS);
    *lvl_hdl = LVL_HDL(u, m, db, ln);
    return (*lvl_hdl) ? SHR_E_NONE : SHR_E_INTERNAL;
}

int
bcmptm_rm_alpm_split_pivot_add(int u, int m, int db, int8_t ln, alpm_arg_t *split_arg,
                               alpm_pivot_t **split_pivot,
                               bkt_split_aux_t *split_aux)
{
    int rv;
    alpm_bkt_elem_t *bpm_elem = NULL;

    SHR_FUNC_ENTER(u);

    rv = bcmptm_rm_alpm_pvt_find(u, m, db,
                                 DB_LAST_LEVEL(u, m, db),
                                 &split_arg->key.t,
                                 &bpm_elem);
    if (rv == SHR_E_NOT_FOUND) {
        /* For parallel mode, now also for combined mode. */
        bpm_elem = VRF_HDL_DFT(u, m, db, split_arg->key.t.ipv,
                               split_arg->key.t.w_vrf);
        split_arg->default_miss = 1;
        ALPM_ASSERT(bpm_elem->key.t.len == VIRTUAL_DEFAULT_LEN);
    }

    split_arg->ad = *(bpm_elem->ad);
    split_arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, db, split_arg->key.t.app);
    /* Will alloc pivot & assign to arg */
    rv = bcmptm_rm_alpm_pivot_insert(u, m, ln, split_arg, split_aux);
    SHR_IF_ERR_VERBOSE_EXIT2(rv, SHR_E_PARTIAL);
    (*split_pivot) = split_arg->pivot[ln];
    (*split_pivot)->bkt->bpm = bpm_elem;
    (*split_pivot)->bkt->log_bkt = split_arg->log_bkt[ln];

    /* Index should be set already */
    ALPM_ASSERT(split_arg->index[ln] >= 0);
    ALPM_ASSERT(split_arg->index[ln] == (*split_pivot)->index);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_read_ln_raw(int u, int m, int db, int8_t ln, int hw_idx, uint32_t *de,
                           uint32_t *dt_bitmap)
{
    alpm_sid_index_map_t map_info;
    int rv;

    map_info.wide = ln;
    map_info.index = hw_idx;

    rv = bcmptm_rm_alpm_uft_banks(u, m, db, ln, NULL,
                                  NULL,
                                  NULL,
                                  NULL);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmptm_rm_alpm_ln_sid_index_map(u, m, &map_info);
    if (rv == SHR_E_UNAVAIL) {
        rv = bcmptm_rm_alpm_read(u, m, LN_SID(u, m, ln), hw_idx,
                                 ALPM_LN_ENT_MAX, de, dt_bitmap);
    } else {
        ALPM_ASSERT(rv == SHR_E_NONE);
        rv = bcmptm_rm_alpm_read(u, m, map_info.sid, map_info.sid_index,
                                 ALPM_LN_ENT_MAX, de, dt_bitmap);
    }
    bcmptm_rm_alpm_internals[u][m][db][ln].read++;
#ifdef ALPM_DEBUG
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        cli_out(" Level %d read %d hw_idx %d \n", ln + 1,
                bcmptm_rm_alpm_internals[u][m][db][ln].read, hw_idx);
        bcmptm_rm_alpm_pt_dump(u, m, LN_SID(u, m, ln), de);
    }
#endif
    return rv;
}

int
bcmptm_rm_alpm_read_ln_hit(int u, int m, int8_t ln, int hw_idx, uint32_t *e)
{
    return bcmptm_rm_alpm_read(u, m, LN_HIT_SID(u, m, ln), hw_idx, 1, e, NULL);
}

int
bcmptm_rm_alpm_write_ln_hit(int u, int m, int8_t ln, int hw_idx, uint32_t *e)
{
    return bcmptm_rm_alpm_write(u, m, LN_HIT_SID(u, m, ln), hw_idx,
                                e, 1, BCMPTM_RM_OP_NORMAL,
                                LN_HIT_DT_BITMAP);
}

int
bcmptm_rm_alpm_write_ln_raw(int u, int m, int db, int8_t ln, int hw_idx, uint32_t *de,
                            uint8_t hit_op, uint32_t modify_bitmap,
                            uint32_t dt_bitmap)
{
    int rv, i;
    static uint32_t hit_entry_clear[1] = {0};
    static uint32_t hit_entry_set[1] = {0xFFFF};
    static uint32_t hit_entry_set2[1] = {0x1};
    uint32_t *hit_entry;
    alpm_sid_index_map_t map_info;
#ifdef ALPM_DEBUG
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        cli_out(" Level %d write %d hw_idx %d \n", ln + 1,
                bcmptm_rm_alpm_internals[u][m][db][ln].write, hw_idx);
        bcmptm_rm_alpm_pt_dump(u, m, LN_SID(u, m, ln), de);
    }
#endif

    map_info.wide = ln;
    map_info.index = hw_idx;
    rv = bcmptm_rm_alpm_uft_banks(u, m, db, ln, NULL,
                                  NULL,
                                  NULL,
                                  NULL);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmptm_rm_alpm_ln_sid_index_map(u, m, &map_info);
    if (rv == SHR_E_UNAVAIL) {
        rv = bcmptm_rm_alpm_write(u, m, LN_SID(u, m, ln), hw_idx, de,
                                  ALPM_LN_ENT_MAX, BCMPTM_RM_OP_NORMAL,
                                  dt_bitmap);
    } else {
        ALPM_ASSERT(rv == SHR_E_NONE);
        rv = bcmptm_rm_alpm_write(u, m, map_info.sid, map_info.sid_index, de,
                                  ALPM_LN_ENT_MAX, BCMPTM_RM_OP_NORMAL,
                                  dt_bitmap);
    }
    bcmptm_rm_alpm_internals[u][m][db][ln].write++;

    if (SHR_SUCCESS(rv) && hit_op == HIT_OP_AUTO) {
        uint8_t hit_version, hit_support;
        hit_support = bcmptm_rm_alpm_hit_support(u, m);
        hit_version = bcmptm_rm_alpm_hit_version(u, m);
        if (hit_support != HIT_MODE_DISABLE) {
            if (hit_version == ALPM_HIT_VERSION_0) {
                hit_entry = hit_support == HIT_MODE_FORCE_SET ?
                            hit_entry_set : hit_entry_clear;
                rv = bcmptm_rm_alpm_write(u, m, LN_HIT_SID(u, m, ln), hw_idx,
                                          hit_entry,
                                          1, BCMPTM_RM_OP_NORMAL,
                                          LN_HIT_DT_BITMAP);


            } else if (hit_version == ALPM_HIT_VERSION_1) {
                hit_entry = hit_support == HIT_MODE_FORCE_SET ?
                            hit_entry_set2 : hit_entry_clear;
                ALPM_ASSERT(modify_bitmap < (1 << MAX_LN_HIT_BITS));
                for (i = 0; i < MAX_LN_HIT_BITS && modify_bitmap; i++) {
                    if (ALPM_BIT_GET(modify_bitmap, i)) {
                        rv = bcmptm_rm_alpm_write(u, m, LN_HIT_SID(u, m, ln),
                                                  LN_HIT_INDEX(hw_idx, i),
                                                  hit_entry,
                                                  1, BCMPTM_RM_OP_NORMAL,
                                                  LN_HIT_DT_BITMAP);
                        ALPM_BIT_CLR(modify_bitmap, i);
                    }
                }
            } else {
                return SHR_E_INTERNAL;
            }
            bcmptm_rm_alpm_internals[u][m][db][ln].write++;
        }
    }
    return rv;
}


int
bcmptm_rm_alpm_bucket_clean(int u, int m, int db, int8_t ln, alpm_pivot_t **pivot)
{
    /*
     * If bucket is empty, delete in_pivot with tail recursion.
     * Handle default route (len = 0) case in vrf delete.
     */
    int8_t prev_ln = PREV_LEVEL(ln);
    int delete_pivot = 0;
    alpm_pivot_t *next_level_pivot;
    rm_alpm_trie_t *bucket_trie;
    alpm_pivot_t *in_pivot = *pivot;
    alpm_arg_t *arg = NULL;
    SHR_FUNC_ENTER(u);

    bucket_trie = in_pivot->bkt->trie;
    ALPM_ASSERT(bucket_trie->trie == NULL || bucket_trie->trie->count == 1);
    if (ln == DB_LAST_LEVEL(u, m, db)) {
        if (in_pivot->key.t.len != 0 && bucket_trie->trie == NULL) {
#if 0
            if (ln == LEVEL3) { /* Three levels mode */
                alpm_pivot_t *pre_pivot;
                pre_pivot = bcmptm_rm_alpm_bucket_pivot_get(u, m, db, prev_ln,
                                                            in_pivot->index);
                if (in_pivot->key.t.len != pre_pivot->key.t.len) {
                    delete_pivot = 1;
                } else if (pre_pivot->bkt->trie->trie->count == 1) {
                    delete_pivot = 1;
                }
            } else { /* Two levels mode, no pre-in_pivot */
                delete_pivot = 1;
            }
#endif
            delete_pivot = 1;
        }
    } else {
        /*
         * Bucket entries all deleted or just one left, that one must be the
         * same as in_pivot
         */
        if (in_pivot->key.t.len != 0 && bucket_trie->trie &&
            bucket_trie->trie->count == 1) {
            rm_alpm_trie_node_t *node = bucket_trie->trie;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_iter_get_first(bucket_trie, &node));
            ALPM_ASSERT(node->type == PAYLOAD);
            next_level_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, bktnode);
            if (next_level_pivot->bkt == NULL) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (next_level_pivot->key.t.len != 0 &&
                !next_level_pivot->bkt->trie->trie) {
                /*
                 * Only when the first level in_pivot deleted,
                 * the next level is allowed to be freed.
                 */
                delete_pivot = 1;
                /* Reset to use common logic to free previous bucket. */
                in_pivot = next_level_pivot;
                prev_ln = ln;
                ln = NEXT_LEVEL(ln);
            }
        } else if (in_pivot->key.t.len != 0 && bucket_trie->trie == NULL) {
            delete_pivot = 1;
        }
    }

    if (delete_pivot) {
        arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
        if (arg == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        bcmptm_rm_alpm_pivot_arg_init(u, m, db, prev_ln, in_pivot, UPDATE_KEY, arg);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_delete(u, m, prev_ln, arg));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_free(u, m, db, ln, arg->log_bkt[prev_ln]));
        /*
         * After pivot freed, the bucket_trie becomes a dangling pointer.
         * ALPM_ASSERT(bucket_trie->trie == 0);
         */
        *pivot = NULL;
    } else if (bucket_trie->trie == NULL) {
        /*
         * Bucket empty, but pivot stay, reset the bucket to 0.
         */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_free(u, m, db, ln, in_pivot->bkt->log_bkt));
        in_pivot->bkt->log_bkt = INVALID_LOG_BKT;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, m, db, PREV_LEVEL(ln),
                                        in_pivot,
                                        NULL, UPDATE_ALL));
    }

exit:
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_pivot_update(int u, int m, int db, int8_t ln,
                            alpm_pivot_t *pivot,
                            alpm_arg_t *arg,
                            uint8_t update_type)
{
    uint8_t alloc = 0;
    SHR_FUNC_ENTER(u);

    if (ln == LEVEL1) {
        /*
         * Always encode key for level-1, as we don't always read level-1
         * before writing.
         */
        update_type = UPDATE_ALL;
    }

    if (arg) {
        ALPM_ASSERT(db == arg->db);
    } else {
        arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
        if (arg == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        alloc = 1;
        bcmptm_rm_alpm_pivot_arg_init(u, m, db, ln, pivot, update_type, arg);
    }

    arg->index[ln] = pivot->index;
    arg->encode_data_only = (update_type == UPDATE_DATA);

    if (ln == LEVEL1) {
        /* Level-1 update */
        if (!arg->encode_data_only) {
            bcmptm_rm_alpm_adata_encode(u, m, ln, arg, &arg->a1);
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_insert(u, m, arg));

    } else {
        ALPM_ASSERT(ln == LEVEL2);
        if (!arg->encode_data_only) {
            bcmptm_rm_alpm_adata_encode(u, m, ln, arg, &arg->a2);
        }

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_entry_write(u, m, ln, arg));

#ifdef ALPM_DEBUG
        if (LOG_CHECK_INFO(BSL_LOG_MODULE) && (update_type & UPDATE_KEY)) {
            alpm_pivot_t *pre_pivot;
            alpm_bkt_elem_t *bkt_elem = NULL;
            alpm_pivot_t *pre_pivot2 = NULL;
            pre_pivot = arg->pivot[PREV_LEVEL(ln)];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_pvt_find(u, m, db, PREV_LEVEL(ln),
                                        &arg->key.t, &pre_pivot2));
            /* ALPM_ASSERT(pre_pivot == pre_pivot2);  This can be false */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_lookup(u, m, &arg->key.t, pre_pivot->bkt,
                                          &bkt_elem));
            ALPM_ASSERT(bkt_elem == pivot);
        }
#endif
    }
exit:
    if (arg && alloc) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_pivot_insert(int u, int m, int8_t ln, alpm_arg_t *arg,
                            bkt_split_aux_t *split_aux)
{
    SHR_FUNC_ENTER(u);

    if (ln == LEVEL1) {
        /* Level-1 insert */
        bcmptm_rm_alpm_adata_encode(u, m, ln, arg, &arg->a1);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_insert(u, m, arg));
    } else {
        int rv;
        ALPM_ASSERT(ln == LEVEL2);
        bcmptm_rm_alpm_adata_encode(u, m, ln, arg, &arg->a2);
        rv = alpm_insert(u, m, ln, arg, split_aux);
        SHR_IF_ERR_VERBOSE_EXIT2(rv, SHR_E_PARTIAL);
    }
    ALPM_ASSERT(arg->index[ln] >= 0);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pivot_delete(int u, int m, int8_t ln, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (ln == LEVEL1) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_delete(u, m, arg));
    } else {
        ALPM_ASSERT(ln == LEVEL2);
        SHR_IF_ERR_EXIT(alpm_delete(u, m, ln, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_split_arg_init(int u, int m, int8_t db, int8_t ln,
                              ln_index_t index,
                              elem_key_t *key,
                              trie_ip_t *pivot_key,
                              uint32_t pivot_len,
                              log_bkt_t log_bkt,
                              alpm_arg_t *arg)
{
    bcmptm_rm_alpm_arg_init(u, m, arg);
    arg->db = db;
    arg->log_bkt[ln] = log_bkt;
    arg->valid = 1;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    arg->key.vt = key->vt;
    arg->key.t.app = key->t.app;
    arg->key.t.max_bits = key->t.max_bits;
    arg->key.t.ipv = key->t.ipv;
    arg->key.t.w_vrf = key->t.w_vrf;
    arg->key.t.len = pivot_len;
    arg->key.t.trie_ip = *pivot_key;

   /*
     * This is to get the correct first level pivot,
     * it also makes sure no pivot find for the first level
     */

    if (ln == LEVEL2 && index != INVALID_INDEX) {
        arg->pivot[LEVEL1] =
            bcmptm_rm_alpm_bucket_pivot_get(u, m, db, ln, index);
#ifdef ALPM_DEBUG
        {
        alpm_pivot_t *pivot;
        bcmptm_rm_alpm_pvt_find(u, m, db, LEVEL1, &arg->key.t, &pivot);
        ALPM_ASSERT(pivot == arg->pivot[LEVEL1]);
        }
#endif
    }

    arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, db, arg->key.t.app);
    bcmptm_rm_alpm_api_key_create(u, m, arg->key.t.max_bits,
                                  pivot_key,
                                  pivot_len,
                                  arg->key.ip_addr);
    bcmptm_rm_alpm_len_to_mask(u, m, arg->key.t.max_bits,
                               pivot_len,
                               &arg->key.ip_mask[0]);


}

void
bcmptm_rm_alpm_pivot_arg_init(int u, int m, int8_t db, int8_t ln,
                              alpm_pivot_t *pivot,
                              uint8_t update_type,
                              alpm_arg_t *arg)
{
    bcmptm_rm_alpm_arg_init(u, m, arg);
    arg->db = db;
    arg->pivot[ln] = pivot;
    if (ln == LEVEL2) {
        arg->pivot[LEVEL1] =
            bcmptm_rm_alpm_bucket_pivot_get(u, m, db, ln, pivot->index);
    }

    if (pivot->bkt->bpm->key.t.len == VIRTUAL_DEFAULT_LEN) {
        arg->default_miss = 1;
    }
    arg->log_bkt[ln] = pivot->bkt->log_bkt;
    arg->valid = 1;
    if (update_type & UPDATE_KEY) {
        /* Key type will be set in bcmptm_rm_alpm_l1_insert */
        arg->key.vt = pivot->key.vt;
        arg->key.t = pivot->key.t;
        bcmptm_rm_alpm_api_key_create(u, m, arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      arg->key.ip_addr);
        bcmptm_rm_alpm_len_to_mask(u, m, arg->key.t.max_bits,
                                   arg->key.t.len,
                                   &arg->key.ip_mask[0]);
    }

    if (update_type & UPDATE_DATA) {
        arg->ad = *pivot->bkt->bpm->ad;
    }
    arg->data_type = bcmptm_rm_alpm_mid_dt_get(u, m, db, pivot->key.t.app);

}

bcmptm_rm_alpm_ln_stat_t *
bcmptm_rm_alpm_ln_stats_get(int u, int m, int8_t db)
{
    if (ln_ha[u][m]) {
        switch (db) {
            case DB0:
                return &(ln_ha[u][m]->db0_stats);
            case DB1:
                return &(ln_ha[u][m]->db1_stats);
            case DB2:
                return &(ln_ha[u][m]->db2_stats);
            case DB3:
                return &(ln_ha[u][m]->db3_stats);
            default:
                return NULL;
        }
    }
    return NULL;

}


int
bcmptm_rm_alpm_ln_recover(int u, int m, int8_t ln, alpm_arg_t *arg)
{
    alpm_vrf_hdl_t *vrf_hdl;

    SHR_FUNC_ENTER(u);
    format_get(u, m, arg);
    vrf_hdl = &VRF_HDL(u, m, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
    if (vrf_hdl->in_use == FALSE) {
        SHR_IF_ERR_EXIT(
            alpm_vrf_recover(u, m, arg, vrf_hdl));
    }
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_bucket_recover(u, m, ln, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ln_pivot_recover(int u, int m, int8_t ln, alpm_arg_t *arg)
{
    alpm_elem_t *elem;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, m, arg->db, ln, false,
                                  &arg->key, &elem));
    pivot = arg->pivot[PREV_LEVEL(ln)];
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_insert(u, m, ln, &arg->key, pivot->bkt, elem));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_insert(u, m, arg->db, ln, arg->a1.direct_route,
                                  &arg->key, elem));
    arg->pivot[ln] = elem;
    elem->index = arg->index[ln];

    if (ln == DB_LAST_LEVEL(u, m, arg->db)) {
        int cnt;
        sal_memcpy(elem->ad, &arg->ad, sizeof(arg->ad));
        bcmptm_rm_alpm_vrf_route_inc(u, m, arg->db, arg->key.t.w_vrf,
                                     arg->key.t.ipv, &cnt);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ln_recover_done(int u, int m, bool full)
{
    int db, ln;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE: SHR_E_UNAVAIL);
    for (db = DB0; db < bcmptm_rm_alpm_database_count(u, m); db++) {
        for (ln = LEVEL2; ln <= DB_LAST_LEVEL(u, m, db); ln++) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_recover_done(u, m, db, ln));
        }
        SHR_IF_ERR_EXIT(alpm_bpm_recover_done(u, m, db));
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_ln_usage_get(int u, int m, int ln,
                            bcmptm_cth_alpm_usage_info_t *info)
{
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *elem;
    rbkt_t *curr;
    int v_rbkts = 0, f_rbkts = 0, m_rbkts = 0;
    int valid_entries = 0;
    int all_entries = 0;
    int i, num_entry;
    rbkt_t *rbkt2;
    info->ln_max_rbkts = 0;
    info->ln_used_rbkts = 0;
    info->ln_rbkts_utilization = 0;
    info->ln_max_entries_rbkts = 0;
    info->ln_used_entries_rbkts = 0;
    info->ln_entries_utilization_rbkts = 0;

    if (!LN_INFO(u, m)) {
        return;
    }

    bcmptm_rm_alpm_bkt_hdl_get(u, m, info->db, ln, &bkt_hdl);

    if (bkt_hdl) {
        m_rbkts = bkt_hdl->rbkts_total;
        SHR_DQ_TRAVERSE(&bkt_hdl->global_group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
            v_rbkts += curr->valid_banks;
            f_rbkts += curr->free_banks;
            rbkt2 = curr;
            for (i = 0; i < curr->valid_banks; i++) {
                valid_entries += shr_util_popcount(rbkt2->entry_bitmap);
                (void)bcmptm_rm_alpm_ln_base_entry_info(u, m, bkt_hdl->format_type,
                                                        rbkt2->format, NULL,
                                                        NULL, &num_entry);
                all_entries += num_entry;
                rbkt2++;
            }
        SHR_DQ_TRAVERSE_END(&bkt_hdl->global_group_list, elem);

        info->ln_max_rbkts = m_rbkts;
        info->ln_used_rbkts = v_rbkts;
        if (m_rbkts) {
            info->ln_rbkts_utilization = v_rbkts * 100 / m_rbkts;
        }
        info->ln_max_entries_rbkts = all_entries;
        info->ln_used_entries_rbkts = valid_entries;
        if (all_entries) {
            info->ln_entries_utilization_rbkts = valid_entries * 100 / all_entries;
        }
    }
}
