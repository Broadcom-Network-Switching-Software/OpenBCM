/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _LPM_H_
#define _LPM_H_
#include <shared/l3.h>

/* 
 *  Reserved VRF values 
 */
#define SOC_L3_VRF_OVERRIDE  _SHR_L3_VRF_OVERRIDE /* Matches before vrf */
                                                  /* specific entries.  */
#define SOC_L3_VRF_GLOBAL    _SHR_L3_VRF_GLOBAL   /* Matches after vrf  */
                                                  /* specific entries.  */
#define SOC_L3_VRF_DEFAULT   _SHR_L3_VRF_DEFAULT  /* Default vrf id.    */ 

#define SOC_APOLLO_L3_DEFIP_URPF_SIZE (0x800)
#define SOC_APOLLO_B0_L3_DEFIP_URPF_SIZE (0xC00)

#define SOC_LPM_LOCK(u)             soc_mem_lock(u, ((SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m))?L3_DEFIP_LEVEL1m:L3_DEFIPm))
#define SOC_LPM_UNLOCK(u)           soc_mem_unlock(u, ((SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m))?L3_DEFIP_LEVEL1m:L3_DEFIPm))

#define PRESERVE_HIT                TRUE 

#if defined(BCM_HELIX4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
#define FB_LPM_HASH_INDEX_NULL  (0x3FFFF)
#define FB_LPM_HASH_INDEX_MASK  (0x1FFFF)
#define FB_LPM_HASH_IPV6_MASK   (0x20000)
#else
#define FB_LPM_HASH_INDEX_NULL  (0xFFFF)
#define FB_LPM_HASH_INDEX_MASK  (0x7FFF)
#define FB_LPM_HASH_IPV6_MASK   (0x8000)
#endif

#define _SOC_HASH_L3_VRF_GLOBAL   0
#define _SOC_HASH_L3_VRF_OVERRIDE 1
#define _SOC_HASH_L3_VRF_DEFAULT  2
#define _SOC_HASH_L3_VRF_SPECIFIC 3

#ifdef BCM_FIREBOLT_SUPPORT
#define IPV6_PFX_ZERO                33
#define MAX_LPM_ZONE_IPV4(u)         (soc_feature(u, soc_feature_td3_lpm_ipmc_war) ? 6 : 3)
#define MAX_LPM_ZONE(u)              (MAX_LPM_ZONE_IPV4(u) * 2)
#define IPV4_PFX_ZERO(u)             (MAX_LPM_ZONE_IPV4(u) * (64 + 32 + 2 + 1))
#define MAX_PFX_ENTRIES(u)           (2 * (IPV4_PFX_ZERO(u)))
#define MAX_PFX_INDEX(u)             (MAX_PFX_ENTRIES(u) - 1)
#define SOC_LPM_INIT_CHECK(u)        (soc_lpm_state[(u)] != NULL)
#define SOC_LPM_STATE(u)             (soc_lpm_state[(u)])
#define SOC_LPM_STATE_START(u, pfx)  (soc_lpm_state[(u)][(pfx)].start)
#define SOC_LPM_STATE_END(u, pfx)    (soc_lpm_state[(u)][(pfx)].end)
#define SOC_LPM_STATE_PREV(u, pfx)  (soc_lpm_state[(u)][(pfx)].prev)
#define SOC_LPM_STATE_NEXT(u, pfx)  (soc_lpm_state[(u)][(pfx)].next)
#define SOC_LPM_STATE_VENT(u, pfx)  (soc_lpm_state[(u)][(pfx)].vent)
#define SOC_LPM_STATE_FENT(u, pfx)  (soc_lpm_state[(u)][(pfx)].fent)
#define SOC_LPM_STATE_HFENT(u, pfx) (soc_lpm_state[(u)][(pfx)].hfent)
#define SOC_LPM_PFX_IS_V4(u, pfx)   ((soc_feature(u, soc_feature_l3_shared_defip_table) || \
                                      soc_feature(u, soc_feature_l3_kt2_shared_defip_table)) ? \
                                     ((pfx) >= IPV4_PFX_ZERO(u) ? TRUE : FALSE) : \
                                     ((((pfx) % (MAX_PFX_ENTRIES(u) / MAX_LPM_ZONE(u))) < IPV6_PFX_ZERO) ?  TRUE : FALSE))
#define SOC_LPM_PFX_IS_V6_64(u, pfx) ((soc_feature(u, soc_feature_l3_shared_defip_table) || \
                                       soc_feature(u, soc_feature_l3_kt2_shared_defip_table)) ? \
                                      ((pfx) < IPV4_PFX_ZERO(u)  ? TRUE : FALSE) : \
                                      ((((pfx) % (MAX_PFX_ENTRIES(u) / MAX_LPM_ZONE(u))) >= IPV6_PFX_ZERO) ?  TRUE : FALSE))

#define SOC_LPM128_PFX_IS_V6_128(u, pfx) \
    ((pfx) >= (MAX_PFX_64_OFFSET + MAX_PFX_32_OFFSET) && ((pfx) < MAX_PFX128_INDEX))
#define SOC_LPM128_PFX_IS_V6_64(u, pfx) \
    ((pfx) >= MAX_PFX_32_OFFSET && (pfx) < (MAX_PFX_64_OFFSET + MAX_PFX_32_OFFSET))
#define SOC_LPM128_PFX_IS_V4(u, pfx) ((pfx) < MAX_PFX_32_OFFSET && (pfx) >= 0)
#define SOC_LPM128_PFX_IS_V6(u, pfx) (!SOC_LPM128_PFX_IS_V4(u, (pfx))) && ((pfx) < MAX_PFX128_INDEX)
#define SOC_LPM_STAT_INIT_CHECK(u) (soc_lpm_stat[u] != NULL) 
#define SOC_LPM_V4_COUNT(u) (soc_lpm_stat[u]->used_v4_count)
#define SOC_LPM_64BV6_COUNT(u) (soc_lpm_stat[u]->used_64b_count)
#define SOC_LPM_128BV6_COUNT(u) (soc_lpm_stat[u]->used_128b_count)
#define SOC_LPM_MAX_V4_COUNT(u) (soc_lpm_stat[u]->max_v4_count)
#define SOC_LPM_MAX_64BV6_COUNT(u) (soc_lpm_stat[u]->max_64b_count)
#define SOC_LPM_MAX_128BV6_COUNT(u) (soc_lpm_stat[u]->max_128b_count)
#define SOC_LPM_V4_HALF_ENTRY_COUNT(u) (soc_lpm_stat[u]->half_entry_count)
#define SOC_LPM_COUNT_INC(val) (val)++
#define SOC_LPM_COUNT_DEC(val) (val)--
#ifdef FB_LPM_DEBUG
#define SOC_LPM_V4_COUNT2(u) (soc_lpm_stat[u]->used_v4_count2)
#define SOC_LPM_64BV6_COUNT2(u) (soc_lpm_stat[u]->used_64b_count2)
#define SOC_LPM_V4_HALF_ENTRY_COUNT2(u) (soc_lpm_stat[u]->half_entry_count2)
#define SOC_LPM_V4_COUNT1(u) (soc_lpm_stat[u]->used_v4_count1)
#define SOC_LPM_64BV6_COUNT1(u) (soc_lpm_stat[u]->used_64b_count1)
#define SOC_LPM_V4_HALF_ENTRY_COUNT1(u) (soc_lpm_stat[u]->half_entry_count1)
#endif

#define SOC_EXT_LPM_STAT_INIT_CHECK(u) (soc_ext_lpm_stat[u] != NULL)
#define SOC_EXT_LPM_V4_COUNT(u) (soc_ext_lpm_stat[u]->used_v4_count)
#define SOC_EXT_LPM_64BV6_COUNT(u) (soc_ext_lpm_stat[u]->used_64b_count)
#define SOC_EXT_LPM_128BV6_COUNT(u) (soc_ext_lpm_stat[u]->used_128b_count)
#define SOC_EXT_LPM_MAX_V4_COUNT(u) (soc_ext_lpm_stat[u]->max_v4_count)
#define SOC_EXT_LPM_MAX_64BV6_COUNT(u) (soc_ext_lpm_stat[u]->max_64b_count)
#define SOC_EXT_LPM_MAX_128BV6_COUNT(u) (soc_ext_lpm_stat[u]->max_128b_count)
#define SOC_EXT_LPM_COUNT_INC(val) (val)++
#define SOC_EXT_LPM_COUNT_DEC(val) (val)--

typedef struct soc_lpm_stat_s {
    uint32 used_v4_count;
    uint16 used_64b_count;
    uint16 used_128b_count;
    uint32 max_v4_count;
    uint16 max_64b_count;
    uint16 max_128b_count;
    uint16 half_entry_count;
#ifdef FB_LPM_DEBUG
    uint16 used_v4_count2;
    uint16 used_64b_count2;
    uint16 used_128b_count2;
    uint16 half_entry_count2;
    uint16 used_v4_count1;
    uint16 used_64b_count1;
    uint16 used_128b_count1;
    uint16 half_entry_count1;
#endif
} soc_lpm_stat_t;
typedef soc_lpm_stat_t *soc_lpm_stat_p;

typedef struct soc_lpm_state_s {
    int start;   /* start index for this prefix length */
    int end;     /* End index for this prefix length */
    int prev;    /* Previous (Lo to Hi) prefix length with non zero entry count*/
    int next;    /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;    /* valid entries */
    int fent;    /* free entries */
    uint8 hfent; /* Indicates Half free entry available for v4 entries. */
} soc_lpm_state_t;
typedef  soc_lpm_state_t *soc_lpm_state_p;

typedef struct soc_ext_lpm_stat_s {
    uint16 used_v4_count;
    uint16 used_64b_count;
    uint16 used_128b_count;
    uint16 max_v4_count;
    uint16 max_64b_count;
    uint16 max_128b_count;
} soc_ext_lpm_stat_t;
typedef soc_ext_lpm_stat_t *soc_ext_lpm_stat_p;

/*
 * The idea of these offsets is to create unique space for each prefix type
 * The precedence is as follows
 * 128BV6 > 64B V6 > V4 irrespective of whether the entry is public or private
 * i.e any prefix of V4 is lesser than min prefix of 64B V6 and any prefix of
 * 64B V6 is lesser than 128B V6 from software perspective 
 */
#if defined(BCM_TRIDENT3_SUPPORT)
#define MAX_PFX_128_OFFSET              (6 * 129)
#define MAX_PFX_64_OFFSET               (6 * 65)
#define MAX_PFX_32_OFFSET               (6 * 33)
#else
#define MAX_PFX_128_OFFSET              (3 * 129)
#define MAX_PFX_64_OFFSET               (3 * 65)
#define MAX_PFX_32_OFFSET               (3 * 33)
#endif
#define MAX_PFX128_ENTRIES              (MAX_PFX_128_OFFSET + \
                                         MAX_PFX_64_OFFSET + \
                                         MAX_PFX_32_OFFSET + 2)
#define MAX_PFX128_INDEX                (MAX_PFX128_ENTRIES - 1)
#define SOC_LPM128_STATE_TABLE(u)       (soc_lpm128_state_table[u])
#define SOC_LPM128_STATE_TABLE_INIT_CHECK(u) \
        (soc_lpm128_state_table[(u)] != NULL)
#define SOC_LPM128_STATE_TABLE_TOTAL_COUNT(u) \
        (soc_lpm128_state_table[u]->total_count)

#define SOC_LPM128_INIT_CHECK(u)        \
        (soc_lpm128_state_table[(u)]->soc_lpm_state != NULL)
#define SOC_LPM128_STATE(u)             \
        (soc_lpm128_state_table[(u)]->soc_lpm_state)
#define SOC_LPM128_STATE_START1(u, lpm_state, pfx) (lpm_state[pfx].start1)
#define SOC_LPM128_STATE_START2(u, lpm_state, pfx) (lpm_state[pfx].start2)
#define SOC_LPM128_STATE_END1(u, lpm_state, pfx) (lpm_state[pfx].end1)
#define SOC_LPM128_STATE_END2(u, lpm_state, pfx) (lpm_state[pfx].end2)
#define SOC_LPM128_STATE_PREV(u, lpm_state, pfx) (lpm_state[pfx].prev)
#define SOC_LPM128_STATE_NEXT(u, lpm_state, pfx) (lpm_state[pfx].next)
#define SOC_LPM128_STATE_VENT(u, lpm_state, pfx) (lpm_state[pfx].vent)
#define SOC_LPM128_STATE_FENT(u, lpm_state, pfx) (lpm_state[pfx].fent)
#define SOC_LPM128_STATE_HFENT(u, lpm_state, pfx) (lpm_state[pfx].hfent)

#define SOC_LPM128_UNRESERVED_INIT_CHECK(u)        \
        (soc_lpm128_state_table[(u)]->soc_lpm_unreserved_state != NULL)
#define SOC_LPM128_UNRESERVED_STATE(u)             \
        (soc_lpm128_state_table[(u)]->soc_lpm_unreserved_state)

#define SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) \
        soc_lpm128_index_to_pfx_group[u]
#define SOC_LPM128_INDEX_TO_PFX_GROUP(u, index) \
        soc_lpm128_index_to_pfx_group[u][index]

#define SOC_LPM128_STAT_V4_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_v4_count)
#define SOC_LPM128_STAT_64BV6_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_64b_count)
#define SOC_LPM128_STAT_128BV6_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_128b_count)
#define SOC_LPM128_STAT_MAX_V4_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.max_v4_count)
#define SOC_LPM128_STAT_MAX_64BV6_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.max_64b_count)
#define SOC_LPM128_STAT_MAX_128BV6_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.max_128b_count)
#define SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.half_entry_count)
#ifdef FB_LPM_DEBUG
#define SOC_LPM128_STAT_V4_COUNT2(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_v4_count2)
#define SOC_LPM128_STAT_64BV6_COUNT2(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_64b_count2)
#define SOC_LPM128_STAT_128BV6_COUNT2(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_128b_count2)
#define SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT2(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.half_entry_count2)
#define SOC_LPM128_STAT_V4_COUNT1(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_v4_count1)
#define SOC_LPM128_STAT_64BV6_COUNT1(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_64b_count1)
#define SOC_LPM128_STAT_128BV6_COUNT1(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.used_128b_count1)
#define SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT1(u) \
        (soc_lpm128_state_table[(u)]->soc_lpm_stat.half_entry_count1)
#endif
/*
 * This structure maintains 128B V6, 64B V6, V4 prefix groups
 * Ideally all entries of same length should be contigous.
 * But for V4, the entries may not be continous because V6 entries occupy odd
 * TCAM entries as well. So if the V4 prefix is not continous, start2 and end2
 * are used to represent this split. 
 * In case of V6 entries, if end1 is crossing tcam boundary, then we add offset
 * of tcam_depth(1024) as we start V6 entry in the odd TCAM. However start2,
 * end2 are not set.
 * fent: For V6 fent of 2 represents 2 entries end1 + 1 and end1 + 1 + 1024. 
 *       For V4 prefixes fent of 2 represents entries end1/end2 + 1, 
 *       end1/end2 + 2
 * vent: For V6 or V4 if we add a entry then vent will be increased by 1.
 */

typedef struct soc_lpm128_state_s {
    int start1;  /* start index for this prefix length */
    int start2;  /* start index for this prefix length */
    int end1;    /* End index for this prefix length */
    int end2;    /* End index for this prefix length */
    int prev;    /* Previous (Lo to Hi) prefix length with non zero entry count*/
    int next;    /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;    /* valid entries */
    int fent;    /* free entries */
    uint8 hfent; /* Indicates Half free entry available for v4 entries. */
} soc_lpm128_state_t;
typedef soc_lpm128_state_t *soc_lpm128_state_p;

typedef struct soc_lpm128_table_s {
    soc_lpm128_state_p soc_lpm_state;
    soc_lpm128_state_p soc_lpm_unreserved_state;
    uint16 total_count;
    soc_lpm_stat_t soc_lpm_stat;
} soc_lpm128_table_t;

typedef enum soc_lpm_entry_type_s {
    socLpmEntryTypeV4 = 0x1,
    socLpmEntryType64BV6 = 0x02,
    socLpmEntryType128BV6 = 0x04,
    socLpmEntryTypeLast = 0x08
} soc_lpm_entry_type_t;

extern soc_lpm_state_p soc_lpm_state[SOC_MAX_NUM_DEVICES];
extern soc_lpm_stat_p soc_lpm_stat[SOC_MAX_NUM_DEVICES];
extern soc_lpm128_table_t *soc_lpm128_state_table[SOC_MAX_NUM_DEVICES];
extern soc_ext_lpm_stat_p soc_ext_lpm_stat[SOC_MAX_NUM_DEVICES];

extern int soc_fb_lpm_state_config(int u, int ipv6_64_depth, int start);
extern int soc_fb_lpm_init(int u);
extern int soc_fb_lpm_deinit(int u);
extern int soc_fb_lpm_insert(int unit, void *entry);
extern int soc_fb_lpm_insert_index(int unit, void *entry, int index);
extern int soc_fb_lpm_delete(int u, void *key_data);
extern int soc_fb_lpm_delete_index(int u, void *key_data, int index);
extern int soc_fb_lpm_match(int u, void *key_data, void *e, int *index_ptr);
extern int soc_fb_lpm_ipv4_delete_index(int u, int index);
extern int soc_fb_lpm_ipv6_delete_index(int u, int index);
extern int soc_fb_lpm_ip4entry0_to_0(int u, void *src, void *dst, int copy_hit);
extern int soc_fb_lpm_ip4entry1_to_1(int u, void *src, void *dst, int copy_hit);
extern int soc_fb_lpm_ip4entry0_to_1(int u, void *src, void *dst, int copy_hit);
extern int soc_fb_lpm_ip4entry1_to_0(int u, void *src, void *dst, int copy_hit);
extern int soc_fb_lpm_vrf_get(int unit, void *lpm_entry, int *vrf);
extern int soc_fb_lpm128_init(int u);
extern int soc_fb_lpm128_deinit(int u);
extern int soc_fb_get_largest_prefix(int u, int ipv6, void *e,
                              int *index, int *pfx_len, int* count);
extern int soc_fb_lpm128_insert(int u, void *hw_entry_lwr, void *hw_entry_upr,
                                int *index);
extern int soc_fb_lpm128_match(int u,
               void *key_data,
               void *key_data_upr,
               void *e,         /* return entry data if found */
               void *eupr,         /* return entry data if found */
               int *index_ptr,
               int *pfx,
               soc_lpm_entry_type_t *type);
extern int soc_fb_lpm128_delete(int u, void *key_data, void* key_data_upr);
extern int soc_fb_lpm_tcam_pair_count_get(int u, int *tcam_pair_count);
extern int soc_fb_lpm128_get_smallest_movable_prefix(int u, int ipv6, void *e,
                                                    void *eupr, int *index,
                                                    int *pfx_len, int *count);
int soc_fb_lpm128_is_v4_64b_allowed_in_paired_tcam(int unit);
int soc_fb_lpm_table_sizes_get(int unit, int *paired_table_size,
                                int *defip_table_size);
int soc_lpm_max_v4_route_get(int u, int *entries);
int soc_lpm_max_64bv6_route_get(int u, int *entries);
int soc_lpm_max_128bv6_route_get(int u, int *entries);
int soc_lpm_free_v4_route_get(int u, int *entries);
int soc_lpm_free_64bv6_route_get(int u, int *entries);
int soc_lpm_free_128bv6_route_get(int u, int *entries);
int soc_lpm_used_v4_route_get(int u, int *entries);
int soc_lpm_used_64bv6_route_get(int u, int *entries);
int soc_lpm_used_128bv6_route_get(int u, int *entries);
int soc_fb_lpm_stat_init(int u);
int soc_fb_lpm128_stat_init(int u);
void soc_lpm_stat_128b_count_update(int u, int incr);
int soc_lpm_reserved_route_get(int u, int *v4_entries,
                               int *v6_64_entries,
                               int *v6_128_entries);
int soc_lpm_fb_urpf_entry_replicate_index_offset(int unit,
                               int *offset);

#ifdef BCM_KATANA_SUPPORT

typedef struct soc_kt_lpm_ipv6_cfg_s {
    uint32 sip_start_offset;
    uint32 dip_start_offset;
    uint32 depth;
} soc_kt_lpm_ipv6_cfg_t;

typedef struct soc_kt_lpm_info_s {
    soc_kt_lpm_ipv6_cfg_t ipv6_128b;
    soc_kt_lpm_ipv6_cfg_t ipv6_64b;
} soc_kt_lpm_ipv6_info_t;

extern soc_kt_lpm_ipv6_info_t *soc_kt_lpm_ipv6_info_get(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_fb_lpm_reinit(int unit, int idx, uint32 *lpm_entry);
extern int soc_fb_lpm_reinit_done(int unit, int ipv6);
extern int soc_fb_lpm128_reinit(int unit, int idx, uint32 *lpm_entry,
                                uint32 *lpm_entry_upr);
extern int soc_fb_lpm128_reinit_done(int unit, int ipv6);

#else
#define soc_fb_lpm_reinit(u, t, s) (SOC_E_UNAVAIL)
#define soc_fb_lpm_reinit_done(u, i) (SOC_E_UNAVAIL)
#define soc_fb_lpm128_reinit(u, i, l, l1) (SOC_E_UNAVAIL)
#define soc_fb_lpm128_reinit_done(u, i) (SOC_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void soc_fb_lpm_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Ranger2PLUS scale numbers with RPF */
#define R2P_V4_MAX            512
#define R2P_64V6_MAX          256
#define R2P_128V6_MAX         128

#endif	/* BCM_FIREBOLT_SUPPORT */

#endif	/* !_LPM_H_ */
