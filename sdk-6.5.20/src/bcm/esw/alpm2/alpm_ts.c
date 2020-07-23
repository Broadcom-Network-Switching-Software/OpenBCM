/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    alpm_ts.c
 * Purpose: ALPM trouble-shooting & sanity-check
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

#define ALPM_CB0 0x01
#define ALPM_CB1 0x02
#define ALPM_CB_ALL_BMP (ALPM_CB0 | ALPM_CB1)

typedef struct _alpm_dbgcnt_s {
    uint32 pvt_used[ALPM_PKM_CNT];
    uint32 pvt_max[ALPM_PKM_CNT];

    uint32 bucket_used;
    uint32 bucket_max;
    uint32 bank_used;
    uint32 bank_max;
    uint32 ent_used[ALPM_PKM_CNT]; /* bkt entries */
    uint32 ent_max;
    uint32 bs[ALPM_BPB_MAX]; /* bucket share = #pvt share same bucket */
} _alpm_dbgcnt_t;

typedef struct _alpm_dbg_data_s {
    uint32 route_cnt;
    uint32 hit_cnt;
    _alpm_dbgcnt_t dc[ALPM_CB_CNT];
    int *bktref[ALPM_CB_CNT];
} _alpm_dbg_data_t;

typedef struct _alpm_sanity_s {
    int error;
    int *bktptr[ALPM_CB_CNT];
} _alpm_sanity_t;

typedef struct _alpm_pvt_bkt_cnt_s {
    uint32 pvt_cnt;
    uint32 bkt_cnt;
    uint32 add_cnt;
    uint32 del_cnt;
    uint32 route_cnt;
    uint8  db_type;
} _alpm_pvt_bkt_cnt_t;

typedef struct _alpm_trv_data_s {
    int unit;
    int vrf_id;
    int ipt;
    int err;
    _alpm_cb_t *acb;
} _alpm_trv_data_t;

typedef enum _alpm_lkup_res_e {
    ALPM_HIT = 1,           /* Hit */
    ALPM_MISS = 2,          /* Miss (Default Miss = 1) */
    ALPM_MISS_USE_AD = 3    /* Miss (Default Miss = 0) and use Assoc Data */
} _alpm_lkup_res_t;

#define VRF_NAME(u, vrf_id) ((vrf_id) == ALPM_VRF_ID_GHI(u) ? "GHi" :    \
                             (vrf_id) == ALPM_VRF_ID_GLO(u) ? "GLo" : "")

#define ALPM_PVT_BKT_CNT_ADD(dst_dc, src_dc)   \
    do {                                        \
        (dst_dc)->pvt_cnt += (src_dc)->pvt_cnt; \
        (dst_dc)->bkt_cnt += (src_dc)->bkt_cnt; \
        (dst_dc)->add_cnt += (src_dc)->add_cnt; \
        (dst_dc)->del_cnt += (src_dc)->del_cnt; \
        (dst_dc)->route_cnt += (src_dc)->route_cnt; \
    } while (0)

#define PRINT_SPLIT_LINE(u)    \
    LOG_CLI((BSL_META_U(u, "--------------------------------------------------------\n")))

#define PRINT_VRF_NOT_FOUND(u) \
    LOG_CLI((BSL_META_U(u, "%9s\n"), "Specific VRF not found"))

#define PRINT_ACB_HEADER(u, i) \
    LOG_CLI((BSL_META_U(u, "\n%s\n"), alpm_util_acb_str[(i)]))

#define PRINT_PVT_BKT_HEADER(u) \
   do { \
        LOG_CLI((BSL_META_U(u, \
                 "\nVRF  PKM     Pivot   Bucket  Add     Delete  |   Comment\n"))); \
    } while (0)

#define PRINT_PVT_BKT_ITEM(u, vrf, ipt, dc) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                 "%-4d %-7s %-7d %-7d %-7d %-7d |   %-7s\n"), (vrf), \
                 alpm_util_ipt_str[(ipt)], (dc)->pvt_cnt, (dc)->bkt_cnt, \
                 (dc)->add_cnt, (dc)->del_cnt, VRF_NAME((u), (vrf)) )); \
    } while (0)

#define PRINT_PVT_BKT_TOT(u, ipt, all) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                 "Tot  %-7s %-7d %-7d %-7d %-7d |\n"), alpm_util_ipt_str[(ipt)], \
                 (all)->pvt_cnt, (all)->bkt_cnt, (all)->add_cnt, (all)->del_cnt)); \
    } while (0)

/* according to ALPMC(u)->_alpm_dbg_info_bmp defines */
char *alpm_dbg_info_str[_ALPM_DBG_INFO_MAX_NUM] = {
    "pvt",
    "rte",
    "cnt",
    "hit",
};

int
alpm_dbg_info_bmp_show(int u)
{
    int i;

    ALPMC_INIT_CHECK(u);

    if (ALPMC(u)->_alpm_dbg_info_bmp != 0) {
        cli_out("alpm_info_bmp is on:");

        if (ALPMC(u)->_alpm_dbg_info_bmp == _ALPM_DBG_INFO_ALL) {
            cli_out(" all -");
        }
        for (i = 0; i < _ALPM_DBG_INFO_MAX_NUM; i++) {
            if (ALPMC(u)->_alpm_dbg_info_bmp & (1 << i)) {
                cli_out(" %s", alpm_dbg_info_str[i]);
            }
        }
        cli_out(".\n");
    } else {
        cli_out("alpm_info_bmp is off.\n");
    }

    return BCM_E_NONE;

}

int
alpm_dbg_info_bmp_set(int u, uint32 flags)
{
    ALPMC_INIT_CHECK(u);

    ALPMC(u)->_alpm_dbg_info_bmp = flags;
    (void)alpm_dbg_info_bmp_show(u);

    return BCM_E_NONE;
}

int
alpm_dbg_bmp_set(int u, uint32 dbg_bmp)
{
    ALPMC_INIT_CHECK(u);

    ALPMC(u)->_alpm_dbg_bmp = dbg_bmp;

    if (ALPMC(u)->_alpm_dbg_bmp != 0) {
        int i;
        cli_out("_alpm_dbg_bmp enabled bit(s) [");
        for (i = 0; i < 32; i++) {
            if (ALPMC(u)->_alpm_dbg_bmp & (1 << i)) {
                cli_out("%d ", i);
            }
        }
        cli_out("]\n");
    } else {
        cli_out("_alpm_dbg_bmp is disabled.\n");
    }

    return BCM_E_NONE;
}

int
alpm_find_best_match(int u, int vrf_id, int pkm, uint32 *sw_key,
                     int sw_len, _bcm_defip_cfg_t *lpm_cfg);

STATIC int
alpm_pvt_bkt_cnt_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *user_data)
{
    _alpm_pvt_bkt_cnt_t *data = (_alpm_pvt_bkt_cnt_t *)user_data;
    if (pvt_node->bkt_trie && pvt_node->bkt_trie->trie) {
        data->bkt_cnt += pvt_node->bkt_trie->trie->count; /* add bkt cnt */
    }
    return BCM_E_NONE;
}

STATIC void
alpm_debug_count_dump(int u, int app, int cb_bmp, int vrf_id, uint32 flags)
{
    int found = FALSE;
    _alpm_pvt_bkt_cnt_t alpm_dc[ALPM_IPT_CNT];
    _alpm_pvt_bkt_cnt_t all_dc[ALPM_IPT_CNT];
    int                 i, vrf, ipt;
    _alpm_cb_t          *acb;
    alpm_lib_trie_t     *pvt_trie;

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(cb_bmp & (1 << i))) {
            continue;
        }
        PRINT_ACB_HEADER(u, i);
        PRINT_PVT_BKT_HEADER(u);
        PRINT_SPLIT_LINE(u);

        found = FALSE;
        acb = ACB(u, i, app);
        sal_memset(&all_dc, 0, sizeof(all_dc));

        for (vrf = 0; vrf <= ALPM_VRF_ID_MAX(u); vrf++) {
            if (vrf_id != -1 && vrf_id != vrf) {
                continue;
            }
            sal_memset(&alpm_dc, 0, sizeof(alpm_dc));

            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                if (!ACB_VRF_INITED(u, acb, vrf, ipt)) {
                    continue;
                }
                pvt_trie = ACB_PVT_TRIE(acb, vrf, ipt);

                if ((pvt_trie == NULL) || (pvt_trie->trie == NULL)) {
                    continue;
                }
                alpm_dc[ipt].pvt_cnt = pvt_trie->trie->count;
                alpm_dc[ipt].add_cnt = ACB_PVT_CTRL(acb, vrf, ipt).cnt_add;
                alpm_dc[ipt].del_cnt = ACB_PVT_CTRL(acb, vrf, ipt).cnt_del;
                alpm_dc[ipt].route_cnt = ACB_PVT_CTRL(acb, vrf, ipt).route_cnt;
                alpm_dc[ipt].db_type = ACB_PVT_CTRL(acb, vrf, ipt).db_type;

                (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf, ipt,
                           alpm_pvt_bkt_cnt_cb, (void *)&alpm_dc[ipt]);
                found = TRUE;

                if (flags & BCM_ALPM_DEBUG_SHOW_FLAG_BKT) {
                    PRINT_PVT_BKT_ITEM(u, vrf, ipt, &alpm_dc[ipt]);
                }
                ALPM_PVT_BKT_CNT_ADD(&all_dc[ipt], &alpm_dc[ipt]);
            }
        }

        if (found == FALSE) {
            PRINT_VRF_NOT_FOUND(u);
        } else {
            PRINT_SPLIT_LINE(u);
            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                PRINT_PVT_BKT_TOT(u, ipt, &all_dc[ipt]);
            }
        }
    }
}

STATIC int
alpm_pfx_debug_cb(_alpm_pfx_node_t *pfx_node, void *user_data)
{
    _alpm_trv_data_t *trv_data = (_alpm_trv_data_t *)user_data;
    char ip_buf[IP6ADDR_STR_LEN];
    int u = trv_data->unit;
    int ipt = trv_data->ipt;
    /*int vrf_id = trv_data->vrf_id;*/
    _alpm_bkt_node_t *bkt_node = pfx_node->bkt_ptr;
    int idx, ent;
    _bcm_defip_cfg_t lpm_cfg;

    if (bkt_node == NULL) {
        /* VRF global low "virtual def rte" without true bucket */
        /* ALPM_INFO((" **PFX %p w/o BKT: Sublen:%d\n",
                   pfx_node, pfx_node->key_len)); */
        return BCM_E_NONE;
    }

    idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
    ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Key & Length (lpm_cfg.user_data will be used in _alpm_find()) */
    alpm_trie_pfx_to_cfg(u, bkt_node->key, bkt_node->key_len, &lpm_cfg);

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    if (bkt_node->key_len != pfx_node->key_len) {
        ALPM_INFO((" **PFX %p->BKT %p: idx:%d.%d Sublen:%d Dest:%d IP:%s"
                   " Error=(bkt_keylen:%d)\n",
                   pfx_node, bkt_node, idx, ent, pfx_node->key_len,
                   bkt_node->adata.defip_ecmp_index, ip_buf,
                   bkt_node->key_len));
    } else {
        ALPM_INFO((" **PFX %p->BKT %p: idx:%d.%d Sublen:%d Dest:%d IP:%s\n",
                   pfx_node, bkt_node, idx, ent, pfx_node->key_len,
                   bkt_node->adata.defip_ecmp_index, ip_buf));
    }

    return BCM_E_NONE;
}

#if 0
STATIC int
alpm_bkt_debug_cb(_alpm_bkt_node_t *bkt_node, void *user_data)
{
    _alpm_trv_data_t *trv_data = (_alpm_trv_data_t *)user_data;
    char ip_buf[IP6ADDR_STR_LEN];
    int u = trv_data->unit;
    int ipt = trv_data->ipt;
    /*int vrf_id = trv_data->vrf_id;*/
    int idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
    int ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
    _bcm_defip_cfg_t lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Key & Length (lpm_cfg.user_data will be used in _alpm_find()) */
    alpm_trie_pfx_to_cfg(u, bkt_node->key, bkt_node->key_len, &lpm_cfg);

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    ALPM_INFO(("  >BKT %p: idx:%d.%d Sublen:%d Dest:%d IP:%s\n",
               bkt_node, idx, ent, bkt_node->key_len,
               bkt_node->adata.defip_ecmp_index, ip_buf));

    return BCM_E_NONE;
}
#endif

STATIC int
alpm_pvt_node_dbg(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                  _alpm_trv_data_t *trv_data)
{
    char ip_buf[IP6ADDR_STR_LEN];
    int ipt = PVT_BKT_IPT(pvt_node);
    int vrf_id = PVT_BKT_VRF(pvt_node);
    int idx, sub_idx;
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    if ((ipt != trv_data->ipt) ||
        (vrf_id != trv_data->vrf_id)) {
        rv = BCM_E_INTERNAL;
    }

    /* Key & Length (lpm_cfg.user_data will be used in _alpm_find()) */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_node->key_len, &lpm_cfg);

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    ALPM_IER(alpm_util_pvt_idx_get(u, acb, pvt_node, &idx, &sub_idx));

    if (PVT_BKT_TRIE(pvt_node) == NULL) {
        if (BCM_SUCCESS(rv)) {
            ALPM_INFO(("L%d PVT %p->BKT %p: idx:%d.%d Sublen:%d IP:%s w/o bkt\n",
                       ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                       idx, sub_idx, pvt_node->key_len, ip_buf));
        } else {
            ALPM_ERR(("L%d PVT %p->BKT %p: idx:%d.%d Sublen:%d IP:%s w/o bkt"
                      " Error=(VRF:%d IPT:%d)\n",
                      ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                      idx, sub_idx, pvt_node->key_len, ip_buf,
                      vrf_id, ipt));
        }
    } else if (PVT_BKT_DEF(pvt_node) == NULL) {
        if (BCM_SUCCESS(rv)) {
            ALPM_INFO(("\nL%d PVT %p->BKT %p: idx:%d.%d Sublen:%d IP:%s w/o def_rte\n",
                       ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                       idx, sub_idx, pvt_node->key_len, ip_buf));
        } else {
            ALPM_ERR(("\nL%d PVT %p->BKT %p: idx:%d.%d Sublen:%d IP:%s w/o def_rte"
                      " Error=(VRF:%d IPT:%d)\n",
                      ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                      idx, sub_idx, pvt_node->key_len, ip_buf,
                      vrf_id, ipt));
        }
    } else {
        if (BCM_SUCCESS(rv)) {
            ALPM_INFO(("\nL%d PVT %p->BKT %p: idx:%d.%d Sublen:%d bpmDest:%d IP:%s\n",
                       ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                       idx, sub_idx, pvt_node->key_len,
                       PVT_BKT_DEF(pvt_node)->adata.defip_ecmp_index, ip_buf));
        } else {
            ALPM_ERR(("\nL%d PVT %p->BKT %p: idx:%d.%d Sublen:%d bpmDest:%d IP:%s"
                      " Error=(VRF:%d IPT:%d)\n",
                      ACB_IDX(acb)+1, pvt_node, PVT_BKT_DEF(pvt_node),
                      idx, sub_idx, pvt_node->key_len,
                      PVT_BKT_DEF(pvt_node)->adata.defip_ecmp_index, ip_buf,
                      vrf_id, ipt));
        }
    }

    return rv;
}

#if 0
STATIC int
alpm_pvt_bkt_debug_cb(int u, _alpm_cb_t *acb,
                  _alpm_pvt_node_t *pvt_node, void *user_data)
{
    _alpm_trv_data_t *trv_data = (_alpm_trv_data_t *)user_data;

    alpm_pvt_node_dbg(u, acb, pvt_node, trv_data);

    if (PVT_BKT_TRIE(pvt_node)) {
        bcm_esw_alpm_bkt_traverse(pvt_node->bkt_trie,
                                  alpm_bkt_debug_cb, trv_data);
    }
    return BCM_E_NONE;
    }
#endif

STATIC int
alpm_pvt_debug_cb(int u, _alpm_cb_t *acb,
                  _alpm_pvt_node_t *pvt_node, void *user_data)
{
    _alpm_trv_data_t *trv_data = (_alpm_trv_data_t *)user_data;

    alpm_pvt_node_dbg(u, acb, pvt_node, trv_data);

    return BCM_E_NONE;
}

STATIC void
alpm_trie_debug_dump(int u, int app, int cb_bmp, int vrf_id)
{
    int         i, vrf, ipt;
    _alpm_cb_t  *acb;
    _alpm_trv_data_t trv_data;

    trv_data.unit = u;
    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(cb_bmp & (1 << i))) {
            continue;
        }
        acb = ACB(u, i, app);
        trv_data.acb = acb;

        for (vrf = 0; vrf <= ALPM_VRF_ID_MAX(u); vrf++) {
            if (vrf_id != -1 && vrf_id != vrf) {
                continue;
            }
            trv_data.vrf_id = vrf;

            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                if (!ACB_VRF_INITED(u, acb, vrf, ipt)) {
                    continue;
                }
                trv_data.ipt = ipt;
                ALPM_INFO(("\n%s VRF:%d IPT:%s\n",
                           alpm_util_acb_str[i], vrf, alpm_util_ipt_str[ipt]));
                (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf, ipt,
                                                alpm_pvt_debug_cb, &trv_data);

                if (ACB_HAS_RTE(acb, vrf)) {
                    ALPM_INFO(("\n"));
                    (void)bcm_esw_alpm_pfx_traverse(u, app, vrf, ipt,
                                                    alpm_pfx_debug_cb, &trv_data);
                }
            }
        }
    }
}

void
alpm_ts_trie_tree_dump(int u, int app, int acb_bmp, int vrf_id)
{
    int i, vrf, ipt;
    _alpm_cb_t  *acb;

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        acb = ACB(u, i, app);

        for (vrf = 0; vrf <= ALPM_VRF_ID_MAX(u); vrf++) {
            if (vrf_id != -1 && vrf_id != vrf) {
                continue;
            }

            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                alpm_lib_trie_t *ptree;
                if (!ACB_VRF_INITED(u, acb, vrf, ipt)) {
                    continue;
                }

                /* PVT trie tree */
                cli_out("----------Printing %d PVT_TRIE vrf%4d, ipt %d----------\n",
                        ACB_IDX(acb), vrf, ipt);
                ptree = ACB_PVT_TRIE(acb, vrf, ipt);
                if (ptree != NULL) {
                    if (ACB_IDX(acb) == 0) {
                        alpm_lib_trie_dump(ptree, alpm_util_trie_pvt_node_print, NULL);
                    } else {
                        alpm_lib_trie_dump(ptree, NULL, NULL);
                    }
                }

                /* Pfx trie tree */
                cli_out("----------Printing %d PFX_TRIE vrf%4d, ipt %d----------\n",
                        ACB_IDX(acb), vrf, ipt);
                ptree = ALPM_VRF_TRIE(u, app, vrf, ipt);
                if (ptree != NULL) {
                    alpm_lib_trie_dump(ptree, alpm_util_trie_bkt_node_print, &ipt);
                }
            } /* ipt */
        } /* vrf */
    } /* acb */
}

void
alpm_ts_trie_tree_dump1(int u, int app, bcm_vrf_t vrf,
                        bcm_l3_alpm_ipt_t ipt_in, int trie_type, int acb_bmp)
{
    int i, vrf_id, ipt;
    _alpm_cb_t  *acb;
    alpm_lib_trie_t *ptree;

    if (vrf == BCM_L3_VRF_GLOBAL) {
        vrf = ALPM_VRF_ID_GLO(u);
    } else if (vrf == BCM_L3_VRF_OVERRIDE) {
        vrf = ALPM_VRF_ID_GHI(u);
    }

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        acb = ACB(u, i, app);

        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            if (vrf != BCM_L3_VRF_ALL && vrf_id != vrf) {
                continue;
            }

            for (ipt = 0; ipt < bcmL3AlpmIptCount; ipt++) {
                if (ipt_in != bcmL3AlpmIptAll && ipt != ipt_in) {
                    continue;
                }

                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    continue;
                }

                ptree = ACB_PVT_TRIE(acb, vrf_id, ipt);
                if (ptree != NULL) {
                    _alpm_cb_merge_ctrl_t merge_ctrl;

                    sal_memset(&merge_ctrl, 0, sizeof(merge_ctrl));
                    merge_ctrl.unit     = u;
                    merge_ctrl.ipt      = ipt;
                    merge_ctrl.vrf_id   = vrf_id;
                    merge_ctrl.acb      = acb;

                    if (trie_type == 0) {
                        /* PVT trie tree */
                        cli_out("========= L%d PVT_TRIE vrf%4d, ipt %d ==========\n",
                                ACB_IDX(acb) + 1, vrf_id, ipt);
                        alpm_lib_trie_dump1(ptree, alpm_util_trie_pvt_node_print1, &merge_ctrl);
                    } else {
                        cli_out("-------- L%d PVT_BKT_TRIE vrf%4d, ipt %d -------\n",
                                ACB_IDX(acb) + 1, vrf_id, ipt);
                        alpm_lib_trie_dump1(ptree, alpm_util_trie_pvt_bkt_print1, &merge_ctrl);
                    }
                }
            } /* ipt */
        } /* vrf_id */
    } /* i */
}

/* BCM.0>alpm <pvttrie | bkttrie> show <1|2|3> [vrf: -4 for all] [ipt: 3 for all]
   - trie_type: 0 for pvttrie, 1 for pvt_bkt trie */
int
alpm_trie_show(int u, int app, bcm_vrf_t vrf,
               bcm_l3_alpm_ipt_t ipt, int trie_type, int acb_bmp)
{
    ALPMC_INIT_CHECK(u);

    L3_LOCK(u);
    alpm_ts_trie_tree_dump1(u, app, vrf, ipt, trie_type, acb_bmp);
    L3_UNLOCK(u);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_esw_alpm_debug_show
 * Purpose:
 *      Show ALPM Sw counters for all levels per VRF.
 *      Detail debug info will be dumped if "debug bcm alpm".
 * Parameters:
 *      u        - (In)Device unit
 *      vrf_id   - (In)Show for a specific VRF ID, -1 for all VRFs.
 *      flags    - (In)Show flags or all.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_debug_show(int u, int app, int vrf_id, uint32 flags)
{

    ALPMC_INIT_CHECK(u);

    if ((vrf_id > ALPM_VRF_ID_MAX(u)) || !flags) {
        return BCM_E_PARAM;
    }

    if (flags & (BCM_ALPM_DEBUG_SHOW_FLAG_CNT | BCM_ALPM_DEBUG_SHOW_FLAG_BKT)) {
        alpm_debug_count_dump(u, app, ALPM_CB_ALL_BMP, vrf_id, flags);
    }

    if (flags & (BCM_ALPM_DEBUG_SHOW_FLAG_INTDBG)) {
        alpm_trie_debug_dump(u, app, ALPM_CB_ALL_BMP, vrf_id);
    }

    if (flags & (BCM_ALPM_DEBUG_SHOW_TRIE_TREE)) {
        alpm_ts_trie_tree_dump(u, app, ALPM_CB_ALL_BMP, vrf_id);
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      alpm_cb_dbg_process
 * Purpose:
 *      ALPM control block (level) debug process.
 *      It's recursive from top to bottom.
 *      Get ALPM data and info from Hw mem at this level.
 * Parameters:
 *      u        - (In)Device unit
 *      vrf_id   - (In)VRF ID
 *      pkm      - (In)PKM
 *      acb      - (In)ALPM control block
 *      fent     - (In)ALPM_DATA entry
 *      pvtfmt   - (In)Pivot bank format (ignored for TCAM)
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      idx      - (In)Memory Index
 *      sub_idx  - (In)Memory sub_idx (for half-entry) or ent index
 *      adata    - (In)Assoc Data structure pointer
 *      dp       - (In/Out)Carrying debug counters
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_cb_dbg_process(int u, int vrf_id, int pkm, _alpm_cb_t *acb, void *fent,
                    int pvtfmt, uint32 *key, int key_len, int idx, int sub_idx,
                    _alpm_bkt_adata_t *adata, _alpm_dbg_data_t *dp)
{
    uint32 bkt_entry[ALPM_MEM_ENT_MAX];
    uint32 fent1[ALPM_MEM_ENT_MAX];
    uint32 ftmp[ALPM_MEM_ENT_MAX];
    uint32 new_key[4];
    uint32 fmt, valid;
    int sublen, new_len, def_miss;
    int idx1, eid, bnk;
    _alpm_bkt_adata_t adata1;
    _alpm_bkt_info_t bkt_info;
    char ip_buf[IP6ADDR_STR_LEN];
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_tbl_t bkt_tbl;
    _alpm_tbl_t pvt_tbl;
    int ipt;

    if (ACB_IDX(acb) >= ACB_CNT(u)) {
        return BCM_E_INTERNAL; /* wrong ALPM control level called */
    }

    ipt = ALPM_PKM2IPT(pkm);
    bkt_tbl = ACB_BKT_TBL(acb, vrf_id);
    pvt_tbl = (ACB_HAS_TCAM(acb) ? acb->pvt_tbl[pkm] :
                ACB_BKT_TBL(ACB_UPR(u, acb), vrf_id));

    /**** Pivot level ****/
    alpm_util_bkt_info_get(u, vrf_id, ipt, pkm, acb, fent, sub_idx,
                           &bkt_info, &sublen, &def_miss);

    /* kshift must be same as previous key_len */
    if (!ACB_BKT_FIXED_FMT(acb, 1) && sublen != key_len) {
        ALPM_ERR(("%s: ALPM_DATA invalid kshift:%d vs pivot len:%d"
                  " idx:%d.%d (%d)\n", ALPM_TBL_NAME(pvt_tbl), sublen,
                  key_len, idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }
    if (BI_BKT_IDX(&bkt_info) >= ACB_BKT_CNT(acb)) {
        ALPM_ERR(("%s: ALPM_DATA invalid bkt_ptr idx:%d.%d (%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }
    dp->dc[ACB_IDX(acb)].pvt_used[pkm]++;
    dp->bktref[ACB_IDX(acb)][BI_BKT_IDX(&bkt_info)]++;
    if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_PVT) {
        alpm_util_cfg_construct(u, vrf_id, ipt, key, key_len, adata, &lpm_cfg);
        if (ALPM_IS_IPV4(ipt)) {
            alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
        } else {
            alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
        }
        if (ACB_HAS_TCAM(acb)) {
            ALPM_INFO(("VRF:%d PKM:%s %s: idx:%d.%d Sublen:%d bpmDest:%d IP:%s\n",
                       vrf_id, alpm_util_pkm_str[pkm], ALPM_TBL_NAME(pvt_tbl),
                       idx, sub_idx, key_len,
                       adata->defip_ecmp_index, ip_buf));
        } else {
            ALPM_INFO(("VRF:%d PKM:%s %s: idx:%d.%d Sublen:%d fmt:%d bpmDest:%d IP:%s\n",
                       vrf_id, alpm_util_pkm_str[pkm], ALPM_TBL_NAME(pvt_tbl),
                       idx, sub_idx, key_len, pvtfmt,
                       adata->defip_ecmp_index, ip_buf));
        }
    }

    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        fmt = bkt_info.bnk_fmt[bnk];
        if ((fmt == 0) || (fmt > ACB_FMT_CNT(acb, vrf_id))) {
            if (fmt > ACB_FMT_CNT(acb, vrf_id)) {
                ALPM_ERR(("%s: ALPM_DATA format wrong idx:%d.%d (bank=%d fmt=%d).\n",
                          ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, bnk, fmt));
            }
            continue;
        }
        dp->dc[ACB_IDX(acb)].bank_used++;

        /**** Bucket Level ****/
        idx1 = ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(&bkt_info), BI_BKT_IDX(&bkt_info), bnk);
        ALPM_IER(alpm_bkt_entry_read(u, ACB_BKT_TBL(acb, vrf_id),
                                     acb, bkt_entry, idx1));

        for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fmt); eid++) {
            uint32 key2[4] = {0};
            int len2 = 0;
            int sub_bkt_ptr = 0;

            /* process bucket entry Prefix */
            alpm_util_ent_ent_get(u, vrf_id, acb, bkt_entry, fmt, eid, ftmp);
            alpm_util_bkt_pfx_get(u, vrf_id, acb, ftmp, fmt, key2, &len2, &valid);
            alpm_util_bkt_adata_get(u, vrf_id, ipt, acb, ftmp, fmt, &adata1, &sub_bkt_ptr);

            if (!valid || BI_SUB_BKT_IDX(&bkt_info) != sub_bkt_ptr) {
                continue;
            } else {
                if (ACB_BKT_FIXED_FMT(acb, 1)) {
                    new_len = len2;
                    sal_memcpy(new_key, key2, sizeof(new_key));
                } else {
                    alpm_util_pfx_cat(u, ipt, key, sublen, key2, len2, new_key, &new_len);
                }
            }

            /* process bucket entry ASSOC_DATA */
            dp->dc[ACB_IDX(acb)].ent_used[pkm]++;

            /* end recursion here if reach to route level (last level) */
            if (ACB_HAS_RTE(acb, vrf_id)) {
                dp->route_cnt++;
                if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_RTE) {
                    alpm_util_cfg_construct(u, vrf_id, ipt, new_key, new_len,
                                            &adata1, &lpm_cfg);
                    if (ALPM_IS_IPV4(ipt)) {
                        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
                    } else {
                        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
                    }

                    if (!ALPM_HIT_SKIP(u)) {
                        int hit_val;
                        ALPM_HIT_LOCK(u);
                        hit_val = alpm_rte_hit_get(u, acb->app, vrf_id, bkt_tbl, idx1, eid);
                        ALPM_HIT_UNLOCK(u);

                        if (hit_val > 0) {
                            dp->hit_cnt++;
                            ALPM_INFO(("! "));
                        }
                    }
                    ALPM_INFO(("VRF:%d PKM:%s %s: Route idx:%d.%d Sublen:%d"
                               " fmt:%d Dest:%d IP:%s\n",
                               vrf_id, alpm_util_pkm_str[pkm],
                               ALPM_TBL_NAME(bkt_tbl),
                               idx1, eid,
                               new_len, fmt, adata1.defip_ecmp_index, ip_buf));
                }
                continue;
            }

            /* prevent going further down if somehow last level is not
               Route level due to internal failure */
            if ((ACB_IDX(acb) + 1) >= ACB_CNT(u)) {
                return BCM_E_INTERNAL;
            }

            /* get ALPM Data entry ready for next level */
            alpm_util_ent_data_get(u, vrf_id, ipt, acb, ftmp, fmt, fent1);
            ALPM_IER(alpm_cb_dbg_process(u, vrf_id, ipt, ACB_DWN(u, acb),
                        fent1, fmt, new_key, new_len, idx1, eid, &adata1, dp));
        } /* for ent */
    } /* for bnk */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_alpm_debug_brief_show
 * Purpose:
 *      Show ALPM Hw mem debug information and counters.
 *      Process all ALPM levels from top to bottom.
 *      Detail debug info will be dumped if "debug bcm alpm".
 * Parameters:
 *      u        - (In)Device unit
 *      phy_bkt  - (In)Physical bucket (not in used)
 *      index    - (In)Memory index (not in used)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_debug_brief_show(int u, int app, int phy_bkt, int index)
{
    int ipv6, key_mode;
    int idx, sub_idx;
    int vrf_id;
    int idx_end;
    int step_size;
    int rv = BCM_E_NONE;
    int rv_tcam;
    uint32 lpm_entry[ALPM_MEM_ENT_MAX];
    uint32 fent[ALPM_MEM_ENT_MAX];
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_cb_t *acb;
    _alpm_bkt_adata_t adata0;
    int pk, pkm, use_alpm;
    uint32 key[4];
    int ipt, key_pfx_len;
    int i, j, bit128 = 0, pid, alloc_sz, k, ent_used_tot;
    uint32 glb_hi_cnt = 0, glb_lo_cnt = 0; /* TCAM direct routes */
    _alpm_dbg_data_t dbg[ALPM_BKT_PID_CNT]; /* per Private and Global */
    _alpm_dbg_data_t *dp;
    _alpm_bkt_pool_conf_t *bp_conf;
    char ip_buf[IP6ADDR_STR_LEN];
    char *lvl_str[] = {
        "2-Level",
        "3-Level"
    };
    char *mode_str[] = {
        "ALPM-Combined",
        "ALPM-Parallel",
        "ALPM-TCAM_ALPM"
    };
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 urpf, alpm, fv, rval;
    uint32 mode = 0;
    uint32 bnkbits = 0;
    char *dw_str = "Double Wide";
    char *sw_str = "Single Wide";
    char *ipver_str[] = {
        "IPv4",
        "IPv6-64",
        "IPv6-128",
        "IPv4-1",
        "IPv6-64-1"
    };
    uint32 fmt_ent_max[] = {
        6,
        3,
        2,
        4,
        3
    };
#endif
    int pid_count;

    ALPMC_INIT_CHECK(u);

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);

        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                continue; /* Combined Mode uses PID=0 only for dbg counters */
            }

            dp = &dbg[pid];
            dp->route_cnt = 0;
            dp->hit_cnt = 0;
            sal_memset(&dp->dc[i], 0, sizeof(_alpm_dbgcnt_t));
            dp->dc[i].bucket_max = ACB_BKT_CNT(acb);
            dp->dc[i].bank_max = BPC_BNK_CNT(bp_conf);
            dp->dc[i].ent_max = dp->dc[i].bank_max * BPC_FMT_ENT_MAX(bp_conf, 1);
            alloc_sz = dp->dc[i].bucket_max * sizeof(int);
            ALPM_ALLOC_EG(dp->bktref[i], alloc_sz, "bktref");

            /* Calculate maximum pivot supported (only useful for TCAM level) */
            if (ACB_HAS_TCAM(acb)) {
                for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {
                    dp->dc[i].pvt_max[pkm] =
                       (pkm == ALPM_PKM_32B ? 2 : 1) * tcam_table_size(u, app, pkm);
                }
            }
        }
    }

    acb = ACB_TOP(u, app); /* from top to bottom */

    for (pk = 0; pk < ALPM_PKM_CNT; pk++) {
        if (ALPM_TCAM_TBL_SKIP(u, pk)) {
            continue; /* skip duplicated or invalid TCAM table */
        }

        idx_end = tcam_table_size(u, app, pk);
        /* Walk all TCAM entries */
        for (idx = 0; idx < idx_end; idx++) {
            ALPM_IEG(tcam_entry_read(u, app, pk, lpm_entry, idx, idx));

            for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
                rv_tcam = tcam_valid_entry_mode_get(u, app, pk, lpm_entry,
                              &step_size, &pkm, &ipv6, &key_mode, sub_idx);
                if (BCM_FAILURE(rv_tcam)) {
                    if (rv_tcam == BCM_E_INTERNAL) {
                        ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type (IPv6):%d"
                                  " in idx:%d.%d\n",
                          TCAM_TBL_NAME(acb, pk), key_mode, ipv6, idx, sub_idx));
                    }
                    continue;
                }
                ipt = ALPM_PKM2IPT(pkm);
                tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &vrf_id);

                /* Combined Mode uses PID=0 only for dbg counters */
                pid = ACB_BKT_VRF_PID(acb, vrf_id);
                dp = &dbg[pid];
                if (vrf_id == ALPM_VRF_ID_GHI(u)) {
                    glb_hi_cnt++;
                } else if (vrf_id == ALPM_VRF_ID_GLO(u)) {
                    glb_lo_cnt++;
                }

                /* Get Pre-pivot key IP_ADDR and prefix length (mask) */
                sal_memset(key, 0, sizeof(key));
                key_pfx_len = 0;
                tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, key);
                tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &key_pfx_len);

                /* process pre-pivor ASSOC_DATA */
                tcam_entry_adata_get(u, app, pkm, lpm_entry, sub_idx, &adata0);
                use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
                           ALPM_VRF_ID_HAS_BKT(u, vrf_id);

                /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
                 * resides in TCAM */
                if (!use_alpm) {
                    dp->route_cnt++;
                    if (ALPMC(u)->_alpm_dbg_info_bmp &
                        (_ALPM_DBG_INFO_PVT | _ALPM_DBG_INFO_RTE)) {
                        alpm_util_cfg_construct(u, vrf_id, ipt, key, key_pfx_len,
                                                &adata0, &lpm_cfg);
                        if (ALPM_IS_IPV4(ipt)) {
                            alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
                        } else {
                            alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
                        }

                        if (!ALPM_HIT_SKIP(u) &&
                            tcam_entry_hit_get(u, app, pkm, lpm_entry, sub_idx)) {
                            dp->hit_cnt++;
                            ALPM_INFO(("! "));
                        }

                        ALPM_INFO(("VRF:%d PKM:%s TCAM: Direct Route"
                                   " idx:%d.%d Sublen:%d Dest:%d IP:%s\n",
                                   vrf_id, alpm_util_pkm_str[pkm], idx, sub_idx, key_pfx_len,
                                   adata0.defip_ecmp_index, ip_buf));
                    }
                    continue;
                }

                if (ALPMC(u)->_alpm_dbg_info_bmp &
                    (_ALPM_DBG_INFO_PVT | _ALPM_DBG_INFO_RTE)) {
                    ALPM_INFO(("\n"));
                }

                /* process ALPM_DATA level recursively from top to bottom */
                tcam_entry_bdata_get(u, app, pkm, lpm_entry, sub_idx, fent);

                ALPM_IEG(alpm_cb_dbg_process(u, vrf_id, pkm, acb, fent, 0,
                              key, key_pfx_len, idx, sub_idx, &adata0, dp));

            } /* for sub_idx */
        } /* for idx */
    } /* for pk */

    if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_CNT) {
        bit128 = !!tcam_table_size(u, app, ALPM_PKM_128);
        /* count bkt sharing */
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            pid_count = alpm_pid_count(app);
            for (pid = 0; pid < pid_count; pid++) {
                bp_conf = ACB_BKT_POOL(acb, pid);
                if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                    continue; /* Combined Mode uses PID=0 only */
                }
                dp = &dbg[pid];
                for (j = 0; j < dp->dc[i].bucket_max; j++) {
                    if (dp->bktref[i][j] != 0) {
                        dp->dc[i].bucket_used++;
                    }

                    for (k = 0; k < ALPM_BPB_MAX; k++) {
                        if (dp->bktref[i][j] == (k+1)) {
                            dp->dc[i].bs[k]++;
                        }
                    }
                }
            }
        }

        LOG_CLI(("\n"));
        LOG_CLI(("%-24s%s %s\n", "Mode:",
                 lvl_str[ACB_CNT(u) - 1], mode_str[ALPM_MODE(u)]));
        LOG_CLI(("%-24s%d\n", "IPv6 Bits:", bit128 ? 128 : 64));

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(u)) { /* for TD3X specific only */
            urpf = SOC_URPF_STATUS_GET(u);
            LOG_CLI(("%-24s%s\n", "uRPF:", urpf ? "Enable" : "Disable"));
            for (i = 0; i < 5; i++) {
                LOG_CLI(("%-24s%s | %2d EPBkt | %2d EPBnk\n",
                    ipver_str[i],
                    ACB_BKT_DW(acb) ? dw_str : sw_str,
                    (fmt_ent_max[i] * ACB_PHY_BNK_PER_BKT(acb)) << ACB_BKT_DW(acb),
                    fmt_ent_max[i]));
            }
            LOG_CLI(("\n"));
            LOG_CLI(("HW Status:\n"));
            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(u, &rval));
            alpm = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, LPM_MODEf);
            if (alpm != 0) {
                mode = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, LOOKUP_MODEf);
            }
            urpf = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, DEFIP_RPF_ENABLEf);
            LOG_CLI(("\n"));
            LOG_CLI(("%-24s%s\n", "Mode:", mode_str[mode]));
            bit128 = !!soc_mem_index_count(u, L3_DEFIP_PAIR_128m);
            LOG_CLI(("%-24s%d\n", "IPv6 Bits:", bit128 ? 128 : 64));
                LOG_CLI(("%-24s%s\n", "uRPF:", urpf ? "Enable" : "Disable"));
                if (alpm == 0) {
                return SOC_E_NONE;
            }
            if (mode == 0 && !urpf) {       /* Combined mode && uRPF */
                fv = 1;
            } else if (mode == 2) {         /* TCAM/ALPM mode */
                fv = 1;
            } else {                        /* Parallel mode */
                fv = 0;
            }
            LOG_CLI(("%-24s%s\n", "IPv4:", fv ? dw_str : sw_str));
            if (mode == 0 && !urpf) {       /* Combined mode && uRPF */
                fv = 1;
            } else if (mode == 2) {         /* TCAM/ALPM mode */
                fv = 1;
            } else {                        /* Parallel mode */
                fv = 0;
            }
            LOG_CLI(("%-24s%s\n", "IPv6-64:", fv ? dw_str : sw_str));
            LOG_CLI(("%-24s%s\n", "IPv6-128:", fv ? dw_str : sw_str));
            if (SOC_REG_IS_VALID(u, ISS_LOG_TO_PHY_BANK_MAP_2r)) {
                SOC_IF_ERROR_RETURN(READ_ISS_LOG_TO_PHY_BANK_MAP_2r(u, &rval));
                bnkbits = soc_reg_field_get(u, ISS_LOG_TO_PHY_BANK_MAP_2r, rval,
                                            ALPM_BANK_MODEf);
            }
            LOG_CLI(("%-24s%d\n", "Num Banks:", bnkbits == 0 ? 8 : 4));
        }
#endif
        LOG_CLI(("%-24s%s\n", "Bucket sharing:", "Enable"));
        LOG_CLI(("%-24s%s\n", "Global/Private Zoned:",
        ALPM_TCAM_ZONED(u, app) ? "Yes" : "No"));
        LOG_CLI(("%-24s%s\n", "SW Prefix Lookup:", "Yes"));
        LOG_CLI(("%-24s%d\n", "Direct Route Glb High:", glb_hi_cnt));
        LOG_CLI(("%-24s%d\n", "Direct Route Glb Low:", glb_lo_cnt));
        LOG_CLI(("---------------------------------------\n"));
        LOG_CLI(("\n"));

        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            LOG_CLI(("== %s\n", alpm_util_acb_str[i]));
            LOG_CLI(("%-24s%d\n", "Num Banks:", ACB_PHY_BNK_PER_BKT(acb)));
            LOG_CLI(("%-24s%d\n", "Bank bits:", ACB_BNK_BITS(acb)));
            LOG_CLI(("%-24s%d\n", "Num Buckets:", ACB_BKT_CNT(acb)));
            LOG_CLI(("%-24s%d\n", "Bucket bits:", ACB_BKT_BITS(acb)));
            LOG_CLI(("%-24s0x%x\n", "Bucket Mask:", ACB_BKT_CNT(acb) - 1));
            LOG_CLI(("\n"));
            pid_count = alpm_pid_count(app);
            for (pid = 0; pid < pid_count; pid++) {
                bp_conf = ACB_BKT_POOL(acb, pid);
                if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                    continue; /* Combined Mode uses PID=0 only */
                }
                dp = &dbg[pid];

                if (ALPM_TCAM_ZONED(u, app)) {
                    LOG_CLI(("%s\n", (pid == 0 ? "-- Private:" : "-- Global:")));
                } else {
                    LOG_CLI(("-- Private & Global:\n"));
                }

                LOG_CLI(("%-24s\n", "Pivots:"));
                for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {
                    if (ACB_HAS_TCAM(acb)) {
                        LOG_CLI(("  %-22s%d/%-4d\n", alpm_util_pkm_str[pkm],
                                 dp->dc[i].pvt_used[pkm],
                                 dp->dc[i].pvt_max[pkm]));
                    } else {
                        LOG_CLI(("  %-22s%d/%-4d\n", alpm_util_pkm_str[pkm],
                                 dp->dc[i].pvt_used[pkm],
                                 dp->dc[i-1].ent_max));
                    }
                }

                if (ALPM_BKT_SHARE_THRES(u)) {
                    LOG_CLI(("%-24s%d/%-4d\n", "Buckets[V4]:",
                             BPC_BKT_USED(bp_conf, ALPM_IPT_V4), dp->dc[i].bucket_max));
                    LOG_CLI(("%-24s%d/%-4d\n", "Buckets[V6]:",
                             BPC_BKT_USED(bp_conf, ALPM_IPT_V6), dp->dc[i].bucket_max));
                }

                LOG_CLI(("%-24s%d/%-4d\n", "Buckets:",
                         dp->dc[i].bucket_used, dp->dc[i].bucket_max));
                LOG_CLI(("%-24s%d/%-4d\n", "Banks:",
                         dp->dc[i].bank_used, dp->dc[i].bank_max));
                ent_used_tot = dp->dc[i].ent_used[ALPM_PKM_32B] +
                    dp->dc[i].ent_used[ALPM_PKM_64B] +
                    dp->dc[i].ent_used[ALPM_PKM_128];
                LOG_CLI(("%-24s%d/%-4d\n", "Bucket Entries:",
                         ent_used_tot, dp->dc[i].ent_max));
                for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {
                    LOG_CLI(("  %-22s%d\n", alpm_util_pkm_str[pkm],
                             dp->dc[i].ent_used[pkm]));
                }

                LOG_CLI(("%s\n", "Bucket Sharing Count:"));
                for (k = 0; k < BPC_BNK_PER_BKT(bp_conf); k++) {
                    LOG_CLI(("  %1d%-21s%d\n", (k+1), "->1:", dp->dc[i].bs[k]));
                }

                if (ACB_HAS_RTE(acb, ALPM_BPC_PID_TO_VRF_ID(u, pid))) {
                    LOG_CLI(("%-24s%d\n", "Total Routes:", dp->route_cnt));
                    LOG_CLI(("%-24s%d\n", "Total Hit Routes:", dp->hit_cnt));
                }
                LOG_CLI(("\n"));
            }
        }
    }

bad:
    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);
        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                continue; /* Combined Mode uses PID=0 only */
            }
            dp = &dbg[pid];
            if (dp->bktref[i]) {
                alpm_util_free(dp->bktref[i]);
                dp->bktref[i] = NULL;
            }
        }
    }
    return rv;
}

/* Compare a prefix sw LPM lookup result (should match to the route prefix) */
STATIC int
alpm_pfx_sw_lpm_route_cb(_alpm_pfx_node_t *pfx_node, void *user_data)
{
    int rv = BCM_E_NONE;
    _alpm_trv_data_t    *trv_data = (_alpm_trv_data_t *)user_data;
    int                 u = trv_data->unit;
    int                 ipt = trv_data->ipt;
    int                 vrf_id = trv_data->vrf_id;
    _alpm_cb_t          *acb = trv_data->acb;
    alpm_lib_trie_t     *pvt_trie = NULL;
    alpm_lib_trie_t     *bkt_trie = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;
    _alpm_bkt_node_t    *bkt_node = NULL;
    char str[80], str1[80];

    /* Exclude virtual default pfx 0/0 inserted in alpm_vrf_init */
    if (pfx_node->bkt_ptr == NULL) {
        return rv;
    }

    /* L1 pvt_trie LPM lookup */
    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    rv = alpm_lib_trie_find_lpm(pvt_trie, pfx_node->key, pfx_node->key_len,
                                (alpm_lib_trie_node_t **)&pvt_node);
    if (BCM_FAILURE(rv) || pvt_node == NULL) {
        trv_data->err++;
        alpm_util_trie_pfx_str(u, ipt, pfx_node->key, pfx_node->key_len, str);
        ALPM_ERR(("vrf:%d %s failed (%d) in L1 pvt_trie lpm find\n",
                  vrf_id, str, rv));
        return rv;
    }

    /* L1 pvt_bkt_trie LPM lookup */
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_find_lpm(bkt_trie, pfx_node->key, pfx_node->key_len,
                                (alpm_lib_trie_node_t **)&bkt_node);
    if (BCM_FAILURE(rv) || bkt_node == NULL) {
        trv_data->err++;
        alpm_util_trie_pfx_str(u, ipt, pfx_node->key, pfx_node->key_len, str);
        alpm_util_trie_pfx_str(u, ipt, pvt_node->key, pvt_node->key_len, str1);
        ALPM_ERR(("vrf:%d %s L1 Pvt %s failed (%d) in L1 pvt_bkt_trie lpm find\n",
                  vrf_id, str, str1, rv));
        return rv;
    }

    /* Next level pivot */
    if (ACB_HAS_PVT(acb, vrf_id)) {
        acb = ACB_DWN(u, acb);

        /* L2 pvt_trie search */
        pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
        pvt_node = NULL;
        rv = alpm_lib_trie_search(pvt_trie, bkt_node->key, bkt_node->key_len,
                                  (alpm_lib_trie_node_t **)&pvt_node);
        if (BCM_FAILURE(rv) || pvt_node == NULL) {
            trv_data->err++;
            alpm_util_trie_pfx_str(u, ipt, pfx_node->key, pfx_node->key_len, str);
            alpm_util_trie_pfx_str(u, ipt, bkt_node->key, bkt_node->key_len, str1);
            ALPM_ERR(("vrf:%d %s L2 Bkt %s failed (%d) in L2 pvt_trie search\n",
                      vrf_id, str, str1, rv));
            return rv;
        }

        /* L2 pvt_bkt_trie LPM lookup */
        bkt_trie = PVT_BKT_TRIE(pvt_node);
        bkt_node = NULL;
        rv = alpm_lib_trie_find_lpm(bkt_trie, pfx_node->key, pfx_node->key_len,
                                    (alpm_lib_trie_node_t **)&bkt_node);
        if (BCM_FAILURE(rv) || bkt_node == NULL) {
            trv_data->err++;
            alpm_util_trie_pfx_str(u, ipt, pfx_node->key, pfx_node->key_len, str);
            alpm_util_trie_pfx_str(u, ipt, pvt_node->key, pvt_node->key_len, str1);
            ALPM_ERR(("vrf:%d %s L2 Pvt %s failed (%d) in L2 pvt_bkt_trie lpm find\n",
                      vrf_id, str, str1, rv));
            return rv;
        }
    }

    /* Final level of route */
    if (ACB_HAS_RTE(acb, vrf_id)) {
        if (pfx_node->bkt_ptr != bkt_node) { /* LPM lookup result no match */
            trv_data->err++;
            alpm_util_trie_pfx_str(u, ipt, pfx_node->key, pfx_node->key_len, str);
            alpm_util_trie_pfx_str(u, ipt, bkt_node->key, bkt_node->key_len, str1);
            ALPM_ERR(("vrf:%d %s and LPM lookup %s mismatch\n",
                      vrf_id, str, str1));
            return rv;
        }
    }
    return rv;
}

/*
 * Function:
 *      alpm_pfx_sw_route_sanity
 * Purpose:
 *      Sw route lookup for all prefix trie routes.
 * Parameters:
 *      u        - (In)Device unit
 *      tot_err  - (In/Out)Total errors during lookup
 * Returns:
 *      BCM_E_XXX
 */
int
alpm_pfx_sw_route_sanity(int u, int app, int *tot_err)
{
    int vrf_id, ipt;
    _alpm_trv_data_t trv_data;
    _alpm_cb_t *acb = ACB(u, 0, app);

    trv_data.err = 0;
    trv_data.unit = u;
    trv_data.acb = acb;
    for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
        trv_data.ipt = ipt;
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                continue;
            }
            trv_data.vrf_id = vrf_id;
            (void)bcm_esw_alpm_pfx_traverse(u, app, vrf_id, ipt,
                                            alpm_pfx_sw_lpm_route_cb, &trv_data);
        }
    }

    if (trv_data.err) {
        ALPM_ERR(("Number of routes with SW LPM routing issue: %d\n", trv_data.err));
        *tot_err += trv_data.err;
    }

    return BCM_E_NONE;
}

/* Compare a prefix Hw lookup result (ip, sublen, intf) */
STATIC int
alpm_pfx_hw_route_cb(_alpm_pfx_node_t *pfx_node, void *user_data)
{
    _alpm_trv_data_t    *trv_data = (_alpm_trv_data_t *)user_data;
    int                 u = trv_data->unit;
    int                 ipt = trv_data->ipt;
    int                 vrf_id = trv_data->vrf_id;
    int                 found_vrf_id;
    int                 ipv6 = ALPM_IS_IPV6(ipt);
    _alpm_bkt_node_t    *bkt_node = pfx_node->bkt_ptr;
    _bcm_defip_cfg_t    lpm_cfg;
    int                 nh_ecmp_idx, dest, err = 0;
    bcm_ip_t            defip_ip_addr = 0;
    bcm_ip6_t           defip_ip6_addr = {0};
    int                 defip_sub_len;
    char                ip_buf[IP6ADDR_STR_LEN];
    int                 rv = BCM_E_NONE;

    if (bkt_node == NULL) {
        return rv;
    }

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    lpm_cfg.defip_flags |= (ipv6 ? BCM_L3_IP6 : 0);

    alpm_trie_pfx_to_cfg(u, pfx_node->key, pfx_node->key_len, &lpm_cfg);

    dest = bkt_node->adata.defip_ecmp_index;
    defip_sub_len = lpm_cfg.defip_sub_len;
    if (ipv6) {
        sal_memcpy(&defip_ip6_addr, &lpm_cfg.defip_ip6_addr, sizeof(defip_ip6_addr));
    } else {
        defip_ip_addr = lpm_cfg.defip_ip_addr;
    }

    rv = bcm_esw_alpm_find(u, &lpm_cfg, &nh_ecmp_idx);

    if (BCM_FAILURE(rv)) {
        err = 1;
    } else {
        found_vrf_id = ALPM_LPM_VRF_ID(u, &lpm_cfg);
        /* VRF override match is OK */
        if (found_vrf_id != ALPM_VRF_ID_GHI(u)) {
            if ((defip_sub_len != lpm_cfg.defip_sub_len) || (dest != nh_ecmp_idx)) {
                err = 2;
            } else {
                if (ipv6) {
                    if (sal_memcmp(&defip_ip6_addr, &lpm_cfg.defip_ip6_addr, sizeof(defip_ip6_addr))) {
                        err = 3;
                    }
                } else {
                    if (defip_ip_addr != lpm_cfg.defip_ip_addr) {
                        err = 3;
                    }
                }
            }
        }
    }

    if (err) {
        if (ipv6) {
            alpm_util_fmt_ip6addr(ip_buf, defip_ip6_addr);
        } else {
            alpm_util_fmt_ipaddr(ip_buf, defip_ip_addr);
        }

        ALPM_ERR(("PFX %p->BKT %p: VRF:%d IP:%s/%d Dest:%d => ",
                  pfx_node, bkt_node, vrf_id, ip_buf, defip_sub_len, dest));

        if (err == 1) {
            ALPM_ERR(("couldn't find in Hw (%d)!\n", rv));
        } else {
            if (ipv6) {
                alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
            } else {
                alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
            }
            ALPM_ERR(("wrong Hw route!\n"));
            ALPM_ERR(("   Current best match Hw route: VRF:%d IP:%s/%d Dest:%d\n",
                      vrf_id, ip_buf, lpm_cfg.defip_sub_len, nh_ecmp_idx));
        }

        trv_data->err++;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      alpm_pfx_hw_route_sanity
 * Purpose:
 *      Hw route lookup for all prefix trie routes.
 * Parameters:
 *      u        - (In)Device unit
 *      tot_err  - (In/Out)Total errors during lookup
 * Returns:
 *      BCM_E_XXX
 */
int
alpm_pfx_hw_route_sanity(int u, int app, int *tot_err)
{
    int vrf_id, ipt;
    _alpm_trv_data_t trv_data;

    trv_data.err = 0;
    trv_data.unit = u;
    for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
        trv_data.ipt = ipt;
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            if (!ACB_VRF_INITED(u, ACB_VRF_BTM(u, vrf_id, app), vrf_id, ipt)) {
                continue;
            }
            trv_data.vrf_id = vrf_id;
            (void)bcm_esw_alpm_pfx_traverse(u, app, vrf_id, ipt,
                                            alpm_pfx_hw_route_cb, &trv_data);
        }
    }

    if (trv_data.err) {
        ALPM_ERR(("\nNumber of routes with HW routing issue: %d\n", trv_data.err));
        *tot_err += trv_data.err;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      alpm_cb_sanity_process
 * Purpose:
 *      ALPM control block (level) sanity process.
 *      It's recursive from top to bottom.
 *      Detection of HW/Sw mismatch or conflict, entry data anormality etc.
 * Parameters:
 *      u        - (In)Device unit
 *      vrf_id   - (In)VRF ID
 *      pkm      - (In)PKM
 *      acb      - (In)ALPM control block
 *      pvtfmt   - (In)Pivot bank format (ignored for TCAM)
 *      fent     - (In)ALPM_DATA entry
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      idx      - (In)Memory Index
 *      sub_idx  - (In)Memory sub_idx (for half-entry) or ent index
 *      adata    - (In)Assoc Data structure pointer
 *      sanity   - (Out)Carrying sanity check info
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_cb_sanity_process(int u, int vrf_id, int pkm, _alpm_cb_t *acb,
                       int pvtfmt, void *fent, uint32 *key, int key_len,
                       int idx, int sub_idx,
                       _alpm_bkt_adata_t *adata, _alpm_sanity_t *sanity)
{
    uint32 bkt_entry[ALPM_MEM_ENT_MAX];
    uint32 fent1[ALPM_MEM_ENT_MAX];
    uint32 ftmp[ALPM_MEM_ENT_MAX];
    uint32 new_key[4];
    uint32 pfx[5], pvt_idx, bkt_idx;
    alpm_lib_trie_t *pvt_trie, *pfx_trie, *bkt_trie;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_pfx_node_t *bkt_node;
    _alpm_pfx_node_t *pfx_node;
    char ip_buf[IP6ADDR_STR_LEN];
    char err_msg[160];
    uint32 fmt, valid;
    int sublen, new_len, def_miss;
    int idx1, eid, bnk, trie_idx, trie_ent;
    _alpm_bkt_adata_t adata1, bm_adata, adata_local;
    int rv = BCM_E_NONE;
    int bkt_empty;
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_bkt_info_t bkt_info;
    int pvt_node_pfx_cnt = 0;
    int hw_bkt_pfx_cnt = 0;
    _alpm_tbl_t bkt_tbl;
    _alpm_tbl_t pvt_tbl;
    int ipt;

    if (ACB_IDX(acb) >= ACB_CNT(u)) {
        return BCM_E_INTERNAL; /* wrong ALPM control level called */
    }

    ipt = ALPM_PKM2IPT(pkm);
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
    bkt_tbl = ACB_BKT_TBL(acb, vrf_id);
    pvt_tbl = (ACB_HAS_TCAM(acb) ? acb->pvt_tbl[pkm] :
                ACB_BKT_TBL(ACB_UPR(u, acb), vrf_id));

    /**** Pivot level ****/
    alpm_util_bkt_info_get(u, vrf_id, ipt, pkm, acb, fent, sub_idx,
                           &bkt_info, &sublen, &def_miss);

    /* kshift must be same as previous key_len */
    if (!ACB_BKT_FIXED_FMT(acb, 1) && sublen != key_len) {
        sanity->error++;
        ALPM_ERR(("%s: ALPM_DATA invalid kshift:%d vs pivot len:%d"
                  " idx:%d.%d (%d)\n", ALPM_TBL_NAME(pvt_tbl), sublen,
                  key_len, idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }
    if (BI_BKT_IDX(&bkt_info) >= ACB_BKT_CNT(acb)) {
        sanity->error++;
        ALPM_ERR(("%s: ALPM_DATA invalid bkt_ptr idx:%d.%d (%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }

    /* HW pivot key vs SW pvt trie */
    rv = BCM_E_NONE;
    alpm_trie_key_to_pfx(u, ipt, key, key_len, pfx);
    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    if (pvt_trie != NULL) {
        rv = alpm_lib_trie_search(pvt_trie, pfx, key_len, (alpm_lib_trie_node_t **)&pvt_node);
        if (BCM_SUCCESS(rv)) {
            if (ACB_HAS_TCAM(acb)) { /* compare tcam_idx */
                if (_tcam_pkm_uses_half_wide(u, pkm, PVT_IS_IPMC(pvt_node))) {
                    pvt_idx = idx << 1 | sub_idx;
                    trie_idx = PVT_IDX(pvt_node) >> 1;
                    trie_ent = PVT_IDX(pvt_node) & 1;
                } else {
                    pvt_idx = idx;
                    trie_idx = PVT_IDX(pvt_node);
                    trie_ent = 0;
                }
                if (pvt_idx != PVT_IDX(pvt_node)) {
                    rv = BCM_E_INTERNAL;
                    sanity->error++;
                    sal_sprintf(err_msg, "Error: duplicated key as pvt tcam_idx:%d.%d",
                                trie_idx, trie_ent);
                }
            }
        } else { /* rv = BCM_E_NOT_FOUND */
            sanity->error++;
            sal_sprintf(err_msg, "Error: not found in pvt trie");
            pvt_node = NULL;
        }
    } else {
        rv = BCM_E_INTERNAL;
        sanity->error++;
        sal_sprintf(err_msg, "Error: pvt trie doesn't exist");
    }

    if (BCM_FAILURE(rv)) {
        alpm_util_cfg_construct(u, vrf_id, ipt, key, key_len, adata, &lpm_cfg);
        if (ALPM_IS_IPV4(ipt)) {
            alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
        } else {
            alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
        }
        if (ACB_HAS_TCAM(acb)) {
            ALPM_ERR(("VRF:%d IPT:%s %s: TCAM idx:%d.%d Sublen:%d bpmDest:%d IP:%s\n ==> %s\n",
                      vrf_id, alpm_util_ipt_str[ipt], ALPM_TBL_NAME(pvt_tbl),
                      idx, sub_idx, key_len, adata->defip_ecmp_index,
                      ip_buf, err_msg));
        } else {
            ALPM_ERR(("VRF:%d IPT:%s %s: PVT idx:%d.%d Sublen:%d fmt:%d bpmDest:%d IP:%s\n ==> %s\n",
                       vrf_id, alpm_util_ipt_str[ipt], ALPM_TBL_NAME(pvt_tbl),
                       idx, sub_idx, key_len, pvtfmt,
                       adata->defip_ecmp_index, ip_buf, err_msg));
        }
    } else {
        if (pvt_node && pvt_node->bkt_trie && pvt_node->bkt_trie->trie) {
            pvt_node_pfx_cnt = pvt_node->bkt_trie->trie->count;
        }
    }

#if 0
    /* Pivot Assoc_Data Hw validation (only if Default_Miss==0) */
    if (def_miss == FALSE) {
        /* through Hw best match lookup */
        ALPM_VERB(("%s: check Pivot idx:%d.%d def_rte => ",
                      ALPM_TBL_NAME(pvt_tbl), idx, sub_idx));
        sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
        /* ALPMTBD: find best match from ALPM only (exclude Direct Routes) */
        rv = alpm_find_best_match(u, vrf_id, pkm, key, key_len, &lpm_cfg);

        if (BCM_FAILURE(rv)) {
            sanity->error++;
            ALPM_ERR(("%s: Pivot idx:%d.%d BPM missing (rv=%d)\n",
                      ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, rv));
        } else {
            alpm_util_adata_cfg_to_trie(u, &lpm_cfg, &bm_adata);
            /* filter flags BCM_L3_HIT & BCM_L3_IP6 from
               bm_adata.defip_flags before memcmp */
            ALPM_FLAGS_FILTER(bm_adata.defip_flags);
            if (sal_memcmp((void *)adata, (void *)&bm_adata,
                           sizeof(_alpm_bkt_adata_t)) != 0) {
                sanity->error++;
                ALPM_ERR(("%s: Pivot idx:%d.%d Assoc Data mismatch"
                          " (current vs best match):\n",
                          ALPM_TBL_NAME(pvt_tbl), idx, sub_idx));
                ALPM_ERR((" defip_flags: %x vs %x, dest: %d vs %d\n",
                          adata->defip_flags, bm_adata.defip_flags,
                          adata->defip_ecmp_index, bm_adata.defip_ecmp_index));
            }
        }
    }
#endif

    /* Pvt default route verification & Assoc_Data Sw validation */
    if (pfx_trie != NULL) {
        rv = alpm_lib_trie_find_lpm(pfx_trie, pfx, key_len,
                       (alpm_lib_trie_node_t **)&pfx_node);
        if (BCM_FAILURE(rv)) {
            if (def_miss == 0) {
                sanity->error++;
                ALPM_ERR(("%s: Pivot idx:%d.%d BPM missing (rv=%d)\n",
                           ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, rv));
            }
        } else {
            if (pvt_node != NULL) {
                if (PVT_BKT_DEF(pvt_node) != pfx_node->bkt_ptr) {
                    sanity->error++;
                    ALPM_ERR(("%s: Pivot idx:%d.%d key_len:%d bpm_len:%d DEF_PFX %p"
                              " diff than BPM BKT %p\n",
                              ALPM_TBL_NAME(pvt_tbl), idx, sub_idx,
                              PVT_KEY_LEN(pvt_node), PVT_BPM_LEN(pvt_node),
                              PVT_BKT_DEF(pvt_node), pfx_node->bkt_ptr));
                } else {
                    if (def_miss == 1 && PVT_BKT_DEF(pvt_node)) {
                        sanity->error++;
                        ALPM_ERR(("%s: Pivot idx:%d.%d Default_Miss=1 but BPM route exits\n",
                                   ALPM_TBL_NAME(pvt_tbl), idx, sub_idx));
                    }
                }
            }

            adata_local = *adata;
            if (pfx_node->bkt_ptr) {
                bm_adata = pfx_node->bkt_ptr->adata;
                /*
                 * filter flags BCM_L3_HIT & BCM_L3_IP6 from
                 * bm_adata.defip_flags and adata.defip_flags
                 * before memcmp.
                 */
                ALPM_FLAGS_FILTER(bm_adata.defip_flags);
                ALPM_FLAGS_FILTER(adata_local.defip_flags);
                if (sal_memcmp((void *)&adata_local, (void *)&bm_adata,
                               sizeof(_alpm_bkt_adata_t)) != 0) {
                    sanity->error++;
                    ALPM_ERR(("%s: Pivot idx:%d.%d Assoc Data mismatch"
                              "(current vs bm) flags:%x vs %x dest:%d vs %d\n",
                              ALPM_TBL_NAME(pvt_tbl), idx, sub_idx,
                              adata_local.defip_flags, bm_adata.defip_flags,
                              adata_local.defip_ecmp_index,
                              bm_adata.defip_ecmp_index));
                }
            } else {
                 /* Parallel & TCAM mix mode, pivot sanity check:
                    no best match def_rte but default_miss = 0 */
                if ((def_miss == 0) && ALPM_TCAM_ZONED(u, acb->app)) {
                    sanity->error++;
                    ALPM_ERR(("%s: Pivot idx:%d.%d w/o bm_rte => "
                              "wrong def_miss:0 dest:%d\n",
                              ALPM_TBL_NAME(pvt_tbl), idx, sub_idx,
                              adata_local.defip_ecmp_index));
                }
            }
        }
    }

    /* Hw bkt_info vs SW bkt_info */
    if (pvt_node != NULL && (!ACB_BKT_FIXED_FMT(acb, 1))) {
        _alpm_bkt_info_t *sw_binfo = &PVT_BKT_INFO(pvt_node);
        int i;
        int times_bnk01 = 0; /* count times that banks change from 0 (disabled) to 1 (used) */
        int prev_bnk0 = TRUE; /* previous bank was 0 (disabled) */
        int bnkpb = ACB_BNK_PER_BKT(acb, vrf_id);
        int rofs = bkt_info.rofs;

        if (bkt_info.rofs != sw_binfo->rofs) {
            sanity->error++;
            ALPM_ERR(("%s: Pivot idx:%d.%d mismatch hw/sw rofs (%d vs %d).\n",
                      ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, bkt_info.rofs, sw_binfo->rofs));
        }

        for (i = rofs; i < rofs + bnkpb; i++) {
            bnk = i % bnkpb;
            if (bkt_info.bnk_fmt[bnk] != sw_binfo->bnk_fmt[bnk]) {
                sanity->error++;
                ALPM_ERR(("%s: Pivot idx:%d.%d mismatch hw/sw bnk_fmt[%d] (%d vs %d).\n",
                          ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, bnk,
                          bkt_info.bnk_fmt[bnk], sw_binfo->bnk_fmt[bnk]));
            }
            /* Check all occupied banks are continuous */
            if (bkt_info.bnk_fmt[bnk] != 0) {
                if (prev_bnk0 == TRUE) {
                    times_bnk01++; /* increment */
                }
                prev_bnk0 = FALSE; /* set to false now */
            } else {
                prev_bnk0 = TRUE; /* set to true again */
            }
        }

        if (times_bnk01 > 1) { /* no continuous used banks */
            sanity->error++;
            ALPM_ERR(("%s: Pivot idx:%d.%d no continuous occupied banks.\n",
                      ALPM_TBL_NAME(pvt_tbl), idx, sub_idx));
        }
    }

    bkt_empty = TRUE; /* each pivot must have at least one valid bucket entry */
    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        fmt = bkt_info.bnk_fmt[bnk];
        if ((fmt == 0) || (fmt > ACB_FMT_CNT(acb, vrf_id))) {
            if (fmt > ACB_FMT_CNT(acb, vrf_id)) {
                sanity->error++;
                ALPM_ERR(("%s: ALPM_DATA format wrong idx:%d.%d (bank=%d fmt=%d).\n",
                          ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, bnk, fmt));
            }
            continue;
        }

        /**** Bucket Level ****/
        idx1 = ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(&bkt_info), BI_BKT_IDX(&bkt_info), bnk);
        if (!ACB_BKT_FIXED_FMT(acb, 1)) { /* for TH3 only */
            if (ACB_VRF_PVT_PTR(acb, vrf_id, idx1) == NULL) {
                sanity->error++;
                ALPM_ERR(("%s: bucket info -> pvt_ptr == NULL at bank %d\n",
                          ALPM_TBL_NAME(bkt_tbl), idx1));
            } else {
                _alpm_pvt_node_t *pvt_node2;

                pvt_node2 = (_alpm_pvt_node_t *)ACB_VRF_PVT_PTR(acb, vrf_id, idx1);
                if (bkt_info.rofs != pvt_node2->bkt_info.rofs ||
                    bkt_info.bkt_idx != pvt_node2->bkt_info.bkt_idx ||
                    sal_memcmp(bkt_info.bnk_fmt, pvt_node2->bkt_info.bnk_fmt, sizeof(bkt_info.bnk_fmt)) != 0) {
                    sanity->error++;
                    ALPM_ERR(("\t%s: sw & hw bkt_info mismatch\n",
                              ALPM_TBL_NAME(bkt_tbl)));
                    ALPM_ERR(("\thw rofs %d bkt_idx %d fmt %d %d %d %d %d %d %d %d\n",
                              bkt_info.rofs, bkt_info.bkt_idx,
                              bkt_info.bnk_fmt[0],
                              bkt_info.bnk_fmt[1],
                              bkt_info.bnk_fmt[2],
                              bkt_info.bnk_fmt[3],
                              bkt_info.bnk_fmt[4],
                              bkt_info.bnk_fmt[5],
                              bkt_info.bnk_fmt[6],
                              bkt_info.bnk_fmt[7]));
                    ALPM_ERR(("\tsw rofs %d bkt_idx %d fmt %d %d %d %d %d %d %d %d\n",
                              pvt_node2->bkt_info.rofs, pvt_node2->bkt_info.bkt_idx,
                              pvt_node2->bkt_info.bnk_fmt[0],
                              pvt_node2->bkt_info.bnk_fmt[1],
                              pvt_node2->bkt_info.bnk_fmt[2],
                              pvt_node2->bkt_info.bnk_fmt[3],
                              pvt_node2->bkt_info.bnk_fmt[4],
                              pvt_node2->bkt_info.bnk_fmt[5],
                              pvt_node2->bkt_info.bnk_fmt[6],
                              pvt_node2->bkt_info.bnk_fmt[7]));
                }
            }
        }
        ALPM_IER(alpm_bkt_entry_read_no_cache(u, ACB_BKT_TBL(acb, vrf_id),
                                              acb, bkt_entry, idx1));

        /* verify bucket bank ptr duplication */
        if (sanity->bktptr[ACB_IDX(acb)][idx1] == -1) {
            sanity->bktptr[ACB_IDX(acb)][idx1] = idx;
        } else {
            /* No need to check conflict bucket for bucket sharing */
            if (!ACB_BKT_FIXED_FMT(acb, vrf_id) &&
                idx != sanity->bktptr[ACB_IDX(acb)][idx1]) {
                sanity->error++;
                ALPM_ERR(("%s: conflict bucket bank at %d (upr_idx %d vs %d)\n",
                          ALPM_TBL_NAME(bkt_tbl), idx1, idx,
                          sanity->bktptr[ACB_IDX(acb)][idx1]));
            } /* else: conflict from higher CB, but this level is "OK" */
        }

        for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fmt); eid++) {
            uint32 key2[4] = {0};
            int len2 = 0;
            int sub_bkt_ptr = 0;

            /* process bucket entry Prefix */
            alpm_util_ent_ent_get(u, vrf_id, acb, bkt_entry, fmt, eid, ftmp);
            alpm_util_bkt_pfx_get(u, vrf_id, acb, ftmp, fmt, key2, &len2, &valid);
            alpm_util_bkt_adata_get(u, vrf_id, ipt, acb, ftmp, fmt, &adata1, &sub_bkt_ptr);

            if (!valid || BI_SUB_BKT_IDX(&bkt_info) != sub_bkt_ptr) {
                continue;
            } else {
                if (ACB_BKT_FIXED_FMT(acb, 1)) {
                    new_len = len2;
                    sal_memcpy(new_key, key2, sizeof(new_key));
                } else {
                    alpm_util_pfx_cat(u, ipt, key, sublen, key2, len2, new_key, &new_len);
                }
            }
            bkt_empty = FALSE; /* at least one bucket entry */
            hw_bkt_pfx_cnt ++;

            rv = BCM_E_NONE;
            if (new_len == 0 &&
                (new_key[0] != 0 || new_key[1] != 0 ||
                 new_key[2] != 0 || new_key[3] != 0)) {
                rv = BCM_E_INTERNAL;
                sanity->error++;
                sal_sprintf(err_msg,
                    "Error: length is 0 but key is not, in bkt%d bnk%d ent%d",
                    BI_BKT_IDX(&bkt_info), bnk, eid);
            }

            if (BCM_SUCCESS(rv)) {
                /* process bucket entry ASSOC_DATA */
                alpm_trie_key_to_pfx(u, ipt, new_key, new_len, pfx);

                /* HW bucket key vs SW bkt trie */
                if (pvt_node != NULL) {
                    bkt_trie = PVT_BKT_TRIE(pvt_node);
                    if (bkt_trie != NULL) {
                        rv = alpm_lib_trie_search(bkt_trie, pfx, new_len,
                                         (alpm_lib_trie_node_t **)&bkt_node);
                        if (BCM_SUCCESS(rv)) {
                            bkt_idx = idx1 | ((eid & ALPM_IDX_ENT_MASK)
                                              << ALPM_IDX_ENT_SHIFT);
                            if (bkt_idx != bkt_node->ent_idx) {
                                rv = BCM_E_INTERNAL;
                                sanity->error++;
                                trie_idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
                                trie_ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
                                sal_sprintf(err_msg,
                                            "Error: duplicated key as bkt ent_idx:%d(bkt%d bnk%d).%d",
                                            trie_idx, ALPM_IDX_TO_BKT(acb, bkt_node->ent_idx),
                                            ALPM_IDX_TO_BNK(acb, bkt_node->ent_idx),
                                            trie_ent);
                            }
                        } else {
                            sanity->error++;
                            sal_sprintf(err_msg, "Error: not found in bkt trie");
                        }
                    } else {
                        rv = BCM_E_INTERNAL;
                        sanity->error++;
                        sal_sprintf(err_msg, "Error: bkt trie doesn't exist");
                    }
                }
            }

            if (BCM_FAILURE(rv)) {
                alpm_util_cfg_construct(u, vrf_id, ipt, new_key, new_len,
                                        &adata1, &lpm_cfg);
                if (ALPM_IS_IPV4(ipt)) {
                    alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
                } else {
                    alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
                }
                ALPM_ERR(("VRF:%d IPT:%s %s: BKT idx:%d(bkt%d bnk%d).%d Sublen:%d"
                          " fmt:%d Dest:%d IP:%s\n ==> %s\n",
                          vrf_id, alpm_util_ipt_str[ipt],
                          ALPM_TBL_NAME(bkt_tbl),
                          idx1, BI_BKT_IDX(&bkt_info), bnk, eid, new_len, fmt,
                          adata1.defip_ecmp_index, ip_buf, err_msg));
            }

            /* end recursion here if reach to route level (last level) */
            if (ACB_HAS_RTE(acb, vrf_id)) {
                /* HW route vs SW prefix trie */
                if (BCM_SUCCESS(rv)) { /* if failed on bkt, bypass pfx check */
                    if (pfx_trie != NULL) {
                        rv = alpm_lib_trie_search(pfx_trie, pfx, new_len,
                                         (alpm_lib_trie_node_t **)&pfx_node);
                        if (BCM_SUCCESS(rv)) {
                            bkt_idx = idx1 | ((eid & ALPM_IDX_ENT_MASK)
                                              << ALPM_IDX_ENT_SHIFT);
                            if (bkt_idx != pfx_node->bkt_ptr->ent_idx) {
                                rv = BCM_E_INTERNAL;
                                sanity->error++;
                                trie_idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
                                trie_ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
                                sal_sprintf(err_msg,
                                            "Error: duplicated key as bkt ent_idx:%d.%d",
                                            trie_idx, trie_ent);
                            }
                        } else {
                            sanity->error++;
                            sal_sprintf(err_msg, "Error: not found in prefix trie");
                        }
                    } else {
                        rv = BCM_E_INTERNAL;
                        sanity->error++;
                        sal_sprintf(err_msg, "Error: pfx trie doesn't exist");
                    }

                    if (BCM_FAILURE(rv)) {
                        alpm_util_cfg_construct(u, vrf_id, ipt, new_key, new_len,
                                                &adata1, &lpm_cfg);
                        if (ALPM_IS_IPV4(ipt)) {
                            alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
                        } else {
                            alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
                        }
                        ALPM_ERR(("VRF:%d IPT:%s %s: RTE idx:%d.%d Sublen:%d"
                                  " fmt:%d Dest:%d IP:%s\n ==> %s\n",
                                  vrf_id, alpm_util_ipt_str[ipt],
                                  ALPM_TBL_NAME(bkt_tbl),
                                  idx1, eid, new_len, fmt,
                                  adata1.defip_ecmp_index, ip_buf, err_msg));
                    }
                }
                continue;
            }

            /* prevent going further down if somehow last level
               is not Route level due to internal failure */
            if ((ACB_IDX(acb) + 1) >= ACB_CNT(u)) {
                return BCM_E_INTERNAL;
            }

            /* get ALPM Data entry ready for next level */
            alpm_util_ent_data_get(u, vrf_id, ipt, acb, ftmp, fmt, fent1);
            ALPM_IER(alpm_cb_sanity_process(u, vrf_id, ipt, ACB_DWN(u, acb),
                                            fmt, fent1, new_key, new_len,
                                            idx1, eid, &adata1, sanity));
        } /* for eid */
    } /* for bnk */

    if (hw_bkt_pfx_cnt != pvt_node_pfx_cnt) {
        sanity->error++;
        ALPM_ERR(("%s: Pivot idx:%d.%d pfx count(%d) does not match to hw bkt(bkt=%d,dw=%d) pfx count(%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, pvt_node_pfx_cnt,
                  PVT_BKT_IDX(pvt_node), ACB_BKT_DW(acb), hw_bkt_pfx_cnt));
    }

    /* each pivot must have at least one valid bucket entry except
     * default pivot */
    if (bkt_empty && key_len != 0) {
        sanity->error++;
        ALPM_ERR(("%s: Pivot idx:%d.%d with empty bucket (or route).\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_alpm_sanity_check
 * Purpose:
 *      Sanity check for ALPM
 * Parameters:
 *      u        - (In)Device unit
 *      mem      - (In)TCAM memory (not in used)
 *      index    - (In)Memory index (not in used)
 *      check_sw - (In)check_sw = 0 for prefix Hw routing check
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_sanity_check(int u, int app, soc_mem_t mem, int index, int check_sw)
{
    int ipv6, key_mode;
    int idx, sub_idx;
    int vrf_id;
    int idx_end;
    int step_size;
    uint32 lpm_entry[ALPM_MEM_ENT_MAX];
    uint32 fent[ALPM_MEM_ENT_MAX];
    _alpm_cb_t *acb;
    _alpm_bkt_adata_t adata0;
    int pk, pkm, use_alpm;
    uint32 key[4];
    int key_pfx_len;
    int i, alloc_sz, pid, tot_error = 0;
    _alpm_sanity_t sanity[ALPM_BKT_PID_CNT];
    _alpm_sanity_t *psanity;
    int rv = BCM_E_NONE;
    int rv_tcam;
    _alpm_bkt_pool_conf_t *bp_conf;
    int pid_count;

    ALPMC_INIT_CHECK(u);

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);

        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                continue; /* Combined Mode uses only PID=0 for sanity counters */
            }

            psanity = &sanity[pid];
            alloc_sz = BPC_BNK_CNT(bp_conf) * sizeof(int);
            ALPM_ALLOC_EG(psanity->bktptr[i], alloc_sz, "bktptr");
            /* initialized to -1 to distinguish index 0 */
            sal_memset(psanity->bktptr[i], 0xff, alloc_sz);
            psanity->error = 0;
        }
    }

    L3_LOCK(u);
    acb = ACB_TOP(u, app); /* from top to bottom */
    for (pk = 0; pk < ALPM_PKM_CNT; pk++) {
        if (ALPM_TCAM_TBL_SKIP(u, pk)) {
            continue; /* skip duplicated or invalid TCAM table */
        }

        /* LOG_CLI(("\n- Processing %s ...\n", TCAM_TBL_NAME(acb, pk))); */
        idx_end = tcam_table_size(u, app, pk);
        /* Walk all TCAM entries */
        for (idx = 0; idx < idx_end; idx++) {
            ALPM_IEG(tcam_entry_read_no_cache(u, app, pk, lpm_entry, idx, idx));

            for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
                rv_tcam = tcam_valid_entry_mode_get(u, app, pk, lpm_entry,
                              &step_size, &pkm, &ipv6, &key_mode, sub_idx);
                if (BCM_FAILURE(rv_tcam)) {
                    if (rv_tcam == BCM_E_INTERNAL) {
                        ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type"
                                  " (IPv6):%d in idx:%d.%d\n",
                                  TCAM_TBL_NAME(acb, pk), key_mode,
                                  ipv6, idx, sub_idx));
                    }
                    continue;
                }
                tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &vrf_id);

                /* process pre-pivor ASSOC_DATA */
                tcam_entry_adata_get(u, app, pkm, lpm_entry, sub_idx, &adata0);
                use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
                           ALPM_VRF_ID_HAS_BKT(u, vrf_id);
                if (!use_alpm) {
                    continue;
                }

                /* Get Pre-pivot key IP_ADDR and prefix length (mask) */
                sal_memset(key, 0, sizeof(key));
                key_pfx_len = 0;
                tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, key);
                tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &key_pfx_len);

                /* process ALPM_DATA level recursively from top to bottom */
                tcam_entry_bdata_get(u, app, pkm, lpm_entry, sub_idx, fent);

                /* Combined Mode uses only PID=0 for sanity counters */
                pid = ACB_BKT_VRF_PID(acb, vrf_id);
                psanity = &sanity[pid];
                ALPM_IEG(alpm_cb_sanity_process(u, vrf_id, pkm, acb, 0, fent,
                                            key, key_pfx_len, idx, sub_idx,
                                            &adata0, psanity));
            } /* for sub_idx */
        } /* for idx */
    } /* for pk */

    if (check_sw == 0) {
        rv = alpm_pfx_hw_route_sanity(u, app, &tot_error);
    } else {
        rv = alpm_pfx_sw_route_sanity(u, app, &tot_error);
    }

bad:
    L3_UNLOCK(u);
    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);
        pid_count = alpm_pid_count(app);
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                continue; /* Combined Mode uses only PID=0 for sanity counters */
            }

            psanity = &sanity[pid];
            if (psanity->bktptr[i]) {
                alpm_util_free(psanity->bktptr[i]);
                psanity->bktptr[i] = NULL;
            }
            tot_error += psanity->error;
        }
    }

    if (rv == BCM_E_NONE) {
        rv = (tot_error ? BCM_E_FAIL : BCM_E_NONE);
    }

    return rv;
}

/*
 * Function:
 *      alpm_cb_bm_lookup
 * Purpose:
 *      ALPM bucket best match lookup per CB by Hw (recursive from top to bottom).
 * Parameters:
 *      u        - (In)Device unit
 *      vrf_id   - (In)VRF ID
 *      pkm      - (In)PKM
 *      acb      - (In)ALPM control block pointer
 *      fent     - (In)ALPM_DATA entry
 *      pvtfmt   - (In)Pivot bank format (ignored for TCAM)
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      idx      - (In)Memory Index
 *      sub_idx  - (In)Memory sub_idx (for half-entry) or ent index
 *      adata    - (In)Assoc Data structure pointer
 *      lpm_cfg  - (Out)BPM defip cfg info found
 *      hit_res  - (Out)Lookup result in the bucket
 *      sw_key   - (In)Key to lookup
 *      sw_len   - (In)Key length to lookup
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_cb_bm_lookup(int u, int vrf_id, int pkm, _alpm_cb_t *acb, void *fent,
                  int pvtfmt, uint32 *key, int key_len, int idx, int sub_idx,
                  _alpm_bkt_adata_t *adata, _bcm_defip_cfg_t *lpm_cfg,
                  _alpm_lkup_res_t *hit_res, uint32 *sw_key, int sw_len)
{
    uint32 bkt_entry[ALPM_MEM_ENT_MAX];
    uint32 ftmp[ALPM_MEM_ENT_MAX];
    uint32 hw_mask[4];
    uint32 hw_key[4];
    uint32 fmt, valid;
    int i, sublen, hw_len, key_ent_cnt;
    int idx1, eid, bnk;
    char ip_buf[IP6ADDR_STR_LEN];
    _bcm_defip_cfg_t lpm_cfg_tmp;
    _alpm_bkt_adata_t adata_tmp;
    _alpm_bkt_adata_t adata1;
    int def_miss = TRUE;
    _alpm_lkup_res_t hit_res1 = ALPM_MISS; /* lookup result from next CB */
    int hit_val;
    /* Used for keeping Longest Matched entry in Bucket */
    int    bm_len = -1;
    int    bm_idx = 0;
    int    bm_ent = 0;
    uint32 bm_fmt = 0;
    uint32 bm_key[4];
    uint32 bm_fent[ALPM_MEM_ENT_MAX];
    _alpm_bkt_info_t bkt_info;
    _alpm_tbl_t bkt_tbl;
    _alpm_tbl_t pvt_tbl;
    int ipt = ALPM_PKM2IPT(pkm);

    if (ACB_IDX(acb) >= ACB_CNT(u)) {
        return BCM_E_INTERNAL; /* wrong ALPM control level called */
    }

    bkt_tbl = ACB_BKT_TBL(acb, vrf_id);
    pvt_tbl = (ACB_HAS_TCAM(acb) ? acb->pvt_tbl[pkm] :
                ACB_BKT_TBL(ACB_UPR(u, acb), vrf_id));

    /**** Pivot level ****/
    alpm_util_bkt_info_get(u, vrf_id, ipt, pkm, acb, fent, sub_idx,
                           &bkt_info, &sublen, &def_miss);

    /* show pivot hit info */
    alpm_util_cfg_construct(u, vrf_id, ipt, key, key_len, adata, &lpm_cfg_tmp);
    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg_tmp.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg_tmp.defip_ip6_addr);
    }

    /*if (ACB_HAS_TCAM(acb)) {
        ALPM_VERB(("Hit pivot in %s: idx:%d.%d VRF:%d%s IP:%s Sublen:%d\n",
                   ALPM_TBL_NAME(pvt_tbl), idx, sub_idx,
                   vrf_id, (vrf_id == ALPM_VRF_ID_GLO(u)? "(Global low)" : ""),
                   ip_buf, key_len));
    } else {
        ALPM_VERB(("Hit pivot in %s: idx:%d.%d fmt:%d IP:%s Sublen:%d\n",
                   ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, pvtfmt, ip_buf, key_len));
    }*/

    /* kshift must be same as previous key_len */
    if (!ACB_BKT_FIXED_FMT(acb, 1) && sublen != key_len) {
        ALPM_ERR(("%s: ALPM_DATA invalid kshift:%d vs pivot len:%d"
                  " idx:%d.%d (%d)\n", ALPM_TBL_NAME(pvt_tbl), sublen,
                  key_len, idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }
    if (BI_BKT_IDX(&bkt_info) >= ACB_BKT_CNT(acb)) {
        ALPM_ERR(("%s: ALPM_DATA invalid bkt_ptr idx:%d.%d (%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }

    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        fmt = bkt_info.bnk_fmt[bnk];
        if ((fmt == 0) || (fmt > ACB_FMT_CNT(acb, vrf_id))) {
            if (fmt > ACB_FMT_CNT(acb, vrf_id)) {
                ALPM_ERR(("%s: ALPM_DATA format wrong idx:%d.%d"
                          " (bank=%d fmt=%d).\n", ALPM_TBL_NAME(pvt_tbl),
                          idx, sub_idx, bnk, fmt));
            }
            continue;
        }

        /**** Bucket Level ****/
        idx1 = ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(&bkt_info), BI_BKT_IDX(&bkt_info), bnk);
        ALPM_IER(alpm_bkt_entry_read(u, ACB_BKT_TBL(acb, vrf_id),
                                     acb, bkt_entry, idx1));

        for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fmt); eid++) {
            uint32 key2[4] = {0};
            int len2 = 0;
            int sub_bkt_ptr = 0;

            /* process bucket entry Prefix */
            alpm_util_ent_ent_get(u, vrf_id, acb, bkt_entry, fmt, eid, ftmp);
            alpm_util_bkt_pfx_get(u, vrf_id, acb, ftmp, fmt, key2, &len2, &valid);
            alpm_util_bkt_adata_get(u, vrf_id, ipt, acb, ftmp, fmt, &adata_tmp, &sub_bkt_ptr);

            if (!valid || BI_SUB_BKT_IDX(&bkt_info) != sub_bkt_ptr) {
                continue;
            }

            if (ACB_BKT_FIXED_FMT(acb, 1)) {
                hw_len = len2;
                sal_memcpy(hw_key, key2, sizeof(hw_key));
            } else {
                alpm_util_pfx_cat(u, ipt, key, sublen, key2, len2, hw_key, &hw_len);
            }

            if (hw_len > sw_len) { /* invalid or not match */
                continue;
            }
            alpm_util_len_to_mask(ipt, hw_len, hw_mask);

            /* new_len <= sw_len: compare all hw_masked key */
            key_ent_cnt = ALPM_KEY_ENT_CNT(ipt);
            for (i = 0; i < key_ent_cnt; i++) {
                if ((sw_key[i] & hw_mask[i]) != (hw_key[i] & hw_mask[i])) {
                    break;
                }
            }
            /* Not match */
            if (i < key_ent_cnt) {
                continue;
            }

            /* Found a match, keep longest matched route */
            if (hw_len > bm_len) {
                bm_len = hw_len;
                bm_fmt = fmt;
                bm_idx = idx1; /* ALPM bucket mem index */
                bm_ent = eid;
                sal_memcpy(bm_key, hw_key, sizeof(hw_key));
                sal_memcpy(bm_fent, ftmp, sizeof(ftmp));
                sal_memcpy(&adata1, &adata_tmp, sizeof(adata_tmp));
            }
        } /* for ent */
    } /* for bnk */

    /* Hit in bucket */
    if (bm_len != -1) {
        /* end recursion down if reach route level (last level) */
        if (ACB_HAS_RTE(acb, vrf_id)) {
            *hit_res = ALPM_HIT;
            alpm_util_cfg_construct(u, vrf_id, ipt, bm_key, bm_len,
                                    &adata1, &lpm_cfg_tmp);
            if (ALPM_IS_IPV4(ipt)) {
                alpm_util_fmt_ipaddr(ip_buf, lpm_cfg_tmp.defip_ip_addr);
            } else {
                alpm_util_fmt_ip6addr(ip_buf, lpm_cfg_tmp.defip_ip6_addr);
            }

            /*ALPM_VERB(("Hit route in %s idx:%d.%d fmt:%d IP:%s Sublen:%d\n",
                       ALPM_TBL_NAME(bkt_tbl), bm_idx, bm_ent,
                       bm_fmt, ip_buf, bm_len));*/
        } else { /* go down CB */
            /* prevent going further down if somehow last level
               is not Route level due to internal failure */
            if ((ACB_IDX(acb) + 1) >= ACB_CNT(u)) {
                return BCM_E_INTERNAL;
            }

            /* get ALPM Data entry ftmp for next level using best match saved */
            alpm_util_ent_data_get(u, vrf_id, ipt, acb, bm_fent, bm_fmt, ftmp);
            ALPM_IER(alpm_cb_bm_lookup(u, vrf_id, ipt, ACB_DWN(u, acb),
                                       ftmp, bm_fmt, bm_key, bm_len, bm_idx, bm_ent,
                                       &adata1, lpm_cfg, &hit_res1, sw_key, sw_len));

            if (hit_res1 == ALPM_HIT) { /* Hit from next CB */
                *hit_res = ALPM_HIT;
                return BCM_E_NONE;
            } else if ((hit_res1 == ALPM_MISS) && (def_miss == TRUE)) {
                *hit_res = ALPM_MISS;
                ALPM_VERB(("Miss in %s idx:%d.%d\n",
                           ALPM_TBL_NAME(bkt_tbl), bm_idx, bm_ent));
                return BCM_E_NONE;
            }
            /* hit_res1 == ALPM_MISS_USE_AD or def_miss == FALSE */
            *hit_res = ALPM_MISS_USE_AD;
            ALPM_VERB(("Miss but use Associated data in %s idx:%d.%d\n",
                       ALPM_TBL_NAME(bkt_tbl), bm_idx, bm_ent));
        }

        /* Reached route level or hit_res1 == ALPM_MISS_USE_AD from next CB */
        /* Get best match Assoc_Data and prepare returned lpm_cfg info*/
        alpm_util_cfg_construct(u, vrf_id, ipt, bm_key, bm_len, &adata1, lpm_cfg);
        lpm_cfg->defip_index =
            alpm_util_ent_phy_idx_get(u, acb, vrf_id, bm_idx); /* ALPM mem index */

        /* Get HIT bit from best matched entry */
        if (!ALPM_HIT_SKIP(u)) {
            ALPM_HIT_LOCK(u);
            hit_val = alpm_rte_hit_get(u, acb->app, vrf_id, bkt_tbl, bm_idx, bm_ent);
            lpm_cfg->defip_flags |= (hit_val > 0 ? BCM_L3_HIT : 0);
            ALPM_HIT_UNLOCK(u);
        }

        return BCM_E_NONE;
    }

    /* Missed in this CB */
    if (def_miss == TRUE) {
        *hit_res = ALPM_MISS; /* Miss at all */
    } else { /* Default Miss = 0 */
        *hit_res = ALPM_MISS_USE_AD; /* Miss but upper CB should use Assoc Data */
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      alpm_find_best_match
 * Purpose:
 *      Get best prefix match entry from DEFIP table (both IPv4 or IPv6).
 *      It's an indenpendent routine, to simalute HW process for incoming
 *      packets. Considering possible mismatch between SW and HW, we need
 *      to go through hardware for lookup regardless of software state.
 * Parameters:
 *      u           - (IN)Device unit number
 *      vrf_id      - (IN)VRF ID to lookup
 *      pkm         - (IN)Packing mode to lookup
 *      sw_key      - (IN)Key to lookup
 *      sw_len      - (IN)Key length to lookup
 *      lpm_cfg     - (OUT)BPM defip cfg info found
 * Returns:
 *      BCM_E_XXX
 */
int
alpm_find_best_match(int u, int vrf_id, int pkm, uint32 *sw_key,
                     int sw_len, _bcm_defip_cfg_t *lpm_cfg)
{
    int i, ipv6, hw_pkm, step_size, key_ent_cnt;
    int idx, sub_idx, idx_min, idx_cnt;
    int idx_end;
    int hw_len, hw_vrf_id;
    int use_alpm, key_mode;
    _alpm_lkup_res_t hit_res = ALPM_MISS;
    uint32 hw_mask[4];
    uint32 hw_key[4];
    uint32 fent[ALPM_MEM_ENT_MAX];
    _alpm_cb_t *acb;
    uint32 lpm_entry[ALPM_MEM_ENT_MAX];
    int do_urpf = 0; /* Find uRPF region */
    int rv_tcam;
    int ipt = ALPM_PKM2IPT(pkm);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    if (!bcm_esw_alpm_inited(u)) {
        return BCM_E_INIT;
    }
    if (!SOC_URPF_STATUS_GET(u) && do_urpf) {
        return BCM_E_PARAM;
    }

    acb = ACB_TOP(u, app); /* from top to bottom */

    idx_min = 0;
    idx_cnt = tcam_table_size(u, app, pkm);
    /* in Parallel or TCAM/ALPM mixed mode search Global High from second half */
    if (ALPM_TCAM_ZONED(u, app)) {
        idx_cnt >>= 1;
        idx_min += idx_cnt;
    }
    if (do_urpf) {
        idx_min += tcam_table_size(u, app, pkm);
    }
    idx_end = idx_min + idx_cnt;

    /* 1) Scanning for Global High routes first */
    for (idx = idx_min; idx < idx_end; idx++) {
        ALPM_IER(tcam_entry_read(u, app, pkm, lpm_entry, idx, idx));

        for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
            rv_tcam = tcam_valid_entry_mode_get(u, app, pkm, lpm_entry,
                          &step_size, &hw_pkm, &ipv6, &key_mode, sub_idx);
            if (BCM_FAILURE(rv_tcam)) {
                if (rv_tcam == BCM_E_INTERNAL) {
                    ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type"
                              " (IPv6):%d in idx:%d.%d\n",
                              TCAM_TBL_NAME(acb, pkm), key_mode,
                              ipv6, idx, sub_idx));
                }
                continue;
            }

            if (pkm != hw_pkm) {
                continue;
            }

            tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &hw_vrf_id);
            if (hw_vrf_id != ALPM_VRF_ID_GHI(u)) { /* only Global Hi */
                continue;
            }

            sal_memset(hw_key, 0, sizeof(hw_key));
            hw_len = 0;
            tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, hw_key);
            tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &hw_len);
            if (hw_len > sw_len) { /* not match */
                continue;
            }
            alpm_util_len_to_mask(ipt, hw_len, hw_mask);

            /* hw_len <= sw_len: compare all hw_masked key */
            key_ent_cnt = ALPM_KEY_ENT_CNT(ipt);
            for (i = 0; i < key_ent_cnt; i++) {
                if ((sw_key[i] & hw_mask[i]) != (hw_key[i] & hw_mask[i])) {
                    break;
                }
            }
            /* Not match */
            if (i < key_ent_cnt) {
                continue;
            }

            /* Found all key matched (must be longest prefix due to order in TCAM) */
            /* prepare returned lpm_cfg info */
            tcam_entry_to_cfg(u, pkm, lpm_entry, sub_idx, lpm_cfg);
            alpm_util_key_to_cfg(u, ipt, hw_key, lpm_cfg);
            lpm_cfg->defip_sub_len = hw_len;
            lpm_cfg->defip_vrf = ALPM_VRF_ID_TO_VRF(u, hw_vrf_id);
            lpm_cfg->defip_flags |= (ipv6 ? BCM_L3_IP6 : 0);
            lpm_cfg->defip_index = idx; /* TCAM table index */

            ALPM_VERB(("Hit Global High route in %s idx:%d.%d\n",
                       TCAM_TBL_NAME(acb, pkm), idx, sub_idx));
            return BCM_E_NONE;
        }
    }

    /* 2) Scanning for entire TCAM table sequentially
       (Private VRF route has precedence over Global Low) */
    idx_min = 0;
    idx_cnt = tcam_table_size(u, app, pkm);
    if (do_urpf) {
        idx_min += tcam_table_size(u, app, pkm);
    }
    idx_end = idx_min + idx_cnt;

    for (idx = idx_min; idx < idx_end; idx++) {
        _alpm_bkt_adata_t adata0;
        ALPM_IER(tcam_entry_read(u, app, pkm, lpm_entry, idx, idx));

        for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
            rv_tcam = tcam_valid_entry_mode_get(u, app, pkm, lpm_entry,
                          &step_size, &hw_pkm, &ipv6, &key_mode, sub_idx);
            if (BCM_FAILURE(rv_tcam)) {
                if (rv_tcam == BCM_E_INTERNAL) {
                    ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type"
                              " (IPv6):%d in idx:%d.%d\n",
                              TCAM_TBL_NAME(acb, pkm), key_mode,
                              ipv6, idx, sub_idx));
                }
                continue;
            }

            if (pkm != hw_pkm) {
                continue;
            }

            tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &hw_vrf_id);
            if ((vrf_id != hw_vrf_id) && (hw_vrf_id < ALPM_VRF_ID_GLO(u))) {
                continue;
            }
            /* valid only when ((vrf_id == hw_vrf_id) ||
                                (hw_vrf_id >= ALPM_VRF_ID_GLO(u))) */
            sal_memset(hw_key, 0, sizeof(hw_key));
            hw_len = 0;
            tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, hw_key);
            tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &hw_len);
            if (hw_len > sw_len) { /* not match */
                continue;
            }
            alpm_util_len_to_mask(ipt, hw_len, hw_mask);

            /* hw_len <= sw_len: compare all hw_masked key */
            key_ent_cnt = ALPM_KEY_ENT_CNT(ipt);
            for (i = 0; i < key_ent_cnt; i++) {
                if ((sw_key[i] & hw_mask[i]) != (hw_key[i] & hw_mask[i])) {
                    break;
                }
            }
            /* Not match */
            if (i < key_ent_cnt) {
                continue;
            }

            /* Hit in TCAM pivot (must be longest prefix due to order in TCAM) */
            tcam_entry_adata_get(u, app, pkm, lpm_entry, sub_idx, &adata0);
            use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
                       ALPM_VRF_ID_HAS_BKT(u, vrf_id);
            if (!use_alpm) {
                /* Global Low Direct route */
                ALPM_VERB(("Hit Global Low route in %s idx:%d.%d\n",
                           TCAM_TBL_NAME(acb, pkm), idx, sub_idx));
            } else {
                /* ALPM bucket lookup from top to bottom */
                tcam_entry_bdata_get(u, app, pkm, lpm_entry, sub_idx, fent);

                alpm_util_cfg_to_key(u, ipt, lpm_cfg, sw_key);
                ALPM_IER(alpm_cb_bm_lookup(u, hw_vrf_id, pkm, acb, fent, 0,
                                           hw_key, hw_len, idx, sub_idx, &adata0,
                                           lpm_cfg, &hit_res, sw_key, sw_len));
                if (hit_res == ALPM_HIT) { /* Hit in bucket */
                    return BCM_E_NONE;
                } else if (hit_res == ALPM_MISS) { /* Miss */
                    ALPM_VERB(("Miss in %s idx:%d.%d\n",
                               TCAM_TBL_NAME(acb, pkm), idx, sub_idx));
                    return(BCM_E_NOT_FOUND);
                } else { /* hit_res == ALPM_MISS_USE_AD */
                    ALPM_VERB(("Miss but use Associated data in %s idx:%d.%d\n",
                               TCAM_TBL_NAME(acb, pkm), idx, sub_idx));
                }
            }

            /* prepare returned lpm_cfg info */
            tcam_entry_to_cfg(u, pkm, lpm_entry, sub_idx, lpm_cfg);
            alpm_util_key_to_cfg(u, ipt, hw_key, lpm_cfg);
            lpm_cfg->defip_sub_len = hw_len;
            lpm_cfg->defip_vrf = ALPM_VRF_ID_TO_VRF(u, hw_vrf_id);
            lpm_cfg->defip_flags |= (ipv6 ? BCM_L3_IP6 : 0);
            lpm_cfg->defip_index = idx; /* TCAM Hw index */

            return BCM_E_NONE;
        }
    }

    /* Private VRF and Global Low Miss */
    ALPM_VERB(("Search miss for given address\n"));

    return(BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      bcm_esw_alpm_find
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      u           - (IN)Device unit number.
 *      lpm_cfg     - (IN/OUT)Key to lookup and found defip information.
 *      nh_ecmp_idx - (OUT)Next hop or ecmp group index of bpm_route
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_find(int u, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int ipt = ALPM_PKM2IPT(pkm);
    uint32 sw_key[4] = {0};
    int sw_len;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }
    alpm_util_ipmask_apply(u, lpm_cfg);

    sw_len = lpm_cfg->defip_sub_len;

    /* Perform hw lookup */
    alpm_util_cfg_to_key(u, ipt, lpm_cfg, sw_key); /* get sw_key[4] */
    ALPM_IER_PRT_EXCEPT(alpm_find_best_match(u, vrf_id, pkm, sw_key, sw_len, lpm_cfg), BCM_E_NOT_FOUND);
    *nh_ecmp_idx = lpm_cfg->defip_ecmp_index;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_alpm_sw_dump
 * Purpose:
 *      dump sw l3 (ALPM).
 * Parameters:
 *      u           - (IN)Device unit number.
 */
void
bcm_esw_alpm_sw_dump(int u)
{
    int app, app_cnt = alpm_app_cnt(u);

    LOG_CLI((BSL_META_U(u, "\n  ALPM Info -\n")));
    for (app = 0; app < app_cnt; app++) {
        bcm_esw_alpm_debug_brief_show(u, app, 0, 0);
        (void)alpm_cb_stat_dump(u, app, 3);
    }
}

static int
alpm_cb_ent_pfx_len_get(int u, _alpm_cb_t *acb,
                        _alpm_pvt_node_t *pvt_node, int ent_idx)
{
    return ALPM_DRV(u)->alpm_ent_pfx_len_get(u, acb, pvt_node, ent_idx);
}

int
alpm_cb_pvt_sanity_cb(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, void *datum)
{
    int a, i, rv = BCM_E_NONE;
    int bkt_node_cnt, bkt_info_cnt;
    int vrf_id, ipt, err = 0;
    int end_bnk, bnkpb, bnk_cnt;
    int pid_count;

    char        *pfx_str = datum;
    alpm_lib_trie_t      *bkt_trie = NULL;
    _alpm_cb_t  *acb_tmp = NULL;
    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_node_t *bkt_node;
    int app = acb->app;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    vrf_id   = PVT_BKT_VRF(pvt_node);
    ipt      = PVT_BKT_IPT(pvt_node);
    bnkpb   = ACB_BNK_PER_BKT(acb, vrf_id);

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_PTR_CHK) {
#define CORRUPT(p)                      \
        (p[-1] != 0xaaaaaaaa ||         \
        p[p[-2]] != 0xbbbbbbbb)

        int pid, pvt_cnt;
        for (a = 0; a < ACB_CNT(u); a++) {
            acb_tmp = ACB(u, a, app);
            pid_count = alpm_pid_count(app);
            for (pid = 0; pid < pid_count; pid++) {
                _alpm_bkt_pool_conf_t *bp_conf;
                uint32 *pvt_ptr;

                if (pid > 0 &&
                    ACB_BKT_POOL(acb_tmp, pid-1) ==
                    ACB_BKT_POOL(acb_tmp, pid)) {
                    continue;
                }

                bp_conf = ACB_BKT_POOL(acb_tmp, pid);
                pvt_cnt = BPC_BNK_CNT(bp_conf);

                for (i = 0; i < pvt_cnt; i++) {
                    pvt_ptr = bp_conf->pvt_ptr[i];
                    if (pvt_ptr == NULL) {
                        continue;
                    }

                    if (CORRUPT(pvt_ptr)) {
                        cli_out("acb %d: pvt_ptr %p in pool %d bkt %d "
                                "bnk %d is corrupted\n",
                                a, pvt_ptr, pid, ALPM_IDX_TO_BKT(acb_tmp, i),
                                ALPM_IDX_TO_BNK(acb_tmp, i));
                        err++;
                        goto bad;
                    }

                    pvt_ptr =
                        (uint32 *)PVT_BKT_TRIE((_alpm_pvt_node_t *)pvt_ptr);
                    if (CORRUPT(pvt_ptr)) {
                        cli_out("acb %d: pvt_bkt_trie %p in pool %d bkt %d "
                                "bnk %d is corrupted\n",
                                a, pvt_ptr, pid, ALPM_IDX_TO_BKT(acb_tmp, i),
                                ALPM_IDX_TO_BNK(acb_tmp, i));
                        err++;
                        goto bad;
                    }
                }
            }
        }
    }

    /* Check Global bank usage and pvt_ptr */
    if ((sal_strcmp(pfx_str, "CBInsert") == 0 ||
         sal_strcmp(pfx_str, "CBDelete") == 0 ||
         sal_strcmp(pfx_str, "SanityAll") == 0) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_BNK_CONF_NULL_PVT) {
        int tab_idx;
        SHR_BITDCL *bnk_bmp;

        /* Private VRFs */
        bnk_bmp = ACB_BNK_BMP(acb, vrf_id, ipt);
        bnk_cnt = ACB_BNK_CNT(acb, vrf_id);

        for (i = 0; i < bnk_cnt; i++) {
            if (SHR_BITGET(bnk_bmp, i)) {
                tab_idx = ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i/bnkpb, i%bnkpb);
                /* If bank is occupied but pvt_ptr is not set, report error
                 * Skip examing bkt 0
                 */
                if ((i/bnkpb) != 0 &&
                    ACB_PVT_PTR(acb, pvt_node, tab_idx) == NULL) {
                    cli_out("\n[%s]PVT-SANITY(CB%d): bkt %d bnk %d was used "
                            "by NULL pvt_ptr\n", pfx_str, acb->acb_idx,
                            i/bnkpb, i%bnkpb);
                    err++;
                    goto bad;
                }
            }
        }
    }

    /* Check Global bank usage and pvt_ptr */
    if ((sal_strcmp(pfx_str, "CBInsert") == 0 ||
         sal_strcmp(pfx_str, "CBDelete") == 0 ||
         sal_strcmp(pfx_str, "SanityAll") == 0) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_BNK_CONF) {
        SHR_BITDCL *bnk_bmp;

        /* Private VRFs */
        bnk_bmp = ACB_BNK_BMP(acb, vrf_id, ipt);

        for (i = 0; i < bnkpb; i++) {
            if (BI_BNK_IS_USED(bkt_info, i)) {
                int gbl_bnk =
                    BI_BKT_IDX(bkt_info) * ACB_BNK_PER_BKT(acb, vrf_id) + i;
                if (BI_ROFS(bkt_info) > i) {
                    gbl_bnk += ACB_BNK_PER_BKT(acb, vrf_id);
                }

                if (!SHR_BITGET(bnk_bmp, gbl_bnk)) {
                    cli_out("\n[%s]PVT-SANITY(CB%d): bkt %d bnk %d was used "
                            "but global bit %d is not SET\n", pfx_str, acb->acb_idx,
                            BI_BKT_IDX(bkt_info), i, gbl_bnk);
                    err++;
                    goto bad;
                }
            }
        }
    }

    /* Check if used banks are continuous */
    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
        end_bnk = -1;
        for (i = PVT_ROFS(pvt_node); i < PVT_ROFS(pvt_node) + bnkpb; i++) {
            if (bkt_info->bnk_fmt[i % bnkpb] == 0) {
                end_bnk = i;
            } else if (end_bnk != -1) {
                /* Once end_bnk is settled, the rest bnks FMT should be 0 */
                cli_out("[%s]PVT-SANITY(CB%d): Used banks are not cont. rofs %d: ",
                        pfx_str, acb->acb_idx, PVT_ROFS(pvt_node));
                for (i = 0; i < bnkpb; i++) {
                    if (PVT_ROFS(pvt_node) == i) {
                        cli_out("^");
                    }
                    cli_out("%d ", bkt_info->bnk_fmt[i]);
                }
                cli_out("\n");
                err++;
                goto bad;
            }
        }
    }

    /* trie-tree count & vet_bmp count,
     * SHOULD NOT be used in the middle of process, e.g. bucket split
     */
    if ((sal_strcmp(pfx_str, "CBInsert") == 0 ||
         sal_strcmp(pfx_str, "CBDelete") == 0 ||
         sal_strcmp(pfx_str, "SanityAll") == 0 ||
         sal_strcmp(pfx_str, "SplitOpvt") == 0 ||
         sal_strcmp(pfx_str, "SplitNpvt") == 0) &&
        bkt_trie->trie &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_COUNT) {
        bkt_node_cnt = bkt_trie->trie->count;
        bkt_info_cnt = 0;
        for (i = 0; i < ALPM_BPB_MAX; i++) {
            uint32 vet_bmp;
            if (bkt_info->bnk_fmt[i] == 0) {
                continue;
            }

            vet_bmp = bkt_info->vet_bmp[i];
            bkt_info_cnt += _shr_popcount(vet_bmp);
        }

        if (bkt_node_cnt != bkt_info_cnt) {
            cli_out("\n[%s]PVT-SANITY(CB%d): entry cnt mismatch in trie %d, bkt_info %d\n",
                    pfx_str, acb->acb_idx, bkt_node_cnt, bkt_info_cnt);
            err++;
            goto bad;
        }
    }

    /* bkt_info & ent_idx, tab_idx & pvt_node pointer
     * SW prefix length & HW prefix length
     *
     * Contruct:
     * - ALPM_IDX_MAKE
     * - ALPM_TAB_IDX_GET(ent_idx)
     *
     * Resolve:
     * - ALPM_IDX_TO_ENT
     * - ALPM_IDX_TO_BNK
     * - ALPM_IDX_TO_BKT
     */
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = alpm_lib_trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       trieTraverseOrderIn, TRUE);
    ALPM_IEG(rv);

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        uint32 ent, bnk, bkt;
        uint32 tab_idx;
        uint32 vet_bmp;
        int sw_len, hw_len;

        bkt_node = pfx_arr->pfx[i];

        /* SW Prefix Length & HW Prefix Length check */
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_LEN) {
            sw_len = bkt_node->key_len;
            hw_len = alpm_cb_ent_pfx_len_get(u, acb, pvt_node,
                                             bkt_node->ent_idx);
            if (!ACB_BKT_FIXED_FMT(acb, 1)) {
                hw_len += PVT_KEY_LEN(pvt_node);
            }
            if (sw_len != hw_len) {
                cli_out("\n[%s]PVT-SANITY(CB%d): Prefix Length mismatch sw %d "
                        "hw %d ent_idx 0x%x\n", pfx_str, acb->acb_idx, sw_len, hw_len,
                        bkt_node->ent_idx);
                err++;
                goto bad;
            }
        }

        ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
        bnk = ALPM_IDX_TO_BNK(acb, bkt_node->ent_idx);
        bkt = ALPM_IDX_TO_BKT(acb, bkt_node->ent_idx);
        if (bnk < PVT_ROFS(pvt_node)) {
            bkt -= 1;
        }

        /* Bucket mismatch */
        if (BI_BKT_IDX(bkt_info) != bkt) {
            cli_out("\n[%s]PVT-SANITY(CB%d): Bucket idx mismatch in bkt-node %d, "
                    "pvt_node %d\n", pfx_str, acb->acb_idx, bkt, BI_BKT_IDX(bkt_info));
            err++;
            goto bad;
        }

        /* Ent, bnk mismatch */
        vet_bmp = bkt_info->vet_bmp[bnk];
        if (bkt_info->bnk_fmt[bnk] == 0 ||
            !SHR_BITGET(&vet_bmp, ent)) {
            cli_out("\n[%s]PVT-SANITY(CB%d): (ent,bnk) mismatch in trie-node (%d,%d)"
                    ", but related valid in bkt_info is (%d, bnk%d-fmt%d)\n",
                    pfx_str, acb->acb_idx, ent, bnk,
                    SHR_BITGET(&vet_bmp, ent),
                    bnk, bkt_info->bnk_fmt[bnk]);
            err++;
            goto bad;
        }

        /* tab_idx and pvt_node pointer */
        tab_idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
        if (tab_idx >= ACB_BNK_CNT(acb, PVT_BKT_VRF(pvt_node))) {
            cli_out("\n[%s]PVT-SANITY(CB%d): Invalid tab_idx value %d "
                    "(ent %d, bnk %d, bkt %d)\n", pfx_str, acb->acb_idx,
                    tab_idx, ent, bnk, bkt);
            err++;
            goto bad;
        } else if (ACB_PVT_PTR(acb, pvt_node, tab_idx) != pvt_node) {
            _alpm_pvt_node_t *acb_pvt_node;
            acb_pvt_node = (_alpm_pvt_node_t *)ACB_PVT_PTR(acb, pvt_node, tab_idx);

            cli_out("\n[%s]PVT-SANITY(CB%d): pvt_ptr of tab_idx %d mismatch "
                    "in acb %p, trie-tree %p\n", pfx_str, acb->acb_idx, tab_idx,
                    acb_pvt_node, pvt_node);
            if (acb_pvt_node != NULL) {
                cli_out("\t- acb %p key_len %d, bkt_info(rofs %d, bkt %d)\n",
                        acb_pvt_node, PVT_KEY_LEN(acb_pvt_node),
                        PVT_ROFS(acb_pvt_node),
                        PVT_BKT_IDX(acb_pvt_node));
                cli_out("\t-trie %p key_len %d, bkt_info(rofs %d, bkt %d)\n",
                        pvt_node, PVT_KEY_LEN(pvt_node),
                        PVT_ROFS(pvt_node),
                        PVT_BKT_IDX(pvt_node));
            }
            err++;
            goto bad;
        }
    }

bad:
    if (err > 0) {
        rv = BCM_E_FAIL;
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    return rv;
}

/*
 * Check software status
 *
 *  - trie-tree count & vet_bmp count
 *  - bkt_info & ent_idx
 */
int
alpm_cb_sanity(int u, int app, int acb_bmp, int vrf_id, int ipt)
{
    int rv = BCM_E_NONE;
    int acb_idx;
    _alpm_cb_t *acb = NULL;

    if (!(ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY)) {
        return rv;
    }

    for (acb_idx = 0; acb_idx < ACB_CNT(u); acb_idx++) {
        if (!(acb_bmp & (1 << acb_idx))) {
            continue;
        }

        acb = ACB(u, acb_idx, app);
        if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
            continue;
        }

        rv = bcm_esw_alpm_pvt_traverse(u, app, acb->acb_idx, vrf_id, ipt,
                                       alpm_cb_pvt_sanity_cb, "SanityAll");
        BCM_IF_ERROR_RETURN(rv);
    }
    return rv;
}

/* ALPM data save */
int
bcm_esw_alpm_data_save(int u, bcm_esw_alpm_data_cb write_cb, void *fp)
{
    int rv = BCM_E_UNAVAIL;

    L3_LOCK(u);
    if (ALPM_DRV(u)->alpm_data_op != NULL) {
        rv = ALPM_DRV(u)->alpm_data_op(u, APP0, 0, write_cb, fp);
    }
    L3_UNLOCK(u);

    return rv;
}

STATIC int
_alpm_data_restore_trv_cb(int unit, void *pattern,
                          void *data1, void *data2, int *cmp_result)
{
    _bcm_defip_cfg_t *lpm_cfg = (_bcm_defip_cfg_t *)data1;

    /* Update defip book-keeping info */
    if ((lpm_cfg->defip_flags & BCM_L3_IP6) == BCM_L3_IP6) {
        BCM_XGS3_L3_DEFIP_IP6_CNT(unit)++;
    } else {
        BCM_XGS3_L3_DEFIP_IP4_CNT(unit)++;
    }
    return BCM_E_NONE;
}

/* ALPM data restore */
int
bcm_esw_alpm_data_restore(int u, bcm_esw_alpm_data_cb read_cb, void *fp)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_l3_trvrs_data_t trv_data;

    sal_memset(&trv_data, 0, sizeof(_bcm_l3_trvrs_data_t));

    trv_data.op_cb = _alpm_data_restore_trv_cb;

    L3_LOCK(u);
    if (ALPM_DRV(u)->alpm_data_op != NULL) {
        rv = ALPM_DRV(u)->alpm_data_op(u, APP0, 1, read_cb, fp);
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_alpm_warmboot_walk(u, APP0, &trv_data);
    }
    L3_UNLOCK(u);

    return rv;
}

#else
typedef int bcm_esw_alpm2_alpm_ts_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
