/*! \file rm_alpm_common.c
 *
 * Databases for ALPM
 *
 * This file contains the databases for ALPM all levels.
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
#include <shr/shr_dq.h>

#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

#define BUCKETS_CHUNK_SIZE   512
#define ELEMS_CHUNK_SIZE     1024
#define ADATAS_CHUNK_SIZE    1024

/*******************************************************************************
 * Private variables
 */
static int src_dbs[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS];
static int num_src_dbs[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*!< ALPM databases */
alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
alpm_internals_t bcmptm_rm_alpm_internals[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS][LEVELS];
uint8_t bcmptm_rm_alpm_mid_dt[BCMDRD_CONFIG_MAX_UNITS][ALPMS][DBS];

/*******************************************************************************
 * Private Functions
 */


/*!
 * \brief Initialize ALPM database
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX
 */
static int
database_init(int u, int m, alpm_config_t *config)
{
    int db_idx;
    alpm_dev_info_t *dev_info;
    int vrf_cnt;
    int alpm_mode;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);

    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    /* Allocate and init ALPM Database */
    ALPM_DB_CNT(u, m) = DBS;

    ALPM_ALLOC(ALPM_DBS(u, m), sizeof(alpm_db_t) * ALPM_DB_CNT(u, m), "bcmptmRmalpmDb");
    sal_memset(ALPM_DBS(u, m), 0, sizeof(alpm_db_t) * ALPM_DB_CNT(u, m));

    num_src_dbs[u][m] = 0;
    alpm_mode = bcmptm_rm_alpm_mode(u, m);
    if (alpm_mode & ALPM_MODE_FP_COMP) {
        /* COMP DST */
        DB_VRF_HDL_CNT(u, m, DB2) = WVRF_COUNT_FP(dev_info->max_fp_op); /* Opaque1: L4_PORT */
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_6));
        /* COMP SRC */
        DB_VRF_HDL_CNT(u, m, DB3) = WVRF_COUNT_FP(dev_info->max_fp_op); /* Opaque1: L4_PORT */
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB3, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB3, IP_VER_6));
        src_dbs[u][m][num_src_dbs[u][m]++] = DB3;

        if (alpm_mode & ALPM_MODE_RPF) {
            /* DST */
            DB_VRF_HDL_CNT(u, m, DB0) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_6));
            /* SRC */
            DB_VRF_HDL_CNT(u, m, DB1) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_6));
            src_dbs[u][m][num_src_dbs[u][m]++] = DB1;
        } else if (alpm_mode & ALPM_MODE_PARALLEL) {
            /* Private */
            DB_VRF_HDL_CNT(u, m, DB0) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_6));
            /* Global Low + Global High */
            DB_VRF_HDL_CNT(u, m, DB1) = WVRF_COUNT_PARALLEL_PUBLIC;
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_6));
        } else if (!(alpm_mode & ALPM_MODE_NO_LPM)) {
            DB_VRF_HDL_CNT(u, m, DB0) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_6));
        }
    } else {
        if (alpm_mode & ALPM_MODE_PARALLEL) {
            /* Private + L3MC*/
            DB_VRF_HDL_CNT(u, m, DB0) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_6));
            /* Global Low + Global High */
            DB_VRF_HDL_CNT(u, m, DB1) = WVRF_COUNT_PARALLEL_PUBLIC;
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB1, IP_VER_6));
        } else {
            DB_VRF_HDL_CNT(u, m, DB0) = WVRF_COUNT(dev_info->max_vrf);
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_4));
            SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB0, IP_VER_6));
        }
        if (alpm_mode & ALPM_MODE_RPF) {
            if (alpm_mode & ALPM_MODE_PARALLEL) {
                /* Private */
                DB_VRF_HDL_CNT(u, m, DB2) = WVRF_COUNT(dev_info->max_vrf);
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_4));
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_6));
                src_dbs[u][m][num_src_dbs[u][m]++] = DB2;
                /* Global Low + Global High */
                DB_VRF_HDL_CNT(u, m, DB3) = WVRF_COUNT_PARALLEL_PUBLIC;
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB3, IP_VER_4));
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB3, IP_VER_6));
                src_dbs[u][m][num_src_dbs[u][m]++] = DB3;
            } else {
                DB_VRF_HDL_CNT(u, m, DB2) = WVRF_COUNT(dev_info->max_vrf);
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_4));
                SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, m, DB2, IP_VER_6));
                src_dbs[u][m][num_src_dbs[u][m]++] = DB2;
            }
        }
    }

    /* Database initialization */
    for (db_idx = 0; db_idx < ALPM_DB_CNT(u, m); db_idx++) {
        alpm_db_t *database_ptr = &ALPM_DB(u, m, db_idx);
        int rv;

        database_ptr->max_levels = config->core.db_levels[db_idx];

        /* VRF handle initialization */
        vrf_cnt = DB_VRF_HDL_CNT(u, m, db_idx);
        if (vrf_cnt == 0) {
            continue;
        }
        ALPM_ALLOC(database_ptr->vrf_hdl[IP_VER_4],
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt,
                   "bcmptmRmalpmIpv4pvthdl");
        ALPM_ALLOC(database_ptr->vrf_hdl[IP_VER_6],
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt,
                   "bcmptmRmalpmIpv6pvthdl");
        sal_memset(database_ptr->vrf_hdl[IP_VER_4], 0,
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt);
        sal_memset(database_ptr->vrf_hdl[IP_VER_6], 0,
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt);

        if (!database_ptr->elems) {
            ALPM_LMM_INIT
                (alpm_elem_t, database_ptr->elems, ELEMS_CHUNK_SIZE,
                 false, rv);
            if (rv != 0) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
        }

        if (!database_ptr->buckets) {
            ALPM_LMM_INIT
                (elem_bkt_t, database_ptr->buckets, BUCKETS_CHUNK_SIZE,
                 false, rv);
            if (rv != 0) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
        }

        if (!database_ptr->adatas) {
            ALPM_LMM_INIT
                (assoc_data_t, database_ptr->adatas, ADATAS_CHUNK_SIZE,
                 false, rv);
            if (rv != 0) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
        }

        if (bcmptm_rm_alpm_is_large_data_type(u, m, db_idx)) {
            bcmptm_rm_alpm_mid_dt_set(u, m, db_idx, DATA_FULL_3);
        } else {
            bcmptm_rm_alpm_mid_dt_set(u, m, db_idx, DATA_FULL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ALPM database
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX
 */
static void
database_cleanup(int u, int m)
{
    int i;

    if (!ALPM_DBS(u, m)) {
        return;
    }

    for (i = 0; i < ALPM_DB_CNT(u, m); i++) {
        alpm_db_t *database_ptr = &ALPM_DB(u, m, i);

        SHR_FREE(database_ptr->vrf_hdl[IP_VER_4]);
        SHR_FREE(database_ptr->vrf_hdl[IP_VER_6]);

        shr_lmm_delete(database_ptr->elems);
        shr_lmm_delete(database_ptr->buckets);
        shr_lmm_delete(database_ptr->adatas);
    }

    SHR_FREE(ALPM_DBS(u, m));
}

/*******************************************************************************
 * Public Functions
 */
format_type_t
bcmptm_rm_alpm_format_type_get(int u, int m, int db, int ln)
{
    if (DB_LAST_LEVEL(u, m, db) == ln) {
        return FORMAT_ROUTE;
    } else {
        return FORMAT_PIVOT;
    }
}

int
bcmptm_rm_alpm_max_levels(int u, int m, int db)
{
    return DB_LEVELS(u, m, db);
}


bool
bcmptm_rm_alpm_db_is_src(int u, int m, int db)
{
    uint8_t i;
    for (i = 0; i < num_src_dbs[u][m]; i++) {
        if (db == src_dbs[u][m][i]) {
            return true;
        }
    }
    return false;
}

int
bcmptm_rm_alpm_db(int u, int m, uint8_t app, uint8_t is_src, alpm_vrf_type_t vt,
                  uint8_t *dbo)
{
    int db;
    int alpm_mode;

    SHR_FUNC_ENTER(u);
    alpm_mode = bcmptm_rm_alpm_mode(u, m);
    if (alpm_mode & ALPM_MODE_FP_COMP) {
        if (app == APP_COMP0 || app == APP_COMP1) {
            db = is_src ? DB3 : DB2;
        } else {
            if (alpm_mode & ALPM_MODE_RPF) {
                db = is_src ? DB1 : DB0;
                /* Validate */
                if (app != APP_LPM && app != APP_L3MC) {
                    SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
                }
            } else {
                if (alpm_mode & ALPM_MODE_PARALLEL) {
                    db = (vt == VRF_PRIVATE) ? DB0 : DB1;
                } else {
                    db = DB0;
                }
                /* Validate */
                if (is_src) {
                    SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
                } else if (app != APP_LPM && app != APP_L3MC) {
                    SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
                }
            }
        }
    } else {
        if (alpm_mode & ALPM_MODE_RPF) {
            if (alpm_mode & ALPM_MODE_PARALLEL) {
                if (is_src) {
                    db = (vt == VRF_PRIVATE) ? DB2 : DB3;
                } else {
                    db = (vt == VRF_PRIVATE) ? DB0 : DB1;
                }
            } else {
                db = is_src ? DB2 : DB0;
            }
            /* Validate */
            if (app != APP_LPM && app != APP_L3MC) {
                SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
            }
        } else {
            if (alpm_mode & ALPM_MODE_PARALLEL) {
                db = (vt == VRF_PRIVATE) ? DB0 : DB1;
            } else {
                db = DB0;
            }
            /* Validate */
            if (is_src) {
                SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
            } else if (app != APP_LPM && app != APP_L3MC) {
                SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
            }
        }
    }
    *dbo = db;
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_bkt_insert(int u, int m, int ln, arg_key_t *key,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t *bkt_elem)
{
    SHR_FUNC_ENTER(u);
    /*
     * No bkt_elem in Level-1
     */
    ALPM_ASSERT(ln == LEVEL3 || ln == LEVEL2);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_insert(u, m, bkt->trie,
                                   &key->t.trie_ip.key[0],
                                   NULL,
                                   key->t.len,
                                   &bkt_elem->bktnode));
    SHR_DQ_INSERT_HEAD(&bkt->list, &bkt_elem->dqnode);
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_bkt_delete(int u, int m, int ln, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bucket)
{
    rm_alpm_trie_node_t *bkt_elem_node = NULL;
    alpm_bkt_elem_t *bkt_elem;

    SHR_FUNC_ENTER(u);
    /*
     * No bucket in Level-1
     */
    ALPM_ASSERT(ln == LEVEL3 || ln == LEVEL2);
    SHR_NULL_CHECK(bkt->trie, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_delete(u, m, bkt->trie,
                                   tp->trie_ip.key,
                                   tp->len,
                                   &bkt_elem_node));
    bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, bkt_elem_node, bktnode);
    SHR_DQ_REMOVE(&bkt_elem->dqnode);
    if (bucket) {
        *bucket = bkt_elem;
    }

    /* Leave it to the bcmptm_rm_alpm_elem_free */
    /*
       if (bucket_trie->trie == NULL) {
        bcmptm_rm_alpm_trie_destroy(u, m, bucket_trie);
       }
     */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_bkt_lookup(int u, int m, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bkt_elem)
{
    int rv;
    rm_alpm_trie_node_t *bkt_elem_node = NULL;
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(bkt->trie, SHR_E_NOT_FOUND);
    rv = bcmptm_rm_alpm_trie_search(bkt->trie,
                                    tp->trie_ip.key,
                                    tp->len,
                                    &bkt_elem_node);
    if (SHR_SUCCESS(rv)) {
        *bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, bkt_elem_node, bktnode);
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_elem_alloc(int u, int m, int db, int ln, bool direct_route,
                          arg_key_t *key,
                          alpm_elem_t **elem_pyld)
{
    int mkl;
    alpm_elem_t *elem;
    elem_bkt_t *bkt;
    SHR_FUNC_ENTER(u);

    elem = shr_lmm_alloc(DB_LMM_ELEMS(u, m, db));
    SHR_NULL_CHECK(elem, SHR_E_MEMORY);
    sal_memset(elem, 0, sizeof(alpm_elem_t));

    if (ln != DB_LAST_LEVEL(u, m, db) && !direct_route) {
        bkt = shr_lmm_alloc(DB_LMM_BKTS(u, m, db));
        SHR_NULL_CHECK(bkt, SHR_E_MEMORY);
        sal_memset(bkt, 0, sizeof(*bkt));
        mkl = bcmptm_rm_alpm_trie_mkl(u, m, key->t.ipv, key->t.app, key->vt);
        bcmptm_rm_alpm_trie_create(u, m, mkl, &bkt->trie);
        SHR_DQ_INIT(&bkt->list);
        elem->bkt = bkt;
    } else {
        elem->ad = shr_lmm_alloc(DB_LMM_ADATAS(u, m, db));
        SHR_NULL_CHECK(elem->ad, SHR_E_MEMORY);
        sal_memset(elem->ad, 0, sizeof(assoc_data_t));
    }

    elem->index = INVALID_INDEX;

    /* Pivot key */
    elem->key.t = key->t;
    elem->key.vt = key->vt;
    *elem_pyld = elem;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_elem_free(int u, int m, int db, int ln, bool direct_route, alpm_elem_t *elem)
{
    ALPM_ASSERT(elem);
    if (ln != DB_LAST_LEVEL(u, m, db) && !direct_route) {
        ALPM_ASSERT(elem->bkt && elem->bkt->trie && !elem->bkt->trie->trie);
        bcmptm_rm_alpm_trie_destroy(u, m, elem->bkt->trie);
        shr_lmm_free(DB_LMM_BKTS(u, m, db), elem->bkt);
        ALPM_ASSERT(elem->ad == NULL);
    } else {
        ALPM_ASSERT(elem->ad);
        shr_lmm_free(DB_LMM_ADATAS(u, m, db), elem->ad);
    }

    shr_lmm_free(DB_LMM_ELEMS(u, m, db), elem);

    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_delete(int u, int m, int db, int ln,
                          bool direct_route,
                          key_tuple_t *tp,
                          alpm_pivot_t **pivot)
{
    rm_alpm_trie_t *pivot_trie;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, tp->w_vrf, tp->ipv, &pivot_trie));
    SHR_NULL_CHECK(pivot_trie, SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_delete(u, m, pivot_trie,
                                   &tp->trie_ip.key[0],
                                   tp->len,
                                   &pivot_node));
    if (pivot) {
        *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
    }

    if (pivot_trie->trie == NULL) { /* all nodes deleted */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_destroy(u, m, db, ln, tp->ipv, tp->w_vrf));
        if (direct_route &&
            !VRF_HDL_IN_USE(u, m, db, tp->ipv, tp->w_vrf) &&
            VRF_HDL_IN_DQ(u, m, db, tp->ipv, tp->w_vrf)) {
            VRF_HDL_IN_DQ(u, m, db, tp->ipv, tp->w_vrf) = FALSE;
            SHR_DQ_REMOVE(&VRF_HDL_DQNODE(u, m, db, tp->ipv, tp->w_vrf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_lookup(int u, int m, int db, int ln,
                          key_tuple_t *tp,
                          alpm_pivot_t **pivot)
{
    int rv;
    rm_alpm_trie_t *pivot_trie;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, tp->w_vrf, tp->ipv, &pivot_trie));
    if (pivot_trie) {
        rv = bcmptm_rm_alpm_trie_search(pivot_trie,
                                        &tp->trie_ip.key[0],
                                        tp->len,
                                        &pivot_node);
        if (SHR_SUCCESS(rv)) {
            if (pivot) {
                *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
            }
        }
    } else {
        rv = SHR_E_NOT_FOUND;
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_find(int u, int m, int db, int ln,
                        key_tuple_t *tp,
                        alpm_pivot_t **pivot)
{
    int rv;
    rm_alpm_trie_t *pivot_trie;
    uint32_t lpm_pfx[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t lpm_len = 0;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, tp->w_vrf, tp->ipv, &pivot_trie));
    if (pivot_trie) {
        rv = bcmptm_rm_alpm_trie_find_lpm(pivot_trie,
                                          &tp->trie_ip.key[0],
                                          tp->len,
                                          &pivot_node,
                                          &lpm_pfx[0],
                                          &lpm_len);
        if (SHR_SUCCESS(rv)) {
            if (pivot) {
                *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
                /* ALPM_ASSERT(pivot->key.t.len == lpm_len); */
            }
        }
    } else {
        rv = SHR_E_NOT_FOUND;
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_insert(int u, int m, int db, int ln,
                          bool direct_route,
                          arg_key_t *key,
                          alpm_pivot_t *pivot)
{
    rm_alpm_trie_t *pivot_trie;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, key->t.w_vrf, key->t.ipv,
                                    &pivot_trie));
    if (pivot_trie == NULL) {
        if (direct_route &&
            !VRF_HDL_IN_USE(u, m, db, key->t.ipv, key->t.w_vrf) &&
            !VRF_HDL_IN_DQ(u, m, db, key->t.ipv, key->t.w_vrf)) {
            VRF_HDL_IN_DQ(u, m, db, key->t.ipv, key->t.w_vrf) = TRUE;
            VRF_HDL_VT(u, m, db, key->t.ipv, key->t.w_vrf) = key->vt;
            VRF_HDL_APP(u, m, db, key->t.ipv, key->t.w_vrf) = key->t.app;
            SHR_DQ_INSERT_HEAD(&DB_VRF_HDL_LIST(u, m, db, key->t.ipv),
                               &VRF_HDL_DQNODE(u, m, db, key->t.ipv, key->t.w_vrf));
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_create(u, m, db, ln,
                                           key->t.ipv,
                                           key->t.w_vrf,
                                           key->t.app,
                                           key->vt));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, key->t.w_vrf, key->t.ipv,
                                        &pivot_trie));
    }
    SHR_NULL_CHECK(pivot_trie, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_insert(u, m, pivot_trie,
                                   &key->t.trie_ip.key[0],
                                   NULL,
                                   key->t.len,
                                   &pivot->pvtnode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_trie_create(int u, int m, int db, int ln, int ipv,
                               int w_vrf, uint8_t app,
                               alpm_vrf_type_t vt)
{
    rm_alpm_trie_t *pivot_trie;
    uint32_t mkl = bcmptm_rm_alpm_trie_mkl(u, m, ipv, app, vt);
    bcmptm_rm_alpm_trie_create(u, m, mkl, &pivot_trie);
    bcmptm_rm_alpm_pvt_trie_set(u, m, db, ln, w_vrf, ipv, pivot_trie);
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_pvt_trie_destroy(int u, int m, int db, int ln, int ipv, int w_vrf)
{
    rm_alpm_trie_t *pivot_trie;
    (void) bcmptm_rm_alpm_pvt_trie_get(u, m, db, ln, w_vrf, ipv, &pivot_trie);
    if (pivot_trie) {
        bcmptm_rm_alpm_trie_destroy(u, m, pivot_trie);
        bcmptm_rm_alpm_pvt_trie_set(u, m, db, ln, w_vrf, ipv, NULL);
    }

    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_trie_get(int u, int m, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t **pivot_trie)
{
    if (pivot_trie) {
        *pivot_trie = VRF_HDL_PTRIE(u, m, db, ipv, w_vrf, ln);
    }
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_trie_set(int u, int m, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t *pivot_trie)
{
    VRF_HDL_PTRIE(u, m, db, ipv, w_vrf, ln) = pivot_trie;
    return SHR_E_NONE;
}

void
bcmptm_rm_alpm_vrf_route_inc(int u, int m, int db, int w_vrf, int ipv, int *cnt)
{
    *cnt = ++VRF_HDL_RCNT(u, m, db, ipv, w_vrf);
}

void
bcmptm_rm_alpm_vrf_route_dec(int u, int m, int db, int w_vrf, int ipv, int *cnt)
{
    *cnt = --VRF_HDL_RCNT(u, m, db, ipv, w_vrf);
}

int
bcmptm_rm_alpm_vrf_route_cnt(int u, int m, int db, int w_vrf, int ipv)
{
    int count;
    count = VRF_HDL_RCNT(u, m, db, ipv, w_vrf);

    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        int last_level = DB_LAST_LEVEL(u, m, db);
        rm_alpm_trie_t *ptrie;

        (void) bcmptm_rm_alpm_pvt_trie_get(u, m, db, last_level,
                                           w_vrf, ipv, &ptrie);
        if (ptrie) {
            ALPM_ASSERT(bcmptm_rm_alpm_trie_count(ptrie) == count);
        } else {
            ALPM_ASSERT(count == 0);
        }
    }
    return count;
}

alpm_db_t*
bcmptm_rm_alpm_database_get(int u, int m, int db)
{
    return &ALPM_DB(u, m, db);
}

int
bcmptm_rm_alpm_database_count(int u, int m)
{
    return ALPM_DB_CNT(u, m);
}

int
bcmptm_rm_alpm_database_init(int u, int m, alpm_config_t *config)
{
    return database_init(u, m, config);
}

void
bcmptm_rm_alpm_database_cleanup(int u, int m)
{
    database_cleanup(u, m);
}

void
bcmptm_rm_alpm_mid_dt_set(int u, int m, int db, alpm_data_type_t full)
{
    bcmptm_rm_alpm_mid_dt[u][m][db] = full;
}

alpm_data_type_t
bcmptm_rm_alpm_mid_dt_get(int u, int m, int db, int app)
{
    return (app == APP_COMP0) ? DATA_REDUCED: bcmptm_rm_alpm_mid_dt[u][m][db];
}

void
bcmptm_rm_alpm_internals_reset(int u, int m)
{
    static int i = 0;
    int db, ln;
    cli_out("%d:\n", i++);
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        for (db = 0; db < DBS; db++) {
            for (ln = 0; ln < LEVELS; ln++) {
                if (bcmptm_rm_alpm_internals[u][m][db][ln].read ||
                    bcmptm_rm_alpm_internals[u][m][db][ln].write) {
                    cli_out("\tDB=%d Level=%d RD=%-7d WR=%-7d SplitW1=%-6d SplitW2=%-6d Splits=%-5d\n",
                            db, (ln + 1),
                            bcmptm_rm_alpm_internals[u][m][db][ln].read,
                            bcmptm_rm_alpm_internals[u][m][db][ln].write,
                            bcmptm_rm_alpm_internals[u][m][db][ln].split_write1,
                            bcmptm_rm_alpm_internals[u][m][db][ln].split_write2,
                            bcmptm_rm_alpm_internals[u][m][db][ln].split
                            );
                }
            }
        }
    }

    sal_memset(bcmptm_rm_alpm_internals, 0, sizeof(bcmptm_rm_alpm_internals));
   /* bcmptm_rm_alpm_ln_stats_dump(u, m, NULL); */
}


