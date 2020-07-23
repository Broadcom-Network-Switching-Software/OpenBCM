/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm.c
 * Purpose: ALPM device independent implementation.
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

extern int alpm_bkt_pfx_idx_backup(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 **bak_idx);
extern int alpm_bkt_pfx_copy(int u, _alpm_cb_t *acb, _alpm_bkt_pfx_arr_t *pfx_arr,
                             _alpm_pvt_node_t *opvt_node, _alpm_pvt_node_t *npvt_node);
extern int alpm_pvt_update_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, uint32 write_flags);
extern int alpm_bkt_pfx_clean(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                              int count, uint32 *ent_idx);
extern void alpm_bkt_bnk_shrink(int u, int ipt, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                                _alpm_bkt_info_t *bkt_info, int *fmt_update);
extern int alpm_pvt_delete_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node);
extern void alpm_bkt_pfx_idx_restore(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 *bak_idx);
extern int alpm_pvt_update_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, uint32 write_flags);
extern int alpm_ppg_op(int u, int ppg_op, void *cb, _alpm_ppg_data_t *ppg_data);

static int
alpm_ppg_hitbit_cb(alpm_lib_trie_node_t *trie, alpm_lib_trie_bpm_cb_info_t *info)
{
    int rv = BCM_E_NONE, unit;
    uint32 pfx_len, pvt_bpm_len;

    _alpm_cb_t *acb;
    _alpm_ppg_cb_user_data_t *user_data;
    _alpm_ppg_data_t *ppg_data;
    _alpm_pvt_node_t *pvt_node;

    user_data = info->user_data;
    unit     = user_data->unit;
    ppg_data = user_data->ppg_data;
    acb      = user_data->ppg_acb;

    pvt_node    = (_alpm_pvt_node_t *)trie;
    pvt_bpm_len = PVT_BPM_LEN(pvt_node);
    pfx_len     = ppg_data->key_len;

    /* Addr & Mask has been screened by propagate function,
     * no need to do it again */
    if (pvt_bpm_len != pfx_len) {
        /* Not qualified and stop from traversing its child nodes */
        return BCM_E_NONE;
    }
    /* bypass MC (direct route) for ppg in case of mix UC & MC in same VRF */
    if (PVT_IS_IPMC(pvt_node)) {
        return BCM_E_NONE;
    }

    /* New idx of def_pfx was already updated in regular routine,
     * Just simply update pivot */
    rv = alpm_pvt_update_by_pvt_node(unit, acb, pvt_node, ALPM_PVT_UPDATE_ASSOC_DATA);

    /* Update callback count */
    user_data->ppg_cnt++;

    return rv;
}

int
alpm_ppg_hitbit(int u, int app, int vrf_id, int ipt, uint32 *key, int key_len)
{
    _alpm_ppg_data_t ppg_data;

    if (ALPM_HIT_SKIP(u)) {
        return BCM_E_NONE;
    }

    if (!ALPM_TCAM_HB_IS_PTR(u)) {
        return BCM_E_NONE;
    }

    sal_memset(&ppg_data, 0, sizeof(_alpm_ppg_data_t));
    ppg_data.app     = app;
    ppg_data.ipt     = ipt;
    ppg_data.vrf_id  = vrf_id;
    ppg_data.key_len = key_len;
    sal_memcpy(ppg_data.key, key, sizeof(ppg_data.key));

    return alpm_ppg_op(u, ALPM_PPG_HBP, alpm_ppg_hitbit_cb, &ppg_data);
}

static void
alpm_bkt_sub_bkt_info(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                     _alpm_bkt_pool_conf_t *bp_conf,
                     int start_bnk, int range, int *rt_ipt, uint32 *sub_bkt_bmp)
{
    int i, bnk_pbk;
    SHR_BITDCL *bnk_bmp;
    _alpm_pvt_node_t *pvt_node;

    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);
    for (i = start_bnk; i < start_bnk + range; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            int tab_idx =
                ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i / bnk_pbk, i % bnk_pbk);
            pvt_node =
                (_alpm_pvt_node_t *)ACB_VRF_PVT_PTR(acb, vrf_id, tab_idx);
            if (pvt_node != NULL) {
                *rt_ipt = PVT_BKT_IPT(pvt_node);
                *sub_bkt_bmp |= 1 << BI_SUB_BKT_IDX(&pvt_node->bkt_info);
            }
        }
    }

    return ;
}

static int
alpm_bkt_pkm_sbkt_chk(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                      _alpm_bkt_pool_conf_t *bp_conf,
                      int start_bnk, int range,
                      uint16 *sub_bkt_idx, int *sub_bkt_ipt)
{
    uint32 sub_bkt_bmp = 0;

    uint16 first_sub_bkt[] = {
        0, /* 0b0000 */ 1, /* 0b0001 */ 0, /* 0b0010 */ 2, /* 0b0011 */
        0, /* 0b0100 */ 1, /* 0b0101 */ 0, /* 0b0110 */ 3, /* 0b0111 */
        0, /* 0b1000 */ 1, /* 0b1001 */ 0, /* 0b1010 */ 2, /* 0b1011 */
        0, /* 0b1100 */ 1, /* 0b1101 */ 0, /* 0b1110 */
    };

    *sub_bkt_ipt = -1;
    alpm_bkt_sub_bkt_info(u, acb, vrf_id, ipt, bp_conf, start_bnk, range,
                          sub_bkt_ipt, &sub_bkt_bmp);
    /* All 4 sub-pkts are used */
    if (sub_bkt_bmp >= 0xf) {
        return BCM_E_NOT_FOUND;
    }

    *sub_bkt_idx = first_sub_bkt[sub_bkt_bmp];
    return BCM_E_NONE;
}

/* step == 0xfffffff means check if *start_bnk is occupied */
static int
alpm_bkt_sharing_bnk_alloc(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                           _alpm_bkt_pool_conf_t *bp_conf,
                           _alpm_bkt_info_t *bkt_info, int *avl_sz,
                           int start_bnk, int alloc_sz, int step)
{
    int rv = BCM_E_NONE;
    int i, j, bnks, bnk_cnt, bnk_end, bnk_pbk, used;
    uint16 sub_bkt_idx = 0;
    int count, range = 0;
    int check_avl, max_avl_sz = 0;
    SHR_BITDCL *bnk_bmp;
    int empty_first;

    sal_memset(bkt_info, 0, sizeof(_alpm_bkt_info_t));
    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    bnk_cnt = BPC_BNK_CNT(bp_conf);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);

    /* if step == 0xfffffff means check_exist */
    check_avl = (alloc_sz == 1) && (step > (bnk_cnt + bnk_pbk));
    assert(check_avl || (start_bnk == 0 && step == bnk_pbk));

    /* Case 1: Check if a bank is available */
    if (check_avl) {
        i = start_bnk;
        range = alloc_sz;
        if (!SHR_BITGET(bnk_bmp, i)) {
            goto _done;
        }

        return BCM_E_NOT_FOUND;
    }

    range = bnk_pbk;
    bnk_end = bnk_cnt - range + 1;

#define _ALLOC_BKT_EMPTY_OR_SHARE(empty_first) \
    do { \
    if (empty_first) { \
        /* Case 2: Allocate bank from empty buckets */ \
        for (i = 0; i < bnk_end; i += step) { \
            SHR_BITTEST_RANGE(bnk_bmp, i, range, used); \
            if (!used) { \
                break; \
            } \
        } \
    } else { \
        /* Case 3: Allocate bank from non-empty buckets \
         * WARNING: V4/V6 should not be sharing same bucket */ \
        i = 0; \
        while (i < bnk_end) { \
            SHR_BITCOUNT_RANGE(bnk_bmp, count, i, range); \
            if (range == count || count == 0) { \
                i += step; \
                continue; \
            } \
            if ((range - count) > max_avl_sz) { \
                int bkt_ipt; \
                rv = alpm_bkt_pkm_sbkt_chk(u, acb, vrf_id, ipt, bp_conf, i, range, \
                                           &sub_bkt_idx, &bkt_ipt); \
                if (rv == BCM_E_NOT_FOUND || bkt_ipt != ipt) { \
                    i += step; \
                    continue; \
                } \
                max_avl_sz = range - count; \
                if (max_avl_sz >= alloc_sz) { \
                    /* Check available sub_bkt_idx, \
                     * total sub_bkt_idx cnt is 4, \
                     * check_exist case doesn't need sub_bkt_idx \
                     */ \
                    break; \
                } \
            } \
            i += step; \
        } \
    } \
    } while (0)

    empty_first = !(ALPM_BKT_SHARE_THRES(u) &&
                   (BPC_BKT_USED(bp_conf, ipt) > (ACB_BKT_CNT(acb) >> 1)) &&
                   (BPC_BNK_USED(bp_conf, ipt) * 100 <= ALPM_BKT_SHARE_THRES(u) * BPC_BKT_TOT_USED(bp_conf) * bnk_pbk));

    _ALLOC_BKT_EMPTY_OR_SHARE(empty_first);
    if (i < bnk_end) {
        goto _done;
    }
    /* Force the use of remaining banks from non-empty when split (always request full bucket) */
    if (!empty_first && alloc_sz == bnk_pbk && max_avl_sz > 0) {
        if (avl_sz != NULL) {
            *avl_sz = max_avl_sz;
        }
        return BCM_E_NOT_FOUND; /* intensionally */
    }
    _ALLOC_BKT_EMPTY_OR_SHARE(!empty_first);
    if (i >= bnk_end) {
        if (avl_sz != NULL) {
            *avl_sz = max_avl_sz;
        }
        return BCM_E_NOT_FOUND;
    }

_done:
    BI_ROFS(bkt_info) = 0;
    BI_BKT_IDX(bkt_info) = i / bnk_pbk;
    BI_SUB_BKT_IDX(bkt_info) = sub_bkt_idx;
    bnks = 0;

    if (ALPM_BKT_SHARE_THRES(u)) {
        /* Inc BKT_USED before bnk_bmp set if no used bits in the bkt */
        SHR_BITTEST_RANGE(bnk_bmp, (i - i % bnk_pbk), bnk_pbk, used);
        if (!used) {
            BPC_BKT_USED(bp_conf, ipt) ++;
        }
    }

    for (j = i; j < i+range; j++) {
        if (!SHR_BITGET(bnk_bmp, j)) {
            bkt_info->bnk_fmt[j % bnk_pbk] =
                alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
            /* bkt_info->vet_bmp default to 0 */
            SHR_BITSET(bnk_bmp, j);
            if (++bnks >= alloc_sz) {
                break;
            }
        }
    }

    BPC_BNK_USED(bp_conf, ipt) += alloc_sz;
    return BCM_E_NONE;
}

/* What is expected here when entering this function:
 * all occupied banks are continuous.
 *    all i in {i | bkt_info->bnk_fmt[i] is used} are continuous AND
 *    foreach i belongs to {i | bkt_info->bnk_fmt[i] is used}:
 *        bkt_info->vet_bmp[i]!=0
 */
int
alpm_cb_expand_in_bkt(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, int bnk_cnt)
{
    int i, rv = BCM_E_FULL;
    int bnkpb, end;
    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_bkt_info_t dst_bkt;

    ALPM_VERB(("Expanding ALPM CB %d bkt %d......\n",
               acb->acb_idx, PVT_BKT_IDX(pvt_node)));

    /* Case 1: bank available in the same bucket
     *         -> occupy the free bank */

    bkt_info = &PVT_BKT_INFO(pvt_node);
    bp_conf  = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    end   = bnkpb;

    /* Occupy the free bank in the same bucket based on bkt_idx */
    for (i = 0; i < end; i++) {
        int glb_bnk;
        if (BI_BNK_IS_USED(bkt_info, i)) {
            continue;
        }

        glb_bnk = PVT_BKT_IDX(pvt_node) * bnkpb + i;
        rv = alpm_bkt_sharing_bnk_alloc(u, acb, PVT_BKT_VRF(pvt_node),
                    PVT_BKT_IPT(pvt_node),
                    bp_conf, &dst_bkt, NULL,
                    glb_bnk, 1, 0xfffffff);
        if (BCM_SUCCESS(rv)) {
            BI_ROFS(bkt_info) = 0;
            bkt_info->bnk_fmt[glb_bnk % bnkpb] =
                dst_bkt.bnk_fmt[glb_bnk % bnkpb];
            goto end;
        }
    }

    /* Case 2: No free bank in the same bucket, but there are other
     * continuous banks available:
     *         -> Move banks to other area */
    rv = alpm_bkt_sharing_bnk_alloc(u, acb, PVT_BKT_VRF(pvt_node),
                PVT_BKT_IPT(pvt_node),
                bp_conf, &dst_bkt, NULL,
                0, bnk_cnt+1, bnkpb);
    if (BCM_SUCCESS(rv)) {
        /* Move banks */
        rv = alpm_bkt_move(u, acb, pvt_node, bkt_info, &dst_bkt);
        goto end;
    }

    /* Other case: table full */

end:
    if (BCM_SUCCESS(rv)) {
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "Expand");
        }
        acb->acb_cnt.c_expand++;
    } else if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_FULL;
    }
    return rv;
}

static int
alpm_cb_merge_pvt(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *src_pvt,
                  _alpm_pvt_node_t *dst_pvt, int merge_dir)
{
    int rv = BCM_E_NONE;
    int i, bnk_pbk;
    int ipt, vrf_id;
    int used;
    uint32  *bak_idx = NULL;

    int step_trie_merged = 0;

    SHR_BITDCL *bnk_bmp;
    alpm_lib_trie_node_t *tn = NULL;
    _alpm_pvt_node_t *prt_pvt, *chd_pvt, *tmp_pvt;
    _alpm_bkt_info_t tmp_bi;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;

    ipt    = PVT_BKT_IPT(src_pvt);
    vrf_id = PVT_BKT_VRF(src_pvt);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);
    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);

    /* 1. Expand the bkt of dst_pvt */
    for (i = PVT_BKT_IDX(dst_pvt) * bnk_pbk;
         i < (PVT_BKT_IDX(dst_pvt) + 1) * bnk_pbk; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            continue;
        }

        PVT_BKT_INFO(dst_pvt).bnk_fmt[i % bnk_pbk] =
            alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);

        if (ALPM_BKT_SHARE_THRES(u)) {
            /* Inc BKT_USED before bnk_bmp set if no used bits in the bkt */
            SHR_BITTEST_RANGE(bnk_bmp, (i - i % bnk_pbk), bnk_pbk, used);
            if (!used) {
                BPC_BKT_USED(bp_conf, ipt) ++;
            }
        }

        SHR_BITSET(bnk_bmp, i);
        BPC_BNK_USED(bp_conf, ipt) += 1;
    }

    /* 2. Copy pfx from src_pvt to dst_pvt */
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = alpm_lib_trie_traverse(PVT_BKT_TRIE(src_pvt), alpm_trie_pfx_cb, pfx_arr,
                       trieTraverseOrderIn, TRUE);
    ALPM_IEG(rv);
    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);
    rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, src_pvt, dst_pvt);
    ALPM_IEG(rv);

    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        prt_pvt = src_pvt;
        chd_pvt = dst_pvt;
    } else {
        prt_pvt = dst_pvt;
        chd_pvt = src_pvt;
    }

    /* 3. Merge trie */
    rv = alpm_lib_trie_merge(PVT_BKT_TRIE(prt_pvt), PVT_BKT_TRIE(chd_pvt)->trie,
                             chd_pvt->key, chd_pvt->key_len);
    ALPM_IEG(rv);
    step_trie_merged = 1;

    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        /* 4. Update pvt bkt_ptr if copy from parent to child */
        sal_memcpy(&tmp_bi, &PVT_BKT_INFO(prt_pvt), sizeof(_alpm_bkt_info_t));
        sal_memcpy(&PVT_BKT_INFO(prt_pvt), &PVT_BKT_INFO(chd_pvt),
                   sizeof(_alpm_bkt_info_t));
        rv = alpm_pvt_update_by_pvt_node(u, acb, prt_pvt, ALPM_PVT_UPDATE_ALPM_DATA);
        if (BCM_FAILURE(rv)) {
            /* Not expecting delete failure here */
            ALPM_ERR(("cb_merge pvt update failed, rv=%d\n", rv));
        }
        /* Recovery bkt info for invalidation */
        sal_memcpy(&PVT_BKT_INFO(prt_pvt), &tmp_bi, sizeof(_alpm_bkt_info_t));
    }

    /* 5. Invalid old entries
     *    Not expecting failures on invalidation */
    rv = alpm_bkt_pfx_clean(u, acb, src_pvt, pfx_arr->pfx_cnt, bak_idx);
    ALPM_IEG(rv);
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        &PVT_BKT_INFO(src_pvt), NULL);
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        &PVT_BKT_INFO(dst_pvt), NULL);

    /* 6. Delete child pvt entry from pvt table */
    rv = alpm_pvt_delete_by_pvt_node(u, acb, chd_pvt);
    if (BCM_FAILURE(rv)) {
        /* Not expecting delete failure here */
        ALPM_ERR(("cb_merge pvt delete failed, rv=%d\n", rv));
    }

    /* 7. Destroy child pvt from trie, tn is chd_pvt */
    rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), chd_pvt->key,
                     PVT_KEY_LEN(chd_pvt), &tn);
    if (BCM_FAILURE(rv)) {
        /* Not expecting delete failure here */
        ALPM_ERR(("cb_merge alpm_lib_trie_delete failed, rv=%d\n", rv));
    }
    /* Should copy bi before alpm_lib_trie_destroy tn */
    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        int tab_idx, bnk;
        _alpm_bkt_info_t *bi;
        bi = &PVT_BKT_INFO(prt_pvt);
        sal_memcpy(bi, &PVT_BKT_INFO(chd_pvt), sizeof(_alpm_bkt_info_t));

        /* Update pvt_ptr for child bucket */
        for (bnk = 0; bnk < bnk_pbk; bnk++) {
            if (BI_BNK_IS_USED(bi, bnk)) {
                tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bi, bnk);
                bp_conf->pvt_ptr[tab_idx] = prt_pvt;
            }
        }
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
        (void)alpm_bkt_pfx_clean(u, acb, dst_pvt, pfx_arr->pfx_cnt, bak_idx);
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }
    return rv;
}

int
alpm_bkt_token_bnks(int u, _alpm_cb_t *acb, int vrf_id,
                    _alpm_pvt_node_t *pvt_node)
{
    int used_cnt = 0;
    int i, bnk_pbk;
    SHR_BITDCL *bnk_bmp;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_pvt_node_t *tmp_node;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnk_bmp = BPC_BNK_BMP(bp_conf, PVT_BKT_IPT(pvt_node));
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);

    for (i = PVT_BKT_IDX(pvt_node) * bnk_pbk;
         i < (PVT_BKT_IDX(pvt_node) + 1) * bnk_pbk; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            int tab_idx =
                ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i / bnk_pbk, i % bnk_pbk);
            tmp_node =
                (_alpm_pvt_node_t *)ACB_VRF_PVT_PTR(acb, vrf_id, tab_idx);
            if (tmp_node && tmp_node != pvt_node) {
                used_cnt ++;
            }
        }
    }

    return used_cnt;
}

static int
alpm_cb_merge_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                 alpm_lib_trie_traverse_state_t *state, void *info)
{
    int u, rv;
    int bnkpb, entpb;
    int vrf_id;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *prt_pvt, *chd_pvt, *src_pvt, *dst_pvt;
    _alpm_cb_merge_ctrl_t *merge_ctrl;
    _alpm_cb_merge_type_t merge_dir = ACB_MERGE_INVALID;

    prt_pvt = (_alpm_pvt_node_t *)ptrie;
    chd_pvt = (_alpm_pvt_node_t *)trie;
    if (!prt_pvt) {
        return BCM_E_NONE;
    }
    if ((PVT_BKT_TRIE(prt_pvt) == NULL) || (PVT_BKT_TRIE(chd_pvt) == NULL)) {
        return BCM_E_NONE;
    }

    if (PVT_BKT_TRIE(prt_pvt)->trie == NULL) {
        merge_dir = ACB_MERGE_PARENT_TO_CHILD;
    }

    if (PVT_BKT_TRIE(chd_pvt)->trie == NULL) {
        merge_dir = ACB_MERGE_CHILD_TO_PARENT;
    }

    merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;
    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;
    vrf_id = merge_ctrl->vrf_id;
    bnkpb  = merge_ctrl->bnk_pbkt;
    entpb  = merge_ctrl->ent_pbnk;

    if (merge_dir == ACB_MERGE_INVALID) {
        int mrts, prt_bnks, chd_bnks;

        mrts = PVT_BKT_TRIE(prt_pvt)->trie->count +
               PVT_BKT_TRIE(chd_pvt)->trie->count;
        if (mrts > bnkpb * entpb) {
            return BCM_E_NONE;
        }

        prt_bnks = bnkpb - alpm_bkt_token_bnks(u, acb, vrf_id, prt_pvt);
        chd_bnks = bnkpb - alpm_bkt_token_bnks(u, acb, vrf_id, chd_pvt);

        if (mrts <= prt_bnks * entpb) {
            merge_dir   = ACB_MERGE_CHILD_TO_PARENT;
        } else if (mrts <= chd_bnks * entpb) {
            merge_dir   = ACB_MERGE_PARENT_TO_CHILD;
        } else {
            return BCM_E_NONE;
        }
    }

    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        src_pvt = prt_pvt;
        dst_pvt = chd_pvt;
    } else {
        src_pvt = chd_pvt;
        dst_pvt = prt_pvt;
    }

    rv = alpm_cb_merge_pvt(u, acb, src_pvt, dst_pvt, merge_dir);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("alpm merge failed rv %d\n", rv));
        return rv;
    }

    *state = trieTraverseStateDel;
    if (++merge_ctrl->merge_count >= ACB_MERGE_THRESHOLD) {
        *state = trieTraverseStateDone;
    }

    acb->acb_cnt.c_merge++;

    return BCM_E_NONE;

}

int
alpm_cb_merge(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int rv = BCM_E_NONE;
    uint8 fmt;
    alpm_lib_trie_t *pvt_trie;
    _alpm_cb_merge_ctrl_t merge_ctrl;

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (!ALPM_MERGE_REQD(u, vrf_id) ||
        pvt_trie == NULL ||
        pvt_trie->trie == NULL) {
        return BCM_E_FULL;
    }

    sal_memset(&merge_ctrl, 0, sizeof(merge_ctrl));
    merge_ctrl.unit     = u;
    merge_ctrl.ipt      = ipt;
    merge_ctrl.vrf_id   = vrf_id;
    merge_ctrl.acb      = acb;

    fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
    merge_ctrl.bnk_pbkt = ACB_BNK_PER_BKT(acb, vrf_id);
    merge_ctrl.ent_pbnk = ACB_FMT_ENT_MAX(acb, vrf_id, fmt);

    rv = alpm_lib_trie_traverse2(pvt_trie, alpm_cb_merge_cb, &merge_ctrl,
                        trieTraverseOrderPost);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("Logic bucket merge failed, ipt %d vrf_id %d rv %d.\n",
                  ipt, vrf_id, rv));
    }

    if (merge_ctrl.merge_count == 0) {
        ALPM_MERGE_STATE_CHKED(u, vrf_id);
        rv = BCM_E_FULL;
    }

    return rv;
}

/* _alpm_bkt_alloc_full to make sure a entire bucket
 * is allocated or return FULL */
static int
alpm_bkt_alloc_attempt(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                       int req_bnk_cnt, int step, _alpm_bkt_info_t *bkt_info,
                       int *avl_bnk_cnt)
{
    int rv;

    rv = alpm_bkt_sharing_bnk_alloc(u, acb, vrf_id, ipt,
                ACB_BKT_VRF_POOL(acb, vrf_id), bkt_info, avl_bnk_cnt,
                0, req_bnk_cnt, step);

    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_FULL;
    }

    return rv;
}

int
alpm_bkt_alloc(int u, _alpm_cb_t *acb, int vrf_id, int ipt, int req_bnk_cnt,
               _alpm_bkt_info_t *bkt_info, int *avl_bnk_cnt, int *pvt_updated)
{
    int rv = BCM_E_NONE;
    rv = alpm_bkt_alloc_attempt(u, acb, vrf_id, ipt, req_bnk_cnt,
                                ACB_BNK_PER_BKT(acb, vrf_id), bkt_info,
                                avl_bnk_cnt);
    if (rv != BCM_E_FULL) {
        return rv;
    }

    /* rv == BCM_E_FULL
     *
     * No free bkt available, start defragment process to create a
     * new available bucket.
     * It's also bucket sharing process for TH/TD3
     */
    rv = alpm_cb_merge(u, acb, vrf_id, ipt);
    if (BCM_SUCCESS(rv)) {
        if (pvt_updated) {
            *pvt_updated = 1;
        } else {
            rv = alpm_bkt_alloc_attempt(u, acb, vrf_id, ipt, req_bnk_cnt,
                                        ACB_BNK_PER_BKT(acb, vrf_id), bkt_info,
                                        avl_bnk_cnt);
        }
    }

    return rv;
}

int
alpm_bkt_bnk_clear(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                   int tab_idx)
{
    int rv = BCM_E_UNAVAIL;
    if (ALPM_DRV(u)->alpm_bkt_bnk_clear) {
        rv = ALPM_DRV(u)->alpm_bkt_bnk_clear(u, acb, pvt_node, tab_idx);
    }
    return rv;
}

#if 0 /* Unused */
int
alpm_bkt_delete(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg, _alpm_pvt_node_t *pvt_node,
                uint32 *key_idx)
{
    int rv = BCM_E_NONE;

    /* ALPMTBD */

    return rv;
}
#endif

#else
typedef int bcm_esw_alpm2_alpm_bkt_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
