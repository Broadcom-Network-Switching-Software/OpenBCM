/*! \file rm_alpm_traverse.c
 *
 * ALPM Traverse support
 *
 * This file contains the resource manager for ALPM Level-1
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

#include "rm_alpm_common.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_trie.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_traverse.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE          BSL_LS_BCMPTM_RMALPM

#define SNAP_INFO(u, m, l)                (snap_info[u][m][l])
#define SNAP_NUM_ENTRIES(u, m, l)         (SNAP_INFO(u, m, l)->num_entries)
#define SNAP_TRIES(u, m, l)               (SNAP_INFO(u, m, l)->trie)
#define SNAP_LMMS(u, m, l)                (SNAP_INFO(u, m, l)->lmm)
#define SNAP_VRF_S(u, m, l)               (SNAP_INFO(u, m, l)->vrf_s)
#define SNAP_VRF_E(u, m, l)               (SNAP_INFO(u, m, l)->vrf_e)


#define SNAP_TRIE(u, m, l, w_vrf)         (SNAP_INFO(u, m, l)->trie[w_vrf])
#define SNAP_LMM(u, m, l, w_vrf)          (SNAP_INFO(u, m, l)->lmm[w_vrf])
#define SNAP_PEAK_IDX(u, m, l)            (SNAP_INFO(u, m, l)->peak_index)

#define SNAP_ENTRIES(u, m, l)             (SNAP_INFO(u, m, l)->entries)
#define SNAP_ENTRY(u, m, l, i)            (SNAP_INFO(u, m, l)->entries[i])
#define SNAP_ENTRY_INDEX(u, m, l, i)      (SNAP_ENTRY(u, m, l, i).index)
#define SNAP_ENTRY_NODE(u, m, l, i)       (SNAP_ENTRY(u, m, l, i).node)
#define SNAP_ENTRY_KEY(u, m, l, i)        (SNAP_ENTRY(u, m, l, i).key)

#define MULTI_LEVELS_FALL_BACK(vt)     (vt == VRF_OVERRIDE)

/*******************************************************************************
  Typedefs
 */

typedef struct snap_key_s {
    int                 w_vrf;
    alpm_ip_ver_t       v6;
    int                 len;
    trie_ip_t          trie_key;
    alpm_vrf_type_t     vt;
} snap_key_t;


typedef struct snap_entry_s {
    rm_alpm_trie_node_t node;
    snap_key_t          key;
    int                 index;
} snap_entry_t;


typedef struct snap_info_s {
    int             peak_index;
    snap_entry_t    *entries;
    int             num_entries;
    rm_alpm_trie_t  **trie;
    int             vrf_s;
    int             vrf_e;
    shr_lmm_hdl_t   *lmm;
} snap_info_t;

typedef struct snap_info2_s {
    uint8_t         cache_valid;
    elem_key_t      last_key;
    shr_dq_t        queue;
    uint32_t        state;
    alpm_vrf_hdl_t  *vrf_hdl;
} snap_info2_t;

typedef struct clone_data_s {
    int u;
    int m;
    int index;
    int lt_idx;
} clone_data_t;


typedef struct traverse_cmp_data_s {
    int u;
    int m;
    alpm_arg_t *arg;
} traverse_cmp_data_t;


/*******************************************************************************
 * Private variables
 */

static snap_info_t *snap_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS][MAX_ALPM_LTS];
static snap_info2_t *snap_info2[BCMDRD_CONFIG_MAX_UNITS][ALPMS][MAX_ALPM_LTS];
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Private Functions
 */
static int
route_lt_idx_encode2(int u, int m, uint8_t app, alpm_vrf_type_t vt,
                     alpm_ip_ver_t ipv, uint8_t is_src,
                     int *oidx)
{
    int idx = 0;
    int count;
    bcmptm_rm_alpm_more_info_t *info;

    SHR_FUNC_ENTER(u);
    count = bcmptm_rm_alpm_lts_count(u, m);
    for (idx = 0; idx < count; idx++) {
        info = bcmptm_rm_alpm_lt_info_get(u, m, idx);
        if (info->app == app &&
            info->ipv6 == ipv &&
            info->is_src == is_src &&
            info->vt == vt) {
            break;
        }
    }
    if (idx == count) {
        *oidx = 0;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    *oidx = idx;

exit:
    SHR_FUNC_EXIT();
}


static int
route_lt_idx_encode(int u, int m, bcmltd_sid_t ltid,
                    uint8_t app, alpm_vrf_type_t vt,
                    alpm_ip_ver_t ipv, uint8_t is_src,
                    int *oidx)
{
    int idx = 0;
    int count;

    SHR_FUNC_ENTER(u);
    if (ltid == BCMLTD_SID_INVALID) {
        SHR_IF_ERR_EXIT(
            route_lt_idx_encode2(u, m, app, vt, ipv, is_src, oidx));
        SHR_EXIT();
    }

    count = bcmptm_rm_alpm_lts_count(u, m);
    for (idx = 0; idx < count; idx++) {
        if (ltid == bcmptm_rm_alpm_lt_get(u, m, idx)) {
            break;
        }
    }
    if (idx == count) {
        *oidx = 0;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    *oidx = idx;

exit:
    SHR_FUNC_EXIT();
}

static int
route_lt_idx_decode(int u, int m, int lt_idx,
                    alpm_vrf_type_t *vt, alpm_ip_ver_t *ipv,
                    uint8_t *app)
{
    bcmptm_rm_alpm_more_info_t *lt_info;

    SHR_FUNC_ENTER(u);
    lt_info = bcmptm_rm_alpm_lt_info_get(u, m, lt_idx);

    if (lt_info) {
        if (vt) {
            *vt = lt_info->vt;
        }
        if (ipv) {
            *ipv = lt_info->ipv6;
        }
        if (app) {
            *app = lt_info->app;
        }
    } else {
        if (vt) {
            *vt = 0;
        }
        if (ipv) {
            *ipv = 0;
        }
        if (app) {
            *app = 0;
        }
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fast compare a given snap key to a peak index
 *
 * \param [in] snk Snapshot key
 * \param [in] lt_idx Route LT index
 * \param [in] index Peak index
 *
 * \return SHR_E_XXX
 */
static int
snap_key_cmp(int u, int m, int lt_idx, snap_key_t *snk, int index)
{
    snap_key_t *snk2;
    int diff = 1;

    snk2 = &SNAP_ENTRY_KEY(u, m, lt_idx, index);

    if (snk2->vt == snk->vt &&
        snk2->w_vrf == snk->w_vrf &&
        snk2->len == snk->len &&
        snk2->v6 == snk->v6 &&
        !sal_memcmp(&snk2->trie_key, &snk->trie_key, sizeof(snk->trie_key))) {
        diff = 0;
    }

    return diff;
}


/*!
 * \brief Search a given key into the snapshot
 *
 * \param [in] snk Snapshot key
 * \param [in] lt_idx Route LT index
 * \param [in] snk Snap key
 * \param [out] index Found index as a subscript of the snapshot array
 *
 * \return SHR_E_XXX
 */
static int
snap_key_search(int u, int m, int lt_idx, snap_key_t *snk, int *index)
{
    rm_alpm_trie_node_t *sn_node = NULL;
    rm_alpm_trie_t *trie;
    snap_entry_t *sne;
    alpm_vrf_type_t vt;
    uint8_t app;

    SHR_FUNC_ENTER(u);

    if (!snap_key_cmp(u, m, lt_idx, snk, SNAP_PEAK_IDX(u, m, lt_idx))) {
        *index = SNAP_PEAK_IDX(u, m, lt_idx);
    } else {
        SHR_IF_ERR_EXIT(
            route_lt_idx_decode(u, m, lt_idx, &vt, NULL, &app));
        if (app == APP_LPM) {
            if (vt == VRF_PRIVATE) {
                trie = SNAP_TRIE(u, m, lt_idx, snk->w_vrf);
            } else {
                trie = SNAP_TRIE(u, m, lt_idx, 0);
            }
        } else if (app == APP_L3MC) {
            trie = SNAP_TRIE(u, m, lt_idx, 0);
        } else if (app == APP_COMP0) {
            trie = SNAP_TRIE(u, m, lt_idx, snk->w_vrf);
        } else if (app == APP_COMP1) {
            trie = SNAP_TRIE(u, m, lt_idx, 0);
        } else {
            trie = NULL;
        }
        if (trie) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_search(trie,
                                           snk->trie_key.key,
                                           snk->len,
                                           &sn_node));
            sne = TRIE_ELEMENT_GET(snap_entry_t*, sn_node, node);
            *index = sne->index;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Snapshot entry clone callback
 *
 * \param [in] trie Source node
 * \param [out] clone_trie Cloned node
 * \param [in|out] datum User data
 *
 * \return SHR_E_XXX
 */
static int
entry_clone(rm_alpm_trie_node_t *trie,
            rm_alpm_trie_node_t **clone_trie,
            void *datum)
{
    alpm_pivot_t *payload = NULL;
    snap_entry_t *entry = NULL;
    clone_data_t *info = NULL;

    payload = (alpm_pivot_t *) trie;
    info = (clone_data_t *) datum;
    entry = &SNAP_ENTRY(info->u, info->m, info->lt_idx, info->index);

    if (entry && payload) {
        entry->key.vt = payload->key.vt;
        entry->key.w_vrf = payload->key.t.w_vrf;
        entry->key.v6 = payload->key.t.ipv;
        entry->key.len = payload->key.t.len;
        sal_memcpy(entry->key.trie_key.key, payload->key.t.trie_ip.key,
                   sizeof(payload->key.t.trie_ip));
        entry->index = info->index++;

        if (clone_trie) {
            *clone_trie = &entry->node;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Destroy the snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 *
 * \return nothing
 */
static void
snapshot_destroy(int u, int m, int lt_idx)
{
    int w_vrf = 0;
    int w_vrf_s = 0;

    if (!SNAP_INFO(u, m, lt_idx)) {
        return;
    }

    w_vrf_s = SNAP_VRF_S(u, m, lt_idx);
    for (w_vrf = w_vrf_s; w_vrf <= SNAP_VRF_E(u, m, lt_idx); w_vrf++) {
        bcmptm_rm_alpm_trie_clone_destroy(u, m, SNAP_TRIE(u, m, lt_idx, w_vrf - w_vrf_s),
                                          SNAP_LMM(u, m, lt_idx, w_vrf - w_vrf_s));
    }

    SHR_FREE(SNAP_ENTRIES(u, m, lt_idx));
    SHR_FREE(SNAP_TRIES(u, m, lt_idx));
    SHR_FREE(SNAP_LMMS(u, m, lt_idx));
    SHR_FREE(SNAP_INFO(u, m, lt_idx));

}

/*!
 * \brief Create ALPM snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_create(int u, int m, int db, int lt_idx, int max_levels)
{
    int w_vrf = 0;
    int w_vrf_s = 0, w_vrf_e = 0;
    alpm_ip_ver_t ipv = 0;
    int num_entries = 0;
    rm_alpm_trie_t *trie_src = NULL;
    alpm_dev_info_t *dev_info;
    clone_data_t clone_data;
    alpm_vrf_type_t vt;
    int vrf_count;
    int last_level;
    uint8_t app;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        route_lt_idx_decode(u, m, lt_idx, &vt, &ipv, &app));

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);

    last_level = MULTI_LEVELS_FALL_BACK(vt) ? LEVEL1 : max_levels - 1;
    if (app == APP_LPM && vt == VRF_PRIVATE) {
        if (bcmptm_rm_alpm_is_large_vrf(u, m)) {
            w_vrf_s = bcmptm_rm_alpm_wvrf_get(u, m, app, vt, -1);
            w_vrf_e = w_vrf_s;
        } else {
            w_vrf_s = 0;
            w_vrf_e = dev_info->max_vrf;
        }
    } else if (app == APP_COMP0 || app == APP_COMP1) {
        w_vrf_s = 0;
        w_vrf_e = WVRF_COUNT_FP(dev_info->max_fp_op) - 1; /* Extend to L4_PORT */
    } else {
        w_vrf_s = bcmptm_rm_alpm_wvrf_get(u, m, app, vt, -1);
        w_vrf_e = w_vrf_s;
    }

    /* Check if table empty before allocating anything */
    for (w_vrf = w_vrf_s; w_vrf <= w_vrf_e; w_vrf++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, m, db, last_level,
                                        w_vrf, ipv, &trie_src));
        if (!trie_src) {
            continue;
        }

        num_entries += bcmptm_rm_alpm_trie_count(trie_src);
    }

    if (num_entries == 0) {
        SHR_EXIT();
    }

    ALPM_ALLOC(SNAP_INFO(u, m, lt_idx), sizeof(snap_info_t), "bcmptmRmalpmSnapinfo");
    sal_memset(SNAP_INFO(u, m, lt_idx), 0, sizeof(snap_info_t));

    SNAP_VRF_S(u, m, lt_idx) = w_vrf_s;
    SNAP_VRF_E(u, m, lt_idx) = w_vrf_e;

    vrf_count = w_vrf_e - w_vrf_s + 1;

    ALPM_ALLOC(SNAP_TRIES(u, m, lt_idx),
               sizeof(rm_alpm_trie_t *) * vrf_count,
               "bcmptmRmalpmSnaptries");

    sal_memset(SNAP_TRIES(u, m, lt_idx), 0,
               sizeof(rm_alpm_trie_t *) * vrf_count);

    ALPM_ALLOC(SNAP_LMMS(u, m, lt_idx),
               sizeof(shr_lmm_hdl_t) * vrf_count,
               "bcmptmRmalpmSnaptries");

    sal_memset(SNAP_LMMS(u, m, lt_idx), 0,
               sizeof(shr_lmm_hdl_t) * vrf_count);

    SNAP_NUM_ENTRIES(u, m, lt_idx) = num_entries;

    ALPM_ALLOC(SNAP_ENTRIES(u, m, lt_idx),
               sizeof(snap_entry_t) * SNAP_NUM_ENTRIES(u, m, lt_idx),
               "bcmptmRmalpmSnaptriesclone");
    sal_memset(SNAP_ENTRIES(u, m, lt_idx), 0,
               sizeof(snap_entry_t) * SNAP_NUM_ENTRIES(u, m, lt_idx));

    sal_memset(&clone_data, 0, sizeof(clone_data));
    clone_data.u = u;
    clone_data.lt_idx = lt_idx;
    clone_data.m = m;

    for (w_vrf = w_vrf_s; w_vrf <= w_vrf_e; w_vrf++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, m, db, last_level,
                                        w_vrf, ipv, &trie_src));
        if (!trie_src) {
            continue;
        }

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_clone_create(u, m, trie_src,
                                             entry_clone,
                                             &clone_data,
                                             &SNAP_TRIE(u, m, lt_idx, w_vrf - w_vrf_s),
                                             &SNAP_LMM(u, m, lt_idx, w_vrf - w_vrf_s)));
    }

exit:
    if (SHR_FUNC_ERR()) {
        snapshot_destroy(u, m, lt_idx);
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Sanity check against snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 *
 * \return SHR_E_XXX
 */
static int
snapshot_check(int u, int m, int lt_idx)
{
    int i;
    SHR_FUNC_ENTER(u);

    if (!LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        SHR_EXIT();
    }

    for (i = 0; i < SNAP_NUM_ENTRIES(u, m, lt_idx); i++) {
        if (SNAP_ENTRY_INDEX(u, m, lt_idx, i) != i) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Refresh the snapshot if already exists
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_refresh(int u, int m, int db, int lt_idx, int max_levels)
{
    SHR_FUNC_ENTER(u);

    snapshot_destroy(u, m, lt_idx);
    SHR_IF_ERR_EXIT(snapshot_create(u, m, db, lt_idx, max_levels));

exit:
    SHR_FUNC_EXIT();

}

static void
snap_key_to_arg(int u, int m, snap_key_t *snk, alpm_arg_t *match_arg)
{
    if (match_arg && snk) {
        match_arg->key.vt = snk->vt;
        match_arg->key.t.w_vrf = snk->w_vrf;
        match_arg->key.t.ipv = snk->v6;
        match_arg->key.t.len = snk->len;
        sal_memcpy(&match_arg->key.t.trie_ip, &snk->trie_key,
                   sizeof(snk->trie_key));
    }
}

static void
snap_arg_to_key(int u, int m, alpm_arg_t *arg, snap_key_t *snk)
{
    if (arg && snk) {
        snk->vt = arg->key.vt;
        snk->w_vrf = arg->key.t.w_vrf;
        snk->v6  = arg->key.t.ipv;
        snk->len = arg->key.t.len;
        sal_memcpy(snk->trie_key.key, arg->key.t.trie_ip.key, sizeof(trie_ip_t));
    }
}


/*!
 * \brief Create (or refresh) snapshot and get first entry in it
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_get_first(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int max_levels)
{
    int rv;
    int lt_idx;
    uint8_t is_src;
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    SHR_IF_ERR_EXIT(
        route_lt_idx_encode(u, m, ltid, arg->key.t.app, arg->key.vt,
                            arg->key.t.ipv, is_src, &lt_idx));
    if (SNAP_INFO(u, m, lt_idx) == NULL) {
        SHR_IF_ERR_EXIT(snapshot_create(u, m, arg->db, lt_idx, max_levels));
    } else {
        SHR_IF_ERR_EXIT(snapshot_refresh(u, m, arg->db, lt_idx, max_levels));
    }

    if (SNAP_INFO(u, m, lt_idx) == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(snapshot_check(u, m, lt_idx));

    snap_key_to_arg(u, m, &SNAP_ENTRY_KEY(u, m, lt_idx, 0), arg);
    ALPM_ASSERT(arg->valid);
    rv = bcmptm_rm_alpm_match(u, m, arg, max_levels);

    if (SHR_SUCCESS(rv)) {
        SNAP_PEAK_IDX(u, m, lt_idx) = 0;
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get next entry in the snapshot based on given key.
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_get_next(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int max_levels)
{
    int idx = 0;
    int rv;
    snap_key_t snk;
    alpm_arg_t *match_arg = NULL;
    int lt_idx;
    uint8_t is_src;

    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    SHR_IF_ERR_EXIT(
        route_lt_idx_encode(u, m, ltid, arg->key.t.app, arg->key.vt,
                            arg->key.t.ipv, is_src, &lt_idx));

    SHR_NULL_CHECK(SNAP_INFO(u, m, lt_idx), SHR_E_UNAVAIL);

    snap_arg_to_key(u, m, arg, &snk);

    rv = snap_key_search(u, m, lt_idx, &snk, &idx);

    if (rv == SHR_E_NOT_FOUND ||
        (rv == SHR_E_NONE && (idx + 1) >= SNAP_NUM_ENTRIES(u, m, lt_idx))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);

    } else if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    match_arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
    if (match_arg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    do {
        bcmptm_rm_alpm_arg_init(u, m, match_arg);
        snap_key_to_arg(u, m, &SNAP_ENTRY_KEY(u, m, lt_idx, ++idx), match_arg);
        match_arg->pkm = arg->pkm;
        match_arg->key.t.app = arg->key.t.app;
        match_arg->key.t.max_bits = arg->key.t.max_bits;
        match_arg->key.kt = arg->key.kt;
        match_arg->key.vt = arg->key.vt;
        match_arg->db = arg->db;
        match_arg->valid = 1;
        rv = bcmptm_rm_alpm_match(u, m, match_arg, max_levels);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    } while (rv == SHR_E_NOT_FOUND && (idx + 1) < SNAP_NUM_ENTRIES(u, m, lt_idx));

    if (rv == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }
    sal_memcpy(arg, match_arg, sizeof(alpm_arg_t));
    SNAP_PEAK_IDX(u, m, lt_idx) = idx;

exit:
    if (match_arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], match_arg);
    }

    SHR_FUNC_EXIT();
}

static int
arg_key_matched(int u, int m, alpm_arg_t *arg, elem_key_t *key)
{
    if (arg->key.t.w_vrf == key->t.w_vrf &&
        arg->key.t.len == key->t.len &&
        arg->key.t.ipv == key->t.ipv &&
        !sal_memcmp(&arg->key.t.trie_ip, &key->t.trie_ip,
                    sizeof(arg->key.t.trie_ip))) {
        return 1; /* matched */
    }
    return 0;
}

static void
traverse_cache_cleanup(int u, int m)
{
    bcmptm_rm_alpm_trie_traverse_cache_cleanup(u, m);

}


static int
traverse_cmp_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    alpm_pivot_t *pivot;
    traverse_cmp_data_t *data = (traverse_cmp_data_t *)user_data;

    assert(node->type == PAYLOAD);
    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    if (arg_key_matched(data->u, data->m, data->arg, &pivot->key)) {
        return SHR_E_BUSY;
    }

    return SHR_E_NONE;
}
/*!
 * \brief Get first entry in active state
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot2_get_next(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int max_levels, bool get_first)
{
    int lt_idx;
    uint8_t is_src;
    rm_alpm_trie_t *trie;
    rm_alpm_trie_node_t *trie_root;
    alpm_pivot_t *latest_pivot; /* last traversed entry */
    elem_key_t *last_key;
    void *payload = NULL;
    shr_dq_t *vrf_list = NULL;
    shr_dq_t *curr_vrf = NULL;
    alpm_vrf_hdl_t *vrf_hdl;
    bool trie_first;
    int last_level;
    bool refresh_cache = false;
    traverse_cmp_data_t data;

    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    SHR_IF_ERR_EXIT(
        route_lt_idx_encode(u, m, ltid, arg->key.t.app, arg->key.vt,
                            arg->key.t.ipv, is_src, &lt_idx));

    last_level = MULTI_LEVELS_FALL_BACK(arg->key.vt) ? LEVEL1 : max_levels - 1;
    vrf_list = &DB_VRF_HDL_LIST(u, m, arg->db, arg->key.t.ipv);
    if (SHR_DQ_EMPTY(vrf_list)) {
        /* Empty */
        snap_info2[u][m][lt_idx]->state = 0;
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (get_first) {
        if (snap_info2[u][m][lt_idx]->state == 1) {
            traverse_cache_cleanup(u, m);
            /* SHR_ERR_EXIT(SHR_E_BUSY); */
        }
        curr_vrf = vrf_list;
        do {
            curr_vrf = SHR_DQ_NEXT(curr_vrf, shr_dq_t*);
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t*, curr_vrf, dqnode);
            if ((vrf_hdl->vt == arg->key.vt || arg->key.t.app != APP_LPM) &&
                vrf_hdl->app == arg->key.t.app) {
                break;
            }

        } while (curr_vrf != vrf_list);
        if (curr_vrf == vrf_list) {
            snap_info2[u][m][lt_idx]->state = 0;
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        trie_first = true;
        snap_info2[u][m][lt_idx]->state = 1; /* In traverse */
    } else {
        vrf_hdl = snap_info2[u][m][lt_idx]->vrf_hdl;
        trie_first = false;
        if (snap_info2[u][m][lt_idx]->cache_valid) {
            last_key = &snap_info2[u][m][lt_idx]->last_key;
            if (!arg_key_matched(u, m, arg, last_key)) { /* cache valid but cache miss */
                refresh_cache = true;
            }
        } else {
            /* Indicate cache invalid */
            refresh_cache = true;
        }
    }
    if (refresh_cache) {
        traverse_cache_cleanup(u, m);
        curr_vrf = SHR_DQ_HEAD(vrf_list, shr_dq_t*);
        snap_info2[u][m][lt_idx]->state = 1; /* In traverse */

        data.u = u;
        data.m = m;
        data.arg = arg;
        while (curr_vrf != vrf_list && payload == NULL) {
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t*, curr_vrf, dqnode);
            if ((vrf_hdl->vt != arg->key.vt && arg->key.t.app == APP_LPM) ||
                vrf_hdl->app != arg->key.t.app) {
                curr_vrf = SHR_DQ_NEXT(curr_vrf, shr_dq_t*);
                continue;
            }
            trie_first = true;
            trie = vrf_hdl->pivot_trie[last_level];
            assert(trie);
            if (trie) {
                trie_root = trie->trie;
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_trie_traverse_get_next(u, m, trie_first, trie_root,
                                                          &snap_info2[u][m][lt_idx]->queue,
                                                          traverse_cmp_cb, &data,
                                                          &payload));
                /* payload != NULL indicates found. */
            }
            trie_first = false;

            curr_vrf = SHR_DQ_NEXT(curr_vrf, shr_dq_t*);

        }
        if (payload == NULL) {
            snap_info2[u][m][lt_idx]->state = 0;
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

    }

    trie = vrf_hdl->pivot_trie[last_level];
    /* assert(trie); possible during traverse & delete. */
    if (trie) {
        trie_root = trie->trie;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_traverse_get_next(u, m, trie_first, trie_root,
                                                  &snap_info2[u][m][lt_idx]->queue,
                                                  NULL, NULL,
                                                  &payload));

    }

    while (payload == NULL) {
        curr_vrf = SHR_DQ_NEXT(&vrf_hdl->dqnode, shr_dq_t*);
        if (curr_vrf == vrf_list) {
            snap_info2[u][m][lt_idx]->state = 0;
            /* traverse_done */
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t*, curr_vrf, dqnode);
        if ((vrf_hdl->vt != arg->key.vt && arg->key.t.app == APP_LPM) ||
            vrf_hdl->app != arg->key.t.app) {
            continue;
        }
        trie_first = true;
        trie = vrf_hdl->pivot_trie[last_level];
        assert(trie);
        if (trie) {
            trie_root = trie->trie;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_traverse_get_next(u, m, trie_first, trie_root,
                                                      &snap_info2[u][m][lt_idx]->queue,
                                                      NULL, NULL,
                                                      &payload));
        }
    }

    snap_info2[u][m][lt_idx]->vrf_hdl = vrf_hdl;

    arg->match_ready = 1;
    latest_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, payload, pvtnode);
    arg->pivot[last_level] = latest_pivot;
    arg->key.t = latest_pivot->key.t;
    arg->key.vt = latest_pivot->key.vt;
    snap_info2[u][m][lt_idx]->cache_valid = 0; /* Invalid cache incase of match failure. */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_match(u, m, arg, max_levels));
    arg->match_ready = 0;
    snap_info2[u][m][lt_idx]->cache_valid = 1;
    snap_info2[u][m][lt_idx]->last_key = latest_pivot->key;

exit:
    SHR_FUNC_EXIT();

}



/*******************************************************************************
 * Public Functions
 */
void
bcmptm_rm_alpm_traverse_cache_invalidate(int u, int m,
                                         bcmltd_sid_t ltid,
                                         alpm_arg_t *arg, bool del)
{
    int lt_idx;
    uint8_t is_src;
    uint8_t do_invalidate = 0;
    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    route_lt_idx_encode(u, m, ltid, arg->key.t.app, arg->key.vt,
                        arg->key.t.ipv, is_src, &lt_idx);
    if (snap_info2[u][m][lt_idx]) {
        if (del) {
            if (snap_info2[u][m][lt_idx]->cache_valid &&
                !arg_key_matched(u, m, arg, &snap_info2[u][m][lt_idx]->last_key)) {
                do_invalidate = snap_info2[u][m][lt_idx]->state;
            }
        } else if (!arg->req_update) {
            do_invalidate = snap_info2[u][m][lt_idx]->state;
        }
    }
    if (do_invalidate) {
        snap_info2[u][m][lt_idx]->cache_valid = 0;
    }
    return;
}

int
bcmptm_rm_alpm_get_first(int u, int m, bcmltd_sid_t ltid,
                         bool snapshot_en,
                         alpm_arg_t *arg,
                         int max_levels)
{
    SHR_FUNC_ENTER(u);

    if (snapshot_en) {
        SHR_ERR_EXIT(snapshot_get_first(u, m, ltid, arg, max_levels));
    } else {
        SHR_ERR_EXIT(snapshot2_get_next(u, m, ltid, arg, max_levels, true));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_get_next(int u, int m, bcmltd_sid_t ltid,
                        bool snapshot_en,
                        alpm_arg_t *arg,
                        int max_levels)
{
    SHR_FUNC_ENTER(u);

    if (snapshot_en) {
        SHR_ERR_EXIT(
            snapshot_get_next(u, m, ltid, arg, max_levels));
    } else {
        SHR_ERR_EXIT(
            snapshot2_get_next(u, m, ltid, arg, max_levels, false));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_traverse_done(int u, int m,
                             bcmltd_sid_t req_ltid,
                             bool snapshot_en, alpm_arg_t *arg)
{
    int lt_idx;
    uint8_t is_src;

    SHR_FUNC_ENTER(u);
    is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);
    SHR_IF_ERR_EXIT(
        route_lt_idx_encode(u, m, req_ltid, arg->key.t.app, arg->key.vt,
                            arg->key.t.ipv, is_src, &lt_idx));
    if (snapshot_en) {
        if (snap_info2[u][m][lt_idx]->state) {
            snap_info2[u][m][lt_idx]->state = 0;
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    } else {
        if (SNAP_INFO(u, m, lt_idx)) {
            snapshot_destroy(u, m, lt_idx);
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_alpm_traverse_init(int u, int m)
{
    int lt_idx;
    SHR_FUNC_ENTER(u);

    for (lt_idx = 0; lt_idx < MAX_ALPM_LTS; lt_idx++) {
        ALPM_ALLOC(snap_info2[u][m][lt_idx], sizeof(snap_info2_t), "bcmptmRmalpmSnapinfo2");
        sal_memset(snap_info2[u][m][lt_idx], 0, sizeof(snap_info2_t));
    }

exit:
    SHR_FUNC_EXIT();

}

void
bcmptm_rm_alpm_traverse_cleanup(int u, int m)
{
    int lt_idx;

    for (lt_idx = 0; lt_idx < MAX_ALPM_LTS; lt_idx++) {
        SHR_FREE(snap_info2[u][m][lt_idx]);
        snapshot_destroy(u, m, lt_idx);
    }

}


