/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_util.c
 * Purpose: ALPM util (device independent implementation).
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

char *alpm_util_pkm_str[] = {
    "32B",
    "64B",
    "128"
};

char *alpm_util_ipt_str[] = {
    "V4",
    "V6"
};

char *alpm_util_acb_str[] = {
    "ACB.0",
    "ACB.1",
};

char alpm_tmpbuf[_ALPM_ERR_MSG_BUF_ENT_SZ];

static uint32 alpm_mem_alloc_cnt;
static uint32 alpm_mem_free_cnt;
static uint32 alpm_mem_alloc_sz;

void *
alpm_util_alloc(unsigned int sz, char *s)
{
    void *rv = NULL;
    rv = sal_alloc(sz, s);
    if (rv != NULL) {
        alpm_mem_alloc_cnt++;
        alpm_mem_alloc_sz += sz;
    }

    return rv;
}

void
alpm_util_free(void *addr)
{
    if (addr != NULL) {
        alpm_mem_free_cnt++;
        return sal_free(addr);
    }
}

void
alpm_util_mem_stat_get(uint32 *alloc_cnt, uint32 *free_cnt)
{
    if (alloc_cnt) {
        *alloc_cnt = alpm_mem_alloc_cnt;
    }

    if (free_cnt) {
        *free_cnt = alpm_mem_free_cnt;
    }
}

void
alpm_util_mem_stat_clear()
{
    alpm_mem_alloc_cnt = 0;
    alpm_mem_free_cnt = 0;
    alpm_mem_alloc_sz = 0;
}

void
alpm_util_snprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    sal_vsnprintf(alpm_tmpbuf + sal_strlen(alpm_tmpbuf), _ALPM_ERR_MSG_BUF_ENT_SZ - 1, fmt, ap);
    va_end(ap);
}

int
alpm_util_trie_max_key_len(int u, int ipt)
{
    int max_key_len;

    max_key_len = ALPM_IS_IPV4(ipt) ?
                       _MAX_KEY_LEN_48_ :
                       _MAX_KEY_LEN_144_;
    return max_key_len;
}

/* Not used
int
alpm_util_trie_max_split_len(int u, int pkm)
{
    int max_split_len;

    max_split_len = ALPMC(u)->_alpm_spl[pkm];

    return max_split_len;
}
*/

void
alpm_util_trie_pfx_str(int u, int ipt, uint32 *pfx, uint32 len, char *str)
{
    uint32 key[5];
    char ip_buf[IP6ADDR_STR_LEN];

    sal_memset(key, 0x0, sizeof(key));
    alpm_trie_pfx_to_key(u, ipt, pfx, len, key);
    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, key[0]);
        sal_sprintf(str, "ipv4=%s/%d", ip_buf, len);
    } else {
        sal_sprintf(str, "ipv6=%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d",
                key[3]>>16, key[3]&0xffff, key[2]>>16, key[2]&0xffff,
                key[1]>>16, key[1]&0xffff, key[0]>>16, key[0]&0xffff, len);
    }
}

void
alpm_util_trie_pfx_print(int u, int ipt, uint32 *pfx, uint32 len, const char *str)
{
    uint32 key[5];
    char ip_buf[IP6ADDR_STR_LEN];

    sal_memset(key, 0x0, sizeof(key));
    alpm_trie_pfx_to_key(u, ipt, pfx, len, key);
    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, key[0]);
        cli_out("%s %s/%d\n", str, ip_buf, len);
    } else {
        cli_out("%s ipv6=%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d\n",
                str, key[3]>>16, key[3]&0xffff, key[2]>>16, key[2]&0xffff,
                key[1]>>16, key[1]&0xffff, key[0]>>16, key[0]&0xffff, len);
    }
}

int
alpm_util_trie_pvt_node_print(alpm_lib_trie_node_t *trie, void *datum)
{
    if (trie != NULL) {
        cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->skip_addr, trie->skip_len,
                trie->count, trie->child[0],
                trie->child[1]);

        if (trie->type == trieNodeTypePayload) {
            int ipt;
            _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;

            ipt = PVT_BKT_IPT(pvt_node);
            alpm_util_trie_pfx_print(0, ipt,
                pvt_node->key, pvt_node->key_len, " ip:");
        } else {
            cli_out("\n");
        }
    }

    return SOC_E_NONE;
}

/* Print internal node key and length */
int
alpm_util_trie_pvt_node_print1(alpm_lib_trie_node_t *trie, uint32 *key, uint32 key_len, void *datum)
{
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)datum;
    int u = merge_ctrl->unit;
    int ipt = merge_ctrl->ipt;

    if (trie != NULL) {
        cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->skip_addr, trie->skip_len,
                trie->count, trie->child[0],
                trie->child[1]);

        if (trie->type == trieNodeTypePayload) {
            _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;

            alpm_util_trie_pfx_print(u, ipt, pvt_node->key, pvt_node->key_len, " ip:");
        } else {
            alpm_util_trie_pfx_print(u, ipt, key, key_len, " key:");
        }
    }

    return SOC_E_NONE;
}

/* Print internal node key and length */
int
alpm_util_trie_bkt_node_print1(alpm_lib_trie_node_t *trie, uint32 *key, uint32 key_len, void *datum)
{
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)datum;
    int u = merge_ctrl->unit;
    int ipt = merge_ctrl->ipt;

    if (trie != NULL) {
        cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->skip_addr, trie->skip_len,
                trie->count, trie->child[0],
                trie->child[1]);

        if (trie->type == trieNodeTypePayload) {
            _alpm_bkt_node_t *bkt_node = (_alpm_bkt_node_t *)trie;

            alpm_util_trie_pfx_print(u, ipt, bkt_node->key, bkt_node->key_len, " ip:");
        } else {
            alpm_util_trie_pfx_print(u, ipt, key, key_len, " key:");
        }
    }

    return SOC_E_NONE;
}

/* Print internal node key and length */
int
alpm_util_trie_pvt_bkt_print1(alpm_lib_trie_node_t *trie, uint32 *key, uint32 key_len, void *datum)
{
    _alpm_cb_merge_ctrl_t *merge_ctrl = (_alpm_cb_merge_ctrl_t *)datum;
    int u = merge_ctrl->unit;
    int ipt = merge_ctrl->ipt;

    if (trie != NULL) {
        cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->skip_addr, trie->skip_len,
                trie->count, trie->child[0],
                trie->child[1]);

        if (trie->type == trieNodeTypePayload) {
            _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;

            alpm_util_trie_pfx_print(u, ipt, pvt_node->key, pvt_node->key_len, " ip:");

            if (PVT_BKT_TRIE(pvt_node)) {
                cli_out(" => BKT trie:\n");
                alpm_lib_trie_dump1(PVT_BKT_TRIE(pvt_node), alpm_util_trie_bkt_node_print1, datum);
                cli_out(" <= BKT trie\n");
            }
        } else {
            alpm_util_trie_pfx_print(u, ipt, key, key_len, " key:");
    }
}

    return SOC_E_NONE;
}

int
alpm_util_trie_pvt_bkt_print(alpm_lib_trie_node_t *trie, void *datum)
{
    if (trie != NULL) {
        cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->skip_addr, trie->skip_len,
                trie->count, trie->child[0],
                trie->child[1]);
        if (trie->type == trieNodeTypePayload) {
            int ipt, vrf_id;
            char vrf_str[16] = {0};
            _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)trie;

            ipt = PVT_BKT_IPT(pvt_node);
            vrf_id = pvt_node->vrf_id;
            sal_sprintf(vrf_str, "%d ", vrf_id);
            alpm_util_trie_pfx_print(0, ipt,
                pvt_node->key, pvt_node->key_len, vrf_str);

            if (PVT_BKT_TRIE(pvt_node)) {
                cli_out(" => BKT trie:\n");
                alpm_lib_trie_dump(PVT_BKT_TRIE(pvt_node), alpm_util_trie_bkt_node_print, datum);
                cli_out(" <= BKT trie\n");
        }
        } else {
            cli_out("\n");
        }
    }

    return SOC_E_NONE;
}

int
alpm_util_trie_bkt_node_print(alpm_lib_trie_node_t *trie, void *datum)
{
    if (trie != NULL) {
        cli_out("trie: %p, type %s, "
                "count:%d Child[0]:%p Child[1]:%p ",
                trie, (trie->type == trieNodeTypePayload) ? "P" : "I",
                trie->count, trie->child[0],
                trie->child[1]);
        if (trie->type == trieNodeTypePayload) {
            int *ipt = (int *)datum;
            _alpm_bkt_node_t *bkt_node = (_alpm_bkt_node_t *)trie;

            alpm_util_trie_pfx_print(0, *ipt,
                bkt_node->key, bkt_node->key_len, "vrf");
        } else {
            cli_out("\n");
        }
    }

    return SOC_E_NONE;
}

void
alpm_util_adata_cfg_to_trie(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            _alpm_bkt_adata_t *bkt_adata)
{
    /* lpm_cfg->defip_ecmp_index is used to store nh_ecmp_idx */
    bkt_adata->defip_flags               = lpm_cfg->defip_flags;
    if (bkt_adata->defip_flags & BCM_L3_IPMC) {
        bkt_adata->defip_ecmp_index          = lpm_cfg->defip_mc_group;
    } else {
        bkt_adata->defip_ecmp_index          = lpm_cfg->defip_ecmp_index;
    }
    bkt_adata->defip_prio                = lpm_cfg->defip_prio;
    bkt_adata->defip_lookup_class        = lpm_cfg->defip_lookup_class;
    bkt_adata->defip_flex_ctr_pool       = lpm_cfg->defip_flex_ctr_pool;
    bkt_adata->defip_flex_ctr_mode       = lpm_cfg->defip_flex_ctr_mode;
    bkt_adata->defip_flex_ctr_base_id    = lpm_cfg->defip_flex_ctr_base_id;
    if (soc_feature(unit, soc_feature_lpm_prefilter) &&
        lpm_cfg->defip_inter_flags & BCM_L3_INTER_FLAGS_PRE_FILTER) {
        bkt_adata->defip_acl_class_id_valid = 1;
        bkt_adata->defip_acl_class_id = lpm_cfg->defip_acl_class_id;
    } else {
        bkt_adata->defip_acl_class_id_valid = 0;
        bkt_adata->defip_acl_class_id = 0;
    }
    return ;
}

void
alpm_util_adata_trie_to_cfg(int unit, _alpm_bkt_adata_t *bkt_adata,
                            _bcm_defip_cfg_t *lpm_cfg)
{
    /* lpm_cfg->defip_ecmp_index is used to store nh_ecmp_idx */
    lpm_cfg->defip_flags               = bkt_adata->defip_flags;

    if (soc_feature(unit, soc_feature_lpm_prefilter) &&
        bkt_adata->defip_acl_class_id_valid) {
        lpm_cfg->defip_acl_class_id = bkt_adata->defip_acl_class_id;
        lpm_cfg->defip_inter_flags |= BCM_L3_INTER_FLAGS_PRE_FILTER;
    } else if (bkt_adata->defip_flags & BCM_L3_IPMC) {
        lpm_cfg->defip_mc_group            = bkt_adata->defip_ecmp_index;
    } else {
        lpm_cfg->defip_ecmp_index          = bkt_adata->defip_ecmp_index;
    }
    lpm_cfg->defip_prio                = bkt_adata->defip_prio;
    lpm_cfg->defip_lookup_class        = bkt_adata->defip_lookup_class;
    lpm_cfg->defip_flex_ctr_pool       = bkt_adata->defip_flex_ctr_pool;
    lpm_cfg->defip_flex_ctr_mode       = bkt_adata->defip_flex_ctr_mode;
    lpm_cfg->defip_flex_ctr_base_id    = bkt_adata->defip_flex_ctr_base_id;
    return ;
}

void
alpm_util_adata_zero_cfg(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    lpm_cfg->defip_flags &=
        ~(BCM_L3_RPE | BCM_L3_DST_DISCARD | BCM_L3_MULTIPATH);
    lpm_cfg->defip_ecmp_index = 0;
    lpm_cfg->defip_mc_group = 0;
    lpm_cfg->defip_prio = 0;
    lpm_cfg->defip_lookup_class = 0;
    lpm_cfg->defip_flex_ctr_pool = 0;
    lpm_cfg->defip_flex_ctr_mode = 0;
    lpm_cfg->defip_flex_ctr_base_id = 0;
    lpm_cfg->defip_acl_class_id = 0;
    return ;
}

int
alpm_acl_app(int app)
{
    return (app == ALPM_APP1 || app == ALPM_APP2);
}

int
alpm_pid_count(int app)
{
    return alpm_acl_app(app) ? ALPM_BKT_PID_ACL_CNT : ALPM_BKT_PID_CNT;
}

int
alpm_app_cnt(int u)
{
    return ALPM_ACL_EN(u) ? ALPM_APPS : 1;
}

uint8
alpm_app_get(int u, uint32 defip_inter_flags)
{
    if (ALPM_ACL_EN(u) && (defip_inter_flags & BCM_L3_INTER_FLAGS_PRE_FILTER)) {
        if (defip_inter_flags & BCM_L3_INTER_FLAGS_PRE_FILTER_SRC) {
            return ALPM_APP2;
        } else {
            return ALPM_APP1;
        }
    }
    return APP0;
}

uint32
alpm_app_flag_get(int u, int app)
{
    uint32 defip_inter_flags = 0;

    if (alpm_acl_app(app)) {
        defip_inter_flags |= BCM_L3_INTER_FLAGS_PRE_FILTER;

        if (app == ALPM_APP2) {
            defip_inter_flags |= BCM_L3_INTER_FLAGS_PRE_FILTER_SRC;
        }
    }

    return defip_inter_flags;
}

uint8
alpm_util_route_type_get(int u, _bcm_defip_cfg_t *lpm_cfg, int app)
{
    int vrf_id = (lpm_cfg == NULL? 1 : ALPM_LPM_VRF_ID(u, lpm_cfg));
    int ipt = (lpm_cfg == NULL ? ALPM_IPT_V4 : ALPM_LPM_IPT(u, lpm_cfg));
    _alpm_cb_t *acb = ACB_VRF_BTM(u, vrf_id, app);

    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        return soc_feature(u, soc_feature_alpm_flex_stat) ? 1 : 0;
    } else {
        return ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt);
    }
}

int
alpm_util_route_type_check(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 my_route_md;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    _alpm_cb_t *acb = ACB_VRF_BTM(u, ALPM_LPM_VRF_ID(u, lpm_cfg), app);

    my_route_md = alpm_util_route_type_get(u, lpm_cfg, app);

    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
        /* check non zero PRI or CLASS_ID */
        if (lpm_cfg->defip_prio || lpm_cfg->defip_lookup_class) {
            if (my_route_md == 0) {
                ALPM_ERR(("**ROUTE.CHECK:route mode conflict - "
                          "non zero PRI or CLASS_ID in reduced mode\n"));
                return BCM_E_PARAM;
            }
        }
    }
    return BCM_E_NONE;
}

int
alpm_util_def_check(int u, _bcm_defip_cfg_t *lpm_cfg, int is_add)
{
    _alpm_cb_t     *acb;
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    /* For routes go to SRAM */
    /* combined search mode protection */
    acb = ACB_VRF_BTM(u, vrf_id, app);
    if (is_add) {
        if (VRF_ROUTE_CNT(acb, vrf_id, ipt) == 0 &&
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) &&
            lpm_cfg->defip_vrf != BCM_L3_VRF_GLOBAL &&
            lpm_cfg->defip_sub_len != 0) {
            ALPM_ERR(("**DEF.CHECK: First route in VRF %d has to "
                      "be a default route in this mode\n", vrf_id));
            return BCM_E_PARAM;
        }
    } else {
        if (VRF_ROUTE_CNT(acb, vrf_id, ipt) > 1 &&
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) &&
            lpm_cfg->defip_vrf != BCM_L3_VRF_GLOBAL &&
            lpm_cfg->defip_sub_len == 0) {
            ALPM_WARN(("**DEF.CHECK: Default route in VRF %d has to "
                      "be the last route to delete in this mode\n", vrf_id));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/* Return ip string from key and length, or bkt_node/pvt_node if not NULL */
/* Not used
int
alpm_util_key_to_str(int u, int ipt, char *ipstr, uint32 *key, uint32 key_len,
                     _alpm_bkt_node_t *bkt_node, _alpm_pvt_node_t *pvt_node)
{
    char ip_buf[IP6ADDR_STR_LEN];
    _bcm_defip_cfg_t lpm_cfg;

    if (!ipstr) {
        return BCM_E_PARAM;
    }

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    if (bkt_node) {
        key_len = bkt_node->key_len;
        alpm_trie_pfx_to_cfg(u, bkt_node->key, bkt_node->key_len, &lpm_cfg);
    } else if (pvt_node) {
        key_len = pvt_node->key_len;
        alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_node->key_len, &lpm_cfg);
    } else if (key) {
        alpm_trie_pfx_to_cfg(u, key, key_len, &lpm_cfg);
    }

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    sal_sprintf(ipstr, "%s/%d", ip_buf, key_len);

    return BCM_E_NONE;
}
*/

void
alpm_util_cfg_to_key(int u, int ipt, _bcm_defip_cfg_t *lpm_cfg, uint32 *key)
{
    uint8  *ip6;

    if (ALPM_IS_IPV4(ipt)) {
        key[0] = lpm_cfg->defip_ip_addr;
    } else {
        ip6 = lpm_cfg->defip_ip6_addr;
        key[0] = ip6[12] << 24 | ip6[13] << 16 | ip6[14] << 8 | ip6[15];
        key[1] = ip6[8] << 24 | ip6[9] << 16 | ip6[10] << 8 | ip6[11];
        key[2] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        key[3] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    }

    return ;
}

void
alpm_util_cfg_to_msk(int u, int ipt, _bcm_defip_cfg_t *lpm_cfg, uint32 *msk)
{
    bcm_ip6_t ip6;

    if (ALPM_IS_IPV4(ipt)) {
        msk[0] = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
    } else {
        bcm_ip6_mask_create(ip6, lpm_cfg->defip_sub_len);
        msk[0] = ip6[12] << 24 | ip6[13] << 16 | ip6[14] << 8 | ip6[15];
        msk[1] = ip6[8] << 24 | ip6[9] << 16 | ip6[10] << 8 | ip6[11];
        msk[2] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        msk[3] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    }

    return ;
}

void
alpm_util_ipmask_apply(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;
    bcm_ip_t mask4;
    bcm_ip6_t mask6;

    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        /* Unchanged byte count.    */
        int idx = lpm_cfg->defip_sub_len / 8;

        bcm_ip6_mask_create(mask6, lpm_cfg->defip_sub_len);
        ip6 = lpm_cfg->defip_ip6_addr;

        /* Apply subnet mask */
        if (idx < BCM_IP6_ADDRLEN) {
            ip6[idx] &= mask6[idx];
            for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
                ip6[idx] = 0;   /* Reset rest of bytes.     */
            }
        }
    } else {
        mask4 = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        /* Apply subnet mask. */
        lpm_cfg->defip_ip_addr &= mask4;
    }

    return ;
}

void
alpm_util_fmt_tbl_util(int unit, char *buf)
{
    int i, ipt, p;
    int vcnt_all;
    int cnt_all;
    int app = APP0;
    int pid_count;

    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bpc;


    cnt_all = 0;
    vcnt_all = 0;
    (void)bcm_esw_alpm_tcam_state_free_get(unit, app, -1, &cnt_all, &vcnt_all);
    cnt_all += vcnt_all;

    if (cnt_all != 0) {
        sal_sprintf(buf+strlen(buf), "L1:%d.%d%% ",
                    vcnt_all * 100 / cnt_all,
                    vcnt_all * 1000 / cnt_all % 10);
    }

    for (i = 0; i < ACB_CNT(unit); i++) {
        acb = ACB(unit, i, app);
        pid_count = alpm_pid_count(app);
        for (p = 0; p < pid_count; p++) {
            bpc = ACB_BKT_POOL(acb, p);
            if (p > 0 && bpc == ACB_BKT_POOL(acb, p-1)) {
                continue;
            }
            for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
                if (ipt > ALPM_IPT_V4 &&
                    BPC_BNK_INFO(bpc, ipt) ==
                    BPC_BNK_INFO(bpc, ipt - 1)) {
                    continue;
                }
                sal_sprintf(buf+strlen(buf), "APP%dL%d.P%d(%s):%d.%d%% ",
                    app,
                    acb->acb_idx + 2, p,
                    alpm_util_ipt_str[ipt],
                    BPC_BNK_CNT(bpc) ?
                        (BPC_BNK_USED(bpc, ipt) * 100 / BPC_BNK_CNT(bpc)) : 0,
                    BPC_BNK_CNT(bpc) ?
                        (BPC_BNK_USED(bpc, ipt) * 1000 / BPC_BNK_CNT(bpc) % 10) : 0);
            }
        }
    }
}

/* Not used
void
alpm_util_fmt_bkt_info(char *buf, int vrf_id, _alpm_cb_t *acb, _alpm_bkt_info_t *bkt_info)
{
    int i;
    sal_sprintf(buf+strlen(buf), "ACB%d->", ACB_IDX(acb));
    sal_sprintf(buf+strlen(buf), "ROFS%d[", BI_ROFS(bkt_info));
    for (i = BI_ROFS(bkt_info); i < BI_ROFS(bkt_info) + ALPM_BPB_MAX; i++) {
        int bnk = i % ALPM_BPB_MAX;
        int fmt = bkt_info->bnk_fmt[bnk];
        if (!BI_BNK_IS_USED(bkt_info, bnk)) {
            continue;
        }
        sal_sprintf(buf+strlen(buf), "B%d(%d)(%d,%d/%d)_", bnk,
                    bkt_info->bnk_fmt[bnk],
                    ACB_FMT_PFX_LEN(acb, vrf_id, fmt),
                    _shr_popcount(bkt_info->vet_bmp[bnk]),
                    ACB_FMT_ENT_MAX(acb, vrf_id, fmt));
    }
    sal_sprintf(buf+strlen(buf), "]");
}
*/

void
alpm_util_fmt_ipaddr(char buf[SAL_IPADDR_STR_LEN], ip_addr_t ipaddr)
{
    sal_sprintf(buf, "%d.%d.%d.%d",
            (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff);
}

void
alpm_util_fmt_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr_t ipaddr)
{
    sal_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            (((uint16)ipaddr[0] << 8) | ipaddr[1]),
            (((uint16)ipaddr[2] << 8) | ipaddr[3]),
            (((uint16)ipaddr[4] << 8) | ipaddr[5]),
            (((uint16)ipaddr[6] << 8) | ipaddr[7]),
            (((uint16)ipaddr[8] << 8) | ipaddr[9]),
            (((uint16)ipaddr[10] << 8) | ipaddr[11]),
            (((uint16)ipaddr[12] << 8) | ipaddr[13]),
            (((uint16)ipaddr[14] << 8) | ipaddr[15]));
}

/* Get pivot index: idx & sub_idx (or ent)  */
int
alpm_util_pvt_idx_get(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, int *idx, int *sub_idx)
{
    int pvt_idx;

    if (ACB_HAS_TCAM(acb)) {
        pvt_idx = PVT_IDX(pvt_node); /* tcam_idx */
        if (_tcam_pkm_uses_half_wide(u, PVT_BKT_PKM(pvt_node), PVT_IS_IPMC(pvt_node))) {
            *idx = pvt_idx >> 1;
            *sub_idx = pvt_idx & 0x01;
        } else {
            *idx = pvt_idx;
            *sub_idx = 0;
        }
    } else {
        /* Get upper CB bucket ent_idx for the pivot */
        ALPM_IER(alpm_pvt_ent_idx_get(u, acb, pvt_node, &pvt_idx));
        *idx = ALPM_TAB_IDX_GET(pvt_idx);
        *sub_idx = ALPM_IDX_TO_ENT(pvt_idx);
    }

    return BCM_E_NONE;
}

void
alpm_util_key_to_cfg(int u, int ipt, uint32 *key, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8  *ip6 = lpm_cfg->defip_ip6_addr;

    if (ALPM_IS_IPV4(ipt)) {
        lpm_cfg->defip_ip_addr = key[0];
    } else {
        ip6[0]  = (uint8) (key[3] >> 24);
        ip6[1]  = (uint8) (key[3] >> 16 & 0xff);
        ip6[2]  = (uint8) (key[3] >> 8 & 0xff);
        ip6[3]  = (uint8) (key[3] & 0xff);
        ip6[4]  = (uint8) (key[2] >> 24);
        ip6[5]  = (uint8) (key[2] >> 16 & 0xff);
        ip6[6]  = (uint8) (key[2] >> 8 & 0xff);
        ip6[7]  = (uint8) (key[2] & 0xff);
        ip6[8]  = (uint8) (key[1] >> 24);
        ip6[9]  = (uint8) (key[1] >> 16 & 0xff);
        ip6[10] = (uint8) (key[1] >> 8 & 0xff);
        ip6[11] = (uint8) (key[1] & 0xff);
        ip6[12] = (uint8) (key[0] >> 24);
        ip6[13] = (uint8) (key[0] >> 16 & 0xff);
        ip6[14] = (uint8) (key[0] >> 8 & 0xff);
        ip6[15] = (uint8) (key[0] & 0xff);
    }
    return ;
}

/*
 * Function:
 *      alpm_util_len_to_mask
 * Purpose:
 *      Convert IPv4 or IPv6 key length to mask.
 * Parameters:
 *      ipt      - (IN)Packing mode (V4, V6)
 *      len      - (IN)Key length
 *      mask     - (OUT)Key mask
 */
void
alpm_util_len_to_mask(int ipt, uint32 len, uint32 *mask)
{
    int i;
    int cnt = ALPM_KEY_ENT_CNT(ipt);

    for (i = 0; i < cnt; i++) { /* initialize all mask to 0 */
        mask[i] = 0;
    }

    for (i = cnt - 1; i >= 0; i--) {
        if (len <= 32) {
            break;
        }
        mask[i] = 0xffffffff;
        len -= 32;
    }
    mask[i] = ~_SHIFT_RIGHT(0xffffffff, len);
    return;
}

/* Convert ALPM ASSOC_DATA and KEY info into lpm_cfg */
int
alpm_util_cfg_construct(int u, int vrf_id, int ipt, uint32 *key, int key_len,
                        _alpm_bkt_adata_t *adata, _bcm_defip_cfg_t *lpm_cfg)
{
    sal_memset(lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
    alpm_util_adata_trie_to_cfg(u, adata, lpm_cfg);
    alpm_util_key_to_cfg(u, ipt, key, lpm_cfg);
    lpm_cfg->defip_sub_len = key_len;
    lpm_cfg->defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    /* TD3ALPMTBD, defip_flags is not determined by pkm */
    lpm_cfg->defip_flags |= (ALPM_IS_IPV4(ipt) ? 0 : BCM_L3_IP6);
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        lpm_cfg->defip_ecmp = 1;
    }

    return BCM_E_NONE;
}

int
alpm_util_bkt_info_get(int u, int vrf_id, int ipt, int pvt_pkm,
                       _alpm_cb_t *acb, void *e,
                       int sub_idx, _alpm_bkt_info_t *bkt_info,
                       int *kshift, int *def_miss)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ALPM_DATA;
    info.vrf_id = vrf_id;
    info.pvt_pkm = pvt_pkm;
    info.ipt = ipt;
    info.ent_fmt = sub_idx;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        if (bkt_info) {
            sal_memcpy(bkt_info, &info.bkt_info, sizeof(_alpm_bkt_info_t));
        }
        if (kshift) {
            *kshift = info.kshift;
        }
        if (def_miss) {
            *def_miss = info.default_miss;
        }
    }
    return rv;
}

/* Return default (minimum) bank format */
uint8
alpm_util_bkt_def_fmt_get(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    return ALPM_DRV(u)->alpm_bkt_def_fmt_get(u, acb, vrf_id, ipt);
}

/* Return Reduced/Full bank format type for a given fmt (for warmboot use) */
int16
alpm_util_bkt_fmt_type_get(int u, int vrf_id, _alpm_cb_t *acb, uint8 fmt)
{
    return ALPM_DRV(u)->alpm_bkt_fmt_type_get(u, vrf_id, acb, fmt);
}

int
alpm_util_ent_ent_get(int u, int vrf_id, _alpm_cb_t *acb, void *e,
                      uint32 fmt, int eid, void *entry)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_ent_ent_get(u, vrf_id, acb, e, fmt, eid, entry);
    return rv;
}

void
alpm_util_pfx_cat(int u, int ipt, uint32 *pfx1, int len1, uint32 *pfx2, int len2,
                  uint32 *new_pfx, int *new_len)
{
    int dst_offset;
    int max_pfx_len[] = {32, 128};

    *new_len = len1 + len2;
    dst_offset = max_pfx_len[ipt] - *new_len;
    sal_memcpy(new_pfx, pfx1, sizeof(uint32) * 4);
    SHR_BITCOPY_RANGE(new_pfx, dst_offset, pfx2, 0, len2);
}

/*
 * Function:
 *      alpm_util_pfx_len_cmp
 * Purpose:
 *      Compare two prefix length.
 * Returns:
 *      a<=>b
 */
int
alpm_util_pfx_len_cmp(void *a, void *b)
{
    _alpm_bkt_node_t **first;
    _alpm_bkt_node_t **second;

    first = (_alpm_bkt_node_t **) a;
    second = (_alpm_bkt_node_t **) b;

    if ((*first)->key_len < (*second)->key_len) {
        return 1;
    } else if ((*first)->key_len > (*second)->key_len) {
        return -1;
    }
    return 0;
}

int
alpm_util_bkt_pfx_get(int u, int vrf_id, _alpm_cb_t *acb, void *e, uint32 fmt,
                      uint32 *new_key, int *new_len, uint32 *valid)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_VALID | ALPM_INFO_MASK_KEYLEN;
    info.ent_fmt = fmt;
    info.vrf_id  = vrf_id;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        *valid = info.ent_valid;
        sal_memcpy(new_key, info.key, sizeof(uint32) * 4);
        *new_len = info.key_len;
    }
    return rv;
}

int
alpm_util_bkt_adata_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb, void *e,
                        int fmt, _alpm_bkt_adata_t *adata, int *arg1)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ASSOC_DATA;
    info.ent_fmt = fmt;
    info.ipt = ipt;
    info.vrf_id = vrf_id;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(adata, &info.adata, sizeof(_alpm_bkt_adata_t));
        if (arg1 != NULL) {
            *arg1 = BI_SUB_BKT_IDX(&info.bkt_info);
        }
    }
    return rv;
}

int
alpm_util_ent_data_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb, void *e,
                       int fmt, void *fent1)
{

    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ALPM_DATA_RAW;
    info.alpm_data_raw = fent1;
    info.ent_fmt = fmt;
    info.ipt = ipt;
    info.vrf_id = vrf_id;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    return rv;
}

int
alpm_util_ent_phy_idx_get(int u, _alpm_cb_t *acb, int vrf_id, int index)
{
    int phy_index;
    _alpm_tbl_t tbl;

    tbl = ACB_BKT_TBL(acb, vrf_id);
    phy_index = ALPM_DRV(u)->mem_ent_phy_idx_get(u, acb, tbl, index);

    return phy_index;
}

int
tcam_table_size(int u, int app, int pkm)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_table_sz(u, app, pkm);
    return rv;
}

int
tcam_valid_entry_mode_get(int u, int app, int pk, void *e, int *step_size,
                          int *pkm, int *ipv6, int *key_mode, int sub_idx)
{
    int rv = BCM_E_UNAVAIL;
    *step_size = 2; /* default step_size */
    rv = ALPM_DRV(u)->tcam_entry_mode_get(u, app, pk, e, step_size, pkm,
                                          ipv6, key_mode, sub_idx);
    return rv;
}

int
tcam_entry_adata_get(int u, int app, int pkm, void *e,
                     int sub_idx, _alpm_bkt_adata_t *adata)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_adata_get(u, app, pkm, e, sub_idx, adata);
    return rv;
}

int
tcam_entry_bdata_get(int u, int app, int pkm, void *e, int sub_idx,  void *fent)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_bdata_get(u, app, pkm, e, sub_idx, fent);
    return rv;
}

int
alpm_bkt_entry_read(int u, _alpm_tbl_t tbl, _alpm_cb_t *acb, void *e, int index)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->mem_entry_read(u, acb, tbl, index, e, FALSE);

    return rv;
}

int
alpm_bkt_entry_read_no_cache(int u, _alpm_tbl_t tbl, _alpm_cb_t *acb, void *e, int index)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->mem_entry_read(u, acb, tbl, index, e, TRUE);

    return rv;
}

int
alpm_default_usage_get(int u, int grp, int *def_route, int *def_lvl,
                       int *def_usage, int *cur_cnt_total)
{
    int         rv = BCM_E_UNAVAIL;

    if (ALPM_DRV(u)->alpm_def_usage_get != NULL) {
        rv = ALPM_DRV(u)->alpm_def_usage_get(u, grp, def_route, def_lvl,
                                             def_usage, cur_cnt_total);
    }

    return rv;
}

void
alpm_pvt_node_print(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    char ip_buf[IP6ADDR_STR_LEN];
    int ipt = PVT_BKT_IPT(pvt_node);
    int vrf_id = PVT_BKT_VRF(pvt_node);
    _bcm_defip_cfg_t lpm_cfg;
#if 0
    _alpm_bkt_info_t *bi = &(pvt_node->bkt_info);
#endif

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    if (ALPM_IS_IPV6(ipt)) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_node->key_len, &lpm_cfg);

    if (ALPM_IS_IPV4(ipt)) {
        alpm_util_fmt_ipaddr(ip_buf, lpm_cfg.defip_ip_addr);
    } else {
        alpm_util_fmt_ip6addr(ip_buf, lpm_cfg.defip_ip6_addr);
    }

    cli_out("L%d vrf:%d pvt:%p ip:%s/%d\n",
             ACB_IDX(acb)+1, vrf_id, pvt_node, ip_buf, pvt_node->key_len);
#if 0
    if (ACB_IDX(acb) == 0) {
        cli_out("rofs:%d fmt:%2d %2d %2d %2d %2d vet:%x %x %x %x %x\n", bi->rofs,
                bi->bnk_fmt[0], bi->bnk_fmt[1], bi->bnk_fmt[2], bi->bnk_fmt[3], bi->bnk_fmt[4],
                bi->vet_bmp[0], bi->vet_bmp[1], bi->vet_bmp[2], bi->vet_bmp[3], bi->vet_bmp[4]);
    } else {
        cli_out("rofs:%d fmt:%2d %2d %2d %2d %2d %2d %2d %2d vet:%x %x %x %x %x %x %x %x\n", bi->rofs,
                bi->bnk_fmt[0], bi->bnk_fmt[1], bi->bnk_fmt[2], bi->bnk_fmt[3],
                bi->bnk_fmt[4], bi->bnk_fmt[5], bi->bnk_fmt[6], bi->bnk_fmt[7],
                bi->vet_bmp[0], bi->vet_bmp[1], bi->vet_bmp[2], bi->vet_bmp[3],
                bi->vet_bmp[4], bi->vet_bmp[5], bi->vet_bmp[6], bi->vet_bmp[7]);
    }
#endif
}

#else
typedef int bcm_esw_alpm2_alpm_util_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
