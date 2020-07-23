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

#define ALPM_PRJ_MIN_USAGE 5 /* 5% of usage as minimum for ALPM route projection */

/* Internal use only */
typedef struct alpm_pvt_trav_cb_data_s {
    int         unit;
    _alpm_cb_t  *acb;
    int         bkt_idx;

    SHR_BITDCL  bkt[256];                   /* 1K|8K, Max: 13 bits */
    SHR_BITDCL  bnk[ALPM_BPB_MAX * 256];    /* bnk|bkt 1K|5 or 8K|8 */
    SHR_BITDCL  ent[ALPM_BPB_MAX * 8192];   /* 10b/13b(bkt) |3b(bnk)| 5b(ent) */
    uint8       bnkfmt[ALPM_BPB_MAX * 8192];/* Bank format type */
} alpm_pvt_trav_cb_data_t;

char *alpm_tbl_str[] = {
    "alpmTblInvalid",
    "alpmTblPvtCmodeL1",
    "alpmTblPvtPmodeL1",
    "alpmTblPvtCmodeL1P128",
    "alpmTblPvtPmodeL1P128",
    "alpmTblBktCmodeL2",
    "alpmTblBktPmodeL2",
    "alpmTblBktPmodeGlbL2",
    "alpmTblBktPmodePrtL2",
    "alpmTblBktCmodeL3",
    "alpmTblBktPmodeGlbL3",
    "alpmTblBktPmodePrtL3",
    "alpmTblCount"
};

_alpm_ctrl_t *alpm_control[SOC_MAX_NUM_DEVICES];

/* Internal functions */
int alpm_pvt_update_by_pvt_node(int, _alpm_cb_t *, _alpm_pvt_node_t *, uint32);
int alpm_bkt_pfx_shrink(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, void *user_data);
static int alpm_vrf_pfx_trie_add(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg, void *udata);
static int alpm_cb_split(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
                         _alpm_bkt_info_t *nbkt_info, int max_spl_cnt);

/* External functions */

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] = {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  key[4] = {0, 0, IP_ADDR0f, IP_ADDR1f}
 * ipv6-128: key[4] = {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
void
alpm_trie_key_to_pfx(int u, int ipt, uint32 *key, uint32 len, uint32 *pfx)
{
    int i;
    uint32 pfx_shift, tmp;

    pfx[0] = pfx[1] = pfx[2] = pfx[3] = pfx[4] = 0;
    /* Obtain prefix length */
    if (ALPM_IS_IPV4(ipt)) {
        pfx[1] = _SHIFT_RIGHT(key[0], 32 - len);
        pfx[0] = 0;
    } else {
        /* PAIR_128 style, for both mode==2 and mode==3  */
        int start;
        uint32 tmp_pfx[5] = {0};
        tmp_pfx[0] = key[0];
        tmp_pfx[1] = key[1];
        tmp_pfx[2] = key[2];
        tmp_pfx[3] = key[3];

        /* shift entire prefix right to remove trailing 0s in prefix */
        /* to simplify, words which are all 0s are ignored to begin with, so
         * a right shift of say 46 is a right shift of 46-32 = 14, but starting
         * from word 1. Once all shifting is done, shift all words to right by 1
         */
        pfx_shift = 128 - len;
        start = pfx_shift / 32;
        pfx_shift = pfx_shift % 32;
        for (i = start; i < 4; i++) {
            tmp_pfx[i] >>= pfx_shift;
            tmp = tmp_pfx[i+1] & ((1 << pfx_shift) - 1);
            tmp = _SHIFT_LEFT(tmp, 32 - pfx_shift);
            tmp_pfx[i] |= tmp;
        }
        /* make shift right justified and also reverse order for trie.
         * Also, note trie sees key only in words 1 to 4
         */
        for (i = start; i < 4; i++) {
            pfx[4 - (i - start)] = tmp_pfx[i];
        }
    }
    return ;
}

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] = {IP_ADDR0f, 0, 0, 0}
 * ipv6:
 *    64:    key[4] = {0, 0, IP_ADDR0f, IP_ADDR1f}
 *   128:    key[4] = {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 *
 * Note: input key must be a uint32 array with size of 5 (i.e., uint32 key[5]).
 */
void
alpm_trie_pfx_to_key(int u, int ipt, uint32 *pfx, uint32 len, uint32 *key)
{
    int key_bytes = sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_144_);

    sal_memcpy(key, pfx, key_bytes);
    if (ALPM_IS_IPV6(ipt)) {
        uint32 pfx_shift, tmp, start, pfx_tmp[6];
        int i;

        sal_memcpy(pfx_tmp, key, key_bytes);
        sal_memset(key, 0, key_bytes);

        /* shift entire prefix left to add trailing 0s in prefix */
        pfx_shift = 128 - len;
        start = (pfx_shift + 31) / 32;
        /* fix corner case: */
        if ((pfx_shift % 32) == 0) {
            start++;
        }
        pfx_shift = pfx_shift % 32;
        for (i = start; i <= 4; i++) {
            pfx_tmp[i] <<= pfx_shift;
            tmp = pfx_tmp[i+1] & ~(0xffffffff >> pfx_shift);
            tmp = _SHIFT_RIGHT(tmp, 32 - pfx_shift);
            if (i < 4) {
                pfx_tmp[i] |= tmp;
            }
        }
        /* make shift left justified and reversed,
         * i.e, lsw goes to index 0 */
        for (i = start; i <= 4; i++) {
            key[3 - (i - start)] = pfx_tmp[i];
        }
    } else {
        key[0] = _SHIFT_LEFT(key[1], 32 - len);
        key[1] = 0;
    }
}

/* ip_addr => lpm_cfg->user_data
 * OUT: pfx in tire format
 */
void
alpm_trie_cfg_to_pfx(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 *pfx)
{
    /* Create Prefix */
    uint32 key[4] = {0};
    uint32 ipt;

    ipt = ALPM_LPM_IPT(u, lpm_cfg);
    alpm_util_cfg_to_key(u, ipt, lpm_cfg, key);
    alpm_trie_key_to_pfx(u, ipt, key, lpm_cfg->defip_sub_len, pfx);

    return ;
}

/* pfx in trie format => ip_addr
 * OUT: lpm_cfg->defip_ip6_addr/defip_ip_addr from trie format
 */
void
alpm_trie_pfx_to_cfg(int u, uint32 *pfx, int pfx_len, _bcm_defip_cfg_t *lpm_cfg)
{
    /* Create Prefix */
    uint32 key[5] = {0};
    uint32 ipt = ALPM_LPM_IPT(u, lpm_cfg);

    alpm_trie_pfx_to_key(u, ipt, pfx, pfx_len, key);
    alpm_util_key_to_cfg(u, ipt, key, lpm_cfg);
    lpm_cfg->defip_sub_len = pfx_len;
    return ;
}

/* Convert TCAM entry to trie pfx format */
int
alpm_trie_ent_to_pfx(int u, int app, int pkm, void *e, int sub_idx, int pfx_len,
                     uint32 *pfx)
{
    int rv = BCM_E_UNAVAIL;
    uint32 key[4] = {0};

    rv = tcam_entry_to_key(u, app, pkm, e, sub_idx, key);

    if (BCM_SUCCESS(rv)) {
        alpm_trie_key_to_pfx(u, ALPM_PKM2IPT(pkm), key, pfx_len, pfx);
    }

    return rv;
}

int
alpm_trie_pfx_cb(alpm_lib_trie_node_t *node, void *user_data)
{
    _alpm_bkt_pfx_arr_t *pfx_arr = (_alpm_bkt_pfx_arr_t *)user_data;
    if (node->type == trieNodeTypePayload) {
        pfx_arr->pfx[pfx_arr->pfx_cnt] = (_alpm_bkt_node_t *)node;
        pfx_arr->pfx_cnt++;
    }
    return BCM_E_NONE;
}

static int
alpm_trie_max_split_len_cb(alpm_lib_trie_node_t *node, void *user_data)
{
    int *pkm;
    int *pfx_len;

    if (node->type == trieNodeTypePayload) {
        uint32 key_len = ((_alpm_bkt_node_t *)node)->key_len;
        int max_pfx_len[] = {32, 64, 128};
        pkm     = (int *)user_data;
        pfx_len = (int *)user_data + 1;

        if (key_len == max_pfx_len[*pkm]) {
            *pfx_len = key_len;
            return BCM_E_BUSY;
        }
        if (*pfx_len < key_len) {
            *pfx_len = key_len;
        }
    }
    return BCM_E_NONE;
}

static void
alpm_trie_max_split_attr(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                         int *split_len,  int *split_cnt)
{
    int splen = 0;
    int rv, pvt_len = 0;
    int kl, key_len[2] = {0};
    alpm_lib_trie_t  *bkt_trie = NULL;
    int vrf_id = PVT_BKT_VRF(pvt_node);
    int pkm, ipt = PVT_BKT_IPT(pvt_node);

    if (ALPM_IS_IPV6(ipt) && ALPM_128B(u)) {
        pkm = ALPM_PKM_128;
    } else {
        pkm = ipt;
    }

    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        int max_split_len[] = {32, 64, 128};
        splen = max_split_len[pkm] - 1;
    } else {
        pvt_len = PVT_KEY_LEN(pvt_node);
        bkt_trie = PVT_BKT_TRIE(pvt_node);
        key_len[0] = pkm;

        rv = alpm_lib_trie_traverse(bkt_trie, alpm_trie_max_split_len_cb, key_len,
                                    trieTraverseOrderIn, TRUE);
        if (BCM_FAILURE(rv) && rv != BCM_E_BUSY) {
            ALPM_ERR(("alpm_lib_trie_traverse(bkt_trie) failed (%d)", rv));
        }

        kl = key_len[1];
        if (acb->acb_idx == 0) {
            splen = (pvt_len + kl + 1) * 1 / 2;
        } else {
            int splen1, splen2;
            splen1 = (int)ACB_VRF_SPLEN_DIFF1(u, acb, vrf_id, ipt);
            splen2 = (int)ACB_VRF_SPLEN_DIFF2(u, acb, vrf_id, ipt);

            if ((kl - pvt_len) <= splen1) {
                splen = kl - splen2;
                *split_cnt = 1 << splen2;
            } else {
                splen = kl - splen1;
            }
            if (splen <= pvt_len) {
                splen = (pvt_len + kl + 1) * 1 / 2;
            }
        }
    }

    *split_len = splen;

    return ;
}

static int
alpm_trie_node_delete_cb(alpm_lib_trie_node_t *node, void *info)
{
    if (node != NULL) {
        alpm_util_free(node);
    }
    return BCM_E_NONE;
}

static int
alpm_trie_tree_destroy(int u, alpm_lib_trie_t *trie)
{
    int rv = BCM_E_NONE;
    if (trie != NULL) {
        /* traverse bucket trie and clear it */
        rv = alpm_lib_trie_traverse(trie, alpm_trie_node_delete_cb,
                           NULL, trieTraverseOrderIn, FALSE);
        if (BCM_SUCCESS(rv)) {
            alpm_lib_trie_destroy(trie);
        }
    }

    return rv;
}

static int
alpm_trie_pvt_bkt_clear_cb(int u, _alpm_cb_t *acb,
                           _alpm_pvt_node_t *pvt_node, void *datum)
{
    int rv = BCM_E_NONE;
    alpm_lib_trie_t *bkt_trie;

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    ALPM_IER(alpm_trie_tree_destroy(u, bkt_trie));
    PVT_BKT_TRIE(pvt_node) = NULL;

    return rv;
}

static int
alpm_trie_clear_all(int u)
{
    int i, rv = BCM_E_NONE;
    int ipt, vrf_id;
    _alpm_cb_t *acb;
    alpm_lib_trie_t *pvt_trie = NULL, *vrf_trie = NULL;
    int app, app_cnt = alpm_app_cnt(u);

    for (app = 0; app < app_cnt; app++) {
        for (ipt = 0; ipt < ALPM_IPT_CNT; ipt ++) {
            for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
                for (i = 0; i < ACB_CNT(u); i++) {
                    acb = ACB(u, i, app);
                    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                        continue;
                    }
                    /* Clear Bucket Trie-tree */
                    rv = bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                            alpm_trie_pvt_bkt_clear_cb, NULL);
                    ALPM_IER(rv);

                    /* Clear PVT Trie-tree */
                    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
                    ALPM_IER(alpm_trie_tree_destroy(u, pvt_trie));
                    ACB_PVT_TRIE(acb, vrf_id, ipt) = NULL;
                }

                /* Clear Prefix Trie-tree */
                vrf_trie = ALPM_VRF_TRIE(u, app, vrf_id, ipt);
                ALPM_IER(alpm_trie_tree_destroy(u, vrf_trie));
                ALPM_VRF_TRIE(u, app, vrf_id, ipt) = NULL;
            }
        }
    }

    return rv;
}

/* Fill Aux entry for operation */
static void
alpm_ppg_prepare(int u, _bcm_defip_cfg_t *lpm_cfg, int replace_len,
                 void *def_bkt_node, _alpm_ppg_data_t *ppg_data)
{
    sal_memset(ppg_data, 0, sizeof(_alpm_ppg_data_t));

    ppg_data->app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    ppg_data->vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ppg_data->ipt    = ALPM_LPM_IPT(u, lpm_cfg);
    ppg_data->key_len = lpm_cfg->defip_sub_len;
    ppg_data->bpm_len = replace_len;
    sal_memcpy(ppg_data->key, lpm_cfg->user_data, sizeof(ppg_data->key));
    /* Get assoc_data */
    ppg_data->bkt_def = def_bkt_node;

    return ;
}

static int
alpm_ppg(int u, alpm_lib_trie_t *pivot, void *prefix, uint32 len,
         int insert, void *cb, void *user_data)
{
    int rv = BCM_E_NONE;
    uint32 pvt_len = 0;
    alpm_lib_trie_node_t *lpm_pvt = NULL;
    alpm_lib_trie_bpm_cb_info_t cb_info;

    cb_info.pfx = (void *)prefix;
    cb_info.len = len;
    cb_info.user_data = user_data;

    /* Find longest prefix match(pivot) of route */
    ALPM_IER(alpm_lib_trie_find_lpm(pivot, prefix, len, &lpm_pvt));
    pvt_len = (TRIE_ELEMENT_GET(_alpm_pvt_node_t *, lpm_pvt, node))->key_len;

    if (pivot->v6_key) {
        rv = _alpm_lib_trie_v6_ppg_prefix(lpm_pvt, pvt_len,
                (void *)prefix, len, cb, &cb_info);
    } else {
        rv = _alpm_lib_trie_ppg_prefix(lpm_pvt, pvt_len,
                (void *)prefix, len, cb, &cb_info);
    }

    return rv;
}

static int
alpm_ppg_assoc_data_cb(alpm_lib_trie_node_t *trie, alpm_lib_trie_bpm_cb_info_t *info)
{
    int rv = BCM_E_NONE, unit;
    int update_pvt = 0;
    uint32 pfx_len, rpl_bpm_len, pvt_bpm_len;

    _alpm_cb_t *acb;
    _alpm_ppg_cb_user_data_t *user_data;
    _alpm_ppg_data_t *ppg_data;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *pvt_def, *ppg_def;

    user_data = info->user_data;
    unit     = user_data->unit;
    ppg_data = user_data->ppg_data;
    acb      = user_data->ppg_acb;

    pvt_node    = (_alpm_pvt_node_t *)trie;
    pvt_bpm_len = PVT_BPM_LEN(pvt_node);
    pfx_len     = ppg_data->key_len;
    rpl_bpm_len = ppg_data->bpm_len;

    /* bypass MC (direct route) for ppg in case of mix UC & MC in same VRF */
    if (PVT_IS_IPMC(pvt_node)) {
        return rv;
    }

    if (user_data->ppg_op == ALPM_PPG_INSERT) {
        rpl_bpm_len = pfx_len;
    }

    /* Addr & Mask has been screened by propagate function,
     * no need to do it again */
    if (user_data->ppg_op == ALPM_PPG_INSERT) {
        if (pvt_bpm_len > pfx_len) {
            /* Not qualified and stop from traversing its child nodes */
            return SOC_E_LIMIT;
        }
    }

    if (user_data->ppg_op == ALPM_PPG_DELETE) {
        if (pvt_bpm_len != pfx_len) {
            /* Not qualified and stop from traversing its child nodes */
            return (pvt_bpm_len > pfx_len) ? SOC_E_LIMIT : SOC_E_NONE;
        }
    }

    /* Update bpm_length accordingly */
    PVT_BPM_LEN(pvt_node) = rpl_bpm_len;

    /* Update PVT_BKT_DEF for pvt_node and do PPG again */
    pvt_def = PVT_BKT_DEF(pvt_node);
    ppg_def = (_alpm_bkt_node_t *)ppg_data->bkt_def;
    if (pvt_def == NULL || ppg_def == NULL || pvt_def == ppg_def) {
        update_pvt = 1;
    } else {
        if (sal_memcmp(&pvt_def->adata, &ppg_def->adata,
                       sizeof(_alpm_bkt_adata_t)) != 0) {
            update_pvt = 1;
        }
    }

    PVT_BKT_DEF(pvt_node) = ppg_def;
    if (update_pvt) {
        rv = alpm_pvt_update_by_pvt_node(unit, acb, pvt_node,
                ALPM_PVT_UPDATE_ASSOC_DATA | ALPM_PVT_UPDATE_ALPM_DATA);
    }

    /* Update callback count */
    user_data->ppg_cnt++;

    return rv;
}

/* Software propagation */
int
alpm_ppg_op(int u, int ppg_op, void *cb, _alpm_ppg_data_t *ppg_data)
{
    int i, rv = BCM_E_NONE;
    int insert = 0;
    uint32 vrf_id = 0;
    uint32 ipt;
    int app;

    alpm_lib_trie_t *pvt_trie;
    _alpm_cb_t *acb;
    _alpm_ppg_cb_user_data_t user_data;

    switch (ppg_op) {
        case ALPM_PPG_INSERT:
            insert = 1;
            break;
        case ALPM_PPG_DELETE:
            insert = 0;
            break;
        case ALPM_PPG_HBP:
            insert = 0;
            break;
        default:
            return BCM_E_NONE;
    }

    vrf_id   = ppg_data->vrf_id;
    ipt      = ppg_data->ipt;
    app      = ppg_data->app;

    user_data.unit     = u;
    user_data.ppg_data = ppg_data;
    user_data.ppg_op   = ppg_op;
    user_data.ppg_cnt  = 0;

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);
        if (ACB_VRF_EMPTY(acb, vrf_id)) {
            continue;
        }
        user_data.ppg_acb  = acb;

        pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
        if (pvt_trie) {
            rv = alpm_ppg(u, pvt_trie, ppg_data->key, ppg_data->key_len,
                          insert, cb, &user_data);
        }
    }

    return rv;

}

int
alpm_ppg_assoc_data(int u, int ppg_op, _alpm_ppg_data_t *ppg_data)
{
    return alpm_ppg_op(u, ppg_op, alpm_ppg_assoc_data_cb, ppg_data);
}

/* Delete ALPM pvt trie only for Direct Routes */
int
alpm_pvt_trie_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 ipt = ALPM_LPM_IPT(u, lpm_cfg);
    alpm_lib_trie_t              *pvt_trie = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u, app), vrf_id, ipt);
    rv = alpm_lib_trie_delete(pvt_trie, lpm_cfg->user_data,
                     lpm_cfg->defip_sub_len, (alpm_lib_trie_node_t **)&pvt_node);
    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("Trie delete node failure\n"));
    }

    alpm_util_free(pvt_node);
    return rv;
}

/* Insert ALPM pvt trie only for Direct Routes which is needed in _tcam_match */
int
alpm_pvt_trie_insert(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    alpm_lib_trie_t              *pvt_trie = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    ALPM_ALLOC_EG(pvt_node, sizeof(_alpm_pvt_node_t), "pvt_node");
    PVT_BKT_VRF(pvt_node) = vrf_id;
    PVT_BKT_PKM(pvt_node) = ALPM_LPM_PKM(u, lpm_cfg);
    PVT_IDX(pvt_node)     = lpm_cfg->defip_index;
    /* Direct route pvt_node doesn't use bkt_trie, def_pfx & bkt_info */
    PVT_KEY_LEN(pvt_node) = lpm_cfg->defip_sub_len;
    PVT_KEY_CPY(pvt_node, lpm_cfg->user_data);
    PVT_FLAGS(pvt_node) = lpm_cfg->defip_flags;

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u, app), vrf_id, PVT_BKT_IPT(pvt_node));
    rv = alpm_lib_trie_insert(pvt_trie, lpm_cfg->user_data,
                              lpm_cfg->defip_sub_len, (alpm_lib_trie_node_t *)pvt_node);
    return rv;

bad:
    if (pvt_node != NULL) {
        alpm_util_free(pvt_node);
    }
    return rv;
}

/* ALPM pvt trie init only for Direct Routes */
int
alpm_pvt_trie_init(int u, int app, int vrf_id, int ipt)
{
    int         rv = BCM_E_NONE;
    uint32      max_key_len;
    _alpm_cb_t  *acb = ACB_TOP(u, app);

    max_key_len = alpm_util_trie_max_key_len(u, ipt);
    ALPM_IER(alpm_lib_trie_init(max_key_len, &ACB_PVT_TRIE(acb, vrf_id, ipt)));
    ACB_VRF_INIT_SET(u, acb, vrf_id, ipt);
    /* VRF DR partially inited: help to decide alpm_vrf_init
       for case of mix MC & UC, in alpm_cb_insert */
    ACB_DR_INIT_SET(u, acb, vrf_id, ipt);

    return rv;
}

static int
alpm_pvt_find_by_key(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                     uint32 *key, uint32 key_len, _alpm_pvt_node_t **pvt_node)
{
    int             rv = BCM_E_NONE;
    alpm_lib_trie_t          *pvt_trie;

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    rv = alpm_lib_trie_find_lpm(pvt_trie, key, key_len,
                       (alpm_lib_trie_node_t **)pvt_node);
    if (BCM_FAILURE(rv)) {
        ALPM_INFO(("**ACB(%d).PVT.FIND failed %d\n", ACB_IDX(acb), rv));
    }

    return rv;
}

/*
 * OUT: pvt_node
 */
static int
alpm_pvt_find(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
              _alpm_pvt_node_t **pvt_node)
{
    int rv;
    rv = alpm_pvt_find_by_key(u, acb, ALPM_LPM_VRF_ID(u, lpm_cfg),
                              ALPM_LPM_IPT(u, lpm_cfg), lpm_cfg->user_data,
                              lpm_cfg->defip_sub_len, pvt_node);
    return rv;
}

static int
alpm_pvt_insert(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = 0;

    if (ACB_HAS_TCAM(acb)) {
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg, ALPM_PVT_UPDATE_ALL);
    } else {
        rv = alpm_cb_insert(u, ACB_UPR(u, acb), lpm_cfg);
    }

    ALPM_INFO(("**ACB(%d).PVT.ADD rv %d\n", ACB_IDX(acb), rv));

    if (BCM_SUCCESS(rv)) {
        VRF_PVT_LEN_CNT_ADD(acb, ALPM_LPM_VRF_ID(u, lpm_cfg),
            ALPM_LPM_IPT(u, lpm_cfg), lpm_cfg->defip_sub_len);
    }

    return rv;
}

static int
alpm_pvt_delete(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = 0;

    if (ACB_HAS_TCAM(acb)) {
        rv = bcm_esw_alpm_tcam_delete(u, lpm_cfg);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("**ACB(%d).PVT.DEL tcam_delete 0x%x/%d vrf %d failed %d\n",
                      ACB_IDX(acb), lpm_cfg->defip_ip_addr,
                      lpm_cfg->defip_sub_len, lpm_cfg->defip_vrf, rv));
        }
    } else {
        rv = alpm_cb_delete(u, ACB_UPR(u, acb), lpm_cfg);
    }

    ALPM_INFO(("**ACB(%d).PVT.DEL rv %d\n", ACB_IDX(acb), rv));

    if (BCM_SUCCESS(rv)) {
        VRF_PVT_LEN_CNT_DEC(acb, ALPM_LPM_VRF_ID(u, lpm_cfg),
            ALPM_LPM_IPT(u, lpm_cfg), lpm_cfg->defip_sub_len);
    }

    return rv;
}

int
alpm_pvt_delete_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (PVT_BKT_TRIE(pvt_node) &&
        PVT_BKT_TRIE(pvt_node)->v6_key) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Encapsulate app info into lpm_cfg */
    lpm_cfg.defip_inter_flags |= alpm_app_flag_get(u, acb->app);

    /* Key & Length */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, PVT_KEY_LEN(pvt_node), &lpm_cfg);
    sal_memcpy(lpm_cfg.user_data, pvt_node->key, sizeof(pvt_node->key));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, PVT_BKT_VRF(pvt_node));

    /* Assoc-data */
    if (PVT_BKT_DEF(pvt_node) != NULL) {
        alpm_util_adata_trie_to_cfg(u, &PVT_BKT_DEF(pvt_node)->adata, &lpm_cfg);
    } else {
        lpm_cfg.default_miss = 1;
    }

    rv = alpm_pvt_delete(u, acb, &lpm_cfg);

    return rv;
}

/* delete newly created pivot from alpm_cb_path_construct if bucket is empty:
   unable to insert route in Level3 due to FULL */
static int
alpm_pvt_delete_by_empty_bkt(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int vrf_id, ipt;
    int bkt_empty;
    alpm_lib_trie_t *bkt_trie = NULL;
    alpm_lib_trie_node_t *tn = NULL;
    _alpm_pvt_node_t *pvt_node = lpm_cfg->pvt_node;

    if (ACB_HAS_TCAM(acb) || pvt_node == NULL) {
        return rv; /* no need to check */
    }

    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ipt    = ALPM_LPM_IPT(u, lpm_cfg);
    bkt_trie = PVT_BKT_TRIE(pvt_node);

    /* Delete pvt entry if bucket is empty */
    bkt_empty = (PVT_KEY_LEN(pvt_node) != 0 && bkt_trie->trie == NULL);
    if (!bkt_empty) {
        return rv; /* no need to delete pvt if bkt is not empty */
    }

    rv = alpm_pvt_delete_by_pvt_node(u, acb, pvt_node);
    ALPM_IER(rv);

    rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), pvt_node->key,
                     PVT_KEY_LEN(pvt_node), &tn);
    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("Trie delete node failure\n"));
    }

    (void)alpm_lib_trie_destroy(bkt_trie);
    alpm_util_free(pvt_node);

    return rv;
}

static int
alpm_pvt_update(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg, uint32 write_flags)
{
    int rv = BCM_E_NONE;

    _alpm_cb_t *upr_acb = NULL;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;

    if (ACB_HAS_TCAM(acb)) {
        /* Use insert routine to update */
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg, write_flags);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("**ACB(%d).PVT.UPD tcam_insert failed %d\n",
                      ACB_IDX(acb), rv));
        }
    } else {
        upr_acb = ACB_UPR(u, acb);
        rv = alpm_cb_find(u, upr_acb, lpm_cfg, &pvt_node, &bkt_node);
        if (BCM_SUCCESS(rv)) {
            rv = alpm_bkt_ent_write(u, upr_acb, pvt_node, lpm_cfg,
                                    bkt_node->ent_idx, write_flags);
        } else {
            ALPM_ERR(("pvt_update->alpm_cb_find: rv %d, ACB.%d pvt_node=%p, bkt_node=%p\n",
                      rv, ACB_IDX(upr_acb), pvt_node, bkt_node));
        }
    }

    ALPM_INFO(("**ACB(%d).PVT.UPD rv %d\n", ACB_IDX(acb), rv));

    return rv;
}

int
alpm_pvt_update_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, uint32 write_flags)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_UPDATE_SKIP) {
        return rv;
    }

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (PVT_BKT_TRIE(pvt_node) &&
        PVT_BKT_TRIE(pvt_node)->v6_key) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Encapsulate app info into lpm_cfg */
    lpm_cfg.defip_inter_flags |= alpm_app_flag_get(u, acb->app);

    /* Key & Length */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, PVT_KEY_LEN(pvt_node), &lpm_cfg);
    sal_memcpy(lpm_cfg.user_data, pvt_node->key, sizeof(pvt_node->key));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, PVT_BKT_VRF(pvt_node));

    /* Assoc-data */
    if (PVT_BKT_DEF(pvt_node) != NULL) {
        alpm_util_adata_trie_to_cfg(u, &PVT_BKT_DEF(pvt_node)->adata, &lpm_cfg);
    } else {
        lpm_cfg.default_miss = 1;
    }

    if (ACB_HAS_TCAM(acb)) {
        lpm_cfg.pvt_node = (uint32 *)pvt_node;
    } else {
        if (ACB_BKT_FIXED_FMT(acb, 1)) {
            lpm_cfg.bkt_kshift = 0;
        } else {
            lpm_cfg.bkt_kshift = PVT_KEY_LEN(pvt_node); /* for upr acb bkt */
        }
    }
    lpm_cfg.bkt_info = &PVT_BKT_INFO(pvt_node);

    rv = alpm_pvt_update(u, acb, &lpm_cfg, write_flags);

    return rv;
}

static int
alpm_bkt_find(int u, _alpm_cb_t *acb,
              _bcm_defip_cfg_t *lpm_cfg,
              _alpm_pvt_node_t *pvt_node,
              _alpm_bkt_node_t **bkt_node)
{
    int rv = BCM_E_NOT_FOUND;
    alpm_lib_trie_t *bkt_trie = PVT_BKT_TRIE(pvt_node);
    if (bkt_trie) {
        rv = alpm_lib_trie_search(bkt_trie, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                         (alpm_lib_trie_node_t **)bkt_node);
    }
    return rv;
}

int
alpm_bkt_ent_get_attempt(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                 _alpm_bkt_info_t *bkt_info, uint8 def_fmt,
                 int pfx_len, uint32 *ent_idx, int *fmt_update, int bkt_test, int *empty_bnk)
{
    /* Start with shorter prefix fmt */
    int i, rv = BCM_E_NONE;
    uint8 min_fmt = 0xff;
    int start, close, step;
    int bnkpb = BPC_BNK_PER_BKT(bp_conf);
    int rofs  = BI_ROFS(bkt_info);
    uint32 eidx = 0;

    /* Different bucket use different method for allocation, then after split,
     * bank distribution will be like this:
     bank 1:
        ----> Bank 0: FMT1[*_______________]
        ----> Bank 1: FMT3[**********______]
        ----> Bank 2: FMT3[**********______]
        ----> Bank 3: FMT3[**********______]
        ----> Bank 4: FMT3[**********______]
        ----> Bank 5: FMT3[________________]
        ----> Bank 6: FMT3[________________]
        ----> Bank 7: FMT3[________________]
     bank 2:
        ----> Bank 0: FMT1[________________]
        ----> Bank 1: FMT1[________________]
        ----> Bank 2: FMT1[________________]
        ----> Bank 3: FMT1[________________]
        ----> Bank 4: FMT3[**______________]
        ----> Bank 5: FMT3[**********______]
        ----> Bank 6: FMT3[**********______]
        ----> Bank 7: FMT3[**********______]
     */

    if (!BPC_BKT_WRA(bp_conf) || bkt_info->reverse == 0) {
        start = rofs;
        close = rofs + bnkpb;
        step  = 1;
    } else {
        start = rofs + bnkpb - 1;
        close = rofs - 1;
        step  = -1;
    }

    *empty_bnk = FALSE;
    /* First round is to get a bank that:
     *  with valid fmt type (assigned) AND
     *  proper prefix length AND
     *  bank is not full
     *
     *  go through each bank to find the most efficient fmt
 */
    for (i = start; ABS(i-close) > 0; i += step) {
        uint8 bnk_fmt = bkt_info->bnk_fmt[i % bnkpb];
        uint32 vet_bmp = bkt_info->vet_bmp[i % bnkpb];

        /* Ignore invalid fmt type */
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb) ||
            bnk_fmt > BPC_FMT_CNT(bp_conf)) {
            continue;
        }

        if (BPC_FMT_BNK(bp_conf, bnk_fmt) == INVALIDfmt) {
            continue;
        }

        /* Ignore full bank */
        if (vet_bmp == (1 << BPC_FMT_ENT_MAX(bp_conf, bnk_fmt)) - 1) {
            continue;
        }

        /* Ignore unfit fmt 1bit Length should be considered */
        if (!BPC_BKT_FIXED_FMT(bp_conf) &&
            pfx_len > BPC_FMT_PFX_LEN(bp_conf, bnk_fmt)) {
            continue;
        }

        if (bnk_fmt < min_fmt) {
            min_fmt = bnk_fmt;
            eidx = i % bnkpb;
        }
    }

    /* ent_idx format
     * -------------------------------
     * | ent idx | bnk idx | bkt idx |
     * -------------------------------
     */
    /* Now we find the bank, also need to find the empty slot in this bank */
    if (min_fmt != 0xff) {
        for (i = 0; i < BPC_FMT_ENT_MAX(bp_conf, min_fmt); i++) {
            if ((bkt_info->vet_bmp[eidx] & (1 << i)) == 0) {
                if (!bkt_test) {
                    if (bkt_info->vet_bmp[eidx] == 0) {
                        *empty_bnk = TRUE; /* check before change of vet_bmp */
                    }
                    bkt_info->bnk_fmt[eidx] = min_fmt;
                    bkt_info->vet_bmp[eidx] |= 1 << i;
                    *ent_idx = ALPM_IDX_MAKE(acb, bkt_info, eidx, i);
                }
                goto end;
            }
        }
    }

    if (BPC_BKT_FIXED_FMT(bp_conf)) {
        rv = BCM_E_FULL;
        goto end;
    }

    /* do second round if missed in the first round:
     *  bank fmt is not fit (e.g. prefix length is shorter) BUT
     *  this bank is empty, SO
     *  we change the bank fmt and make it available.
     */
    for (i = start; ABS(i-close) > 0; i += step) {
        uint8 fmt, bnk_fmt = bkt_info->bnk_fmt[i % bnkpb];
        uint32 vet_bmp = bkt_info->vet_bmp[i % bnkpb];

        /* Ignore invalid fmt type */
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb) ||
            bnk_fmt > BPC_FMT_CNT(bp_conf) ||
            vet_bmp != 0) {
            continue;
        }

        /* vet_bmp is empty && bnk_fmt is valid
         * from most efficient def_fmt (matches route mode) to least efficient fmt */
        for (fmt = def_fmt; fmt <= BPC_FMT_CNT(bp_conf); fmt++) {
            if (BPC_FMT_BNK(bp_conf, fmt) == INVALIDfmt) {
                continue;
            }
            if (pfx_len <= BPC_FMT_PFX_LEN(bp_conf, fmt)) {
                /* Entry 0 of bank i */
                if (!bkt_test) {
                    if (bkt_info->vet_bmp[i % bnkpb] == 0) {
                        *empty_bnk = TRUE; /* check before change of vet_bmp */
                    }

                    bkt_info->vet_bmp[i % bnkpb] |= 1;
                    bkt_info->bnk_fmt[i % bnkpb] = fmt;
                    *ent_idx = ALPM_IDX_MAKE(acb, bkt_info, i % bnkpb, 0);

                    if (bnk_fmt != fmt && fmt_update != NULL) {
                        *fmt_update = TRUE;
                    }
                }
                goto end;
            }
        }
    }

    rv = BCM_E_FULL;
end:
    if (rv == BCM_E_FULL) {
        ALPM_VERB(("**ACB(%d).ENT.get: failed (FULL)\n", ACB_IDX(acb)));
    }
    return rv;
}

int
alpm_bkt_ent_write(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node,
                   _bcm_defip_cfg_t *lpm_cfg, int ent_idx, uint32 write_flags)
{
    int rv;
    rv = ALPM_DRV(u)->alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, ent_idx, write_flags);
    return rv;
}

int
alpm_bkt_bnks_get(int u, int vrf_id, _alpm_cb_t *acb,
                  _alpm_bkt_info_t *bkt_info, int *count)
{
    int rv = BCM_E_NONE;
    int i, bnk_cnt = 0;

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        uint8 bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i) &&
            bnk_fmt <= ACB_FMT_CNT(acb, vrf_id)) {
            bnk_cnt ++;
        }
    }

    if (count) {
        *count = bnk_cnt;
    }

    return rv;
}

static int
alpm_bkt_ent_get(int u, int vrf_id, _alpm_cb_t *acb,
                 _alpm_pvt_node_t *pvt_node, int pfx_len,
                 uint32 *ent_idx, int *fmt_update, int bkt_test)
{
    int rv = BCM_E_NONE;
    uint8 def_fmt;
    _alpm_bkt_info_t *bkt_info = &PVT_BKT_INFO(pvt_node);
    int empty_bnk;

    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, PVT_BKT_IPT(pvt_node));
    rv = alpm_bkt_ent_get_attempt(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                          bkt_info, def_fmt, pfx_len,
                          ent_idx, fmt_update, bkt_test, &empty_bnk);
    if (rv == BCM_E_FULL) {
        int bnk_cnt = 0;
        /* If all banks are full, check if bucket expansion is required */
        alpm_bkt_bnks_get(u, vrf_id, acb, bkt_info, &bnk_cnt);
        if (ACB_BNK_PER_BKT(acb, vrf_id) > bnk_cnt) {
            if (bkt_test) {
                return BCM_E_NONE;
            }
            /* To do bucket expansion here */
            if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
                rv = alpm_cb_expand_in_bkt(u, acb, pvt_node, bnk_cnt);
            } else {
                rv = alpm_cb_expand(u, acb, pvt_node, bnk_cnt, fmt_update);
            }
            if (BCM_SUCCESS(rv)) {
                rv = alpm_bkt_ent_get_attempt(u, acb,
                        ACB_BKT_VRF_POOL(acb, vrf_id),
                        bkt_info, def_fmt, pfx_len,
                        ent_idx, fmt_update, bkt_test, &empty_bnk);
            }
        }
    }

    return rv;
}

static int
alpm_bkt_bnk_chk_rsvd(int u, int ipt, _alpm_bkt_pool_conf_t *bp_conf, int bnk)
{
    int rv = BCM_E_NONE;
    int used = 0;
    SHR_BITDCL *bnk_bmp;

    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);

    SHR_BITTEST_RANGE(bnk_bmp, bnk, 1, used);
    if (used) {
        rv = BCM_E_NOT_FOUND;
    } else {
        SHR_BITSET_RANGE(bnk_bmp, bnk, 1);
        BPC_BNK_USED(bp_conf, ipt) += 1;
    }

    return rv;
}

static int
alpm_bkt_bnk_alloc_reverse(int u, int ipt, _alpm_bkt_pool_conf_t *bp_conf,
                           int *bnk, int alloc_sz, int step)
{
    int rv = BCM_E_NONE;
    int i, bnk_cnt, bnk_end, used = 0;
    SHR_BITDCL *bnk_bmp;

    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    bnk_cnt = BPC_BNK_CNT(bp_conf);
    bnk_end = bnk_cnt - alloc_sz + 1;
    for (i = bnk_end - 1; i >= 0; i += step) {
        SHR_BITTEST_RANGE(bnk_bmp, i, alloc_sz, used);
        if (!used) {
            break;
        }
    }

    if (i < 0) { /* include *start_bnk is used when step=0xfffffff */
        rv = BCM_E_NOT_FOUND;
    } else {
        SHR_BITSET_RANGE(bnk_bmp, i, alloc_sz);
        BPC_BNK_USED(bp_conf, ipt) += alloc_sz;
        *bnk = i;
    }

    if (rv == BCM_E_NONE) {
        ALPM_INFO(("bnk alloc start %d sz %d\n", *bnk, alloc_sz));
    }

    return rv;
}

static int
alpm_bkt_bnk_alloc(int u, int ipt, _alpm_bkt_pool_conf_t *bp_conf,
                   int *bnk, int alloc_sz, int step)
{
    int rv = BCM_E_NONE;
    int i, bnk_cnt, bnk_end, used = 0;
    SHR_BITDCL *bnk_bmp;

    if (step < 0) {
        return alpm_bkt_bnk_alloc_reverse(u, ipt, bp_conf, bnk, alloc_sz, step);
    }

    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    bnk_cnt = BPC_BNK_CNT(bp_conf);
    bnk_end = bnk_cnt - alloc_sz + 1;
    i = bnk_end;
    if (step == BPC_BNK_PER_BKT(bp_conf)) {
        for (i = 0; i < bnk_end; i += step) {
            SHR_BITTEST_RANGE(bnk_bmp, i, step, used);
            if (!used) {
                break;
            }
        }
    }

    if (i >= bnk_end) {
        for (i = 0; i < bnk_end; i += step) {
            SHR_BITTEST_RANGE(bnk_bmp, i, alloc_sz, used);
            if (!used) {
                break;
            }
        }
    }

    if (i >= bnk_end) { /* include *start_bnk is used when step=0xfffffff */
        rv = BCM_E_NOT_FOUND;
    } else {
        SHR_BITSET_RANGE(bnk_bmp, i, alloc_sz);
        BPC_BNK_USED(bp_conf, ipt) += alloc_sz;
        *bnk = i;
    }

    if (rv == BCM_E_NONE) {
        ALPM_INFO(("bnk alloc start %d sz %d\n", *bnk, alloc_sz));
    }

    return rv;
}

void
alpm_bkt_bnk_free(int u, int ipt, _alpm_cb_t *acb,
                  _alpm_bkt_pool_conf_t *bp_conf,
                  _alpm_bkt_info_t *bkt_info, int bnk)
{
    int gbl_bnk, bkt_bnk, tab_idx;
    int used;
    SHR_BITDCL *bnk_bmp;

    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    bkt_bnk = BPC_BNK_PER_BKT(bp_conf);
    gbl_bnk = BI_BKT_IDX(bkt_info) * bkt_bnk + bnk;
    if (BI_ROFS(bkt_info) > bnk) {
        gbl_bnk += bkt_bnk;
    }

    SHR_BITCLR(bnk_bmp, gbl_bnk);
    BPC_BNK_USED(bp_conf, ipt) -= 1;

    if (ALPM_BKT_SHARE_THRES(u)) {
        /* TD3: Dec BKT_USED after bnk_bmp clear if no more used bits in the bkt */
        SHR_BITTEST_RANGE(bnk_bmp, (gbl_bnk - gbl_bnk % bkt_bnk), bkt_bnk, used);
        if (!used) {
            BPC_BKT_USED(bp_conf, ipt) --;
        }
    }

    tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, bnk);
    if (bp_conf->pvt_ptr != NULL) {
        bp_conf->pvt_ptr[tab_idx] = NULL;
    }

    /* Suppose to update bkt_info->rofs when bnk is freed */
    if (BPC_BKT_WRA(bp_conf) && bnk == BI_ROFS(bkt_info) &&
        (gbl_bnk < (BPC_BNK_CNT(bp_conf) - 1))) {
        int new_rofs = (bnk + 1) % bkt_bnk;
        if (new_rofs < BI_ROFS(bkt_info)) {
            BI_BKT_IDX(bkt_info)++;
        }
        BI_ROFS(bkt_info) = new_rofs;
    }

    BI_BNK_FREE(bkt_info, bnk);

    ALPM_INFO(("bnk  free bnk %d \n", gbl_bnk));

    acb->acb_cnt.c_bnkfree++;

    return;
}

void
alpm_bkt_bnk_shrink(int u, int ipt, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                    _alpm_bkt_info_t *bkt_info, int *fmt_update)
{
    int i, bnkpb, bnk;
    int bnks, bnke;

    bnkpb = BPC_BNK_PER_BKT(bp_conf);

    /* Store bnks and bnke because bkt_bnk_free could change rofs value */
    bnks  = BI_ROFS(bkt_info);
    bnke  = bnkpb + BI_ROFS(bkt_info);

    for (i = bnks; i < bnke; i++) {
        bnk = i % bnkpb;
        if (BI_BNK_IS_USED(bkt_info, bnk) && bkt_info->vet_bmp[bnk] == 0) {
            /* Release bank bnk */
            ALPM_INFO(("**ACB(%d).BNK.RLS: RBBE (%d %d %d *)\n",
                       ACB_IDX(acb), -1, BI_BKT_IDX(bkt_info), bnk));

            if (fmt_update != NULL) {
                *fmt_update = 1;
            }
            alpm_bkt_bnk_free(u, ipt, acb, bp_conf, bkt_info, bnk);
        }
    }

    acb->acb_cnt.c_bnkshrk++;

    return;
}

/* _alpm_bkt_alloc_full to make sure a entire bucket
 * is allocated or return FULL */
static int
alpm_bnk_alloc_attempt(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                       int req_bnk_cnt, int step, _alpm_bkt_info_t *bkt_info,
                       int *avl_bnk_cnt)
{
    int rv;
    int i, bnk = 0, bnkpb;

    rv = alpm_bkt_bnk_alloc(u, ipt, ACB_BKT_VRF_POOL(acb, vrf_id), &bnk,
                            req_bnk_cnt, bkt_info->reverse ? -1 : step);
    if (BCM_SUCCESS(rv)) {
        bnkpb = ACB_BNK_PER_BKT(acb, vrf_id);
        BI_ROFS(bkt_info) = bnk % bnkpb;
        BI_BKT_IDX(bkt_info) = bnk / bnkpb;
        sal_memset(bkt_info->vet_bmp, 0, sizeof(bkt_info->vet_bmp));
        sal_memset(bkt_info->bnk_fmt, 0, sizeof(bkt_info->bnk_fmt));
        for (i = 0; i < req_bnk_cnt; i++) {
            bkt_info->bnk_fmt[(BI_ROFS(bkt_info) + i) % bnkpb]
                = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
        }
    }

    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_FULL;
    }

    return rv;
}

int
alpm_bkt_bnk_nearest_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                         int left_bnk, int right_bnk, int *nst_bnk)
{
    int i, rv = BCM_E_NONE;
    int nst_left, nst_right;

    nst_left  = -1;
    nst_right = bnk_sz;

    if (left_bnk >= 0) {
        for (i = left_bnk; i >= 0; i--) {
            if (!SHR_BITGET(bnk_bmp, i)) {  /* TBD performance */
                break;
            }
        }
        nst_left = i;
    }

    if (right_bnk < bnk_sz) {
        for (i = right_bnk; i < bnk_sz; i++) {
            if (!SHR_BITGET(bnk_bmp, i)) {  /* TBD performance */
                break;
            }
        }
        nst_right = i;
    }

    if (nst_left >= 0 && nst_right < bnk_sz) {
        if ((left_bnk - nst_left) > (nst_right - right_bnk)) {
            *nst_bnk = nst_right;
        } else {
            *nst_bnk = nst_left;
        }
    } else if (nst_left >= 0) {
        *nst_bnk = nst_left;
    } else if (nst_right < bnk_sz) {
        *nst_bnk = nst_right;
    } else {
        rv = BCM_E_FULL;
    }

    return rv;
}

static int
alpm_bkt_pfx_ent_idx_update(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                            _alpm_bkt_info_t *src_bkt, int src_bnk,
                            _alpm_bkt_info_t *dst_bkt, int dst_bnk)
{
    int i, rv = BCM_E_UNAVAIL;
    int sb = src_bnk;
    int db = dst_bnk;
    uint32 bak_idx;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;
    alpm_lib_trie_t      *bkt_trie = NULL;

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = alpm_lib_trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       trieTraverseOrderIn, TRUE);
    ALPM_IEG(rv);
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        int ent, bnk, bkt;

        bkt_node = pfx_arr->pfx[i];
        ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
        bnk = ALPM_IDX_TO_BNK(acb, bkt_node->ent_idx);
        bkt = ALPM_IDX_TO_BKT(acb, bkt_node->ent_idx);
        if (bnk < PVT_ROFS(pvt_node)) {
            bkt -= 1;
        }

        if (src_bnk == -1) {
            sb = bnk;
            db = bnk;
        }

        bak_idx = bkt_node->ent_idx;
        if (bnk == sb && bkt == BI_BKT_IDX(src_bkt)) {
            bkt_node->ent_idx = ALPM_IDX_MAKE(acb, dst_bkt, db, ent);
            if (ALPM_TCAM_HB_IS_PTR(u) && bak_idx != bkt_node->ent_idx) {
                ALPM_IEG(alpm_ppg_hitbit(u, acb->app, PVT_BKT_VRF(pvt_node),
                            PVT_BKT_IPT(pvt_node), bkt_node->key, bkt_node->key_len));
            }
        }
    }

bad:
    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }
    return rv;

}

static int
alpm_bkt_bnk_copy(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                  _alpm_bkt_info_t *src_bkt, int src_bnk,
                  _alpm_bkt_info_t *dst_bkt, int dst_bnk)
{
    int rv = BCM_E_UNAVAIL;
    int src_idx, dst_idx;

    src_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, src_bkt, src_bnk);
    dst_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, dst_bnk);
    rv = ALPM_DRV(u)->alpm_bkt_bnk_copy(u, acb, pvt_node, src_bkt, dst_bkt,
                                        src_idx, dst_idx);
    if (BCM_SUCCESS(rv)) {
        /* Performance improvement ALPMTBD in pfx_ent_idx_update */
        rv = alpm_bkt_pfx_ent_idx_update(u, acb, pvt_node, src_bkt, src_bnk,
                                         dst_bkt, dst_bnk);
    }

    return rv;
}

/* Receive a new bank 'to_bnk' on one side and release old bank on
 * another side
 *
 * When we reach here, to_bnk should be neighbour of pvt_node's bkt_info.
 * After moving is done, free bank to free_bnk.
 */
static int
alpm_bkt_bnk_ripple(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node, int to_bnk, int *free_bnk)
{
    int i, ipt, rv = BCM_E_NONE;
    int bnkpb, start_bnk, end_bnk, mv_bnk;
    int mb, tb, tab_idx = 0;
    uint8  bnk_fmt;
    uint32 vet_bmp;
    _alpm_bkt_info_t *bkt_info, dst_bkt;
    int vrf_id;

    vrf_id = PVT_BKT_VRF(pvt_node);
    ipt = PVT_BKT_IPT(pvt_node);
    bkt_info = &PVT_BKT_INFO(pvt_node);
    bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));
    start_bnk = PVT_ROFS(pvt_node);
    /* Find end bank */
    for (i = 1; i < bnkpb; i++) {
        if (!BI_BNK_IS_USED(bkt_info, start_bnk % bnkpb)) {
            start_bnk++;
            i--;
            continue;
        }
        if (!BI_BNK_IS_USED(bkt_info, (start_bnk+i) % bnkpb)) {
            break;
        }
    }
    start_bnk = PVT_BKT_IDX(pvt_node) * bnkpb + start_bnk;
    end_bnk = start_bnk + i - 1;

    /* 1. copy bank on the other side to to_bnk */
    /* S: start, E: end, T: to_bnk
     -----------------     -----------------
     | |#|#|#|#|#| | |  -> |#|#|#|#|#| | | |
     -----------------     -----------------
      ^ ^       ^                     ^
      T S       E                     T
     -----------------     -----------------
     | |#|#|#|#|#| | |  -> | | |#|#|#|#|#| |
     -----------------     -----------------
        ^       ^ ^           ^
        S       E T           T
     */
    if (to_bnk < start_bnk) {
        mv_bnk = end_bnk;
        /* Adjust start_bnk */
        start_bnk = to_bnk;
    } else {
        mv_bnk = start_bnk;

        /* Adjust start_bnk */
        start_bnk++;
        while (start_bnk < end_bnk) {
            if (!BI_BNK_IS_USED(bkt_info, start_bnk % bnkpb)) {
                start_bnk++;
            } else {
                break;
            }
        }
    }
    mb = mv_bnk % bnkpb;
    tb = to_bnk % bnkpb;

    /* Save src_idx before rofs, bkt-idx changes */
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, mb);
    }

    sal_memset(&dst_bkt, 0, sizeof(dst_bkt));
    BI_ROFS(&dst_bkt)    = start_bnk % bnkpb;
    BI_BKT_IDX(&dst_bkt) = start_bnk / bnkpb;

    ALPM_IER(alpm_bkt_bnk_copy(u, acb, pvt_node, bkt_info, mb,
                                &dst_bkt, tb));
    /* 2. Update bkt_info info */
    bnk_fmt = bkt_info->bnk_fmt[mb];
    vet_bmp = bkt_info->vet_bmp[mb];
    ALPM_IER(alpm_bkt_bnk_chk_rsvd(u, ipt, ACB_BKT_VRF_POOL(acb,
             PVT_BKT_VRF(pvt_node)), to_bnk));
    /* bkt_info could be changed here includes:
     * rofs, bkt-idx, bnk_fmt, so we need to save those values.
     */
    alpm_bkt_bnk_free(u, ipt, acb, ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)), bkt_info, mb);

    PVT_ROFS(pvt_node)    = BI_ROFS(&dst_bkt);
    PVT_BKT_IDX(pvt_node) = BI_BKT_IDX(&dst_bkt);
    bkt_info->bnk_fmt[tb] = bnk_fmt;
    bkt_info->vet_bmp[tb] = vet_bmp;

    /* 3. Update PVT */
    if (pvt_node != NULL) {
        rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node, ALPM_PVT_UPDATE_ALPM_DATA);
        ALPM_IER(rv);
    }

    /* 4. invalidate old bnk */
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        ALPM_IER(alpm_bkt_bnk_clear(u, acb, pvt_node, tab_idx));
    }

    if (tb != mb) {
        bkt_info->bnk_fmt[mb] = 0;
        bkt_info->vet_bmp[mb] = 0;
    }

    if (free_bnk != NULL) {
        *free_bnk = mv_bnk;
    }

    acb->acb_cnt.c_ripple++;

    return rv;
}

/* alpm_bkt_move requires at least one used bank to move (src & dst).
   Zero used bank causes infinite while loop (e.g., no Level 3 route
   for initial default pivot in parallel mode) */
int
alpm_bkt_move(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
              _alpm_bkt_info_t *src_bkt, _alpm_bkt_info_t *dst_bkt)
{
    int rv = BCM_E_NONE;
    int bnkpb, sbnk, dbnk;
    int lsbnk, ldbnk; /* Local src/dst bnk */
    _alpm_bkt_info_t tmp_bkt;
    int vrf_id;

    vrf_id = PVT_BKT_VRF(pvt_node);
    bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));
    dbnk  = BI_ROFS(dst_bkt);
    ldbnk = dbnk % bnkpb;
    while (!BI_BNK_IS_USED(dst_bkt, ldbnk)) {
        dbnk ++;
        ldbnk = dbnk % bnkpb;
    }

    /* Copy */
    for (sbnk = src_bkt->rofs; sbnk < src_bkt->rofs + bnkpb; sbnk++) {
        lsbnk = sbnk % bnkpb;
        ldbnk = dbnk % bnkpb;
        if (!BI_BNK_IS_USED(src_bkt, lsbnk)) {
            continue;
        }

        dst_bkt->bnk_fmt[ldbnk] = src_bkt->bnk_fmt[lsbnk];
        dst_bkt->vet_bmp[ldbnk] = src_bkt->vet_bmp[lsbnk];

        ALPM_IER(alpm_bkt_bnk_copy(u, acb, pvt_node, src_bkt, lsbnk,
                                   dst_bkt, ldbnk));

        dbnk++;
        ldbnk = dbnk % bnkpb;
        while (!BI_BNK_IS_USED(dst_bkt, ldbnk)) {
            dbnk++;
            ldbnk = dbnk % bnkpb;
        }
    }

    /* swap src_bkt <-> dst_bkt info */
    sal_memcpy(&tmp_bkt, src_bkt, sizeof(_alpm_bkt_info_t));
    sal_memcpy(src_bkt, dst_bkt, sizeof(_alpm_bkt_info_t));
    sal_memcpy(dst_bkt, &tmp_bkt, sizeof(_alpm_bkt_info_t));

    if (pvt_node != NULL) {
        rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node, ALPM_PVT_UPDATE_ALPM_DATA);
        ALPM_IER(rv);
    }

    /* Invalidate src bucket */
    for (sbnk = 0; sbnk < bnkpb; sbnk++) {
        if (!BI_BNK_IS_USED(dst_bkt, sbnk)) {
            continue;
        }

        if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
            ALPM_IER(alpm_bkt_bnk_clear(u, acb, pvt_node,
                    ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, sbnk)));
        }
        dst_bkt->vet_bmp[sbnk] = 0;
    }

    /* Release bnk to global pool */
    alpm_bkt_bnk_shrink(u, PVT_BKT_IPT(pvt_node), acb,
        ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)), dst_bkt, NULL);

    return rv;
}

/*
 *
  0 1 2 3 4 5 6 7
 -----------------
 | |#|#|#|#|#|*|*|
 -----------------
  ^   --->  ^
 from       to

 -----------------
 | |*|*|*|#|#|#| |
 -----------------
          ^ <-- ^
          to    from

 * PLEASE NOTICE THAT from_bnk AND to_bnk INDICATE FREE BANK LOCATION,
 * NOT USED BANK LOCATION.
 */
static int
alpm_bkt_free_bnk_move(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                       int from_bnk, int to_bnk, int ipt)
{
    int mv_bnk, step;
    _alpm_pvt_node_t *pvt_node = NULL;
    int tab_idx, free_bnk = -1, bnkpb;
    int rv = BCM_E_NONE;
    SHR_BITDCL *bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);

    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    step = from_bnk < to_bnk ? 1 : -1;
    mv_bnk = from_bnk + step;
    while (ABS(mv_bnk - to_bnk) >= 0) {
        tab_idx = ALPM_TAB_IDX_GET_BKT_BNK(acb, 0,
                    mv_bnk/bnkpb, mv_bnk%bnkpb);
        if (bp_conf->pvt_ptr &&
            bp_conf->pvt_ptr[tab_idx] == NULL) {
            /* Skip currently reserved bnks (pending to use
               in later steps such as a defrag process) */
            if (!SHR_BITGET(bnk_bmp, mv_bnk)) {
                from_bnk = mv_bnk;
            }
            mv_bnk += step;
            continue;
        }

        if (bp_conf->pvt_ptr != NULL) {
            pvt_node = (_alpm_pvt_node_t *)bp_conf->pvt_ptr[tab_idx];
            rv = alpm_bkt_bnk_ripple(u, acb, pvt_node, from_bnk, &free_bnk);
            ALPM_IEG(rv);
            if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
                rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "FreeBnkMove");
                ALPM_IEG(rv);
            }
        }

        if (free_bnk == to_bnk) {
            break;
        } else {
            from_bnk = free_bnk;
            mv_bnk = from_bnk + step;
        }
    }

bad:
    return rv;
}

/*
 * To find a minimum range which contains req_cnt free banks
 *
 * min_left and min_right are return value indicate the range.
 */
int
alpm_bkt_free_bnk_range_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                            int req_cnt, int *rng_left, int *rng_right,
                            int *rng_mid)
{
    int i, rv = BCM_E_NONE;
    int alloc_sz, idx, free_cnt = 0;
    int *free_bnk_arr = NULL;

    int min_idx = -1, min_range;

    alloc_sz = bnk_sz * sizeof(int);
    ALPM_ALLOC_EG(free_bnk_arr, alloc_sz, "free_bnk_arr");
    /*
     -------------------------------------------------------------
     | |#|#| | | |#|#|#|#| | |#|#| | |#|#| |#| |#| |#|#|#|#| | |#|
     -------------------------------------------------------------
      ^                                                         ^
      0                                                        bnk_sz-1
     */
    min_range = bnk_sz;
    for (i = 0; i < bnk_sz; i++) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            free_bnk_arr[free_cnt] = i;
            idx = free_cnt - (req_cnt - 1);
            if (idx >= 0) {
                if (min_range > (free_bnk_arr[free_cnt] - free_bnk_arr[idx])) {
                    min_range = (free_bnk_arr[free_cnt] - free_bnk_arr[idx]);
                    min_idx = free_cnt;
                }
            }
            free_cnt ++;
        }
    }

    if (min_idx != -1) {
        *rng_left = free_bnk_arr[min_idx - (req_cnt - 1)];
        *rng_right = free_bnk_arr[min_idx];
        *rng_mid = free_bnk_arr[min_idx - (req_cnt - 1) / 2];
    } else {
        rv = BCM_E_FULL;
    }

bad:
    if (free_bnk_arr != NULL) {
        alpm_util_free(free_bnk_arr);
    }
    return rv;
}

void
alpm_bkt_free(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
              _alpm_bkt_info_t *bkt_info)
{
    int bnk;
    _alpm_bkt_pool_conf_t *bp_conf;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    for (bnk = 0; bnk < ALPM_BPB_MAX; bnk++) {
        if (BI_BNK_IS_USED(bkt_info, bnk)) {
            alpm_bkt_bnk_free(u, ipt, acb, bp_conf, bkt_info, bnk);
        }
    }
    bkt_info->reverse = 0;

    return;
}

/* OUT: bkt_info */
int
alpm_bnk_alloc(int u, _alpm_cb_t *acb, int vrf_id, int ipt, int req_bnk_cnt,
               _alpm_bkt_info_t *bkt_info, int *avl_bnk_cnt)
{
    int rv = BCM_E_NONE;
    rv = alpm_bnk_alloc_attempt(u, acb, vrf_id, ipt, req_bnk_cnt,
                                ACB_BNK_PER_BKT(acb, vrf_id), bkt_info,
                                avl_bnk_cnt);
    if (rv != BCM_E_FULL) {
        goto end;
    }

    /* rv == BCM_E_FULL
     *
     * No free bkt available, start defragment process to create a
     * new available bucket.
     * It's also bucket sharing process for TH/TD3
     */

    rv = alpm_cb_defrag(u, acb, vrf_id, ipt);
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bnk_alloc_attempt(u, acb, vrf_id, ipt, req_bnk_cnt,
                                    1, bkt_info, avl_bnk_cnt);
    }

end:
    return rv;
}

int
alpm_bkt_insert(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg, _alpm_pvt_node_t *pvt_node,
                uint32 *ent_idx)
{
    int rv = BCM_E_NONE;
    int fmt_update = 0;
    int vrf_id = 0;
    int pfx_len;

    vrf_id = PVT_BKT_VRF(pvt_node);
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        pfx_len = lpm_cfg->defip_sub_len;
    } else {
        pfx_len = lpm_cfg->defip_sub_len - PVT_KEY_LEN(pvt_node);
    }

    rv = alpm_bkt_ent_get(u, vrf_id, acb, pvt_node, pfx_len, ent_idx,
                          &fmt_update, 0);
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, *ent_idx, ALPM_PVT_UPDATE_ALL);
        if (BCM_SUCCESS(rv) && fmt_update && !ACB_BKT_FIXED_FMT(acb, vrf_id)) {
            _alpm_tcam_write_t *tcam_write = lpm_cfg->tcam_write;

            if (tcam_write && (tcam_write->hw_idx >= 0)) {
                int rv1, pkm, ipt;
                ipt = PVT_BKT_IPT(pvt_node);
                pkm = ALPM_PKM_RECOVER(u, ipt);
                rv1 = _tcam_entry_write(u, acb->app, pkm, NULL, tcam_write->ent,
                                        tcam_write->hw_idx,
                                        tcam_write->hw_idx,
                                        FALSE);
                ALPM_INFO(("Write to TCAM HW at hw_idx %d by pvt update (%d)\n",
                           tcam_write->hw_idx, rv1));
                tcam_write->hw_idx = -1; /* avoid write again */
            }

            rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node, ALPM_PVT_UPDATE_ALPM_DATA);
        }
        if (BCM_FAILURE(rv)) {
            /* rv = alpm_bkt_ent_del(u, acb, pvt_node, *ent_idx); */
        }
    }

    if (BCM_FAILURE(rv)) {
        ALPM_VERB(("**ACB(%d).BKT.INS: failed %d\n", ACB_IDX(acb), rv));
    }

    return rv;
}

int
alpm_bkt_pfx_idx_backup(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 **bak_idx)
{
    int i, rv = BCM_E_NONE;
    int count = pfx_arr->pfx_cnt;
    uint32 *pfx_idx;

    ALPM_ALLOC_EG(pfx_idx, count * sizeof(uint32), "bak_idx");

    for (i = 0; i < count; i++) {
        pfx_idx[i] = pfx_arr->pfx[i]->ent_idx;
    }

    if (bak_idx != NULL) {
        *bak_idx = pfx_idx;
    }

bad:
    return rv;
}

void
alpm_bkt_pfx_idx_restore(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 *bak_idx)
{
    int i;
    uint32 tmp_idx;
    int count = pfx_arr->pfx_cnt;

    for (i = 0; i < count; i++) {
        tmp_idx = pfx_arr->pfx[i]->ent_idx;
        pfx_arr->pfx[i]->ent_idx = bak_idx[i];
        bak_idx[i] = tmp_idx;
    }

    return ;
}

int
alpm_bkt_pfx_merge(int u, _alpm_cb_t *acb,
                  _alpm_bkt_pfx_arr_t *pfx_arr,
                  _alpm_pvt_node_t *pvt_node,
                  _alpm_bkt_info_t *new_bkt)
{
    int i;
    int rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->alpm_bkt_pfx_copy(u, acb, pfx_arr, pvt_node,
                                        pvt_node, new_bkt);
    ALPM_IER(rv);

    if (!ALPM_TCAM_HB_IS_PTR(u)) {
        return rv;
    }

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        ALPM_IER(alpm_ppg_hitbit(u, acb->app, PVT_BKT_VRF(pvt_node), PVT_BKT_IPT(pvt_node),
                              pfx_arr->pfx[i]->key, pfx_arr->pfx[i]->key_len));
    }

    return rv;
}

int
alpm_bkt_pfx_copy(int u, _alpm_cb_t *acb,
                  _alpm_bkt_pfx_arr_t *pfx_arr,
                  _alpm_pvt_node_t *opvt_node,
                  _alpm_pvt_node_t *npvt_node)
{
    int i;
    int rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->alpm_bkt_pfx_copy(u, acb, pfx_arr, opvt_node,
                                        npvt_node, NULL);
    ALPM_IER(rv);

    if (!ALPM_TCAM_HB_IS_PTR(u)) {
        return rv;
    }

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        ALPM_IER(alpm_ppg_hitbit(u, acb->app, PVT_BKT_VRF(opvt_node), PVT_BKT_IPT(opvt_node),
                              pfx_arr->pfx[i]->key, pfx_arr->pfx[i]->key_len));
    }

    return rv;
}

int
alpm_bkt_pfx_clean(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node,
                   int count, uint32 *ent_idx)
{
    int rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->alpm_bkt_pfx_clean(u, acb, pvt_node, count, ent_idx);

    return rv;
}

int
alpm_bkt_pfx_shrink(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, void *user_data)
{
    int         i, rv = BCM_E_NONE;
    uint32      *bak_idx = NULL;
    alpm_lib_trie_t      *bkt_trie = NULL;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_info_t *bkt_info = NULL;
    int         shrink_empty_only;
    int         from_bkt_split = 0;
    int         used_bnks = 0;
    int         bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));
    int         nbkt_release = 0;
    _alpm_bkt_info_t nbkt_info, obkt_info;
    _alpm_bkt_info_t *src_bkt = &obkt_info;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    shrink_empty_only = *(int *)user_data;
    from_bkt_split = (shrink_empty_only == 2);
    if (shrink_empty_only) {
        int tb, tbnk;
        int has_empty_bnk = 0;

        for (tb = BI_ROFS(bkt_info); tb < BI_ROFS(bkt_info) + bnkpb; tb++) {
            tbnk = tb % bnkpb;
            if (!BI_BNK_IS_USED(bkt_info, tbnk)) {
                continue;
            }
    	    if (bkt_info->vet_bmp[tbnk] == 0) {
                has_empty_bnk = 1;
            } else {
                used_bnks++; /* occupied bnks count */
            }
        }
        if (!has_empty_bnk) {
            if (from_bkt_split) {
                shrink_empty_only = 0;
            } else {
                return rv;
            }
        }
    }

    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       trieTraverseOrderIn, TRUE);
    ALPM_IEG(rv);
    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);

    if (!ACB_BKT_FIXED_FMT(acb, PVT_BKT_VRF(pvt_node))) {
        if (shrink_empty_only) { /* has empty bnks */
            /* Disable "empty" bnks (bnk_fmt!=0) by update pivot
               due to vet_bmp=0 before move in from other bnks */
            uint8 bnk_fmt_bak[ALPM_BPB_MAX];
            sal_memcpy(bnk_fmt_bak, bkt_info->bnk_fmt, sizeof(bnk_fmt_bak));
            for (i = 0; i < bnkpb; i++) {
                if (bkt_info->vet_bmp[i] == 0) {
                    bkt_info->bnk_fmt[i] = 0;
                }
            }
            rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node,
                                             ALPM_PVT_UPDATE_ALPM_DATA);
            ALPM_IEG(rv);
            /* restore original bnk_fmt */
            sal_memcpy(bkt_info->bnk_fmt, bnk_fmt_bak, sizeof(bnk_fmt_bak));

        } else { /* all occupied bnks */
            if (used_bnks > 1) {
                int req_bnks, sbnk;

                /* Check bkt pfx merge (sort) in a new bkt with less bnks */
                sal_memset(&nbkt_info, 0, sizeof(_alpm_bkt_info_t));
                rv = alpm_bkt_pfx_merge_check_and_alloc(u, acb, pvt_node, pfx_arr,
                                              &nbkt_info, used_bnks - 1, &req_bnks);
                if (BCM_SUCCESS(rv)) {
                    /* for release new allocated bkt if fails */
                    nbkt_release = 1;

                    /* for !FIXED_FMT:
                     * Need to sort pfx_arr here, node with longer prefix length comes first,
                     * Should let longer bkt node to choose bkt_ent_get first
                     */
                    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
                        _shr_sort(pfx_arr->pfx, pfx_arr->pfx_cnt,
                                  sizeof(_alpm_bkt_node_t *), alpm_util_pfx_len_cmp);
                    }
                    /* Merge entries from old bucket to new bucket */
                    rv = alpm_bkt_pfx_merge(u, acb, pfx_arr,
                                            pvt_node, &nbkt_info);
                    ALPM_IEG(rv);

                    /* Save obkt_info, replace pvt_node with nbkt_info */
                    sal_memcpy(src_bkt, bkt_info, sizeof(_alpm_bkt_info_t));
                    sal_memcpy(bkt_info, &nbkt_info, sizeof(_alpm_bkt_info_t));
                    nbkt_release = 2; /* after pvt_node swap obkt to nbkt */

                    /* Update pivot with new bucket after merge */
                    rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node,
                                                     ALPM_PVT_UPDATE_ALPM_DATA);
                    ALPM_IEG(rv);

                    /* Invalidate old src bucket */
                    for (sbnk = 0; sbnk < bnkpb; sbnk++) {
                        if (!BI_BNK_IS_USED(src_bkt, sbnk)) {
                            continue;
                        }
                        src_bkt->vet_bmp[sbnk] = 0;
                    }

                    /* Release old bnk to global pool */
                    alpm_bkt_bnk_shrink(u, PVT_BKT_IPT(pvt_node), acb,
                        ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)), src_bkt, NULL);

                    nbkt_release = 0; 

                } else {
                    if (rv == BCM_E_FULL) { /* FULL is OK, no merge-shrink needed */
                        rv = BCM_E_NONE;
                    } else {
                        ALPM_IEG(rv);
                    }
                }
            }
            goto bad; /* End of merge-shrink */
        }
    }

    rv = ALPM_DRV(u)->alpm_bkt_pfx_shrink(u, acb, pvt_node, pfx_arr,
                                          shrink_empty_only);

    if (BCM_SUCCESS(rv)) {
        int fmt_update = 0;
        alpm_bkt_bnk_shrink(u, PVT_BKT_IPT(pvt_node), acb,
                ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                bkt_info, &fmt_update);
        if (fmt_update && !ACB_BKT_FIXED_FMT(acb, PVT_BKT_VRF(pvt_node))) {
            rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node, ALPM_PVT_UPDATE_ALPM_DATA);
            ALPM_IEG(rv);
        }
    }

    if (ALPM_TCAM_HB_IS_PTR(u)) {
        for (i = 0; i < pfx_arr->pfx_cnt; i++) {
            if (bak_idx[i] == pfx_arr->pfx[i]->ent_idx) {
                continue;
            }
            rv = alpm_ppg_hitbit(u, acb->app, PVT_BKT_VRF(pvt_node), PVT_BKT_IPT(pvt_node),
                                 pfx_arr->pfx[i]->key, pfx_arr->pfx[i]->key_len);
            ALPM_IEG(rv);
        }
    }

bad:
    
    if (nbkt_release) {
        if (nbkt_release == 2) {
            /* pvt_node->bkt_info restore from saved obkt_info */
            sal_memcpy(bkt_info, src_bkt, sizeof(_alpm_bkt_info_t));
        }
        alpm_bkt_free(u, acb, PVT_BKT_VRF(pvt_node), PVT_BKT_IPT(pvt_node), &nbkt_info);
    }

    if (bak_idx != NULL) {
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    return rv;
}

static int
alpm_bkt_trie_split_cb(alpm_lib_trie_node_t *trie, alpm_lib_trie_bpm_cb_info_t *info)
{
    /* trieNodeTypeInternalPpg indicates internally created node for ppg */
    if (trie->type != trieNodeTypeInternalPpg) {
        return alpm_trie_pfx_cb(trie, (void *)info->user_data);
    }

    return BCM_E_NONE;
}

int
alpm_bkt_split_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                  alpm_lib_trie_traverse_state_t *state, void *info)
{
    _alpm_bkt_split_cb_data_t *cb_data = (_alpm_bkt_split_cb_data_t *)info;
    /* Split from default bkt */
    if (ACB_IDX(cb_data->acb) > 0 && cb_data->pvt_len == 0) {
        if (ptrie->count < cb_data->bkt_cur_cnt &&
            trie->count < ptrie->count) {
            return 1;
        }
    }

    return 0;
}

static int
alpm_bkt_trie_split(int u, _alpm_cb_t *acb,
                    _bcm_defip_cfg_t *lpm_cfg,
                    alpm_lib_trie_t **spl_bkt_trie,
                    _alpm_pvt_node_t **spl_pvt_node,
                    int max_key_len,
                    int *max_spl_cnt)
{
    int                 rv;
    int                 pkm, ipt, vrf_id, max_split_len, bkt_cnt;
    uint32              pvt_len, pvt_key[5] = {0};

    _alpm_pvt_node_t    *ppn = NULL, *cpn = NULL;
    alpm_lib_trie_t     *pbt = NULL, *cbt = NULL;

    if (lpm_cfg->spl_pn != NULL) {
        ppn = (_alpm_pvt_node_t *)lpm_cfg->spl_pn;
    } else {
        ppn = ACB_HAS_TCAM(acb) ? lpm_cfg->l1_pvt_node : lpm_cfg->pvt_node;
    }
    pbt = PVT_BKT_TRIE(ppn);
    bkt_cnt = pbt->trie->count;
    ALPM_IEG(alpm_lib_trie_init(max_key_len, &cbt));
    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    ipt = ALPM_PKM2IPT(pkm);
    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    if (lpm_cfg->spl_key_len != 0) {
        rv = alpm_lib_trie_split2(pbt, lpm_cfg->spl_key, lpm_cfg->spl_key_len,
                                  pvt_key, &pvt_len, &cbt->trie, 0xfffffff,
                                  TRUE);
    } else {
        _alpm_bkt_split_cb_data_t scb_data;
        scb_data.acb = acb;
        scb_data.bkt_cur_cnt = pbt->trie->count;
        scb_data.pvt_len = PVT_KEY_LEN(ppn);
        alpm_trie_max_split_attr(u, acb, ppn, &max_split_len, max_spl_cnt);
        rv = alpm_lib_trie_split(pbt, max_split_len, pvt_key, &pvt_len,
                 &cbt->trie, FALSE,
                 ACB_BKT_FIXED_FMT(acb, vrf_id) ? NULL : alpm_bkt_split_cb,
                 &scb_data, *max_spl_cnt);
    }
    ALPM_IEG(rv);

    /* Split bucket2A in non-last level could cause misplaced routes in other
     * buckets in next level of current level.
     *   Note: bucket2A: bucketA in Level 2.
     *         bucket3B: bucketB in Level 3.
     *   Case 1:
     *       If split node is trieNodeTypePayload node, no misplaced routes are generated.
     *   Case 2:
     *       If split node is trieNodeTypeInternal node, then:
     *       Case 2.1:
     *           If lpm of split node doesn't exist in this bucket2A, Good.
     *       Case 2.2:
     *           If lpm of split node(Say it's pivot of bucket3B) exists in this
     *           bucket2A, then:
     *
     *           Case 2.2.1:
     *               no route in bucket3B is covered by split node. Good.
     *           Case 2.2.2:
     *               routes in bucket3B are covered by split node.
     *               ====> Misplaced Routes (Need to move them to new location)
     */
    if (ACB_HAS_PVT(acb, vrf_id)) {
        if (cbt->trie->type == trieNodeTypeInternal) {
            _alpm_pvt_node_t *spl_npn = NULL;
            _alpm_bkt_node_t *lpm_bn = NULL;
            _alpm_bkt_node_t tmp_bn;
            /* Case 2 */
            spl_npn = lpm_cfg->spl_npn;
            if (spl_npn) {
                sal_memcpy(tmp_bn.key, spl_npn->key, sizeof(tmp_bn.key));
                tmp_bn.key_len = spl_npn->key_len;
            }
            if (spl_npn && spl_npn->key_len < pvt_len) {
                (void)alpm_lib_trie_insert(pbt, tmp_bn.key,
                                           tmp_bn.key_len, (alpm_lib_trie_node_t *)&tmp_bn);
            }
            rv = alpm_lib_trie_find_lpm(pbt, pvt_key, pvt_len, (alpm_lib_trie_node_t **)&lpm_bn);
            if (spl_npn && spl_npn->key_len < pvt_len) {
                (void)alpm_lib_trie_delete(pbt, tmp_bn.key,
                                  tmp_bn.key_len, (alpm_lib_trie_node_t **)&tmp_bn);
            }
            if (BCM_FAILURE(rv)) {
                acb->acb_cnt.c_spl_mp_cs21++;
                rv = BCM_E_NONE;
            } else {
                /* Case 2.2 */
                _alpm_cb_t *nxt_cb = NULL;
                _alpm_pvt_node_t *lpm_pn = NULL;
                _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
                alpm_lib_trie_t      *pvt_trie;
                alpm_lib_trie_node_t *tmp_node;

                nxt_cb = ACB_DWN(u, acb);
                pvt_trie = ACB_PVT_TRIE(nxt_cb, vrf_id, ipt);
                if (spl_npn && spl_npn->key_len < pvt_len) {
                    (void)alpm_lib_trie_insert(pvt_trie, spl_npn->key,
                                               spl_npn->key_len, (alpm_lib_trie_node_t *)spl_npn);
                }
                rv = alpm_pvt_find_by_key(u, nxt_cb, vrf_id, ipt,
                        lpm_bn->key, lpm_bn->key_len, &lpm_pn);
                if (BCM_SUCCESS(rv) && PVT_BKT_TRIE(lpm_pn)->trie) {
                    alpm_lib_trie_bpm_cb_info_t cb_info;
                    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
                    cb_info.pfx = pvt_key;
                    cb_info.len = pvt_len;
                    cb_info.user_data = pfx_arr;
                    /* PERF OPT point */
                    rv = alpm_lib_trie_ppg(PVT_BKT_TRIE(lpm_pn), 0,
                             pvt_key, pvt_len, alpm_bkt_trie_split_cb,
                             &cb_info);
                    if (BCM_SUCCESS(rv)) {
                        if (pfx_arr->pfx_cnt == 0) {
                            acb->acb_cnt.c_spl_mp_cs221++;
                        } else {
                            acb->acb_cnt.c_spl_mp_cs222++;

                            rv = BCM_E_BUSY;
                            if (spl_npn && spl_npn == lpm_pn) {
                                lpm_cfg->spl_pn = lpm_cfg->spl_opn;
                            } else {
                                lpm_cfg->spl_pn = lpm_pn;
                            }
                            sal_memcpy(lpm_cfg->spl_key, pvt_key, sizeof(lpm_cfg->spl_key));
                            lpm_cfg->spl_key_len = pvt_len;
                        }
                    }
                    alpm_util_free(pfx_arr);
                }
                if (spl_npn && spl_npn->key_len < pvt_len) {
                    (void)alpm_lib_trie_delete(pvt_trie, spl_npn->key,
                                      spl_npn->key_len, &tmp_node);
                }
            }
        } else {
            /* Case 1: split node is trieNodeTypePayload node. */
            acb->acb_cnt.c_spl_mp_cs1++;
        }
    }

    ALPM_IEG(rv);

    ALPM_ALLOC_EG(cpn, sizeof(_alpm_pvt_node_t), "pvt_node");
    PVT_BKT_TRIE(cpn)  = cbt;
    PVT_BKT_VRF(cpn)   = vrf_id;
    PVT_BKT_PKM(cpn)   = pkm;
    PVT_KEY_CPY(cpn, pvt_key);
    PVT_KEY_LEN(cpn)   = pvt_len;
    PVT_FLAGS(cpn)     = lpm_cfg->defip_flags;
    *spl_pvt_node = cpn;
    *spl_bkt_trie = cbt;

    ALPM_INFO(("**ACB(%d).BKT.SPLIT %d entries from bkt %d(cnt %d)\n",
               ACB_IDX(acb), cbt->trie->count,
               PVT_BKT_IDX(ppn), bkt_cnt));

    return rv;

bad:

    if (cbt != NULL) {
        if (cbt->trie != NULL) {
            int rv2;
            rv2 = alpm_lib_trie_merge(pbt, cbt->trie, pvt_key, pvt_len);
            if (BCM_FAILURE(rv2)) {
                ALPM_ERR(("alpm_lib_trie_merge error in trie split, rv %d\n", rv2));
            }
        }

        alpm_lib_trie_destroy(cbt);
    }

    if (cpn != NULL) {
        alpm_util_free(cpn);
    }

    return rv;
}

static int
alpm_bkt_split_pvt_add(int u, _alpm_cb_t *acb,
                       _bcm_defip_cfg_t *pvt_lpm_cfg,
                       _alpm_pvt_node_t *pvt_node)
{
    int                 rv = BCM_E_NONE;
    int                 ipt, vrf_id;
    int                 pvt_len = 0;
    alpm_lib_trie_t     *pfx_trie = NULL;
    _alpm_pfx_node_t    *pfx_node = NULL;

    pvt_len = PVT_KEY_LEN(pvt_node);
    ipt     = ALPM_LPM_IPT(u, pvt_lpm_cfg);
    vrf_id  = ALPM_LPM_VRF_ID(u, pvt_lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);

    /* Assoc-data and bpm_len info of pvt_lpm_cfg comes from alpm_lib_trie_find_lpm */
    ALPM_IER(alpm_lib_trie_find_lpm(pfx_trie, pvt_node->key, pvt_len,
                              (alpm_lib_trie_node_t **)&pfx_node));
    PVT_BPM_LEN(pvt_node) = pfx_node->key_len;
    PVT_BKT_DEF(pvt_node) = pfx_node->bkt_ptr;

    /* Construct KEY(IP address), should come from pvt_key */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_len, pvt_lpm_cfg);
    sal_memcpy(pvt_lpm_cfg->user_data, pvt_node->key, sizeof(uint32) * 5);
    /* Construct Assoc-data */
    if (pfx_node->bkt_ptr != NULL) {
        alpm_util_adata_trie_to_cfg(u, &pfx_node->bkt_ptr->adata, pvt_lpm_cfg);
        pvt_lpm_cfg->default_miss = 0;
    } else {
        alpm_util_adata_zero_cfg(u, pvt_lpm_cfg);
        pvt_lpm_cfg->default_miss = 1;
    }
    rv = alpm_pvt_insert(u, acb, pvt_lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        PVT_IDX(pvt_node) = pvt_lpm_cfg->defip_index;
        rv = alpm_lib_trie_insert(ACB_PVT_TRIE(acb, vrf_id, ipt), pvt_node->key, pvt_len,
                                  (alpm_lib_trie_node_t *)pvt_node);
    }
    /* rv = *_E_FULL is allowed here */

    return rv;
}

/* Case of add new L1 pvt during L2 bkt split:
   If new L2 bkt belongs to new L1 pvt, add new L2 bkt.
*/
static int
alpm_bkt_add_to_npvt(int u, _alpm_cb_t *acb,
                     _bcm_defip_cfg_t *lpm_cfg,
                     _alpm_pvt_node_t *npvt_node)
{
    int                  rv = BCM_E_NONE;
    int                  vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                  ipt = ALPM_LPM_IPT(u, lpm_cfg);
    uint32               *prefix, length;
    uint32               ent_idx;
    int                  bkt_trie_added = 0;

    alpm_lib_trie_t      *bkt_trie = NULL;
    alpm_lib_trie_node_t *tmp_node = NULL;
    _alpm_bkt_node_t     *bkt_node = NULL;
    _alpm_pvt_node_t     *pvt_node;

    _alpm_tcam_write_t *tcam_write = lpm_cfg->tcam_write;

    ALPM_IEG(alpm_pvt_find(u, acb, lpm_cfg, &pvt_node));
    if (pvt_node == npvt_node) { /* belongs to new L1 pvt */
        lpm_cfg->l1_pvt_node = pvt_node;
        /* lpm_cfg->tcam_write may update here due to new L2 pvt added */

        rv = alpm_bkt_insert(u, acb, lpm_cfg, pvt_node, &ent_idx);

        tcam_write->rv = rv;
        if (BCM_FAILURE(rv)) {
            /* If FULL, return no error, continue and will try again
               bkt split when back to alpm_cb_insert */
            if (rv == BCM_E_FULL) {
                ALPM_INFO(("ACB(%d): new L2 bkt add FULL (%d) and retry\n", ACB_IDX(acb), rv));
                return BCM_E_NONE;
            } else {
                ALPM_ERR(("ACB(%d): new L2 bkt add failed (%d)\n", ACB_IDX(acb), rv));
            }
        } else {
            ALPM_INFO(("ACB(%d): new L2 bkt add OK\n", ACB_IDX(acb)));
        }
    } else {
        return BCM_E_NONE; /* exit */
    }

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
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_insert(bkt_trie, prefix, length, (alpm_lib_trie_node_t *)bkt_node);
    ALPM_IEG(rv);
    bkt_trie_added = 1;

    ALPM_IEG(alpm_vrf_pfx_trie_add(u, acb, lpm_cfg, bkt_node));

    if (BCM_SUCCESS(rv) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
        rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "CBInsert");
    }

    VRF_ROUTE_ADD(acb, vrf_id, ipt);

    return rv;

bad:
    if (bkt_trie_added) {
        (void)alpm_lib_trie_delete(bkt_trie, prefix, length, &tmp_node);
    }
    if (bkt_node != NULL) {
        alpm_util_free(bkt_node);
    }

    return rv;
}

/*
 * OUT pvt_node,
 * OUT bkt_node
 */

int
alpm_cb_find(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
             _alpm_pvt_node_t **pvt_node, _alpm_bkt_node_t **bkt_node)
{
    int rv = BCM_E_NONE;

    rv = alpm_pvt_find(u, acb, lpm_cfg, pvt_node);
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bkt_find(u, acb, lpm_cfg, *pvt_node, bkt_node);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Find and update the match in the database.
 * Returns BCM_E_NONE, if found and updated.
 * BCM_E_NOT_FOUND if not found
 * BCM_E_FAIL, if update failed.
 */
static int
alpm_cb_update(int u, _alpm_cb_t *acb,
               _bcm_defip_cfg_t *lpm_cfg)
{
    _alpm_ppg_data_t pdata;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node;
    int         rv = BCM_E_NONE;

    /* Find and update the entry */
    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    bkt_node = (_alpm_bkt_node_t *)lpm_cfg->bkt_node;
    if (bkt_node == NULL) {
        ALPM_ERR(("**ACB(%d).FIND.UPDATE bkt_node == NULL...\n",
                  ACB_IDX(acb)));
        return BCM_E_FAIL;
    }
    alpm_util_adata_cfg_to_trie(u, lpm_cfg, &bkt_node->adata);
    ALPM_IER(alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, bkt_node->ent_idx, ALPM_PVT_UPDATE_ASSOC_DATA));

    if (ACB_HAS_RTE(acb, ALPM_LPM_VRF_ID(u, lpm_cfg))) {
        /* Perform INSERT propagation to update assoc_data */
        alpm_ppg_prepare(u, lpm_cfg, lpm_cfg->defip_sub_len, bkt_node, &pdata);
        ALPM_IER(alpm_ppg_assoc_data(u, ALPM_PPG_INSERT, &pdata));
    }
    return rv;
}

static int
alpm_vrf_pfx_trie_add(int u, _alpm_cb_t *acb,
                      _bcm_defip_cfg_t *lpm_cfg, void *udata)
{
    int         rv = BCM_E_NONE, ipt, vrf_id;
    uint32      *prefix;
    uint32      length;

    alpm_lib_trie_t      *pfx_trie = NULL;
    alpm_lib_trie_node_t *lpmp;
    _alpm_pfx_node_t *pfx_node = NULL, *tmp_node;

    vrf_id = lpm_cfg->defip_vrf;
    if (vrf_id == BCM_L3_VRF_GLOBAL) {
        /* GLobal low */
        vrf_id = ALPM_VRF_ID_GLO(u);
    } else if (vrf_id < 0) {
        assert(0);
    }

    if (ACB_HAS_PVT(acb, vrf_id)) {
        return rv;
    }

    ipt = ALPM_LPM_IPT(u, lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);

    if (lpm_cfg->defip_sub_len == 0) {
        /* Add a route already in trie */
        lpmp = NULL;
        rv = alpm_lib_trie_find_lpm(pfx_trie, 0, 0, &lpmp);
        if (BCM_SUCCESS(rv)) {
            tmp_node = (_alpm_bkt_node_t *)lpmp;
            tmp_node->bkt_ptr = udata;
        }

        return rv;
    }

    /* Non default route start from here */
    prefix = lpm_cfg->user_data;
    length = lpm_cfg->defip_sub_len;

    ALPM_ALLOC_EG(pfx_node, sizeof(_alpm_pfx_node_t), "VRF_trie_node");

    sal_memcpy(pfx_node->key, prefix, sizeof(pfx_node->key));
    pfx_node->key_len = length;
    pfx_node->ent_idx = lpm_cfg->defip_index;
    pfx_node->bkt_ptr = udata;

    rv = alpm_lib_trie_insert(pfx_trie, prefix, length, (alpm_lib_trie_node_t *)pfx_node);
    if (BCM_FAILURE(rv)) {
        if (pfx_node != NULL) {
            alpm_util_free(pfx_node);
        }
    }

bad:
    return rv;
}

static int
alpm_vrf_pfx_trie_delete(int u, _alpm_cb_t *acb,
                         _bcm_defip_cfg_t *lpm_cfg, void *udata,
                         _alpm_bkt_node_t **alt_def_pfx,
                         uint32 *alt_bpm_len)
{
    int         rv = BCM_E_NONE, rv2, ipt, vrf_id;
    uint32      *prefix;
    uint32      pfx_len, bpm_len;

    alpm_lib_trie_t      *pfx_trie = NULL;
    alpm_lib_trie_node_t *lpmp = NULL;
    _alpm_pfx_node_t *pfx_node = NULL, *tmp_node;

    vrf_id = lpm_cfg->defip_vrf;
    if (vrf_id == BCM_L3_VRF_GLOBAL) {
        /* Global low */
        vrf_id = ALPM_VRF_ID_GLO(u);
    } else if (vrf_id < 0) {
        assert(0);
    }

    if (ACB_HAS_PVT(acb, vrf_id)) {
        return rv;
    }

    ipt = ALPM_LPM_IPT(u, lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
    prefix      = lpm_cfg->user_data;
    pfx_len     = lpm_cfg->defip_sub_len;

    if (pfx_len != 0) {
        rv = alpm_lib_trie_delete(pfx_trie, prefix, pfx_len, &lpmp);
        pfx_node = (_alpm_bkt_node_t *)lpmp;
        ALPM_IEG(rv);

        /* Find the alternative best prefix match node */
        lpmp = NULL;
        rv = alpm_lib_trie_find_lpm(pfx_trie, prefix, pfx_len, &lpmp);
        tmp_node = (_alpm_pfx_node_t *)lpmp;
        if (BCM_SUCCESS(rv)) {
            _alpm_bkt_node_t *t = (_alpm_bkt_node_t *)(tmp_node->bkt_ptr);
            if (t != NULL) {
                /* Means default route exists */
                bpm_len = t->key_len;
            } else {
                /* This should happen only if the lpm is a default route */
                bpm_len = 0;
            }

            *alt_def_pfx = t;
            *alt_bpm_len = bpm_len;
        } else {
            /* Not expected */
            ALPM_IEG(rv);
        }
    } else {
        /* Delete a default route */
        lpmp = NULL;
        rv = alpm_lib_trie_find_lpm(pfx_trie, prefix, pfx_len, &lpmp);
        tmp_node = (_alpm_pfx_node_t *)lpmp;
        tmp_node->bkt_ptr = NULL;
    }

    if (pfx_node != NULL) {
        alpm_util_free(pfx_node);
    }

    return rv;

bad:
    /* Recover if necessary */
    if (pfx_node != NULL) {
        rv2 = alpm_lib_trie_insert(pfx_trie, prefix,
                                   pfx_len, (alpm_lib_trie_node_t *)pfx_node);
        if (BCM_FAILURE(rv2)) {
            ALPM_ERR(("Recover failed\n"));
        }
    }
    return rv;
}

static int
alpm_vrf_pfx_trie_init(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int rv = BCM_E_NONE;
    alpm_lib_trie_t  *root = NULL;
    uint32  max_key_len;
    uint32  key[5] = {0};
    _alpm_pfx_node_t    *pfx_node = NULL;

    max_key_len = alpm_util_trie_max_key_len(u, ipt);

    ALPM_ALLOC_EG(pfx_node, sizeof(*pfx_node), "Payload for pfx trie key");
    sal_memset(pfx_node, 0, sizeof(*pfx_node));
    rv = alpm_lib_trie_init(max_key_len, &ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt));
    if (BCM_SUCCESS(rv)) {
        root = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
        rv = alpm_lib_trie_insert(root, key, 0, &(pfx_node->node));
    }

    if (BCM_FAILURE(rv)) {
        alpm_util_free(pfx_node);
        if (root != NULL) {
            alpm_lib_trie_destroy(root);
            ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt) = NULL;
        }
    }

bad:
    return rv;
}

int
alpm_vrf_pfx_trie_deinit(int u, int app, int vrf_id, int ipt)
{
    int rv = BCM_E_NONE;
    alpm_lib_trie_t  *root = NULL;
    uint32  key[5] = {0};
    _alpm_pfx_node_t    *pfx_node = NULL;

    root = ALPM_VRF_TRIE(u, app, vrf_id, ipt);
    if (root == NULL) {
        return rv;
    }
    rv = alpm_lib_trie_delete(root, key, 0, (alpm_lib_trie_node_t **)&pfx_node);
    if (BCM_SUCCESS(rv)) {
        alpm_util_free(pfx_node);
        ALPM_IER(alpm_lib_trie_destroy(root));
        ALPM_VRF_TRIE(u, app, vrf_id, ipt) = NULL;
    }

    return rv;
}

int
alpm_vrf_pvt_node_init(int u, _alpm_cb_t *acb,
                       alpm_lib_trie_t *pvt_root,
                       _bcm_defip_cfg_t *pvt_cfg,
                       _alpm_pvt_node_t **pvt_node)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id;
    int                 ipt;
    uint32              max_key_len;
    _alpm_pvt_node_t    *tmp_node = NULL;

    ipt = ALPM_LPM_IPT(u, pvt_cfg);
    vrf_id = ALPM_LPM_VRF_ID(u, pvt_cfg);

    ALPM_ALLOC_EG(tmp_node, sizeof(_alpm_pvt_node_t), "pvt_node");

    /* kshift = 0 */
    max_key_len = alpm_util_trie_max_key_len(u, ipt);
    ALPM_IEG(alpm_lib_trie_init(max_key_len, &PVT_BKT_TRIE(tmp_node)));
    if (pvt_cfg->bkt_info) {
        sal_memcpy(&PVT_BKT_INFO(tmp_node), pvt_cfg->bkt_info, sizeof(_alpm_bkt_info_t));
    }
    PVT_BKT_VRF(tmp_node)   = vrf_id;
    PVT_BKT_PKM(tmp_node)   = ALPM_LPM_PKM(u, pvt_cfg);
    PVT_BKT_DEF(tmp_node)   = NULL;
    PVT_BPM_LEN(tmp_node)   = 0;
    PVT_KEY_LEN(tmp_node)   = pvt_cfg->defip_sub_len;
    PVT_KEY_CPY(tmp_node,  pvt_cfg->user_data);
    PVT_FLAGS(tmp_node)     = pvt_cfg->defip_flags;

    PVT_IDX(tmp_node)       = pvt_cfg->defip_index;

    *pvt_node = tmp_node;

    return rv;

bad:
    if (tmp_node != NULL) {
        alpm_util_free(tmp_node);
    }

    if (PVT_BKT_TRIE(tmp_node) != NULL) {
        alpm_lib_trie_destroy(PVT_BKT_TRIE(tmp_node));
        PVT_BKT_TRIE(tmp_node) = NULL;
    }

    return rv;
}

static int
alpm_vrf_pvt_node_delete(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                         _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    _alpm_pvt_node_t    *pvt_node = NULL;
    alpm_lib_trie_t              *pvt_root = NULL;

    pvt_root = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (pvt_root == NULL) {
        return rv;
    }
    rv = alpm_lib_trie_delete(pvt_root, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                     (alpm_lib_trie_node_t **)&pvt_node);
    if (BCM_SUCCESS(rv)) {
        ALPM_IER(alpm_lib_trie_destroy(PVT_BKT_TRIE(pvt_node)));
        alpm_util_free(pvt_node);
    }

    return rv;
}

int
alpm_vrf_pvt_trie_deinit(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                         _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;

    rv = alpm_vrf_pvt_node_delete(u, acb, vrf_id, ipt, lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        ALPM_IER(alpm_lib_trie_destroy(ACB_PVT_TRIE(acb, vrf_id, ipt)));
        ACB_PVT_TRIE(acb, vrf_id, ipt) = NULL;
    }

    return rv;
}

static int
alpm_cb_pvt_add(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int pvt_node_inited = 0, pvt_root_inited = 0;

    uint32              max_key_len;
    alpm_lib_trie_t     *pvt_root;
    _alpm_bkt_info_t    bkt_info;
    _alpm_pvt_node_t    *pvt_node = NULL;

    pvt_root = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (pvt_root == NULL) {
        max_key_len = alpm_util_trie_max_key_len(u, ipt);
        ALPM_IEG(alpm_lib_trie_init(max_key_len, &ACB_PVT_TRIE(acb, vrf_id, ipt)));
        pvt_root = ACB_PVT_TRIE(acb, vrf_id, ipt);
        pvt_root_inited = 1;
    }
    ALPM_IEG(alpm_vrf_pvt_node_init(u, acb, pvt_root, lpm_cfg, &pvt_node));
    pvt_node_inited = 1;
    if (!pvt_root_inited) {
        _alpm_pfx_node_t    *pfx_node = NULL;
        alpm_lib_trie_t     *pfx_trie = NULL;

        pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
        if (pfx_trie != NULL) {
            ALPM_IER(alpm_lib_trie_find_lpm(pfx_trie, pvt_node->key, pvt_node->key_len,
                                   (alpm_lib_trie_node_t **)&pfx_node));
            PVT_BPM_LEN(pvt_node) = pfx_node->key_len;
            PVT_BKT_DEF(pvt_node) = pfx_node->bkt_ptr;
        }
    }

    if (ACB_HAS_TCAM(acb)) { /* need for TCAM write */
        lpm_cfg->pvt_node = (uint32 *)pvt_node;
    }

retry:
    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        rv = alpm_bkt_alloc(u, acb, vrf_id, ipt, 1, &bkt_info, NULL, NULL);
    } else {
        rv = alpm_bnk_alloc(u, acb, vrf_id, ipt, 1, &bkt_info, NULL);
    }

    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
    lpm_cfg->bkt_info = &bkt_info;
    sal_memcpy(&PVT_BKT_INFO(pvt_node), &bkt_info, sizeof(_alpm_bkt_info_t));

    rv = alpm_pvt_insert(u, acb, lpm_cfg);
    /* Upper level of BCM_E_BUSY point,
     * ACB_HAS_RTE may not be a good condition, but considering
     * it's only 3 levels so it also works.
     */
    if (rv == BCM_E_BUSY && lpm_cfg->spl_pn && ACB_HAS_RTE(acb, vrf_id)) {
        alpm_bkt_free(u, acb, vrf_id, ipt, &bkt_info);
        sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
        rv = alpm_bnk_alloc(u, acb, vrf_id, ipt,
                            ACB_BNK_PER_BKT(acb, vrf_id),
                            &bkt_info, NULL);
        ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

        /* Case 2.2.2, split given l3 pvt_node first */
        rv = alpm_cb_split(u, acb, lpm_cfg, &bkt_info, 0xffff);

        if (BCM_SUCCESS(rv)) {
            lpm_cfg->spl_pn = NULL;
            lpm_cfg->spl_key_len = 0;
            lpm_cfg->l1_pvt_node = NULL;
            /* PERF OPT point */
            goto retry;
        }
    }
    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

    PVT_IDX(pvt_node) = lpm_cfg->defip_index;
    lpm_cfg->pvt_node = (uint32 *)pvt_node;
    ALPM_IEG(alpm_lib_trie_insert(ACB_PVT_TRIE(acb, vrf_id, ipt),
             pvt_node->key, pvt_node->key_len,
             (void *)pvt_node));

bad:
    if (BCM_FAILURE(rv)) {
        int rv2 = BCM_E_NONE;
        /* ALPMTBD rollback */
        if (lpm_cfg->bkt_info != NULL) {
            alpm_bkt_free(u, acb, vrf_id, ipt, &bkt_info);
            lpm_cfg->bkt_info = NULL;
        }

        if (pvt_root_inited) {
            rv2 = alpm_vrf_pvt_trie_deinit(u, acb, vrf_id, ipt, lpm_cfg);
        } else if (pvt_node_inited) {
            rv2 = alpm_vrf_pvt_node_delete(u, acb, vrf_id, ipt, lpm_cfg);
        }

        if (BCM_FAILURE(rv2) && pvt_node != NULL) {
            (void) alpm_lib_trie_destroy(PVT_BKT_TRIE(pvt_node));
            alpm_util_free(pvt_node);
        }
    }

    return rv;

}

static void
alpm_vrf_spl_len_calc(int u, _alpm_cb_t *acb, int vrf_id, uint8 db_type,
                      uint32 *splen1, uint32 *splen2)
{
    int idx, bpb;
    uint32 diff1[5] = {6, 5, 7, 6, 0};
    uint32 diff2[5] = {2, 3, 3, 4, 0};

    bpb = ACB_BNK_PER_BKT(acb, vrf_id) > 4;

    if (ACB_IDX(acb) > 0) {
        idx = bpb << 1 | db_type;
    } else {
        idx = 4;
    }
    *splen1 = diff1[idx];
    *splen2 = diff2[idx];

    return;
}

/*
 * alpm_vrf_init
 * Add a VRF default route when a L3 VRF is added
 * Adds a 0.0 entry into VRF
 */
static int
alpm_vrf_init(int u, _alpm_cb_t *acb, int vrf_id, int ipt, uint8 db_type)
{
    int                 rv = BCM_E_NONE;
    _bcm_defip_cfg_t    pvt_cfg;

    /* Need to set db_type at the beginning */
    ACB_VRF_DB_TYPE_SET(u, acb, vrf_id, ipt, db_type);

    sal_memset(&pvt_cfg, 0, sizeof(pvt_cfg));
    pvt_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    pvt_cfg.defip_flags |= ALPM_IS_IPV6(ipt) ? BCM_L3_IP6 : 0;

    if (ALPM_TCAM_ZONED(u, acb->app)) {
        pvt_cfg.default_miss = 1;
    }

    /* Encapsulate app info into lpm_cfg */
    pvt_cfg.defip_inter_flags |= alpm_app_flag_get(u, acb->app);

    ALPM_IEG_PRT_EXCEPT(alpm_cb_pvt_add(u, acb, vrf_id, ipt, &pvt_cfg), BCM_E_FULL);

    ACB_VRF_INIT_SET(u, acb, vrf_id, ipt);
    ACB_DR_INIT_CLEAR(u, acb, vrf_id, ipt); /* clear DR partially inited */

    alpm_vrf_spl_len_calc(u, acb, vrf_id, db_type,
                          &ACB_VRF_SPLEN_DIFF1(u, acb, vrf_id, ipt),
                          &ACB_VRF_SPLEN_DIFF2(u, acb, vrf_id, ipt));

    if (ACB_HAS_RTE(acb, vrf_id)) {
        rv = alpm_vrf_pfx_trie_init(u, acb, vrf_id, ipt);
    }

bad:
    if (BCM_FAILURE(rv)) {
        /* ALPMTBD rollback */
        if (pvt_cfg.bkt_info != NULL) {
            alpm_bkt_free(u, acb, vrf_id, ipt, (_alpm_bkt_info_t *)pvt_cfg.bkt_info);
        }
        (void)alpm_vrf_pvt_trie_deinit(u, acb, vrf_id, ipt, &pvt_cfg);
        ACB_VRF_INIT_CLEAR(u, acb, vrf_id, ipt);
    }
    return rv;

}

/*
 * alpm_vrf_deinit
 * Delete the VRF default route when a L3 VRF is destroyed
 */
int
alpm_vrf_deinit(int u, int app, int vrf_id, int ipt)
{
    int                 i, rv = BCM_E_NONE;
    _bcm_defip_cfg_t    lpm_cfg;
    _alpm_pvt_node_t    *pvt_node = NULL;
    _alpm_cb_t          *acb;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    if (ALPM_TCAM_ZONED(u, app)) {
        lpm_cfg.default_miss = 1;
    }

    /* Encapsulate app info into lpm_cfg */
    lpm_cfg.defip_inter_flags |= alpm_app_flag_get(u, app);

    for (i = ACB_CNT(u) - 1; i >= 0; i--) {
        acb = ACB(u, i, app);
        if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
            continue;
        }

        ALPM_IER(alpm_pvt_find(u, acb, &lpm_cfg, &pvt_node));
        /* Banks are already freed via _pfx_shrink */
        /* add Entry into tcam as default routes for the VRF */
        if (ACB_HAS_TCAM(acb)) {
            lpm_cfg.pvt_node = pvt_node;
            alpm_bkt_bnk_shrink(u, ipt, acb,
                       ACB_BKT_VRF_POOL(acb, vrf_id),
                       &PVT_BKT_INFO(pvt_node), NULL);
        } else {
            lpm_cfg.pvt_node = NULL;
        }
        lpm_cfg.bkt_node = NULL;
        ALPM_IER(alpm_pvt_delete(u, acb, &lpm_cfg));
        ALPM_IER(alpm_vrf_pvt_trie_deinit(u, acb, vrf_id, ipt, &lpm_cfg));
        ACB_VRF_INIT_CLEAR(u, acb, vrf_id, ipt);
    }

    rv = alpm_vrf_pfx_trie_deinit(u, app, vrf_id, ipt);

    return rv;
}

/* Check the bank usage against threshold for TH3 only when bnk_cnt > 8K */
static int
alpm_cb_bnk_threshold_exceed(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    _alpm_bkt_pool_conf_t *bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    if (!ACB_BKT_FIXED_FMT(acb, 1) &&
        ACB_HAS_RTE(acb, vrf_id) &&
        (BPC_BNK_CNT(bp_conf) > 8192) &&
        ((BPC_BNK_USED(bp_conf, ipt) * 100 / BPC_BNK_CNT(bp_conf)) >
        ALPMC(u)->_alpm_bnk_threshold)) {
        return TRUE;
    }

    return FALSE;
}

/* What is expected here when entering this function:
 * all occupied banks are continuous.
 *    all i in {i | bkt_info->bnk_fmt[i] is used} are continuous AND
 *    foreach i belongs to {i | bkt_info->bnk_fmt[i] is used}:
 *        bkt_info->vet_bmp[i]!=0
 */
int
alpm_cb_expand(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, int bnk_cnt, int *fmt_update)
{
    int i, ipt, rv = BCM_E_FULL;
    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pool_conf_t *bp_conf;
    int bnkpb, start, end;
    int leftnb, rightnb;
    int def_fmt;

    ipt = PVT_BKT_IPT(pvt_node);
    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, PVT_BKT_VRF(pvt_node), ipt);
    *fmt_update = FALSE;

    ALPM_VERB(("Expanding ALPM CB %d bkt %d......\n",
               acb->acb_idx, PVT_BKT_IDX(pvt_node)));

    /* There 4 cases when expanding a bucket */
    /* Case 1: neighbour bank available
     *         -> occupy neighbour bank */
    /*
      0 1 2 3 4 5 6 7
     -----------------
     |#|#|#|#|#| | | |
     -----------------
      ^
     rofs=0
     -----------------
     | | |#|#|#|#| | |
     -----------------
          ^
         rofs=2
     -----------------
     | | | |#|#|#|#|#|
     -----------------
            ^
           rofs=3          bkt+1
     -----------------     -----------------
     | | | | | |#|#|#|     |#|#| | | | | | |
     -----------------     -----------------
                ^
               rofs=5
     */

    /* There is a neighbour */
    bkt_info = &PVT_BKT_INFO(pvt_node);
    bp_conf  = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    start = PVT_ROFS(pvt_node);
    end   = PVT_ROFS(pvt_node) + bnkpb;

    /* Get neighbour bank offset based on bkt-idx */
    for (i = start; i < end; i++) {
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb)) {
            break;
        }
    }

    leftnb  = PVT_BKT_IDX(pvt_node) * bnkpb + start - 1;
    rightnb = PVT_BKT_IDX(pvt_node) * bnkpb + i;
    if (leftnb >= 0) {
        rv = alpm_bkt_bnk_chk_rsvd(u, ipt, bp_conf, leftnb);
        if (BCM_SUCCESS(rv)) {
            if (PVT_BKT_IDX(pvt_node) != (leftnb / bnkpb)) {
                /* Update */
                PVT_BKT_IDX(pvt_node) = leftnb / bnkpb;
            }

            PVT_ROFS(pvt_node) = leftnb % bnkpb;
            bkt_info->bnk_fmt[PVT_ROFS(pvt_node)] = def_fmt;
            *fmt_update = TRUE;
            goto end;
        }
    }

    if (rightnb < BPC_BNK_CNT(bp_conf)) {
        rv = alpm_bkt_bnk_chk_rsvd(u, ipt, bp_conf, rightnb);
        if (BCM_SUCCESS(rv)) {
            bkt_info->bnk_fmt[rightnb % bnkpb] = def_fmt;
            *fmt_update = TRUE;
            goto end;
        }
    }

    /* Case 2: No neighbour bank, but there are other
     * continuous banks available:
     *         -> Move banks to other area */
    start = 0;
    rv = alpm_bkt_bnk_alloc(u, ipt, bp_conf, &start, bnk_cnt+1, 1);
    if (BCM_SUCCESS(rv)) {
        _alpm_bkt_info_t dst_bkt;

        sal_memset(&dst_bkt, 0, sizeof(dst_bkt));
        BI_ROFS(&dst_bkt) = start % bnkpb;
        BI_BKT_IDX(&dst_bkt) = start / bnkpb;
        for (i = 0; i < bnk_cnt; i++) {
            dst_bkt.bnk_fmt[(BI_ROFS(&dst_bkt) + i) % bnkpb] = def_fmt;
        }

        if (bnk_cnt > 0) {
            /* Move banks */
            rv = alpm_bkt_move(u, acb, pvt_node, bkt_info, &dst_bkt);
        } else {
            /* Swap bkt_info only */
            sal_memcpy(bkt_info, &dst_bkt, sizeof(_alpm_bkt_info_t));
        }

        /* expand one bank after alpm_bkt_move */
        bkt_info->bnk_fmt[(PVT_ROFS(pvt_node) + i) % bnkpb] = def_fmt;
        *fmt_update = TRUE;
        goto end;
    }

    /* Check bank usage threshold */
    if (alpm_cb_bnk_threshold_exceed(u, acb, PVT_BKT_VRF(pvt_node), ipt)) {
        /* return E_RESOURCE in ALPM.expand to no split with unexpected
           bucket situation. It will change to E_FULL outside */
        return BCM_E_RESOURCE;
    }

    /* Case 3: No neighbour bank, no continuous banks available
     *         -> move empty bank to neighbour bank */
    /* ALPMTBD, can be optimized by using partial defragmentation to
     * create a free bucket then do Case 2.
     */

    /* Find nearest unused bit position */
    if (leftnb >= 0 || rightnb < BPC_BNK_CNT(bp_conf)) {
        int nst_bnk = -1;
        rv = alpm_bkt_bnk_nearest_get(u,
                    BPC_BNK_BMP(bp_conf, ipt),
                    BPC_BNK_CNT(bp_conf),
                    leftnb, rightnb, &nst_bnk);
        if (BCM_SUCCESS(rv)) {
            /* Move unused nst_bnk */
            if (nst_bnk > rightnb) {
                /* Move nst_bnk to rightnb */
                rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, nst_bnk, rightnb, ipt);
                ALPM_IER(rv);
                /* Now rightnb is free */
                rv = alpm_bkt_bnk_chk_rsvd(u, ipt, bp_conf, rightnb);
                if (BCM_SUCCESS(rv)) {
                    bkt_info->bnk_fmt[rightnb % bnkpb] = def_fmt;
                    *fmt_update = TRUE;
                    goto end;
                }
            } else if (nst_bnk < leftnb) {
                /* Move nst_bnk to leftnb */
                rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, nst_bnk, leftnb, ipt);
                ALPM_IER(rv);
                /* Now leftnb is free */
                rv = alpm_bkt_bnk_chk_rsvd(u, ipt, bp_conf, leftnb);
                if (BCM_SUCCESS(rv)) {
                    if (PVT_BKT_IDX(pvt_node) != (leftnb / bnkpb)) {
                        /* Update */
                        PVT_BKT_IDX(pvt_node) = leftnb / bnkpb;
                    }
                    PVT_ROFS(pvt_node) = leftnb % bnkpb;
                    bkt_info->bnk_fmt[PVT_ROFS(pvt_node)] = def_fmt;

                    *fmt_update = TRUE;
                    goto end;
                }
            } else {
                /* Shouldn't be here */
                rv = BCM_E_FULL;
            }
        }
    }

    /* Case 4: table full */

end:
    if (BCM_SUCCESS(rv)) {
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "Expand");
        }
        acb->acb_cnt.c_expand++;
    }
    return rv;
}

int
alpm_cb_defrag(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int rv = BCM_E_NONE;
    int i, j, bnkpb;
    int rngl, rngr, rngm;
    _alpm_bkt_pool_conf_t *bp_conf;
    SHR_BITDCL *bnk_bmp;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnkpb   = BPC_BNK_PER_BKT(bp_conf);

    if (BPC_BNK_CNT(bp_conf) - BPC_BNK_USED(bp_conf, ipt) < bnkpb) {
        return BCM_E_FULL;
    }

   /* Check bank usage threshold */
    if (alpm_cb_bnk_threshold_exceed(u, acb, vrf_id, ipt)) {
        return BCM_E_FULL;
    }

    /* Find best zone for defragmentation */
    bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
    rv = alpm_bkt_free_bnk_range_get(u,
            bnk_bmp, BPC_BNK_CNT(bp_conf),
            bnkpb, &rngl, &rngr, &rngm);

    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

    /*
    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_DEFRAG) {
        cli_out("free range get: bpb %d, range left %d, right %d, mid %d\n",
                bnkpb, rngl, rngr, rngm);
        alpm_util_bnk_range_print(u, bnk_bmp, 0xfffffff, 32, rngl, rngr);
    }
    */

    /*
     ---------------------------
     | |#|#| | | | |#|#|#| | | |
     ---------------------------
      ^         ^             ^
      rngl      rngm          rngr

     ---------------------------
     | |#| |#| |#| |#| |#| |#| |
     ---------------------------
      ^           ^           ^
      rngl        rngm        rngr
     */
    for (i = rngm - 1; i > rngl; i--) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            continue;
        }
        for (j = i - 1; j >= rngl; j--) {
            if (SHR_BITGET(bnk_bmp, j)) {
                continue;
            }

            /* bnk_bmp could be changed here */
            rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, j, i, ipt);
            ALPM_IEG(rv);
            break;
        }
    }

    for (i = rngm + 1; i < rngr; i++) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            continue;
        }
        for (j = i + 1; j <= rngr; j++) {
            if (SHR_BITGET(bnk_bmp, j)) {
                continue;
            }

            /* bnk_bmp could be changed here */
            rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, j, i, ipt);
            ALPM_IEG(rv);
            break;
        }
    }
    /*
    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_DEFRAG) {
        cli_out("After Defrag: \n");
        alpm_util_bnk_range_print(u, bnk_bmp, 0xfffffff, 32,
                                  rngl, rngr);
    }
    */

    acb->acb_cnt.c_defrag++;

bad:
    return rv;
}

static int
alpm_cb_split(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
              _alpm_bkt_info_t *nbkt_info, int max_spl_cnt)
{
    int     rv = BCM_E_NONE;
    int     max_key_len, ipt;
    int     bkt_allocated = 0;
    int     vrf_id = 0;
    int     shrink_empty_only = 2;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    uint32  *bak_idx = NULL;

    alpm_lib_trie_t      *nbkt_trie = NULL;

    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_pvt_node_t    *opvt_node = NULL;
    _alpm_pvt_node_t    *npvt_node = NULL;
    _bcm_defip_cfg_t    pvt_lpm_cfg;
    _alpm_tcam_write_t  *tcam_write = lpm_cfg->tcam_write;

    ipt    = ALPM_LPM_IPT(u, lpm_cfg);
    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    max_key_len = alpm_util_trie_max_key_len(u, ipt);
    /* 1. alpm_lib_trie_split to get split-prefix set
     * Allocate a new bucket based on the split prefixes
     * min_sz, in TH-style-ALPM devices, it's entry count,
     * in FMT-enabled deivces, it's bank count */
    rv = alpm_bkt_trie_split(u, acb, lpm_cfg, &nbkt_trie, &npvt_node,
                             max_key_len, &max_spl_cnt);
    ALPM_IEG(rv);

    if (max_spl_cnt < 0xffff && !ACB_BKT_FIXED_FMT(acb, 1)) {
        alpm_bkt_free(u, acb, vrf_id, ipt, nbkt_info);
        sal_memset(nbkt_info, 0, sizeof(_alpm_bkt_info_t));
        nbkt_info->reverse = 1;
        rv = alpm_bnk_alloc(u, acb, vrf_id, ipt, ACB_BNK_PER_BKT(acb, vrf_id),
                            nbkt_info, NULL);
        /* Should not see error here */
        ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
    }

    /* 3. HW: Copy split prefixes from old bucket to new bucket */
    sal_memcpy(&PVT_BKT_INFO(npvt_node), nbkt_info, sizeof(_alpm_bkt_info_t));
    bkt_allocated = TRUE;

    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = alpm_lib_trie_traverse(nbkt_trie, alpm_trie_pfx_cb, pfx_arr,
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

    /* 3. Save old ent_idx of prefixes for recovery */
    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);

    if (lpm_cfg->spl_pn != NULL) {
        opvt_node = (_alpm_pvt_node_t *)lpm_cfg->spl_pn;
    } else {
        opvt_node = ACB_HAS_TCAM(acb) ? lpm_cfg->l1_pvt_node : lpm_cfg->pvt_node;
    }

    /* bkt_pfx_copy updates ent_idx of each prefix in the new trie */
    rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, opvt_node, npvt_node);
    ALPM_IEG(rv);

    /* Free unused banks of new pivot to Global bank pool */
    alpm_bkt_bnk_shrink(u, ipt, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                        &PVT_BKT_INFO(npvt_node), NULL);

    if (!ACB_BKT_FIXED_FMT(acb, 1) && ACB_HAS_TCAM(acb)) {
        /* Check if an extra split is required */
        rv = alpm_bkt_ent_get(u, vrf_id, acb, npvt_node, lpm_cfg->defip_sub_len, NULL, NULL, 1);
        if (rv == BCM_E_FULL) {
            /* Need 2 free tcam entries to guarantee (1 for current npvt, 1 for extra split pvt) */
            int rsvd = 1;
            /* Cannot affort another split, so should return here */
            if (bcm_esw_alpm_tcam_avail(u, app, vrf_id, ipt, PVT_KEY_LEN(npvt_node), 0) <= rsvd) {
                ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
            }
        }
    }

    /* 4. HW: insert new pivot
     *    recursion happens here: BCM_E_FULL is expected
     */
    sal_memcpy(&pvt_lpm_cfg, lpm_cfg, sizeof(pvt_lpm_cfg));
    /* never insert new pvt with HIT=1 (from original lpm_cfg->defip_flags) */
    pvt_lpm_cfg.defip_flags &= ~BCM_L3_HIT;
    if (ACB_HAS_TCAM(acb)) {
        pvt_lpm_cfg.pvt_node = (uint32 *)npvt_node;
        pvt_lpm_cfg.bkt_info = NULL;
    } else {
        pvt_lpm_cfg.l1_pvt_node = NULL;
        pvt_lpm_cfg.pvt_node = NULL;
        pvt_lpm_cfg.spl_pn = NULL;
        pvt_lpm_cfg.spl_opn = opvt_node;
        pvt_lpm_cfg.spl_npn = npvt_node;
        pvt_lpm_cfg.bkt_info = (uint32 *)&PVT_BKT_INFO(npvt_node);
        if (ACB_BKT_FIXED_FMT(acb, 1)) {
            pvt_lpm_cfg.bkt_kshift = 0;
        } else {
            /* for upr acb bkt */
            pvt_lpm_cfg.bkt_kshift = PVT_KEY_LEN(npvt_node);
        }
    }
    pvt_lpm_cfg.bkt_node = NULL;

    /* pvt_lpm_cfg.tcam_write has same pointer as lpm_cfg->tcam_write */
    rv = alpm_bkt_split_pvt_add(u, acb, &pvt_lpm_cfg, npvt_node);
    /* rv = *_E_FULL is allowed here */
    if (rv == BCM_E_FULL) {
        ALPM_INFO(("**ACB(%d).BKT.SPLIT return Full\n", ACB_IDX(acb)));
    } else if (rv == BCM_E_BUSY) {
        lpm_cfg->spl_pn = pvt_lpm_cfg.spl_pn;
        sal_memcpy(lpm_cfg->spl_key, pvt_lpm_cfg.spl_key, sizeof(lpm_cfg->spl_key));
        lpm_cfg->spl_key_len = pvt_lpm_cfg.spl_key_len;
    }

    ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

    /* Case of add new L1 pvt during L2 bkt split:
       - If new L2 bkt belongs to new L1 pvt, add new L2 bkt first.
       - Then write returned lpm_cfg->tcam_write to TCAM HW */
    if (tcam_write != NULL) {
        rv = alpm_bkt_add_to_npvt(u, acb, lpm_cfg, npvt_node);
        if (tcam_write->hw_idx >= 0) {
            int rv1;
            rv1 = _tcam_entry_write(u, app, ALPM_LPM_PKM(u, lpm_cfg), NULL, tcam_write->ent,
                                    tcam_write->hw_idx, tcam_write->hw_idx,
                                    FALSE);
            ALPM_INFO(("Write to TCAM HW at hw_idx %d after add new L2 bkt\n",
                       tcam_write->hw_idx));
            ALPM_IEG(rv1);
        }
        /* check returned rv after _tcam_entry_write done */
        ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
    }

    /*
     * In case of pivot insert failure, we need to :
     *     a). recover split_trie
     *     b). Invalidate new bucket.
     */

    /* 5. HW: Invalidate split prefixes in the old bucket */
    rv = alpm_bkt_pfx_clean(u, acb, opvt_node, pfx_arr->pfx_cnt, bak_idx);
    ALPM_IEG(rv);

    /* 6. SW: Shrink old bucket,
     * A important step to make sure banks are continuously occupied by pivot
     */
    rv = alpm_bkt_pfx_shrink(u, acb, opvt_node, &shrink_empty_only);
    ALPM_IEG(rv);

    /* 7. SW: release temporary resources */
    alpm_util_free(bak_idx);
    alpm_util_free(pfx_arr);

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
        rv = alpm_cb_pvt_sanity_cb(u, acb, opvt_node, "SplitOpvt");
        if (BCM_SUCCESS(rv)) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, npvt_node, "SplitNpvt");
        }
    }

    if ((PVT_KEY_LEN(opvt_node) != 0 &&
        PVT_BKT_TRIE(opvt_node)->trie == NULL)) {
        alpm_lib_trie_node_t *tn = NULL;
        rv = alpm_pvt_delete_by_pvt_node(u, acb, opvt_node);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("alpm_pvt_delete_by_pvt_node(trie==NULL) failure\n"));
        }
        rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), opvt_node->key,
                         PVT_KEY_LEN(opvt_node), &tn);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("Trie delete node failure\n"));
        }
        (void)alpm_lib_trie_destroy(PVT_BKT_TRIE(opvt_node));
        alpm_util_free(tn);

        /* Set lpm_cfg the corresponding opvt_node pointer to NULL */
        if (lpm_cfg->spl_pn == opvt_node) {
            lpm_cfg->spl_pn = NULL;
        }
        if (lpm_cfg->l1_pvt_node == opvt_node) {
            lpm_cfg->l1_pvt_node = NULL;
        }
        if (lpm_cfg->pvt_node == opvt_node) {
            lpm_cfg->pvt_node = NULL;
        }
    }

    acb->acb_cnt.c_split++;
    if (BCM_SUCCESS(rv)) {
        ALPM_MERGE_STATE_CHANGED(u, vrf_id);
    }

    return rv;

bad:
    /* Clean new bucket */
    if (bak_idx != NULL) {
        alpm_bkt_pfx_idx_restore(u, pfx_arr, bak_idx);
        (void)alpm_bkt_pfx_clean(u, acb, npvt_node, pfx_arr->pfx_cnt, bak_idx);
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    /* free allocated bucket */
    if (bkt_allocated) {
        sal_memcpy(nbkt_info, &PVT_BKT_INFO(npvt_node), sizeof(_alpm_bkt_info_t));
    }
    alpm_bkt_free(u, acb, vrf_id, ipt, nbkt_info);

    if (nbkt_trie != NULL) {
        if (nbkt_trie->trie != NULL) {
            (void)alpm_lib_trie_merge(PVT_BKT_TRIE(opvt_node), nbkt_trie->trie,
                                      npvt_node->key, PVT_KEY_LEN(npvt_node));
        }

        alpm_lib_trie_destroy(nbkt_trie);
    }

    if (npvt_node != NULL) {
        alpm_util_free(npvt_node);
    }

    return rv;
}

int
alpm_cb_delete(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int ipt, vrf_id, pfx_len;
    int bkt_empty;
    uint32 *prefix;
    int shrink_empty_only = 1;

    _alpm_ppg_data_t ppg_data;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node = NULL;
    _alpm_bkt_node_t *alt_def_pfx = NULL;
    uint32           alt_bmp_len = 0;

    alpm_lib_trie_t *bkt_trie = NULL;
    alpm_lib_trie_node_t *tn = NULL;

    ipt     = ALPM_LPM_IPT(u, lpm_cfg);
    vrf_id  = ALPM_LPM_VRF_ID(u, lpm_cfg);
    prefix  = lpm_cfg->user_data;
    pfx_len = lpm_cfg->defip_sub_len;

    /* Check Init */
    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
        return BCM_E_PARAM;
    }

    pvt_node = ACB_HAS_TCAM(acb) ? lpm_cfg->l1_pvt_node : lpm_cfg->pvt_node;
    if (pvt_node == NULL) {
        ALPM_IEG(alpm_pvt_find(u, acb, lpm_cfg, &pvt_node));
    }

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_delete(bkt_trie, prefix, pfx_len, &tn);
    ALPM_IEG(rv);
    bkt_node = (_alpm_bkt_node_t *)tn;

    rv = alpm_vrf_pfx_trie_delete(u, acb, lpm_cfg, pvt_node,
                                  &alt_def_pfx, &alt_bmp_len);
    ALPM_IEG(rv);

    /* do insert propagation to update existing default datas */
    if (ACB_HAS_RTE(acb, vrf_id)) {
        /* Fill in AUX Scratch and perform PREFIX Operation */
        alpm_ppg_prepare(u, lpm_cfg, alt_bmp_len, alt_def_pfx, &ppg_data);
        rv = alpm_ppg_assoc_data(u, ALPM_PPG_DELETE, &ppg_data);
        ALPM_IEG(rv);
    }

    /* Delete pvt entry if bucket is empty */
    bkt_empty = (PVT_KEY_LEN(pvt_node) != 0 && bkt_trie->trie == NULL);
    if (bkt_empty) {
        rv = alpm_pvt_delete_by_pvt_node(u, acb, pvt_node);
        ALPM_IEG(rv);
    }

    /* Invalid the bucket entry */
    rv = alpm_bkt_pfx_clean(u, acb, pvt_node, 1, &bkt_node->ent_idx);
    ALPM_IEG(rv);

    if (ACB_BKT_WRA(acb, PVT_BKT_VRF(pvt_node))) {
        if (bkt_empty) {
            alpm_bkt_bnk_shrink(u, ipt, acb,
                    ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                    &PVT_BKT_INFO(pvt_node), NULL);
        } else {
            rv = alpm_bkt_pfx_shrink(u, acb, pvt_node, &shrink_empty_only);
            ALPM_IEG(rv);
        }
    } else {
        if (bkt_empty || bkt_trie->trie != NULL ||
            VRF_ROUTE_CNT(acb, vrf_id, ipt) == 0) {
            alpm_bkt_bnk_shrink(u, ipt, acb,
                    ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                    &PVT_BKT_INFO(pvt_node), NULL);
        }
    }

    alpm_util_free(bkt_node);

    if (bkt_empty) {
        rv = alpm_lib_trie_delete(ACB_PVT_TRIE(acb, vrf_id, ipt), pvt_node->key,
                         PVT_KEY_LEN(pvt_node), &tn);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("Trie delete node failure\n"));
        }

        (void)alpm_lib_trie_destroy(bkt_trie);
        alpm_util_free(pvt_node);
    } else {
        if (BCM_SUCCESS(rv) &&
            ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "CBDelete");
        }
    }

    if (BCM_SUCCESS(rv)) {
        ALPM_MERGE_STATE_CHANGED(u, vrf_id);
    }

    VRF_ROUTE_DEC(acb, vrf_id, ipt);

    return rv;

bad:
    /* Roll back ? */
    return rv;
}

uint32
alpm_cb_pvt_most_ref_len_get(int u, uint32 *pvt_len_cnt, int max_len)
{
    int i;
    int max_cnt = 0, max_ref_len = 0;

    for (i = 0; i < 128; i++) {
        if (i >= max_len) {
            break;
        }
        if (max_cnt < pvt_len_cnt[i]) {
            max_cnt = pvt_len_cnt[i];
            max_ref_len = i;
        }
    }
    return max_ref_len;
}

static int
alpm_trie_pfx_len_ref_cnt_cb(alpm_lib_trie_node_t *node, void *user_data)
{
    uint32 *pl_ref_cnt;

    pl_ref_cnt = (uint32 *)user_data;
    if (node->type == trieNodeTypePayload) {
        uint32 key_len = ((_alpm_bkt_node_t *)node)->key_len;
        pl_ref_cnt[key_len]++;
    }
    return BCM_E_NONE;
}

int
alpm_cb_path_construct(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int vrf_id, ipt;
    int fmt_update = 0;

    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ipt    = ALPM_LPM_IPT(u, lpm_cfg);

    if (lpm_cfg->l1_pvt_node != NULL && !ACB_HAS_TCAM(acb)) {
        _alpm_bkt_info_t bkt_info;
        _alpm_cb_t *top_cb = ACB_TOP(u, acb->app);
        _alpm_pvt_node_t *l1_pvt_node = lpm_cfg->l1_pvt_node;
        rv = alpm_bkt_ent_get(u, vrf_id, top_cb, l1_pvt_node,
                              l1_pvt_node->key_len, NULL, &fmt_update, 1);
        /* return FULL when Expand exceeds bnk_threshold */
        if (rv == BCM_E_RESOURCE) {
            return BCM_E_FULL;
        }

        if (rv == BCM_E_FULL) {
            sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
            rv = alpm_bnk_alloc(u, top_cb, vrf_id, ipt,
                                ACB_BNK_PER_BKT(top_cb, vrf_id),
                                &bkt_info, NULL);
            ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);
            rv = alpm_cb_split(u, top_cb, lpm_cfg, &bkt_info, 0xffff);
            if (rv == BCM_E_BUSY && ACB_HAS_RTE(acb, vrf_id) && lpm_cfg->spl_pn) {
                sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
                rv = alpm_bnk_alloc(u, acb, vrf_id, ipt,
                                    ACB_BNK_PER_BKT(acb, vrf_id),
                                    &bkt_info, NULL);
                ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);
                /* Case 2.2.2, split given l3 pvt_node first */
                rv = alpm_cb_split(u, acb, lpm_cfg, &bkt_info, 0xffff);
                ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);
                lpm_cfg->pvt_node = NULL;
                lpm_cfg->spl_pn = NULL;
                lpm_cfg->spl_key_len = 0;
            } else if (BCM_FAILURE(rv)) {
                ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);
            } else {
                lpm_cfg->pvt_node = NULL;
            }
            lpm_cfg->l1_pvt_node = NULL;
        } else {
            int most_ref_len = 0;
            uint32 pl_ref_cnt[128];
            _bcm_defip_cfg_t l2_lpm_cfg;
            _alpm_pvt_node_t *pvt_node;

            pvt_node = lpm_cfg->l1_pvt_node;
            sal_memcpy(&l2_lpm_cfg, lpm_cfg, sizeof(l2_lpm_cfg));

            sal_memset(pl_ref_cnt, 0, sizeof(pl_ref_cnt));
            rv = alpm_lib_trie_traverse(PVT_BKT_TRIE(pvt_node),
                        alpm_trie_pfx_len_ref_cnt_cb, pl_ref_cnt,
                        trieTraverseOrderIn, TRUE);
            if (BCM_SUCCESS(rv)) {
                most_ref_len = alpm_cb_pvt_most_ref_len_get(u, pl_ref_cnt,
                                    l2_lpm_cfg.defip_sub_len);
            }
            if (most_ref_len == 0) {
                most_ref_len = alpm_cb_pvt_most_ref_len_get(u,
                                    VRF_PVT_LEN_CNT_PTR(acb, vrf_id, ipt),
                                    l2_lpm_cfg.defip_sub_len);
            }

            if (pvt_node->key_len < most_ref_len &&
                most_ref_len < l2_lpm_cfg.defip_sub_len) {
                l2_lpm_cfg.defip_sub_len = most_ref_len;
            } else {
                l2_lpm_cfg.defip_sub_len = pvt_node->key_len;
            }
            l2_lpm_cfg.bkt_kshift = l2_lpm_cfg.defip_sub_len;
            alpm_util_ipmask_apply(u, &l2_lpm_cfg);
            alpm_trie_cfg_to_pfx(u, &l2_lpm_cfg, l2_lpm_cfg.user_data);
            if (PVT_BKT_DEF(pvt_node)) {
                alpm_util_adata_trie_to_cfg(u, &PVT_BKT_DEF(pvt_node)->adata,
                                            &l2_lpm_cfg);
                l2_lpm_cfg.default_miss = 0;
            } else {
                alpm_util_adata_zero_cfg(u, &l2_lpm_cfg);
                l2_lpm_cfg.default_miss = 1;
            }
            rv = alpm_cb_pvt_add(u, acb, vrf_id, ipt, &l2_lpm_cfg);
            if (BCM_FAILURE(rv)) {
                ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);
            } else {
                lpm_cfg->pvt_node = l2_lpm_cfg.pvt_node;
            }
            lpm_cfg->l1_pvt_node = NULL;
        }
    }

    return rv;
}

int
alpm_cb_insert(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 ipt = ALPM_LPM_IPT(u, lpm_cfg);
    uint32              *prefix, length;
    uint32              ent_idx;
    int                 retry_count = 0, retry_max = 3;
    int                 bkt_trie_added = 0;

    alpm_lib_trie_t              *bkt_trie = NULL;
    alpm_lib_trie_node_t         *tmp_node = NULL;

    _alpm_ppg_data_t    ppg_data;
    _alpm_bkt_node_t    *bkt_node = NULL;
    _alpm_pvt_node_t    *pvt_node, *upr_pvt_node;
    _alpm_tcam_write_t  tcam_write;

    /* Init ALPM structure for first insert or was DR partially inited (MC first) */
    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt) || ACB_DR_INITED(u, acb, vrf_id, ipt)) {
        uint8 db_type = alpm_util_route_type_get(u, lpm_cfg, acb->app);
        ALPM_IEG_PRT_EXCEPT(alpm_vrf_init(u, acb, vrf_id, ipt, db_type), BCM_E_FULL);
    }

    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        retry_max = 1;
    }

    pvt_node = ACB_HAS_TCAM(acb) ? lpm_cfg->l1_pvt_node : lpm_cfg->pvt_node;

retry:
    if (pvt_node == NULL) {
        ALPM_IEG(alpm_pvt_find(u, acb, lpm_cfg, &pvt_node));
        if (ACB_HAS_TCAM(acb)) {
            lpm_cfg->l1_pvt_node = pvt_node;
        } else {
            rv = alpm_pvt_find(u, ACB_UPR(u, acb), lpm_cfg, &upr_pvt_node);
            ALPM_IEG(rv);
            if (pvt_node && upr_pvt_node) {
                if (upr_pvt_node->key_len <= pvt_node->key_len) {
                    lpm_cfg->pvt_node = pvt_node;
                } else {
                    lpm_cfg->l1_pvt_node = upr_pvt_node;
                    ALPM_IEG_PRT_EXCEPT(alpm_cb_path_construct(u, acb, lpm_cfg), BCM_E_FULL);
                    pvt_node = lpm_cfg->pvt_node;
                    if (pvt_node == NULL) {
                        goto retry;
                    }
                }
            }
        }
    }

    /* entry shouldn't exist, insert the entry into the RAM */
    rv = alpm_bkt_insert(u, acb, lpm_cfg, pvt_node, &ent_idx);

    /* return FULL when Expand exceeds bnk_threshold */
    if (rv == BCM_E_RESOURCE) {
        rv = BCM_E_FULL;
        ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
    }

retry_spl:
    if (rv == BCM_E_FULL && retry_count < retry_max) {
        int     avl_bnk_cnt = 0;
        int     max_spl_cnt = 0xffff;
        _alpm_bkt_info_t bkt_info;
        _alpm_tcam_write_t *ptcam_write;

        /* Case of add new L1 pvt from L2 bkt split:
           This is only for 3-lvl ALPM (TH3).
           TCAM entry pending to write to HW until add new L2 bkt if it
           belongs to new L1 pvt (otherwise mis-forwarding may happen) */
        if (ACB_IDX(acb) != ACB_VRF_BTM_IDX(u, vrf_id, acb->app)) {
            tcam_write.hw_idx = -1; /* initialized as invalid */
            tcam_write.rv = 0xffff; /* init tcam_write.rv with 0xffff, will be changed
                                       to BCM_E_XXX in L2 bkt insert within L2 bkt split */
            lpm_cfg->tcam_write = &tcam_write;
        } else {
            lpm_cfg->tcam_write = NULL;
        }

        /* Perform bucket merge before allocation only for fixed fmt */
        if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
            int pvt_updated = 0;
            uint8 def_fmt;
            if (!bcm_esw_alpm_tcam_avail(u, acb->app, vrf_id, ipt, lpm_cfg->defip_sub_len, 0)) {
                rv = alpm_cb_merge(u, acb, vrf_id, ipt);
                ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
                /* pvt node could be merged and freed,
                 * Need a faster version ALPMTBD */
                pvt_node = NULL;
                goto retry;
            }
            sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
            rv = alpm_bkt_alloc(u, acb, vrf_id, ipt,
                                ACB_BNK_PER_BKT(acb, vrf_id),
                                &bkt_info, &avl_bnk_cnt, &pvt_updated);
            if (pvt_updated) {
                /* pvt node could be merged and freed,
                 * Need a faster version ALPMTBD */
                pvt_node = NULL;
                goto retry;
            } else if (rv == BCM_E_FULL) {
                if (avl_bnk_cnt > 0) {
                    sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
                    /* Should successfully get the avl_bnk_cnt */
                    rv = alpm_bkt_alloc(u, acb, vrf_id, ipt,
                                        avl_bnk_cnt, &bkt_info, NULL, NULL);
                }
                ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
            } else {
                avl_bnk_cnt = ACB_BNK_PER_BKT(acb, vrf_id);
            }

            def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
            max_spl_cnt = avl_bnk_cnt * ACB_FMT_ENT_MAX(acb, vrf_id, def_fmt) - 1;
        } else {
            sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
            rv = alpm_bnk_alloc(u, acb, vrf_id, ipt,
                                ACB_BNK_PER_BKT(acb, vrf_id),
                                &bkt_info, &avl_bnk_cnt);
            ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);
        }

        rv = alpm_cb_split(u, acb, lpm_cfg, &bkt_info, max_spl_cnt);
        /* Upper level of BCM_E_BUSY point,
         * ACB_HAS_RTE may not be a good condition, but considering
         * it's only 3 levels so it also works.
         */
        if (rv == BCM_E_BUSY && ACB_HAS_RTE(acb, vrf_id) && lpm_cfg->spl_pn) {
            sal_memset(&bkt_info, 0, sizeof(_alpm_bkt_info_t));
            rv = alpm_bnk_alloc(u, acb, vrf_id, ipt,
                                ACB_BNK_PER_BKT(acb, vrf_id),
                                &bkt_info, &avl_bnk_cnt);
            ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

            /* Case 2.2.2, split given l3 pvt_node first */
            rv = alpm_cb_split(u, acb, lpm_cfg, &bkt_info, max_spl_cnt);
            if (BCM_SUCCESS(rv)) {
                lpm_cfg->spl_pn = NULL;
                lpm_cfg->spl_key_len = 0;
                /* PERF OPT point */
                pvt_node = NULL;
                goto retry;
            }
        }

        ALPM_IEG_PRT_EXCEPT(rv, BCM_E_FULL);

        ptcam_write = lpm_cfg->tcam_write;
        if (ptcam_write != NULL) {
            if (ptcam_write->rv == BCM_E_NONE) {
                goto _exit; /* already done in alpm_bkt_add_to_npvt */
            } else if (ptcam_write->rv == BCM_E_FULL) {
                rv = BCM_E_FULL;
                retry_count ++;
                goto retry_spl; /* skip alpm_bkt_insert */
            }
        }

        /* Need a faster version ALPMTBD */
        pvt_node = NULL;
        retry_count ++;
        goto retry;
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
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_insert(bkt_trie, prefix, length, (alpm_lib_trie_node_t *)bkt_node);
    ALPM_IEG(rv);
    bkt_trie_added = 1;

    rv = alpm_vrf_pfx_trie_add(u, acb, lpm_cfg, bkt_node);
    ALPM_IEG(rv);

    /* do insert propagation to update existing default datas */
    if (ACB_HAS_RTE(acb, vrf_id)) {
        /* Fill in AUX Scratch and perform PREFIX Operation */
        alpm_ppg_prepare(u, lpm_cfg, 0, bkt_node, &ppg_data);
        rv = alpm_ppg_assoc_data(u, ALPM_PPG_INSERT, &ppg_data);
    }
    if (BCM_SUCCESS(rv) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
        rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "CBInsert");
    }

    VRF_ROUTE_ADD(acb, vrf_id, ipt);
_exit:
    lpm_cfg->tcam_write = NULL;
    if (rv == BCM_E_FULL) {
        acb->acb_cnt.c_full++;
    }
    return rv;

bad:
    if (bkt_trie_added) {
        (void)alpm_lib_trie_delete(bkt_trie, prefix, length, &tmp_node);
    }
    if (bkt_node != NULL) {
        alpm_util_free(bkt_node);
    }
    lpm_cfg->tcam_write = NULL;
    if (rv == BCM_E_FULL) {
        acb->acb_cnt.c_full++;
    }
    return rv;
}

static int
bcm_esw_alpm_ctrl_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPM_DRV(u)) {
        ALPM_DRV(u)->alpm_ctrl_deinit(u);
    }

    if (ALPMC(u)->_vrf_pfx_hdl != NULL) {
        alpm_util_free(ALPMC(u)->_vrf_pfx_hdl);
    }

    if (ALPMC(u)->_alpm_merge_state != NULL) {
        alpm_util_free(ALPMC(u)->_alpm_merge_state);
    }

    if (ALPMC(u)->_alpm_err_msg_buf != NULL) {
        alpm_util_free(ALPMC(u)->_alpm_err_msg_buf);
    }

    if (ALPMC(u) != NULL) {
        alpm_util_free(ALPMC(u));
        ALPMC(u) = NULL;
    }

    return rv;
}

int
bcm_esw_alpm_ctrl_init(int u)
{
    int rv = BCM_E_NONE;
    int vrf_id_cnt;
    int alloc_sz;
    int pair_ent_cnt = 0;
    int bnk_thres;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(u);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int def_ipv6_128b_entries = tcam_depth * max_tcams / 4;

    alloc_sz = sizeof(_alpm_ctrl_t);
    ALPM_ALLOC_EG(ALPMC(u), alloc_sz, "ALPMC");

    ALPMC(u)->_alpm_max_vrf_id = SOC_VRF_MAX(u);

    /* Init ALPMC variables */
    if (SAL_BOOT_SIMULATION && SOC_IS_TOMAHAWK3(u)) {
        ALPM_HIT_SKIP(u) = soc_property_get(u, spn_L3_ALPM_HIT_SKIP, 1);
    } else {
        ALPM_HIT_SKIP(u) = soc_property_get(u, spn_L3_ALPM_HIT_SKIP, 0);
    }
    ALPM_MODE(u) = soc_alpm_cmn_mode_get(u);
    if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) ||
        ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM)) {
        ALPM_TCAM_ZONED(u, APP0) = TRUE;
    }
    ALPM_TCAM_ZONED(u, ALPM_APP1) = FALSE;
    ALPM_TCAM_ZONED(u, ALPM_APP2) = FALSE;

    /* Due to swith control L3Max128BV6Entries, pair_ent_cnt
       shouldn't come directly from spn_NUM_IPV6_LPM_128B_ENTRIES */
    ALPM_128B(u) = SOC_ALPM_128B_ENABLE(u);
    pair_ent_cnt = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    if (pair_ent_cnt > (def_ipv6_128b_entries * 2)) {
        pair_ent_cnt = def_ipv6_128b_entries * 2;
    }

    ALPM_TCAM_PAIR_BLK_CNT(u) = (pair_ent_cnt + (tcam_depth - 1)) / tcam_depth;

    /* these two thresholds are applicable only for TH3 */
    ALPMC(u)->_alpm_bulk_wr_threshold =
        soc_property_get(u, "l3_alpm2_bulk_wr_threshold", 16);
    bnk_thres = soc_property_get(u, spn_L3_ALPM2_BNK_THRESHOLD, 95);
    if (bnk_thres <= 0 || bnk_thres > 100) {
        bnk_thres = 95;
    }
    ALPMC(u)->_alpm_bnk_threshold = bnk_thres;

    ALPMC(u)->_alpm_spl[ALPM_PKM_32B] =
        soc_property_get(u, "l3_alpm2_spl_hw", 24);
    ALPMC(u)->_alpm_spl[ALPM_PKM_64B] =
        soc_property_get(u, "l3_alpm2_spl_sw", 56);
    ALPMC(u)->_alpm_spl[ALPM_PKM_128] =
        soc_property_get(u, "l3_alpm2_spl_dw", 120);

    ALPMC(u)->_alpm_dbg_bmp = soc_property_get(u, "l3_alpm2_dbg_bmp", 0x0);
    if (ALPMC(u)->_alpm_dbg_bmp != 0) {
        int i;
        ALPM_INFO(("[ALPM-INIT] _alpm_dbg_bmp enabled bit(s) ["));
        for (i = 0; i < 32; i++) {
            if (ALPMC(u)->_alpm_dbg_bmp & (1 << i)) {
                ALPM_INFO(("%d ", i));
            }
        }
        ALPM_INFO(("]\n"));
    }

    ALPMC(u)->_alpm_dbg_info_bmp = soc_property_get(u, "l3_alpm2_dbg_info_bmp", 0x07);

    /* ALPM defrag on full config properties (TH3 only) */
    if (SOC_IS_TOMAHAWK3(u)) {
        ALPMC(u)->_alpm_defrag_on_full = soc_property_get(u, "l3_alpm2_defrag_on_full", 1);
        if (ALPMC(u)->_alpm_defrag_on_full) {
            ALPMC(u)->_alpm_defrag_max_time =
                soc_property_get(u, "l3_alpm2_defrag_max_time_ms", 1000);
            if (ALPMC(u)->_alpm_defrag_max_time < 200 &&
                ALPMC(u)->_alpm_defrag_max_time > 0) {
                ALPMC(u)->_alpm_defrag_max_time = 200;
            }
            if (ALPMC(u)->_alpm_defrag_max_time > 5000) {
                ALPMC(u)->_alpm_defrag_max_time = 5000;
            }
            ALPMC(u)->_alpm_defrag_max_l2cnt =
                soc_property_get(u, "l3_alpm2_defrag_max_l2cnt", -1);
            ALPMC(u)->_alpm_defrag_max_l3cnt =
                soc_property_get(u, "l3_alpm2_defrag_max_l3cnt", -1);
            if (ALPMC(u)->_alpm_defrag_max_l3cnt == 0) {
                ALPMC(u)->_alpm_defrag_max_l3cnt = -1;
            }
            if (ALPMC(u)->_alpm_defrag_max_l3cnt < 20 &&
                ALPMC(u)->_alpm_defrag_max_l3cnt > 0) {
                ALPMC(u)->_alpm_defrag_max_l3cnt = 20;
            }
            ALPMC(u)->_alpm_defrag_status = ALPM_DEFRAG_OK;
        }
    }

    /* Private VRFs + 2 Global VRFs */
    vrf_id_cnt = ALPM_VRF_ID_CNT(u);
    alloc_sz = sizeof(_alpm_pfx_ctrl_t) * vrf_id_cnt;
    ALPM_ALLOC_EG(ALPMC(u)->_vrf_pfx_hdl, alloc_sz, "_vrf_pfx_hdl");

    alloc_sz = sizeof(int) * vrf_id_cnt;
    ALPM_ALLOC_EG(ALPMC(u)->_alpm_merge_state, alloc_sz, "_alpm_merge_state");

    alloc_sz = _ALPM_ERR_MSG_BUF_CNT * _ALPM_ERR_MSG_BUF_ENT_SZ;
    ALPM_ALLOC_EG(ALPMC(u)->_alpm_err_msg_buf, alloc_sz, "_alpm_err_msg_buf");

    if (soc_feature(u, soc_feature_lpm_prefilter)) {
        ALPMC(u)->_alpm_acl_en = soc_property_get(u, spn_ALPM_PRE_FILTER, 0x0);
        SOC_CONTROL(u)->l3_defip_aacl = ALPMC(u)->_alpm_acl_en;
        if (ALPMC(u)->_alpm_acl_en) {
            if (soc_feature(u, soc_feature_alpm_flex_stat)) {
                /* When pre-filter is enabled, flex counter must be disabled. */
                rv = BCM_E_CONFIG;
                goto bad;
            }

            if (ALPM_TCAM_ZONED(u, APP0)) {
                /* When pre-filter is enabled, must be combined mode. */
                rv = BCM_E_CONFIG;
                goto bad;

            }
        }
    }

    if (soc_feature(u, soc_feature_alpm2)) {
        if (SOC_IS_TOMAHAWK3(u)) {
            ALPMC(u)->alpm_driver = &th3_alpm_driver;
        } else
        {
            ALPMC(u)->alpm_driver = &th_alpm_driver;
        }
    }

    if (ALPM_128B(u)) {
        ALPM_BKT_RSVD(u) = soc_property_get(u, spn_L3_ALPM_IPV6_128B_BKT_RSVD, 0);
        if (ALPM_BKT_RSVD(u)) {

            /* Soc config property l3_alpm_ipv6_bkt_rsvd_percent allows user
               to adjust V4 vs V6 bucket space. It's defined as percentage of
               ALPM bucket space reserved for V6. Default value is set to:
               TCAM[v6]/Total_TCAM like before */
            int v6_bkt_rsvd_percent = soc_property_get(u, "l3_alpm_ipv6_bkt_rsvd_percent",
                                                       -1);

            if (v6_bkt_rsvd_percent < 0 || v6_bkt_rsvd_percent > 100) {
                /* Invalid rsvd percent or not set */
                /* Based on 8K Buckets */
                ALPM_BKT_RSVD_CNT(u) = pair_ent_cnt * 2 * ALPM_BKT_RSVD_TOTAL /
                                       (tcam_depth * max_tcams);
            } else {
                /* Valid rsvd percent */
                /* Based on 8K Buckets */
                ALPM_BKT_RSVD_CNT(u) = v6_bkt_rsvd_percent * ALPM_BKT_RSVD_TOTAL / 100;
            }
        }
    }

    ALPM_DATA_AUTOSAVE(u) = soc_property_get(u, spn_ALPM_DATA_AUTOSAVE, 0);

    rv = ALPM_DRV(u)->alpm_ctrl_init(u);
    if (BCM_FAILURE(rv)) {
        goto bad;
    }

    return rv;

bad:
    bcm_esw_alpm_ctrl_deinit(u);
    return rv;
}

/* ALPM ctrl cleanup for faster delete_all */
int
bcm_esw_alpm_ctrl_cleanup(int u)
{
    int rv = BCM_E_NONE;
    int rv1;
    int vrf_id_cnt;
    int alloc_sz;

    rv = alpm_trie_clear_all(u);

    vrf_id_cnt = ALPM_VRF_ID_CNT(u);
    alloc_sz = sizeof(int) * vrf_id_cnt;
    sal_memset(ALPMC(u)->_alpm_merge_state, 0, alloc_sz);

    /*alloc_sz = _ALPM_ERR_MSG_BUF_CNT * _ALPM_ERR_MSG_BUF_ENT_SZ;
    sal_memset(ALPMC(u)->_alpm_err_msg_buf, 0, alloc_sz);*/

    if (ALPM_DRV(u)->alpm_ctrl_cleanup != NULL) {
        rv1 = ALPM_DRV(u)->alpm_ctrl_cleanup(u);
    } else {
        rv1 = BCM_E_UNAVAIL;
    }

    if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
        rv = rv1; /* report first error */
    }
    return rv;
}

/* Initialize ALPM Subsytem */

int
bcm_esw_alpm_deinit(int u)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(alpm_trie_clear_all(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_trace_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_hit_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_tcam_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_ctrl_deinit(u));
    alpm_util_mem_stat_clear();

    return rv;
}

static void
bcm_esw_alpm_init_done(int u, int rv)
{
    int i;
    int banks, mode, urpf;
    int app, app_cnt;
    char *mode_str[] = {
        "invalid",
        "combined",
        "parallel",
        "mixed"
    };
    char *app_str[] = {
        "forwarding",
        "pre-filter",
        "pre-filter"
    };
    int acl_done = FALSE;

    if (BCM_FAILURE(rv)) {
        LOG_CLI(("*** unit %d: alpm init failed\n", u));
        return;
    }

    /* pass */
    /* *** unit 0: alpm level 2 loaded: 4 banks in combined-128 (urpf) */
    app_cnt = alpm_app_cnt(u);
    urpf = SOC_URPF_STATUS_GET(u);
    mode = ALPM_MODE(u);

    for (app = 0; app < app_cnt; app++) {
        if (alpm_acl_app(app) && acl_done == TRUE) {
            continue;
        }

        for (i = 0; i < ACB_CNT(u); i++) {
            banks = ACB_PHY_BNK_PER_BKT(ACB(u, i, app));
            LOG_CLI(("*** unit %d: alpm %s level %d loaded: %d banks in %s-%s %s\n",
                     u, app_str[app], i+2, banks, mode_str[mode+1],
                     ALPM_128B(u) ? "128" : "64", urpf ? "(urpf)" : ""));

        }

        if (alpm_acl_app(app)) {
            acl_done = TRUE;
        }
    }
}

int
bcm_esw_alpm_init(int u)
{
    int rv = BCM_E_NONE;

    if (bcm_esw_alpm_inited(u)) {
        ALPM_IEG(bcm_esw_alpm_deinit(u));
    }

    ALPM_IEG(bcm_esw_alpm_ctrl_init(u));
    ALPM_IEG(bcm_esw_alpm_tcam_init(u));
    ALPM_IEG(bcm_esw_alpm_hit_init(u));
    ALPM_IEG(bcm_esw_alpm_trace_init(u));

    ALPMC(u)->_alpm_inited = TRUE;

bad:
    bcm_esw_alpm_init_done(u, rv);
    return rv;
}

int
bcm_esw_alpm_inited(int u)
{
    return (ALPMC(u) && ALPMC(u)->_alpm_inited) ? TRUE : FALSE;
}

/*
 * Function:
 *      bcm_esw_alpm_cleanup
 * Purpose:
 *      Fast ALPM delete all
 * Parameters:
 *      u         - (IN)Device unit number.
 *      trv_data  - (IN)Delete pattern + compare,act,notify routines for TCAM Direct routes.
 *      trv_data1 - (IN)Delete pattern + compare,act,notify routines for ALPM routes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_cleanup(int u, _bcm_l3_trvrs_data_t *trv_data,
                     _bcm_l3_trvrs_data_t *trv_data1)
{
    int rv = BCM_E_NONE;
    int rv1;
    int us = 0;

    if (bcm_esw_alpm_inited(u)) {

        if (!ALPM_HIT_SKIP(u) && ALPMDH(u)) {
            us = ALPMDH(u)->interval; /* save */
            rv = alpm_dist_hitbit_enable_set(u, 0);
        }

        /* Fast delete all by travel data in below order:
           1) TCAM direct routes + ecmp_nh (trv_data)
           2) TCAM Hw table
           3) ALPM ecmp_nh (trv_data1) */
        rv1 = bcm_esw_alpm_fast_delete_all(u, trv_data, trv_data1);
        if (BCM_FAILURE(rv1)) {
            ALPM_ERR(("ALPM fast_delete_all traverse failed (%d)\n", rv1));
        }
        if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
            rv = rv1; /* report first error */
        }

        rv1 = bcm_esw_alpm_ctrl_cleanup(u);
        if (BCM_FAILURE(rv1)) {
            ALPM_ERR(("ALPM ctrl cleanup failed (%d)\n", rv1));
        }
        if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
            rv = rv1; /* report first error */
        }

        rv1 = bcm_esw_alpm_tcam_cleanup(u);
        if (BCM_FAILURE(rv1)) {
            ALPM_ERR(("ALPM tcam cleanup failed (%d)\n", rv1));
        }
        if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
            rv = rv1; /* report first error */
        }

        if (!ALPM_HIT_SKIP(u) && ALPMDH(u)) {
            if (ALPM_DRV(u)->alpm_hit_cleanup != NULL) {
                rv1 = ALPM_DRV(u)->alpm_hit_cleanup(u);
            } else {
                rv1 = BCM_E_UNAVAIL;
            }
            if (BCM_FAILURE(rv1)) {
                ALPM_ERR(("ALPM hitbit cleanup failed (%d)\n", rv1));
            }
            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }

            rv1 = alpm_dist_hitbit_enable_set(u, us);
            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }
        }

        /* #define ALPM_TRACE_OP_DELETE_ALL 2 */
        bcm_esw_alpm_trace_log(u, 2, NULL, 0, rv);
    }

    return rv;
}

/*
 * bcm_esw_alpm_insert
 * ALPM entry insertion. For variable fmt, resource check and
 * free by defrag if insert FULL and retry insertion.
 */
int
bcm_esw_alpm_insert(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    _alpm_cb_t *acb = ACB_VRF_BTM(u, vrf_id, app);
    _bcm_defip_cfg_t lpm_cfg0;

    /* save original lpm_cfg */
    if (ALPMC(u)->_alpm_defrag_on_full) {
        sal_memcpy(&lpm_cfg0, lpm_cfg, sizeof(lpm_cfg0));
    }

    rv = bcm_esw_alpm_add(u, lpm_cfg);

    if (ALPMC(u)->_alpm_defrag_on_full && rv == BCM_E_FULL) {
        int rv1;
         /* Check the resource and free */
        rv1 = bcm_esw_alpm_resource_check_and_free(u, lpm_cfg);
        if (BCM_SUCCESS(rv1)) {
            /* restore original lpm_cfg and lookup again
               to get new pvt_node & l1_pvt_node after defrag */
            sal_memcpy(lpm_cfg, &lpm_cfg0, sizeof(lpm_cfg0));
            rv1 = bcm_esw_alpm_lookup(u, lpm_cfg);

            if (BCM_FAILURE(rv1)) {
                if (rv1 == BCM_E_NOT_FOUND) { /* new route */
                    rv = bcm_esw_alpm_add(u, lpm_cfg);
                } else {
                    ALPM_ERR(("ALPM re-lookup failed (%d)\n", rv1));
                }
            }
        } else {
            if (rv1 != BCM_E_FULL) {
                ALPM_ERR(("ALPM resource check and free failed (%d)\n", rv1));
            }
        }

        if (rv == BCM_E_FULL) { /* inc FullAfterDefrag count */
            acb->acb_cnt.c_fullafterdefrag++;
        }
    }

    return(rv);
}

/*
 * bcm_esw_alpm_add
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
bcm_esw_alpm_add(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE, rv2;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int                 pvt_info = lpm_cfg->defip_alpm_cookie;
    _alpm_cb_t          *acb = NULL;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    alpm_util_ipmask_apply(u, lpm_cfg);
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    /* Routes reside in TCAM only in below cases:
     *   Global High routes
     *   Global low routes in Mixed mode
     */
    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        /* Init pvt_trie for direct routes */
        acb = ACB_TOP(u, app);
        if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
            ALPM_IER(alpm_pvt_trie_init(u, app, vrf_id, ipt));
        }

        /* Insert into TCAM directly and return */
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg, ALPM_PVT_UPDATE_ALL);
        if (BCM_SUCCESS(rv)) {
            if (!(pvt_info & BCM_ALPM_LPM_LOOKUP_HIT)) {
                VRF_ROUTE_ADD(acb, vrf_id, ipt);
            }
            /* Insert only Direct route pvt trie */
            rv = alpm_pvt_trie_insert(u, lpm_cfg);
            if (rv == BCM_E_EXISTS &&
                (lpm_cfg->defip_flags & BCM_L3_REPLACE)) {
                rv = BCM_E_NONE;
            }
        }
        if (rv == BCM_E_FULL) {
            acb->acb_cnt.c_full++;
        }
        return(rv);
    }

    /* Route mode and default route validation */
    ALPM_IER(alpm_util_route_type_check(u, lpm_cfg));
    ALPM_IER(alpm_util_def_check(u, lpm_cfg, TRUE));
    acb = ACB_VRF_BTM(u, vrf_id, app);

    /* Find and update the entry */
    if (pvt_info & BCM_ALPM_LOOKUP_HIT) {
        rv = alpm_cb_update(u, acb, lpm_cfg);
        return rv;
    }

    /* entry is not found in database */
    /* Insert the entry into the database, if FULL, split the bucket */
    /* Insert prefix into trie */
    /* Split trie : Insertion into trie results into Split */
    /* Allocate a TCAM entry for PIVOT and bucket and move entries */
    rv = alpm_cb_path_construct(u, acb, lpm_cfg);
    if (rv == BCM_E_FULL) {
        acb->acb_cnt.c_full++;
    }
    ALPM_IER_PRT_EXCEPT(rv, BCM_E_FULL);

    rv = alpm_cb_insert(u, acb, lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        lpm_cfg->defip_index =
            alpm_util_ent_phy_idx_get(u, acb, vrf_id, lpm_cfg->defip_index);
    } else if (rv != BCM_E_FULL) {
        ALPM_ERR(("**ACB(%d).INSERT: Failed %d\n", ACB_IDX(acb), rv));
    } else { /* FULL */
        int rv3;
        rv3 = alpm_pvt_delete_by_empty_bkt(u, acb, lpm_cfg);
        if (BCM_FAILURE(rv3)) {
            ALPM_ERR(("**ACB(%d).INSERT: Full. "
                      "Pvt delete by empty bkt: Failed %d\n", ACB_IDX(acb), rv3));
        }
    }

    rv2 = alpm_cb_sanity(u, app, 0x3, vrf_id, ipt);
    rv = (rv == BCM_E_NONE) ? rv2 : rv;

    return(rv);
}

/*
 * bcm_esw_alpm_lookup
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
bcm_esw_alpm_lookup(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    _alpm_cb_t          *acb = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL, *upr_pvt_node = NULL;
    _alpm_bkt_node_t    *bkt_node = NULL, *upr_bkt_node = NULL;
    int                 ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    alpm_util_ipmask_apply(u, lpm_cfg);

    /* Prepare trie key format in lpm_cfg->user_data for further use */
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        if (!ACB_VRF_INITED(u, ACB_TOP(u, app), vrf_id, ipt)) {
            ALPM_VERB(("**LOOKUP:VRF %d is not initialized\n", vrf_id));
            return BCM_E_NOT_FOUND;
        }
        rv = bcm_esw_alpm_tcam_match(u, lpm_cfg, &lpm_cfg->defip_index);
        if (BCM_SUCCESS(rv)) {
            lpm_cfg->defip_alpm_cookie = BCM_ALPM_LPM_LOOKUP_HIT;
        }
        return rv;
    }

    acb = ACB_VRF_BTM(u, vrf_id, app);

    /* Check VRF/Key_mode based initialization */
    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
        ALPM_VERB(("**LOOKUP:VRF %d is not initialized\n", vrf_id));
        return BCM_E_NOT_FOUND;
    }

    /* Find and update the entry */
    rv = alpm_cb_find(u, acb, lpm_cfg, &pvt_node, &bkt_node);
    if (!ACB_HAS_TCAM(acb)) {
        (void)alpm_cb_find(u, ACB_UPR(u, acb), lpm_cfg,
                           &upr_pvt_node, &upr_bkt_node);
        if (pvt_node && upr_pvt_node) {
            if (upr_pvt_node->key_len <= pvt_node->key_len) {
                lpm_cfg->pvt_node = (uint32 *)pvt_node;
                lpm_cfg->bkt_node = (uint32 *)bkt_node;
                lpm_cfg->l1_pvt_node = NULL;
            } else {
                lpm_cfg->pvt_node = NULL;
                lpm_cfg->bkt_node = NULL;
                lpm_cfg->l1_pvt_node = (uint32 *)upr_pvt_node;
                rv = BCM_E_NOT_FOUND;
            }
        }
    } else {
        lpm_cfg->l1_pvt_node = (uint32 *)pvt_node;
        lpm_cfg->pvt_node = (uint32 *)pvt_node;
        lpm_cfg->bkt_node = (uint32 *)bkt_node;
    }

    if (BCM_SUCCESS(rv)) {
        /* Copy ALPM memory to LPM entry */
        alpm_util_adata_trie_to_cfg(u, &bkt_node->adata, lpm_cfg);
        lpm_cfg->defip_index =
            alpm_util_ent_phy_idx_get(u, acb, vrf_id, bkt_node->ent_idx);
        lpm_cfg->defip_alpm_cookie = BCM_ALPM_LOOKUP_HIT;
        if (ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt)) {
            lpm_cfg->defip_alpm_cookie |= ALPM_ROUTE_FULL_MD;
        }
        /* Get HIT bit from matched entry */
        if (!ALPM_HIT_SKIP(u)) {
            int idx, ent;
            int hit_val;
            _alpm_tbl_t tbl;
            ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
            idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
            tbl = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id));
            ALPM_HIT_LOCK(u);
            hit_val = alpm_rte_hit_get(u, app, vrf_id, tbl, idx, ent);
            lpm_cfg->defip_flags |= (hit_val > 0 ? BCM_L3_HIT : 0);
            ALPM_HIT_UNLOCK(u);
        }
    }

    return(rv);
}

/*
 * bcm_esw_alpm_delete
 */
int
bcm_esw_alpm_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int     rv = BCM_E_NONE;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node;

    uint32  ipt = ALPM_LPM_IPT(u, lpm_cfg);
    uint32  vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    alpm_util_ipmask_apply(u, lpm_cfg);
    /* ALPM prefix */
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    /* Prefix resides in TCAM directly */
    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        acb = ACB_TOP(u, app);
        rv = bcm_esw_alpm_tcam_delete(u, lpm_cfg);

        if (BCM_SUCCESS(rv)) {
            /* Delete only pvt trie for direct routes */
            rv = alpm_pvt_trie_delete(u, lpm_cfg);
            VRF_ROUTE_DEC(acb, vrf_id, ipt);
            if (VRF_ROUTE_CNT(acb, vrf_id, ipt) == 0) {
                (void)alpm_lib_trie_destroy(ACB_PVT_TRIE(acb, vrf_id, ipt));
                ACB_VRF_INIT_CLEAR(u, acb, vrf_id, ipt);
                ACB_DR_INIT_CLEAR(u, acb, vrf_id, ipt);
                ACB_PVT_TRIE(acb, vrf_id, ipt) = NULL;
            }
        }
        return rv;
    }

    rv = alpm_util_def_check(u, lpm_cfg, FALSE);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    acb = ACB_VRF_BTM(u, vrf_id, app);

    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
        ALPM_ERR(("**ACB(%d).DELETE:VRF %d/%d is not initialized\n",
                  ACB_IDX(acb), vrf_id, ipt));
        return BCM_E_NONE;
    }

    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    bkt_node = (_alpm_bkt_node_t *)lpm_cfg->bkt_node;

    if (bkt_node == NULL) {
        _alpm_pvt_node_t *upr_pvt_node = NULL;
        _alpm_bkt_node_t *upr_bkt_node = NULL;

        (void)alpm_cb_find(u, acb, lpm_cfg, &pvt_node, &bkt_node);
        if (!ACB_HAS_TCAM(acb)) {
            (void)alpm_cb_find(u, ACB_UPR(u, acb), lpm_cfg,
                               &upr_pvt_node, &upr_bkt_node);
            if (pvt_node && upr_pvt_node) {
                if (upr_pvt_node->key_len <= pvt_node->key_len) {
                    lpm_cfg->pvt_node = (uint32 *)pvt_node;
                    lpm_cfg->bkt_node = (uint32 *)bkt_node;
                    lpm_cfg->l1_pvt_node = NULL;
                } else {
                    lpm_cfg->pvt_node = NULL;
                    lpm_cfg->bkt_node = NULL;
                    lpm_cfg->l1_pvt_node = (uint32 *)upr_pvt_node;
                    bkt_node = NULL;
                }
            }
        } else {
            lpm_cfg->l1_pvt_node = (uint32 *)pvt_node;
            lpm_cfg->pvt_node = (uint32 *)pvt_node;
            lpm_cfg->bkt_node = (uint32 *)bkt_node;
        }
    }

    if (bkt_node == NULL) {
        lpm_cfg->pvt_node = NULL;
        return BCM_E_NOT_FOUND;
    }

    rv = alpm_cb_delete(u, acb, lpm_cfg);

    if (BCM_SUCCESS(rv) && VRF_ROUTE_CNT(acb, vrf_id, ipt) == 0) {
        rv = alpm_vrf_deinit(u, app, vrf_id, ipt);
    }

    return(rv);
}

/*
 * bcm_esw_alpm_update
 */
int
bcm_esw_alpm_update(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv;

    rv = bcm_esw_alpm_lookup(u, lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_alpm_add(u, lpm_cfg);
    }

    return rv;
}

/* Get current ALPM route data mode in the VRF and IP type */
int
bcm_esw_alpm_vrf_data_mode_get(int u, bcm_vrf_t vrf, uint32 flags,
                               bcm_l3_vrf_route_data_mode_t *mode)
{
    int ipt, vrf_id;
    _alpm_cb_t *acb;

    if (bcm_esw_alpm_inited(u)) {
        vrf_id = ALPM_VRF_TO_VRFID(u, vrf);
        ipt = (flags & BCM_L3_IP6) ? ALPM_IPT_V6 : ALPM_IPT_V4;

        acb = ACB_VRF_BTM(u, vrf_id, APP0);
        *mode = (bcm_l3_vrf_route_data_mode_t) ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt);
        ALPM_INFO(("bcm_esw_alpm_vrf_data_mode_get(VRF:%d) = %d\n", vrf_id, *mode));
        return BCM_E_NONE;
    } else {
        return BCM_E_UNAVAIL;
    }
}

/* Set ALPM route data mode in the VRF and IP type */
int
bcm_esw_alpm_vrf_data_mode_set(int u, bcm_vrf_t vrf, uint32 flags,
                               bcm_l3_vrf_route_data_mode_t mode)
{
    int i, ipt, vrf_id;
    _alpm_cb_t *acb;
    bcm_l3_vrf_route_data_mode_t cur_mode;
    int app, app_cnt = alpm_app_cnt(u);

    if (bcm_esw_alpm_inited(u)) {
        vrf_id = ALPM_VRF_TO_VRFID(u, vrf);
        ipt = (flags & BCM_L3_IP6) ? ALPM_IPT_V6 : ALPM_IPT_V4;

        /* Direct routes is always Full mode */
        if (!ALPM_VRF_ID_HAS_BKT(u, vrf_id)) {
            if (mode == bcmL3VrfRouteDataModeFull) {
                ALPM_INFO(("Direct route is always Full mode (VRF:%d)\n", vrf_id));
                return BCM_E_NONE;
            } else {
                ALPM_ERR(("Direct route should be always Full mode (VRF:%d)\n", vrf_id));
                return BCM_E_FAIL;
            }
        }

        for (app = 0; app < app_cnt; app++) {
            for (i = 0; i < ACB_CNT(u); i++) {
                acb = ACB(u, i, app);
                cur_mode = (bcm_l3_vrf_route_data_mode_t)
                                ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt);
                if (mode == cur_mode) {
                    return BCM_E_NONE; /* no change */
                }

                /* ALPMTBD: initial implementation -
                   not allow to change once VRF initialized */
                if (ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    ALPM_ERR(("VRF:%d IPT:%d ALPM route data mode unable to change once"
                              " VRF initialized\n", vrf_id, ipt));
                    return BCM_E_FAIL;
                }

                ACB_VRF_DB_TYPE_SET(u, acb, vrf_id, ipt, (uint8) mode);
                alpm_vrf_spl_len_calc(u, acb, vrf_id, (uint8) mode,
                          &ACB_VRF_SPLEN_DIFF1(u, acb, vrf_id, ipt),
                          &ACB_VRF_SPLEN_DIFF2(u, acb, vrf_id, ipt));

            }
        }

        ALPM_INFO(("bcm_esw_alpm_vrf_data_mode_set(VRF:%d) = %d\n", vrf_id, mode));
        return BCM_E_NONE;
    } else {
        return BCM_E_UNAVAIL;
    }
}

static int
bcm_esw_alpm_pvt_trav_int_cb(alpm_lib_trie_node_t *trie, void *datum)
{
    int rv = BCM_E_NONE;
    _alpm_pvt_trav_t    *trav_st;
    _alpm_pvt_node_t    *pvt_node;

    trav_st = (_alpm_pvt_trav_t *)datum;
    if (trie->type == trieNodeTypePayload) {
        pvt_node = (_alpm_pvt_node_t *)trie;
        rv = trav_st->user_cb(trav_st->acb->unit, trav_st->acb,
                              pvt_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given pivot trie-tree
 */
int
bcm_esw_alpm_pvt_traverse(int u, int app, int acb_idx, int vrf_id, int ipt,
                          bcm_esw_alpm_pvt_trav_cb trav_fn, void *user_data)
{
    int rv = BCM_E_NONE;
    _alpm_cb_t        *acb;
    _alpm_pvt_trav_t  trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    acb = ACB(u, acb_idx, app);

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.acb       = acb;
    trav_st.user_cb   = trav_fn;
    trav_st.user_data = user_data;

    rv = alpm_lib_trie_traverse(ACB_PVT_TRIE(acb, vrf_id, ipt), bcm_esw_alpm_pvt_trav_int_cb,
                       (void *)&trav_st, trieTraverseOrderIn, TRUE);
    return rv;
}

STATIC int
bcm_esw_alpm_pfx_trav_int_cb(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                             alpm_lib_trie_traverse_state_t *state, void *info)
{
    int rv = BCM_E_NONE;
    _alpm_pfx_trav_t    *trav_st;
    _alpm_pfx_node_t    *pfx_node;

    /* Reset state everytime */
    *state = trieTraverseStateNone;

    trav_st = (_alpm_pfx_trav_t *)info;
    if (trie->type == trieNodeTypePayload) {
        pfx_node = (_alpm_pfx_node_t *)trie;
        rv = trav_st->user_cb(pfx_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given prefix trie-tree
 */
int
bcm_esw_alpm_pfx_traverse(int u, int app, int vrf_id, int ipt,
                          bcm_esw_alpm_pfx_trav_cb trav_fn, void *user_data)
{
    int rv = BCM_E_NONE;
    _alpm_pfx_trav_t        trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;

    /* Use delete safe _trie_postorder_traverse2() for update_match */
    rv = alpm_lib_trie_traverse2(ALPM_VRF_TRIE(u, app, vrf_id, ipt),
                  bcm_esw_alpm_pfx_trav_int_cb,
                  (void *)&trav_st, trieTraverseOrderPost);

    return rv;
}

STATIC int
bcm_esw_alpm_bkt_trav_int_cb(alpm_lib_trie_node_t *trie, void *datum)
{
    int rv = BCM_E_NONE;
    _alpm_bkt_trav_t    *trav_st;
    _alpm_bkt_node_t    *bkt_node;

    trav_st = (_alpm_bkt_trav_t *)datum;
    if (trie->type == trieNodeTypePayload) {
        bkt_node = (_alpm_bkt_node_t *)trie;
        rv = trav_st->user_cb(bkt_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given bkt trie-tree
 */
int
bcm_esw_alpm_bkt_traverse(alpm_lib_trie_t *bkt_trie,
                          bcm_esw_alpm_bkt_trav_cb trav_fn, void *user_data)
{
    _alpm_bkt_trav_t        trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;

    alpm_lib_trie_traverse(bkt_trie, bcm_esw_alpm_bkt_trav_int_cb,
                  (void *)&trav_st, trieTraverseOrderIn, TRUE);

    return BCM_E_NONE;
}

/* Get count for given pivot trie-tree */
int
bcm_esw_alpm_pvt_count_get(int u, _alpm_pvt_node_t *pvt_node, int *count)
{
    int rv = BCM_E_NONE;
    if (count && PVT_BKT_TRIE(pvt_node) && PVT_BKT_TRIE(pvt_node)->trie) {
        *count = PVT_BKT_TRIE(pvt_node)->trie->count;
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

void
bcm_esw_bkt_node_flexctr_attr_get(uint32 *bnode, int *base_idx, int *ofst_mod, int *pool_num)
{
    _alpm_bkt_node_t *bkt_node;

    bkt_node = (_alpm_bkt_node_t *)bnode;
    if (bkt_node) {
        *base_idx = bkt_node->adata.defip_flex_ctr_base_id;
        *ofst_mod = bkt_node->adata.defip_flex_ctr_mode;
        *pool_num = bkt_node->adata.defip_flex_ctr_pool;
    }
    return ;
}

void
bcm_esw_bkt_node_flexctr_attr_set(uint32 *bnode, int base_idx, int ofst_mod, int pool_num)
{
    _alpm_bkt_node_t *bkt_node;

    bkt_node = (_alpm_bkt_node_t *)bnode;
    if (bkt_node) {
        bkt_node->adata.defip_flex_ctr_base_id = base_idx;
        bkt_node->adata.defip_flex_ctr_mode = ofst_mod;
        bkt_node->adata.defip_flex_ctr_pool = pool_num;
    }
    return ;
}

/*
 * Function:
 *      bcm_esw_alpm_route_capacity_get
 * Purpose:
 *      Get the min or max capacity for ALPM routes from tables:
 * L3_DEFIP_ALPM_IPV4, L3_DEFIP_ALPM_IPV6_64, L3_DEFIP_ALPM_IPV6_128
 *
 * Parameters:
 *      u           - Device unit
 *      mem         - Legacy memory type
 *      max_entries - Maximum result returned.
 *      min_entries - Minimum result returned.
 */
int
bcm_esw_alpm_route_capacity_get(int u, soc_mem_t mem,
                                int *max_entries, int *min_entries)
{
    int rv = BCM_E_NONE;

    if (max_entries == NULL && min_entries == NULL) {
        return BCM_E_PARAM;
    }

    rv = ALPM_DRV(u)->alpm_cap_get(u, mem, max_entries, min_entries, APP0);

    return rv;
}

/* Trouble-shooting helper functions */

static int
alpm_cb_pvt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int i, j, count = 0;
    int index; /* for next level bkt index */
    int idx, sub_idx;
    _alpm_bkt_info_t *bkt_info;
    char vet_str[ALPM_BPB_MAX*2+2] = {0};
    alpm_pvt_trav_cb_data_t *cb_data;

    cb_data = (alpm_pvt_trav_cb_data_t *)datum;
    (void)bcm_esw_alpm_pvt_count_get(u, pvt_node, &count);

    bkt_info = &PVT_BKT_INFO(pvt_node);
    if (cb_data->bkt_idx == -1 ||
        cb_data->bkt_idx == PVT_BKT_IDX(pvt_node)) {
        ALPM_IER(alpm_util_pvt_idx_get(u, acb, pvt_node, &idx, &sub_idx));
        cli_out("Level%d pvt_idx:%d.%d | bkt_info rofs:%d idx:%d total %d\n",
                acb->acb_idx + 1, idx, sub_idx,
                PVT_ROFS(pvt_node), PVT_BKT_IDX(pvt_node), count);
        for (i = 0; i < ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node)); i++) {
            for (j = 0; j < 16; j++) {
                if (bkt_info->vet_bmp[i] & (1 << j)) {
                    vet_str[j] = '*';
                } else {
                    vet_str[j] = '_';
                }
            }
            index = ALPM_TAB_IDX_GET_BKT_BNK(acb, PVT_ROFS(pvt_node),
                                             PVT_BKT_IDX(pvt_node), i);
            if (bkt_info->bnk_fmt[i] != 0) {
                cli_out("----> Bank %d: FMT%d[%s] index:%5d\n", i,
                        bkt_info->bnk_fmt[i], vet_str, index);
            }
        }
    }

    return 0;
}

int
alpm_cb_pvt_dump(int u, int app, int acb_bmp, int bkt_idx)
{
    int i, vrf_id, ipt, rv = BCM_E_NONE;
    _alpm_cb_t *acb;
    alpm_pvt_trav_cb_data_t *cb_data;

    ALPMC_INIT_CHECK(u);

    ALPM_ALLOC_EG(cb_data, sizeof(alpm_pvt_trav_cb_data_t), "cb_data");

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        acb = ACB(u, i, app);
        cb_data->unit = u;
        cb_data->acb = acb;
        cb_data->bkt_idx = bkt_idx;
        for (ipt = 0; ipt < ALPM_IPT_CNT; ipt ++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    continue;
                }
                cli_out("ALPM CB[%d] Dumping Pivot vrf_id %d, ipt %s\n", i,
                        vrf_id, alpm_util_ipt_str[ipt]);
                (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                        alpm_cb_pvt_dump_cb, cb_data);
            }
        }
    }

    sal_free(cb_data);

bad:
    return rv;
}

static int
alpm_cb_bkt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int rv = BCM_E_NONE;
    int i, j, ent_bit, ent_shift;
    uint8 bnk_fmt;

    _alpm_bkt_info_t    *bkt_info;
    alpm_pvt_trav_cb_data_t *cb_data;

#define _ENT_BITS   5

    cb_data = (alpm_pvt_trav_cb_data_t *)datum;
    ent_shift = _shr_popcount(ALPM_BPB_MAX - 1) + _ENT_BITS;
    bkt_info = &PVT_BKT_INFO(pvt_node);
    SHR_BITSET(cb_data->bkt, PVT_BKT_IDX(pvt_node));

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i)) {
            int bnk_idx;
            if (i < PVT_ROFS(pvt_node)) {
                bnk_idx =
                    ((PVT_BKT_IDX(pvt_node) + 1) <<
                    _shr_popcount(ALPM_BPB_MAX - 1) | i);
            } else {
                bnk_idx =
                    (PVT_BKT_IDX(pvt_node) <<
                    _shr_popcount(ALPM_BPB_MAX - 1) | i);
            }
            SHR_BITSET(cb_data->bnk, bnk_idx);
            cb_data->bnkfmt[bnk_idx] = bnk_fmt;
        }
        for (j = 0; j < sizeof(bkt_info->vet_bmp[0]) * 8; j++) {
            if (SHR_BITGET(&bkt_info->vet_bmp[i], j)) {
                ent_bit = (i << _ENT_BITS) | j;
                if (i < PVT_ROFS(pvt_node)) {
                    ent_bit = ((PVT_BKT_IDX(pvt_node) + 1) << ent_shift) | ent_bit;
                } else {
                    ent_bit = (PVT_BKT_IDX(pvt_node) << ent_shift) | ent_bit;
                }
                SHR_BITSET(cb_data->ent, ent_bit);
            }
        }
    }

    return rv;
}

static void
alpm_cb_bkt_pool_dump(int u, int ipt, _alpm_cb_t *acb,
                      _alpm_bkt_pool_conf_t *bp_conf,
                      char *prefix)
{
    int j, k;
    int bnk_cnt, total_bnk_cnt, bkt_cnt;
    SHR_BITDCL *bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);

#define BKT_PER_BLK 64

    bkt_cnt = ACB_BKT_CNT(acb);

    cli_out("  %s %s BANK POOL USAGE\n", prefix, alpm_util_ipt_str[ipt]);
    cli_out("------------------------------------------------------------"
            "-------------------------------\n");
    for (j = 0; j < bkt_cnt / BKT_PER_BLK; j++) {
        int tmp_bnk = 0;
        total_bnk_cnt = 0;
        cli_out("BKT%04d-%04d [", j * BKT_PER_BLK, (j+1) * BKT_PER_BLK - 1);
        for (k = j * BKT_PER_BLK; k < (j+1) * BKT_PER_BLK; k++) {
            SHR_BITCOUNT_RANGE(bnk_bmp, bnk_cnt,
                k * BPC_BNK_PER_BKT(bp_conf), BPC_BNK_PER_BKT(bp_conf));
            total_bnk_cnt += bnk_cnt;
            if (bnk_cnt >= 16) {
                cli_out("@");
            } else {
                cli_out("%x", bnk_cnt);
            }
        }
        tmp_bnk =
            total_bnk_cnt * 1000 / (BPC_BNK_PER_BKT(bp_conf) * BKT_PER_BLK);
        cli_out("] %2d.%d%%\n", tmp_bnk / 10, tmp_bnk % 10);
    }
}

static void
alpm_cb_bkt_bnk_dump(int u, int ipt, _alpm_cb_t *acb,
                     _alpm_bkt_pool_conf_t *bp_conf, char *prefix)
{
    int rv = BCM_E_NONE, j, k, bnk, vrf_id, ipt1;
    int vrf_min, vrf_max;
    int bnk_cnt, total_bnk_cnt;
    int ent_cnt, total_ent_cnt, max_ent_cnt;
    int bkt_cnt;

    alpm_pvt_trav_cb_data_t *cdata = NULL;

#define BKT_PER_BLK 64
#define _ENT_BITS   5

    bkt_cnt = ACB_BKT_CNT(acb);

    ALPM_ALLOC_EG(cdata, sizeof(*cdata), "bktusage");

    if (ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u)) ==
        ACB_BKT_VRF_POOL(acb, 1)) {
        vrf_min = 0;
        vrf_max = ALPM_VRF_ID_GLO(u);
    } else if (bp_conf == ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u))) {
        vrf_min = ALPM_VRF_ID_GLO(u);
        vrf_max = ALPM_VRF_ID_GLO(u);
    } else {
        vrf_min = 0;
        vrf_max = ALPMC(u)->_alpm_max_vrf_id;
    }

    for (ipt1 = ipt; ipt1 < ALPM_IPT_CNT; ipt1++) {
        if ((ipt1 == ipt) ||
            (ipt1 > ipt && /* same pool case (V4/V6) */
            BPC_BNK_INFO(bp_conf, ipt1) ==
            BPC_BNK_INFO(bp_conf, ipt1 - 1))) {

            for (vrf_id = vrf_min; vrf_id <= vrf_max; vrf_id++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt1)) {
                    continue;
                }
                (void)bcm_esw_alpm_pvt_traverse(u, acb->app, acb->acb_idx, vrf_id, ipt1,
                        alpm_cb_bkt_dump_cb, cdata);
            }
        }
    }

    cli_out("  BKT RANGE |[---------------- banks used per bucket(%7s %3s) -"
            "-----------] AVGBK AVGET\n", prefix, alpm_util_ipt_str[ipt]);
    cli_out("------------------------------------------------------------"
            "-------------------------------\n");
    for (j = 0; j < bkt_cnt / BKT_PER_BLK; j++) {
        int tmp_bnk, tmp_ent;
        total_bnk_cnt = 0;
        total_ent_cnt = 0;
        max_ent_cnt   = 0;
        cli_out("BKT%04d-%04d [", j * BKT_PER_BLK, (j+1) * BKT_PER_BLK - 1);
        for (k = j * BKT_PER_BLK; k < (j+1) * BKT_PER_BLK; k++) {
            if (SHR_BITGET(cdata->bkt, k)) {
                SHR_BITCOUNT_RANGE(cdata->bnk, bnk_cnt,
                    k << _shr_popcount(ALPM_BPB_MAX - 1), ALPM_BPB_MAX);
                total_bnk_cnt += bnk_cnt;
                if (bnk_cnt >= 16) {
                    cli_out("@");
                } else {
                    cli_out("%x", bnk_cnt);
                }
            } else {
                cli_out("%s", "_");
            }
            for (bnk = 0; bnk < BPC_BNK_PER_BKT(bp_conf); bnk++) {
                int bnk_idx =
                    (k << _shr_popcount(ALPM_BPB_MAX - 1) | bnk);
                SHR_BITCOUNT_RANGE(cdata->ent, ent_cnt, bnk_idx << _ENT_BITS, (2 << (_ENT_BITS - 1)));
                total_ent_cnt += ent_cnt;
                max_ent_cnt += BPC_FMT_ENT_MAX(bp_conf, cdata->bnkfmt[bnk_idx]);
            }
        }
        tmp_bnk =
            total_bnk_cnt * 1000 / (BPC_BNK_PER_BKT(bp_conf) * BKT_PER_BLK);
        tmp_ent = (max_ent_cnt > 0)
                    ? total_ent_cnt * 1000 / max_ent_cnt
                    : 0;
        cli_out("] %2d.%d%% %2d.%d%%\n", tmp_bnk / 10, tmp_bnk % 10,
            tmp_ent / 10, tmp_ent % 10);
    }

    alpm_util_free(cdata);

bad:
    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("alpm_cb_bkt_bnk_dump error %d\n", rv));
    }
    return;

}

/*
 * 0-8/0-5 : count of occupied banks)
 * _ : free bucket
 * 30% : average bank percentage per 64 bucket
 * 40% : average entry percentage per 64 bucket
 *
 * BKT0000-0063 [32458___________] 30% 40%
 * BKT0064-0127 [_3133___________] 30% 40%
 */
int
alpm_cb_bkt_dump(int u, int app, int acb_bmp)
{
    int i, ipt;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *gbl_bp = NULL;
    _alpm_bkt_pool_conf_t *prt_bp = NULL;

#define BKT_PER_BLK 64

    ALPMC_INIT_CHECK(u);

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Dumping BUCKET usage:\n", i);

        acb     = ACB(u, i, app);
        gbl_bp  = ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u));
        prt_bp  = ACB_BKT_VRF_POOL(acb, 1);

        for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
            if (ipt > ALPM_IPT_V4 &&
                BPC_BNK_INFO(gbl_bp, ipt) ==
                BPC_BNK_INFO(gbl_bp, ipt - 1)) {
                continue;
            }
            alpm_cb_bkt_pool_dump(u, ipt, acb, gbl_bp,
                                  gbl_bp == prt_bp ? "GLOBAL/PRIVATE" : "GLOBAL");
            alpm_cb_bkt_bnk_dump(u, ipt, acb, gbl_bp,
                                 gbl_bp == prt_bp ? "GLO/PRI" : "GLOBAL");
        }

        if (gbl_bp == prt_bp) {
            continue;
        }

        for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
            if (ipt > ALPM_IPT_V4 &&
                BPC_BNK_INFO(prt_bp, ipt) ==
                BPC_BNK_INFO(prt_bp, ipt - 1)) {
                continue;
            }

            alpm_cb_bkt_pool_dump(u, ipt, acb, prt_bp, "PRIVATE");
            alpm_cb_bkt_bnk_dump(u, ipt, acb, prt_bp, "PRIVATE");
        }
    }

    return BCM_E_NONE;
}

static int
alpm_cb_fmt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int i;
    _alpm_bkt_info_t *bkt_info;
    uint32 *fmt = (uint32 *)datum;
    uint8 bnk_fmt;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    for (i = 0; i < ALPM_BPB_MAX; i++) {
        bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i)) {
            fmt[bnk_fmt] ++;
        }
    }

    return 0;
}

/*
 * Pivot CB:
 *  FMT 1: 10% (1)
 *  FMT 2:  0%
 *  FMT 3: 83%
 *  FMT 4:
 *  FMT 5:
 *  FMT 6:
 *  FMT 7:
 *  FMT 8:
 *
 * Route CB:
 *  FMT 1: 2O%
 *  FMT 2:
 *  ...
 *  FMT 12:
 */
int
alpm_cb_fmt_dump(int u, int app, int acb_bmp)
{
    int i, j, pid, vrf_id, ipt, fmt_sum;
    _alpm_cb_t *acb;
    uint32 fmt[2][16];
    int pid_count;

    ALPMC_INIT_CHECK(u);

    sal_memset(fmt, 0, sizeof(fmt));

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }

        acb = ACB(u, i, app);

        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            _alpm_bkt_pool_conf_t *bp_conf;
            bp_conf = ACB_BKT_POOL(acb, pid);

            if (pid > 0 &&
                ACB_BKT_POOL(acb, pid-1) == ACB_BKT_POOL(acb, pid)) {
                continue;
            }

            cli_out("ALPM CB[%d.%d %s] Dumping FMT distribution:\n", i, pid,
                    BPC_FMT_TYPE(bp_conf) == ALPM_FMT_RTE ? "Route" : "Pivot");
            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt ++) {
                for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                        continue;
                    }
                    (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                            alpm_cb_fmt_dump_cb, fmt[i]);
                }
            }

            fmt_sum = 0;
            for (j = 1; j <= BPC_FMT_CNT(bp_conf); j++) {
                fmt_sum += fmt[i][j];
            }

            /* Avoid Floating point exception */
            if (fmt_sum == 0) {
                fmt_sum = 0xdeadbee;
            }

            if (fmt_sum == 0xdeadbee) {
                cli_out("\tAll ZERO\n\n");
                continue;
            }

            for (j = 1; j <= BPC_FMT_CNT(bp_conf); j++) {
                if (fmt[i][j] != 0) {
                    cli_out("\tFMT %2d(Len %3d): %2d.%d%% (Count %d)\n", j,
                            BPC_FMT_PFX_LEN(bp_conf, j),
                            fmt[i][j] * 100 / fmt_sum,
                            (fmt[i][j] * 1000 / fmt_sum) % 10,
                            fmt[i][j]);
                }
            }
        }
    }

    return BCM_E_NONE;
}

static int
alpm_cb_pvtlen_dump_cb(int u, _alpm_cb_t *acb,
                       _alpm_pvt_node_t *pvt_node, void *datum)
{
    uint32 *pvt_len = (uint32 *)datum;

    if (pvt_node && PVT_KEY_LEN(pvt_node) <= 128) {
        pvt_len[PVT_KEY_LEN(pvt_node)]++;
    }

    return 0;
}

/*
 * Pivot CB:
 *  Pivot Len 0: 10% (1)
 *  ......
 *  Pivot Len 128: 20% (2)
 *
 * Route CB:
 *  ......
 */
int
alpm_cb_pvtlen_dump(int u, int app, int acb_bmp)
{
    int i, j, k, vrf_id, ipt, pvt_len_sum[2];
    _alpm_cb_t *acb;
    uint32 pvt_len[2][129];

    ALPMC_INIT_CHECK(u);

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }

        acb = ACB(u, i, app);
        sal_memset(pvt_len, 0, sizeof(pvt_len));
        for (ipt = 0; ipt < ALPM_IPT_CNT; ipt ++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    continue;
                }
                (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                        alpm_cb_pvtlen_dump_cb,
                        pvt_len[ACB_FMT_TYPE(acb, vrf_id)]);
            }
        }

        for (k = 0; k < 2; k++) {
            pvt_len_sum[k] = 0;
            for (j = 0; j <= 128; j++) {
                pvt_len_sum[k] += pvt_len[k][j];
            }
            /* Avoid Floating point exception */
            if (pvt_len_sum[k] == 0) {
                pvt_len_sum[k] = 0xdeadbee;
            }
            if (pvt_len_sum[k] == 0xdeadbee) {
                continue;
            }

            cli_out("ALPM CB[%d %s] Dumping Pivot Length distribution:\n", i,
                    k == 0 ? "Pivot" : "Route");
            for (j = 0; j <= 128; j++) {
                if (pvt_len[k][j] != 0) {
                    cli_out("\tPVT_LEN %3d: %2d.%d%% (Count %d)\n", j,
                            pvt_len[k][j] * 100 / pvt_len_sum[k],
                            (pvt_len[k][j] * 1000 / pvt_len_sum[k]) % 10,
                            pvt_len[k][j]);
                }
            }
        }
    }
    return BCM_E_NONE;
}

/* BCM.0>alpm stat show <1|2|3> */
int
alpm_cb_stat_dump(int u, int app, int acb_bmp)
{
    int i, j, p, ipt;
    uint32 alloc_cnt, free_cnt, dbg_bmp, dbg_on = 0;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bp_conf = NULL;
    int pid_count;

#define _DBG_BMP_STR_CNT    10
    char *dbg_bmp_str[_DBG_BMP_STR_CNT] = {
        "PvtSanity",
        "PvtSanityLen",
        "PvtDefrag",
        "PvtSanityNode",
        "PvtSanityCount",
        "WriteCacheOnly",
        "BnkConf",
        "PvtUpdateSkip",
        "HitBit",
        ""
    };

    ALPMC_INIT_CHECK(u);

    dbg_bmp = ALPMC(u)->_alpm_dbg_bmp;

    cli_out("ALPM debug:\n");
    for (i = 0; i < _DBG_BMP_STR_CNT; i++) {
        if (dbg_bmp & (1 << i)) {
            cli_out("\tbit %d %s\n", i, dbg_bmp_str[i]);
            dbg_on = 1;
        }
    }
    if (!dbg_on) {
        cli_out("\toff\n");
    }

    cli_out("ALPM recorded error messages:\n");
    for (i = 0; i < _ALPM_ERR_MSG_CUR_IDX(u); i++) {
        cli_out("%3d: %s", i, _ALPM_ERR_MSG_ENT(u, i));
    }
    cli_out("\n");

    alpm_util_mem_stat_get(&alloc_cnt, &free_cnt);
    cli_out("ALPM Host mem: alloc %8d free %8d\n", alloc_cnt, free_cnt);
    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Statistics:\n\n", i);
        acb = ACB(u, i, app);

        if (ACB_HAS_TCAM(acb)) {
            bcm_esw_alpm_tcam_zone_dump(u, app, "\t");
            cli_out("\n");
        }

#define _BKT_POOL_USAGE_PRINT(acb_idx, pid, bp_conf, ipt)                   \
        cli_out("\tBucket Pool %d(%s) Usage(Level%d): %6d/%6d = %d.%d%%\n", \
            pid, alpm_util_ipt_str[ipt], acb_idx + 2,                       \
            BPC_BNK_USED(bp_conf, ipt),                                     \
            BPC_BNK_CNT(bp_conf),                                           \
            BPC_BNK_CNT(bp_conf) ?                                          \
                (BPC_BNK_USED(bp_conf, ipt) * 100 / BPC_BNK_CNT(bp_conf)) : 0, \
            BPC_BNK_CNT(bp_conf) ?                                          \
                ((BPC_BNK_USED(bp_conf, ipt) * 1000 / BPC_BNK_CNT(bp_conf)) % 10) : 0);
        pid_count = alpm_pid_count(app);
        for (p = 0; p < pid_count; p++) {
            bp_conf = ACB_BKT_POOL(acb, p);
            if (p > 0 && bp_conf == ACB_BKT_POOL(acb, p-1)) {
                continue;
            }
            for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
                if (ipt > ALPM_IPT_V4 &&
                    BPC_BNK_INFO(bp_conf, ipt) ==
                    BPC_BNK_INFO(bp_conf, ipt - 1)) {
                    continue;
                }
                _BKT_POOL_USAGE_PRINT(acb->acb_idx, p, bp_conf, ipt)
            }
        }

        cli_out("\tMemory Stats: \n");
        for (j = 0; j < alpmTblCount; j++) {
            if (acb->acb_cnt.c_mem[j][0] != 0 || acb->acb_cnt.c_mem[j][1] != 0) {
                cli_out("\t  %22s: RD %10d WR %10d\n", alpm_tbl_str[j],
                        acb->acb_cnt.c_mem[j][0],
                        acb->acb_cnt.c_mem[j][1]);
            }
        }
        cli_out("\tDefrag : %8d\n", acb->acb_cnt.c_defrag);
        cli_out("\tMerge  : %8d\n", acb->acb_cnt.c_merge);
        cli_out("\tExpand : %8d\n", acb->acb_cnt.c_expand);
        cli_out("\tSplit  : %8d\n", acb->acb_cnt.c_split);
        cli_out("\tRipple : %8d\n", acb->acb_cnt.c_ripple);
        cli_out("\tBNKshrk: %8d\n", acb->acb_cnt.c_bnkshrk);
        cli_out("\tBNKfree: %8d\n", acb->acb_cnt.c_bnkfree);
        cli_out("\tSMCS1  : %8d\n", acb->acb_cnt.c_spl_mp_cs1);
        cli_out("\tSMCS21 : %8d\n", acb->acb_cnt.c_spl_mp_cs21);
        cli_out("\tSMCS221: %8d\n", acb->acb_cnt.c_spl_mp_cs221);
        cli_out("\tSMCS222: %8d\n", acb->acb_cnt.c_spl_mp_cs222);
        cli_out("\tFULL:    %8d\n", acb->acb_cnt.c_full);
        if (ALPMC(u)->_alpm_defrag_on_full) {
            cli_out("\tDefragOnFull:    %d\n", acb->acb_cnt.c_defragonfull);
            cli_out("\tFullAfterDefrag: %d\n", acb->acb_cnt.c_fullafterdefrag);
        }
        cli_out("\n");
    }
    return BCM_E_NONE;
}

/* BCM.0>alpm config show <1|2|3> */
int
alpm_cb_conf_dump(int u, int app, int acb_bmp)
{
    int i, pid, ipt, fmt;
    _alpm_cb_t *acb;
    int pid_count;

    ALPMC_INIT_CHECK(u);

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Bank Config:\n", i);
        acb = ACB(u, i, app);

        cli_out("\t    bkt_bits: %d\n", ACB_BKT_BITS(acb));
        cli_out("\t     bkt_cnt: %d\n", ACB_BKT_CNT(acb));

        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            _alpm_bkt_pool_conf_t *bp_conf;

            if (pid > 0 &&
                ACB_BKT_POOL(acb, pid-1) == ACB_BKT_POOL(acb, pid)) {
                continue;
            }

            bp_conf = ACB_BKT_POOL(acb, pid);
            for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
                if (ipt > ALPM_IPT_V4 &&
                    BPC_BNK_INFO(bp_conf, ipt) ==
                    BPC_BNK_INFO(bp_conf, ipt - 1)) {
                    continue;
                }
                cli_out("\t%d.bnk_pool.%d: %p\n", ipt, pid, BPC_BNK_BMP(bp_conf, ipt));
                cli_out("\t%d.bnk_used.%d: %d\n", ipt, pid, BPC_BNK_USED(bp_conf, ipt));
                cli_out("\t%d.bnk_rsvd.%d: %d\n", ipt, pid, BPC_BNK_RSVD(bp_conf, ipt));
            }

            cli_out("\t bnk_total.%d: %d\n", pid, BPC_BNK_CNT(bp_conf));
            cli_out("\t  bnk_pbkt.%d: %d\n", pid, BPC_BNK_PER_BKT(bp_conf));
            cli_out("\t wrap_arnd.%d: %d\n", pid, BPC_BKT_WRA(bp_conf));
            cli_out("\t fixed_fmt.%d: %d\n", pid, BPC_BKT_FIXED_FMT(bp_conf));
            cli_out("\t   fmt_cnt.%d: %d\n", pid, BPC_FMT_CNT(bp_conf));

            cli_out("\t----------------------------------------------------\n");
            cli_out("\tFV |");
            for (fmt = 0; fmt <= BPC_FMT_CNT(bp_conf); fmt++) {
                cli_out("%3d|", fmt);
            }
            cli_out("\n");
            cli_out("\t----------------------------------------------------\n");
            cli_out("\tCAP|");
            for (fmt = 0; fmt <= BPC_FMT_CNT(bp_conf); fmt++) {
                cli_out("%3d|", BPC_FMT_ENT_MAX(bp_conf, fmt));
            }
            cli_out("\n");
            cli_out("\t----------------------------------------------------\n");
            cli_out("\tPfL|");
            for (fmt = 0; fmt <= BPC_FMT_CNT(bp_conf); fmt++) {
                cli_out("%3d|", BPC_FMT_PFX_LEN(bp_conf, fmt));
            }
            cli_out("\n");
            cli_out("\t----------------------------------------------------\n");

        }
    }
    return BCM_E_NONE;
}

/*
 * Get ALPM resource usage per group
 */
int
bcm_esw_alpm_resource_get(int u, bcm_l3_route_group_t grp, bcm_l3_alpm_resource_t *res)
{
    int app = 0;
    int mc = 0;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bp_conf = NULL;
    int vt, ipt, vrf, vrf_id, i;
    int rv = BCM_E_NONE;

    ALPMC_INIT_CHECK(u);

    if (res == NULL) {
        return BCM_E_PARAM;
    }

    switch (grp) {
    case bcmL3RouteGroupPrivateV4:
        vt = VT_PRIVATE;
        ipt = IPV4;
        break;
    case bcmL3RouteGroupGlobalV4:
        vt = VT_GLOBAL;
        ipt = IPV4;
        break;
    case bcmL3RouteGroupOverrideV4:
        vt = VT_OVERRIDE;
        ipt = IPV4;
        break;
    case bcmL3RouteGroupPrivateV6:
        vt = VT_PRIVATE;
        ipt = IPV6;
        break;
    case bcmL3RouteGroupGlobalV6:
        vt = VT_GLOBAL;
        ipt = IPV6;
        break;
    case bcmL3RouteGroupOverrideV6:
        vt = VT_OVERRIDE;
        ipt = IPV6;
        break;
    default:
        return BCM_E_PARAM;
    }

    vrf = (vt == VT_GLOBAL ? BCM_L3_VRF_GLOBAL :
           vt == VT_OVERRIDE ? BCM_L3_VRF_OVERRIDE : 1);

    vrf_id = ALPM_VRF_TO_VRFID(u, vrf);

    sal_memset(res, 0, sizeof(*res));

    L3_LOCK(u);

    /* acb for GLO */
    acb = ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM) ?
          ACB_TOP(u, app) : ACB_VRF_BTM(u, vrf_id, app);

    if (vt == VT_GLOBAL) {
        res->route_cnt = ACB_PVT_CTRL(acb, ALPM_VRF_ID_GLO(u), ipt).route_cnt;
    } else if (vt == VT_OVERRIDE) {
        res->route_cnt = ACB_PVT_CTRL(ACB_TOP(u, app), ALPM_VRF_ID_GHI(u), ipt).route_cnt;
    } else { /* private cnt = defip_cnt - glo - ghi */
        res->route_cnt =
            (ipt == 0 ? BCM_XGS3_L3_DEFIP_IP4_CNT(u) : BCM_XGS3_L3_DEFIP_IP6_CNT(u)) -
            ACB_PVT_CTRL(acb, ALPM_VRF_ID_GLO(u), ipt).route_cnt -
            ACB_PVT_CTRL(ACB_TOP(u, app), ALPM_VRF_ID_GHI(u), ipt).route_cnt;
    }

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);

        if (ACB_HAS_TCAM(acb)) {
            rv = tcam_resource_info_get(u, app, vrf, ipt, mc, &(res->lvl_usage[0]));

            if ((ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM) && vt == VT_GLOBAL) ||
                vt == VT_OVERRIDE) {
                break; /* Direct routes end here */
            }
        }

        bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
        res->lvl_usage[i + 1].cnt_used = BPC_BNK_USED(bp_conf, ipt);
        res->lvl_usage[i + 1].cnt_total = BPC_BNK_CNT(bp_conf) -
                                          BPC_BNK_RSVD(bp_conf, ipt);
    }

    L3_UNLOCK(u);

    return rv;
}

int
alpm_resource_info_get(int u, _alpm_resource_info_t *info)
{
    int grp;
    int rv = BCM_E_NONE;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        rv = bcm_esw_alpm_resource_get(u, grp, &(info->res[grp]));
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/* Get applicable bank usage threshold configured as limit for: grp & max_lvl */
int
alpm_bnk_limit_usage(int u, bcm_l3_route_group_t grp, int max_lvl)
{
    int app = 0, limit_usage = 100;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bp_conf = NULL;
    int vt, vrf, vrf_id;

    if (max_lvl <= 1 || max_lvl > ACB_CNT(u)) { /* Level1 & Level2 use always 100% */
        return limit_usage;
    }

    switch (grp) {
    case bcmL3RouteGroupPrivateV4:
    case bcmL3RouteGroupPrivateV6:
        vt = VT_PRIVATE;
        break;
    case bcmL3RouteGroupGlobalV4:
    case bcmL3RouteGroupGlobalV6:
        vt = VT_GLOBAL;
        break;
    default:
        return limit_usage;
    }

    vrf = (vt == VT_GLOBAL ? BCM_L3_VRF_GLOBAL : 1);
    vrf_id = ALPM_VRF_TO_VRFID(u, vrf);

    acb = ACB(u, max_lvl - 1, app);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    if (!ACB_BKT_FIXED_FMT(acb, 1) &&
        ACB_HAS_RTE(acb, vrf_id) &&
        (BPC_BNK_CNT(bp_conf) > 8192)) {

        limit_usage = ALPMC(u)->_alpm_bnk_threshold; /* config property */
    }

    return limit_usage;
}

/* v4v6_ratio = expected (v4_cnt/v6_cnt) * 100 */
int
alpm_route_projection(int u, int v4v6_ratio)
{
    int rv = BCM_E_NONE;
    int grp, prj_route, max_lvl = 0, max_usage = 0;
    int i, max_route, mix_route;

    _alpm_resource_info_t resource;
    _alpm_resource_info_t *info = &resource;

    rv = alpm_resource_info_get(u, info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Show current ALPM resource usage */
    cli_out("- Current ALPM route count & resource usage\n");
    cli_out("\t\t Routes L1_usage L2_usage L3_usage L1_used:total L2_used:total L3_used:total\n");
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        int tot, usage[BCM_L3_ALPM_LEVELS];
        bcm_l3_alpm_resource_t *p = &(info->res[grp]);

        /* calculate level's usage = cnt_used / cnt_total */
        for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
            tot = p->lvl_usage[i].cnt_total;
            if (tot == 0) {
                tot = 1; /* avoid div by 0 */
            }
            usage[i] = (p->lvl_usage[i].cnt_used * 10000) / tot; /* usage in 10000% */
        }

        cli_out("%s:%7d  %3d.%02d  %3d.%02d  %3d.%02d %6d:%6d  %6d:%6d  %6d:%6d\n",
                alpm_route_grp_name[grp],
                p->route_cnt,
                usage[0]/100, usage[0]%100,
                usage[1]/100, usage[1]%100,
                usage[2]/100, usage[2]%100,
                p->lvl_usage[0].cnt_used, p->lvl_usage[0].cnt_total,
                p->lvl_usage[1].cnt_used, p->lvl_usage[1].cnt_total,
                p->lvl_usage[2].cnt_used, p->lvl_usage[2].cnt_total);
    }

    /* Show route projection based on current route trend */
    cli_out("\n- Route projection based on current route trend\n");
    i = 0;
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {

        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
            if (grp == bcmL3RouteGroupGlobalV4 ||
                grp == bcmL3RouteGroupGlobalV6) {
                continue; /* skip Global grp in Combined mode */
            }
        }

        rv = bcm_esw_alpm_route_grp_projection(u, grp, info, &prj_route,
                                               &max_lvl, &max_usage);
        if (BCM_FAILURE(rv)) {
            continue; /* projection unavailable */
        }

        cli_out("%s: Prj:%7d (max L%d_usage:%3d%%)\n",
                ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) ?
                alpm_route_comb_name[grp] : alpm_route_grp_name[grp],
                prj_route, max_lvl + 1, max_usage);
        i++;
    }
    if (i == 0) {
        cli_out("      <none>\n");
    }

    /* Show route projection based on test DB result interpolation */
    cli_out("\n- Route projection based on test DB result interpolation and configuration\n");
    if (v4v6_ratio) {
        cli_out("  (using mixed V4/V6 ratio: %d%%)\n",v4v6_ratio);
    } else {
        cli_out("  (uni-dimensional projection)\n");
    }
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {

        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
            if (grp == bcmL3RouteGroupGlobalV4 ||
                grp == bcmL3RouteGroupGlobalV6) {
                continue; /* skip Global grp in Combined mode */
            }
        }

        rv = bcm_esw_alpm_route_grp_testdb(u, grp, info, &max_route,
                                           &mix_route, v4v6_ratio,
                                           &max_lvl, &max_usage);

        if (BCM_FAILURE(rv)) {
            continue; /* default unavailable */
        }

        if (v4v6_ratio) {
            cli_out("%s: Max:%7d Mix:%7d (max L%d_usage:%3d%%)\n",
                    ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) ?
                    alpm_route_comb_name[grp] : alpm_route_grp_name[grp],
                    max_route, mix_route, max_lvl + 1, max_usage);
        } else {
            cli_out("%s: Max:%7d (max L%d_usage:%3d%%)\n",
                    ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) ?
                    alpm_route_comb_name[grp] : alpm_route_grp_name[grp],
                    max_route, max_lvl + 1, max_usage);
        }
    }
    cli_out("\n");

    return rv;
}

/* Route projection based on current route trend */
int
bcm_esw_alpm_route_grp_projection(int u, bcm_l3_route_group_t grp,
                                  _alpm_resource_info_t *info,
                                  int *prj_route, int *def_lvl, int *def_usage)
{
    int i, route_cnt, ghi_route_cnt;
    int grp_comb; /* same ipt, opposite combined grp (private vs global) */
    int grp_ghi; /* same ipt, global high grp */
    int grp_ghi_ipt; /* opposite ipt, global high grp */
    int max_lvl, max_usage, max_usage100, lv_usage;
    int multi_ghi_share;
    int limit_usage, delta;
    int rv = BCM_E_NONE;

    bcm_l3_alpm_resource_t *res = &(info->res[grp]);

    ALPMC_INIT_CHECK(u);
    *prj_route = 0;

    switch (grp) {
    case bcmL3RouteGroupPrivateV4:
        grp_comb = bcmL3RouteGroupGlobalV4;
        grp_ghi = bcmL3RouteGroupOverrideV4;
        grp_ghi_ipt = bcmL3RouteGroupOverrideV6;
        break;
    case bcmL3RouteGroupGlobalV4:
        grp_comb = bcmL3RouteGroupPrivateV4;
        grp_ghi = bcmL3RouteGroupOverrideV4;
        grp_ghi_ipt = bcmL3RouteGroupOverrideV6;
        break;
    case bcmL3RouteGroupPrivateV6:
        grp_comb = bcmL3RouteGroupGlobalV6;
        grp_ghi = bcmL3RouteGroupOverrideV6;
        grp_ghi_ipt = bcmL3RouteGroupOverrideV4;
        route_cnt = res->route_cnt; /* grp route_cnt */
        break;
    case bcmL3RouteGroupGlobalV6:
        grp_comb = bcmL3RouteGroupPrivateV6;
        grp_ghi = bcmL3RouteGroupOverrideV6;
        grp_ghi_ipt = bcmL3RouteGroupOverrideV4;
        break;
    case bcmL3RouteGroupOverrideV4:
    case bcmL3RouteGroupOverrideV6:
        max_usage = res->lvl_usage[0].cnt_used * 100L / res->lvl_usage[0].cnt_total;
        if (res->route_cnt > 1 && max_usage >= ALPM_PRJ_MIN_USAGE) {
            /* Direct routes return L1 cnt_total as projection (x2 for V4) */
            *prj_route = (grp == bcmL3RouteGroupOverrideV4) ?
                          res->lvl_usage[0].cnt_total * 2 :
                          res->lvl_usage[0].cnt_total;

            /* in ALPM_64B mode, projection based on actual v4v6_ratio */
            if (!ALPM_128B(u) && info->res[bcmL3RouteGroupOverrideV6].route_cnt) {
                int v4v6_ratio = info->res[bcmL3RouteGroupOverrideV4].route_cnt * 100L /
                    info->res[bcmL3RouteGroupOverrideV6].route_cnt;

                if (grp == bcmL3RouteGroupOverrideV4) {
                    *prj_route = (long) *prj_route * v4v6_ratio / (v4v6_ratio + 200);
                } else {
                    *prj_route = *prj_route * 200L / (v4v6_ratio + 200);
                }
            }
            *def_lvl = 0;
            *def_usage = 100;
        } else {
            rv = BCM_E_UNAVAIL;
        }
        return rv;
    default:
        return BCM_E_PARAM;
    }

    route_cnt = res->route_cnt; /* grp route_cnt */
    /* In Combined mode, route projection is for combined private & global grps */
    if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
        route_cnt += info->res[grp_comb].route_cnt;
    }

    ghi_route_cnt = info->res[grp_ghi].route_cnt; /* gloabl high grp route_cnt */
    /* in ALPM_64b mode, ghi routes count for both IPTs */
    if (!ALPM_128B(u)) {
        ghi_route_cnt += info->res[grp_ghi_ipt].route_cnt;
    }

    /* find maximum resource usage (worst) */
    max_lvl = 0;
    max_usage100 = 0; /* max_usage100 in x100 percent (higher resolution) */
    if (route_cnt > 0) {
        for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
            if (res->lvl_usage[i].cnt_used == 0 || res->lvl_usage[i].cnt_total == 0) {
                break;
            }

            /* Give 1% of preference to L3 usage */
            delta = (i == BCM_L3_ALPM_LEVELS - 1) ? 100 : 0;

            lv_usage = res->lvl_usage[i].cnt_used * 10000L / res->lvl_usage[i].cnt_total;
            if ((lv_usage + delta) > max_usage100) {
                max_lvl = i;
                max_usage100 = lv_usage;
            }
        }
    }

    max_usage = (max_usage100 + 50) / 100; /* max_usage in percent (round) */

    /* Multi-dimensional routes shared with GHI routes when max_lvl=0 */
    multi_ghi_share = (max_lvl == 0) && (ghi_route_cnt > 2);

    /* Use projection formula: prj_route = route_cnt*limit_usage/max_usage */
    if (!multi_ghi_share && max_usage >= ALPM_PRJ_MIN_USAGE) {
        limit_usage = alpm_bnk_limit_usage(u, grp, max_lvl);
        if (limit_usage < max_usage) {
            limit_usage = max_usage; /* actual usage is bigger than configured limit_usage */
            *prj_route = route_cnt;
        } else {
            *prj_route = route_cnt * limit_usage * 100L / max_usage100;
        }
        *def_lvl = max_lvl;
        *def_usage = limit_usage;
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/* Route projection based on test DB result interpolation or configuration */
int
bcm_esw_alpm_route_grp_testdb(int u, bcm_l3_route_group_t grp,
                              _alpm_resource_info_t *info,
                              int *max_route, int *mix_route, int v4v6_ratio,
                              int *def_lvl, int *def_usage)
{
    int ipt;
    int grp_ipt; /* opposite ipt grp */
    int rv = BCM_E_NONE;

    bcm_l3_alpm_resource_t *res = &(info->res[grp]);
    int cur_cnt_total[] = {res->lvl_usage[0].cnt_total,
                           res->lvl_usage[1].cnt_total,
                           res->lvl_usage[2].cnt_total};

    ALPMC_INIT_CHECK(u);
    *max_route = 0;
    *mix_route = 0;

    switch (grp) {
    case bcmL3RouteGroupPrivateV4:
        grp_ipt = bcmL3RouteGroupPrivateV6;
        ipt = IPV4;
        break;
    case bcmL3RouteGroupGlobalV4:
        grp_ipt = bcmL3RouteGroupGlobalV6;
        ipt = IPV4;
        break;
    case bcmL3RouteGroupPrivateV6:
        grp_ipt = bcmL3RouteGroupPrivateV4;
        ipt = IPV6;
        break;
    case bcmL3RouteGroupGlobalV6:
        grp_ipt = bcmL3RouteGroupGlobalV4;
        ipt = IPV6;
        break;
    case bcmL3RouteGroupOverrideV4:
    case bcmL3RouteGroupOverrideV6:
        /* Direct routes return L1 cnt_total as projection (x2 for V4) */
        *max_route = (grp == bcmL3RouteGroupOverrideV4) ?
                      res->lvl_usage[0].cnt_total * 2 :
                      res->lvl_usage[0].cnt_total;
        *mix_route = *max_route;

        if (v4v6_ratio && !ALPM_128B(u)) {
            if (grp == bcmL3RouteGroupOverrideV4) {
                *mix_route = (long) *max_route * v4v6_ratio / (v4v6_ratio + 200);
            } else {
                *mix_route = *max_route * 200L / (v4v6_ratio + 200);
            }
        }
        *def_lvl = 0;
        *def_usage = 100;
        return rv;
    default:
        return BCM_E_PARAM;
    }

    rv = alpm_default_usage_get(u, grp, max_route, def_lvl, def_usage, cur_cnt_total);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    *mix_route = *max_route;
    if (*def_lvl == 0 && ALPM_128B(u)) {
        return rv; /* no V4V6_share in L1 */
    }

    if (v4v6_ratio) {
        bcm_l3_alpm_resource_t *res_ipt = &(info->res[grp_ipt]);
        int max_route_ipt, def_lvl_ipt, def_usage_ipt;
        int cur_cnt_total_ipt[] = {res_ipt->lvl_usage[0].cnt_total,
                                   res_ipt->lvl_usage[1].cnt_total,
                                   res_ipt->lvl_usage[2].cnt_total};
        rv = alpm_default_usage_get(u, grp_ipt, &max_route_ipt, &def_lvl_ipt,
                                    &def_usage_ipt, cur_cnt_total_ipt);
        if (BCM_SUCCESS(rv) && def_lvl_ipt == *def_lvl && max_route_ipt > 0) {
            if (ipt == IPV4) {
                *mix_route = *max_route / (1.0f + *max_route * 100.0f / ((long) max_route_ipt * v4v6_ratio));
            } else {
                *mix_route = *max_route / (1.0f + (long) *max_route * v4v6_ratio / (max_route_ipt * 100.0f));
            }
        }
    }

    return rv;
}

int alpm_table_dump(int u, int app, int vrf_id, soc_mem_t mem, int copyno, int index,
                    int fmt, int ent_idx, int flags)
{
    return ALPM_DRV(u)->alpm_table_dump(u, app, vrf_id, mem, copyno, index, fmt, ent_idx, flags);
}

/* Check bucket prefix minimum requirement and availability
   of a new bucket for entry merge (sort).
   - nbkt_info: (OUT) new bucket to fit all prefix requirement.
   - req_bnks: (OUT) required banks in the new bucket. */
int
alpm_bkt_pfx_merge_check_and_alloc(int u, _alpm_cb_t *acb,
                         _alpm_pvt_node_t *pvt_node,
                         _alpm_bkt_pfx_arr_t *pfx_arr,
                         _alpm_bkt_info_t *nbkt_info,
                         int avl_free_bnks, int *req_bnks)
{
    int rv = BCM_E_NONE;
    int i, fmt;
    uint8 def_fmt;
    int vrf_id = PVT_BKT_VRF(pvt_node);
    int ipt = PVT_BKT_IPT(pvt_node);
    int avl_ent[ALPM_FMT_TP_CNT]; /* available entry cnt per fmt */
    int req_ent[ALPM_FMT_TP_CNT]; /* request entry cnt per fmt */

    sal_memset(avl_ent, 0, sizeof(avl_ent));
    sal_memset(req_ent, 0, sizeof(req_ent));
    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
    *req_bnks = 0;

    /* Get req_ent */
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        int pfx_len = pfx_arr->pfx[i]->key_len - PVT_KEY_LEN(pvt_node);
        for (fmt = def_fmt; fmt <= ACB_FMT_CNT(acb, vrf_id); fmt++) {
            if (ACB_FMT_BNK(acb, vrf_id, fmt) == INVALIDfmt) {
                continue;
            }
            if (pfx_len <= ACB_FMT_PFX_LEN(acb, vrf_id, fmt)) {
                req_ent[fmt]++;
                break;
            }
        }
    }

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

    /* Check bucket availability for req_bnks */
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bnk_alloc(u, acb, vrf_id, ipt, *req_bnks, nbkt_info, NULL);
    }

    return rv;
}

/* Check ALPM resource when initial insertion FULL, and try to free resource by defrag */
int
bcm_esw_alpm_resource_check_and_free(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int l2cnt = 0;
    int l3cnt = 0;
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    _alpm_cb_t *acb = ACB_VRF_BTM(u, vrf_id, app);

    /* propoerty l3_alpm2_defrag_on_full disabled */
    if (ALPMC(u)->_alpm_defrag_on_full == 0) {
        return BCM_E_FULL;
    }

    /* avoid unnecessary defrag if previous defrag ends no merge,
       unless after ALPM_DEFRAG_DELETE_MIN */
    if (ALPMC(u)->_alpm_defrag_status == ALPM_DEFRAG_END_NO_MERGE &&
        bcm_esw_alpm_trace_delete(u) < ALPM_DEFRAG_DELETE_MIN) {
        return BCM_E_FULL;
    } else {
        rv = bcm_esw_alpm_defrag(u, BCM_L3_VRF_ALL, bcmL3AlpmIptAll,
                                 ALPMC(u)->_alpm_defrag_max_time,
                                 ALPMC(u)->_alpm_defrag_max_l2cnt,
                                 ALPMC(u)->_alpm_defrag_max_l3cnt,
                                 &l2cnt, &l3cnt);

        acb->acb_cnt.c_defragonfull++;
    }

    if (BCM_SUCCESS(rv)) {
        if (l2cnt == 0 && l3cnt == 0 ) {
            /*cli_out("#%d Nothing to defrag\n", bcm_esw_alpm_trace_seq(u));*/
            ALPMC(u)->_alpm_defrag_status = ALPM_DEFRAG_END_NO_MERGE;
            bcm_esw_alpm_trace_delete_clear(u);
            rv = BCM_E_FULL;
        } else {
            /*cli_out("#%d Defrag successful (L2 merges:%d, L3 merges:%d)\n",
                    bcm_esw_alpm_trace_seq(u), l2cnt, l3cnt);*/
            ALPMC(u)->_alpm_defrag_status = ALPM_DEFRAG_OK;
        }
    }

    return rv;
}

#else
/* Just to make ISO compilers happy. */
typedef int bcm_esw_alpm2_alpm_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
