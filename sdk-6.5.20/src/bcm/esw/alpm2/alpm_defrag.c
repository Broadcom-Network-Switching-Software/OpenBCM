/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_defrag.c
 * Purpose: ALPM defrag for Tomahawk3.
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

/* #define DEFRAG_DEBUG    1
   #define DEFRAG_RESULT   1 */

#define TR_SEQ 0 /* trace seq */

#define MERGE_REQ_NORMAL    0 /* Normal merge: child to parent pivot */
#define MERGE_REQ_COVER_BKT 1 /* Cover parent (covered bkt by child pivot) to child pivot */
#define MERGE_REQ_NEW_BKT   2 /* Normal merge: child to parent pivot using a new bucket */

#define MIN_TIME_DEFRAG      50 /* Minimum defrag time in ms from API */
#define MAX_POLL_DEFRAG    5000 /* Defrag time poll count */
#define CHECK_INC_POLL        1 /* Traverse & check time_check inc_poll */
#define MERGE_INC_POLL      100 /* Merge time_check inc_poll */
#define MAX_MERGE_CNT     65535 /* Maximum allowable merge count */

#define ALPM_DEFRAG_EXIT(mergectr) \
    (((mergectr)->l3_merge_cnt >= (mergectr)->max_l3_merge_cnt &&  \
      (mergectr)->l2_merge_cnt >= (mergectr)->max_l2_merge_cnt) || \
     ((mergectr)->check_time && (mergectr)->check_time->timeout))

#define L2_DEFRAG_EXIT(mergectr) \
    (((mergectr)->l2_merge_cnt >= (mergectr)->max_l2_merge_cnt) || \
     ((mergectr)->check_time && (mergectr)->check_time->timeout))

#define L3_DEFRAG_EXIT(mergectr) \
    (((mergectr)->l3_merge_cnt >= (mergectr)->max_l3_merge_cnt) || \
     ((mergectr)->check_time && (mergectr)->check_time->timeout))

#define PRT_IS_PAYLOAD(prt_type) (prt_type == trieNodeTypePayload)
#define PRT_IS_INTERNAL(prt_type) (prt_type != trieNodeTypePayload)

/* Initialize ALPM defrag check time (max_time in ms) */
STATIC int
_alpm_check_time_init(int max_time, int max_poll,
                      _alpm_time_poll_t *check_time)
{
    if (check_time) {
        check_time->poll = 0;
        check_time->max_poll = max_poll;
        check_time->max_duration = max_time * 1000; /* in usec */
        check_time->start_time = sal_time_usecs();
        check_time->timeout = FALSE;
    }

    return BCM_E_NONE;
}

/* ALPM defrag check time
     inc_poll: weighted poll increment number (>=1).
     check_time: installed time info.
   Return BCM_E_TIMEOUT if current_time - start_time >= max_duation */
STATIC int
_alpm_check_time(int inc_poll, _alpm_time_poll_t *check_time)
{
    if (check_time) {
        if (check_time->timeout) {
            return BCM_E_TIMEOUT; /* already timeout */
        }

        check_time->poll += inc_poll;
        if (check_time->poll >= check_time->max_poll) {
            int duration = SAL_USECS_SUB(sal_time_usecs(),
                                         check_time->start_time);
            check_time->poll = 0; /* reset poll */

            if (duration >= check_time->max_duration) {
                check_time->timeout = TRUE;
                return BCM_E_TIMEOUT;
            }
        }
    }

    return BCM_E_NONE;
}

/* Return TRUE if bkt_pfx/bkt_key_len is covered by a pvt_pfx/pvt_key_len */
STATIC int
_alpm_bkt_key_cover_by_pvt(int u, int ipt,
                           uint32 *bkt_pfx,
                           uint32 bkt_key_len,
                           uint32 *pvt_pfx,
                           uint32 pvt_key_len)
{
    int i, key_ent_cnt;
    uint32 pvt_mask[4] = {0};
    uint32 bkt_key[5] = {0};
    uint32 pvt_key[5] = {0};

    /* No covered by pvt_node key if bkt_node key is shorter */
    if (bkt_key_len < pvt_key_len) {
        return FALSE;
    }

    /* Get pvt_node key and mask */
    alpm_trie_pfx_to_key(u, ipt, pvt_pfx, pvt_key_len, pvt_key);
    alpm_util_len_to_mask(ipt, pvt_key_len, pvt_mask);

    /* Get bkt_node key */
    alpm_trie_pfx_to_key(u, ipt, bkt_pfx, bkt_key_len, bkt_key);

    key_ent_cnt = ALPM_KEY_ENT_CNT(ipt);
    for (i = 0; i < key_ent_cnt; i++) {
        if ((bkt_key[i] & pvt_mask[i]) != (pvt_key[i] & pvt_mask[i])) {
            return FALSE; /* no covered if any part of mask doesn't match */
        }
    }

    return TRUE; /* bkt_node key covered by pvt_node key */
}

/* Transfer bkt_node in pfx_arr from src_bkt_trie to dst_bkt_trie */
STATIC int
_alpm_trie_transfer(int u,
                    alpm_lib_trie_t *dst_bkt_trie,
                    alpm_lib_trie_t *src_bkt_trie,
                    _alpm_bkt_pfx_arr_t *pfx_arr)
{
    int rv = BCM_E_NONE, rv1;
    int i, trf_i;
    alpm_lib_trie_node_t *tn = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;

    /* delete pfx_arr bkt_node from src_bkt_trie and re-insert to dst_bkt_trie */
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        bkt_node = pfx_arr->pfx[i];

        rv = alpm_lib_trie_delete(src_bkt_trie, bkt_node->key,
                                  bkt_node->key_len, &tn);
        ALPM_IEG(rv);

        bkt_node->node.count = 0;
        rv = alpm_lib_trie_insert(dst_bkt_trie, bkt_node->key,
                     bkt_node->key_len, (alpm_lib_trie_node_t *)bkt_node);
        ALPM_IEG(rv);
    }

    return rv;

bad:
    /* restore back if failed to transfer */
    trf_i = i;
    for (i = 0; i <= trf_i; i++) {
        bkt_node = pfx_arr->pfx[i];

        bkt_node->node.count = 0;
        rv1 = alpm_lib_trie_insert(src_bkt_trie, bkt_node->key,
                     bkt_node->key_len, (alpm_lib_trie_node_t *)bkt_node);
        ALPM_IER(rv1);

        rv1 = alpm_lib_trie_delete(dst_bkt_trie, bkt_node->key,
                                  bkt_node->key_len, &tn);
        ALPM_IER(rv1);
    }

    return rv;
}

/* callback function to collect bkt_node pfx_arr covered by new_pvt */
STATIC int
alpm_trie_pfx_merge_cb(alpm_lib_trie_node_t *node, void *user_data)
{
    _alpm_bkt_pfx_merge_arr_t *merge_data = (_alpm_bkt_pfx_merge_arr_t *)user_data;

    if (node->type == trieNodeTypePayload) {
        _alpm_bkt_pfx_arr_t *pfx_arr = merge_data->pfx_arr;
        _alpm_pvt_node_t    *new_pvt = merge_data->new_pvt;
        _alpm_bkt_node_t    *bkt_node = (_alpm_bkt_node_t *)node;
        int                 u = merge_data->unit;
        int                 ipt = PVT_BKT_IPT(new_pvt);

        /* only covered bkt_node by new_pvt is taking */
        if (_alpm_bkt_key_cover_by_pvt(u, ipt, bkt_node->key, bkt_node->key_len,
                                       new_pvt->key, new_pvt->key_len)) {
            pfx_arr->pfx[pfx_arr->pfx_cnt] = (_alpm_bkt_node_t *)node;
            pfx_arr->pfx_cnt++;
        }
    }
    return BCM_E_NONE;
}

/* callback function to check cover routes by new_pvt_key/new_pvt_key_len.
   return FULL if covered by one route (exist traverse) */
STATIC int
_alpm_bkt_cover_check_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                         alpm_lib_trie_traverse_state_t *state, void *user_data)
{
    _alpm_bkt_node_t *bkt_node = (_alpm_bkt_node_t *)trie;
    _alpm_bkt_merge_req_data_t *req_data = (_alpm_bkt_merge_req_data_t *)user_data;
    _alpm_pvt_node_t *pvt_node = req_data->pvt_node;
    _alpm_cb_t *acb = req_data->acb;
    int u = acb->unit;
    int ipt = PVT_BKT_IPT(pvt_node);
    int covered = FALSE;

    covered = _alpm_bkt_key_cover_by_pvt(u, ipt,
                                     bkt_node->key,
                                     bkt_node->key_len,
                                     req_data->new_pvt_key,
                                     req_data->new_pvt_key_len);
    if (covered) {
        return BCM_E_FULL; /* exit traverse */
    }
    return BCM_E_NONE;
}

/* callback function to obtain fmt req_ent when merge
   to a new_pvt_key/new_pvt_key_len.
   Note: if new_pvt_key is NULL, bkt_node is always covered by new_pvt_key */
STATIC int
_alpm_bkt_fmt_req_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                     alpm_lib_trie_traverse_state_t *state, void *user_data)
{
    _alpm_bkt_node_t *bkt_node = (_alpm_bkt_node_t *)trie;
    _alpm_bkt_merge_req_data_t *req_data = (_alpm_bkt_merge_req_data_t *)user_data;
    _alpm_cb_t *acb = req_data->acb;
    _alpm_pvt_node_t *pvt_node = req_data->pvt_node;
    int *req_ent = req_data->req_ent;
    int pfx_len;
    int vrf_id = pvt_node->vrf_id;
    int fmt;

    if (req_data->new_pvt_key != NULL) { /* check bkt covered by a new pivot key */
        int u = acb->unit;
        int ipt = PVT_BKT_IPT(pvt_node);
        int covered = FALSE;

        /* In 3 levels ALPM, if L2 bkt_node->key_len is shorter than new pvt,
           we need to check its L3 routes. One covered L3 route implies
           the internal merge is no mergeable (leave it for payload merge) */
        if (bkt_node->key_len < req_data->new_pvt_key_len &&
            ACB_HAS_PVT(acb, vrf_id)) {
            int rv;
            _alpm_pvt_node_t *pvt_node = NULL;
            alpm_lib_trie_t *pvt_trie, *bkt_trie;

            acb = ACB_DWN(u, acb);
            pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
            if (!pvt_trie) {
                return BCM_E_NOT_FOUND;
            }
            /* Search corresponding next level pvt_node */
            rv = alpm_lib_trie_search(pvt_trie, bkt_node->key, bkt_node->key_len,
                                      (alpm_lib_trie_node_t **)&pvt_node);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            bkt_trie = PVT_BKT_TRIE(pvt_node);
            if (!bkt_trie) {
                return BCM_E_NOT_FOUND;
            }
            /* traverse2 for next level route cover check
               (return FULL and exit traverse if one route covered) */
            rv = alpm_lib_trie_traverse2(bkt_trie, _alpm_bkt_cover_check_cb,
                                         req_data, trieTraverseOrderPost);
            /* If FULL, no mergeable and exit traverse.
               Otherwise return BCM_E_NONE as no covered case
               and continue traversing */
            return rv;

        } else {
            covered = _alpm_bkt_key_cover_by_pvt(u, ipt,
                                             bkt_node->key,
                                             bkt_node->key_len,
                                             req_data->new_pvt_key,
                                             req_data->new_pvt_key_len);

            if (covered == FALSE) {
                return BCM_E_NONE;
            }
            (req_data->cover_cnt)++;
        }
    }

    pfx_len = bkt_node->key_len - req_data->new_pvt_key_len;

    for (fmt = req_data->def_fmt; fmt <= ACB_FMT_CNT(acb, vrf_id); fmt++) {
        if (ACB_FMT_BNK(acb, vrf_id, fmt) == INVALIDfmt) {
            continue;
        }
        if (pfx_len <= ACB_FMT_PFX_LEN(acb, vrf_id, fmt)) {
            req_ent[fmt]++;
            break;
        }
    }

    assert(fmt <= ACB_FMT_CNT(acb, vrf_id)); /* at least bkt fits one fmt */

    return BCM_E_NONE;
}

/* Get a parent pivot fmt available and request (optional) ent arrays.
   avl_free_bnks: available continuous free bnks (bkt expansion in unused bnks).
   req_bnks: current needed banks.
*/
STATIC int
_alpm_cb_pvt_fmt_ent_avl_get(int u, _alpm_cb_t *acb,
                             _alpm_pvt_node_t *pvt_node,
                             int *req_ent,
                             int *avl_ent,
                             int *avl_free_bnks,
                             int *req_bnks)
{
    int rv = BCM_E_NONE;
    int ipt, vrf_id;
    int i, sb, bnk, bnk_pbk, fmt;
    int continuous_bnk = TRUE;
    SHR_BITDCL *bnk_bmp;
    _alpm_bkt_info_t *bkt_info = &PVT_BKT_INFO(pvt_node);

    ipt    = PVT_BKT_IPT(pvt_node);
    vrf_id = PVT_BKT_VRF(pvt_node);
    bnk_pbk = ACB_BNK_PER_BKT(acb, vrf_id);
    bnk_bmp = ACB_BNK_BMP(acb, vrf_id, ipt);
    sb = PVT_BKT_IDX(pvt_node) * bnk_pbk + PVT_ROFS(pvt_node);

    /* scan bucket bnk fmt */
    for (i = sb; i < sb + bnk_pbk && i < ACB_BNK_CNT(acb, vrf_id); i++) {
        bnk = i % bnk_pbk;
        fmt = bkt_info->bnk_fmt[bnk];
        if ((fmt < ALPM_FMT_TP_CNT) && /* existing valid bnk fmt */
            (ACB_FMT_BNK(acb, vrf_id, fmt) != INVALIDfmt)) {
            int bnk_ent_cnt = _shr_popcount(bkt_info->vet_bmp[bnk]);
            if (req_ent) {
                req_ent[fmt] += bnk_ent_cnt;
            }
            avl_ent[fmt] += ACB_FMT_ENT_MAX(acb, vrf_id, fmt) - bnk_ent_cnt;
            (*req_bnks)++;
        } else { /* check for continuous free_bnks */
            if (continuous_bnk == TRUE) {
                if (!SHR_BITGET(bnk_bmp, i)) {
                    (*avl_free_bnks)++;
                } else {
                    continuous_bnk = FALSE; /* one bnk used by other pivot */
                }
            }
        }
    }

    return rv;
}

/* Get a pivot (could be parent or child) fmt request for
   merge to new_pvt_key/new_pvt_key_len.
   Return FULL if no mergeable detected.
   Note: input new_pvt_key set to NULL for a child pivot
         (bkt always covered by pivot) */
STATIC int
_alpm_cb_pvt_fmt_ent_req_get(int u, _alpm_cb_t *acb,
                             uint32 *new_pvt_key,
                             uint32 new_pvt_key_len,
                             _alpm_pvt_node_t *pvt_node,
                             int *req_ent,
                             int *cover_cnt)
{
    int rv = BCM_E_NONE;
    int ipt, vrf_id;
    uint8 def_fmt;
    _alpm_bkt_merge_req_data_t req_data;

    ipt    = PVT_BKT_IPT(pvt_node);
    vrf_id = PVT_BKT_VRF(pvt_node);
    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);

    /* Child pivot request fmt can't obtain directly from chd bkt_info. Child
       merged to parent new pfx_len = chd_bkt_node->key_len - new_pvt_key_len.
       Need traverse chd_pvt bkt_trie to obtain req_ent per fmt */
    if (PVT_BKT_TRIE(pvt_node)) {
        req_data.acb             = acb;
        req_data.cover_cnt       = 0;
        req_data.new_pvt_key     = new_pvt_key;
        req_data.new_pvt_key_len = new_pvt_key_len;
        req_data.pvt_node        = pvt_node;
        req_data.req_ent         = req_ent;
        req_data.def_fmt         = def_fmt;

        /* traverse2 for next level cover check (return FULL if no mergeable) */
        rv = alpm_lib_trie_traverse2(PVT_BKT_TRIE(pvt_node),
                                     _alpm_bkt_fmt_req_cb,
                                     &req_data, trieTraverseOrderPost);
        if (cover_cnt != NULL) {
            *cover_cnt = req_data.cover_cnt; /* report how many bkt pfx
                                                cnt was covered by new pvt */
        }
    }

    return rv;
}

/* Check for bucket merge if fmt request entry array can fit to
   fmt available entry array in used banks and available free banks.

   This should match to the way we get ent_idx in alpm_bkt_ent_get_attempt.

   Return BCM_E_NONE if request fit available, else BCM_E_FULL.
   Notes:
     - avl_ent & req_ent will be updated during check.
     - req_bnks is the current needed banks (accumulated if input not cleared).
     - arrays avl_ent & req_ent will be updated. */
STATIC int
_alpm_cb_fmt_ent_req_fit_avl(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                            int *avl_ent, int *req_ent,
                            int avl_free_bnks, int *req_bnks)
{
    int rv = BCM_E_NONE;
    int i, fmt;
    uint8 def_fmt;

    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);

    /* Scan all fmt request ent (from largest to smallest fmt) */
    for (fmt = ACB_FMT_CNT(acb, vrf_id); fmt >= def_fmt; fmt--) {
        if (req_ent[fmt]) {
            /* Scan fmt available ent (from fmt to largest) */
            for (i = fmt; i <= ACB_FMT_CNT(acb, vrf_id); i++) {
                if (avl_ent[i]) {
                    if (avl_ent[i] >= req_ent[fmt]) { /* req fits avl */
                        avl_ent[i] -= req_ent[fmt];
                        req_ent[fmt] = 0;
                        break; /* done for this fmt req and exit from this loop */
                    } else { /* existing avl_ent not enough */
                        req_ent[fmt] -= avl_ent[i];
                        avl_ent[i] = 0;
                    }
                }

                /* During merge all avl_free_bnks are initialized with def_fmt and
                   can be assigned in the first round of alpm_bkt_ent_get_attempt */
                if (req_ent[fmt] && fmt == def_fmt) {
                    if (avl_free_bnks) {
                        int fmt_entpb, add_bnks;

                        fmt_entpb = ACB_FMT_ENT_MAX(acb, vrf_id, fmt);
                        add_bnks = (req_ent[fmt] + fmt_entpb - 1) / fmt_entpb;
                        /* needed bnks can't exceeds current avl_free_bnks */
                        if (add_bnks > avl_free_bnks) {
                            add_bnks = avl_free_bnks;
                        }

                        avl_ent[fmt] += add_bnks * fmt_entpb; /* update new avl_ent */
                        avl_free_bnks -= add_bnks;
                        (*req_bnks) += add_bnks;

                        if (avl_ent[fmt] >= req_ent[fmt]) { /* remaining req fits */
                            avl_ent[fmt] -= req_ent[fmt];
                            req_ent[fmt] = 0;
                            break; /* done for this fmt req and exit from this loop */
                        } else { /* new updated avl_ent not enough */
                            req_ent[fmt] -= avl_ent[fmt];
                            avl_ent[fmt] = 0;
                        }
                    }
                }
            }

            /* Then try to use avl_free_bnks for fmt remaining req */
            if (req_ent[fmt]) {
                if (avl_free_bnks) {
                    int fmt_entpb, add_bnks;

                    fmt_entpb = ACB_FMT_ENT_MAX(acb, vrf_id, fmt);
                    add_bnks = (req_ent[fmt] + fmt_entpb - 1) / fmt_entpb;
                    /* needed bnks can't exceeds current avl_free_bnks */
                    if (add_bnks > avl_free_bnks) {
                        add_bnks = avl_free_bnks;
                    }

                    avl_ent[fmt] += add_bnks * fmt_entpb; /* update new avl_ent */
                    avl_free_bnks -= add_bnks;
                    (*req_bnks) += add_bnks;

                    if (avl_ent[fmt] >= req_ent[fmt]) { /* remaining req fits */
                        avl_ent[fmt] -= req_ent[fmt];
                        req_ent[fmt] = 0;
                        continue; /* done for this fmt req and continue */
                    } else { /* new updated avl_ent not enough */
                        req_ent[fmt] -= avl_ent[fmt];
                        avl_ent[fmt] = 0;
                    }
                }
            }

            /* After scan all avl_ent or avl_free_bnks */
            if (req_ent[fmt]) {
                break; /* failed for fmt req */
            }
        }
    }

    if (fmt >= def_fmt) {
        rv = BCM_E_FULL; /* failed */
    }

    return rv;
}

/* Check mergeability for:
    - prt_type = PAYLOAD: P1 + P2 => P2 (payload)
        check chd_pvt + prt_pvt => prt_pvt
    - prt_type = INTERNAL: P1 + P2 + some PP covered by P3 => P3 (internal)
        check chd_pvt + lc_pvt + some of prt_pvt covered by new_pvt => new_pvt.
   Return BCM_E_NONE if mergeable, otherwise return BCM_E_FULL.
   Notes:
     - prt_pvt: immediate payload parent pivot
     - prt_key/prt_key_len: parent pivot key and length
     - bkt_info: returns new bkt banks info allocated.
     - merge_req: return three different MERGE_REQ_xxx */
STATIC int
alpm_cb_pvt_mergeable(int u, _alpm_cb_t *acb,
                      int prt_type,
                      uint32 *prt_key,
                      uint32 prt_key_len,
                      _alpm_pvt_node_t *prt_pvt,
                      _alpm_pvt_node_t *chd_pvt,
                      _alpm_pvt_node_t *lc_pvt,
                      _alpm_bkt_info_t *bkt_info,
                      int *merge_req)
{
    int rv;
    int vrf_id = chd_pvt->vrf_id;
    int ipt = PVT_BKT_IPT(chd_pvt);
    int req_bnks = 0;
    int avl_free_bnks = 0;
    int avl_ent[ALPM_FMT_TP_CNT]; /* available entry cnt per fmt */
    int req_ent[ALPM_FMT_TP_CNT]; /* request entry cnt per fmt */

    sal_memset(avl_ent, 0, sizeof(avl_ent));
    sal_memset(req_ent, 0, sizeof(req_ent));

    *merge_req = MERGE_REQ_NORMAL;

    if (PRT_IS_PAYLOAD(prt_type)) { /* P1 + P2 => P2 (payload) */

        /* 1) Get chd_pvt req */
        rv = _alpm_cb_pvt_fmt_ent_req_get(u, acb, NULL, prt_key_len,
                                           chd_pvt, req_ent, NULL);

        /* 2) First try with existing prt_pvt avl bucket (with expansion) */
        (void)_alpm_cb_pvt_fmt_ent_avl_get(u, acb, prt_pvt, NULL, avl_ent,
                                           &avl_free_bnks, &req_bnks);
        rv = _alpm_cb_fmt_ent_req_fit_avl(u, acb, vrf_id, ipt, avl_ent, req_ent,
                                          avl_free_bnks, &req_bnks);
        if (BCM_SUCCESS(rv)) {
            return rv; /* use original bkt */
        }

        /* 3) Now try with a full new bucket (by copying current prt_bkt) */
        /* if current req_bnks is already a full bucket banks */
        if (req_bnks >= ACB_BNK_PER_BKT(acb, vrf_id)) {
            return rv; /* FULL */
        }
        /* check with additional avl_free_bnks */
        avl_free_bnks = ACB_BNK_PER_BKT(acb, vrf_id) - req_bnks;
        *merge_req = MERGE_REQ_NEW_BKT;

    } else { /* P1 + P2 + P4 (covered by P3) => P3 (internal) */

        /* Start with a full bkt banks */
        avl_free_bnks = ACB_BNK_PER_BKT(acb, vrf_id);

        /* 1) Get prt_pvt req (bkt covered by new prt_key/prt_key_len) */
        if (prt_pvt && PVT_BKT_TRIE(prt_pvt) && PVT_BKT_TRIE(prt_pvt)->trie) {
            int cover_cnt = 0;

            rv = _alpm_cb_pvt_fmt_ent_req_get(u, acb, prt_key, prt_key_len,
                                               prt_pvt, req_ent, &cover_cnt);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* gprt_pvt bucket routes are all covered by new pvt (internal),
               instead of delete gprt_pvt, leave to payload merge */
            if (PVT_BKT_TRIE(prt_pvt)->trie->count == cover_cnt) {
                rv = BCM_E_FULL;
                return rv;
            }

            if (cover_cnt) {
                rv = _alpm_cb_fmt_ent_req_fit_avl(u, acb, vrf_id, ipt, avl_ent,
                                                  req_ent, avl_free_bnks, &req_bnks);
                if (BCM_FAILURE(rv)) {
                    if (rv == BCM_E_FULL) {
                        return BCM_E_INTERNAL; /* unexpected FULL (for first bkt) */
                    }
                    return rv;
                }
                /* check with remaining avl_free_bnks */
                avl_free_bnks = ACB_BNK_PER_BKT(acb, vrf_id) - req_bnks;
                *merge_req = MERGE_REQ_COVER_BKT;
            }
        }

        /* 2) Get chd_pvt req */
        rv = _alpm_cb_pvt_fmt_ent_req_get(u, acb, NULL, prt_key_len,
                                           chd_pvt, req_ent, NULL);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        rv = _alpm_cb_fmt_ent_req_fit_avl(u, acb, vrf_id, ipt, avl_ent, req_ent,
                                          avl_free_bnks, &req_bnks);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* check with remaining avl_free_bnks */
        avl_free_bnks = ACB_BNK_PER_BKT(acb, vrf_id) - req_bnks;

        /* 3) Finally add lc_pvt req */
        rv = _alpm_cb_pvt_fmt_ent_req_get(u, acb, NULL, prt_key_len,
                                           lc_pvt, req_ent, NULL);
    }

    rv = _alpm_cb_fmt_ent_req_fit_avl(u, acb, vrf_id, ipt, avl_ent, req_ent,
                                      avl_free_bnks, &req_bnks);
    /* check with bucket availability for req_bnks */
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bnk_alloc(u, acb, vrf_id, ipt, req_bnks, bkt_info, NULL);
    }

    return rv;
}

STATIC int
alpm_l2_merge_bkt_insert(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
                         _alpm_pvt_node_t *l1_pvt_node)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id, ipt;
    uint32              *prefix, length;
    uint32              ent_idx;
    _alpm_bkt_node_t    *bkt_node = NULL;

    ipt    = PVT_BKT_IPT(l1_pvt_node);
    vrf_id = PVT_BKT_VRF(l1_pvt_node);

    /* entry shouldn't exist, insert the entry into the RAM */
    rv = alpm_bkt_insert(u, acb, lpm_cfg, l1_pvt_node, &ent_idx);

    /* return FULL when Expand exceeds bnk_threshold */
    if (rv == BCM_E_RESOURCE) {
        rv = BCM_E_FULL;
    }

    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

    lpm_cfg->defip_index = ent_idx;

    /* Allocate Payload */
    ALPM_ALLOC_EG(bkt_node, sizeof(_alpm_bkt_node_t), "bkt_node");
    prefix = lpm_cfg->user_data;
    length = lpm_cfg->defip_sub_len;
    sal_memcpy(bkt_node->key, prefix, sizeof(bkt_node->key));
    bkt_node->key_len   = length;
    bkt_node->ent_idx   = ent_idx;
    alpm_util_adata_cfg_to_trie(u, lpm_cfg, &bkt_node->adata);
    /* Insert the prefix */
    /* bucket trie insert */
    rv = alpm_lib_trie_insert(PVT_BKT_TRIE(l1_pvt_node), prefix,
                              length, (alpm_lib_trie_node_t *)bkt_node);
    ALPM_IEG(rv);

    VRF_ROUTE_ADD(acb, vrf_id, ipt);
    return rv;

bad:
    if (bkt_node != NULL) {
        alpm_util_free(bkt_node);
    }
    return rv;
}

/* Insert a new parent pivot node (prt_pvt: prt_key, prt_key_len & bkt_info)
   If merge_req = MERGE_REQ_COVER_BKT:
     Merge P1 + P2 + cover bkt P4 => P3 (internal).
     Covered bkt from gprt_pvt (gran parent P4) will be
     copied as initial P3->B3 bkt.
   Otherwise:
     Merge P1 + P2 => P3 (internal).
     New empty bkt will be created (P3->B3).
     gprt_pvt in this case is actually chd_pvt used as reference
     for same prt_pvt ipt and vrf_id info.
   - new_prt_pvt is returned.
   - l1_pvt_node is needed for l2 bkt insert */
STATIC int
alpm_cb_new_pvt_insert(int u, _alpm_cb_t *acb,
                       uint32 *prt_key,
                       uint32 prt_key_len,
                       _alpm_bkt_info_t *bkt_info,
                       _alpm_pvt_node_t **new_prt_pvt,
                       _alpm_pvt_node_t *gprt_pvt,
                       _alpm_pvt_node_t *l1_pvt_node,
                       int merge_req)
{
    int rv = BCM_E_NONE;
    int ipt, vrf_id, max_key_len;
    _bcm_defip_cfg_t lpm_cfg;
    _bcm_defip_cfg_t *pvt_lpm_cfg;
    _alpm_pvt_node_t *prt_pvt = NULL; /* new prt_pvt */
    alpm_lib_trie_t  *prt_bkt_trie = NULL;
    alpm_lib_trie_t  *pfx_trie = NULL;
    _alpm_pfx_node_t *pfx_node = NULL;
    _alpm_bkt_info_t *cur_bkt_info = bkt_info; /* save for revert */
    int step_trie_merged = 0;
    int shrink_empty_only = 1; /* treat as route delete from gprt_pvt */
    uint32 *bak_idx = NULL;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_pfx_merge_arr_t merge_data;

    ipt    = PVT_BKT_IPT(gprt_pvt);
    vrf_id = PVT_BKT_VRF(gprt_pvt);
    max_key_len = alpm_util_trie_max_key_len(u, ipt);

    /* 1) Init prt_pvt bkt_trie */
    ALPM_IEG(alpm_lib_trie_init(max_key_len, &prt_bkt_trie));

    /* 2) Create prt_pvt node */
    ALPM_ALLOC_EG(prt_pvt, sizeof(_alpm_pvt_node_t), "pvt_node");
    PVT_BKT_TRIE(prt_pvt)  = prt_bkt_trie;
    PVT_BKT_VRF(prt_pvt)   = vrf_id;
    PVT_BKT_PKM(prt_pvt)   = PVT_BKT_PKM(gprt_pvt);
    PVT_KEY_CPY(prt_pvt, prt_key);
    PVT_KEY_LEN(prt_pvt)   = prt_key_len;
    PVT_FLAGS(prt_pvt)     = PVT_FLAGS(gprt_pvt);

    /* Copy bkt_info */
    sal_memcpy(&PVT_BKT_INFO(prt_pvt), bkt_info, sizeof(_alpm_bkt_info_t));

    /* Assoc-data and bpm_len info comes from alpm_lib_trie_find_lpm */
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
    ALPM_IEG(alpm_lib_trie_find_lpm(pfx_trie, prt_key, prt_key_len,
                              (alpm_lib_trie_node_t **)&pfx_node));
    PVT_BPM_LEN(prt_pvt) = pfx_node->key_len;
    PVT_BKT_DEF(prt_pvt) = pfx_node->bkt_ptr;

    /* 3) Copy covered pfx from gprt_pvt to prt_pvt */
    if (merge_req == MERGE_REQ_COVER_BKT) {
        ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");

        merge_data.unit = u;
        merge_data.new_pvt = prt_pvt;
        merge_data.pfx_arr = pfx_arr;

        rv = alpm_lib_trie_traverse(PVT_BKT_TRIE(gprt_pvt), alpm_trie_pfx_merge_cb,
                                    (void *)&merge_data, trieTraverseOrderIn, TRUE);

#ifdef DEFRAG_DEBUG
        cli_out(">>> Transfer cover bkt from gran parent to new parent pvt"
                "(pfx_cnt=%d/%d)!\n",
                pfx_arr->pfx_cnt, PVT_BKT_TRIE(gprt_pvt)->trie->count);
#endif
        ALPM_IEG(rv);

        /* for !FIXED_FMT:
         * Need to sort pfx_arr here, node with longer prefix length comes first,
         * Should let longer bkt node to choose bkt_ent_get first
         */
        if (!ACB_BKT_FIXED_FMT(acb, 1)) {
            _shr_sort(pfx_arr->pfx, pfx_arr->pfx_cnt, sizeof(_alpm_bkt_node_t *),
                      alpm_util_pfx_len_cmp);
        }

        rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
        ALPM_IEG(rv);
        rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, gprt_pvt, prt_pvt);
        ALPM_IEG(rv);

        /* Transfer covered pfx tries from gprt_pvt to new prt_pvt bkt_trie */
        rv = _alpm_trie_transfer(u, PVT_BKT_TRIE(prt_pvt),
                                 PVT_BKT_TRIE(gprt_pvt), pfx_arr);
        ALPM_IEG(rv);
        step_trie_merged = 1;
    }

    /* 4) Create pvt_lpm_cfg */
    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
    pvt_lpm_cfg = &lpm_cfg;

    pvt_lpm_cfg->defip_flags |= (ipt ? BCM_L3_IP6 : 0);
    pvt_lpm_cfg->defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);

    /* Key & Length */
    alpm_trie_pfx_to_cfg(u, prt_key, prt_key_len, pvt_lpm_cfg);
    sal_memcpy(pvt_lpm_cfg->user_data, prt_key, sizeof(uint32) * 5);

    /* Construct Assoc-data */
    if (pfx_node->bkt_ptr != NULL) {
        alpm_util_adata_trie_to_cfg(u, &pfx_node->bkt_ptr->adata, pvt_lpm_cfg);
        pvt_lpm_cfg->default_miss = 0;
    } else {
        alpm_util_adata_zero_cfg(u, pvt_lpm_cfg);
        pvt_lpm_cfg->default_miss = 1;
    }

    if (ACB_HAS_TCAM(acb)) {
        pvt_lpm_cfg->pvt_node = (uint32 *)prt_pvt;
    } else {
        pvt_lpm_cfg->bkt_kshift = prt_key_len; /* for upr acb bkt */
    }

    /* Bank shrink before insert pivot (disable unused banks to avoid
       traffic miss) */
    cur_bkt_info = &PVT_BKT_INFO(prt_pvt);
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        cur_bkt_info, NULL);

    /* Restore original rofs & bkt_idx after alpm_bkt_bnk_shrink if all
       reserved bnks are freed (rofs and bkt_idx maybe changed) */
    if (merge_req != MERGE_REQ_COVER_BKT) {
        cur_bkt_info->rofs = bkt_info->rofs;
        cur_bkt_info->bkt_idx = bkt_info->bkt_idx;
    }
    pvt_lpm_cfg->bkt_info = cur_bkt_info;

    /* 5) Insert pvt_lpm_cfg */
    if (ACB_HAS_TCAM(acb)) {
        rv = bcm_esw_alpm_tcam_insert(u, pvt_lpm_cfg, ALPM_PVT_UPDATE_ALL);
    } else {
        rv = alpm_l2_merge_bkt_insert(u, ACB_UPR(u, acb),
                                      pvt_lpm_cfg, l1_pvt_node);
    }

    ALPM_INFO(("**ACB(%d).PVT.ADD rv %d\n", ACB_IDX(acb), rv));

    if (BCM_SUCCESS(rv)) {
        VRF_PVT_LEN_CNT_ADD(acb, vrf_id, ipt, pvt_lpm_cfg->defip_sub_len);

        /* Insert now new prt_pvt node into pvt_trie */
        PVT_IDX(prt_pvt) = pvt_lpm_cfg->defip_index;
        rv = alpm_lib_trie_insert(ACB_PVT_TRIE(acb, vrf_id, ipt), prt_key,
                                  prt_key_len, (alpm_lib_trie_node_t *)prt_pvt);
        *new_prt_pvt = prt_pvt;
    }
    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

    /* 6) Invalid old entries in gprt_pvt.
          Not expecting failures on invalidation */
    if (merge_req == MERGE_REQ_COVER_BKT) {
        rv = alpm_bkt_pfx_clean(u, acb, gprt_pvt, pfx_arr->pfx_cnt, bak_idx);
        ALPM_IEG(rv);

        /* 7) SW: Shrink old bucket,
         * A important step to make sure banks are continuously occupied by pivot.
         * Note we use shrink_empty_only=1 to avoid allocating new bucket in
         * alpm_bkt_pfx_shrink, which may steal pre-reserved banks (but already
         * disabled to avoid traffic miss) for upcoming merges from P1 and P2 to P3.
         */
        rv = alpm_bkt_pfx_shrink(u, acb, gprt_pvt, &shrink_empty_only);
        ALPM_IEG(rv);

        alpm_util_free(bak_idx);
        alpm_util_free(pfx_arr);

        if ((PVT_KEY_LEN(gprt_pvt) != 0 &&
            PVT_BKT_TRIE(gprt_pvt)->trie == NULL)) {
            alpm_lib_trie_node_t *tn = NULL;
            rv = alpm_pvt_delete_by_pvt_node(u, acb, gprt_pvt);
            if (BCM_FAILURE(rv)) {
                ALPM_ERR(("alpm_pvt_delete_by_pvt_node(trie==NULL) failure\n"));
            }
            rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), gprt_pvt->key,
                             PVT_KEY_LEN(gprt_pvt), &tn);
            if (BCM_FAILURE(rv)) {
                ALPM_ERR(("Trie delete node failure\n"));
            }
            (void)alpm_lib_trie_destroy(PVT_BKT_TRIE(gprt_pvt));
            alpm_util_free(tn);
        }
    }

    return rv;

bad:
    if (step_trie_merged) {
        /* transfer back */
        _alpm_trie_transfer(u, PVT_BKT_TRIE(gprt_pvt),
                            PVT_BKT_TRIE(prt_pvt), pfx_arr);
    }

    /* Clean new bucket */
    if (bak_idx != NULL) {
        alpm_bkt_pfx_idx_restore(u, pfx_arr, bak_idx);
        (void)alpm_bkt_pfx_clean(u, acb, prt_pvt, pfx_arr->pfx_cnt, bak_idx);
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    /* Free already allocated new prt_pvt cur_bkt_info */
    alpm_bkt_free(u, acb, vrf_id, ipt, cur_bkt_info);

    if (prt_bkt_trie != NULL) {
        alpm_lib_trie_destroy(prt_bkt_trie);
    }

    if (prt_pvt != NULL) {
        alpm_util_free(prt_pvt);
    }

    return rv;
}

/* Merge from chd_pvt to prt_pvt (payload): P1 + P2 => P2
   if bkt_info != NULL: use new bkt.
   else use current bkt (expansion). */
STATIC int
alpm_cb_merge_payload_pvt(int u, _alpm_cb_t *acb,
                          _alpm_pvt_node_t *chd_pvt,
                          _alpm_pvt_node_t *prt_pvt,
                          _alpm_bkt_info_t *bkt_info)
{
    int rv = BCM_E_NONE;
    int ipt, vrf_id;
    uint32 *bak_idx = NULL;
    int use_new_bkt;

    int step_trie_merged = 0;
    uint8 prt_reverse;
    alpm_lib_trie_node_t *tn = NULL;
    _alpm_pvt_node_t *tmp_pvt;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_info_t *src_bkt_info;

    ipt    = PVT_BKT_IPT(chd_pvt);
    vrf_id = PVT_BKT_VRF(chd_pvt);

    use_new_bkt = bkt_info != NULL ? TRUE : FALSE;
    src_bkt_info = &PVT_BKT_INFO(prt_pvt);

    if (use_new_bkt) {
        int bnk_cnt;

        /* 1. Move banks from prt_pvt original src_bkt_info to new bkt_info */
        alpm_bkt_bnks_get(u, vrf_id, acb, src_bkt_info, &bnk_cnt);
        if (bnk_cnt > 0) {
            int bnkpb = BPC_BNK_PER_BKT(ACB_BKT_VRF_POOL(acb, vrf_id));
            int def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
            int i, fbnks = 0;
            /* Assuming bkt_info has contiguous used banks from ROFS */
            for (i = bkt_info->rofs + bnk_cnt; i < bkt_info->rofs + bnkpb; i++) {
                if (!BI_BNK_IS_USED(bkt_info, i % bnkpb)) {
                    break;
                }

                bkt_info->bnk_fmt[i % bnkpb] = 0; /* force disable before move */
                fbnks++;
            }
            rv = alpm_bkt_move(u, acb, prt_pvt, src_bkt_info, bkt_info);
            ALPM_IEG(rv);
            /* restore forced disabled bnks (now in src_bkt_info) */
            for (i = src_bkt_info->rofs + bnk_cnt; i < src_bkt_info->rofs + bnk_cnt + fbnks; i++) {
                src_bkt_info->bnk_fmt[i % bnkpb] = def_fmt;
            }
        } else {
            /* Swap bkt_info only */
            sal_memcpy(src_bkt_info, bkt_info, sizeof(_alpm_bkt_info_t));
        }
    } else { /* original bkt expansion */
        int i, sb, bnk, bnk_pbk, fmt;
        int continuous_bnk = TRUE;
        _alpm_bkt_pool_conf_t *bp_conf;
        SHR_BITDCL *bnk_bmp;

        bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
        bnk_pbk = BPC_BNK_PER_BKT(bp_conf);
        bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);

        /* parent start bkt bnk global index */
        sb = PVT_BKT_IDX(prt_pvt) * bnk_pbk + PVT_ROFS(prt_pvt);

        /* 1. Expand the bkt of dst_pvt from rofs (continuous bnks) */
        for (i = sb; i < sb + bnk_pbk && i < ACB_BNK_CNT(acb, vrf_id); i++) {
            bnk = i % bnk_pbk;
            fmt = src_bkt_info->bnk_fmt[bnk];
            if ((fmt >= ALPM_FMT_TP_CNT) || /* invalid bnk fmt */
                (ACB_FMT_BNK(acb, vrf_id, fmt) == INVALIDfmt)) {

                if (continuous_bnk == TRUE) {
                    if (!SHR_BITGET(bnk_bmp, i)) {
                        src_bkt_info->bnk_fmt[bnk] =
                            alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);

                        SHR_BITSET(bnk_bmp, i);
                        BPC_BNK_USED(bp_conf, ipt) += 1;
                    } else {
                        continuous_bnk = FALSE; /* one bnk used by other pivot */
                    }
                }
            }
        }
    }

    /* 2. Copy pfx from chd_pvt to prt_pvt */
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = alpm_lib_trie_traverse(PVT_BKT_TRIE(chd_pvt), alpm_trie_pfx_cb, pfx_arr,
                       trieTraverseOrderIn, TRUE);
    ALPM_IEG(rv);

    /* for !FIXED_FMT:
     * Need to sort pfx_arr here, node with longer prefix length comes first,
     * Should let longer bkt node to choose bkt_ent_get first
     */
    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
        _shr_sort(pfx_arr->pfx, pfx_arr->pfx_cnt, sizeof(_alpm_bkt_node_t *),
                  alpm_util_pfx_len_cmp);
    }

    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);

    /* Merge shouldn't use reverse allocation during bkt_pfx_copy.
       Otherwise continuous bank can't be guaranteed */
    prt_reverse = prt_pvt->bkt_info.reverse;
    prt_pvt->bkt_info.reverse = 0;
    rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, chd_pvt, prt_pvt);
    prt_pvt->bkt_info.reverse = prt_reverse;
    ALPM_IEG(rv);

    /* 3. Merge trie */
    rv = alpm_lib_trie_merge(PVT_BKT_TRIE(prt_pvt), PVT_BKT_TRIE(chd_pvt)->trie,
                             chd_pvt->key, chd_pvt->key_len);
    ALPM_IEG(rv);
    step_trie_merged = 1;

    /* 4. Bank shrink and update prt_pvt */
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        &PVT_BKT_INFO(prt_pvt), NULL);
    rv = alpm_pvt_update_by_pvt_node(u, acb, prt_pvt, ALPM_PVT_UPDATE_ALPM_DATA);
    if (BCM_FAILURE(rv)) {
        /* Not expecting failure here */
        ALPM_ERR(("cb_merge pvt update failed, rv=%d\n", rv));
    }

    /* 5. Delete child pvt entry from pvt table */
    rv = alpm_pvt_delete_by_pvt_node(u, acb, chd_pvt);
    if (BCM_FAILURE(rv)) {
        /* Not expecting delete failure here */
        ALPM_ERR(("cb_merge pvt delete failed, rv=%d\n", rv));
    }

    /* 6. Invalid old entries
     *    Not expecting failures on invalidation */
    rv = alpm_bkt_pfx_clean(u, acb, chd_pvt, pfx_arr->pfx_cnt, bak_idx);
    ALPM_IEG(rv);
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        &PVT_BKT_INFO(chd_pvt), NULL);

    /* 7. Destroy child pvt from trie, tn is chd_pvt */
    rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), chd_pvt->key,
                     PVT_KEY_LEN(chd_pvt), &tn);
    if (BCM_FAILURE(rv)) {
        /* Not expecting delete failure here */
        ALPM_ERR(("cb_merge alpm_lib_trie_delete failed, rv=%d\n", rv));
    }

    tmp_pvt = (_alpm_pvt_node_t *)tn;
    alpm_lib_trie_destroy(PVT_BKT_TRIE(tmp_pvt));
    alpm_util_free(tmp_pvt);
    alpm_util_free(bak_idx);
    alpm_util_free(pfx_arr);

    return rv;

bad:
    if (step_trie_merged) {
        uint32  pvt[5], len;
        alpm_lib_trie_split2(PVT_BKT_TRIE(prt_pvt), chd_pvt->key,
                             chd_pvt->key_len, pvt, &len,
                             &(PVT_BKT_TRIE(chd_pvt)->trie),
                             PVT_BKT_TRIE(chd_pvt)->trie->count, 1);
    }

    /* Clean new bucket */
    if (bak_idx != NULL) {
        alpm_bkt_pfx_idx_restore(u, pfx_arr, bak_idx);
        (void)alpm_bkt_pfx_clean(u, acb, prt_pvt, pfx_arr->pfx_cnt, bak_idx);
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }
    return rv;
}

/* L3 bucket merge callback */
STATIC int
alpm_l3_merge_cb(alpm_lib_trie_node_t *pptrie,
                 alpm_lib_trie_node_t *ptrie,
                 alpm_lib_trie_node_t *trie,
                 uint32 *prt_key, uint32 prt_key_len,
                 alpm_lib_trie_traverse_state_t *state, void *info)
{
    int u, rv;
    int vrf_id, ipt;
    int prt_type;
    _alpm_cb_t *acb;
    _alpm_bkt_node_t *prt_bkt, *chd_bkt, *lc_bkt;
    _alpm_pvt_node_t *prt_pvt = NULL; /* immediate payload parent pivot */
    _alpm_pvt_node_t *chd_pvt = NULL;
    _alpm_pvt_node_t *lc_pvt = NULL;
    _alpm_pvt_node_t *l1_pvt_node = NULL;
    _alpm_pvt_node_t *new_prt_pvt = NULL;
    alpm_lib_trie_t *pvt_trie;
    int merge_req = MERGE_REQ_NORMAL;
    _alpm_bkt_info_t bkt_info;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    if (ptrie == NULL) {
        merge_ctrl->lc_trie = NULL; /* reset from root */
        return BCM_E_NONE;
    }

    if (_alpm_check_time(CHECK_INC_POLL, merge_ctrl->check_time) == BCM_E_TIMEOUT) {
        *state = trieTraverseStateDone;
    }
    prt_type = ptrie->type; /* immediate parent type (payload or internal) */

    /* For internal parent, save left child and its parent
       (to check mergeability with right child) */
    if (PRT_IS_INTERNAL(prt_type) &&
        (merge_ctrl->lc_trie == NULL || merge_ctrl->lc_ptrie != ptrie)) {
        merge_ctrl->lc_trie = trie;
        merge_ctrl->lc_ptrie = ptrie;
        return BCM_E_NONE;
    }

    /* immediate payload parent (NULL if no exist) */
    prt_bkt = (_alpm_bkt_node_t *)pptrie;
    chd_bkt = (_alpm_bkt_node_t *)trie;
    lc_bkt  = (_alpm_bkt_node_t *)merge_ctrl->lc_trie;
    merge_ctrl->lc_trie = NULL; /* reset after used */
    merge_ctrl->lc_ptrie = NULL;

    assert((PRT_IS_PAYLOAD(prt_type) && pptrie == ptrie) ||
           (PRT_IS_INTERNAL(prt_type) && lc_bkt != NULL));

    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;
    vrf_id = merge_ctrl->vrf_id;
    ipt = merge_ctrl->ipt;
    l1_pvt_node = merge_ctrl->l1_pvt_node;

    assert(l1_pvt_node != NULL);

    /* find corresponding L2 payload prt_pvt, chd_pvt
       or lc_pvt from L2 pvt_trie */
    acb = ACB_DWN(u, acb);
    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);

    if (!pvt_trie) {
        return BCM_E_NONE;
    }

    if (prt_bkt) { /* immediate payload parent */
        rv = alpm_lib_trie_search(pvt_trie, prt_bkt->key, prt_bkt->key_len,
                                  (alpm_lib_trie_node_t **)&prt_pvt);
        if (rv == BCM_E_NOT_FOUND) {
            alpm_util_trie_pfx_print(u, ipt,  prt_bkt->key, prt_bkt->key_len,
                                     "prt_pvt not found:");
        }
        ALPM_IER(rv);
    }

    if (lc_bkt) {
        rv = alpm_lib_trie_search(pvt_trie, lc_bkt->key, lc_bkt->key_len,
                                  (alpm_lib_trie_node_t **)&lc_pvt);
        if (rv == BCM_E_NOT_FOUND) {
            alpm_util_trie_pfx_print(u, ipt,  lc_bkt->key, lc_bkt->key_len,
                                     "lc_pvt not found:");
        }
        ALPM_IER(rv);
    }

    if (chd_bkt) {
        rv = alpm_lib_trie_search(pvt_trie, chd_bkt->key, chd_bkt->key_len,
                                  (alpm_lib_trie_node_t **)&chd_pvt);
        if (rv == BCM_E_NOT_FOUND) {
            alpm_util_trie_pfx_print(u, ipt,  chd_bkt->key, chd_bkt->key_len,
                                     "chd_pvt not found:");
        }
        ALPM_IER(rv);
    }

    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
    rv = alpm_cb_pvt_mergeable(u, acb, prt_type, prt_key, prt_key_len,
                               prt_pvt, chd_pvt, lc_pvt, &bkt_info, &merge_req);

    if (BCM_FAILURE(rv)) {
        _alpm_check_time(CHECK_INC_POLL, merge_ctrl->check_time);
        if (L3_DEFRAG_EXIT(merge_ctrl)) {
            *state = trieTraverseStateDone;
        }
        return BCM_E_NONE; /* no mergeable is OK */
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
    if (PRT_IS_PAYLOAD(prt_type)) {
        cli_out("\n- %s %s #%d prt: cnt %d chd: cnt %d\n",
                merge_req == MERGE_REQ_NEW_BKT ? "L3 payload new bucket"
                                               : "L3 payload",
                BCM_FAILURE(rv) ? "merge impossible" : "merge OK",
                merge_ctrl->merge_count,
                PVT_BKT_TRIE(prt_pvt)->trie ?
                    PVT_BKT_TRIE(prt_pvt)->trie->count : 0,
                PVT_BKT_TRIE(chd_pvt)->trie->count);
        alpm_pvt_node_print(u, acb, prt_pvt);
        alpm_pvt_node_print(u, acb, chd_pvt);
    } else {
        if (merge_req == MERGE_REQ_COVER_BKT) {
            cli_out("\n- %s #%d prt: cnt %d lc: cnt %d chd: cnt %d\n",
                    BCM_FAILURE(rv) ? "L3 internal cover bkt merge impossible"
                                    : "L3 internal cover bkt merge OK",
                    merge_ctrl->merge_count,
                    PVT_BKT_TRIE(prt_pvt)->trie->count,
                    PVT_BKT_TRIE(lc_pvt)->trie->count,
                    PVT_BKT_TRIE(chd_pvt)->trie->count);
            alpm_util_trie_pfx_print(u, ipt, prt_key, prt_key_len,
                                     "Parent key:");
            alpm_pvt_node_print(u, acb, prt_pvt);
            alpm_pvt_node_print(u, acb, lc_pvt);
            alpm_pvt_node_print(u, acb, chd_pvt);
        } else {
            cli_out("\n- %s #%d lc: cnt %d chd: cnt %d\n",
                    BCM_FAILURE(rv) ? "L3 internal merge impossible"
                                    : "L3 internal merge OK",
                    merge_ctrl->merge_count,
                    PVT_BKT_TRIE(lc_pvt)->trie->count,
                    PVT_BKT_TRIE(chd_pvt)->trie->count);
            alpm_util_trie_pfx_print(u, ipt, prt_key, prt_key_len,
                                     "Parent key:");
            alpm_pvt_node_print(u, acb, lc_pvt);
            alpm_pvt_node_print(u, acb, chd_pvt);
        }
    }
    }
#endif

    if (PRT_IS_PAYLOAD(prt_type)) { /* P1 + P2 => P2 (payload) */
        rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, prt_pvt,
                        merge_req == MERGE_REQ_NEW_BKT ? &bkt_info : NULL);
    } else { /* P1 + P2 + covered bkt P4 => P3 (internal) */

        /* 1. Create a new parent pivot */
        rv = alpm_cb_new_pvt_insert(u, acb, prt_key, prt_key_len,
                         &bkt_info, &new_prt_pvt,
                         merge_req == MERGE_REQ_COVER_BKT ? prt_pvt : chd_pvt,
                         l1_pvt_node, merge_req);
        if (rv == BCM_E_FULL) {
            return BCM_E_NONE; /* FULL is OK, continue traverse */
        }
        ALPM_IER(rv);

        /* 2. Merge chd_pvt */
        rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, new_prt_pvt, NULL);
        ALPM_IER(rv);

        /* 3. Merge lc_pvt */
        rv = alpm_cb_merge_payload_pvt(u, acb, lc_pvt, new_prt_pvt, NULL);
        ALPM_IER(rv);
    }

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("L3 alpm merge failed rv %d merge %d\n", rv, acb->acb_cnt.c_merge));
        return rv;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 0, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("L3 merge (%d %d): seq:%d merge:%d\n", prt_type, merge_req,
                    bcm_esw_alpm_trace_seq(u), acb->acb_cnt.c_merge);
        }
    }
#endif
    ++merge_ctrl->merge_count;
    ++merge_ctrl->l3_merge_cnt;
    _alpm_check_time(MERGE_INC_POLL, merge_ctrl->check_time);
     if (L3_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    acb->acb_cnt.c_merge++;
    return BCM_E_NONE;
}

/* L3 bucket payload merge callback */
STATIC int
alpm_l3_merge_payload_cb(alpm_lib_trie_node_t *ptrie,
                         alpm_lib_trie_node_t *trie,
                         alpm_lib_trie_traverse_state_t *state, void *info)
{
    int u, rv;
    int vrf_id, ipt;
    int prt_type;
    _alpm_cb_t *acb;
    _alpm_bkt_node_t *prt_bkt, *chd_bkt;
    _alpm_pvt_node_t *prt_pvt = NULL;
    _alpm_pvt_node_t *chd_pvt = NULL;
    _alpm_pvt_node_t *l1_pvt_node = NULL;
    alpm_lib_trie_t *pvt_trie;
    int merge_req = MERGE_REQ_NORMAL;
    _alpm_bkt_info_t bkt_info;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    if (ptrie == NULL) {
        return BCM_E_NONE;
    }

    prt_type = ptrie->type;

    prt_bkt = (_alpm_bkt_node_t *)ptrie;
    chd_bkt = (_alpm_bkt_node_t *)trie;

    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;
    vrf_id = merge_ctrl->vrf_id;
    ipt = merge_ctrl->ipt;
    l1_pvt_node = merge_ctrl->l1_pvt_node;

    assert(l1_pvt_node != NULL);

    /* find corresponding L2 payload prt_pvt, chd_pvt or lc_pvt from L2 pvt_trie */
    acb = ACB_DWN(u, acb);
    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);

    if (!pvt_trie) {
        return BCM_E_NONE;
    }

    if (prt_bkt) { /* immediate payload parent */
        rv = alpm_lib_trie_search(pvt_trie, prt_bkt->key, prt_bkt->key_len,
                                  (alpm_lib_trie_node_t **)&prt_pvt);
        if (rv == BCM_E_NOT_FOUND) {
            alpm_util_trie_pfx_print(u, ipt,  prt_bkt->key, prt_bkt->key_len,
                                     "prt_pvt not found:");
        }
        ALPM_IER(rv);
    }

    if (chd_bkt) {
        rv = alpm_lib_trie_search(pvt_trie, chd_bkt->key, chd_bkt->key_len,
                                  (alpm_lib_trie_node_t **)&chd_pvt);
        if (rv == BCM_E_NOT_FOUND) {
            alpm_util_trie_pfx_print(u, ipt,  chd_bkt->key, chd_bkt->key_len,
                                     "chd_pvt not found:");
        }
        ALPM_IER(rv);
    }

    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
    rv = alpm_cb_pvt_mergeable(u, acb, prt_type, NULL, prt_pvt->key_len,
                               prt_pvt, chd_pvt, NULL, &bkt_info, &merge_req);

    if (BCM_FAILURE(rv)) {
        _alpm_check_time(CHECK_INC_POLL, merge_ctrl->check_time);
        if (L3_DEFRAG_EXIT(merge_ctrl)) {
            *state = trieTraverseStateDone;
        }
        return BCM_E_NONE; /* no mergeable is allowed */
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        cli_out("\n- %s %s #%d prt: cnt %d chd: cnt %d\n",
                merge_req == MERGE_REQ_NEW_BKT ? "L3 payload new bucket"
                                               : "L3 payload",
                BCM_FAILURE(rv) ? "merge impossible" : "merge OK",
                merge_ctrl->merge_count,
                PVT_BKT_TRIE(prt_pvt)->trie ?
                    PVT_BKT_TRIE(prt_pvt)->trie->count : 0,
                PVT_BKT_TRIE(chd_pvt)->trie->count);
        alpm_pvt_node_print(u, acb, prt_pvt);
        alpm_pvt_node_print(u, acb, chd_pvt);
    }
#endif

    /* P1 + P2 => P2 (payload) */
    rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, prt_pvt,
                       merge_req == MERGE_REQ_NEW_BKT ? &bkt_info : NULL);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("L3 alpm payload merge failed rv %d merge %d\n", rv, acb->acb_cnt.c_merge));
        return rv;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 0, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("L3 payload (%d): seq:%d merge:%d\n", merge_req,
                    bcm_esw_alpm_trace_seq(u), acb->acb_cnt.c_merge);
        }
    }
#endif
    ++merge_ctrl->merge_count;
    ++merge_ctrl->l3_merge_cnt;
    _alpm_check_time(MERGE_INC_POLL, merge_ctrl->check_time);
     if (L3_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    acb->acb_cnt.c_merge++;
    return BCM_E_NONE;
}

/* L2 bucket merge callback */
STATIC int
alpm_l2_merge_cb(alpm_lib_trie_node_t *pptrie,
                 alpm_lib_trie_node_t *ptrie,
                 alpm_lib_trie_node_t *trie,
                 uint32 *prt_key, uint32 prt_key_len,
                 alpm_lib_trie_traverse_state_t *state, void *info)
{
    int u, rv;
    int prt_type;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *prt_pvt = NULL; /* immediate payload parent pivot */
    _alpm_pvt_node_t *chd_pvt = NULL;
    _alpm_pvt_node_t *lc_pvt = NULL;
    _alpm_pvt_node_t *new_prt_pvt = NULL;
    int merge_req = MERGE_REQ_NORMAL;
    _alpm_bkt_info_t bkt_info;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    if (!ptrie) {
        merge_ctrl->lc_trie = NULL; /* reset from root */
        return BCM_E_NONE;
    }

    prt_type = ptrie->type;

    /* For internal parent, save left child and its parent
       (to check mergeability with right child) */
    if (PRT_IS_INTERNAL(prt_type) &&
        (merge_ctrl->lc_trie == NULL || merge_ctrl->lc_ptrie != ptrie)) {
        merge_ctrl->lc_trie = trie;
        merge_ctrl->lc_ptrie = ptrie;
        return BCM_E_NONE;
    }

    prt_pvt = (_alpm_pvt_node_t *)pptrie; /* payload parent */
    chd_pvt = (_alpm_pvt_node_t *)trie;
    lc_pvt  = (_alpm_pvt_node_t *)merge_ctrl->lc_trie;
    merge_ctrl->lc_trie = NULL; /* reset after used */
    merge_ctrl->lc_ptrie = NULL;

    assert((PRT_IS_PAYLOAD(prt_type) && pptrie == ptrie) ||
           (PRT_IS_INTERNAL(prt_type) && lc_pvt != NULL));

    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;

    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
    rv = alpm_cb_pvt_mergeable(u, acb, prt_type, prt_key, prt_key_len,
                               prt_pvt, chd_pvt, lc_pvt, &bkt_info, &merge_req);

    if (BCM_FAILURE(rv)) {
        _alpm_check_time(CHECK_INC_POLL, merge_ctrl->check_time);
        if (L2_DEFRAG_EXIT(merge_ctrl)) {
            *state = trieTraverseStateDone;
        }
        return BCM_E_NONE;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
    int ipt = merge_ctrl->ipt;
    if (PRT_IS_PAYLOAD(prt_type)) {
        cli_out("\n- %s %s #%d prt: cnt %d chd: cnt %d\n",
                merge_req == MERGE_REQ_NEW_BKT ? "L2 payload new bucket"
                                               : "L2 payload",
                BCM_FAILURE(rv) ? "merge impossible" : "merge OK",
                merge_ctrl->merge_count,
                PVT_BKT_TRIE(prt_pvt)->trie ?
                    PVT_BKT_TRIE(prt_pvt)->trie->count : 0,
                PVT_BKT_TRIE(chd_pvt)->trie->count);
        alpm_pvt_node_print(u, acb, prt_pvt);
        alpm_pvt_node_print(u, acb, chd_pvt);
    } else {
        if (merge_req == MERGE_REQ_COVER_BKT) {
            cli_out("\n- %s #%d prt: cnt %d lc: cnt %d chd: cnt %d\n",
                    BCM_FAILURE(rv) ? "L2 internal cover bkt merge impossible"
                                    : "L2 internal cover bkt merge OK",
                    merge_ctrl->merge_count,
                    PVT_BKT_TRIE(prt_pvt)->trie->count,
                    PVT_BKT_TRIE(lc_pvt)->trie->count,
                    PVT_BKT_TRIE(chd_pvt)->trie->count);
            alpm_util_trie_pfx_print(u, ipt, prt_key, prt_key_len,
                                     "Parent key:");
            alpm_pvt_node_print(u, acb, prt_pvt);
            alpm_pvt_node_print(u, acb, lc_pvt);
            alpm_pvt_node_print(u, acb, chd_pvt);
        } else {
            cli_out("\n- %s #%d lc: cnt %d chd: cnt %d\n",
                    BCM_FAILURE(rv) ? "L2 internal merge impossible"
                                    : "L2 internal merge OK",
                    merge_ctrl->merge_count,
                    PVT_BKT_TRIE(lc_pvt)->trie->count,
                    PVT_BKT_TRIE(chd_pvt)->trie->count);
            alpm_util_trie_pfx_print(u, ipt, prt_key, prt_key_len,
                                     "Parent key:");
            alpm_pvt_node_print(u, acb, lc_pvt);
            alpm_pvt_node_print(u, acb, chd_pvt);
        }
    }
    }
#endif

    if (PRT_IS_PAYLOAD(prt_type)) { /* P1 + P2 => P2 (payload) */
        rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, prt_pvt,
                           merge_req == MERGE_REQ_NEW_BKT ? &bkt_info : NULL);
    } else { /* P1 + P2 => P3 (internal) */

        /* 1. Create a new parent pivot */
        rv = alpm_cb_new_pvt_insert(u, acb, prt_key, prt_key_len,
                         &bkt_info, &new_prt_pvt,
                         merge_req == MERGE_REQ_COVER_BKT ? prt_pvt : chd_pvt,
                         NULL, merge_req);
        if (rv == BCM_E_FULL) {
            return BCM_E_NONE; /* FULL is OK, continue traverse */
        }
        ALPM_IER(rv);

        /* 2. Merge chd_pvt */
        rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, new_prt_pvt, NULL);
        ALPM_IER(rv);

        /* 3. Merge lc_pvt */
        rv = alpm_cb_merge_payload_pvt(u, acb, lc_pvt, new_prt_pvt, NULL);
        ALPM_IER(rv);
    }

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("L2 alpm merge failed rv %d %d\n", rv, acb->acb_cnt.c_merge));
        return rv;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 0, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("L2 merge (%d %d): seq:%d merge:%d\n", prt_type, merge_req,
                    bcm_esw_alpm_trace_seq(u), acb->acb_cnt.c_merge);
        }
    }
#endif
    ++merge_ctrl->merge_count;
    ++merge_ctrl->l2_merge_cnt;
    _alpm_check_time(MERGE_INC_POLL, merge_ctrl->check_time);
     if (L2_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    acb->acb_cnt.c_merge++;
    return BCM_E_NONE;
}

/* L2 bucket merge to payload prt_pvt callback */
STATIC int
alpm_l2_merge_payload_cb(alpm_lib_trie_node_t *ptrie,
                         alpm_lib_trie_node_t *trie,
                         alpm_lib_trie_traverse_state_t *state, void *info)
{
    int u, rv;
    int prt_type;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *prt_pvt, *chd_pvt;
    int merge_req = MERGE_REQ_NORMAL;
    _alpm_bkt_info_t bkt_info;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    if (!ptrie) {
        return BCM_E_NONE;
    }

    prt_type = ptrie->type;

    prt_pvt = (_alpm_pvt_node_t *)ptrie;
    chd_pvt = (_alpm_pvt_node_t *)trie;

    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;

    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
    rv = alpm_cb_pvt_mergeable(u, acb, prt_type, NULL, prt_pvt->key_len,
                               prt_pvt, chd_pvt, NULL, &bkt_info, &merge_req);

    if (BCM_FAILURE(rv)) {
        _alpm_check_time(CHECK_INC_POLL, merge_ctrl->check_time);
        if (L2_DEFRAG_EXIT(merge_ctrl)) {
            *state = trieTraverseStateDone;
        }
        return BCM_E_NONE;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        cli_out("\n- %s %s #%d prt: cnt %d chd: cnt %d\n",
                merge_req == MERGE_REQ_NEW_BKT ? "L2 payload new bucket"
                                               : "L2 payload",
                BCM_FAILURE(rv) ? "merge impossible" : "merge OK",
                merge_ctrl->merge_count,
                PVT_BKT_TRIE(prt_pvt)->trie ?
                    PVT_BKT_TRIE(prt_pvt)->trie->count : 0,
                PVT_BKT_TRIE(chd_pvt)->trie->count);
        alpm_pvt_node_print(u, acb, prt_pvt);
        alpm_pvt_node_print(u, acb, chd_pvt);
    }
#endif

    /* P1 + P2 => P2 (payload) */
    rv = alpm_cb_merge_payload_pvt(u, acb, chd_pvt, prt_pvt,
                       merge_req == MERGE_REQ_NEW_BKT ? &bkt_info : NULL);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("L2 alpm payload merge failed rv %d merge %d\n",
                  rv, acb->acb_cnt.c_merge));
        return rv;
    }

#ifdef DEFRAG_DEBUG
    if (bcm_esw_alpm_trace_seq(u) >= TR_SEQ) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 0, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("L2 payload (%d): seq:%d merge:%d\n", merge_req,
                    bcm_esw_alpm_trace_seq(u), acb->acb_cnt.c_merge);
        }
    }
#endif
    ++merge_ctrl->merge_count;
    ++merge_ctrl->l2_merge_cnt;
    _alpm_check_time(MERGE_INC_POLL, merge_ctrl->check_time);
     if (L2_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    acb->acb_cnt.c_merge++;
    return BCM_E_NONE;
}

/* L3 defrag callback */
STATIC int
_alpm_l3_defrag_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                  alpm_lib_trie_traverse_state_t *state, void *info)
{
    int rv;
    _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    /* Carry current l1 pvt_node for later l2 pvt insert (saves l1 pvt find) */
    merge_ctrl->l1_pvt_node = pvt_node;

    /* traverse3 for l2_pvt_bkt_trie merge */
    rv = alpm_lib_trie_traverse3(PVT_BKT_TRIE(pvt_node), alpm_l3_merge_cb,
                                 info, trieTraverseOrderPost);

    if (L3_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    return rv;

}

/* L3 payload defrag callback */
STATIC int
_alpm_l3_payload_defrag_cb(alpm_lib_trie_node_t *ptrie,
                           alpm_lib_trie_node_t *trie,
                           alpm_lib_trie_traverse_state_t *state,
                           void *info)
{
    int rv;
    _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;

    /* Carry current l1 pvt_node for later l2 pvt insert (saves l1 pvt find) */
    merge_ctrl->l1_pvt_node = pvt_node;

    /* traverse2 for l2_pvt_bkt_trie payload merge */
    rv = alpm_lib_trie_traverse2(PVT_BKT_TRIE(pvt_node),
                                 alpm_l3_merge_payload_cb,
                                 info, trieTraverseOrderPost);

    if (L3_DEFRAG_EXIT(merge_ctrl)) {
        *state = trieTraverseStateDone;
    }
    return rv;

}

STATIC int
alpm_l3_defrag(int u, int app, int vrf_id, int ipt,
               _alpm_cb_merge_ctrl_t *merge_ctrl)
{
    int rv = BCM_E_NONE;
    alpm_lib_trie_t *pvt_trie;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    int prev_cnt = 0;

    if (L3_DEFRAG_EXIT(merge_ctrl)) {
        goto done;
    }

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (pvt_trie == NULL || pvt_trie->trie == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* 1. Internal defrag */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse2 for only payload l1_pvt_trie */
        rv = alpm_lib_trie_traverse2(pvt_trie, _alpm_l3_defrag_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L3 defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L3_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

    if (merge_ctrl->merge_count > 0) {
        goto done; /* internal merge is enough */
    }

    /* 2. Try payload defrag if no free bkt for internal merge */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse2 for only payload l1_pvt_trie */
        rv = alpm_lib_trie_traverse2(pvt_trie, _alpm_l3_payload_defrag_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L3 payload defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L3_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

    if (merge_ctrl->merge_count == 0) {
        goto done; /* done if payload defrag doesn't free any bkt */
    }

    /* 3. Final internal defrag (after payload defrag) */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse2 for only payload l1_pvt_trie */
        rv = alpm_lib_trie_traverse2(pvt_trie, _alpm_l3_defrag_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L3 final defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L3_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

done:
    return rv;
}

STATIC int
alpm_l2_defrag(int u, int app, int vrf_id, int ipt,
               _alpm_cb_merge_ctrl_t *merge_ctrl)
{
    int rv = BCM_E_NONE;
    alpm_lib_trie_t *pvt_trie;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    int prev_cnt = 0;

    if (L2_DEFRAG_EXIT(merge_ctrl)) {
        goto done;
    }

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (pvt_trie == NULL ||
        pvt_trie->trie == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* 1. Internal defrag */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse3 for l1_pvt_trie merge */
        rv = alpm_lib_trie_traverse3(pvt_trie, alpm_l2_merge_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L2 defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L2_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

    if (merge_ctrl->merge_count > 0) {
        goto done; /* internal merge is enough */
    }

    /* 2. Try payload defrag if no free bkt for internal merge */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse2 for l1_pvt_trie to payload prt_pvt merge */
        rv = alpm_lib_trie_traverse2(pvt_trie, alpm_l2_merge_payload_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L2 payload defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L2_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

    if (merge_ctrl->merge_count == 0) {
        goto done; /* done if payload defrag doesn't free any bkt */
    }

    /* 3. Final internal defrag (after payload defrag) */
    merge_ctrl->merge_count = 0;
    do {
        prev_cnt = merge_ctrl->merge_count;
        merge_ctrl->lc_trie = NULL;
        merge_ctrl->lc_ptrie = NULL;
        merge_ctrl->l1_pvt_node = NULL;
        /* traverse3 for l1_pvt_trie merge */
        rv = alpm_lib_trie_traverse3(pvt_trie, alpm_l2_merge_cb,
                                     merge_ctrl, trieTraverseOrderPost);
#ifdef DEFRAG_RESULT
        cli_out("L2 final defrag cnt = %d\n", merge_ctrl->merge_count);
#endif
        ALPM_IER(rv);
        if (L2_DEFRAG_EXIT(merge_ctrl)) {
            goto done;
        }
    } while (prev_cnt < merge_ctrl->merge_count);

done:
    return rv;
}

/*
 * Function:
 *     bcm_esw_alpm_defrag
 * Purpose:
 *     Main function of ALPM defrag for a specific or all (vrf, ipt) routes,
 *     with a given restriction on maximum time of execution or merge counts.
 * Parameters:
 *                u - (IN) BCM device number
 *              vrf - (IN) vrf>=0 for a specific private VRF,
 *                         BCM_L3_VRF_GLOBAL for Global low,
 *                         BCM_L3_VRF_ALL for all VRFs.
 *           ipt_in - (IN) IP types: bcmL3AlpmIptV4, bcmL3AlpmIptV6, or bcmL3AlpmIptAll.
 *      max_time_ms - (IN) Maximum time in ms for defrag. -1 or 0 for no time restriction.
 * max_l2_merge_cnt - (IN) Maximum L2 bucket merge count (stop when reach this number),
 *                         0 for no doing merge in this level, -1 for all possible merges.
 * max_l3_merge_cnt - (IN) Maximum L3 bucket merge count (stop when reach this number),
 *                         0 for no doing merge in this level, -1 for all possible merges.
 *     l2_merge_cnt - (OUT) L2 bucket merge counter achieved during this defrag API call.
 *     l3_merge_cnt - (OUT) L3 bucket merge counter achieved during this defrag API call.
 * Returns:
 *     BCM_E_XXX:
 *      - BCM_E_UNAVAIL: if this feature is not supported (eg. ALPM disabled).
 * Notes:
 *     Currently this feature is only supported in TH3 ALPM2.
 */
int
bcm_esw_alpm_defrag(int u,
                    bcm_vrf_t vrf,
                    bcm_l3_alpm_ipt_t ipt_in,
                    int max_time_ms,
                    int max_l2_merge_cnt,
                    int max_l3_merge_cnt,
                    int *l2_merge_cnt,
                    int *l3_merge_cnt)
{
    int rv = BCM_E_NONE;
    int ipt;
    int vrf_id;
    int app = APP0;
    _alpm_cb_merge_ctrl_t merge_ctrl;
    _alpm_time_poll_t check_time;
    _alpm_cb_t  *acb0 = ACB(u, 0, app);
    _alpm_cb_t  *acb1 = ACB(u, 1, app);

    ALPMC_INIT_CHECK(u);

    if (vrf == BCM_L3_VRF_GLOBAL) {
        vrf = ALPM_VRF_ID_GLO(u);
    }

    L3_LOCK(u);

    sal_memset(&merge_ctrl, 0, sizeof(merge_ctrl));
    merge_ctrl.max_l2_merge_cnt = (max_l2_merge_cnt < 0) ? MAX_MERGE_CNT
                                                         : max_l2_merge_cnt;
    merge_ctrl.max_l3_merge_cnt = (max_l3_merge_cnt < 0) ? MAX_MERGE_CNT
                                                         : max_l3_merge_cnt;

    if (max_time_ms > 0) {
        if (max_time_ms < MIN_TIME_DEFRAG) {
            max_time_ms = MIN_TIME_DEFRAG;
        }

        _alpm_check_time_init(max_time_ms, MAX_POLL_DEFRAG, &check_time);
        merge_ctrl.check_time = &check_time;
    }

    /* Exclude Override (Global high) or any Direct routes for defrag */
    for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_GLO(u); vrf_id++) {
        if ((vrf != BCM_L3_VRF_ALL && vrf_id != vrf) ||
            (vrf_id == ALPM_VRF_ID_GLO(u) &&
             ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM))) {
            continue;
        }

        for (ipt = 0; ipt < bcmL3AlpmIptCount; ipt++) {
            if (ipt_in != bcmL3AlpmIptAll && ipt != ipt_in) {
                continue;
            }
            merge_ctrl.unit     = u;
            merge_ctrl.ipt      = ipt;
            merge_ctrl.vrf_id   = vrf_id;
            merge_ctrl.acb      = acb0;

            if ((max_l3_merge_cnt != 0) &&
                ACB_HAS_PVT(acb0, vrf_id) &&    /* level1 bkt is pivot */
                ACB_VRF_INITED(u, acb1, vrf_id, ipt)) {
                rv = alpm_l3_defrag(u, app, vrf_id, ipt, &merge_ctrl);
                ALPM_IEG(rv);
            }

            if ((max_l2_merge_cnt != 0) &&
                ACB_VRF_INITED(u, acb0, vrf_id, ipt)) {
                rv = alpm_l2_defrag(u, app, vrf_id, ipt, &merge_ctrl);
                ALPM_IEG(rv);
            }

            if (ALPM_DEFRAG_EXIT(&merge_ctrl)) {
                goto bad; /* exit */
            }
        }
    }
bad:
    L3_UNLOCK(u);
    if (l2_merge_cnt != NULL) {
        *l2_merge_cnt = merge_ctrl.l2_merge_cnt;
    }
    if (l3_merge_cnt != NULL) {
        *l3_merge_cnt = merge_ctrl.l3_merge_cnt;
    }

    return rv;
}

#else
typedef int bcm_esw_alpm2_alpm_defrag_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
