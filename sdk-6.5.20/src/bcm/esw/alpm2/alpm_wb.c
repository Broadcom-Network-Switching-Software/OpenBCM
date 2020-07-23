/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    alpm_wb.c
 * Purpose: ALPM warmboot and update match
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

typedef struct alpm_def_rte_info_s {
    int vrf_id;
    int ipt;
    int bkt_idx;
    _alpm_bkt_adata_t adata;
} alpm_def_rte_info_t;

typedef struct _alpm_pfx_update_match_s {
    int unit;
    int vrf_id;
    int ipt;
    int *def_rte_cnt;
    int app;
    alpm_def_rte_info_t *def_rte;
    _bcm_l3_trvrs_data_t *trv_data;
} _alpm_pfx_update_match_t;

extern int tcam_wb_reinit(int u, int app, int vrf_id, int pkm, int idx, int key_len, int mc);
extern int tcam_wb_reinit_done(int u, int app);
extern int alpm_vrf_deinit(int u, int app, int vrf_id, int ipt);

STATIC int
alpm_pvt_bpm_len_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *user_data)
{
    alpm_lib_trie_t *pfx_trie = (alpm_lib_trie_t *)user_data;
    _alpm_pfx_node_t *pfx_node = NULL;
    int rv = BCM_E_NONE;

    /* Find LPM for this pivot key */
    rv = alpm_lib_trie_find_lpm(pfx_trie, pvt_node->key,
                       pvt_node->key_len, (alpm_lib_trie_node_t **)&pfx_node);
    if (rv == BCM_E_NONE) {
        PVT_BKT_DEF(pvt_node) = pfx_node->bkt_ptr;
        PVT_BPM_LEN(pvt_node) = pfx_node->key_len; /* this is pvt bpm_len */
        ALPM_INFO(("alpm_lib_trie_find_lpm() OK rv:%d key:0x%08x key_len:%d bpm_len:%d\n",
                rv, pvt_node->key[1], pvt_node->key_len, PVT_BPM_LEN(pvt_node)));
    } else {
        ALPM_ERR(("alpm_lib_trie_find_lpm() Failed rv:%d key:0x%08x key_len:%d v6_key:%d\n",
                  rv, pvt_node->key[1], pvt_node->key_len, pfx_trie->v6_key));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      alpm_pivot_bpm_len_update
 * Purpose:
 *      Update pivot bpm_length after prefix trie is obtained from warmboot.
 * Parameters:
 *      u        - Device unit
 * Returns:
 *      void
 */
STATIC void
alpm_pivot_bpm_len_update(int u, int app)
{
    int         i, vrf_id, ipt;
    _alpm_cb_t  *acb;
    alpm_lib_trie_t *pfx_trie;

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);
        for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_MAX(u); vrf_id++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    continue;
                }
                pfx_trie = ALPM_VRF_TRIE(u, app, vrf_id, ipt);
                /* For those entries which doesn't use alpm bucket
                 * e.g.: Global Low in ALPM_TCAM mode */
                if (pfx_trie == NULL) {
                    continue;
                }

                ALPM_INFO(("LC(%d) VRF:%d IPT:%s\n",
                           i, vrf_id, alpm_util_ipt_str[ipt]));
                (void)bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                                     alpm_pvt_bpm_len_cb, (void *)pfx_trie);
            }
        }
    }
}

/*
 * Function:
 *      alpm_wb_vrf_init
 * Purpose:
 *      ALPM VRF pvt and pfx trie init from warmboot.
 *      Add a VRF default route when a L3 VRF is added.
 *      Adds a 0.0 entry into VRF.
 * Parameters:
 *      u        - Device unit
 *      acb      - ALPM control block pointer
 *      vrf_id   - VRF ID
 *      pkm      - Packing mode (32B, 64B, 128)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_wb_vrf_init(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int     rv = BCM_E_NONE;
    uint32  max_key_len;
    alpm_lib_trie_t  *root = NULL;
    uint32  key[5] = {0, 0, 0, 0, 0};
    _alpm_pfx_node_t    *pfx_node = NULL;

    max_key_len = alpm_util_trie_max_key_len(u, ipt);
    ALPM_IER(
        alpm_lib_trie_init(max_key_len, &ACB_PVT_TRIE(acb, vrf_id, ipt)));

    ACB_VRF_INIT_SET(u, acb, vrf_id, ipt);
    ACB_DR_INIT_CLEAR(u, acb, vrf_id, ipt);

    if (ACB_HAS_RTE(acb, vrf_id)) {
        rv = alpm_lib_trie_init(max_key_len, &ALPM_VRF_TRIE(u, ACB_APP(acb),
                                vrf_id, ipt));
        if (BCM_SUCCESS(rv)) {
            ALPM_ALLOC_EG(pfx_node, sizeof(*pfx_node), "Payload for pfx trie key");
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
    }

bad:
    return rv;
}

/*
 * Function:
 *      alpm_wb_pvt_insert
 * Purpose:
 *      ALPM pivot restore and insert from warmboot.
 * Parameters:
 *      u        - Device unit
 *      acb      - ALPM control blobk pointer
 *      vrf_id   - VRF ID
 *      pkm      - Packing mode (32B, 64B, 128)
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      idx      - (In)Memory Index
 *      sub_idx  - (In)Memory sub_idx (for half-entry) or ent index
 *      adata    - (In)Assoc_Data of bucket node
 *      pvt_node - (Out)Pointer of new pvt node inserted
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_wb_pvt_insert(int u, _alpm_cb_t *acb, int vrf_id, int pkm,
                       uint32 *key, int key_len, int idx, int sub_idx,
                       _alpm_bkt_adata_t *adata, _alpm_pvt_node_t **pvt_node)
{
    int                 rv = BCM_E_NONE;
    int                 ipt = ALPM_PKM2IPT(pkm);
    uint32              max_key_len;
    uint32              pvt_idx = idx;
    uint32              pfx[5];
    _alpm_pvt_node_t    *npvt_node = NULL;
    alpm_lib_trie_t     *pvt_trie = NULL;
    int                 ipmc = (adata->defip_flags & BCM_L3_IPMC);

    ALPM_ALLOC_EG(npvt_node, sizeof(_alpm_pvt_node_t), "pvt_node");

    max_key_len = alpm_util_trie_max_key_len(u, ipt);
    ALPM_IEG(alpm_lib_trie_init(max_key_len, &PVT_BKT_TRIE(npvt_node)));

    PVT_BKT_VRF(npvt_node)   = vrf_id;
    PVT_BKT_PKM(npvt_node)   = pkm;
    PVT_KEY_LEN(npvt_node)   = key_len;

    alpm_trie_key_to_pfx(u, ipt, key, key_len, pfx);
    PVT_KEY_CPY(npvt_node, pfx);

    /* pvt_idx format
     *  L3_DEFIP_LEVEL1:       idx << 1 | sub_idx
     *  L3_DEFIP_PAIR_LEVEL1:  idx
     *
     * Level2:
     * -------------------------------
     * | ent idx | bnk idx | bkt idx |
     * -------------------------------
     */
    if (ACB_HAS_TCAM(acb)) {
        if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
            pvt_idx = idx << 1 | sub_idx;
        } else {
            pvt_idx = idx;
        }
    }
    PVT_IDX(npvt_node) = pvt_idx;

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, ipt);
    rv = alpm_lib_trie_insert(pvt_trie, npvt_node->key, key_len,
                              (alpm_lib_trie_node_t *)npvt_node);
bad:
    if (BCM_FAILURE(rv)) {
        if (npvt_node != NULL) {
            alpm_util_free(npvt_node);
        }
    }
    *pvt_node = npvt_node;
    return rv;
}

/*
 * Function:
 *      alpm_wb_bkt_insert
 * Purpose:
 *      ALPM bucket restore and insert from warmboot.
 * Parameters:
 *      u        - Device unit
 *      vrf_id   - VRF ID
 *      ipt      - Packing mode (V4, V6)
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      ent_idx  - (In)Hw mem location
 *      adata    - (In)Assoc_Data of bucket node
 *      pvt_node - (In)Pointer of pvt node to insert for bucket
 *      bkt_node - (Out)Pointer of new bucket node inserted
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_wb_bkt_insert(int u, int vrf_id, int ipt,
                       uint32 *key, int key_len, int ent_idx,
                       _alpm_bkt_adata_t *adata,
                       _alpm_pvt_node_t *pvt_node,
                       _alpm_bkt_node_t **bkt_node)
{
    int                 rv = BCM_E_NONE;
    uint32              pfx[5];
    _alpm_bkt_node_t    *nbkt_node = NULL;
    alpm_lib_trie_t     *bkt_trie = NULL;

    ALPM_ALLOC_EG(nbkt_node, sizeof(_alpm_bkt_node_t), "bkt_node");
    alpm_trie_key_to_pfx(u, ipt, key, key_len, pfx);
    sal_memcpy(nbkt_node->key, pfx, sizeof(nbkt_node->key));
    nbkt_node->key_len = key_len;
    nbkt_node->ent_idx = ent_idx;

    sal_memcpy(&nbkt_node->adata, adata, sizeof(_alpm_bkt_adata_t));
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = alpm_lib_trie_insert(bkt_trie, nbkt_node->key, key_len,
                              (alpm_lib_trie_node_t *)nbkt_node);
bad:
    if (BCM_FAILURE(rv)) {
        if (nbkt_node != NULL) {
            alpm_util_free(nbkt_node);
        }
    }
    *bkt_node = nbkt_node;
    return rv;
}

/*
 * Function:
 *      alpm_wb_pfx_trie_add
 * Purpose:
 *      ALPM prefix restore and insert from warmboot.
 * Parameters:
 *      u        - Device unit
 *      vrf_id   - VRF ID
 *      pkm      - Packing mode (32B, 64B, 128)
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      ent_idx  - (In)Hw mem location
 *      adata    - (In)Assoc_Data of prefix node
 *      bkt_node - (In)Pointer of bkt node for prefix node inserted
 * Returns:
 *      BCM_E_XXX
 */
int
alpm_wb_pfx_trie_add(int u, _alpm_cb_t *acb, int vrf_id, int ipt,
                     uint32 *key, int key_len, int ent_idx,
                     _alpm_bkt_adata_t *adata,
                     _alpm_bkt_node_t *bkt_node)
{
    int                 rv = BCM_E_NONE;
    uint32              pfx[5];
    _alpm_pfx_node_t    *pfx_node = NULL, *tmp_node;
    alpm_lib_trie_t     *pfx_trie = NULL;
    alpm_lib_trie_node_t         *lpmp;

    ALPM_ALLOC_EG(pfx_node, sizeof(_alpm_pfx_node_t), "VRF_trie_node");
    alpm_trie_key_to_pfx(u, ipt, key, key_len, pfx);
    sal_memcpy(pfx_node->key, pfx, sizeof(pfx_node->key));
    pfx_node->key_len = key_len;
    pfx_node->ent_idx = ent_idx;

    sal_memcpy(&pfx_node->adata, adata, sizeof(_alpm_bkt_adata_t));
    pfx_node->bkt_ptr = bkt_node;
    pfx_trie = ALPM_VRF_TRIE(u, ACB_APP(acb), vrf_id, ipt);
    if (key_len == 0) {
        /* Add a route already in trie */
        lpmp = NULL;
        rv = alpm_lib_trie_find_lpm(pfx_trie, 0, 0, &lpmp);
        if (BCM_SUCCESS(rv)) {
            tmp_node = (_alpm_bkt_node_t *)lpmp;
            tmp_node->bkt_ptr = bkt_node;
            return rv;
        }
    }
    rv = alpm_lib_trie_insert(pfx_trie, pfx_node->key, key_len,
                              (alpm_lib_trie_node_t *)pfx_node);
bad:
    if (BCM_FAILURE(rv)) {
        if (pfx_node != NULL) {
            alpm_util_free(pfx_node);
        }
    }
    return rv;
}

/*
 * Function:
 *      alpm_cb_wb_process
 * Purpose:
 *      ALPM warmboot CB process (recursive from top to bottom).
 *      Read HW mem and restore SW state and tries.
 * Parameters:
 *      u        - Device unit
 *      vrf_id   - VRF ID
 *      pkm      - Packing mode (32B, 64B, 128)
 *      acb      - ALPM control block pointer
 *      fent     - (In)ALPM_DATA entry
 *      key      - (In)Key prefix
 *      key_len  - (In)Key prefix length
 *      idx      - (In)Memory Index
 *      sub_idx  - (In)Memory sub_idx (for half-entry) or ent index
 *      adata    - (In)Assoc Data structure pointer
 *      route_cnt- (Out)Accumulated total route count pointer
 *      trv_data - (In)pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
alpm_cb_wb_process(int u, int vrf_id, int pkm, _alpm_cb_t *acb,
                   void *fent, uint32 *key, int key_len, int idx,
                   int sub_idx, _alpm_bkt_adata_t *adata,
                   uint32 *route_cnt, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 bkt_entry[ALPM_MEM_ENT_MAX];
    uint32 fent1[ALPM_MEM_ENT_MAX];
    uint32 ftmp[ALPM_MEM_ENT_MAX];
    uint32 new_key[4];
    uint32 fmt, valid;
    int ipt, pvt_pkm = pkm;
    int sublen, new_len, def_miss;
    int idx1, eid, bnk, ent_idx;
    int nh_ecmp_idx, cmp_result;
    int bank_occupied;
    _alpm_bkt_adata_t adata1;
    char ip_buf[IP6ADDR_STR_LEN];
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_bkt_info_t bkt_info;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;
    _alpm_tbl_t bkt_tbl;
    _alpm_tbl_t pvt_tbl;

    if (ACB_IDX(acb) >= ACB_CNT(u)) {
        return BCM_E_INTERNAL; /* wrong ALPM control level called */
    }

    ipt = ALPM_PKM2IPT(pkm);
    bkt_tbl = ACB_BKT_TBL(acb, vrf_id);
    pvt_tbl = (ACB_HAS_TCAM(acb) ? acb->pvt_tbl[pvt_pkm] :
                    ACB_BKT_TBL(ACB_UPR(u, acb), vrf_id));
    /**** Pivot level ****/
    alpm_util_bkt_info_get(u, vrf_id, ipt, pvt_pkm, acb, fent, sub_idx,
                           &bkt_info, &sublen, &def_miss);

    ALPM_INFO(("%s: Pivot idx:%d.%d key:0x%08x len:%d dest:%d\n",
               ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, key[0], sublen,
               adata->defip_ecmp_index));
    /* kshift=sublen, must be same as previous key_len */
    if (!ACB_BKT_FIXED_FMT(acb, 1) && sublen != key_len) {
        ALPM_ERR(("%s: ALPM_DATA invalid kshift:%d vs pivot len:%d at %d.%d (%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), sublen, key_len, idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }
    if (BI_BKT_IDX(&bkt_info) >= ACB_BKT_CNT(acb)) {
        ALPM_ERR(("%s: ALPM_DATA invalid bkt_ptr at %d.%d (%d)\n",
                  ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, BI_BKT_IDX(&bkt_info)));
        return BCM_E_NONE; /* just skip the pivot */
    }

    /**** 0) ALPM VRF init ****/
    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
        ALPM_IER(alpm_wb_vrf_init(u, acb, vrf_id, ipt));
    }

    /**** 1) Pivot trie restore ****/
    ALPM_IER(alpm_wb_pvt_insert(u, acb, vrf_id, pkm, key, key_len, idx, sub_idx, adata, &pvt_node));
    /* Restore pvt_node->bkt_info: rofs & bkt_idx */
    PVT_ROFS(pvt_node) = BI_ROFS(&bkt_info);
    PVT_BKT_IDX(pvt_node) = BI_BKT_IDX(&bkt_info);
    PVT_SUB_BKT_IDX(pvt_node) = BI_SUB_BKT_IDX(&bkt_info);

    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        fmt = bkt_info.bnk_fmt[bnk];
        if ((fmt == 0) ||  (fmt > ACB_FMT_CNT(acb, vrf_id))) {
            if (fmt > ACB_FMT_CNT(acb, vrf_id)) {
                ALPM_ERR(("%s: ALPM_DATA format wrong idx:%d.%d (bank=%d fmt=%d).\n",
                          ALPM_TBL_NAME(pvt_tbl), idx, sub_idx, bnk, fmt));
            }
            continue;
        }
        /* Restore pvt_node->bkt_info: bnk_fmt[bnk] */
        pvt_node->bkt_info.bnk_fmt[bnk] = fmt;

        /* Restore ACB_VRF_DB_TYPE(u, acb, vrf_id, pkm) given fmt */
        ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt) =
            (uint8) alpm_util_bkt_fmt_type_get(u, vrf_id, acb, (uint8)fmt);

        /**** Bucket Level ****/
        idx1 = ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(&bkt_info), BI_BKT_IDX(&bkt_info), bnk);
        ALPM_IER(alpm_bkt_entry_read_no_cache(u, ACB_BKT_TBL(acb, vrf_id),
                                              acb, bkt_entry, idx1));
        bank_occupied = 0;
        for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fmt); eid++) {
            uint32 key2[4] = {0};
            int len2 = 0;
            int sub_bkt_ptr = 0;

            /* process bucket entry Prefix */
            alpm_util_ent_ent_get(u, vrf_id, acb, bkt_entry, fmt, eid, ftmp);
            alpm_util_bkt_pfx_get(u, vrf_id, acb, ftmp, fmt, key2, &len2, &valid);

            /* process bucket entry ASSOC_DATA */
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
                bank_occupied = 1;
            }
            /* Restore pvt_node->bkt_info: vet_bmp[bnk] */
            pvt_node->bkt_info.vet_bmp[bnk] |= 1 << eid;
            ent_idx = ALPM_IDX_MAKE(acb, &pvt_node->bkt_info, bnk, eid);

            /**** 2) Bucket trie restore ****/
            ALPM_IER(alpm_wb_bkt_insert(u, vrf_id, ipt, new_key, new_len,
                                     ent_idx, &adata1, pvt_node, &bkt_node));

            ACB_PVT_PTR(acb, pvt_node, idx1) = pvt_node;

            /* end recursion here if reach to route level (last level) */
            if (ACB_HAS_RTE(acb, vrf_id)) {
                VRF_ROUTE_ADD(acb, vrf_id, ipt);
                alpm_util_cfg_construct(u, vrf_id, ipt, new_key, new_len,
                                        &adata1, &lpm_cfg);
                nh_ecmp_idx = lpm_cfg.defip_ecmp_index;
                if (ALPM_IS_IPV4(ipt)) {
                    alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
                } else {
                    alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
                }

                (*route_cnt)++;
                ALPM_INFO(("%s: Route idx:%d.%d key:0x%08x len:%d ",
                      ALPM_TBL_NAME(bkt_tbl), idx1, eid, new_key[0], new_len));
                ALPM_INFO(("=> Found route %d: IP_addr:%s Sublen:%d Dest:%d\n",
                      *route_cnt, ip_buf, new_len, adata1.defip_ecmp_index));

                /* Execute operation routine if any. */
                if (trv_data && trv_data->op_cb) {
                    (void)(*trv_data->op_cb)(u, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx,
                                                &cmp_result);
                }

                /**** 3) Prefix trie restore ****/
                ALPM_IER(alpm_wb_pfx_trie_add(u, acb, vrf_id, ipt, new_key,
                                      new_len, ent_idx, &adata1, bkt_node));
                continue;
            }

            /* prevent going further down if somehow last level
               is not Route level due to internal failure */
            if ((ACB_IDX(acb) + 1) >= ACB_CNT(u)) {
                return BCM_E_INTERNAL;
            }

            /* get ALPM Data entry ready for next level */
            alpm_util_ent_data_get(u, vrf_id, ipt, acb, ftmp, fmt, fent1);
            ALPM_IER(alpm_cb_wb_process(u, vrf_id, pkm, ACB_DWN(u, acb),
                                        fent1, new_key, new_len, idx1, eid,
                                        &adata1, route_cnt, trv_data));

        } /* for ent */
        if (!bank_occupied) {
            pvt_node->bkt_info.bnk_fmt[bnk] = 0;
        } else {
            SHR_BITDCL *bnk_bmp;
            _alpm_bkt_pool_conf_t *bp_conf;
            uint32 bank_idx = 0;
            int bnkpb;
            int used;
            bp_conf = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
            bnk_bmp = BPC_BNK_BMP(bp_conf, ipt);
            bnkpb = BPC_BNK_PER_BKT(bp_conf);
            bank_idx = BI_BKT_IDX(&bkt_info) * bnkpb + bnk;
            if (BI_ROFS(&bkt_info) > bnk) {
                bank_idx += bnkpb;
            }

            if (ALPM_BKT_SHARE_THRES(u)) {
                /* Inc BKT_USED before bnk_bmp set if no used bits in the bkt */
                SHR_BITTEST_RANGE(bnk_bmp, (bank_idx - bank_idx % bnkpb), bnkpb, used);
                if (!used) {
                    BPC_BKT_USED(bp_conf, ipt) ++;
                }
            }

            SHR_BITSET(bnk_bmp, bank_idx);
            BPC_BNK_USED(bp_conf, ipt) ++;
        }
    } /* for bnk */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_alpm_warmboot_walk
 * Purpose:
 *      Recover LPM and ALPM entries for both IPv4 and IPv6
 * Parameters:
 *      u        - (IN)Device unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_warmboot_walk(int u, int app, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6, key_mode;
    int idx, sub_idx;
    int vrf_id;
    int idx_end;
    int nh_ecmp_idx;
    int step_size;
    int cmp_result;
    int rv = BCM_E_FAIL;
    uint32 lpm_entry[ALPM_MEM_ENT_MAX];
    uint32 fent[ALPM_MEM_ENT_MAX];
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_cb_t *acb;
    _alpm_bkt_adata_t adata0;
    int pk, pkm, use_alpm;
    uint32 route_cnt = 0;
    uint32 key[4];
    int key_pfx_len;
    int tcam_idx_reinit;
    int rv_tcam;
    int ipmc;

    if (!bcm_esw_alpm_inited(u)) {
        return BCM_E_INIT;
    }

    acb = ACB_TOP(u, app); /* from top to bottom */

    for (pk = 0; pk < ALPM_PKM_CNT; pk++) {
        if (ALPM_TCAM_TBL_SKIP(u, pk)) {
            continue; /* skip duplicated or invalid TCAM table */
        }

        if (app == APP0) {
            idx = 0;
        } else if (app == ALPM_APP1) {
            idx = tcam_table_size(u, APP0, pk);
            if (SOC_URPF_STATUS_GET(u)) {
                idx <<= 1;
            }
        } else if (app == ALPM_APP2) {
            idx = tcam_table_size(u, APP0, pk);
            if (SOC_URPF_STATUS_GET(u)) {
                idx <<= 1;
            }
            idx += tcam_table_size(u, ALPM_APP1, pk);
        } else {
            idx = 0;
        }

        idx_end = idx + tcam_table_size(u, app, pk);
        /* Walk all TCAM entries */
        for (; idx < idx_end; idx++) {
            ALPM_IEG(tcam_entry_read_no_cache(u, app, pk, lpm_entry, idx, idx));

            /* to avoid double TCAM reinit if both sub_idx are valid */
            tcam_idx_reinit = FALSE;
            for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
                int ipt;
                rv_tcam = tcam_valid_entry_mode_get(u, app, pk, lpm_entry,
                              &step_size, &pkm, &ipv6, &key_mode, sub_idx);
                if (BCM_FAILURE(rv_tcam)) {
                    if (rv_tcam == BCM_E_INTERNAL) {
                        ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type (IPv6):%d in idx:%d.%d\n",
                                  TCAM_TBL_NAME(acb, pk), key_mode, ipv6, idx, sub_idx));
                    }
                    continue;
                }

                ipt = ALPM_PKM2IPT(pkm);

                tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &vrf_id);
                /* process pre-pivot ASSOC_DATA */
                tcam_entry_adata_get(u, app, pkm, lpm_entry, sub_idx, &adata0);
                use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
                           ALPM_VRF_ID_HAS_BKT(u, vrf_id);

                /* Get Pre-pivot key IP_ADDR and prefix length (mask) */
                sal_memset(key, 0, sizeof(key));
                key_pfx_len = 0;

                tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, key);
                tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &key_pfx_len);

                /* only do tcam_reinit once per TCAM index */
                if (tcam_idx_reinit == FALSE) {
                    tcam_wb_reinit(u, app, vrf_id, pkm, idx, key_pfx_len,
                                   !!(adata0.defip_flags & BCM_L3_IPMC));
                    tcam_idx_reinit = TRUE;
                }

                /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
                 * resides in TCAM */
                if (!use_alpm) {
                    alpm_util_cfg_construct(u, vrf_id, ipt, key, key_pfx_len,
                                            &adata0, &lpm_cfg);

                    if (!ALPM_HIT_SKIP(u) &&
                        tcam_entry_hit_get(u, app, pkm, lpm_entry, sub_idx)) {
                        lpm_cfg.defip_flags |= BCM_L3_HIT;
                    }

                    ipmc = lpm_cfg.defip_flags & BCM_L3_IPMC;

                    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
                        lpm_cfg.defip_index = idx << 1 | sub_idx;
                    } else {
                        lpm_cfg.defip_index = idx;
                    }
                    nh_ecmp_idx = lpm_cfg.defip_ecmp_index;
                    route_cnt++;

                    /* TCAM pvt trie restore for Direct Routes only */
                    if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                        (void)alpm_pvt_trie_init(u, app, vrf_id, ipt);
                    }
                    alpm_trie_key_to_pfx(u, ipt, key, key_pfx_len, lpm_cfg.user_data);
                    (void)alpm_pvt_trie_insert(u, &lpm_cfg);
                    VRF_ROUTE_ADD(acb, vrf_id, ipt);

                    if (trv_data && trv_data->op_cb) {
                        (void) (*trv_data->op_cb)(u, (void *)trv_data,
                                                  (void *)&lpm_cfg,
                                                  (void *)&nh_ecmp_idx,
                                                  &cmp_result);
                    }
                    continue;
                }

                ALPM_INFO(("VRF:%d %s\n", vrf_id, alpm_util_pkm_str[pkm]));

                /* process ALPM_DATA level recursively from top to bottom */
                tcam_entry_bdata_get(u, app, pkm, lpm_entry, sub_idx, fent);

                ALPM_IEG(alpm_cb_wb_process(u, vrf_id, pkm, acb, fent,
                                            key, key_pfx_len, idx, sub_idx,
                                            &adata0, &route_cnt, trv_data));
            } /* for sub_idx */
        } /* for idx */
    } /* for pk */

    /* Update bpm_len of pivot after perfix trie is restored */
    alpm_pivot_bpm_len_update(u, app);

    /* Update TCAM state after tcam_reinit */
    ALPM_IEG(tcam_wb_reinit_done(u, app));

#ifdef ALPM_WARM_BOOT_DEBUG
    /* TD3ALPMTBD */
    /* bcm_esw_alpm_sw_dump(u); */
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;
bad:
    return (rv);
}

/*
 * Function:
 *      alpm_wb_test
 * Purpose:
 *      ALPM warmboot walk test function (simulates with ALPM soft-reinit).
 * Parameters:
 *      u        - (IN)Device unit number.
 * Returns:
 *      BCM_E_XXX
 */
/*
int
alpm_wb_test(int u)
{
    int vrf_id, ipt;
    int rv = BCM_E_NONE;

    if (bcm_esw_alpm_inited(u)) {
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                alpm_vrf_deinit(u, vrf_id, ipt);
            }
        }

        bcm_esw_alpm_tcam_deinit(u);
        ALPM_DRV(u)->alpm_ctrl_deinit(u);

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
    }
    bcm_esw_alpm_init(u);
    LOG_CLI(("ALPM module soft-reset!\n"));
    LOG_CLI(("ALPM warmboot walk: restoring from Hw mem ...\n"));
    rv = bcm_esw_alpm_warmboot_walk(u, app, NULL);

    return rv;
}
*/

STATIC int
alpm_pfx_update_match_cb(_alpm_pfx_node_t *pfx_node, void *user_data)
{
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_pfx_update_match_t *upd_data = (_alpm_pfx_update_match_t *)user_data;
    _bcm_l3_trvrs_data_t *trv_data = upd_data->trv_data;
    int *def_rte_cnt = upd_data->def_rte_cnt;
    alpm_def_rte_info_t *def_rte = upd_data->def_rte;
    int u = upd_data->unit;
    int vrf_id = upd_data->vrf_id;
    int ipt = upd_data->ipt;
    int app = upd_data->app;

    int idx;
    int ent;
    _alpm_bkt_adata_t adata;
    int nh_ecmp_idx, cmp_result;
    char ip_buf[IP6ADDR_STR_LEN];

    _alpm_cb_t *acb = ACB_VRF_BTM(u, vrf_id, app);
    _alpm_tbl_t bkt_tbl = ACB_BKT_TBL(acb, vrf_id);
    uint32 key[5] = {0};

    if (pfx_node->bkt_ptr == NULL) {
        /* VRF global low "virtual def rte" */
        return BCM_E_NONE;
    }

    idx = ALPM_TAB_IDX_GET(pfx_node->bkt_ptr->ent_idx);
    ent = ALPM_IDX_TO_ENT(pfx_node->bkt_ptr->ent_idx);
    adata = pfx_node->bkt_ptr->adata;

    alpm_trie_pfx_to_key(u, ipt, pfx_node->key, pfx_node->key_len, key);
    alpm_util_cfg_construct(u, vrf_id, ipt, key, pfx_node->key_len,
                            &adata, &lpm_cfg);

    if ((trv_data->flags & BCM_L3_IP6) != (lpm_cfg.defip_flags & BCM_L3_IP6)) {
        return BCM_E_NONE;
    }

    /* Dist hitbit (skip if def_rte==NULL for fast delete_all) */
    if (!ALPM_HIT_SKIP(u) && def_rte) {
        int hit_val;
        ALPM_HIT_LOCK(u);
        hit_val = alpm_rte_hit_get(u, app, vrf_id, bkt_tbl, idx, ent);
        if (hit_val > 0) {
            lpm_cfg.defip_flags |= BCM_L3_HIT;
        }
        ALPM_HIT_UNLOCK(u);
    }

    lpm_cfg.defip_index = alpm_util_ent_phy_idx_get(u, acb, vrf_id, idx);
    nh_ecmp_idx = lpm_cfg.defip_ecmp_index;
    adata.defip_flags = lpm_cfg.defip_flags;

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    ALPM_INFO(("%s: Route idx:%d.%d IP_addr:%s Sublen:%d Dest:%d\n",
               ALPM_TBL_NAME(bkt_tbl), idx, ent,
               ip_buf, pfx_node->key_len, lpm_cfg.defip_ecmp_index));

    /* Check if this is default route (skip if def_rte==NULL for fast delete_all) */
    /* Subnet length zero will indicate default route */
    if (def_rte && (lpm_cfg.defip_sub_len == 0)) {
        if (*def_rte_cnt < ALPM_VRF_ID_CNT(u) * 2) {
            def_rte[*def_rte_cnt].vrf_id = vrf_id;
            def_rte[*def_rte_cnt].ipt = ipt;
            def_rte[*def_rte_cnt].bkt_idx = idx;
            sal_memcpy(&def_rte[*def_rte_cnt].adata,
                       &adata, sizeof(adata));
            (*def_rte_cnt)++;
        }
    } else {
        /* Execute operation routine if any. */
        if (trv_data && trv_data->op_cb) {
            int rv = (*trv_data->op_cb)(u, (void *)trv_data,
                                        (void *)&lpm_cfg,
                                        (void *)&nh_ecmp_idx,
                                        &cmp_result);
            if (BCM_FAILURE(rv)) {
                ALPM_ERR(("vrf:%d %s/%d traverse op_cb failed (%d)\n",
                          vrf_id, ip_buf, pfx_node->key_len, rv));
                return rv;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
alpm_pfx_trav_update_match(int u,
                           int app,
                           alpm_def_rte_info_t *def_rte,
                           int *def_rte_cnt,
                           _bcm_l3_trvrs_data_t *trv_data)
{
    int         vrf_id, ipt;
    _alpm_pfx_update_match_t upd_data;
    int rv = BCM_E_NONE;

    upd_data.unit = u;
    upd_data.trv_data = trv_data;
    upd_data.def_rte = def_rte;
    upd_data.def_rte_cnt = def_rte_cnt;

    for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
        upd_data.app = app;
        upd_data.ipt = ipt;
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            int rv1 = BCM_E_NONE;
            if (!ACB_VRF_INITED(u, ACB_VRF_BTM(u, vrf_id, app), vrf_id, ipt)) {
                continue;
            }
            upd_data.vrf_id = vrf_id;
            rv1 = bcm_esw_alpm_pfx_traverse(u, app, vrf_id, ipt,
                                            alpm_pfx_update_match_cb,
                                            &upd_data);
            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      tcam_direct_route_update_match
 * Purpose:
 *      Update/Delete all TCAM direct route entries matching certain rule.
 * Parameters:
 *      u        - (IN)Device unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.
 * Returns:
 *      VOID
 */
STATIC int
tcam_direct_route_update_match(int u, int app, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6, key_mode;
    int idx, sub_idx;
    int vrf_id;
    int idx_end;
    int nh_ecmp_idx;
    int step_size;
    int cmp_result;
    uint32 lpm_entry[ALPM_MEM_ENT_MAX];
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_cb_t *acb;
    _alpm_bkt_adata_t adata0;
    int pk, ipt, pkm, use_alpm;
    uint32 key[4];
    int key_pfx_len;
    int rv_tcam;
    int val_sub_idx_cnt, val_sub_idx[4];
    int ipv6_req;
    int rv = BCM_E_NONE;

    ipv6_req = trv_data->flags & BCM_L3_IP6 ? TRUE : FALSE;

    acb = ACB_TOP(u, app); /* TCAM level */

    for (pk = 0; pk < ALPM_PKM_CNT; pk++) {
        if (ALPM_TCAM_TBL_SKIP(u, pk)) {
            continue; /* skip duplicated or invalid TCAM table */
        }

        idx_end = tcam_table_size(u, app, pk);
        /* Walk all TCAM entries (backward) */
        for (idx = (idx_end - 1); idx >= 0; idx--) {
            ALPM_IEG(tcam_entry_read_no_cache(u, app, pk, lpm_entry, idx, idx));

            /* pre-process valid sub_idx (to process backward) */
            val_sub_idx_cnt = 0;
            for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size) {
                rv_tcam = tcam_valid_entry_mode_get(u, app, pk, lpm_entry,
                              &step_size, &pkm, &ipv6, &key_mode, sub_idx);
                if (BCM_FAILURE(rv_tcam)) {
                    if (rv_tcam == BCM_E_INTERNAL) {
                        ALPM_ERR(("%s: Invalid Key Mode:%d or Key Type (IPv6):%d in idx:%d.%d\n",
                          TCAM_TBL_NAME(acb, pk), key_mode, ipv6, idx, sub_idx));
                    }
                    continue;
                } else {
                    if (ipv6 != ipv6_req) {
                        continue;
                    }
                    val_sub_idx[val_sub_idx_cnt++] = sub_idx;
                }
            }

            /* walk backward valid sub_idx */
            while (val_sub_idx_cnt > 0) {
                sub_idx = val_sub_idx[--val_sub_idx_cnt];
                rv_tcam = tcam_valid_entry_mode_get(u, app, pk, lpm_entry,
                              &step_size, &pkm, &ipv6, &key_mode, sub_idx);
                if (BCM_FAILURE(rv_tcam)) {
                    continue;
                }

                ipt = ALPM_PKM2IPT(pkm);

                tcam_entry_vrf_id_get(u, app, pkm, lpm_entry, sub_idx, &vrf_id);

                /* Get Pre-pivot key IP_ADDR and prefix length (mask) */
                sal_memset(key, 0, sizeof(key));
                key_pfx_len = 0;

                tcam_entry_to_key(u, app, pkm, lpm_entry, sub_idx, key);
                tcam_entry_pfx_len_get(u, app, pkm, lpm_entry, sub_idx, &key_pfx_len);

                /* process pre-pivot ASSOC_DATA */
                tcam_entry_adata_get(u, app, pkm, lpm_entry, sub_idx, &adata0);
                use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
                           ALPM_VRF_ID_HAS_BKT(u, vrf_id);

                /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
                 * resides in TCAM */
                if (!use_alpm) {
                    alpm_util_cfg_construct(u, vrf_id, ipt, key, key_pfx_len,
                                            &adata0, &lpm_cfg);

                    if (!ALPM_HIT_SKIP(u) &&
                        tcam_entry_hit_get(u, app, pkm, lpm_entry, sub_idx)) {
                        lpm_cfg.defip_flags |= BCM_L3_HIT;
                    }

                    lpm_cfg.defip_index = idx;
                    nh_ecmp_idx = lpm_cfg.defip_ecmp_index;

                    if (trv_data && trv_data->op_cb) {
                        (void) (*trv_data->op_cb)(u, (void *)trv_data,
                                                  (void *)&lpm_cfg,
                                                  (void *)&nh_ecmp_idx,
                                                  &cmp_result);
                    }
                    continue;
                }
            } /* while val_sub_idx_cnt */
        } /* for idx */
    } /* for pk */

bad:
    return rv;
}

/*
 * Function:
 *      bcm_esw_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      u        - (IN)Device unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_update_match(int u, _bcm_l3_trvrs_data_t *trv_data)
{
    int idx;
    int nh_ecmp_idx;
    int cmp_result;
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;
    uint32 key[4];
    int key_pfx_len;
    int def_rte_cnt = 0;
    int def_rte_sz;
    alpm_def_rte_info_t *def_rte = NULL;
#ifdef BCM_WARM_BOOT_SUPPORT
    int app, app_cnt = alpm_app_cnt(u);
#endif

    if (!bcm_esw_alpm_inited(u)) {
        return BCM_E_INIT;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(u)) {
        for (app = 0; app < app_cnt; app++) {
            rv = bcm_esw_alpm_warmboot_walk(u, app, trv_data);
            if (BCM_FAILURE(rv)) {
                ALPM_ERR(("ERROR!  ALPM Warmboot recovery failed\n"));
                break;
            }
        }
        return (rv);
    }
#endif

    /* Update match for all TCAM direct routes */
    ALPM_IEG(tcam_direct_route_update_match(u, APP0, trv_data));

    /* Allocate memory to store default route meta data,
     * x2 means v4 and v6 */
    def_rte_sz = 2 * ALPM_VRF_ID_CNT(u) * sizeof(alpm_def_rte_info_t);
    ALPM_ALLOC_EG(def_rte, def_rte_sz, "def_rte");

    /* ALPM routes use prefix trie traverse for update match,
       to overcome bkt bnk shrinking during update match */
    rv = alpm_pfx_trav_update_match(u, APP0, def_rte, &def_rte_cnt, trv_data);

    /* Process Default routes */
    sal_memset(key, 0, sizeof(key));
    key_pfx_len = 0;
    for (idx = 0; idx < def_rte_cnt; idx++) {
        alpm_util_cfg_construct(u, def_rte[idx].vrf_id, def_rte[idx].ipt, key,
                                key_pfx_len, &def_rte[idx].adata, &lpm_cfg);

        /* TD3ALPMTBD: add Hitbit */
        lpm_cfg.defip_index = def_rte[idx].bkt_idx;
        nh_ecmp_idx = lpm_cfg.defip_ecmp_index;

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            int rv1 = BCM_E_NONE;
            rv1 = (*trv_data->op_cb)(u, (void *)trv_data,
                                    (void *)&lpm_cfg,
                                    (void *)&nh_ecmp_idx,
                                     &cmp_result);
            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }
        }
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    /* TD3ALPMTBD */
    /* bcm_esw_alpm_sw_dump(u); */
#endif

bad:
    if (def_rte != NULL) {
        alpm_util_free(def_rte);
    }

    return (rv);
}

/*
 * Function:
 *      bcm_esw_alpm_fast_delete_all
 * Purpose:
 *      Fast delete all entries in defip table using different methods:
 *      - Direct routes: normal delete (route + ecmp_nh)
 *      - TCAM Hw table delete
 *      - ALPM routes: delete only ecmp_nh
 * Parameters:
 *      u         - (IN)Device unit number.
 *      trv_data  - (IN)Delete pattern + compare,act,notify routines for TCAM Direct routes.
 *      trv_data1 - (IN)Delete pattern + compare,act,notify routines for ALPM routes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_fast_delete_all(int u, _bcm_l3_trvrs_data_t *trv_data,
                             _bcm_l3_trvrs_data_t *trv_data1)
{
    int rv = BCM_E_NONE;
    int rv1;
    int app, app_cnt = alpm_app_cnt(u);

    for (app = 0; app < app_cnt; app++) {
        /* TCAM direct routes: delete route + nh */
        if (BCM_XGS3_L3_DEFIP_IP4_CNT(u)) {
            trv_data->flags = 0;
            ALPM_IEG(tcam_direct_route_update_match(u, app, trv_data));
        }
        if (BCM_XGS3_L3_DEFIP_IP6_CNT(u)) {
            trv_data->flags |= BCM_L3_IP6;
            ALPM_IEG(tcam_direct_route_update_match(u, app, trv_data));
        }

        /* Cleanup TCAM Hw table to avoid wrong routing traffic
           when delete_all ecmp_nh_idx without delete ALPM routes */
        if (ALPM_DRV(u)->tcam_hw_cleanup != NULL) {
            ALPM_DRV(u)->tcam_hw_cleanup(u, app);
        }

        /* ALPM routes fast delete_all using prefix trie traverse */
        if (BCM_XGS3_L3_DEFIP_IP4_CNT(u)) {
            trv_data1->flags = 0;
            rv1 = alpm_pfx_trav_update_match(u, app, NULL, NULL, trv_data1);

            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }
        }

        if (BCM_XGS3_L3_DEFIP_IP6_CNT(u)) {
            trv_data1->flags |= BCM_L3_IP6;
            rv1 = alpm_pfx_trav_update_match(u, app, NULL, NULL, trv_data1);

            if (BCM_FAILURE(rv1) && BCM_SUCCESS(rv)) {
                rv = rv1; /* report first error */
            }
        }
    }

bad:
    return (rv);
}

#else
typedef int bcm_esw_alpm2_alpm_wb_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
