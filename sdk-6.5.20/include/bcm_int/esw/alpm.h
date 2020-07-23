/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alpm.h
 * Purpose:     Function declarations for ALPM Internal functions.
 */

#ifndef _BCM_INT_ALPM_H_
#define _BCM_INT_ALPM_H_

#ifdef ALPM_ENABLE

/* SOC_ALPM_LOOKUP_HIT */
#define BCM_ALPM_LOOKUP_HIT             (1 << 31)
/* SOC_ALPM_COOKIE_FLEX */
#define ALPM_ROUTE_FULL_MD              (1 << 29)
/* SOC_ALPM_LPM_LOOKUP_HIT */
#define BCM_ALPM_LPM_LOOKUP_HIT         (1 << 28)

/* ref: esw/dump.c
 #define DUMP_TABLE_RAW       0x01
 #define DUMP_TABLE_HEX       0x02
 #define DUMP_TABLE_ALL       0x04
 #define DUMP_TABLE_CHANGED   0x08
 #define DUMP_TABLE_EXT       0x10
 #define DUMP_TABLE_PIPE_X    0x20
 #define DUMP_TABLE_PIPE_Y    0x40
 #define DUMP_TABLE_PIPE_0    0x80
 #define DUMP_TABLE_PIPE_1    0x100
 #define DUMP_TABLE_PIPE_2    0x200
 #define DUMP_TABLE_PIPE_3    0x400
 #define DUMP_TABLE_NO_CACHE  0x800
 #define DUMP_TABLE_PHY_INDEX 0x1000
 #define DUMP_TABLE_SIMPLIFIED 0x2000
 #define DUMP_TABLE_FORMAT 0x4000
 #define DUMP_TABLE_ALL_DMA_THRESHOLD 8
 #define DUMP_TABLE_ITER_BEFORE_YIELD 8
 */
#define ALPM_DUMP_TABLE_RAW          0x01
#define ALPM_DUMP_TABLE_HEX          0x02
#define ALPM_DUMP_TABLE_ALL          0x04
#define ALPM_DUMP_TABLE_CHANGED      0x08
#define ALPM_DUMP_TABLE_NOCACHE      0x800

/* Max entry per bank */
#define BCM_ESW_ALPM_EPB_MAX        40

/* ALPM UnitTest definitions */
#define ALPM_UT_FLAG_IP_INCR    (1U << 1)
#define ALPM_UT_FLAG_LEN_INCR   (1U << 2)
#define ALPM_UT_FLAG_VRF_INCR   (1U << 3)
#define ALPM_UT_FLAG_SPEED      (1U << 4)
#define ALPM_UT_FLAG_NOMSG      (1U << 5)

#define ALPM_UT_V4 (1 << 0)
#define ALPM_UT_V6 (1 << 1)

#define ALPM_UT_OPC_ADD     0
#define ALPM_UT_OPC_DEL     1

typedef struct alpm_ut_route_db_s {
    int         type;
    int         vrf;
    char        *base_addr;
    uint32      pfx_len;
    int         count;
    int         opc;
} alpm_ut_route_db_t;

typedef enum _alpm_vrf_type_e {
    VT_PRIVATE = 0,
    VT_GLOBAL = 1,
    VT_OVERRIDE = 2,
    VTS = 3
} _alpm_vrf_type_t;

typedef enum _alpm_ip_type_e {
    IPV4 = 0,
    IPV6 = 1,
    IPS = 2
} _alpm_ip_type_t;

/* ALPM resource info structure */
typedef struct _alpm_resource_info_s {
    bcm_l3_alpm_resource_t res[bcmL3RouteGroupCounter];
} _alpm_resource_info_t;


/* ESW externs */
extern int bcm_esw_alpm_init(int u);
extern int bcm_esw_alpm_inited(int u);
extern int bcm_esw_alpm_cleanup(int u, _bcm_l3_trvrs_data_t *trv_data,
                                _bcm_l3_trvrs_data_t *trv_data1);
extern int bcm_esw_alpm_fast_delete_all(int u, _bcm_l3_trvrs_data_t *trv_data,
                                        _bcm_l3_trvrs_data_t *trv_data1);
extern int bcm_esw_alpm_lookup(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_insert(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_add(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_delete(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_update(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_find(int u, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_esw_alpm_update_match(int u, _bcm_l3_trvrs_data_t *trv_data);
extern int bcm_esw_alpm_vrf_data_mode_get(int u, bcm_vrf_t vrf, uint32 flags,
                                          bcm_l3_vrf_route_data_mode_t *mode);
extern int bcm_esw_alpm_vrf_data_mode_set(int u, bcm_vrf_t vrf, uint32 flags,
                                          bcm_l3_vrf_route_data_mode_t mode);
extern int bcm_esw_alpm_resource_check_and_free(int u, _bcm_defip_cfg_t *lpm_cfg);

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int bcm_th3_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            int nh_ecmp_idx);
extern int bcm_th3_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_th3_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                            *nh_ecmp_idx);
extern int bcm_th3_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_th3_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int bcm_th_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int nh_ecmp_idx);
extern int bcm_th_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_th_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                            *nh_ecmp_idx);
extern int bcm_th_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
#endif

/* Extern functions of alpm_tcam.c */
extern int bcm_esw_alpm_tcam_insert(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 write_flags);
extern int bcm_esw_alpm_tcam_match(int u, _bcm_defip_cfg_t *lpm_cfg, int *idx);
extern int bcm_esw_alpm_tcam_delete(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_tcam_init(int u);
extern int bcm_esw_alpm_tcam_deinit(int u);
extern int bcm_esw_alpm_tcam_cleanup(int u);
extern int bcm_esw_alpm_tcam_state_free_get(int, int app, int zn, int *fcnt, int *vcnt);
extern void bcm_esw_alpm_tcam_zone_dump(int u, int app, char *pfx_str);
extern void bcm_esw_alpm_tcam_zone_state_dump(int u, int app);
extern int bcm_esw_alpm_tcam_avail(int u, int app, int vrf_id, int ipt, int key_len, int mc);

/* APIs distributed hitbits */
extern int bcm_esw_alpm_hit_init(int u);
extern int bcm_esw_alpm_hit_deinit(int u);
extern int bcm_esw_alpm_clear_hit(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_hit_enabled(int u);
extern int bcm_esw_alpm_hit_enable(int unit, int enable);
extern int alpm_dist_hitbit_interval_get(int u);
extern int alpm_dist_hitbit_enable_set(int u, int us);

/* Extern fucntions for alpm_tr.c */
typedef int (*bcm_esw_alpm_trace_dump_cb)(int u, void *user_data, char *log_data);

extern int bcm_esw_alpm_trace_enabled(int u);
extern int bcm_esw_alpm_trace_dump(int u, int showflags, bcm_esw_alpm_trace_dump_cb dump_cb, void *user_data);
extern int bcm_esw_alpm_trace_cnt(int u);
extern int bcm_esw_alpm_trace_seq(int u);
extern int bcm_esw_alpm_trace_delete(int u);
extern void bcm_esw_alpm_trace_delete_clear(int u);
extern int bcm_esw_alpm_trace_init(int u);
extern int bcm_esw_alpm_trace_deinit(int u);
extern int bcm_esw_alpm_trace_set(int u, int val);
extern int bcm_esw_alpm_trace_clear(int u);
extern int bcm_esw_alpm_trace_sanity(int u, int enable, int start, int end);
extern int bcm_esw_alpm_trace_wrap_set(int u, int val);
extern int bcm_esw_alpm_trace_log(int u, int is_del, _bcm_defip_cfg_t *cfg, int nh_idx, int rc);

/* Extern functions for alpm_ts.c */
typedef int (*bcm_esw_alpm_data_cb)(int u, void *fp, char *data, int num);

extern int bcm_esw_alpm_data_save(int u, bcm_esw_alpm_data_cb write_cb, void *fp);
extern int bcm_esw_alpm_data_restore(int u, bcm_esw_alpm_data_cb read_cb, void *fp);
extern void bcm_esw_alpm_sw_dump(int u);
extern int bcm_esw_alpm_debug_show(int u, int app, int vrf_id, uint32 flags);
extern int bcm_esw_alpm_debug_brief_show(int u, int app, int bucket, int index);
extern int bcm_esw_alpm_sanity_check(int u, int app, soc_mem_t mem, int index,
                                     int check_sw);

/* External definitions for dbg_info */
#define _ALPM_DBG_INFO_PVT  (1 << 0)
#define _ALPM_DBG_INFO_RTE  (1 << 1)
#define _ALPM_DBG_INFO_CNT  (1 << 2)
#define _ALPM_DBG_INFO_HIT  (1 << 3)
#define _ALPM_DBG_INFO_MAX_NUM    4
#define _ALPM_DBG_INFO_ALL  0xffffffff

extern int alpm_dbg_info_bmp_show(int u);
extern int alpm_dbg_info_bmp_set(int u, uint32 flags);
extern int alpm_dbg_bmp_set(int u, uint32 dbg_bmp);

/* External definitions for dump functions */
extern int alpm_cb_pvt_dump(int u, int app, int acb_bmp, int bkt_idx);
extern int alpm_cb_fmt_dump(int u, int app, int acb_bmp);
extern int alpm_cb_pvtlen_dump(int u, int app, int acb_bmp);
extern int alpm_cb_bkt_dump(int u, int app, int acb_bmp);
extern int alpm_cb_stat_dump(int u, int app, int acb_bmp);
extern int alpm_cb_conf_dump(int u, int app, int acb_bmp);
extern int alpm_trie_show(int u, int app, bcm_vrf_t vrf,
                          bcm_l3_alpm_ipt_t ipt,
                          int trie_type, int acb_bmp);

/* Other Externs */
extern int bcm_esw_alpm_route_capacity_get(int u, soc_mem_t mem,
                                           int *max_entries, int *min_entries);
extern void bcm_esw_bkt_node_flexctr_attr_get(uint32 *bnode, int *base_idx, int *ofst_mod, int *pool_num);
extern void bcm_esw_bkt_node_flexctr_attr_set(uint32 *bnode, int base_idx, int ofst_mod, int pool_num);
extern void th_alpm_table_ent_dump(int u, int app, int vrf_id, soc_mem_t mem, void *entry,
                int copyno, int index, int fmt,
                int ent_idx, int flags, char *pfx);
extern void alpm_util_fmt_tbl_util(int unit, char *buf);
extern void alpm_util_fmt_ipaddr(char buf[SAL_IPADDR_STR_LEN], ip_addr_t ipaddr);
extern void alpm_util_fmt_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr_t ipaddr);

extern int  alpm_table_dump(int u, int app, int vrf_id, soc_mem_t mem, int copyno, int index,
                            int fmt, int ent_idx, int flags);

extern int bcm_esw_alpm_resource_get(int u, bcm_l3_route_group_t grp, bcm_l3_alpm_resource_t *res);
extern int alpm_resource_info_get(int u, _alpm_resource_info_t *info);

extern int alpm_resource_usage_dump(int u);
extern int alpm_resource_usage_show(int u);
extern int alpm_route_projection(int u, int v4v6_ratio);

extern int bcm_esw_alpm_route_grp_projection(int u, bcm_l3_route_group_t grp,
                                             _alpm_resource_info_t *info,
                                             int *prj_route, int *def_lvl, int *def_usage);
extern int bcm_esw_alpm_route_grp_testdb(int u, bcm_l3_route_group_t grp,
                                         _alpm_resource_info_t *info,
                                         int *max_route, int *mix_route, int v4v6_ratio,
                                         int *def_lvl, int *def_usage);

extern int bcm_esw_alpm_trace_cb_register(int unit,
                                          bcm_l3_alpm_trace_cb_f write_cb,
                                          void *user_data);
extern int bcm_esw_alpm_trace_cb_unregister(int unit,
                                            bcm_l3_alpm_trace_cb_f write_cb);

extern int bcm_esw_alpm_defrag(int u,
                               bcm_vrf_t vrf,
                               bcm_l3_alpm_ipt_t ipt,
                               int max_time_ms,
                               int max_l2_merge_cnt,
                               int max_l3_merge_cnt,
                               int *l2_merge_cnt,
                               int *l3_merge_cnt);

#endif /* ALPM_ENABLE */

#endif /* _BCM_INT_ALPM_H_ */

