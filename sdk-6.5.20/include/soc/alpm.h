/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * trie data structure for ALPM
 */

#define ALPM_IPV4_BKT_COUNT             24
#define ALPM_IPV4_BKT_COUNT_FLEX        16
#define ALPM_IPV6_64_BKT_COUNT          16
#define ALPM_IPV6_64_BKT_COUNT_FLEX     12
#define ALPM_IPV6_128_BKT_COUNT         8
#define ALPM_RAW_BKT_COUNT_DW           8   /* Double Wide */
#define ALPM_RAW_BKT_COUNT              4

#define SOC_ALPM_V6_SCALE_CHECK(u, v6) \
    ((v6) \
     && !soc_alpm_mode_get((u)) && !SOC_URPF_STATUS_GET((u)))

#define SOC_ALPM_RPF_SRC_DISCARD        (1)
#define SOC_ALPM_STAT_FLEX              (1 << 1)


#define SOC_ALPM_LOOKUP_HIT             (1 << 31)
#define SOC_ALPM_DELETE_ALL             (1 << 30)
#define SOC_ALPM_COOKIE_FLEX            (1 << 29)
#define SOC_ALPM_LPM_LOOKUP_HIT         (1 << 28)

#define SOC_ALPM_TCAM_INDEX_MASK        (~(SOC_ALPM_LOOKUP_HIT |\
                                           SOC_ALPM_DELETE_ALL |\
                                           SOC_ALPM_COOKIE_FLEX))

#define SOC_ALPM_BKT_ENTRY_TO_IDX(idx)  (((idx) >> 2) & 0x3FFF)
/* given bucket index shift left to bucket entry idx position */
#define SOC_ALPM_BKT_IDX_TO_ENTRY(idx)  ((idx) << 2)

/* size of individual l3 defip tcam in no. of entries */
#define SOC_L3_DEFIP_TCAM_SIZE(u)       SOC_CONTROL(u)->l3_defip_tcam_size

/* size of individual l3_defip tcam in no. of bits to represent size */
#define SOC_L3_DEFIP_TCAM_SIZE_SHIFT    10

/* for TD2 this is: ((addr << 1) & ~0x7ff) + (addr & 0x3ff) */
#define SOC_ALPM_128_ADDR_LWR(u, addr) \
    ((((addr) << 1) & ~(2*SOC_L3_DEFIP_TCAM_SIZE(u) - 1)) + \
    ((addr) & (SOC_L3_DEFIP_TCAM_SIZE(u) - 1)))

#define SOC_ALPM_128_ADDR_UPR(u, addr) \
    (SOC_ALPM_128_ADDR_LWR((u), (addr)) + SOC_L3_DEFIP_TCAM_SIZE(u))

/* get pair tcam index given underlying defip index */
#define SOC_ALPM_128_DEFIP_TO_PAIR(u, idx) \
    (((idx) & (SOC_L3_DEFIP_TCAM_SIZE(u) - 1)) + \
    (((idx) >> 1) & ~(SOC_L3_DEFIP_TCAM_SIZE(u) - 1)))

#define SOC_ALPM_RAW_BUCKET_SIZE(u, v6)  \
    (SOC_ALPM_V6_SCALE_CHECK((u), (v6))? \
     ALPM_RAW_BKT_COUNT_DW :             \
     ALPM_RAW_BKT_COUNT)

#define SOC_ALPM_RAW_INDEX_DECODE(u, v6, alpm_index, raw_id, ent_id)\
do {                                                                \
    (raw_id) = (alpm_index) % SOC_ALPM_RAW_BUCKET_SIZE((u), (v6));  \
    (ent_id) = (alpm_index) >> 16;                                  \
} while (0)

/* for ALPM debug show */
#define SOC_ALPM_DEBUG_SHOW_FLAG_PVT    (1U << 0)
#define SOC_ALPM_DEBUG_SHOW_FLAG_BKT    (1U << 1)
#define SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG (1U << 2)
#define SOC_ALPM_DEBUG_SHOW_FLAG_CNT    (1U << 3)
#define SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG (1U << 4)
#define SOC_ALPM_DEBUG_SHOW_FLAG_TRIE_TREE (1U << 5)
#define SOC_ALPM_DEBUG_SHOW_FLAG_ALL    (~0U)

extern int soc_alpm_init(int u);
extern int soc_alpm_deinit(int u);
extern int soc_alpm_insert(int u, void *entry_data, uint32 src_flags, 
                           int bkt_idx, int pivot_info); 
extern int soc_alpm_warmboot_lpm_reinit(int, int, int, void *);
extern int soc_alpm_warmboot_lpm_reinit_done(int);
extern int soc_alpm_warmboot_bucket_bitmap_set(int, int, int);
extern int soc_alpm_warmboot_pivot_add(int, int, void *, int, int);
extern int soc_alpm_warmboot_prefix_insert(int, int, int, void *, void *,
                                           int, int, int);
extern int soc_alpm_128_warmboot_lpm_reinit(int, int, int, void *);
extern int soc_alpm_128_warmboot_lpm_reinit_done(int);
extern int soc_alpm_128_warmboot_bucket_bitmap_set(int, int, int);
extern int soc_alpm_128_warmboot_pivot_add(int, int, void *, int, int);
extern int soc_alpm_128_warmboot_prefix_insert(int, int, void *, void *,
                                               int, int, int);
extern int soc_alpm_debug_show(int u, int vrf_id, uint32 flags);
extern int soc_alpm_debug_brief_show(int u, int phy_bkt, int index);

#ifdef ALPM_WARM_BOOT_DEBUG
extern void soc_alpm_lpm_sw_dump(int);
extern void soc_alpm_128_warmboot_lpm_sw_dump(int);
#endif
extern int soc_alpm_delete(int u, void *entry_data, int bkt_idx, 
                           int pivot_info); 
extern int soc_alpm_lookup(int u, void *key_data,
                           void *e, /* return entry data if found */
                           int *index_ptr, int *cookie);
extern int soc_alpm_find_best_match(int u, void *key_data, void *e, 
                                    int *index_ptr, int do_urpf);
extern int soc_alpm_128_insert(int u, void *entry_data, uint32 src_flags,
                               int bkt_idx, int pivot_info); 
extern int soc_alpm_128_delete(int u, void *entry_data, int bkt_idx, 
                               int pivot_info); 
extern int soc_alpm_128_lookup(int u, void *key_data,
                               void *e, /* return entry data if found */
                               int *index_ptr, int *cookie);
extern int soc_alpm_128_find_best_match(int u, void *key_data, void *e,    
                                        int *index_ptr, int do_urpf);
extern int soc_alpm_urpf_enable(int unit, int enable);
extern int soc_alpm_ipmc_war(int unit, int install);
extern int soc_alpm_128_ipmc_war(int unit, int install);

extern int soc_alpm_lpm_ip4entry0_to_0(int u, void *src, void *dst, int copy_hit);
extern int soc_alpm_lpm_ip4entry1_to_1(int u, void *src, void *dst, int copy_hit);
extern int soc_alpm_lpm_ip4entry0_to_1(int u, void *src, void *dst, int copy_hit);
extern int soc_alpm_lpm_ip4entry1_to_0(int u, void *src, void *dst, int copy_hit);
extern int soc_alpm_lpm_vrf_get(int unit, void *lpm_entry, int *vrf_id, int *vrf);
extern int soc_alpm_128_lpm_vrf_get(int unit, void *lpm_entry, int *vrf_id, int *vrf);
extern int soc_alpm_mode_get(int u);

/* For Tomahawk */
#define SOC_TH_ALPM_SCALE_CHECK(u, v6) \
    ((l3_alpm_ipv4_double_wide[(u)] || (v6)) && \
    ((ALPM_CTRL(u).alpm_mode != SOC_ALPM_MODE_PARALLEL) && !SOC_URPF_STATUS_GET((u))))

extern int soc_th_alpm_warmboot_lpm_reinit(int, int, int, void *);
extern int soc_th_alpm_warmboot_lpm_reinit_done(int);
extern int soc_th_alpm_warmboot_bucket_bitmap_set(int, int, int, int);
extern int soc_th_alpm_warmboot_pivot_add(int, int, void *, int, int);
extern int soc_th_alpm_warmboot_pivot_bmp_len_update(int unit, int ipv6,
                                                     void *lpm_entry, int tcam_idx);
extern int soc_th_alpm_warmboot_prefix_insert(int, int, int, void *, void *,
                                              int, int, int);
extern int soc_th_alpm_128_warmboot_lpm_reinit(int, int, int, void *);
extern int soc_th_alpm_128_warmboot_lpm_reinit_done(int);
extern int soc_th_alpm_128_warmboot_bucket_bitmap_set(int, int, int, int);
extern int soc_th_alpm_128_warmboot_pivot_add(int, int, void *, int, int);
extern int soc_th_alpm_128_warmboot_pivot_bmp_len_update(int unit, int ipv6,
                                                         void *lpm_entry, int tcam_idx);
extern int soc_th_alpm_128_warmboot_prefix_insert(int, int, void *, void *,
                                                  int, int, int);
extern int soc_th_alpm_delete(int u, void *entry_data, int bkt_idx, 
                              int pivot_info); 
extern int soc_th_alpm_lookup(int u, void *key_data,
                              void *e, /* return entry data if found */
                              int *index_ptr, int *cookie);
extern int soc_th_alpm_128_insert(int u, void *entry_data, uint32 src_flags,
                                  int bkt_idx, int pivot_info); 
extern int soc_th_alpm_128_delete(int u, void *entry_data, int bkt_idx, 
                                  int pivot_info); 
extern int soc_th_alpm_128_lookup(int u, void *key_data,
                                  void *e, /* return entry data if found */
                                  int *index_ptr, int *cookie);
extern int soc_th_alpm_urpf_enable(int unit, int enable);

extern int soc_th_alpm_lpm_vrf_get(int unit, void *lpm_entry, int *vrf_id, int *vrf);
extern int soc_th_alpm_128_lpm_vrf_get(int unit, void *lpm_entry, int *vrf_id, int *vrf);
extern int soc_th_alpm_init(int u);
extern int soc_th_alpm_deinit(int u);
extern int soc_th_alpm_insert(int u, void *entry_data, uint32 src_flags, 
                              int bkt_idx, int pivot_info); 
extern int soc_th_alpm_lpm_ip4entry0_to_0(int u, void *src, 
                                          void *dst, int copy_hit);
extern int soc_th_alpm_lpm_ip4entry1_to_1(int u, void *src, void *dst, 
                                          int copy_hit);
extern int soc_th_alpm_lpm_ip4entry0_to_1(int u, void *src, void *dst, 
                                          int copy_hit);
extern int soc_th_alpm_lpm_ip4entry1_to_0(int u, void *src, void *dst, 
                                          int copy_hit);
extern void soc_th_alpm_lpm_state_dump(int u);
extern int soc_th_alpm_debug_show(int u, int vrf_id, uint32 flags);
extern int soc_th_alpm_debug_brief_show(int u, int bucket, int index);

extern int soc_th_alpm_find_best_match(int u, void *key_data, void *e, 
                                       int *index_ptr, int do_urpf);
extern int soc_th_alpm_128_find_best_match(int u, void *key_data, void *e,    
                                           int *index_ptr, int do_urpf);
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void soc_th_alpm_sw_dump(int unit);
extern void soc_alpm_sw_dump(int unit);
#endif

/* ALPM Sanity Checker */
extern int soc_alpm_sanity_check(int u, soc_mem_t mem, int index, int check_sw);
extern int soc_th_alpm_sanity_check(int u, soc_mem_t mem, int index, int check_sw);

extern int soc_alpm_ipmc_war(int unit, int install); 
extern int soc_alpm_128_ipmc_war(int unit, int install);

/* ALPM common */
extern int soc_alpm_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_alpm_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern soc_mem_t soc_alpm_cmn_bkt_view_get(int unit, int index);
extern void soc_alpm_cmn_bkt_view_set(int unit, int index, soc_mem_t view);
extern int soc_alpm_cmn_banks_get(int unit);
extern int soc_alpm_cmn_mode_get(int unit);

/* ALPM Capacity get*/
extern int soc_alpm_route_capacity_get(int unit, int route_type,
                                       int *max_entries, int *min_entries);
extern int soc_th_alpm_route_capacity_get(int unit, int route_type,
                                          int *max_entries, int *min_entries);


