/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * common defines between v4 and v6-128 code
 */

#ifndef _ESW_TRIDENT2_ALPM_INT_H
#define _ESW_TRIDENT2_ALPM_INT_H

#include <soc/esw/trie.h>
#include <soc/tomahawk.h>

#define SOC_ALPM_MODE_INVALID       -1
#define SOC_ALPM_MODE_COMBINED      0
#define SOC_ALPM_MODE_PARALLEL      1
#define SOC_ALPM_MODE_TCAM_ALPM     2

#define _MAX_KEY_LEN_   144
#if defined(BCM_FIREBOLT6_SUPPORT)
#define VRF_ID_LEN      (13)
#elif defined(BCM_TRIDENT2PLUS_SUPPORT)
#define VRF_ID_LEN      (12)
#elif defined(BCM_TOMAHAWK_SUPPORT)
#define VRF_ID_LEN      (11)
#else
#define VRF_ID_LEN      (10)
#endif


#define MAX_VRF_ID   ((1 << VRF_ID_LEN) + 1)
#define _MAX_KEY_WORDS_ (BITS2WORDS(_MAX_KEY_LEN_))

#define ALPM_IPV4   0
#define ALPM_IPV6   1
#define ALPM_IPVERS 2

#define ALPM_PREFIX_IN_TCAM(u, vrf_id) ((vrf_id == SOC_L3_VRF_OVERRIDE) || \
                  ((soc_alpm_cmn_mode_get(u) == SOC_ALPM_MODE_TCAM_ALPM) && \
                                        (vrf_id == SOC_L3_VRF_GLOBAL)))

/* Bucket Management Functions */
/* The Buckets from the shared RAM are assigned to TCAM PIVOTS to store
 * prefixes. The bucket once assigned to a TCAM PIVOT will be in use till the
 * PIVOT is active (at least one Prefix is used in the bucket). If no entries
 * are in the bucket, the bucket can be treated as free. The bucket pointers
 * follow TCAM entries when the TCAM entires are moved up or down to make
 * space.
 */

/* Shared bitmap routines are used to track bucket usage
 * For TH, in Parallel mode with uRPF enable, Global low & Private routes can
 * share same physical bucket, separate bmap are used for tracking each bucket
 * usage, for other mode or in TD2, they just point to same memory address */
typedef struct soc_alpm_bucket_s {
    SHR_BITDCL *alpm_vrf_bucket_bmap[ALPM_IPVERS];
    SHR_BITDCL *alpm_glb_bucket_bmap[ALPM_IPVERS];
    int alpm_bucket_bmap_size;
    int bucket_count;
    int next_free;
    int v6_resv_bucket_count;
} soc_alpm_bucket_t;

extern soc_alpm_bucket_t soc_alpm_bucket[];
/* For TD2, we just use one bmap, glb bmap is not used */
#define SOC_ALPM_BUCKET_BMAP(u)         (soc_alpm_bucket[u].alpm_vrf_bucket_bmap[0])
#define SOC_ALPM_BUCKET_BMAP_BYTE(u,i)  (soc_alpm_bucket[u].alpm_bucket_bmap[i])
#define SOC_ALPM_BUCKET_BMAP_SIZE(u)    (soc_alpm_bucket[u].alpm_bucket_bmap_size)
#define SOC_ALPM_BUCKET_NEXT_FREE(u)    (soc_alpm_bucket[u].next_free)
#define SOC_ALPM_BUCKET_COUNT(u)        (soc_alpm_bucket[u].bucket_count)
#define SOC_ALPM_BUCKET_MAX_INDEX(u)    (soc_alpm_bucket[u].bucket_count - 1)

extern int l3_alpm_sw_prefix_lookup[SOC_MAX_NUM_DEVICES];
#define SOC_ALPM_SW_LOOKUP(u)           (l3_alpm_sw_prefix_lookup[u])

#define SOC_ALPM_RPF_BKT_IDX(u, bkt)    \
    (bkt + SOC_ALPM_BUCKET_COUNT(u))

/* For TH, in Parallel mode, we combine the buckets */
extern soc_alpm_bucket_t soc_th_alpm_bucket[];

#define SOC_TH_ALPM_VRF_BUCKET_BMAP(u, ipv) \
            (soc_th_alpm_bucket[u].alpm_vrf_bucket_bmap[ipv])
#define SOC_TH_ALPM_GLB_BUCKET_BMAP(u, ipv) \
            (soc_th_alpm_bucket[u].alpm_glb_bucket_bmap[ipv])
#define SOC_TH_ALPM_BUCKET_BMAP_BYTE(u,i)   \
            (soc_th_alpm_bucket[u].alpm_bucket_bmap[i])
#define SOC_TH_ALPM_BUCKET_BMAP_SIZE(u)     \
            (soc_th_alpm_bucket[u].alpm_bucket_bmap_size)
#define SOC_TH_ALPM_BUCKET_NEXT_FREE(u)     \
            (soc_th_alpm_bucket[u].next_free)
#define SOC_TH_ALPM_BUCKET_COUNT(u)         \
            (soc_th_alpm_bucket[u].bucket_count)
#define SOC_TH_ALPM_V6_RESV_BUCKET_COUNT(u) \
            (soc_th_alpm_bucket[u].v6_resv_bucket_count)
#define SOC_TH_ALPM_BUCKET_MAX_INDEX(u)     \
            (soc_th_alpm_bucket[u].bucket_count - 1)
#define SOC_TH_ALPM_RPF_BKT_IDX(u, bkt)     \
            (bkt + SOC_TH_ALPM_BUCKET_COUNT(u))

#define PRESERVE_HIT                    TRUE

typedef struct _payload_s payload_t;
struct _payload_s {
    trie_node_t node; /*trie node */
    payload_t *next; /* list node */
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];
    unsigned int len;
    int index;   /* Memory location */
    payload_t *bkt_ptr;
};

/*
 * Table Operations for ALPM
 */

/* Generic AUX operation function */
typedef enum _soc_aux_op_s {
    INSERT_PROPAGATE,
    DELETE_PROPAGATE,
    PREFIX_LOOKUP,
    HITBIT_REPLACE
}_soc_aux_op_t;

extern int _soc_alpm_aux_op(int u, _soc_aux_op_t aux_op,
                 defip_aux_scratch_entry_t *aux_entry, int update_scratch,
                 int *hit, int *tcam_index, int *bucket_index, int aux_flags);
extern int _soc_mem_alpm_read(int unit, soc_mem_t mem, int copyno,
                              int index, void *entry_data);
extern int _ipmask2pfx(uint32 ipv4m, int *mask_len);

extern int soc_alpm_assign(int u, int vrf, soc_mem_t mem, int *bucket_pointer, int *shuffled);
extern int alpm_bucket_assign(int u, int v6, int *bucket_pointer);
extern int alpm_bucket_release(int u, int bucket_pointer, int v6);
extern int alpm_bucket_is_assigned(int u, int bucket_ptr, int ipv6, int *used);

extern int soc_th_alpm_assign(int u, int vrf, soc_mem_t mem,
                              int *log_bkt, int *shuffled, int *free_count);
extern int soc_th_alpm_bucket_assign(int u, int vrf, int v6, int *log_bkt);
extern int soc_th_alpm_bucket_release(int u, int log_bkt, int vrf, int v6);
extern int soc_th_alpm_bucket_is_assigned(int u, int log_bkt, int vrf, int v6, int *used);

/* Debug counter structure */
typedef struct alpm_vrf_counter_s {
    uint32 v4;
    uint32 v6_64;
    uint32 v6_128;
} alpm_vrf_counter_t;

/* Per VRF PIVOT and Prefix trie. Each VRF will host a trie based on IPv4, IPV6-64 and IPV6-128
 * This seperation reduces the complexity of trie management.
 */
typedef struct alpm_vrf_handle_s {
    trie_t *pivot_trie_ipv4;     /* IPV4 Pivot trie */
    trie_t *pivot_trie_ipv6;     /* IPV6-64 Pivot trie */
    trie_t *pivot_trie_ipv6_128; /* IPV6-128 Pivot trie */
    trie_t *prefix_trie_ipv4;    /* IPV4 Pivot trie */
    trie_t *prefix_trie_ipv6;    /* IPV6-64 Pivot trie */
    trie_t *prefix_trie_ipv6_128;    /* IPV6-128 Prefix trie */
    defip_entry_t *lpm_entry;          /* IPv4 Default LPM entry */
    defip_entry_t *lpm_entry_v6;       /* IPv6 Default LPM entry */
    defip_pair_128_entry_t *lpm_entry_v6_128;   /* IPv6-128 Default LPM entry */
    int count_v4;                /* no. of routes for this vrf */
    int count_v6_64;
    int count_v6_128;

    /* Debug counter */
    alpm_vrf_counter_t add;
    alpm_vrf_counter_t del;
    alpm_vrf_counter_t bkt_split;
    alpm_vrf_counter_t pivot_used;
    alpm_vrf_counter_t lpm_full;
    alpm_vrf_counter_t lpm_shift;

    int init_done;               /* Init for VRF completed */
                                 /* ready to accept route additions */
    int flex;                    /* Programmed with flex counter */
} alpm_vrf_handle_t;

/* Use MAX_VRF_ID to store VRF_OVERRIDE routes debug info */
extern alpm_vrf_handle_t *alpm_vrf_handle[SOC_MAX_NUM_DEVICES];

/*
 * Bucket Hnadle
 */
typedef struct alpm_bucket_handle_s {
    trie_t *bucket_trie;   /* trie of Prefix within this bucket */
    int bucket_index;      /* bucket Pointer */

    /* Debug counter */
    int    vrf;
    uint16 ipv6;
    uint16 def;
    uint16 min;
    uint16 max;

} alpm_bucket_handle_t;

/*
 * Pivot Structure
 */
typedef struct alpm_pivot_s {
    trie_node_t node; /*trie node */
    /* dq_t        listnode;*/ /* list node */
    alpm_bucket_handle_t *bucket;  /* Bucket trie */
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];  /* pivot */
    unsigned int len;                             /* pivot length */
    unsigned int bpm_len;                         /* pivot bpm_len */
    int tcam_index;   /* TCAM index where the pivot is inserted */
} alpm_pivot_t;

/* Bucket sharing Data Structure */

#define SUB_BKT_SHIFT               (3)
#define SUB_BKT_MASK                ((1U << SUB_BKT_SHIFT) - 1)
#define ALPM_LOG_BKT(phy, sub)      (((phy) << SUB_BKT_SHIFT) | ((sub) & SUB_BKT_MASK))
#define ALPM_PHY_BKT(u, log)        (((log) >> SUB_BKT_SHIFT) & ALPM_CTRL(u).bkt_mask)
#define ALPM_SUB_BKT(u, log)        ((log) & SUB_BKT_MASK)

#define SOC_TH_MAX_ALPM_VIEWS       (5)     /* IPv4, IPv6-64, IPv6-128, IPv4-Flex, IPv6-64-Flex */
#define SOC_TH_MAX_BUCKET_ENTRIES(u) (SOC_IS_TRIDENT3X(u) ? (96+1) : (48+1))
#define SOC_TH_MAX_SUB_BUCKETS      (4)     /* Max to 4 sub bucket in TH    */

typedef struct alpm_bkt_usg_s {
    uint8 count;        /* No. of entries in physical bucket                */
    uint8 sub_bkts;     /* Bitmap of logical buckets                        */
    uint8 global;       /* 1: Global, 0: Private                            */
    int16 pivots[4];    /* Pivot indices corresponding to logical buckets   */
} alpm_bkt_usg_t;

typedef struct alpm_bkt_bmp_s {
    uint32 bkt_count;
    SHR_BITDCL *bkt_bmp;
} alpm_bkt_bmp_t;

#define SOC_ALPM_BU_PTR(u, p)               (&(bkt_usage[(u)][(p)]))
#define SOC_ALPM_BU_COUNT(u, p)             SOC_ALPM_BU_PTR(u, p)->count
#define SOC_ALPM_BU_SUB_BKTS(u, p)          SOC_ALPM_BU_PTR(u, p)->sub_bkts
#define SOC_ALPM_BU_GLOBAL(u, p)            SOC_ALPM_BU_PTR(u, p)->global
#define SOC_ALPM_BU_PIVOT_IDX(u, p, s)      SOC_ALPM_BU_PTR(u, p)->pivots[(s)]
#define SOC_ALPM_BU_PIVOT_IDX_UPDATE(u, pivot)                      \
    {                                                               \
        int phy_bkt, sub_bkt;                                       \
        phy_bkt = PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(u, (pivot)));  \
        sub_bkt = ALPM_SUB_BKT(u, phy_bkt);                         \
        phy_bkt = ALPM_PHY_BKT(u, phy_bkt);                         \
        if (SOC_ALPM_BU_SUB_BKT_IS_SET(u, phy_bkt, sub_bkt)) {      \
            SOC_ALPM_BU_PIVOT_IDX(u, phy_bkt, sub_bkt) = (pivot);   \
        }                                                           \
    }

#define SOC_ALPM_BU_SUB_BKT_SET(u, p, s)   \
    (SOC_ALPM_BU_PTR(u, (p))->sub_bkts |= 1U << (s))
#define SOC_ALPM_BU_SUB_BKT_CLR(u, p, s)   \
    (SOC_ALPM_BU_PTR(u, (p))->sub_bkts &= ~(1U << (s)))
#define SOC_ALPM_BU_SUB_BKT_IS_SET(u, p, s)  \
    (SOC_ALPM_BU_PTR(u, (p))->sub_bkts & (1U << (s)))

#define SOC_ALPM_GBU_PTR(u, v, c)           (&global_bkt_usage[(u)][(v)][(c)])
#define SOC_ALPM_GBU_BKT_BMP(u, v, c)       SOC_ALPM_GBU_PTR(u, v, c)->bkt_bmp
#define SOC_ALPM_GBU_BKT_COUNT(u, v, c)     SOC_ALPM_GBU_PTR(u, v, c)->bkt_count


/* Get available bank based on given bank_disable bitmap
 * - bank_num : a return value for next available bank id
 * - bank_count: total bank number
 * - bank_disable: disabled bank bitmap
 */

#define ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable) \
        do { \
            bank_num = 0;\
            while ((bank_disable & (1U << bank_num)) && \
                   (bank_num < bank_count)) { \
                bank_num ++; \
            } \
        } while (0)
#define ALPM_NEXT_AVAIL_BANK(bank_num, bank_count, bank_disable) \
        do { \
            bank_num ++; \
            while ((bank_disable & (1U << bank_num)) && \
                   (bank_num < bank_count)) { \
                bank_num ++; \
            } \
        } while (0)


/* Used for store returned entry_index in lookup function
 * defip_index =
 * -----------------------------------------------------
 * | 3 bits (sub bucket index) | 29 bits (entry index) |
 * -----------------------------------------------------
 */
#define ALPM_ENT_INDEX_BITS     (32 - SUB_BKT_SHIFT)
#define ALPM_ENT_INDEX_MASK     ((1 << ALPM_ENT_INDEX_BITS) - 1)
#define ALPM_ENT_INDEX(idx)     ((idx) & ALPM_ENT_INDEX_MASK)

/* TH: 13, AP & TH2: 14 */
#define SOC_ALPM_BUCKET_BITS(u) ((SOC_IS_TOMAHAWK(u) || SOC_IS_TRIDENT3X(u)) ? 13 : 14)

#define SOC_TH_ALPM_AIDX_TO_PHY_BKT(u, aidx)                    \
            (((aidx) >> (ALPM_CTRL(u).bank_bits)) & ALPM_CTRL(u).bkt_mask)

#define SOC_TH_ALPM_AIDX_TO_SUB_BKT(u, aidx) ((aidx) >> ALPM_ENT_INDEX_BITS)

#define SOC_TH_ALPM_AIDX_TO_LOG_BKT(u, aidx)                    \
            ALPM_LOG_BKT(SOC_TH_ALPM_AIDX_TO_PHY_BKT(u, aidx),  \
                         SOC_TH_ALPM_AIDX_TO_SUB_BKT(u, aidx))

#define SOC_TH_ALPM_AIDX(u, phy, sub, bank, ent)                            \
            ((sub) << ALPM_ENT_INDEX_BITS |                                 \
             (ent) << (ALPM_CTRL(u).bkt_bits + ALPM_CTRL(u).bank_bits) |    \
             (phy) << (ALPM_CTRL(u).bank_bits) |                            \
             (bank))

extern alpm_bkt_usg_t   *bkt_usage[SOC_MAX_NUM_DEVICES];
extern alpm_bkt_bmp_t   *global_bkt_usage[SOC_MAX_NUM_DEVICES][SOC_TH_MAX_ALPM_VIEWS];

/* TCAM Pivot management */

#define MAX_PIVOT_COUNT               (16384)
#define TH_MAX_PIVOT_COUNT(u)         \
    ((SOC_IS_TOMAHAWK2(u) || SOC_IS_TRIDENT3X(u)) ? 32768 : 16384)
#define MAX_BANK_COUNT(u)   (SOC_IS_TRIDENT3X(u) ? 8 : 4)

/* Array of Pivots */
extern alpm_pivot_t **tcam_pivot[SOC_MAX_NUM_DEVICES];

#define ALPM_TCAM_PIVOT(u, index)    tcam_pivot[u][index]

#define PIVOT_BUCKET_HANDLE(p)    (p)->bucket
#define PIVOT_BUCKET_TRIE(p)      ((p)->bucket)->bucket_trie
#define PIVOT_BUCKET_INDEX(p)     ((p)->bucket)->bucket_index

/* Debug counter */
#define PIVOT_BUCKET_VRF(p)       ((p)->bucket)->vrf
#define PIVOT_BUCKET_IPV6(p)      ((p)->bucket)->ipv6
#define PIVOT_BUCKET_DEF(p)       ((p)->bucket)->def
#define PIVOT_BUCKET_MIN(p)       ((p)->bucket)->min
#define PIVOT_BUCKET_MAX(p)       ((p)->bucket)->max
#define PIVOT_BUCKET_COUNT(p)     PIVOT_BUCKET_TRIE(p)->trie->count

#define PIVOT_BUCKET_ENT_CNT_UPDATE(p)      \
    if (PIVOT_BUCKET_TRIE(p)->trie != NULL) { \
        int _count = PIVOT_BUCKET_COUNT(p); \
        if (((p)->bucket)->min == 0) {      \
            ((p)->bucket)->min = _count;    \
        } else if (_count < ((p)->bucket)->min) { \
            ((p)->bucket)->min = _count;    \
        }                                   \
        if (_count > ((p)->bucket)->max) {  \
            ((p)->bucket)->max = _count;    \
        }                                   \
    }

#define PIVOT_TCAM_INDEX(p)       ((p)->tcam_index)
#define PIVOT_TCAM_BPMLEN(p)      ((p)->bpm_len)


#define VRF_PIVOT_TRIE_IPV4(u, vrf)         \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv4
#define VRF_PIVOT_TRIE_IPV6(u, vrf)         \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv6
#define VRF_PIVOT_TRIE_IPV6_128(u, vrf)     \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv6_128
#define VRF_PREFIX_TRIE_IPV4(u, vrf)        \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv4
#define VRF_PREFIX_TRIE_IPV6(u, vrf)        \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv6
#define VRF_PREFIX_TRIE_IPV6_128(u, vrf)    \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv6_128

#define L3_DEFIP_MODE_V4        (0)
#define L3_DEFIP_MODE_64        (1)
#define L3_DEFIP_MODE_128       (2)
#define L3_DEFIP_MODE_MAX       (3)

#define RAW_OLD_BKT_DIP   0
#define RAW_OLD_BKT_SIP   1
#define RAW_NEW_BKT_DIP   2
#define RAW_NEW_BKT_SIP   3
#define RAW_RB_BKT_DIP    4
#define RAW_RB_BKT_SIP    5
#define RAW_BKT_NUM       6

#define VRF_TRIE_INIT_DONE(u, vrf, v6, val)      \
do {\
    alpm_vrf_handle[u][vrf].init_done &= ~(1 << (v6));\
    alpm_vrf_handle[(u)][(vrf)].init_done |= ((val) & 1) << (v6);\
} while (0)

#define VRF_TRIE_INIT_COMPLETED(u, vrf, v6)     \
    ((alpm_vrf_handle[u][vrf].init_done & (1 << (v6))) != 0)

#define VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf)     \
    alpm_vrf_handle[u][vrf].lpm_entry
#define VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf)     \
    alpm_vrf_handle[u][vrf].lpm_entry_v6
#define VRF_TRIE_DEFAULT_ROUTE_IPV6_128(u, vrf) \
    alpm_vrf_handle[u][vrf].lpm_entry_v6_128

#define VRF_PIVOT_FULL_INC(u, vrf, v6)                  \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v4++;      \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v6_64++;   \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v6_128++;  \
    }                                                   \
} while (0)

#define VRF_PIVOT_SHIFT_INC(u, vrf, v6)                 \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v4++;     \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v6_64++;  \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v6_128++; \
    }                                                   \
} while (0)

#define VRF_PIVOT_REF_CNT(u, vrf_id, vrf, v6)                             \
   (((vrf_id) == SOC_L3_VRF_OVERRIDE)      ?                              \
    (((v6) == 0) ? alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v4  :      \
    (((v6) == 1) ? alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_64 :    \
                   alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_128)) : \
    (((v6) == 0) ? alpm_vrf_handle[(u)][(vrf)].pivot_used.v4  :           \
    (((v6) == 1) ? alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_64 :         \
                   alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_128)))


#define VRF_FLEX_COMPLETED  VRF_PIVOT_REF_CNT


#define VRF_FLEX_SET(u, vrf_id, vrf, v6, val)                        \
do {                                                                 \
    if ((vrf_id) == SOC_L3_VRF_OVERRIDE) {                           \
        alpm_vrf_handle[(u)][MAX_VRF_ID].flex &= ~(1 << (v6));       \
        alpm_vrf_handle[(u)][MAX_VRF_ID].flex |= ((!!(val))) << (v6);\
    } else {                                                         \
        alpm_vrf_handle[(u)][(vrf)].flex &= ~(1 << (v6));            \
        alpm_vrf_handle[(u)][(vrf)].flex |= ((!!(val))) << (v6);     \
    }                                                                \
} while (0)

#define VRF_FLEX_IS_SET(u, vrf_id, vrf, v6, add)                    \
   (((add) || VRF_FLEX_COMPLETED(u, vrf_id, vrf, v6)) &&            \
    (((vrf_id) == SOC_L3_VRF_OVERRIDE) ?                            \
     ((alpm_vrf_handle[(u)][MAX_VRF_ID].flex & (1 << (v6))) != 0) : \
     ((alpm_vrf_handle[(u)][(vrf)].flex & (1 << (v6))) != 0)))


#define VRF_PIVOT_REF_INC(u, vrf_id, vrf, v6)                     \
do {                                                              \
    if ((vrf_id) == SOC_L3_VRF_OVERRIDE) {                        \
        if (!(v6)) {                                              \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v4++;     \
        } else if ((v6) == 1) {                                   \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_64++;  \
        } else {                                                  \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_128++; \
        }                                                         \
    } else {                                                      \
        if (!(v6)) {                                              \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v4++;          \
        } else if ((v6) == 1) {                                   \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_64++;       \
        } else {                                                  \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_128++;      \
        }                                                         \
    }                                                             \
} while (0)

#define VRF_PIVOT_REF_DEC(u, vrf_id, vrf, v6)                     \
do {                                                              \
    if ((vrf_id) == SOC_L3_VRF_OVERRIDE) {                        \
        if (!(v6)) {                                              \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v4--;     \
        } else if ((v6) == 1) {                                   \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_64--;  \
        } else {                                                  \
            alpm_vrf_handle[(u)][MAX_VRF_ID].pivot_used.v6_128--; \
        }                                                         \
    } else {                                                      \
        if (!(v6)) {                                              \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v4--;          \
        } else if ((v6) == 1) {                                   \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_64--;       \
        } else {                                                  \
            alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_128--;      \
        }                                                         \
    }                                                             \
} while (0)

#define VRF_BUCKET_SPLIT_INC(u, vrf, v6)                \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v4++;     \
    } else if ((v6) == L3_DEFIP_MODE_64) {              \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v6_64++;  \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v6_128++; \
    }                                                   \
} while (0)

#define VRF_TRIE_ROUTES_INC(u, vrf_id, vrf, v6)              \
do {                                                         \
    if ((vrf_id) == SOC_L3_VRF_OVERRIDE) {                   \
        if (!(v6)) {                                         \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v4++;     \
            alpm_vrf_handle[(u)][MAX_VRF_ID].add.v4++;       \
        } else if ((v6) == L3_DEFIP_MODE_64) {               \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_64++;  \
            alpm_vrf_handle[(u)][MAX_VRF_ID].add.v6_64++;    \
        } else {                                             \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_128++; \
            alpm_vrf_handle[(u)][MAX_VRF_ID].add.v6_128++;   \
        }                                                    \
    } else {                                                 \
        if (!(v6)) {                                         \
            alpm_vrf_handle[(u)][(vrf)].count_v4++;          \
            alpm_vrf_handle[(u)][(vrf)].add.v4++;            \
        } else if ((v6) == L3_DEFIP_MODE_64) {               \
            alpm_vrf_handle[(u)][(vrf)].count_v6_64++;       \
            alpm_vrf_handle[(u)][(vrf)].add.v6_64++;         \
        } else {                                             \
            alpm_vrf_handle[(u)][(vrf)].count_v6_128++;      \
            alpm_vrf_handle[(u)][(vrf)].add.v6_128++;        \
        }                                                    \
    }                                                        \
} while (0)

#define VRF_TRIE_ROUTES_DEC(u, vrf_id, vrf, v6)              \
do {                                                         \
    if ((vrf_id) == SOC_L3_VRF_OVERRIDE) {                   \
        if (!(v6)) {                                         \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v4--;     \
            alpm_vrf_handle[(u)][MAX_VRF_ID].del.v4++;       \
        } else if ((v6) == L3_DEFIP_MODE_64) {               \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_64--;  \
            alpm_vrf_handle[(u)][MAX_VRF_ID].del.v6_64++;    \
        } else {                                             \
            alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_128--; \
            alpm_vrf_handle[(u)][MAX_VRF_ID].del.v6_128++;   \
        }                                                    \
    } else {                                                 \
        if (!(v6)) {                                         \
            alpm_vrf_handle[(u)][(vrf)].count_v4--;          \
            alpm_vrf_handle[(u)][(vrf)].del.v4++;            \
        } else if ((v6) == L3_DEFIP_MODE_64) {               \
            alpm_vrf_handle[(u)][(vrf)].count_v6_64--;       \
            alpm_vrf_handle[(u)][(vrf)].del.v6_64++;         \
        } else {                                             \
            alpm_vrf_handle[(u)][(vrf)].count_v6_128--;      \
            alpm_vrf_handle[(u)][(vrf)].del.v6_128++;        \
        }                                                    \
    }                                                        \
} while (0)


#define VRF_TRIE_ROUTES_CNT(u, vrf_id, vrf, v6)                      \
   (((vrf_id) == SOC_L3_VRF_OVERRIDE)      ?                         \
    (((v6) == 0) ? alpm_vrf_handle[(u)][MAX_VRF_ID].count_v4  :      \
    (((v6) == 1) ? alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_64 :    \
                   alpm_vrf_handle[(u)][MAX_VRF_ID].count_v6_128)) : \
    (((v6) == 0) ? alpm_vrf_handle[(u)][(vrf)].count_v4  :           \
    (((v6) == 1) ? alpm_vrf_handle[(u)][(vrf)].count_v6_64 :         \
                   alpm_vrf_handle[(u)][(vrf)].count_v6_128)))


/* Used to store the list of Prefixes that need to be moved to new bucket */
#define MAX_PREFIX_PER_BUCKET 96
#define SOC_ALPM_LPM_LOCK(u)             soc_mem_lock(u, L3_DEFIPm)
#define SOC_ALPM_LPM_UNLOCK(u)           soc_mem_unlock(u, L3_DEFIPm)
#define _ALPM_IF_ERROR_GOTO(rv, label, done, doing) {if ((rv) < 0) {goto label;} done=doing;}
#define MAX_RAW_COUNT        -1
#define INVALID_BPM_LEN      -1

#define SOC_ALPM_MEM_VIEW(mem)                       \
            (mem == L3_DEFIP_ALPM_IPV4m      ? 0 :   \
             mem == L3_DEFIP_ALPM_IPV6_64m   ? 1 :   \
             mem == L3_DEFIP_ALPM_IPV6_128m  ? 2 :   \
             mem == L3_DEFIP_ALPM_IPV4_1m    ? 3 :   \
             mem == L3_DEFIP_ALPM_IPV6_64_1m ? 4 : 0)

#define SOC_ALPM_MEM_V6(mem)                       \
            (mem == L3_DEFIP_ALPM_IPV4m      ? L3_DEFIP_MODE_V4 :   \
             mem == L3_DEFIP_ALPM_IPV6_64m   ? L3_DEFIP_MODE_64 :   \
             mem == L3_DEFIP_ALPM_IPV6_128m  ? L3_DEFIP_MODE_128:   \
             mem == L3_DEFIP_ALPM_IPV4_1m    ? L3_DEFIP_MODE_V4 :   \
             mem == L3_DEFIP_ALPM_IPV6_64_1m ? L3_DEFIP_MODE_64 : 0)

/* Used for AUX operation */
#define SOC_ALPM_AUX_DEF_ROUTE     (1 << 0)
#define SOC_ALPM_AUX_SRC_DISCARD   (1 << 1)

typedef enum {
    MERGE_PHY = 0,
    MERGE_LOG_CHILD_TO_PARENT,
    MERGE_LOG_PARENT_TO_CHILD
} soc_alpm_merge_type_t;

#define REPART_THRESHOLD 10
#define MERGE_THRESHOLD  10

typedef struct {
    int u;
    int merge_count;
    int orig_vrf;
    int vrf;
    int v6;
    soc_mem_t mem;
    int max_count;

    int shuffled;
    int free_count;     /* Max free count as a result of merge or repart */
    int log_bkt;        /* out */
} soc_alpm_merge_info_t;


typedef struct {
    payload_t *prefix[MAX_PREFIX_PER_BUCKET];
    int count;
} alpm_mem_prefix_array_t;

typedef struct _alpm_pfx_info_s {
    soc_mem_t       mem;
    void            *alpm_data;
    void            *alpm_sip_data;
    int             pivot_idx_from;
    int             log_bkt_from;
    int             log_bkt_to;
    int             vrf;
} alpm_pfx_info_t;

/* Internal Debug counter */
typedef struct _alpm_int_dbg_cnt_s {
    uint32 bkt_split;
    uint32 bkt_share;
    uint32 bkt_merge;
    uint32 bkt_merge2;
    uint32 bkt_repart;
    uint32 ppg_cb_insert;
    uint32 ppg_cb_delete;
    uint32 ppg_cb_hitbit;
} _alpm_int_dbg_cnt_t;

typedef struct trie_bpm_cb_user_data_s {
    int unit;
    int ipv6;
    int aux_op;
    int count;
    int flags;

    /* carry out data based on aux_op value */
    int pfx_len;
    int old_idx;
    int new_idx;
    void *entry;
} trie_bpm_cb_user_data_t;

typedef struct _soc_alpm_fld_s {
    /* L3_DEFIP */
    soc_field_t key_mode0;
    soc_field_t key_mode1;
    soc_field_t key_mode_mask0;
    soc_field_t key_mode_mask1;
    soc_field_t key_type0;
    soc_field_t key_type1;
    soc_field_t key_type_mask0;
    soc_field_t key_type_mask1;
    /* L3_DEFIP_PAIR_128 */
    soc_field_t key_mode0_upr;
    soc_field_t key_mode0_lwr;
    soc_field_t key_mode1_upr;
    soc_field_t key_mode1_lwr;
    soc_field_t key_mode_mask0_upr;
    soc_field_t key_mode_mask0_lwr;
    soc_field_t key_mode_mask1_upr;
    soc_field_t key_mode_mask1_lwr;
    soc_field_t key_type0_upr;
    soc_field_t key_type0_lwr;
    soc_field_t key_type1_upr;
    soc_field_t key_type1_lwr;
    soc_field_t key_type_mask0_upr;
    soc_field_t key_type_mask0_lwr;
    soc_field_t key_type_mask1_upr;
    soc_field_t key_type_mask1_lwr;
    /* L3_DEFIP_AUX_SCRATCH */
    soc_field_t key_mode;
    soc_field_t key_type;
} _soc_alpm_fld_t;

typedef struct _soc_alpm_ctrl_s {
    int alpm_mode;
    int bkt_sharing;
    int half_bkt_mode;
    int sw_prefix_lkup;
    int num_banks;
    int bank_bits;
    int num_bkts;
    int bkt_bits;
    uint32 bkt_mask;
    uint32 bank_disable_bmp[2]; /* 0:Global, 1:Private */
    uint32 bank_disable_bmp_8b[2]; /* 0:Global, 1:Private */
    int double_wide[L3_DEFIP_MODE_MAX];
    int ent_per_bank[L3_DEFIP_MODE_MAX];
    int cnt_ent_per_bkt[L3_DEFIP_MODE_MAX];
    int gp_zoned;

    int hit_bits_skip;
    int trie_propagate;
    int trie_user_ppg;
    int hit_idx_upd;
    int force_aux_op;
    int alpm_scaling;
    int aux_enable;

    uint8 _alpm_128b;
    uint8 _alpm_128b_bkt_rsvd;
    uint32 _alpm_128b_bkt_rsvd_cnt;

    _soc_alpm_fld_t fld;

    int ctrl_inited;
} _soc_alpm_ctrl_t;

extern _alpm_int_dbg_cnt_t _alpm_dbg_cntr[];
extern _soc_alpm_ctrl_t soc_alpm_control[];

#define ALPM_CTRL(u) (soc_alpm_control[(u)])

#define MODE0_f(u)                      ALPM_CTRL(u).fld.key_mode0
#define MODE1_f(u)                      ALPM_CTRL(u).fld.key_mode1
#define MODE_MASK0_f(u)                 ALPM_CTRL(u).fld.key_mode_mask0
#define MODE_MASK1_f(u)                 ALPM_CTRL(u).fld.key_mode_mask1
#define ENTRY_TYPE0_f(u)                ALPM_CTRL(u).fld.key_type0
#define ENTRY_TYPE1_f(u)                ALPM_CTRL(u).fld.key_type1
#define ENTRY_TYPE_MASK0_f(u)           ALPM_CTRL(u).fld.key_type_mask0
#define ENTRY_TYPE_MASK1_f(u)           ALPM_CTRL(u).fld.key_type_mask1
#define MODE0_UPR_f(u)                  ALPM_CTRL(u).fld.key_mode0_upr
#define MODE0_LWR_f(u)                  ALPM_CTRL(u).fld.key_mode0_lwr
#define MODE1_UPR_f(u)                  ALPM_CTRL(u).fld.key_mode1_upr
#define MODE1_LWR_f(u)                  ALPM_CTRL(u).fld.key_mode1_lwr
#define MODE_MASK0_UPR_f(u)             ALPM_CTRL(u).fld.key_mode_mask0_upr
#define MODE_MASK0_LWR_f(u)             ALPM_CTRL(u).fld.key_mode_mask0_lwr
#define MODE_MASK1_UPR_f(u)             ALPM_CTRL(u).fld.key_mode_mask1_upr
#define MODE_MASK1_LWR_f(u)             ALPM_CTRL(u).fld.key_mode_mask1_lwr
#define ENTRY_TYPE0_UPR_f(u)            ALPM_CTRL(u).fld.key_type0_upr
#define ENTRY_TYPE0_LWR_f(u)            ALPM_CTRL(u).fld.key_type0_lwr
#define ENTRY_TYPE1_UPR_f(u)            ALPM_CTRL(u).fld.key_type1_upr
#define ENTRY_TYPE1_LWR_f(u)            ALPM_CTRL(u).fld.key_type1_lwr
#define ENTRY_TYPE_MASK0_UPR_f(u)       ALPM_CTRL(u).fld.key_type_mask0_upr
#define ENTRY_TYPE_MASK0_LWR_f(u)       ALPM_CTRL(u).fld.key_type_mask0_lwr
#define ENTRY_TYPE_MASK1_UPR_f(u)       ALPM_CTRL(u).fld.key_type_mask1_upr
#define ENTRY_TYPE_MASK1_LWR_f(u)       ALPM_CTRL(u).fld.key_type_mask1_lwr
#define MODE_f(u)                       ALPM_CTRL(u).fld.key_mode
#define ENTRY_TYPE_f(u)                 ALPM_CTRL(u).fld.key_type

extern _alpm_int_dbg_cnt_t soc_alpm_dbg_cnt[SOC_MAX_NUM_DEVICES];
extern void _soc_th_alpm_mask_prefix_create(int u, int mode, uint32 mask_len,
                                            uint32 *mask);
extern void soc_alpm_db_ent_type_encoding(int u, int vrf, uint32 *db_type,
                                          uint32 *ent_type);
extern int _soc_alpm_rpf_entry(int u, int idx);
extern int _soc_alpm_find_in_bkt(int u, soc_mem_t mem, int bucket_index,
                                 int bank_disable, uint32 *e, void *alpm_data,
                                 int *key_index, int v6);
extern int alpm_mem_prefix_array_cb(trie_node_t *node, void *info);
extern int alpm_delete_node_cb(trie_node_t *node, void *info);
extern int _soc_alpm_insert_in_bkt(int u, soc_mem_t mem, int bucket_index,
                                   int bank_disable, void *alpm_data, void *alpm_sip_data,
                                   uint32 *e, int *key_index, int v6);
extern int _soc_alpm_delete_in_bkt(int u, soc_mem_t mem, int delete_bucket,
                                   int bank_disable, void *bufp2, uint32 *e,
                                   int *key_index, int v6);
extern int _soc_th_alpm_lpm_entry_view_get(int u, int ipv6, int flex);
extern int _soc_th_alpm_lpm_flex_get(int u, int ipv6, int entry_view);
extern int soc_alpm_128_init(int u);
extern int soc_alpm_128_state_clear(int u);
extern int soc_alpm_128_deinit(int u);
extern int soc_alpm_128_lpm_init(int u);
extern int soc_alpm_128_lpm_deinit(int u);
extern int soc_alpm_physical_idx(int u, soc_mem_t mem, int index, int full);
extern int soc_alpm_logical_idx(int u, soc_mem_t mem, int index, int full);

extern int _soc_alpm_mem_index(int u, soc_mem_t mem, int bucket_index,
                               int offset, uint32 bank_disable, int *key_index);
extern int _soc_alpm_raw_bucket_read(int u, soc_mem_t mem, int bucket_index,
                                     void *raw_entry, void *raw_sip_entry);
extern int _soc_alpm_raw_bucket_write(int u, soc_mem_t mem, int bucket_index,
                                      uint32 bank_disable,
                                      void *raw_entry, void *raw_sip_entry,
                                      int entry_cnt);
extern void _soc_alpm_raw_mem_read(int unit, soc_mem_t mem, void *raw,
                                   int ent_id, void *entry);
extern void _soc_alpm_raw_mem_write(int unit, soc_mem_t mem, void *raw,
                                    int index, int ent_id, void *entry);
extern void _soc_alpm_raw_parity_set(int u, soc_mem_t mem, void *alpm_data);

extern uint32 soc_alpm_bank_dis(int u, int vrf);
extern int _soc_alpm_bkt_entry_cnt(int u, soc_mem_t mem);
extern int _soc_alpm_128_lpm_free_entries(int u, int vrf);
extern int _soc_alpm_128_bucket_merge(int u, soc_mem_t mem, int pivot_idx_from,
                                      int pivot_idx_to, soc_alpm_merge_type_t merge_dir,
                                      int *shuffled);
extern int _soc_alpm_128_bucket_repartition(int u, soc_mem_t mem, int pivot_idx_from,
                                            int pivot_idx_to, trie_node_t **new_trie,
                                            int *shuffled);

/* For TH */
extern void soc_th_alpm_dbg_urpf(int u);
extern int _soc_th_alpm_mem_prefix_array_cb(trie_node_t *node, void *info);
extern int soc_th_alpm_hit_bits_update(int u, alpm_mem_prefix_array_t *pa, int *new_idx,
                                       int ipv6, int vrf);
extern int _soc_th_alpm_aux_op(int u, _soc_aux_op_t aux_op,
                               defip_aux_scratch_entry_t *aux_entry, int update_scratch,
                               int *hit, int *tcam_index, int *bktid , int aux_flags);
extern int _soc_th_alpm_rpf_entry(int u, int idx);
extern int _soc_th_alpm_find_in_bkt(int u, soc_mem_t mem, int bktid,
                                    int bank_disable, uint32 *e, void *alpm_data,
                                    int *key_index, int v6);
extern int _soc_th_alpm_insert_in_bkt(int u, soc_mem_t mem, int bktid,
                                      int bank_disable, void *alpm_data, void *alpm_sip_data,
                                      uint32 *e, int *key_index, int v6);
extern int _soc_th_alpm_delete_in_bkt(int u, soc_mem_t mem, int delete_bucket,
                                      int bank_disable, void *bufp2, uint32 *e,
                                      int *key_index, int v6);
extern int soc_th_alpm_128_init(int u);
extern int soc_th_alpm_128_state_clear(int u);
extern int soc_th_alpm_128_deinit(int u);
extern int soc_th_alpm_128_lpm_init(int u);
extern int soc_th_alpm_128_lpm_deinit(int u);
extern int soc_th_alpm_physical_idx(int u, soc_mem_t mem, int index, int full);
extern int soc_th_alpm_logical_idx(int u, soc_mem_t mem, int index, int full);
extern int _soc_th_alpm_mem_index(int u, soc_mem_t mem, int phy_bkt,
                                  int offset, uint32 bank_disable, int *key_index);

extern void soc_th_alpm_bank_dis(int u, int vrf, uint32 *b4, uint32 *b8);
extern int _soc_th_alpm_bkt_entry_cnt(int u, soc_mem_t alpm_mem);

/* For bucket sharing */
extern int _soc_th_alpm_128_bucket_merge(int u, soc_mem_t mem, int log_bkt_from,
                                         int log_bkt_to, soc_alpm_merge_type_t merge_dir,
                                         int *shuffled);
extern int _soc_th_alpm_128_lpm_free_entries(int u, int vrf);
extern int _soc_th_alpm_128_bucket_repartition(int u, soc_mem_t mem, int log_bkt_from,
                                               int log_bkt_to, int max_count,
                                               trie_node_t **new_trie,
                                               int *shuffled);
extern void soc_th_alpm_bucket_usage_incr(int u, int log_bkt, int pivot_index, int vrf, soc_mem_t mem, int diff);
extern void soc_th_alpm_bucket_usage_decr(int u, int log_bkt, int clear, int vrf, soc_mem_t mem, int diff, int log_bkt_to);

#endif /* _ESW_TRIDENT2_ALPM_INT_H */
