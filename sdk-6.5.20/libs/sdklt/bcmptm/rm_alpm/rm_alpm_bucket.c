/*! \file rm_alpm_bucket.c
 *
 * Resource Manager for ALPM Level-N bucket
 *
 * This file contains the resource manager for ALPM Level-N bucket
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

#include "rm_alpm_common.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_device.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_trie.h"


/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE              BSL_LS_BCMPTM_RMALPM

#define BKT_MGMT(u, m)                 (bucket_mgmt[u][m])
#define BKT_MGMT_EXEC(u, m, fn)        (BKT_MGMT(u, m)->fn)

/*******************************************************************************
 * Typedefs
 */

typedef struct bucket_trie_sanity_ctrl_s {
    int unit;
    int m;
    int db;
    int ln;
    alpm_pivot_t *pivot;
} bucket_trie_sanity_ctrl_t;

typedef struct pivot_trie_sanity_ctrl_s {
    int unit;
    int m;
    int db;
    int ln;
    rm_alpm_trie_t *pivot_trie[MAX_LEVELS];
} pivot_trie_sanity_ctrl_t;
/*******************************************************************************
 * Private Variables
 */
static bucket_mgmt_t *bucket_mgmt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Private Functions
 */
static int
bucket_trie_sanity_cb(rm_alpm_trie_node_t *trie, void *info)
{
    int rv;
    int db, ln, u, m;
    alpm_pivot_t *pivot, *pre_pivot;
    alpm_bkt_elem_t *bkt_elem;
    alpm_pivot_t *next_level_pivot;
    bucket_trie_sanity_ctrl_t *bucket_trie_sanity_ctrl =
        (bucket_trie_sanity_ctrl_t *) info;

    db = bucket_trie_sanity_ctrl->db;
    m = bucket_trie_sanity_ctrl->m;
    ln = bucket_trie_sanity_ctrl->ln;
    u = bucket_trie_sanity_ctrl->unit;

    bkt_elem = TRIE_ELEMENT_GET(alpm_bkt_elem_t*, trie, bktnode);
    bcmptm_rm_alpm_pvt_find(u, m, db, PREV_LEVEL(ln), &bkt_elem->key.t, &pivot);
    if (ln == LEVEL3) {
        bcmptm_rm_alpm_pvt_find(u, m, db, PREV_LEVEL(PREV_LEVEL(ln)),
                                &bkt_elem->key.t, &pre_pivot);
        assert(pre_pivot ==
            bcmptm_rm_alpm_bucket_pivot_get(u, m, db, PREV_LEVEL(ln), pivot->index));
        assert(pivot == bucket_trie_sanity_ctrl->pivot);
    } else {
        bucket_trie_sanity_ctrl_t ctrl1;


        next_level_pivot = bkt_elem;
        ctrl1.pivot = next_level_pivot;
        ctrl1.db = db;
        ctrl1.ln = NEXT_LEVEL(ln);
        ctrl1.unit = u;
        assert(next_level_pivot->bkt);
        rv = bcmptm_rm_alpm_trie_traverse(u, m,
                                          next_level_pivot->bkt->trie->trie,
                                          bucket_trie_sanity_cb,
                                          &ctrl1,
                                          TRAVERSE_BFS);
        assert(rv == SHR_E_NONE);

    }
    return 0;
}

static int
pivot_trie_sanity_cb(rm_alpm_trie_node_t *trie, void *info)
{
    int rv;
    alpm_pivot_t *pivot;
    bucket_trie_sanity_ctrl_t ctrl1;
    pivot_trie_sanity_ctrl_t *ctrl2 = (pivot_trie_sanity_ctrl_t *) info;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, trie, pvtnode);
    ctrl1.pivot = pivot;
    ctrl1.m = ctrl2->m;
    ctrl1.db = ctrl2->db;
    ctrl1.ln = ctrl2->ln;
    ctrl1.unit = ctrl2->unit;

    rv = bcmptm_rm_alpm_trie_traverse(ctrl1.unit, ctrl1.m,
                                      pivot->bkt->trie->trie,
                                      bucket_trie_sanity_cb,
                                      &ctrl1,
                                      TRAVERSE_BFS);
    assert(rv == SHR_E_NONE);
    return rv;
}

static void
bucket_merge_sanity_check(int u, int m, int db, int ln,
                          alpm_pivot_t *parent_pivot,
                          alpm_pivot_t *child_pivot)
{
    alpm_pivot_t *child_pivot_delete = NULL;
    int rv;
    rv = bcmptm_rm_alpm_trie_merge(u, m, parent_pivot->bkt->trie,
                                   child_pivot->bkt->trie->trie,
                                   &child_pivot->key.t.trie_ip.key[0],
                                   child_pivot->key.t.len);
    child_pivot->bkt->trie->trie = NULL;
    assert(rv == 0);
    rv = bcmptm_rm_alpm_pvt_delete(u, m, db, PREV_LEVEL(ln), FALSE,
                                   &child_pivot->key.t, &child_pivot_delete);
    assert(rv == 0);
    assert(child_pivot == child_pivot_delete);
    rv = bcmptm_rm_alpm_elem_free(u, m, db, PREV_LEVEL(ln), false, child_pivot_delete);
    assert(rv == 0);
}

static int
bucket_merge_sanity_cb(rm_alpm_trie_node_t *ptrie, rm_alpm_trie_node_t *trie,
                       rm_alpm_trie_traverse_states_e_t *state, void *info)
{
    int u;
    int db, ln, m;
    alpm_pivot_t *parent_pivot, *child_pivot;
    pivot_trie_sanity_ctrl_t *merge_ctrl = (pivot_trie_sanity_ctrl_t *) info;
    rm_alpm_trie_node_t *lpm;

    /* Always reset state */
    *state = TRAVERSE_STATE_NONE;

    db = merge_ctrl->db;
    m = merge_ctrl->m;
    ln = merge_ctrl->ln;
    u = merge_ctrl->unit;

    parent_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, ptrie, pvtnode);
    child_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, trie, pvtnode);
    if (!parent_pivot || !child_pivot) {
        return SHR_E_NONE;
    }
    bcmptm_rm_alpm_trie_find_lpm2(merge_ctrl->pivot_trie[PREV_LEVEL(ln)],
                                  &child_pivot->key.t.trie_ip.key[0],
                                  child_pivot->key.t.len, &lpm);
    assert(lpm == ptrie);

    bucket_merge_sanity_check(u, m, db, ln, parent_pivot, child_pivot);
    *state = TRAVERSE_STATE_DELETED;
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_bkt_hdl_init(int u, int m, bool recover)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bkt_hdl_init)) {
        return BKT_MGMT_EXEC(u, m, bkt_hdl_init)(u, m, recover);
    }
    return SHR_E_UNAVAIL;
}

void
bcmptm_rm_alpm_bkt_hdl_cleanup(int u, int m)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bkt_hdl_cleanup)) {
        BKT_MGMT_EXEC(u, m, bkt_hdl_cleanup)(u, m);
    }
}

int
bcmptm_rm_alpm_bkt_hdl_get(int u, int m, int db, int ln, bkt_hdl_t **bkt_hdl)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bkt_hdl_get)) {
        return BKT_MGMT_EXEC(u, m, bkt_hdl_get)(u, m, db, ln, bkt_hdl);
    }
    return SHR_E_UNAVAIL;
}

void
bcmptm_rm_alpm_adata_encode(int u, int m, int ln, alpm_arg_t *arg, void *adata)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, alpm_data_encode)) {
        BKT_MGMT_EXEC(u, m, alpm_data_encode)(u, m, ln, arg, adata);
    }
}

void
bcmptm_rm_alpm_bucket_pivot_set(int u, int m, int db, int ln, log_bkt_t group,
                                alpm_pivot_t *pivot)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_pivot_set)) {
        BKT_MGMT_EXEC(u, m, bucket_pivot_set)(u, m, db, ln, group, pivot);
    }
}

alpm_pivot_t *
bcmptm_rm_alpm_bucket_pivot_get(int u, int m, int db, int ln, ln_index_t ln_index)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_pivot_get)) {
        return BKT_MGMT_EXEC(u, m, bucket_pivot_get)(u, m, db, ln, ln_index);
    }
    return NULL;
}


int
bcmptm_rm_alpm_bucket_free(int u, int m, int db, int ln, log_bkt_t group)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_free)) {
        return BKT_MGMT_EXEC(u, m, bucket_free)(u, m, db, ln, group);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_alloc(int u, int m, int ln, alpm_arg_t *arg, log_bkt_t *group)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_alloc)) {
        return BKT_MGMT_EXEC(u, m, bucket_alloc)(u, m, ln, arg, group);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_delete(int u, int m, int db, int ln, alpm_pivot_t *pivot,
                             ln_index_t ent_idx)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_delete)) {
        return BKT_MGMT_EXEC(u, m, bucket_delete)(u, m, db, ln, pivot, ent_idx);
    }
    return SHR_E_UNAVAIL;

}
int
bcmptm_rm_alpm_bucket_insert(int u, int m, int ln, alpm_arg_t *arg,
                             alpm_pivot_t *pivot)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_insert)) {
        return BKT_MGMT_EXEC(u, m, bucket_insert)(u, m, ln, arg, pivot);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_split_insert(int u, int m, int ln, alpm_arg_t *arg,
                                   alpm_pivot_t *parent_pivot,
                                   bkt_split_aux_t *split_aux)

{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_split_insert)) {
        return BKT_MGMT_EXEC(u, m, bucket_split_insert)
                (u, m, ln, arg, parent_pivot, split_aux);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_recover(int u, int m, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_recover)) {
        return BKT_MGMT_EXEC(u, m, bucket_recover)(u, m, ln, arg);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_recover_done(int u, int m, int db, int ln)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_recover_done)) {
        return BKT_MGMT_EXEC(u, m, bucket_recover_done)(u, m, db, ln);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_entry_read(int u, int m, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, entry_read)) {
        return BKT_MGMT_EXEC(u, m, entry_read)(u, m, ln, arg);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_entry_hw_idx_get(int u, int m, int db, int ln,
                                       ln_index_t ent_idx,
                                       uint32_t *hw_idx)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, entry_hw_idx_get)) {
        *hw_idx = BKT_MGMT_EXEC(u, m, entry_hw_idx_get)(u, m, db, ln, ent_idx);
        return SHR_E_NONE;
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_entry_write(int u, int m, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, entry_write)) {
        return BKT_MGMT_EXEC(u, m, entry_write)(u, m, ln, arg);
    }
    return SHR_E_UNAVAIL;

}

void
bcmptm_rm_alpm_bucket_sanity(int u, int m, int db, int ln, log_bkt_t log_bkt)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_sanity)) {
        BKT_MGMT_EXEC(u, m, bucket_sanity)(u, m, db, ln, log_bkt);
    }
}


void
bcmptm_rm_alpm_buckets_sanity(int u, int m, int db, int ln, int w_vrf, bool merge)
{
    int ipv, rv;
    rm_alpm_trie_t *pivot_trie;
    pivot_trie_sanity_ctrl_t ctrl = {0};
    alpm_vrf_hdl_t *vrf_hdl;
    shr_dq_t *elem;

    ctrl.unit = u;
    ctrl.m = m;
    ctrl.db = db;
    ctrl.ln = ln;
    for (ipv = IP_VER_4; ipv < IP_VER_COUNT; ipv++) {
        SHR_DQ_TRAVERSE(&DB_VRF_HDL_LIST(u, m, db, ipv), elem)
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t *, elem, dqnode);
            if (vrf_hdl->w_vrf == w_vrf || w_vrf == -1) {
                pivot_trie = vrf_hdl->pivot_trie[PREV_LEVEL(ln)];
                if (!vrf_hdl->in_use) {
                    continue;
                }
                ALPM_ASSERT(pivot_trie);
                sal_memcpy(ctrl.pivot_trie, vrf_hdl->pivot_trie,
                           sizeof(vrf_hdl->pivot_trie));
                rv = bcmptm_rm_alpm_trie_traverse(u, m,
                                                  pivot_trie->trie,
                                                  pivot_trie_sanity_cb,
                                                  &ctrl,
                                                  TRAVERSE_BFS);
                assert(rv == SHR_E_NONE);
                if (merge) {
                    /* This is a disruptive sanity check, can only do once. */
                    rv = bcmptm_rm_alpm_trie_traverse2(pivot_trie->trie,
                                                       bucket_merge_sanity_cb,
                                                       &ctrl,
                                                       TRAVERSE_POSTORDER);
                    assert(rv == SHR_E_NONE);
                }
            }
        SHR_DQ_TRAVERSE_END(&DB_VRF_HDL_LIST(u, m, db, ipv), elem);
    }
}

int
bcmptm_rm_alpm_bucket_resource_check(int u, int m, alpm_arg_t *arg, int extra)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_resource_check)) {
        return BKT_MGMT_EXEC(u, m, bucket_resource_check)(u, m, arg, extra);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_table_dump(int u, int m, int ln, int lvl_cnt, alpm_arg_t *arg, int *count)
{
    if (BKT_MGMT(u, m) && BKT_MGMT_EXEC(u, m, bucket_table_dump)) {
        return BKT_MGMT_EXEC(u, m, bucket_table_dump)(u, m, ln, lvl_cnt, arg, count);
    }
    return SHR_E_UNAVAIL;
}


int
bcmptm_rm_alpm_bucket_init(int u, int m)
{
    alpm_dev_info_t *dev_info;
    SHR_FUNC_ENTER(u);

    ALPM_ALLOC(bucket_mgmt[u][m], sizeof(bucket_mgmt_t), "bcmptmRmalpmBktcbs");
    sal_memset(bucket_mgmt[u][m], 0, sizeof(bucket_mgmt_t));

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    switch (dev_info->alpm_ver) {
        case ALPM_VERSION_1:
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_ver1_register(u, bucket_mgmt[u][m]));
            break;
        case ALPM_VERSION_0:
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_ver0_register(u, bucket_mgmt[u][m]));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_bucket_cleanup(int u, int m)
{
    SHR_FREE(bucket_mgmt[u][m]);
}



