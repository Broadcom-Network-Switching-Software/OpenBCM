/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        alpm_util.h
 * Purpose:     Function declarations for ALPM UTIL Internal functions.
 */

#ifndef _BCM_INT_ALPM_UTIL_H_
#define _BCM_INT_ALPM_UTIL_H_

#include <soc/defs.h>

#ifdef ALPM_ENABLE

/* ALPM */
typedef struct _alpm_log_s {
    uint16  op;
    uint16  sublen;
    int     vrf;
    int     nh_idx;
    int     rc; /* op returned code */
    int     seq; /* route sequence */
    uint32  flags;
    union u {
        bcm_ip_t    ip;
        bcm_ip6_t   ip6;
    } u;
} _alpm_log_t;

typedef struct _alpm_trace_s {
    int inited;
    int trace_en;
    int sanity_en;  /* trace sanity freq_cnt */
    int sanity_start;
    int sanity_end;
    int wrap_en;
    int wrap;       /* log wrapped TRUE/FALSE */
    int cnt;
    _alpm_log_t *buf;
    _alpm_log_t *curr;
    _alpm_log_t *start;
} _alpm_trace_t;

#define ALPM_TRACE_SIZE         0x400000
#define ALPM_TRACE_DUMP_SIZE    10000

#define ALPMTR(u)           (alpm_trace[u])
#define ALPMTR_BUF(u)       (ALPMTR(u)->buf)
#define ALPMTR_START(u)     (ALPMTR(u)->start)
#define ALPMTR_CURR(u)      (ALPMTR(u)->curr)
#define ALPMTR_CNT(u)       (ALPMTR(u)->cnt)
#define ALPMTR_WRAP(u)      (ALPMTR(u)->wrap)
#define ALPMTR_TRACE_EN(u)  (ALPMTR(u)->trace_en)
#define ALPMTR_SANITY_EN(u) (ALPMTR(u)->sanity_en)
#define ALPMTR_SANITY_START(u) (ALPMTR(u)->sanity_start)
#define ALPMTR_SANITY_END(u) (ALPMTR(u)->sanity_end)
#define ALPMTR_WRAP_EN(u)   (ALPMTR(u)->wrap_en)
#define ALPMTR_INITED(u)    (ALPMTR(u)->inited)

/* Max sub_idx count for both un-pair or pair TCAM
   (for pair TCAM step_size=2) */
#define ALPM_TCAM_SUBCNT    2

#define ALPM_HALF_FIELD_NAME(half, fld0, fld1) ((half) == 0 ? (fld0) : (fld1))

#define ALPM_KEY_ENT_CNT(ipt) ((ipt) == ALPM_IPT_V4 ? 1 : 4)

#define ALPM_IS_IPV4(pkm) ((pkm) == ALPM_PKM_32B ? TRUE : FALSE)
#define ALPM_IS_IPV6(pkm) ((pkm) == ALPM_PKM_32B ? FALSE : TRUE)
#define TCAM_TBL_NAME(acb, pkm)     (alpm_tbl_str[acb->pvt_tbl[pkm]])
#define ALPM_TBL_NAME(tbl)          (alpm_tbl_str[tbl])
#define ALPM_FLAGS_FILTER(flag) ((flag) &= (BCM_L3_MULTIPATH |                \
                                            BCM_L3_RPE | BCM_L3_DST_DISCARD))

extern char *alpm_util_pkm_str[];
extern char *alpm_util_ipt_str[];
extern char *alpm_util_acb_str[];
extern char *alpm_tbl_str[];
extern _alpm_trace_t *alpm_trace[];

/* External functions of alpm_ts.c */
extern void bcm_esw_alpm_sw_dump(int u);
extern int bcm_esw_alpm_debug_show(int u, int vrf_id, uint32 flags);
extern int bcm_esw_alpm_debug_brief_show(int u, int bucket, int index);
extern int bcm_esw_alpm_sanity_check(int u, soc_mem_t mem, int index,
                                     int check_sw);
extern int alpm_pfx_hw_route_sanity(int u, int *tot_err);

/* Additional ALPM & TCAM driver wrapper */
extern int alpm_util_bkt_info_get(int u, int vrf_id, int ipt, int pvt_pkm, _alpm_cb_t *acb,
                                  void *e, int sub_idx, _alpm_bkt_info_t *bkt_info,
                                  int *kshift, int *def_miss);
extern uint8 alpm_util_bkt_def_fmt_get(int u, _alpm_cb_t *acb, int vrf_id,
                                     int ipt);
extern int16 alpm_util_bkt_fmt_type_get(int u, int vrf_id, _alpm_cb_t *acb, uint8 fmt);
extern int alpm_util_ent_ent_get(int u, int vrf_id, _alpm_cb_t *acb, void *e,
                                 uint32 fmt, int eid, void *ftmp);
extern void alpm_util_pfx_cat(int u, int ipt, uint32 *pfx1, int len1,
                              uint32 *pfx2, int len2, uint32 *new_pfx,
                              int *new_len);
extern int alpm_util_pfx_len_cmp(void *a, void *b);
extern int alpm_util_bkt_pfx_get(int u, int vrf_id, _alpm_cb_t *acb, void *e,
                                 uint32 fmt, uint32 *new_key, int *new_len,
                                 uint32 *valid);
extern int alpm_util_bkt_adata_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb, void *ftmp, int fmt,
                                   _alpm_bkt_adata_t *adata1, int *arg1);
extern int alpm_util_ent_data_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb, void *ftmp,
                                  int fmt, void *fent1);
extern int alpm_util_ent_phy_idx_get(int u, _alpm_cb_t *acb,
                                     int vrf_id, int index);
extern int alpm_util_bkt_token_bnks(int u, _alpm_cb_t *acb, int vrf_id,
                                    _alpm_pvt_node_t *pvt_node);

extern void alpm_trace_print(int u, int showflags, _alpm_log_t *p, char *data);
extern int _tcam_entry_write(int u, int pkm, void *e, int index, int s_index);
extern int tcam_table_size(int u, int pkm);
extern int tcam_entry_vrf_id_get(int u, int pkm, void *e,
                                  int sub_idx, int *vrf_id);
extern int tcam_valid_entry_mode_get(int u, int pk, void *e, int *step_size,
                              int *pkm, int *ipv6, int *key_mode, int sub_idx);
extern int tcam_entry_valid(int u, int pkm, void *e, int sub_idx);
extern int tcam_entry_pfx_len_get(int u, int pkm, void *e,
                                   int sub_idx, int *pfx_len);
extern int tcam_entry_adata_get(int u, int pkm, void *e, int sub_idx,
                                _alpm_bkt_adata_t *adata);
extern int tcam_entry_bdata_get(int u, int pkm, void *e,
                                int sub_idx,  void *fent);
extern int tcam_entry_hit_get(int u, int pkm, void *e, int sub_dx);
extern int tcam_cache_hit_get(int u, int pkm, int tcam_idx);
extern int tcam_hitbit_sync_cache(int u);
extern int tcam_entry_to_cfg(int u, int pkm, void *e, int x,
                              _bcm_defip_cfg_t *lpm_cfg);
extern int tcam_entry_read(int u, int pkm, void *e, int index, int s_index);
extern int alpm_bkt_entry_read(int u, _alpm_tbl_t tbl, _alpm_cb_t *acb,
                               void *e, int index);
extern int tcam_entry_read_no_cache(int u, int pkm, void *e,
                                    int index, int s_index);
extern int alpm_bkt_entry_read_no_cache(int u, _alpm_tbl_t tbl,
                            _alpm_cb_t *acb, void *e, int index);
/* ALPM utils */
extern void * alpm_util_alloc(unsigned int sz, char *s);
extern void alpm_util_free(void *addr);
extern void alpm_util_mem_stat_get(uint32 *alloc_cnt, uint32 *free_cnt);
extern void alpm_util_mem_stat_clear(void);
extern int alpm_util_route_capacity_get(int u, soc_mem_t mem,
                                   int *max_entries, int *min_entries);
extern void
alpm_util_fmt_tbl_util(int unit, char *buf);
extern void
alpm_util_fmt_bkt_info(char *buf, int vrf_id, _alpm_cb_t *acb, _alpm_bkt_info_t *bkt_info);
extern void
alpm_util_fmt_ipaddr(char buf[SAL_IPADDR_STR_LEN], ip_addr_t ipaddr);
extern void
alpm_util_fmt_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr_t ipaddr);
extern void
alpm_trie_key_to_pfx(int u, int ipt, uint32 *key, uint32 len, uint32 *pfx);
extern void
alpm_trie_pfx_to_key(int u, int ipt, uint32 *pfx, uint32 len, uint32 *key);
extern void
alpm_util_key_to_cfg(int u, int ipt, uint32 *key, _bcm_defip_cfg_t *lpm_cfg);
extern void
alpm_util_len_to_mask(int ipt, uint32 len, uint32 *mask);
extern int
alpm_util_cfg_construct(int u, int vrf_id, int ipt, uint32 *key, int key_len,
                        _alpm_bkt_adata_t *adata, _bcm_defip_cfg_t *lpm_cfg);
extern int
alpm_util_key_to_str(int u, int pkm, char * ipstr, uint32 *key, uint32 key_len,
                     _alpm_bkt_node_t *bkt_node, _alpm_pvt_node_t *pvt_node);
extern int
alpm_util_trie_max_key_len(int u, int ipt);

extern int
alpm_util_trie_max_split_len(int u, int pkm);

extern void
alpm_util_trie_pfx_print(int u, int ipt, uint32 *pfx, uint32 len,
                         const char *str);

extern void
alpm_util_adata_cfg_to_trie(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            _alpm_bkt_adata_t *bkt_adata);

extern void
alpm_util_adata_trie_to_cfg(int unit, _alpm_bkt_adata_t *bkt_adata,
                            _bcm_defip_cfg_t *lpm_cfg);

extern void
alpm_util_adata_zero_cfg(int unit, _bcm_defip_cfg_t *lpm_cfg);

extern uint8
alpm_util_route_type_get(int u, _bcm_defip_cfg_t *lpm_cfg);

extern int
alpm_util_route_type_check(int u, _bcm_defip_cfg_t *lpm_cfg);

extern int
alpm_util_def_check(int u, _bcm_defip_cfg_t *lpm_cfg, int is_add);

extern int
alpm_util_is_my_pivot(int u, uint32 *trie_key, uint32 trie_len,
                      uint32 *pvt_key, uint32 pvt_len);

extern void
alpm_util_cfg_to_key(int u, int ipt, _bcm_defip_cfg_t *lpm_cfg, uint32 *key);

extern void
alpm_util_cfg_to_msk(int u, int ipt, _bcm_defip_cfg_t *lpm_cfg, uint32 *msk);

extern void
alpm_util_ipmask_apply(int u, _bcm_defip_cfg_t *lpm_cfg);

extern void
alpm_util_bnk_range_print(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                          int bnk_per_row, int start, int end);

extern int
alpm_util_pvt_idx_get(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, int *idx, int *sub_idx);

#endif /* ALPM_ENABLE */

#endif /* _BCM_INT_ALPM_UTIL_H_ */

