/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    alpm.c
 * Purpose: Primitives for LPM management in ALPM - Mode.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <shared/bsl.h>
#include <soc/l3x.h>
#ifdef ALPM_ENABLE
#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>
#include <soc/esw/alpm_trie_v6.h>



#define ALPM_REPART_DEBUG
#define ALPM_USE_RAW_VIEW
/* indicate whether HIT flag is supported */

#define SHR_SHIFT_LEFT(val, count) \
    (((count) == 32) ? 0 : (val) << (count))

#define SHR_SHIFT_RIGHT(val, count) \
    (((count) == 32) ? 0 : (val) >> (count))

#define SOC_ALPM_LPM_CACHE_FIELD_CREATE(m, f) soc_field_info_t * m##f
#define SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(m_u, s, m, f) \
              (s)->m##f = soc_mem_fieldinfo_get(m_u, m, f)

/* Add support for Extended based on Flex Counter support type */
#define SOC_ALPM_TABLE_MEM(_u_, _v6_, _mem_, _flex_)             \
    _mem_ = (_v6_) ? ((_flex_) ? L3_DEFIP_ALPM_IPV6_64_1m : L3_DEFIP_ALPM_IPV6_64m) \
                   : ((_flex_) ? L3_DEFIP_ALPM_IPV4_1m : L3_DEFIP_ALPM_IPV4m)

/* Add suport for Extended */
#define SOC_ALPM_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v6ent_)  \
            _buf_ = ((_v6_) ? ((uint32 *)&(_v6ent_)) : ((uint32 *)&(_v4ent_)))


/* this is a left shift of the prefix by 32/64 - length  and convert to
   brcm order of ip words */
#define ALPM_TRIE_TO_NORMAL_IP(prefix, length, v6) \
do {\
    if (!(v6)) {\
        prefix[0] = SHR_SHIFT_LEFT(prefix[1], 32 - length);\
        prefix[1] = 0;\
    } else {\
        int macro_pfx_shift = 64 - length;\
        int macro_tmp;\
        if (macro_pfx_shift < 32) {\
            /* get upper half shifted */\
            macro_tmp = prefix[3] << macro_pfx_shift;\
            /* isolate shifted stuff off lower half */\
            macro_tmp |= SHR_SHIFT_RIGHT(prefix[4], 32 - macro_pfx_shift);\
            /* swap the order of words */\
            prefix[0] = prefix[4] << macro_pfx_shift;\
            prefix[1] = macro_tmp;\
            prefix[2] = prefix[3] = prefix[4] = 0;\
        } else {\
            prefix[1] = SHR_SHIFT_LEFT(prefix[4], macro_pfx_shift - 32);\
            prefix[0] = prefix[2] = prefix[3] = prefix[4] = 0;\
        }\
    }\
} while (0)


soc_alpm_bucket_t soc_alpm_bucket[SOC_MAX_NUM_DEVICES];
int l3_alpm_sw_prefix_lookup[SOC_MAX_NUM_DEVICES];
_soc_alpm_ctrl_t soc_alpm_control[SOC_MAX_NUM_DEVICES] = {{0}};
_alpm_int_dbg_cnt_t soc_alpm_dbg_cnt[SOC_MAX_NUM_DEVICES];

void soc_alpm_lpm_state_dump(int u);
static int soc_alpm_lpm_init(int u);
static int soc_alpm_lpm_deinit(int u);
static int soc_alpm_lpm_insert(int u, void *entry_data, int *index,
                               int src_default, int src_discard, int bpm_len);
static int soc_alpm_lpm_delete(int u, void *key_data);
static int _soc_alpm_fill_aux_entry_for_op(int u,
               void *key_data,
               int ipv6,    /* Entry is ipv6. */
               int db_type, /* database type */
               int ent_type,
               int replace_len, /* used for DElete propagate */
               defip_aux_scratch_entry_t *aux_entry);

static int _soc_alpm_lpm_match(int u,
               void *key_data,
               void *e,         /* return entry data if found */
               int *index_ptr,  /* return key location */
               int *pfx_len,    /* Key prefix length. vrf + 32 + prefix len for IPV6*/
               int *ipv6);       /* Entry is ipv6. */
static int soc_alpm_lpm_match(int u,
               void *key_data,
               void *e,         /* return entry data if found */
               int *index_ptr);  /* return key location */
static int soc_alpm_shared_mem_init(int u);
static int _soc_alpm_mem_ent_init(int unit, void *lpm_entry, 
                       void *alpm_entry, void *alpm_sip_entry, soc_mem_t mem,
                       uint32 src_flags, uint32 *default_route);
static int
_soc_alpm_lpm_ent_init(int unit, void *alpm_entry, soc_mem_t mem, int ipv6, int vrf_id,
                       int bucket, int index, void *lpm_entry, int flex);
static int
_soc_alpm_lpm_ent_key_init(int unit, uint32 *key, int len, int vrf, int ipv6,
                      defip_entry_t *lpm_entry, int init);

static int soc_alpm_vrf_delete(int u, int vrf, int v6);
static int _soc_alpm_lpm_prefix_length_get(int u, void *entry, int *pfx_len);


#define FB_LPM_HASH_SUPPORT 1

typedef struct soc_alpm_lpm_state_s {
    int start;  /* start index for this prefix length */
    int end;    /* End index for this prefix length */
    int prev;   /* Previous (Lo to Hi) prefix length with non zero entry count*/
    int next;   /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;   /* valid entries */
    int fent;   /* free entries */
} soc_alpm_lpm_state_t, *soc_alpm_lpm_state_p;

#define SOC_MEM_COMPARE_RETURN(a, b) {          \
        if ((a) < (b)) { return -1; }           \
        if ((a) > (b)) { return  1; }           \
}

/* Can move to SOC Control structures */
static soc_alpm_lpm_state_p soc_alpm_lpm_state[SOC_MAX_NUM_DEVICES];

#define ALPM_INVALID_INDEX      -1
#define CONVERT_NONE            -1
#define WRITE_PIVOT_WITH_SRC_INDEX                0
#define WRITE_PIVOT_WITH_SRC_INDEX_CONVERT        1
#define WRITE_PIVOT_WITH_URPF_PARAMS              2
#define WRITE_PIVOT_WITHOUT_INSERT_HASH           0x80000000


#define ALPM_IPV6_PFX_ZERO               33
#define ALPM_MAX_VRF_PFX_ENTRIES         (64 + 32 + 2 + 1)
#define ALPM_MAX_VRF_PFX_INDEX           (ALPM_MAX_VRF_PFX_ENTRIES - 1)
#define ALPM_MAX_PFX_ENTRIES             (3 * ALPM_MAX_VRF_PFX_ENTRIES)
#define ALPM_MAX_PFX_INDEX               (ALPM_MAX_PFX_ENTRIES - 1)
#define SOC_ALPM_PFX_IS_V4(u, pfx)       (((pfx) % ALPM_MAX_VRF_PFX_ENTRIES) < ALPM_IPV6_PFX_ZERO)
#define SOC_ALPM_PFX_IS_V6_64(u, pfx)    (((pfx) % ALPM_MAX_VRF_PFX_ENTRIES) >= ALPM_IPV6_PFX_ZERO && \
                                          (pfx) != ALPM_MAX_VRF_PFX_INDEX)

#define SOC_ALPM_LPM_INIT_CHECK(u)        (soc_alpm_lpm_state[(u)] != NULL)
#define SOC_ALPM_LPM_STATE(u)             (soc_alpm_lpm_state[(u)])
#define SOC_ALPM_LPM_STATE_START(u, pfx)  (soc_alpm_lpm_state[(u)][(pfx)].start)
#define SOC_ALPM_LPM_STATE_END(u, pfx)    (soc_alpm_lpm_state[(u)][(pfx)].end)
#define SOC_ALPM_LPM_STATE_PREV(u, pfx)  (soc_alpm_lpm_state[(u)][(pfx)].prev)
#define SOC_ALPM_LPM_STATE_NEXT(u, pfx)  (soc_alpm_lpm_state[(u)][(pfx)].next)
#define SOC_ALPM_LPM_STATE_VENT(u, pfx)  (soc_alpm_lpm_state[(u)][(pfx)].vent)
#define SOC_ALPM_LPM_STATE_FENT(u, pfx)  (soc_alpm_lpm_state[(u)][(pfx)].fent)

typedef struct soc_lpm_field_cache_s {
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, CLASS_ID0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, CLASS_ID1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DST_DISCARD0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DST_DISCARD1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_COUNT0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_COUNT1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_PTR0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_PTR1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_ROUTE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_ROUTE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MULTICAST_ROUTE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MULTICAST_ROUTE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPA_ID0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPA_ID1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, EXPECTED_L3_IIF0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, EXPECTED_L3_IIF1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, L3MC_INDEX0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, L3MC_INDEX1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, HIT0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, HIT1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, NEXT_HOP_INDEX0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, NEXT_HOP_INDEX1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, PRI0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, PRI1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VALID0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VALID1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_HIGH0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_HIGH1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ALG_HIT_IDX0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ALG_HIT_IDX1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ALG_BKT_PTR0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ALG_BKT_PTR1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DEFAULT_MISS0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DEFAULT_MISS1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_POOL_NUMBER0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_POOL_NUMBER1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_OFFSET_MODE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_VIEW0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_VIEW1f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, SRC_DISCARD0f);
    SOC_ALPM_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, SRC_DISCARD1f);

} soc_lpm_field_cache_t, *soc_lpm_field_cache_p;
static soc_lpm_field_cache_p soc_lpm_field_cache_state[SOC_MAX_NUM_DEVICES];

#define SOC_MEM_OPT_F32_GET(m_unit, m_mem, m_entry_data, m_field) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->m_mem##m_field))

#define SOC_MEM_OPT_F32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->m_mem##m_field), (m_val))


#define SOC_MEM_OPT_FIELD_VALID(m_unit, m_mem, m_field) \
                ((soc_lpm_field_cache_state[(m_unit)]->m_mem##m_field) != NULL)


#ifdef FB_LPM_HASH_SUPPORT
typedef struct _soc_alpm_lpm_hash_s {
    int         unit;
    int         entry_count;    /* Number entries in hash table */
    int         index_count;    /* Hash index max value + 1 */
    uint16      *table;         /* Hash table with 16 bit index */
    uint16      *link_table;    /* To handle collisions */
} _soc_alpm_lpm_hash_t;

typedef uint32 _soc_alpm_lpm_hash_entry_t[5];
typedef int (*_soc_alpm_lpm_hash_compare_fn)(_soc_alpm_lpm_hash_entry_t key1,
                                           _soc_alpm_lpm_hash_entry_t key2);
static _soc_alpm_lpm_hash_t *_fb_lpm_hash_tab[SOC_MAX_NUM_DEVICES];
#define SOC_ALPM_LPM_STATE_HASH(u)           (_fb_lpm_hash_tab[(u)])

#define TD2_ALPM_HASH_INDEX_NULL (0xFFFF)
#define TD2_ALPM_HASH_INDEX_MASK (0x7FFF)
#define TD2_ALPM_HASH_IPV6_MASK  (0x8000)

#define SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)                 \
    odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);     \
    odata[1] =                                                               \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
    odata[2] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);     \
    odata[3] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);\
    if ( (!(SOC_IS_HURRICANE(u))) &&   \
        (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                \
        int m_vrf;\
        (void) soc_alpm_lpm_vrf_get(u, entry_data, (int *)&odata[4], &m_vrf); \
    } else {                                                                 \
        odata[4] = 0;                                                        \
    }                                                                       


#define SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, odata)               \
    odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);     \
    odata[1] =                                                               \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
    odata[2] = 0;                                                            \
    odata[3] = 0x80000001;                                                   \
    if ((!(SOC_IS_HURRICANE(u))) && \
        (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                \
        int m_vrf;\
        (void) soc_alpm_lpm_vrf_get(u, entry_data, (int *)&odata[4], &m_vrf); \
    } else {                                                                 \
        odata[4] = 0;                                                        \
    }                                                                       

#define SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, odata)               \
    odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);     \
    odata[1] =                                                               \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);       \
    odata[2] = 0;                                                            \
    odata[3] = 0x80000001;                                                   \
    if ((!(SOC_IS_HURRICANE(u))) &&  \
    (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f))) {                    \
        int m_vrf;\
        defip_entry_t m_tmp_ipv4;\
        (void )soc_alpm_lpm_ip4entry1_to_0(u, entry_data, &m_tmp_ipv4, 0);    \
        (void) soc_alpm_lpm_vrf_get(u, &m_tmp_ipv4, (int *)&odata[4], &m_vrf); \
    } else {                                                                 \
        odata[4] = 0;                                                        \
    }                                                                       

#define SOC_FB_LPM_HASH_ENTRY_GET _soc_alpm_lpm_hash_entry_get

static
void _soc_alpm_lpm_hash_entry_get(int u, void *e,
                                int index, _soc_alpm_lpm_hash_entry_t r_entry,
                                int *v);
static
uint16 _soc_alpm_lpm_hash_compute(uint8 *data, int data_nbits);
static
int _soc_alpm_lpm_hash_create(int unit,
                            int entry_count,
                            int index_count,
                            _soc_alpm_lpm_hash_t **fb_lpm_hash_ptr);
static
int _soc_alpm_lpm_hash_destroy(_soc_alpm_lpm_hash_t *fb_lpm_hash);
static
int _soc_alpm_lpm_hash_lookup(_soc_alpm_lpm_hash_t          *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t    entry,
                            int                         pfx,
                            uint16                      *key_index);
static
int _soc_alpm_lpm_hash_insert(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 old_index,
                            uint16 new_index,
                            uint32 *rvt_index);
static
int _soc_alpm_lpm_hash_revert(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            uint16 new_index,
                            uint32 rvt_index);
static
int _soc_alpm_lpm_hash_delete(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 delete_index);

static
int _soc_alpm_lpm_hash_verify(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 index);

/* Use MAX_VRF_ID to store VRF_OVERRIDE routes debug info */
alpm_vrf_handle_t *alpm_vrf_handle[SOC_MAX_NUM_DEVICES];

alpm_pivot_t **tcam_pivot[SOC_MAX_NUM_DEVICES];

STATIC int
_soc_alpm_sw_propagate_required(int unit)
{
    int i, mem_cnt;
    soc_mem_t mem_chk[] = {
        L3_DEFIPm,
        L3_DEFIP_PAIR_128m,
        L3_DEFIP_AUX_TABLEm
    };

    mem_cnt = sizeof(mem_chk) / sizeof(mem_chk[0]);
    for (i = 0; i < mem_cnt; i++) {
        if (soc_mem_cache_get(unit, mem_chk[i], MEM_BLOCK_ANY)) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC int
_soc_alpm_ctrl_init(int u)
{
    sal_memset(&ALPM_CTRL(u), 0, sizeof(_soc_alpm_ctrl_t));

    ALPM_CTRL(u).trie_propagate = _soc_alpm_sw_propagate_required(u);
    ALPM_CTRL(u).hit_bits_skip = soc_property_get(u, spn_L3_ALPM_HIT_SKIP, 0);

    ALPM_CTRL(u).fld.key_mode0              = MODE0f;
    ALPM_CTRL(u).fld.key_mode1              = MODE1f;
    ALPM_CTRL(u).fld.key_mode_mask0         = MODE_MASK0f;
    ALPM_CTRL(u).fld.key_mode_mask1         = MODE_MASK1f;
    ALPM_CTRL(u).fld.key_type0              = ENTRY_TYPE0f;
    ALPM_CTRL(u).fld.key_type1              = ENTRY_TYPE1f;
    ALPM_CTRL(u).fld.key_type_mask0         = ENTRY_TYPE_MASK0f;
    ALPM_CTRL(u).fld.key_type_mask1         = ENTRY_TYPE_MASK1f;
    ALPM_CTRL(u).fld.key_mode0_upr          = MODE0_UPRf;
    ALPM_CTRL(u).fld.key_mode0_lwr          = MODE0_LWRf;
    ALPM_CTRL(u).fld.key_mode1_upr          = MODE1_UPRf;
    ALPM_CTRL(u).fld.key_mode1_lwr          = MODE1_LWRf;
    ALPM_CTRL(u).fld.key_mode_mask0_upr     = MODE_MASK0_UPRf;
    ALPM_CTRL(u).fld.key_mode_mask0_lwr     = MODE_MASK0_LWRf;
    ALPM_CTRL(u).fld.key_mode_mask1_upr     = MODE_MASK1_UPRf;
    ALPM_CTRL(u).fld.key_mode_mask1_lwr     = MODE_MASK1_LWRf;
    ALPM_CTRL(u).fld.key_type0_upr          = ENTRY_TYPE0_UPRf;
    ALPM_CTRL(u).fld.key_type0_lwr          = ENTRY_TYPE0_LWRf;
    ALPM_CTRL(u).fld.key_type1_upr          = ENTRY_TYPE1_UPRf;
    ALPM_CTRL(u).fld.key_type1_lwr          = ENTRY_TYPE1_LWRf;
    ALPM_CTRL(u).fld.key_type_mask0_upr     = ENTRY_TYPE_MASK0_UPRf;
    ALPM_CTRL(u).fld.key_type_mask0_lwr     = ENTRY_TYPE_MASK0_LWRf;
    ALPM_CTRL(u).fld.key_type_mask1_upr     = ENTRY_TYPE_MASK1_UPRf;
    ALPM_CTRL(u).fld.key_type_mask1_lwr     = ENTRY_TYPE_MASK1_LWRf;
    ALPM_CTRL(u).fld.key_mode               = MODEf;
    ALPM_CTRL(u).fld.key_type               = ENTRY_TYPEf;

    ALPM_CTRL(u).ctrl_inited = 1;
    return SOC_E_NONE;
}

STATIC int
_soc_alpm_sw_propagate(int u, trie_t *pivot, void *prefix, uint32 len,
                          int insert, void *cb, void *user_data)
{
    int rv = SOC_E_NONE;
    uint32 pivot_len = 0;
    trie_node_t *lpm_pvt = NULL;
    trie_bpm_cb_info_t cb_info;

    cb_info.pfx = (void *)prefix;
    cb_info.len = len;
    cb_info.user_data = user_data;

    /* Find longest prefix match(pivot) of route */
    SOC_IF_ERROR_RETURN(trie_find_lpm(pivot, prefix, len, &lpm_pvt));
    pivot_len = (TRIE_ELEMENT_GET(payload_t *, lpm_pvt, node))->len;

    if (pivot->v6_key) {
        rv = pvt_trie_v6_propagate_prefix(lpm_pvt, pivot_len,
                                          (void *)prefix, len, cb, &cb_info);
    } else {
        rv = pvt_trie_propagate_prefix(lpm_pvt, pivot_len,
                                       (void *)prefix, len, cb, &cb_info);
    }

    return rv;
}

STATIC int
_soc_alpm_propagate_callback(trie_node_t *trie,
                             trie_bpm_cb_info_t *info)
{
    int rv = SOC_E_NONE;
    int i, unit, mode;
    trie_bpm_cb_user_data_t *user_data;
    void *aux_entry;
    alpm_pivot_t *pivot_pyld;
    int tcam_index, lpm_index, aux_index;
    int aux_flags;

    /* use memory size to declear replace_data field value */
    defip_aux_scratch_entry_t replace_data;
    soc_mem_t aux_mem = L3_DEFIP_AUX_SCRATCHm;

    /* L3_DEFIP_AUX_TABLE variables */
    defip_aux_table_entry_t shadow_entry;
    uint32 pfx_len, rpl_bpm_len, pvt_bpm_len;

#define _URPF_INDEX(urpf, index, mem) \
    (urpf ? (index + (soc_mem_index_count(unit, mem) >> 1)) : index)

    user_data = info->user_data;
    unit = user_data->unit;
    aux_entry = user_data->entry;
    aux_flags = user_data->flags;

    /* For uRPF case, DB_TYPE is odd, see soc_alpm_db_ent_type_encoding */
    if (soc_mem_field32_get(unit, aux_mem, aux_entry, DB_TYPEf) & 0x1) {
        return SOC_E_NONE;
    }

    pivot_pyld = (alpm_pivot_t *)trie;
    pvt_bpm_len = PIVOT_TCAM_BPMLEN(pivot_pyld);
    pfx_len = soc_mem_field32_get(unit, aux_mem, aux_entry, IP_LENGTHf);

    /* Addr & Mask has been screened by propagate function,
     * no need to do it again */
    if (user_data->aux_op == INSERT_PROPAGATE) {
        if (pvt_bpm_len > pfx_len) {
            /* Not qualified */
            return SOC_E_NONE;
        }
    }

    if (user_data->aux_op == DELETE_PROPAGATE) {
        if (pvt_bpm_len != pfx_len) {
            /* Not qualified */
            return SOC_E_NONE;
        }
    }

    rpl_bpm_len = soc_mem_field32_get(unit, aux_mem, aux_entry, REPLACE_LENf);
    if (user_data->aux_op == INSERT_PROPAGATE) {
        rpl_bpm_len = pfx_len;
    }

    /* Update bpm length accordingly */
    PIVOT_TCAM_BPMLEN(pivot_pyld) = rpl_bpm_len;

    /* update callback count, hw propagate is counting on this */
    user_data->count ++;

    sal_memset(&replace_data, 0, sizeof(replace_data));
    soc_mem_field_get(unit, aux_mem, aux_entry, REPLACE_DATAf,
                      (uint32 *)&replace_data);
    mode = soc_mem_field32_get(unit, aux_mem, aux_entry, MODEf);

    tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);
    if (mode == 0 || mode == 1) {
        soc_mem_t lpm_mem = L3_DEFIPm;
        defip_entry_t lpm_entry;

        /* Update L3_DEFIPm */
        lpm_index = soc_alpm_logical_idx(unit, lpm_mem, tcam_index >> 1, 1);
        aux_index = tcam_index >> 1;

        LOG_INFO(BSL_LS_SOC_ALPM,
            (BSL_META_U(unit, "ALPM SW %s ppg cb: lpm_mem %s index %d ent %d,"
            "AUX_TABLE index %d\n"),
            (user_data->aux_op == INSERT_PROPAGATE) ? "Add" : "Del",
            SOC_MEM_NAME(unit, lpm_mem), lpm_index,
            (tcam_index & 1), aux_index));

        soc_mem_lock(unit, lpm_mem);
        rv = _soc_mem_alpm_read_cache(unit, lpm_mem, MEM_BLOCK_ANY,
                                      lpm_index, &lpm_entry);
        if (SOC_SUCCESS(rv)) {
            if (mode == 1) {
                /* IPv6_64 */
                soc_mem_field_set(unit, lpm_mem, (uint32 *)&lpm_entry,
                    REPLACE_DATA0f, (uint32 *)&replace_data);
            } else {
                /* IPv4 */
                soc_mem_field_set(unit, lpm_mem, (uint32 *)&lpm_entry,
                    (tcam_index & 1) ? REPLACE_DATA1f : REPLACE_DATA0f,
                    (uint32 *)&replace_data);
            }
            _soc_mem_alpm_write_cache(unit, lpm_mem, MEM_BLOCK_ALL,
                                      lpm_index, &lpm_entry);
            if (SOC_URPF_STATUS_GET(unit)) {
                lpm_index = _URPF_INDEX(TRUE, lpm_index, L3_DEFIPm);
                (void)_soc_mem_alpm_read_cache(unit, lpm_mem, MEM_BLOCK_ANY,
                                               lpm_index, &lpm_entry);
                if (mode == 1) {
                    soc_mem_field_set(unit, lpm_mem, (uint32 *)&lpm_entry,
                        REPLACE_DATA0f, (uint32 *)&replace_data);
                    soc_mem_field32_set(unit, lpm_mem, &lpm_entry,
                        RPE0f, (aux_flags & SOC_ALPM_AUX_DEF_ROUTE) ? 1 : 0);
                    soc_mem_field32_set(unit, lpm_mem,
                        &lpm_entry, SRC_DISCARD0f,
                        (aux_flags & SOC_ALPM_AUX_SRC_DISCARD) ? 1 : 0);
                } else {
                    soc_mem_field_set(unit, lpm_mem, (uint32 *)&lpm_entry,
                        (tcam_index & 1) ? REPLACE_DATA1f : REPLACE_DATA0f,
                        (uint32 *)&replace_data);
                    soc_mem_field32_set(unit, lpm_mem, &lpm_entry,
                        (tcam_index & 1) ? RPE1f : RPE0f,
                        (aux_flags & SOC_ALPM_AUX_DEF_ROUTE) ? 1 : 0);
                    soc_mem_field32_set(unit, lpm_mem, &lpm_entry,
                        (tcam_index & 1) ? SRC_DISCARD1f : SRC_DISCARD0f,
                        (aux_flags & SOC_ALPM_AUX_SRC_DISCARD) ? 1 : 0);
                }
                _soc_mem_alpm_write_cache(unit, lpm_mem, MEM_BLOCK_ALL,
                                          lpm_index, &lpm_entry);
            }
        }
        soc_mem_unlock(unit, lpm_mem);

        /* Update L3_DEFIP_AUX_TABLEm */
        lpm_mem = L3_DEFIP_AUX_TABLEm;

        soc_mem_lock(unit, lpm_mem);
        rv = _soc_mem_alpm_read_cache(unit, lpm_mem, MEM_BLOCK_ANY,
                                      aux_index, &shadow_entry);
        if (SOC_SUCCESS(rv)) {
            if (mode == 1) { /* IPv6_64 */
                soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                    BPM_LENGTH0f, rpl_bpm_len);
                soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                    BPM_LENGTH1f, rpl_bpm_len);
            } else { /* IPv4 */
                soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                    (tcam_index & 1) ? BPM_LENGTH1f : BPM_LENGTH0f,
                    rpl_bpm_len);
            }
            _soc_mem_alpm_write_cache(unit, lpm_mem, MEM_BLOCK_ALL,
                                      aux_index, &shadow_entry);
            if (SOC_URPF_STATUS_GET(unit)) {
                aux_index = _URPF_INDEX(TRUE, aux_index, L3_DEFIP_AUX_TABLEm);
                (void)_soc_mem_alpm_read_cache(unit, lpm_mem, MEM_BLOCK_ANY,
                                               aux_index, &shadow_entry);
                if (mode == 1) { /* IPv6_64 */
                    soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                        BPM_LENGTH0f, rpl_bpm_len);
                    soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                        BPM_LENGTH1f, rpl_bpm_len);
                } else { /* IPv4 */
                    soc_mem_field32_set(unit, lpm_mem, &shadow_entry,
                        (tcam_index & 1) ? BPM_LENGTH1f : BPM_LENGTH0f,
                        rpl_bpm_len);
                }
                _soc_mem_alpm_write_cache(unit, lpm_mem, MEM_BLOCK_ALL,
                                          aux_index, &shadow_entry);
            }
        }
        soc_mem_unlock(unit, lpm_mem);

    } else if (mode == 3) {
        soc_mem_t lpm128_mem = L3_DEFIP_PAIR_128m;
        defip_pair_128_entry_t lpm128_entry;

        lpm_index = soc_alpm_logical_idx(unit, L3_DEFIP_PAIR_128m,
                        SOC_ALPM_128_DEFIP_TO_PAIR(unit, tcam_index >> 1), 1);
        aux_index = SOC_ALPM_128_DEFIP_TO_PAIR(unit, tcam_index >> 1);
        LOG_INFO(BSL_LS_SOC_ALPM,
            (BSL_META_U(unit, "ALPM SW %s ppg cb: lpm_mem %s index %d, "
            "AUX_TABLE index0 %d index1 %d\n"),
            (user_data->aux_op == INSERT_PROPAGATE) ? "Add" : "Del",
            SOC_MEM_NAME(unit, lpm128_mem), lpm_index,
            SOC_ALPM_128_ADDR_LWR(unit, aux_index),
            SOC_ALPM_128_ADDR_UPR(unit, aux_index)));

        /* Update L3_DEFIP_PAIR_128m */
        soc_mem_lock(unit, lpm128_mem);
        rv = _soc_mem_alpm_read_cache(unit, lpm128_mem, MEM_BLOCK_ANY,
                                      lpm_index, &lpm128_entry);
        if (SOC_SUCCESS(rv)) {
            soc_mem_field_set(unit, lpm128_mem, (uint32 *)&lpm128_entry,
                REPLACE_DATAf, (uint32 *)&replace_data);
            _soc_mem_alpm_write_cache(unit, lpm128_mem, MEM_BLOCK_ALL,
                                      lpm_index, &lpm128_entry);
            if (SOC_URPF_STATUS_GET(unit)) {
                lpm_index = _URPF_INDEX(TRUE, lpm_index, L3_DEFIP_PAIR_128m);
                (void)_soc_mem_alpm_read_cache(unit, lpm128_mem, MEM_BLOCK_ANY,
                                               lpm_index, &lpm128_entry);
                soc_mem_field_set(unit, lpm128_mem, (uint32 *)&lpm128_entry,
                    REPLACE_DATAf, (uint32 *)&replace_data);
                soc_mem_field32_set(unit, lpm128_mem, &lpm128_entry,
                    RPEf, (aux_flags & SOC_ALPM_AUX_DEF_ROUTE) ? 1 : 0);
                soc_mem_field32_set(unit, lpm128_mem,
                    &lpm128_entry, SRC_DISCARDf,
                    (aux_flags & SOC_ALPM_AUX_SRC_DISCARD) ? 1 : 0);
                _soc_mem_alpm_write_cache(unit, lpm128_mem, MEM_BLOCK_ALL,
                    lpm_index, &lpm128_entry);
            }
        }
        soc_mem_unlock(unit, lpm128_mem);

        /* Update L3_DEFIP_AUX_TABLEm */
        lpm128_mem = L3_DEFIP_AUX_TABLEm;

        soc_mem_lock(unit, lpm128_mem);
        /* Needs to update 2 AUX_TABLE entries */
        for (i = 0; i < 2; i++) {
            int index;
            index = ((i == 0) ?
                     SOC_ALPM_128_ADDR_LWR(unit, aux_index) :
                     SOC_ALPM_128_ADDR_UPR(unit, aux_index));
            rv = _soc_mem_alpm_read_cache(unit, lpm128_mem, MEM_BLOCK_ANY,
                                          index, &shadow_entry);
            if (SOC_SUCCESS(rv)) {
                soc_mem_field32_set(unit, lpm128_mem, &shadow_entry,
                    BPM_LENGTH0f, rpl_bpm_len);
                soc_mem_field32_set(unit, lpm128_mem, &shadow_entry,
                    BPM_LENGTH1f, rpl_bpm_len);
                _soc_mem_alpm_write_cache(unit, lpm128_mem, MEM_BLOCK_ALL,
                                          index, &shadow_entry);
                if (SOC_URPF_STATUS_GET(unit)) {
                    index = _URPF_INDEX(TRUE, index, lpm128_mem);
                    (void)_soc_mem_alpm_read_cache(unit, lpm128_mem,
                                MEM_BLOCK_ANY, index, &shadow_entry);
                    soc_mem_field32_set(unit, lpm128_mem, &shadow_entry,
                        BPM_LENGTH0f, rpl_bpm_len);
                    soc_mem_field32_set(unit, lpm128_mem, &shadow_entry,
                        BPM_LENGTH1f, rpl_bpm_len);
                    _soc_mem_alpm_write_cache(unit, lpm128_mem, MEM_BLOCK_ALL,
                                              index, &shadow_entry);
                }
            }
        }
        soc_mem_unlock(unit, lpm128_mem);
    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * ipv4:     ip6_word[4] =
 *               {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  ip6_word[4] =
 *               {0, 0, IP_ADDR0f, IP_ADDR1f}
 * ipv6-128: ip6_word[4] =
 *               {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
STATIC void
_soc_alpm_trie_prefix_create(int u, int mode, uint32 *ip6_word, uint32 len,
                                uint32 *pfx)
{
    int i;
    uint32 pfx_shift, tmp;

    pfx[0] = pfx[1] = pfx[2] = pfx[3] = pfx[4] = 0;
    /* Obtain prefix length */
    if (mode == 0) {
        pfx[1] = SHR_SHIFT_RIGHT(ip6_word[0], 32 - len);
        pfx[0] = 0;
    } else if (mode == 1) {
        pfx[4] = ip6_word[2];
        pfx[3] = ip6_word[3];

        pfx_shift = 64 - len;
        if (pfx_shift < 32) {
            /* get lower half shifted */
            pfx[4] >>= pfx_shift;
            /* isolate shifted stuff of upper half */
            tmp = SHR_SHIFT_LEFT(pfx[3], 32 - pfx_shift);
            pfx[3] >>= pfx_shift;
            pfx[4] |= tmp;
        } else {
            pfx[4] = SHR_SHIFT_RIGHT(pfx[3], pfx_shift - 32);
            pfx[3] = 0;
        }
    } else {
        /* PAIR_128 style, for both mode==2 and mode==3  */
        int start;
        uint32 tmp_pfx[5] = {0};
        tmp_pfx[0] = ip6_word[0];
        tmp_pfx[1] = ip6_word[1];
        tmp_pfx[2] = ip6_word[2];
        tmp_pfx[3] = ip6_word[3];

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
            tmp = SHR_SHIFT_LEFT(tmp, 32 - pfx_shift);
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

/* Create prefix & len in trie format from aux entry */
STATIC void
_soc_alpm_aux_prefix_create(int u, void *aux_entry, uint32 *pfx, uint32 *len)
{
    uint32 ip6_word[4];
    int mode;
    soc_mem_t aux_mem = L3_DEFIP_AUX_SCRATCHm;

    soc_mem_field_get(u, aux_mem, aux_entry, IP_ADDRf, (uint32 *)ip6_word);
    *len = soc_mem_field32_get(u, aux_mem, aux_entry, IP_LENGTHf);
    mode = soc_mem_field32_get(u, aux_mem, aux_entry, MODEf);

    return _soc_alpm_trie_prefix_create(u, mode, ip6_word, *len, pfx);
}

/* Software propagation */
STATIC int
_soc_alpm_aux_sw_op(int u, _soc_aux_op_t aux_op, void *aux_entry,
                    int *match_cnt, int aux_flags)
{
    trie_t *pivot;
    int rv = SOC_E_NONE;
    int vld, mode, dbt, vrf = 0;
    uint32 prefix[5];
    uint32 len;
    trie_bpm_cb_user_data_t user_data;
    soc_mem_t aux_mem = L3_DEFIP_AUX_SCRATCHm;
    int insert = 0;

    switch (aux_op) {
        case INSERT_PROPAGATE:
            insert = 1;
            break;
        case DELETE_PROPAGATE:
            insert = 0;
            break;
        case HITBIT_REPLACE:
        case PREFIX_LOOKUP:
        default:
            return SOC_E_NONE;
    }

    vld  = soc_mem_field32_get(u, aux_mem, aux_entry, VALIDf);
    mode = soc_mem_field32_get(u, aux_mem, aux_entry, MODEf);
    vrf  = soc_mem_field32_get(u, aux_mem, aux_entry, VRFf);
    dbt  = soc_mem_field32_get(u, aux_mem, aux_entry, DB_TYPEf);

    /* Need to distinguish vrf=0 and Global low */
    if ((vrf == 0) && (dbt <= 1)) {
        vrf = SOC_VRF_MAX(u) + 1;
    }

    /* Mode indicates entry type: 00:IPv4, 01: 64-Ipv6, 11: IPv6-128 */
    if (mode == 0) {
        pivot = VRF_PIVOT_TRIE_IPV4(u, vrf);
    } else if (mode == 1) {
        pivot = VRF_PIVOT_TRIE_IPV6(u, vrf);
    } else if (mode == 3) {
        pivot = VRF_PIVOT_TRIE_IPV6_128(u, vrf);
    } else {
        return SOC_E_INTERNAL;
    }

    _soc_alpm_aux_prefix_create(u, aux_entry, (void *)prefix, &len);

    sal_memset(&user_data, 0, sizeof(user_data));
    user_data.unit     = u;
    user_data.entry    = aux_entry;
    user_data.aux_op   = aux_op;
    user_data.flags    = aux_flags;

    if (vld && pivot) {
        rv = _soc_alpm_sw_propagate(u, pivot, prefix, len, insert,
                                    _soc_alpm_propagate_callback, &user_data);
    }

    if (insert) {
        soc_alpm_dbg_cnt[u].ppg_cb_insert += user_data.count;
    } else {
        soc_alpm_dbg_cnt[u].ppg_cb_delete += user_data.count;
    }

    if (match_cnt != NULL) {
        *match_cnt = user_data.count;
    }

    return rv;

}


/* returns 0 for Combined Search mode and 1 for Parallel Search mode */
int
soc_alpm_mode_get(int u)
{
    if (soc_trident2_alpm_mode_get(u) == 1) {
        return 1;
    } else {
        return 0;
    }
}


int
_soc_alpm_mask_len_get(int u, int ipv6, void *entry, int *pfx)
{
    int     rv;
    uint32  ipv4a;

    if (ipv6) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
            return(rv);
        }
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK1f);
        if (*pfx) {
            if (ipv4a != 0xffffffff)  {
                return(SOC_E_PARAM);
            }
            *pfx += 32;
        } else {
            if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
                return(rv);
            }
        }
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
            return(rv);
        }
    }

    return SOC_E_NONE;
}

/**
 * Tomahawk & Trident2+ ALPM VRF[10], VRF[11] bit WAR, Trident2 no change.
 * Need to handle Warmboot as well.
 *
 * TH's Pivot Scan Engine is not comparing bit 10 of the VRF[10:0].
 * TD2+ also has this issue, it's Pivot Scan Engine is not comparing
 * bits 11:10 of VRF[11:0]
 *
 * Proposed WAR:
 * New DB_TYPE encodings. In TD2+ we sacrifice FCOE(ENTRY_TYPE=1) entry support
 * to get 4K VRF support.
 * 
 * Tips for future FCoE implementation:
 *   Mark VALID0/1 bits to 0 for FCoE entries, so we don't actually care 
 *   ENTRY_TYPE bits in L3_DEFIP_AUX_TABLE, like Global High routes.
 * 
 * ----------------------------------------------------------
 * |                        | No uRPF       | uRPF          |
 * ----------------------------------------------------------
 * | DB_TYPE(Global)        | 0             | 1             |
 * ----------------------------------------------------------
 * | DB_TYPE(VRF0-1023)     | 2             | 3             |    TD2
 * -------------------------------------------------------------------
 * | DB_TYPE(VRF1024-2047)  | 4             | 5             |    TH
 * -------------------------------------------------------------------
 * | DB_TYPE(VRF2048-3071)  | 6             | 7             |
 * ----------------------------------------------------------
 * | DB_TYPE(VRF3072-4095)  | 4+EntType=1   | 5+EntType=1   |    TD2+
 * -------------------------------------------------------------------
 */
void
soc_alpm_db_ent_type_encoding(int u, int vrf, uint32 *db_type, uint32 *ent_type)
{
    int dbt, ent, vrf_msb;
    uint32 db_type_conf[4] = {2, 4, 6, 4};
    uint32 en_type_conf[4] = {0, 0, 0, 1};

    /* Global Low routes always use db_type 0 */
    if (vrf == (SOC_VRF_MAX(u) + 1)) {
        dbt = 0;
        ent = 0;
    } else if (SOC_IS_TRIDENT2PLUS(u) || SOC_IS_TOMAHAWKX(u)) {
        /* TD2+ and TH */
        vrf_msb = (vrf >> 10) & 0x3;
        dbt = db_type_conf[vrf_msb];
        ent = en_type_conf[vrf_msb];
    } else {
        /* other chips including TD2 (TD2,TH+,TH2,AP...) */
        dbt = 2;
        ent = 0;
    }

    if (db_type != NULL) {
        *db_type = dbt;
    }

    if (ent_type != NULL) {
        *ent_type = ent;
    }

    return ;
}

int
_soc_alpm_rpf_entry(int u, int idx)
{
    int bkt;
    bkt = (idx >> 2) & 0x3fff;
    bkt += SOC_ALPM_BUCKET_COUNT(u);
    /* form rpf alpm entry index */
    return (idx & ~(0x3fff << 2)) | (bkt << 2);
}

/* for v4 full = 0, for v6-64 full = 1 */
int
soc_alpm_physical_idx(int u, soc_mem_t mem, int index, int full)
{
    int tmp = index & 1;

    if (full) {
        return soc_trident2_l3_defip_index_map(u, mem, index);
    }
    index >>= 1;
    index = soc_trident2_l3_defip_index_map(u, mem, index);
    index <<= 1;
    index |= tmp;
    return index;
}

int
soc_alpm_logical_idx(int u, soc_mem_t mem, int index, int full)
{
    int tmp = index & 1;

    if (full) {
        return soc_trident2_l3_defip_index_remap(u, mem, index);
    }
    index >>= 1;
    index = soc_trident2_l3_defip_index_remap(u, mem, index);
    index <<= 1;
    index |= tmp;
    return index;
}

static void
_soc_alpm_pfx_len_to_mask(int u, void *lpm_entry, int len, int ipv6)
{
    uint32 mask = 0;

    if (ipv6) {
        if (len >= 32) {
            mask = 0xffffffff;
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, mask);
            mask = ~SHR_SHIFT_RIGHT(0xffffffff, len - 32);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, mask);
        } else {
            mask = ~(0xffffffff >> len);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, mask);
            /* make sure lower word of mask is 0 */
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, 0);
        }
    } else {
        assert(len <= 32);
        mask = (len == 32) ? 0xffffffff : ~(0xffffffff >> len);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, mask);
    }
}
/* Create Prefix */
/* Prefix is created based on the trie key packing expectations */
/* Prefix[0] will have higher order Word, Prefix[1] will have lower order word
 * in case of IPV6. For IPV4 prefix[0] will contain the prefix
 */
static int
_alpm_prefix_create(int u, void *entry, uint32 *prefix, uint32 *pfx_len, 
                    int *default_route)
{
    int ip, ip_mask, ipv6;
    int pfx = 0;
    int rv = SOC_E_NONE;
    uint32 pfx_shift, tmp;

    prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;

    ipv6 = soc_mem_field32_get(u, L3_DEFIPm, entry, MODE0f);

    /* Form Prefix based on the entry */
    ip = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR0f);
    ip_mask = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);

    prefix[1] = ip;

    ip = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR1f);
    ip_mask = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR_MASK1f);

    prefix[0] = ip;

    /* Obtain prefix length */
    if (ipv6) {
        prefix[4] = prefix[1];
        prefix[3] = prefix[0];
        prefix[1] = prefix[0] = 0;
        ip_mask = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ip_mask, &pfx)) < 0) {
            return(rv);
        }
        ip_mask = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR_MASK1f);
        if (pfx) {
            if (ip_mask != 0xffffffff)  {
                return(SOC_E_PARAM);
            }
            pfx += 32;
        } else {
            if ((rv = _ipmask2pfx(ip_mask, &pfx)) < 0) {
                return(rv);
            }
        }
        pfx_shift = 64 - pfx;
        if (pfx_shift < 32) {
            /* get lower half shifted */
            prefix[4] >>= pfx_shift;
            /* isolate shifted stuff of upper half */
            tmp = SHR_SHIFT_LEFT(prefix[3], 32 - pfx_shift);
            prefix[3] >>= pfx_shift;
            prefix[4] |= tmp;
        } else {
            prefix[4] = SHR_SHIFT_RIGHT(prefix[3], pfx_shift - 32);
            prefix[3] = 0;
        }
    } else {
        ip_mask = soc_mem_field32_get(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ip_mask, &pfx)) < 0) {
            return(rv);
        }
        prefix[1] = SHR_SHIFT_RIGHT(prefix[1], 32 - pfx);
        prefix[0] = 0;
    }
    *pfx_len = pfx;
    *default_route = (prefix[0] == 0) && (prefix[1] == 0) && (pfx == 0);
    return SOC_E_NONE;
}

#if 0
extern int sh_process_command(int u,char * c);
extern void soc_alpm_lpm_sw_dump(int unit);
void soc_debug_collect(int u, int vrf, int v6, soc_mem_t mem, void *entry)
{
    int i;

    sh_process_command(u, "debug =");

    LOG_CLI(("\n====Collecting info for unit(%d), vrf(%d), v6(%d), mem(%s), entry(%p)\n", 
            u, vrf, v6, SOC_MEM_NAME(u, mem), entry));

    /* Dump system configuration */
    LOG_CLI(("\n====Dumping current config.bcm\n"));
    sh_process_command(u, "config show");

    /* Debug counter */
    LOG_CLI(("\n====Dumping ALPM debug counters\n"));
    for (i = 0; i < DEBUG_VAR_NUMBER; i++) {
        if (debug_var2[i] != 0) {
            LOG_CLI(("debug_var2[%2d] = %d\n", i, debug_var2[i]));
        }
    }

    /* Dump current entry */
    if (entry != NULL) {
        LOG_CLI(("\n====Dumping ALPM current operating entry\n"));
        soc_mem_entry_dump_if_changed(u, mem, entry, "ALPM-DBG: ");
    }

    /* Dump ALPM Memories */
    LOG_CLI(("\n====Dumping ALPM memories\n"));
    sh_process_command(u, "d chg L3_DEFIP");
    sh_process_command(u, "d chg L3_DEFIP_pair_128");
    sh_process_command(u, "d chg L3_DEFIP_aux_table");
    sh_process_command(u, "d chg L3_DEFIP_AUX_SCRATCH");
    sh_process_command(u, "d chg L3_DEFIP_alpm_ipv4");
    sh_process_command(u, "d chg L3_DEFIP_alpm_ipv6_64");
    sh_process_command(u, "d chg L3_DEFIP_alpm_ipv6_128");

    /* Dump Pivot trie */
    LOG_CLI(("\n====Dumping ALPM Pivot trie-tree for VRF %d\n", vrf));
    trie_dump(v6 ? VRF_PIVOT_TRIE_IPV6_128(u, vrf) : VRF_PIVOT_TRIE_IPV4(u, vrf), 
              NULL, NULL);

    /* Dump VRF trie */
    LOG_CLI(("\n====Dumping VRF route trie-tree for VRF %d\n", vrf));
    trie_dump(v6 ? VRF_PREFIX_TRIE_IPV6_128(u, vrf) : VRF_PREFIX_TRIE_IPV4(u, vrf), 
              NULL, NULL);

    /* Dump Bucket trie */
    LOG_CLI(("\n====Dumping Bucket trie-tree for VRF %d\n", vrf));
    for (i = 0; i < MAX_PIVOT_COUNT; i++) {
        if (ALPM_TCAM_PIVOT(u, i) != NULL) {
            trie_t *trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT(u, i));
            LOG_CLI(("\t==Dumping Trie-tree for TCAM %d, index %d\n", i, 
                    soc_alpm_logical_idx(u, L3_DEFIPm, i >> 1, 1)));
            trie_dump(trie, NULL, NULL);
        }
    }

    /* Dump ALPM LPM SW state */
    LOG_CLI(("\n====Dumping ALPM LPM SW state\n"));
    soc_alpm_lpm_sw_dump(u);

    /* Running Sanity check */
    LOG_CLI(("\n====Running Bucket Sanity check\n"));
    soc_alpm_sanity_check(u, L3_DEFIPm, -1);

    return ;
}
#endif

int
_soc_alpm_find_in_bkt(int u, soc_mem_t mem, int bucket_index, int bank_disable,
                      uint32 *e, void *alpm_data, int *key_index, int v6)
{
    int rv;
    rv = soc_mem_alpm_lookup(u, mem, bucket_index,
                             MEM_BLOCK_ANY,
                             bank_disable,
                             e, alpm_data, key_index);
    if (SOC_SUCCESS(rv)) {
        return rv;
    }
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        return soc_mem_alpm_lookup(u, mem, bucket_index + 1, MEM_BLOCK_ANY,
                             bank_disable, e, alpm_data, key_index);
    }
    return rv;
}


static int
_soc_alpm_sw_pivot_find(int u, uint32 *prefix, uint32 length, int v6, int vrf, 
                        int *hit, int *tcam_index, int *bucket_index)
{
    int rv = SOC_E_NONE;
    trie_t  *pivot_trie;
    trie_node_t *lpmp = NULL;
    alpm_pivot_t *pivot_pyld;

    if (v6) {
        pivot_trie = VRF_PIVOT_TRIE_IPV6(u, vrf);
    } else {
        pivot_trie = VRF_PIVOT_TRIE_IPV4(u, vrf);
    }
    rv = trie_find_lpm(pivot_trie, prefix, length, &lpmp);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Pivot find failed\n")));
        return rv;
    }
    pivot_pyld = (alpm_pivot_t *)lpmp;
    
    *hit = 1;
    *tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);
    *bucket_index = PIVOT_BUCKET_INDEX(pivot_pyld);
    
    return SOC_E_NONE;
}

uint32
soc_alpm_bank_dis(int u, int vrf)
{
    uint32 bank_disable;
    int global = (vrf == SOC_VRF_MAX(u) + 1) ? 1 : 0;
    if (soc_alpm_mode_get(u) == SOC_ALPM_MODE_PARALLEL &&
        SOC_URPF_STATUS_GET(u)) {
        bank_disable = global ? 0x3 : 0xC;
    } else {
        bank_disable = 0;
    }
    return bank_disable;
}

int
_soc_alpm_sw_prefix_check(int u,
                          soc_mem_t mem,      /* Mem type for ALPM */
                          int v6,
                          void *key_data,     /* TCAM entry data */
                          int vrf_id,
                          int vrf)
{
    int         rv = SOC_E_NONE;
    uint32 prefix[5], length;
    int default_route = 0;
    trie_t  *pivot_trie, *bkt_trie;
    trie_node_t *lpmp = NULL;
    alpm_pivot_t *pivot_node;

    if (vrf_id == 0) {
        if (soc_alpm_mode_get(u)) {
            /* cannot have 0 as a VRF in parallel mode */
            return SOC_E_PARAM;
        }
    }

    if (vrf_id != SOC_L3_VRF_OVERRIDE) {
        /* Create Prefix */
        rv = _alpm_prefix_create(u, key_data, prefix, &length,
                                 &default_route);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_sw_prefix_check: prefix create failed\n")));
            return rv;
        }

        if (v6) {
            pivot_trie = VRF_PIVOT_TRIE_IPV6(u, vrf);
        } else {
            pivot_trie = VRF_PIVOT_TRIE_IPV4(u, vrf);
        }

        rv = trie_find_lpm(pivot_trie, prefix, length, &lpmp);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_sw_prefix_check: Pivot find failed\n")));
            return rv;
        }
        pivot_node = (alpm_pivot_t *)lpmp;
        bkt_trie = pivot_node->bucket->bucket_trie;
        if (bkt_trie != NULL) {
            rv = trie_search(bkt_trie, prefix, length,
                             (trie_node_t **)&bkt_trie);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_sw_prefix_check: prefix find failed\n")));
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_sw_prefix_check: bkt trie doesn't exist\n")));
        }
    }
    return rv;
}

/*
 * Find and read the match in the database.
 * Returns SOC_E_NONE, if found.
 * SOC_E_NOT_FOUND if not found
 * SOC_E_FAIL, if lookup failed.
 */
static int
_soc_alpm_find(int u,
               void *key_data,     /* TCAM entry data */
               soc_mem_t mem,      /* Mem type for ALPM */
               void *alpm_data,    /* Alpm data if match */
               int *tcam_index,    /* TCAM INDEX */
               int *bucket_index,  /* return Bucket Index*/
               int *index_ptr,     /* return key location */
               int sw_find         /* 1: search from trie, 0: search from aux */
              )
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         vrf_id, vrf, v6;
    int         key_index ;
    uint32      db_type, ent_type, bank_disable;
    int         rv = SOC_E_NONE;
    int         hit = 0;
    uint32      hit_y[SOC_MAX_MEM_FIELD_WORDS];
    uint32      hit_bit;
    soc_field_t hitf[] = {HIT_0f, HIT_1f, HIT_2f, HIT_3f};

    v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0f);
    if (v6) {
        if (!(v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE1f))) {
            return (SOC_E_PARAM);
        }
    }

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));
    if (vrf_id == 0) {
        if (soc_alpm_mode_get(u)) {
            /* cannot have 0 as a VRF in parallel mode */
            return SOC_E_PARAM;
        }
    }

    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);
    bank_disable = soc_alpm_bank_dis(u, vrf);

    /* For VRF_OVERRIDE (Global High) entries, software needs to maintain the
     * index */
    /* Prefix resides in TCAM directly */
    if (vrf_id != SOC_L3_VRF_OVERRIDE) {
        if (sw_find) {
            uint32 prefix[5], length;
            int default_route = 0;
            
            /* Create Prefix */
            rv = _alpm_prefix_create(u, key_data, prefix, &length, &default_route);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_insert: prefix create failed\n")));

                return rv;
            }
            
            rv = _soc_alpm_sw_pivot_find(u, prefix, length, v6, vrf, &hit,
                                         tcam_index, bucket_index);
            SOC_IF_ERROR_RETURN(rv);
        } else {
            defip_aux_scratch_entry_t aux_entry;
            /* Fill in AUX Scratch and perform Lookup Operation */
            SOC_IF_ERROR_RETURN(
                _soc_alpm_fill_aux_entry_for_op(u, key_data, v6, db_type,
                                                ent_type, 0, &aux_entry));
            SOC_IF_ERROR_RETURN(
                _soc_alpm_aux_op(u, PREFIX_LOOKUP, &aux_entry, TRUE, &hit, 
                                 tcam_index, bucket_index, 0));
        }

        if (hit) { /* Entry is found in the bucket_index */
            /* Fill entry for lookup */
            _soc_alpm_mem_ent_init(u, key_data, e, 0, mem, 0, 0);

            /* Perform lookup */
            rv = _soc_alpm_find_in_bkt(u, mem, *bucket_index, bank_disable,
                                       e, alpm_data, &key_index, v6);
            if (SOC_SUCCESS(rv)) {
                *index_ptr = key_index;

                if (!soc_feature(u, soc_feature_alpm_hit_bits_not_support) &&
                        soc_feature(u, soc_feature_two_ingress_pipes)) {
                    /* Get hit bit from Y Pipe. */
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(u, L3_DEFIP_ALPM_HIT_ONLY_Ym,
                                     MEM_BLOCK_ANY, ((key_index >> 2) & 0x1ffff),
                                     &hit_y));
                    hit_bit = soc_mem_field32_get(u,
                                  L3_DEFIP_ALPM_HIT_ONLY_Ym, &hit_y,
                                  hitf[key_index & 0x3]);
                    if (hit_bit) {
                        /* X | Y. */
                        soc_mem_field32_set(u, mem, alpm_data, HITf, hit_bit);
                    }
                }
            } 
        } else {
            rv = SOC_E_NOT_FOUND;
        }
    }
    return rv;
}


/*
 * Find and update the match in the database.
 * Returns SOC_E_NONE, if found and updated.
 * SOC_E_NOT_FOUND if not found
 * SOC_E_FAIL, if update failed.
 */
static int
_soc_alpm_find_and_update(int u,
                          void *key_data,  /* TCAM entry data */
                          void *alpm_data, /* Alpm data to be updated with */
                          void *alpm_sip_data,
                          soc_mem_t mem,   /* Mem view for ALPM data */
                          int key_index)  /* key location */
{
    defip_aux_scratch_entry_t aux_entry;
    int         vrf_id, v6, vrf;
    int         bucket_index;
    uint32      db_type, ent_type;
    int         rv = SOC_E_NONE;
    int         hit = 0, tmp = 0;
    int         tcam_index;
    int         aux_flags = 0;

    v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0f);
    if (v6) {
        if (!(v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE1f))) {
            return (SOC_E_PARAM);
        }
    }

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));
    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);

    /* For VRF_OVERRIDE (Global High) entries */
    if (vrf_id != SOC_L3_VRF_OVERRIDE) {
        /* Fill in AUX Scratch and perform Lookup Operation */
        SOC_IF_ERROR_RETURN(
           _soc_alpm_fill_aux_entry_for_op(u, key_data, v6, db_type, ent_type,
                                           0, &aux_entry));
        /* Perform lookup */
        /* Update the entry */
        SOC_IF_ERROR_RETURN(
            soc_mem_write(u, mem, MEM_BLOCK_ANY, key_index, 
                          alpm_data));
        if (SOC_URPF_STATUS_GET(u)) {
            /* Update the sip entry */
            SOC_IF_ERROR_RETURN(soc_mem_write(u, mem, MEM_BLOCK_ANY, 
                                _soc_alpm_rpf_entry(u, key_index), 
                                alpm_sip_data));
        }
        /* set REPLACE_LEN field to correct value */
        tmp = soc_mem_field32_get(u, L3_DEFIP_AUX_SCRATCHm, 
                                 &aux_entry, IP_LENGTHf);
        soc_mem_field32_set(u, L3_DEFIP_AUX_SCRATCHm, &aux_entry, 
                            REPLACE_LENf, tmp);

        if (SOC_URPF_STATUS_GET(u)) {
            if (soc_mem_field32_get(u, mem, alpm_sip_data, DEFAULTROUTEf)) {
                aux_flags |= SOC_ALPM_AUX_DEF_ROUTE;
            }
            if (soc_mem_field32_get(u, mem, alpm_sip_data, SRC_DISCARDf)) {
                aux_flags |= SOC_ALPM_AUX_SRC_DISCARD;
            }
        }

        SOC_IF_ERROR_RETURN(
            _soc_alpm_aux_op(u, DELETE_PROPAGATE, &aux_entry, TRUE,
                             &hit, &tcam_index, &bucket_index, aux_flags));
    }
    return rv;
}


/* Callback function for Traverse, to find Leaf nodes */
int 
alpm_mem_prefix_array_cb(trie_node_t *node, void *info)
{
    alpm_mem_prefix_array_t *prefix_array = (alpm_mem_prefix_array_t *)info;
    if (node->type == PAYLOAD) {
        prefix_array->prefix[prefix_array->count] = (payload_t *)node;
        prefix_array->count++;
    }
    return SOC_E_NONE;
}

int 
alpm_delete_node_cb(trie_node_t *node, void *info)
{
    if (node != NULL) {
        sal_free(node);
    }
    return SOC_E_NONE;
}


/* src and dst can be same */
static int
_soc_alpm_aux_entry_convert(int u, void *src, void *dst, int convert)
{
    soc_field_t f0[] = {
                VRF0f,
                MODE0f,
                VALID0f,
                IP_LENGTH0f,
                BPM_LENGTH0f,
                IP_ADDR0f,
                DB_TYPE0f,
                ENTRY_TYPE0f};
    soc_field_t f1[] = {
                VRF1f,
                MODE1f,
                VALID1f,
                IP_LENGTH1f,
                BPM_LENGTH1f,
                IP_ADDR1f,
                DB_TYPE1f,
                ENTRY_TYPE1f};
    soc_field_t *sf, *df;
    int i = 0;
    uint32 v;

    switch (convert) {
    case 0:
        sf = f0;
        df = f0;
        break;
    case 1:
        sf = f0;
        df = f1;
        break;
    case 2:
        sf = f1;
        df = f0;
        break;
    case 3:
        sf = f1;
        df = f1;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    for (i = 0; i < COUNTOF(f0); i++) {
        v = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm, src, sf[i]);
        soc_mem_field32_set(u, L3_DEFIP_AUX_TABLEm, dst, df[i], v);
    }

    return SOC_E_NONE;
}


/* create shadow entry copy of primary tcam entry at index */
static int
_soc_alpm_tcam_shadow_ent_init(int u, int s_index, int index, void *entry,
                               int convert, void *shadow, int bpm_len, int v4_half)
{
    uint32 tmp, ip_mask, v6, db_type, ent_type, urpf = 0;
    soc_mem_t mem = L3_DEFIPm;
    soc_mem_t s_mem = L3_DEFIP_AUX_TABLEm;
    defip_entry_t tmp_ipv4;
    int rv = SOC_E_NONE, pfx, pfx2, vrf_id, vrf_id2, vrf, vrf2;
    defip_aux_table_entry_t src_shadow;
    void *shadow_ptr = shadow;
    int       flex_v6_64 = 0;

    /* convert from logical to physical indices */
    s_index = soc_alpm_physical_idx(u, L3_DEFIPm, s_index, 1);

    if (convert == CONVERT_NONE) {
        SOC_IF_ERROR_RETURN(soc_mem_read(u, s_mem, MEM_BLOCK_ANY, s_index, 
                            shadow));
    } else {
        tmp = soc_alpm_physical_idx(u, L3_DEFIPm, index, 1);
        SOC_IF_ERROR_RETURN(soc_mem_read(u, s_mem, MEM_BLOCK_ANY, tmp,
                            shadow));
        if (s_index != tmp) {
            SOC_IF_ERROR_RETURN(soc_mem_read(u, s_mem, MEM_BLOCK_ANY, s_index, 
                                &src_shadow));
            shadow_ptr = (void*) &src_shadow;
        }

        SOC_IF_ERROR_RETURN(_soc_alpm_aux_entry_convert(u, shadow_ptr, shadow,
                                                        convert));
    }

    tmp = soc_mem_field32_get(u, mem, entry, VRF_ID_0f);
    soc_mem_field32_set(u, s_mem, shadow, VRF0f, tmp);
    tmp = soc_mem_field32_get(u, mem, entry, VRF_ID_1f);
    soc_mem_field32_set(u, s_mem, shadow, VRF1f, tmp);

    tmp = soc_mem_field32_get(u, mem, entry, MODE0f);
    soc_mem_field32_set(u, s_mem, shadow, MODE0f, tmp);
    tmp = soc_mem_field32_get(u, mem, entry, MODE1f);
    soc_mem_field32_set(u, s_mem, shadow, MODE1f, tmp);
    v6 = tmp;

    if (bpm_len == INVALID_BPM_LEN) {
        /* Clean dirty data */
        if (soc_mem_field32_get(u, s_mem, shadow, VALID0f) == 0) {
            soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH0f, 0);
        }

        if (soc_mem_field32_get(u, s_mem, shadow, VALID1f) == 0) {
            soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH1f, 0);
        }
    } else {
        assert(v6 == (v4_half==0));
        if (v6) {
            soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH0f, bpm_len);
            soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH1f, bpm_len);
        } else {
            if (v4_half & 1) {
                soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH1f, bpm_len);
            } else {
                soc_mem_field32_set(u, s_mem, shadow, BPM_LENGTH0f, bpm_len);
            }
        }
    }
    /* for now simply copy all fields. Let caller decide which half to use */
    tmp = soc_mem_field32_get(u, mem, entry, VALID0f);
    soc_mem_field32_set(u, s_mem, shadow, VALID0f, tmp);
    tmp = soc_mem_field32_get(u, mem, entry, VALID1f);
    soc_mem_field32_set(u, s_mem, shadow, VALID1f, tmp);

    
    /* IP_LENGTH */
    tmp = soc_mem_field32_get(u, mem, entry, IP_ADDR_MASK0f);
    if ((rv = _ipmask2pfx(tmp, &pfx)) < 0) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_ipmask2pfx fails mask0 0x%x pfx %d\n"),
                              tmp, pfx));
        return rv;
    }
    ip_mask = soc_mem_field32_get(u, mem, entry, IP_ADDR_MASK1f);
    if ((rv = _ipmask2pfx(ip_mask, &pfx2)) < 0) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_ipmask2pfx fails mask1 0x%x pfx2 %d\n"),
                              ip_mask, pfx2));
        return rv;
    }
    if (v6) {
        soc_mem_field32_set(u, s_mem, shadow, IP_LENGTH0f, pfx + pfx2);
        soc_mem_field32_set(u, s_mem, shadow, IP_LENGTH1f, pfx + pfx2);
    } else {
        soc_mem_field32_set(u, s_mem, shadow, IP_LENGTH0f, pfx);
        soc_mem_field32_set(u, s_mem, shadow, IP_LENGTH1f, pfx2);
    }
        
    /* IP_ADDR */
    tmp = soc_mem_field32_get(u, mem, entry, IP_ADDR0f);
    soc_mem_field32_set(u, s_mem, shadow, IP_ADDR0f, tmp);
    tmp = soc_mem_field32_get(u, mem, entry, IP_ADDR1f);
    soc_mem_field32_set(u, s_mem, shadow, IP_ADDR1f, tmp);

    /* DB_TYPE & ENTRY_TYPE */
    /* for v4 need to be careful */
    if (!v6) {
        sal_memcpy(&tmp_ipv4, entry, sizeof(tmp_ipv4));
        /* vrf_id for ipv4 entry 0 */
        rv = soc_alpm_lpm_vrf_get(u, (void *)&tmp_ipv4, &vrf_id, &vrf);
        SOC_IF_ERROR_RETURN(rv);

        /* vrf_id for ipv4 entry 1 */
        SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(u, &tmp_ipv4, 
                            &tmp_ipv4, PRESERVE_HIT));
        rv = soc_alpm_lpm_vrf_get(u, (void *)&tmp_ipv4, &vrf_id2, &vrf2);
        SOC_IF_ERROR_RETURN(rv);
    } else {
        rv = soc_alpm_lpm_vrf_get(u, entry, &vrf_id, &vrf);
    }
    if (SOC_URPF_STATUS_GET(u)) {
        if (index >= (soc_mem_index_count(u, L3_DEFIPm) >> 1)) {
            urpf = 1;
        }
    }

    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);

    if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && v6 &&
        soc_mem_field32_get(u, L3_DEFIPm, entry, ENTRY_VIEW0f)) {
        if (pfx2 == 32 && pfx == 32) {
            flex_v6_64 = 1;
        }
    }

    if (vrf_id == SOC_L3_VRF_OVERRIDE ||
        (soc_feature(u, soc_feature_ipmc_defip) &&
         soc_mem_field32_get(u, mem, entry, MULTICAST_ROUTE0f)) ||
         flex_v6_64) {
        soc_mem_field32_set(u, s_mem, shadow, VALID0f, 0);
        db_type = 0;
    } else {
        if (urpf) {
            db_type += 1;
        }
    }

    soc_mem_field32_set(u, s_mem, shadow, DB_TYPE0f, db_type);
    tmp = soc_mem_field32_get(u, mem, entry, ENTRY_TYPE0f);
    soc_mem_field32_set(u, s_mem, shadow, ENTRY_TYPE0f, tmp | ent_type);
    if (!v6) {
        soc_alpm_db_ent_type_encoding(u, vrf2, &db_type, &ent_type);

        if (vrf_id2 == SOC_L3_VRF_OVERRIDE ||
            (soc_feature(u, soc_feature_ipmc_defip) &&
             soc_mem_field32_get(u, mem, entry, MULTICAST_ROUTE1f))) {
            soc_mem_field32_set(u, s_mem, shadow, VALID1f, 0);
            db_type = 0; /* don't care */
        } else {
            /* Database to be searched is VRF - DIP */
            if (urpf) {
                db_type += 1;
            }
        }
        soc_mem_field32_set(u, s_mem, shadow, DB_TYPE1f, db_type);
        tmp = soc_mem_field32_get(u, mem, entry, ENTRY_TYPE1f);
        soc_mem_field32_set(u, s_mem, shadow, ENTRY_TYPE1f, tmp | ent_type);
    } else {
        if (vrf_id == SOC_L3_VRF_OVERRIDE ||
            (soc_feature(u, soc_feature_ipmc_defip) &&
             soc_mem_field32_get(u, mem, entry, MULTICAST_ROUTE1f)) ||
             flex_v6_64) {
            soc_mem_field32_set(u, s_mem, shadow, VALID1f, 0);
        }
        soc_mem_field32_set(u, s_mem, shadow, DB_TYPE1f, db_type);
        tmp = soc_mem_field32_get(u, mem, entry, ENTRY_TYPE1f);
        soc_mem_field32_set(u, s_mem, shadow, ENTRY_TYPE1f, tmp | ent_type);
    }

    /* modify bucket index if urpf is enabled */
    if (urpf) {
        tmp = soc_mem_field32_get(u, mem, entry, ALG_BKT_PTR0f);
        if (tmp && tmp < SOC_ALPM_BUCKET_COUNT(u)) {
            tmp += SOC_ALPM_BUCKET_COUNT(u);
            soc_mem_field32_set(u, mem, entry, ALG_BKT_PTR0f, tmp);
        }
        if (!v6) {
            tmp = soc_mem_field32_get(u, mem, entry, ALG_BKT_PTR1f);
            if (tmp && tmp < SOC_ALPM_BUCKET_COUNT(u)) {
                tmp += SOC_ALPM_BUCKET_COUNT(u);
                soc_mem_field32_set(u, mem, entry, ALG_BKT_PTR1f, tmp);
            }
        }
    }

    /* BPM_LENGTH */
    /* don't modify, preserve existing values */
    return SOC_E_NONE;
}

static int
_soc_alpm_write_pivot_aux(int u, int s_index, int index, void *entry,
                          int convert, int bpm_len, int v4_half)
{
    defip_aux_table_entry_t shadow;

    SOC_IF_ERROR_RETURN(_soc_alpm_tcam_shadow_ent_init(u, 
            s_index, index, entry, convert, (void *)&shadow, bpm_len, v4_half));

    /* write to defip has to be logical */
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, 
                         index, entry));

    /* write to shadow table has to be physical */
    index = soc_alpm_physical_idx(u, L3_DEFIPm, index, 1);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(u, MEM_BLOCK_ANY,
                         index, &shadow));
    return SOC_E_NONE;
}

int
_soc_alpm_insert_in_bkt(int u, soc_mem_t mem, int bucket_index,
                        int bank_disable, void *alpm_data, void *alpm_sip_data,
                        uint32 *e, int *key_index, int v6)
{
    int rv;

    rv = soc_mem_alpm_insert(u, mem, bucket_index,
                             MEM_BLOCK_ANY,
                             bank_disable,
                             alpm_data, e, key_index);
    if (rv == SOC_E_FULL) {
        /* in combined search mode non-urpf try inserting into next bkt for v6*/
        if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
            rv = soc_mem_alpm_insert(u, mem, bucket_index+1,
                             MEM_BLOCK_ANY,
                             bank_disable,
                             alpm_data, e, key_index);
        }
    }

    if (SOC_SUCCESS(rv)) {
        if (SOC_URPF_STATUS_GET(u)) {
            rv = soc_mem_write(u, mem, MEM_BLOCK_ANY,
                               _soc_alpm_rpf_entry(u, *key_index),
                               alpm_sip_data);
        }
    }
    return rv;
}

#define _TD2_ALPM_BANK_NUM      (4)
#define _TD2_ALPM_BANK_MASK     ((1 << _TD2_ALPM_BANK_NUM) - 1)

/* ALPM Table Address Translation
 * Bank     addr[1:0]
 * Bucket   addr[15:2]
 * Entry    addr[16]    L3_DEFIP_ALPM_IPV6_128m
 *          addr[16:17] L3_DEFIP_ALPM_IPV6_64m
 *          addr[16:18] L3_DEFIP_ALPM_IPV4m
 */
int 
_soc_alpm_mem_index(int u, soc_mem_t mem, int bucket_index, int offset, 
                    uint32 bank_disable, int *mem_index)
{
    int i, bidx = 0;
    int baddr_arr[4] = {0};
    int bnk_index = 0;
    int ent_index = 0;
    int avail_bank_num;
    
    int v6 = 0;
    int ent_cnt = 6;

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
        ent_cnt = 6;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        ent_cnt = 4;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        ent_cnt = 4;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        ent_cnt = 3;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        ent_cnt = 2;
        v6 = 1;
        break;
    default:
        break;
    }

    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        if (offset >= ALPM_RAW_BKT_COUNT * ent_cnt) {
            bucket_index ++;
            offset -= ALPM_RAW_BKT_COUNT * ent_cnt;
        }
    }

    avail_bank_num = _TD2_ALPM_BANK_NUM - 
                     _shr_popcount(bank_disable & _TD2_ALPM_BANK_MASK); 

    if (bucket_index >= (1 << 16) || offset >= avail_bank_num * ent_cnt) {
        return SOC_E_FULL;
    }
    ent_index = offset % ent_cnt;

    /* Prepare bank address array for offset */
    for (i = 0; i < _TD2_ALPM_BANK_NUM; i++) {
        if ((1 << i) & bank_disable) {
            continue;
        }

        baddr_arr[bidx++] = i;
    }
    bnk_index = baddr_arr[offset / ent_cnt];
    
    *mem_index = (ent_index << 16)      | 
                 (bucket_index << 2)    |
                 (bnk_index);
    return SOC_E_NONE;
}

#ifdef ALPM_USE_RAW_VIEW

void
_soc_alpm_raw_mem_read(int unit, soc_mem_t mem, void *raw_entry,
                       int ent_id, void *entry)
{

    soc_mem_info_t meminfo;
    soc_field_info_t fieldinfo;
    /* Exclude HIT bit */
    int entry_bits = soc_mem_entry_bits(unit, mem) - 1;

    /* Construct presdo memory & field info, to take advantage of 
     * socmem.c function get original view */
    meminfo.flags = 0;
    meminfo.bytes = sizeof(defip_alpm_raw_entry_t);
    fieldinfo.flags = SOCF_LE;
    fieldinfo.bp =  entry_bits * ent_id;
    fieldinfo.len = entry_bits;

    (void)soc_meminfo_fieldinfo_field_get(raw_entry, &meminfo,
                                          &fieldinfo, entry);

}

void
_soc_alpm_raw_mem_write(int unit, soc_mem_t mem, void *raw_entry,
                        int index, int ent_id, void *entry)
{
    soc_mem_info_t meminfo;
    soc_field_info_t fieldinfo;
    /* Exclude HIT bit */
    int entry_bits = soc_mem_entry_bits(unit, mem) - 1;

    (void) soc_mem_cache_invalidate(unit, mem, MEM_BLOCK_ANY, index);

    meminfo.flags = 0;
    meminfo.bytes = sizeof(defip_alpm_raw_entry_t);
    fieldinfo.bp =  entry_bits * ent_id;
    fieldinfo.len = entry_bits;
    fieldinfo.flags = SOCF_LE;

    (void)soc_meminfo_fieldinfo_field_set(raw_entry, &meminfo,
                                          &fieldinfo, entry);

}

int
_soc_alpm_raw_bucket_read(int u, soc_mem_t mem, int bucket_index, 
                          void *raw_entry, void *raw_sip_entry)
{
    int i, v6 = 1;
    int raw_index, raw_sip_index;
    defip_alpm_raw_entry_t *ent = raw_entry;
    defip_alpm_raw_entry_t *entsip = raw_sip_entry;

    if (mem == L3_DEFIP_ALPM_IPV4m ||
        mem == L3_DEFIP_ALPM_IPV4_1m) {
        v6 = 0;
    }
    
    /* Read all bucket entries by using ALPM_RAW view */
    raw_index = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);
    
    for (i = 0; i < SOC_ALPM_RAW_BUCKET_SIZE(u, v6); i++) {
        SOC_IF_ERROR_RETURN(
            _soc_mem_alpm_read(u, L3_DEFIP_ALPM_RAWm, MEM_BLOCK_ANY,
                               raw_index + i, &ent[i]));
        if (SOC_URPF_STATUS_GET(u)) {
            raw_sip_index = _soc_alpm_rpf_entry(u, raw_index + i);                
            SOC_IF_ERROR_RETURN(
                _soc_mem_alpm_read(u, L3_DEFIP_ALPM_RAWm, MEM_BLOCK_ANY,
                                   raw_sip_index, &entsip[i]));
        }
    }

    return SOC_E_NONE;
}

int
_soc_alpm_raw_bucket_write(int u, soc_mem_t mem, int bucket_index, 
                           uint32 bank_disable,
                           void *raw_entry, void *raw_sip_entry, int entry_cnt)
{
    int i = 0, bank, v6 = 1;
    int raw_index, raw_sip_index, raw_ent_cnt;
    defip_alpm_raw_entry_t *ent = raw_entry;
    defip_alpm_raw_entry_t *entsip = raw_sip_entry;
    int ent_per_bank = 6;

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
        ent_per_bank = 6;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        ent_per_bank = 4;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        ent_per_bank = 4;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        ent_per_bank = 3;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        ent_per_bank = 2;
        v6 = 1;
        break;
    default:
        break;
    }

    /* Write all bucket entries by using ALPM_RAW view */
    raw_index = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);
    if (entry_cnt == MAX_RAW_COUNT) {
        raw_ent_cnt = SOC_ALPM_RAW_BUCKET_SIZE(u, v6);
    } else {
        raw_ent_cnt = (entry_cnt / ent_per_bank) + 1;
    }

    for (bank = 0; bank < SOC_ALPM_RAW_BUCKET_SIZE(u, v6); bank++) {
        if ((1 << (bank % _TD2_ALPM_BANK_NUM)) & bank_disable) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(u, L3_DEFIP_ALPM_RAWm, MEM_BLOCK_ANY, 
                                    raw_index + bank, &ent[bank]));
        _soc_trident2_alpm_bkt_view_set(u, raw_index + bank, mem);
        if (SOC_URPF_STATUS_GET(u)) {
            raw_sip_index = _soc_alpm_rpf_entry(u, raw_index + bank);
            _soc_trident2_alpm_bkt_view_set(u, raw_sip_index, mem);
            SOC_IF_ERROR_RETURN(soc_mem_write(u, L3_DEFIP_ALPM_RAWm, MEM_BLOCK_ANY, 
                                    raw_sip_index, &entsip[bank]));
        }
        if (++i == raw_ent_cnt) {
            break;
        }
    }

    return SOC_E_NONE;
}

void _soc_alpm_raw_parity_set(int u, soc_mem_t mem, void *alpm_data)
{
    int i, entry_words, bit1count = 0;
    uint32 *entry = alpm_data;

    entry_words = soc_mem_entry_words(u, mem);

    /* Count bit1 number */
    for (i = 0; i < entry_words; i++) {
        bit1count += _shr_popcount(entry[i]);
    }

    /* If the number is odd, we should set PARITY bit */
    if (bit1count & 0x1) {
        if (SOC_MEM_FIELD_VALID(u, mem, EVEN_PARITYf)) {
            soc_mem_field32_set(u, mem, alpm_data, EVEN_PARITYf, 1);
        }
    }
}

static void
_soc_alpm_raw_bucket_prepare_undo(int u,
                                  alpm_mem_prefix_array_t **alloc_pfx_array,
                                  void **alloc_raw_bkt,
                                  int  **alloc_new_index)
{
    if (*alloc_raw_bkt) {
        sal_free(*alloc_raw_bkt);
    }
    if (*alloc_pfx_array) {
        sal_free(*alloc_pfx_array);
        *alloc_pfx_array = NULL;
    }
    if (*alloc_new_index) {
        sal_free(*alloc_new_index);
        *alloc_new_index = NULL;
    }
}

static int
_soc_alpm_raw_bucket_prepare(int u, soc_mem_t mem,
                             trie_t     *bkt_trie,
                             int         bucket_index_from,
                             int         bucket_index_to,
                             uint32     bank_disable,
                             void       *alpm_data,
                             void       *alpm_sip_data,
                             alpm_mem_prefix_array_t **alloc_pfx_array,
                             void       **alloc_raw_bkt,
                             int        **alloc_new_index,
                             int        *new_index_pos,
                             int        to_exist)
{
    int     i, j = 0, rv = SOC_E_NONE;
    int     raw_size, raw_bkt_size, alloc_size;
    void    *raw_ent;
    void    *raw_bkt[RAW_BKT_NUM] = {NULL};
    int     raw_id = 0;
    int     ent_id = 0;
    int     key_index;
    /* allocation pointers all set to NULL */
    alpm_mem_prefix_array_t *pfx_array = NULL;
    int     *index_array = NULL;
    int     v6 = SOC_ALPM_MEM_V6(mem);
    /* Search Result buffers */
    defip_alpm_ipv4_1_entry_t alpmv4_entry, alpmv4_sip_entry;
    defip_alpm_ipv6_64_1_entry_t alpmv6_entry, alpmv6_sip_entry;
    void  *bufp = NULL, *sip_bufp = NULL;
    defip_alpm_ipv4_1_entry_t alpmv4_entry2;
    defip_alpm_ipv6_64_1_entry_t alpmv6_entry2;
    void  *bufp2 = NULL;
    int   max_count = 0;

    /* Assign entry buf based on table being used */
    SOC_ALPM_ENTRY_BUF(v6, mem, bufp, alpmv4_entry, alpmv6_entry);
    SOC_ALPM_ENTRY_BUF(v6, mem, sip_bufp, alpmv4_sip_entry, alpmv6_sip_entry);
    SOC_ALPM_ENTRY_BUF(v6, mem, bufp2, alpmv4_entry2, alpmv6_entry2);

    *alloc_raw_bkt   = NULL;
    *alloc_new_index = NULL;
    *alloc_pfx_array = NULL;

    /* Alloc prefix array */
    alloc_size = sizeof(alpm_mem_prefix_array_t);
    pfx_array = sal_alloc(alloc_size, "prefix array");
    if (pfx_array == NULL) {
        goto cleanup;
    }

    /* Fill prefix array */
    sal_memset(pfx_array, 0, alloc_size);
    rv = trie_traverse(bkt_trie, alpm_mem_prefix_array_cb,
                       pfx_array, _TRIE_INORDER_TRAVERSE);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Alloc index array */
    alloc_size = sizeof(int) * pfx_array->count;
    index_array = sal_alloc(alloc_size, "index_array");
    if (index_array == NULL) {
        goto cleanup;
    }
    sal_memset(index_array, -1, alloc_size);

    raw_size = sizeof(defip_alpm_raw_entry_t);
    raw_bkt_size = raw_size * ALPM_RAW_BKT_COUNT_DW;
    raw_bkt[0] = sal_alloc(RAW_BKT_NUM * raw_bkt_size, "Raw memory buffer");
    if (raw_bkt[0] == NULL) {
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    sal_memset(raw_bkt[0], 0, RAW_BKT_NUM * raw_bkt_size);
    for (i = 1; i < RAW_BKT_NUM; i++) {
        raw_bkt[i] = (uint8 *)raw_bkt[0] + raw_bkt_size * i;
    }

    rv = _soc_alpm_raw_bucket_read(u, mem, bucket_index_from,
                                   raw_bkt[RAW_OLD_BKT_DIP],
                                   raw_bkt[RAW_OLD_BKT_SIP]);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    sal_memcpy(raw_bkt[RAW_RB_BKT_DIP], raw_bkt[RAW_OLD_BKT_DIP], raw_bkt_size);
    sal_memcpy(raw_bkt[RAW_RB_BKT_SIP], raw_bkt[RAW_OLD_BKT_SIP], raw_bkt_size);

    if (to_exist) {
        rv = _soc_alpm_raw_bucket_read(u, mem, bucket_index_to,
                                       raw_bkt[RAW_NEW_BKT_DIP],
                                       raw_bkt[RAW_NEW_BKT_SIP]);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        max_count = _soc_alpm_bkt_entry_cnt(u, mem);
    }

    for (i = 0; i < pfx_array->count; i++) {
        payload_t *pfx = pfx_array->prefix[i];
        if (pfx->index > 0) {
            /* if this is the new prefix it does not exist in the
             * bucket, no point reading */
            SOC_ALPM_RAW_INDEX_DECODE(u, v6, pfx->index, raw_id, ent_id);
            raw_ent = (uint8 *)raw_bkt[RAW_OLD_BKT_DIP] + raw_id * raw_size;
            _soc_alpm_raw_mem_read(u, mem, raw_ent, ent_id, bufp);
            _soc_alpm_raw_mem_write(u, mem, raw_ent, pfx->index,
                                    ent_id, soc_mem_entry_null(u, mem));

            if (SOC_URPF_STATUS_GET(u)) {
                raw_ent = (uint8 *)raw_bkt[RAW_OLD_BKT_SIP] + raw_id * raw_size;
                _soc_alpm_raw_mem_read(u, mem, raw_ent, ent_id, sip_bufp);
                _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                        _soc_alpm_rpf_entry(u, pfx->index),
                                        ent_id, soc_mem_entry_null(u, mem));
            }

            if (to_exist) {
                for (;j < max_count; j++) {
                    rv = _soc_alpm_mem_index(u, mem, bucket_index_to, j,
                                             bank_disable, &key_index);
                    if (SOC_FAILURE(rv)) {
                        goto cleanup;
                    }
                    SOC_ALPM_RAW_INDEX_DECODE(u, v6, key_index, raw_id, ent_id);
                    raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_DIP] + raw_id * raw_size;
                    _soc_alpm_raw_mem_read(u, mem, raw_ent, ent_id, bufp2);
                    if (soc_mem_field32_get(u, mem, bufp2, VALIDf) == 0) {
                        break;
                    }
                }
                if (j == max_count) {
                    rv = SOC_E_INTERNAL;
                    goto cleanup;
                }
                _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                        key_index, ent_id, bufp);
                if (SOC_URPF_STATUS_GET(u)) {
                    raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_SIP] + raw_id * raw_size;
                    _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                            _soc_alpm_rpf_entry(u, key_index),
                                            ent_id, sip_bufp);
                }
            } else {
                rv = _soc_alpm_mem_index(u, mem, bucket_index_to, i,
                                         bank_disable, &key_index);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
                SOC_ALPM_RAW_INDEX_DECODE(u, v6, key_index, raw_id, ent_id);
                raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_DIP] + raw_id * raw_size;
                _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                        key_index, ent_id, bufp);
                if (SOC_URPF_STATUS_GET(u)) {
                    raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_SIP] + raw_id * raw_size;
                    _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                            _soc_alpm_rpf_entry(u, key_index),
                                            ent_id, sip_bufp);
                }
            }
        } else {
            rv = _soc_alpm_mem_index(u, mem, bucket_index_to, i,
                                     bank_disable, &key_index);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            if (!new_index_pos) {
                rv = SOC_E_PARAM;
                goto cleanup;
            }
            *new_index_pos = i;

            /* New entry, need to take care of Parity for raw view */
            _soc_alpm_raw_parity_set(u, mem, alpm_data);
            SOC_ALPM_RAW_INDEX_DECODE(u, v6, key_index, raw_id, ent_id);
            raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_DIP] + raw_id * raw_size;
            _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                    key_index, ent_id, alpm_data);
            if (SOC_URPF_STATUS_GET(u)) {
                _soc_alpm_raw_parity_set(u, mem, alpm_sip_data);
                raw_ent = (uint8 *)raw_bkt[RAW_NEW_BKT_SIP] + raw_id * raw_size;
                _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                        _soc_alpm_rpf_entry(u, key_index),
                                        ent_id, alpm_sip_data);
            }
        }
        index_array[i] = key_index;
    }

    for (i = 0; i < RAW_BKT_NUM; i++) {
        alloc_raw_bkt[i] = raw_bkt[i];
    }
    *alloc_new_index = index_array;
    *alloc_pfx_array = pfx_array;

    return rv;

cleanup:
    if (raw_bkt[0]) {
        sal_free(raw_bkt[0]);
    }
    if (index_array) {
        sal_free(index_array);
    }
    if (pfx_array) {
        sal_free(pfx_array);
    }
    return rv;
}


static void
_soc_alpm_raw_bucket_copy_undo(int u, soc_mem_t mem,
                               int bucket_index,
                               uint32 bank_disable,
                               void *new_raw_bufp, void *new_raw_sip_bufp,
                               int *new_index, int index_count)
{
    int  i;
    int  raw_size = sizeof(defip_alpm_raw_entry_t);
    int  raw_id = 0;
    int  ent_id = 0;
    void *raw_ent;
    int  v6 = SOC_ALPM_MEM_V6(mem);

    /* Restore the new bucket to null */
    for (i = 0; i < index_count; i++) {
        if (new_index[i] < 0) {
            continue;
        }

        SOC_ALPM_RAW_INDEX_DECODE(u, v6, new_index[i], raw_id, ent_id);
        raw_ent = (uint8 *)new_raw_bufp + raw_id * raw_size;
        _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                new_index[i], ent_id, soc_mem_entry_null(u, mem));
        if (SOC_URPF_STATUS_GET(u)) {
            raw_ent = (uint8 *)new_raw_sip_bufp + raw_id * raw_size;
            _soc_alpm_raw_mem_write(u, mem, raw_ent,
                                    _soc_alpm_rpf_entry(u, new_index[i]),
                                    ent_id, soc_mem_entry_null(u, mem));
        }
    }

    (void)_soc_alpm_raw_bucket_write(u, mem, bucket_index, bank_disable,
                                     new_raw_bufp, new_raw_sip_bufp, MAX_RAW_COUNT);
}

/* Populate entries in pfx array to logical bucket */
static int
_soc_alpm_raw_bucket_copy(int u, soc_mem_t mem,
                          int       bucket_index,
                          uint32    bank_disable,
                          void      *new_raw_bufp,
                          void      *new_raw_sip_bufp,
                          int       *new_index,
                          int       index_count)
{
    int                             rv;
    rv = _soc_alpm_raw_bucket_write(u, mem, bucket_index, bank_disable,
                                    new_raw_bufp, new_raw_sip_bufp, MAX_RAW_COUNT);

    if (SOC_FAILURE(rv)) {
        /* Delete entries in new bucket */
        _soc_alpm_raw_bucket_copy_undo(u, mem, bucket_index, bank_disable,
                                       new_raw_bufp, new_raw_sip_bufp,
                                       new_index, index_count);
    }
    return rv;
}



static INLINE void
_soc_alpm_raw_bucket_inval_undo(int u, soc_mem_t mem, int bucket_index,
                                uint32 bank_disable,
                                void *old_raw_bufp, void *old_raw_sip_bufp)
{

    (void)_soc_alpm_raw_bucket_write(u, mem, bucket_index,
                                     bank_disable, (void *)old_raw_bufp,
                                     (void *)old_raw_sip_bufp, MAX_RAW_COUNT);
    return;
}
static int
_soc_alpm_raw_bucket_inval(int u, soc_mem_t mem, int bucket_index,
                           uint32 bank_disable,
                           void *old_bufp,
                           void *old_sip_bufp,
                           void *rb_bufp,
                           void *rb_sip_bufp)
{
    int rv = SOC_E_NONE;

    /* Invalidation process
     * 1. Erase old copied entries
     * 2. Re-validate those entries if invalidation fails
     */

    /* Update old bucket with split entries invalidated */
    rv =  _soc_alpm_raw_bucket_write(u, mem, bucket_index,
                                     bank_disable, old_bufp,
                                     old_sip_bufp, MAX_RAW_COUNT);
    /* re-validate old bucket entries */
    if (SOC_FAILURE(rv)) {
        _soc_alpm_raw_bucket_inval_undo(u, mem, bucket_index,
                                        bank_disable, rb_bufp,
                                        rb_sip_bufp);
    }

    return rv;
}

#else

static void
_soc_alpm_bucket_prepare_undo(int u,
                              alpm_mem_prefix_array_t **alloc_pfx_array,
                              void **alloc_bufp,
                              void **alloc_sip_bufp,
                              int  **alloc_new_index)
{
    if (*alloc_pfx_array) {
        sal_free(*alloc_pfx_array);
        *alloc_pfx_array = NULL;
    }
    if (*alloc_bufp) {
        sal_free(*alloc_bufp);
        *alloc_bufp = NULL;
    }
    if (*alloc_sip_bufp) {
        sal_free(*alloc_sip_bufp);
        *alloc_sip_bufp = NULL;
    }
    if (*alloc_new_index) {
        sal_free(*alloc_new_index);
        *alloc_new_index = NULL;
    }
}

static int
_soc_alpm_bucket_prepare(int u, soc_mem_t mem,
                         trie_t *bkt_trie,
                         alpm_mem_prefix_array_t **alloc_pfx_array,
                         void **alloc_bufp,
                         void **alloc_sip_bufp,
                         int  **alloc_new_index)
{
    int i, rv = SOC_E_NONE;
    int entry_size;
    int alloc_size;
    /* allocation pointers all set to NULL */
    void *bufp = NULL, *sip_bufp = NULL;
    int  *index_array = NULL;
    alpm_mem_prefix_array_t *pfx_array = NULL;

    *alloc_bufp      = NULL;
    *alloc_sip_bufp  = NULL;
    *alloc_new_index = NULL;
    *alloc_pfx_array = NULL;

    /* Alloc prefix array */
    alloc_size = sizeof(alpm_mem_prefix_array_t);
    pfx_array = sal_alloc(alloc_size, "prefix array");
    if (pfx_array == NULL) {
        goto cleanup;
    }

    /* Fill prefix array */
    sal_memset(pfx_array, 0, alloc_size);
    rv = trie_traverse(bkt_trie, alpm_mem_prefix_array_cb,
                       pfx_array, _TRIE_INORDER_TRAVERSE);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Alloc index array */
    alloc_size = sizeof(int) * pfx_array->count;
    index_array = sal_alloc(alloc_size, "index_array");
    if (index_array == NULL) {
        goto cleanup;
    }
    sal_memset(index_array, -1, alloc_size);

    /* Alloc bufp & sip_bufp */
    entry_size = (L3_DEFIP_ALPM_IPV6_64m == mem ||
                  L3_DEFIP_ALPM_IPV6_64_1m == mem) ?
                  sizeof(defip_alpm_ipv6_64_1_entry_t) :
                  sizeof(defip_alpm_ipv4_1_entry_t);

    alloc_size = entry_size * pfx_array->count;
    bufp = sal_alloc(alloc_size, "alloc_bufp");
    if (bufp == NULL) {
        goto cleanup;
    }
    if (SOC_URPF_STATUS_GET(u)) {
        sip_bufp  = sal_alloc(alloc_size, "alloc_sip_bufp");
        if (sip_bufp == NULL) {
            goto cleanup;
        }
    }

    /* Fill bufp & sip bufp */
    for (i = 0; i < pfx_array->count; i++) {
        payload_t *pfx = pfx_array->prefix[i];

        if (pfx->index > 0) { /* old pfx */
            rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY, pfx->index,
                                    (uint8 *)bufp + i * entry_size);

            if (SOC_FAILURE(rv)) {
                break;
            }

            if (SOC_URPF_STATUS_GET(u)) {
                rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY,
                        _soc_alpm_rpf_entry(u, pfx->index),
                        (uint8 *)sip_bufp + i * entry_size);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
    }
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    *alloc_bufp      = bufp;
    *alloc_sip_bufp  = sip_bufp;
    *alloc_new_index = index_array;
    *alloc_pfx_array = pfx_array;

    return rv;

cleanup:
    if (bufp) {
        sal_free(bufp);
    }
    if (sip_bufp) {
        sal_free(sip_bufp);
    }
    if (index_array) {
        sal_free(index_array);
    }
    if (pfx_array) {
        sal_free(pfx_array);
    }
    return rv;
}


static void
_soc_alpm_bucket_copy_undo(int u, soc_mem_t mem, int count, int *new_index)
{
    int i;

    for (i = 0; i < count; i++) {
        if (new_index[i] < 0) {
            continue;
        }
        soc_mem_write(u, mem, MEM_BLOCK_ANY, new_index[i],
                      soc_mem_entry_null(u, mem));
        if (SOC_URPF_STATUS_GET(u)) {
            soc_mem_write(u, mem, MEM_BLOCK_ANY,
                          _soc_alpm_rpf_entry(u, new_index[i]),
                          soc_mem_entry_null(u, mem));
        }
    }
}

/* Populate entries in pfx array to logical bucket */
static int
_soc_alpm_bucket_copy(int u, soc_mem_t mem, int vrf,
                      alpm_mem_prefix_array_t *pfx_array,
                      int       bucket_index,
                      void      *old_bufp,
                      void      *old_sip_bufp,
                      void      *new_bufp,
                      void      *new_sip_bufp,
                      int       *new_index,
                      int       *new_index_pos)
{
    int                             rv;
    void                            *bufp, *sip_bufp;
    int                             entsz, key_index;
    uint32                          bank_disable = 0;
    int                             v6, i;
    uint32                          e[SOC_MAX_MEM_FIELD_WORDS];
    bank_disable = soc_alpm_bank_dis(u, vrf);

    v6 = SOC_ALPM_MEM_V6(mem);

    entsz = v6 ? sizeof(defip_alpm_ipv6_64_1_entry_t) :
                 sizeof(defip_alpm_ipv4_1_entry_t);

    for (i = 0; i < pfx_array->count; i++) {
        if (pfx_array->prefix[i]->index > 0) { /* old prefix */

            bufp = (uint8 *)old_bufp + i * entsz;
            if (SOC_URPF_STATUS_GET(u)) {
                sip_bufp = (uint8 *)old_sip_bufp + i * entsz;
            }

            /* entry shouldn't exist, insert the entry into the RAM */
            rv = _soc_alpm_insert_in_bkt(u, mem, bucket_index,
                                         bank_disable, bufp, sip_bufp, e,
                                         &key_index, v6);
        } else { /* for split case only */
            if (!new_bufp || !new_index_pos) {
                rv = SOC_E_PARAM;
                goto _rollback;
            }
            *new_index_pos = i;
            rv = _soc_alpm_insert_in_bkt(u, mem, bucket_index,
                                         bank_disable, new_bufp, new_sip_bufp, e,
                                         &key_index, v6);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "Failed to insert prefix "
                                  "0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),
                       pfx_array->prefix[i]->key[1],
                       pfx_array->prefix[i]->key[2],
                       pfx_array->prefix[i]->key[3],
                       pfx_array->prefix[i]->key[4], bucket_index));

            goto _rollback;
        }
        new_index[i] = key_index;
    }

_rollback:
    if (SOC_FAILURE(rv)) {
        /* Delete entries in new bucket */
        _soc_alpm_bucket_copy_undo(u, mem, pfx_array->count, new_index);
    }

    return rv;
}





static void
_soc_alpm_bucket_inval_undo(int u, soc_mem_t mem,
                            alpm_mem_prefix_array_t *pfx_array,
                            void *old_bufp, void *old_sip_bufp)
{
    int i;
    int entry_size = (L3_DEFIP_ALPM_IPV6_64m == mem ||
                      L3_DEFIP_ALPM_IPV6_64_1m == mem) ?
                      sizeof(defip_alpm_ipv6_64_1_entry_t) :
                      sizeof(defip_alpm_ipv4_1_entry_t);

    for (i = 0; i < pfx_array->count; i++) {
        payload_t *prefix = pfx_array->prefix[i];

        if (prefix->index > 0) {
            soc_mem_write(u, mem, MEM_BLOCK_ALL, prefix->index,
                          (uint8 *)old_bufp + i * entry_size);

            if (SOC_URPF_STATUS_GET(u)) {
                /* UPRF entry */
                soc_mem_write(u, mem, MEM_BLOCK_ALL,
                              _soc_alpm_rpf_entry(u, prefix->index),
                              (uint8 *)old_sip_bufp + i * entry_size);
            }
        }
    }
    return;
}
static int
_soc_alpm_bucket_inval(int u, soc_mem_t mem,
                       alpm_mem_prefix_array_t *pfx_array,
                       void *old_bufp, void *old_sip_bufp)
{
    int i, rv = SOC_E_NONE;

    /* Invalidation process
     * 1. Erase old copied entries
     * 2. Re-validate those entries if invalidation fails
     */

    for (i = 0; i < pfx_array->count; i ++) {
        payload_t *prefix = pfx_array->prefix[i];

        if (prefix->index >= 0) {
            /* zero out old entries now that split is done */
            rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, prefix->index,
                               soc_mem_entry_null(u, mem));
            if (SOC_FAILURE(rv)) {
                break;
            }
            if (SOC_URPF_STATUS_GET(u)) {
                rv = soc_mem_write(u, mem, MEM_BLOCK_ANY, 
                                   _soc_alpm_rpf_entry(u, prefix->index),
                                   soc_mem_entry_null(u, mem));
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
    }
    /* re-validate old bucket entries */
    if (SOC_FAILURE(rv)) {
        _soc_alpm_bucket_inval_undo(u, mem, pfx_array, old_bufp, old_sip_bufp);
    }

    return rv;
}


#endif /* ALPM_USE_RAW_VIEW */



STATIC int
_soc_alpm_split_lpm_init(int             u,
                         soc_mem_t       mem,
                         int             vrf,
                         int             bucket_index,
                         void            *alpm_data,
                         void            *alpm_sip_data,
                         uint32          *pivot,
                         int             length,
                         defip_entry_t   *lpm_entry,
                         uint32          *bpm_len,
                         uint32          *src_discard,
                         uint32          *src_default)
{
    int                             rv = SOC_E_NONE, v6;
    trie_node_t                     *lpm = NULL;
    payload_t                       *bkt_lpm = NULL;
    trie_t                          *pfx_trie;
    uint32                          tmp_pivot[5] = {0};

    v6 = SOC_ALPM_MEM_V6(mem);

    /* prefix trie */
    if (v6) {
        pfx_trie = VRF_PREFIX_TRIE_IPV6(u, vrf);
    } else {
        pfx_trie = VRF_PREFIX_TRIE_IPV4(u, vrf);
    }


    /* find longest prefix that covers pivot */
    lpm = NULL;
    rv = trie_find_lpm(pfx_trie, pivot, length, &lpm);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "unit %d Unable to find lpm for pivot: "
                              "0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),
                   u, pivot[0], pivot[1], pivot[2], pivot[3], pivot[4],
                   length));

        return rv;
    }
    bkt_lpm = ((payload_t *)lpm)->bkt_ptr;

    /* Initialize the lpm entry to insert */
    if (bkt_lpm) {
        defip_alpm_ipv4_1_entry_t alpmv4_entry, alpmv4_sip_entry;
        defip_alpm_ipv6_64_1_entry_t alpmv6_entry, alpmv6_sip_entry;
        int             index, vrf_id;
        void            *bufp = NULL, *sip_bufp = NULL;

        SOC_ALPM_ENTRY_BUF(v6, mem, bufp, alpmv4_entry, alpmv6_entry);
        SOC_ALPM_ENTRY_BUF(v6, mem, sip_bufp, alpmv4_sip_entry, alpmv6_sip_entry);

        /* this means this bucket default route is an actual route */
        /* initialize new pivot's associated data with data of lpm match */
        index = bkt_lpm->index;
        if (index == -1) {
            /* this means the new route is the bucket's default */
            if (alpm_data == NULL || alpm_sip_data == NULL) {
                return SOC_E_PARAM;
            }
            sal_memcpy(bufp, alpm_data, v6 ?
                       sizeof(defip_alpm_ipv6_64_1_entry_t) :
                       sizeof(defip_alpm_ipv4_1_entry_t));
            if (SOC_URPF_STATUS_GET(u)) {
                *src_default = soc_mem_field32_get(u, mem, alpm_sip_data,
                                                          DEFAULTROUTEf);
                *src_discard = soc_mem_field32_get(u, mem, alpm_sip_data,
                                                          SRC_DISCARDf);
            }
        } else {
            rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY, index, bufp);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
            if (SOC_URPF_STATUS_GET(u)) {
                rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY,
                                        _soc_alpm_rpf_entry(u, index),
                                        sip_bufp);
                *src_default = soc_mem_field32_get(u, mem, sip_bufp,
                                                          DEFAULTROUTEf);
                *src_discard = soc_mem_field32_get(u, mem, sip_bufp,
                                                          SRC_DISCARDf);
            }
        }
        if (SOC_FAILURE(rv)) {
            return rv;
        }

        vrf_id = vrf;
        if (vrf == (SOC_VRF_MAX(u) + 1)) {
            vrf_id = SOC_L3_VRF_GLOBAL;
        }
        rv = _soc_alpm_lpm_ent_init(u, bufp, mem, v6, vrf_id, bucket_index,
                                    0, lpm_entry,
                                    VRF_FLEX_IS_SET(u, vrf_id, vrf, v6, 1));
        if (SOC_FAILURE(rv)) {
            return rv;
        }
        /* remember the bpm_len of this pivot. Need to put in shadow */
        *bpm_len = bkt_lpm->len;
    } else {
        defip_entry_t *lpm_key;

        lpm_key = v6 ? VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf) :
              VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf);
        sal_memcpy(lpm_entry, lpm_key, sizeof(defip_entry_t));

        /* the sip ad does not matter here */
        soc_L3_DEFIPm_field32_set(u, lpm_entry, ALG_BKT_PTR0f, bucket_index);
    }

    /* Convert is a self-modified op, thus use tmp one */
    sal_memcpy(tmp_pivot, pivot, sizeof(tmp_pivot));

    /* Convert from trie format to h/w format */
    ALPM_TRIE_TO_NORMAL_IP(tmp_pivot, length, v6);
    (void) _soc_alpm_lpm_ent_key_init(u, tmp_pivot, length, vrf, v6,
                          lpm_entry, 0);

    return rv;
}

static int
_soc_alpm_split_pivot_create_undo(int u, int v6, int vrf, uint32 *pivot,
                                  uint32 length, alpm_pivot_t *alpm_pivot,
                                  int clear)
{
    int                         rv = SOC_E_NONE, pivot_index;
    alpm_bucket_handle_t        *bucket_handle;
    trie_t                      *pivot_trie = NULL;
    trie_node_t                 *delp = NULL;

    if (v6) {
        pivot_trie = VRF_PIVOT_TRIE_IPV6(u, vrf);
    } else {
        pivot_trie = VRF_PIVOT_TRIE_IPV4(u, vrf);
    }

    if (alpm_pivot == NULL) {
        rv = trie_delete(pivot_trie, pivot, length, &delp);
        alpm_pivot = (alpm_pivot_t *) delp;
    }
    if (SOC_SUCCESS(rv)) {
        bucket_handle   = alpm_pivot->bucket;
        pivot_index     = PIVOT_TCAM_INDEX(alpm_pivot);

        trie_destroy(PIVOT_BUCKET_TRIE(alpm_pivot));
        sal_free(alpm_pivot);
        sal_free(bucket_handle);

        /* For normal destroy (other than rollback) when the entry is deleted
         * from lpm. The pivot_index is already replaced to another index.
         */
        if (clear) {
            ALPM_TCAM_PIVOT(u, pivot_index) = NULL;
        }
    }

    return rv;
}
static int
_soc_alpm_split_pivot_create(int u, int v6, int vrf, int bucket_index,
                             trie_node_t     *split_trie_root,
                             uint32          *pivot,
                             uint32          length,
                             uint32          bpm_len,
                             alpm_pivot_t    **new_pivot)
{
    int                             rv = SOC_E_NONE;
    alpm_pivot_t                    *pivot_pyld;
    alpm_bucket_handle_t            *bucket_handle;
    trie_t                          *pivot_trie = NULL;
    if (v6) {
        pivot_trie = VRF_PIVOT_TRIE_IPV6(u, vrf);
    } else {
        pivot_trie = VRF_PIVOT_TRIE_IPV4(u, vrf);
    }

    /* Add New Pivot to the pivot trie */
    bucket_handle = sal_alloc(sizeof(alpm_bucket_handle_t),
                             "ALPM Bucket Handle");
    if (bucket_handle == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to allocate memory "
                              "for PIVOT trie node \n")));
        return SOC_E_MEMORY;
    }
    sal_memset(bucket_handle, 0, sizeof(*bucket_handle));
    pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),
                            "Payload for new Pivot");
    if (pivot_pyld == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to allocate memory "
                              "for PIVOT trie node \n")));
        sal_free(bucket_handle);
        return SOC_E_MEMORY;
    }
    sal_memset(pivot_pyld, 0, sizeof(*pivot_pyld));
    PIVOT_BUCKET_HANDLE(pivot_pyld) = bucket_handle;
    (void) trie_init(v6 ? _MAX_KEY_LEN_144_ :_MAX_KEY_LEN_48_,
                     &PIVOT_BUCKET_TRIE(pivot_pyld));
    PIVOT_BUCKET_TRIE(pivot_pyld)->trie = split_trie_root;
    PIVOT_BUCKET_INDEX(pivot_pyld) = bucket_index;
    PIVOT_BUCKET_VRF(pivot_pyld) = vrf;
    PIVOT_BUCKET_IPV6(pivot_pyld) = v6;
    PIVOT_BUCKET_DEF(pivot_pyld) = FALSE;
    PIVOT_TCAM_BPMLEN(pivot_pyld)= bpm_len;

    (pivot_pyld)->key[0] = pivot[0];
    (pivot_pyld)->key[1] = pivot[1];
    (pivot_pyld)->key[2] = pivot[2];
    (pivot_pyld)->key[3] = pivot[3];
    (pivot_pyld)->key[4] = pivot[4];
    (pivot_pyld)->len    = length;

    /* insert into pivot trie */
    rv = trie_insert(pivot_trie, pivot, NULL, length,
                     (trie_node_t *)pivot_pyld);
    if (SOC_SUCCESS(rv)) {
        if (new_pivot) {
            *new_pivot = pivot_pyld;
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "failed to insert into pivot trie\n")));
        _soc_alpm_split_pivot_create_undo(u, v6, vrf, pivot,
                                          length, pivot_pyld, 1);
    }

    return rv;
}

static int
_soc_alpm_bucket_split(int u, alpm_pfx_info_t *pfx_info, int *index_ptr)
{
    trie_node_t             *split_trie_root;
    uint32                  e[SOC_MAX_MEM_FIELD_WORDS];
    uint32                  length, bpm_len = 0;
    uint32                  bank_disable = 0;
    uint32                  pivot[5] = {0};
    uint32                  src_discard, src_default;
    int                     key_index, i;
    trie_t                  *bkt_trie;
    defip_entry_t           lpm_entry;
    alpm_mem_prefix_array_t *pfx_array = NULL;
    int                     *new_index = NULL;
    int                     rv = SOC_E_NONE, new_index_pos = -1;
    int                     tcam_index, pivot_index;
    alpm_pivot_t            *new_pivot = NULL;
    int         v6 = (L3_DEFIP_ALPM_IPV6_64m == pfx_info->mem ||
                      L3_DEFIP_ALPM_IPV6_64_1m == pfx_info->mem ||
                      L3_DEFIP_ALPM_IPV6_128m == pfx_info->mem);
#ifdef ALPM_USE_RAW_VIEW
    void                    *raw_bkt[RAW_BKT_NUM] = {0};
#else
    void                    *bufp = NULL, *sip_bufp = NULL;
#endif
    enum {
        _SPLIT_TRIE_NONE,
        _SPLIT_TRIE_SPLIT,
        _SPLIT_LPM_INIT,
        _SPLIT_PIVOT_CREATE,
        _SPLIT_PREPARE,
        _SPLIT_COPY,
        _SPLIT_LPM_INSERT,
        _SPLIT_INVALIDATE
    } doing = _SPLIT_TRIE_NONE, done = _SPLIT_TRIE_NONE;

    doing = _SPLIT_TRIE_SPLIT;
    bkt_trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT(u, pfx_info->pivot_idx_from));
    rv = trie_split(bkt_trie, v6 ? _MAX_KEY_LEN_144_ : _MAX_KEY_LEN_48_, FALSE,
                    pivot, &length, &split_trie_root, NULL, FALSE, 1024);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _SPLIT_LPM_INIT;
    rv = _soc_alpm_split_lpm_init(u,
                                  pfx_info->mem,
                                  pfx_info->vrf,
                                  pfx_info->log_bkt_to,
                                  pfx_info->alpm_data,
                                  pfx_info->alpm_sip_data,
                                  pivot, length,
                                  &lpm_entry, &bpm_len,
                                  &src_discard, &src_default);

    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _SPLIT_PIVOT_CREATE;
    rv = _soc_alpm_split_pivot_create(u, v6, pfx_info->vrf, pfx_info->log_bkt_to,
                                      split_trie_root, pivot, length, bpm_len,
                                      &new_pivot);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    /* Optimized Memory invalidate,
     * Invalidate need to happen only after all the entries are
     * written to the new pivot and the new pivot is valid. Otherwise
     * there is a chance of MISS.
     *
     * hitless move:
     * - Move entries to new bucket.
     * - activate new pivot
     * - delete entries from old bucket
     */
    bank_disable = soc_alpm_bank_dis(u, pfx_info->vrf);

    doing = _SPLIT_PREPARE;
#ifdef ALPM_USE_RAW_VIEW
    rv = _soc_alpm_raw_bucket_prepare(u, pfx_info->mem, PIVOT_BUCKET_TRIE(new_pivot),
                                      pfx_info->log_bkt_from, pfx_info->log_bkt_to,
                                      bank_disable,
                                      pfx_info->alpm_data, pfx_info->alpm_sip_data,
                                      &pfx_array, raw_bkt, &new_index,
                                      &new_index_pos, 0);
#else
    rv = _soc_alpm_bucket_prepare(u, pfx_info->mem, PIVOT_BUCKET_TRIE(new_pivot),
                                  &pfx_array, &bufp, &sip_bufp, &new_index);
#endif
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _SPLIT_COPY;

#ifdef ALPM_USE_RAW_VIEW
    rv = _soc_alpm_raw_bucket_copy(u, pfx_info->mem, pfx_info->log_bkt_to,
                                   bank_disable, raw_bkt[RAW_NEW_BKT_DIP],
                                   raw_bkt[RAW_NEW_BKT_SIP],
                                   new_index, pfx_array->count);
#else
    rv = _soc_alpm_bucket_copy(u, pfx_info->mem, pfx_info->vrf, pfx_array,
                               pfx_info->log_bkt_to,
                               bufp, sip_bufp, pfx_info->alpm_data,
                               pfx_info->alpm_sip_data, new_index, &new_index_pos);
#endif
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _SPLIT_LPM_INSERT;
    rv = soc_alpm_lpm_insert(u, &lpm_entry, &tcam_index, src_default, src_discard, bpm_len);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    /* TCAM index from lpm code is logical. Convert to physical idx */
    tcam_index = soc_alpm_physical_idx(u, L3_DEFIPm,  tcam_index, v6);
    pivot_index = tcam_index << (v6 ? 1 : 0);
    PIVOT_TCAM_INDEX(new_pivot)         = pivot_index;
    ALPM_TCAM_PIVOT(u, pivot_index)     = new_pivot;

    doing = _SPLIT_INVALIDATE;
#ifdef ALPM_USE_RAW_VIEW
    rv = _soc_alpm_raw_bucket_inval(u, pfx_info->mem, pfx_info->log_bkt_from,
                                    bank_disable,
                                    raw_bkt[RAW_OLD_BKT_DIP],
                                    raw_bkt[RAW_OLD_BKT_SIP],
                                    raw_bkt[RAW_RB_BKT_DIP],
                                    raw_bkt[RAW_RB_BKT_SIP]);
#else
    rv = _soc_alpm_bucket_inval(u, pfx_info->mem, pfx_array, bufp, sip_bufp);
#endif
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    if (new_index_pos != -1) { /* New prefix goes to new split bucket */
        *index_ptr = new_index[new_index_pos];
    } else { /* To old parent bucket, do actual insert */
        /* Add Entry to old bucket RAM */
        rv = _soc_alpm_insert_in_bkt(u, pfx_info->mem, pfx_info->log_bkt_from, /* this should be from old pivot*/
                                     bank_disable, pfx_info->alpm_data, pfx_info->alpm_sip_data,
                                     e, &key_index, v6);

        if (SOC_FAILURE(rv)) {
            /* recover by matching trie state to h/w state */
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u, "Could not insert new "
                                  "prefix into trie after split\n")));
            /* since split was successful, leave split state as-is */
            pfx_info->log_bkt_to = -1;
            return rv;
        }

        *index_ptr = key_index;
    }

    PIVOT_BUCKET_ENT_CNT_UPDATE(new_pivot);
    VRF_BUCKET_SPLIT_INC(u, pfx_info->vrf, v6);

    /* Update new index */
    for (i = 0; i < pfx_array->count; i++) {
        pfx_array->prefix[i]->index = new_index[i];
    }
#ifdef ALPM_USE_RAW_VIEW
    _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);
#else
    _soc_alpm_bucket_prepare_undo(u, &pfx_array, &bufp, &sip_bufp, &new_index);
#endif

    return rv;

_rollback:
    /* fall through on all cases */
    switch (done)  {
    case _SPLIT_LPM_INSERT:
        soc_alpm_lpm_delete(u, &lpm_entry);

    case _SPLIT_COPY:
        /* Delete entries in new bucket */
#ifdef ALPM_USE_RAW_VIEW
        _soc_alpm_raw_bucket_copy_undo(u, pfx_info->mem, pfx_info->log_bkt_to, bank_disable,
                                       raw_bkt[RAW_NEW_BKT_DIP],
                                       raw_bkt[RAW_NEW_BKT_SIP], new_index, pfx_array->count);
#else
        _soc_alpm_bucket_copy_undo(u, pfx_info->mem, pfx_array->count, new_index);
#endif

    case _SPLIT_PREPARE:
#ifdef ALPM_USE_RAW_VIEW
        _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);
#else
        _soc_alpm_bucket_prepare_undo(u, &pfx_array, &bufp, &sip_bufp, &new_index);
#endif

    case _SPLIT_PIVOT_CREATE:
        _soc_alpm_split_pivot_create_undo(u, v6, pfx_info->vrf, pivot, length, NULL, 0);

    case _SPLIT_LPM_INIT:
        /* nothing to undo */

    case _SPLIT_TRIE_SPLIT:
        trie_merge(bkt_trie, split_trie_root, pivot, length);

    case _SPLIT_TRIE_NONE:
    default:
        /* nothing to rollback for first step */
        break;
    }

    switch (doing) {
    case _SPLIT_TRIE_SPLIT:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Could not split bucket")));
        break;

    case _SPLIT_PIVOT_CREATE:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u, "failed to insert into pivot bkt_trie\n")));
        break;

    case _SPLIT_LPM_INSERT:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to add new pivot to tcam\n")));

        if (rv == SOC_E_FULL) {
            VRF_PIVOT_FULL_INC(u, pfx_info->vrf, v6);
        }
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Split fails at step %d\n"), doing));
        break;
    }

    return rv;
}


/*
 * Insert entry into ALPM database.
 * Returns SOC_E_NONE, if found and updated.
 * SOC_E_FAIL, if insert failed.
 */
static int
_soc_alpm_insert(int u,
                 int v6,
                 void *key_data,  /* TCAM entry data */
                 soc_mem_t mem,   /* Mem view for ALPM data */
                 void *alpm_data, /* Alpm data to be updated with */
                 void *alpm_sip_data,
                 int *index_ptr,
                 int bucket_index,
                 int tcam_index)
{
    alpm_pivot_t *old_pivot;
    defip_aux_scratch_entry_t aux_entry;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    uint32      prefix[5], length;
    int         vrf, vrf_id;
    int         key_index;
    int         rv = SOC_E_NONE;
    uint32      db_type, ent_type, bank_disable;
    int         hit =0;
    int         split = 0;
    trie_t      *bkt_trie = NULL , *pfx_trie = NULL;
    trie_node_t *lpmp = NULL, *lpmp2 = NULL;
    payload_t   *bkt_payload = NULL, *pfx_payload = NULL, *tmp_pyld;
    int         default_route = 0;
    /* Search Result buffers */
    alpm_pfx_info_t pfx_info;
    int         new_bucket_index = -1;
    int         bkt_trie_inserted = 0, pfx_trie_inserted = 0;
    int         shuffled = 0;
    int         aux_flags = 0;

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));
    bank_disable = soc_alpm_bank_dis(u, vrf);
    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);

    /* Get table memory. */
    SOC_ALPM_TABLE_MEM(u, v6, mem, VRF_FLEX_IS_SET(u, vrf_id, vrf, v6, 1));

    /* Insert the prefix into the bucket trie, whether the insert was
     * successul or resulted in Split. Spliting the trie after insertion
     * yeilds better pivot and better split.
     */
    /* Create Prefix */
    rv = _alpm_prefix_create(u, key_data, prefix, &length, &default_route);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_soc_alpm_insert: prefix create failed\n")));
        return rv;
    }

    /* Fill in AUX Scratch and perform PREFIX Operation */
    sal_memset(&aux_entry, 0, sizeof(defip_aux_scratch_entry_t));
    SOC_IF_ERROR_RETURN(
       _soc_alpm_fill_aux_entry_for_op(u, key_data, v6, db_type, ent_type, 0,
                                       &aux_entry));

    if (bucket_index == 0) {
        if (SOC_ALPM_SW_LOOKUP(u)) {
            rv = _soc_alpm_sw_pivot_find(u, prefix, length, v6, vrf, &hit,
                                         &tcam_index, &bucket_index);
            SOC_IF_ERROR_RETURN(rv);
        } else {
            /* no cookie provided */
            SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(u, PREFIX_LOOKUP, &aux_entry,
                                TRUE, &hit, &tcam_index, &bucket_index, 0));
        }

        if (hit == 0) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_insert: "
                                  "Could not find bucket to insert prefix\n")));
            return SOC_E_NOT_FOUND;
        }
    }
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        bucket_index &= ~0x1;
    }

    /* entry shouldn't exist, insert the entry into the RAM */
    rv = _soc_alpm_insert_in_bkt(u, mem, bucket_index,
                                 bank_disable, alpm_data, alpm_sip_data,
                                 e, &key_index, v6);

    if (rv == SOC_E_NONE)  {
        *index_ptr = key_index;
    } else if (rv == SOC_E_FULL) {
        /* The wish count is a matter of choice, but it must be bigger than
         * old_count to yeild a better result: find a new home as much
         * as possible to avoid split.
         * The sense behind this is splitting will result in repartition,
         * which is more costly.
         */
        rv = soc_alpm_assign(u, vrf, mem, &new_bucket_index, &shuffled);
        SOC_IF_ERROR_RETURN(rv);
        if (shuffled) {
            if (SOC_ALPM_SW_LOOKUP(u)) {
                rv = _soc_alpm_sw_pivot_find(u, prefix, length, v6, vrf, &hit,
                                             &tcam_index, &bucket_index);
            } else {
                rv = _soc_alpm_aux_op(u, PREFIX_LOOKUP, &aux_entry, TRUE, &hit,
                                      &tcam_index, &bucket_index, 0);
            }
            SOC_IF_ERROR_RETURN(rv);
        }

        split = 1;
        /* this indicates a new route, which will be used to decide
         * if the new route happens to be new pivot.
         */
        key_index = -1;
    } else {
        return rv;
    }

    old_pivot = ALPM_TCAM_PIVOT(u, tcam_index);
    if (old_pivot == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u, "ins: pivot index %d bkt %d is not valid \n"),
                  tcam_index, bucket_index));
        return SOC_E_PARAM;
    }

    /* Allocate Payload */
    bkt_payload = sal_alloc(sizeof(payload_t), "Payload for Key");
    if (bkt_payload == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to allocate memory for "
                              "bkt_payload entry\n")));
        rv = SOC_E_MEMORY;
        goto _rollback;
    }

    pfx_payload = sal_alloc(sizeof(payload_t), "Payload for pfx trie key");
    if (pfx_payload == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to allocate memory for "
                              "pfx trie node\n")));
        rv = SOC_E_MEMORY;
        goto _rollback;
    }
    sal_memset(bkt_payload, 0, sizeof(*bkt_payload));
    sal_memset(pfx_payload, 0, sizeof(*pfx_payload));
    sal_memcpy(bkt_payload->key, prefix, sizeof(prefix));
    bkt_payload->len    = length;
    /* Needs update for split case, updated below */
    bkt_payload->index  = key_index;
    /* create duplicate for prefix bkt_trie */
    sal_memcpy(pfx_payload, bkt_payload, sizeof(*bkt_payload));
    pfx_payload->bkt_ptr = bkt_payload;

    /* Insert the prefix */
    /* bucket trie insert */
    bkt_trie = PIVOT_BUCKET_TRIE(old_pivot);
    rv = trie_insert(bkt_trie, prefix, NULL, length, (trie_node_t *)bkt_payload);
    if (SOC_SUCCESS(rv)) {
        bkt_trie_inserted = 1;
    } else {
        goto _rollback;
    }

    /* prefix trie insert */
    if (v6) {
        pfx_trie = VRF_PREFIX_TRIE_IPV6(u, vrf);
    } else {
        pfx_trie = VRF_PREFIX_TRIE_IPV4(u, vrf);
    }
    if (!default_route) {
        /* default route already in trie */
        rv = trie_insert(pfx_trie, prefix, NULL, length,
                        (trie_node_t *)pfx_payload);
        if (SOC_SUCCESS(rv)) {
            pfx_trie_inserted = 1;
        }
    } else {
        /* update the default route */
        lpmp = NULL;
        rv = trie_find_lpm(pfx_trie, 0, 0, &lpmp);
        tmp_pyld = (payload_t *) lpmp;
        if (SOC_SUCCESS(rv)) {
            tmp_pyld->bkt_ptr = bkt_payload;
        }
    }
    if (SOC_FAILURE(rv)) {
        goto _rollback;
    }

    /* Performs lazy splitting */
    if (split) { /* Split */
        sal_memset(&pfx_info, 0, sizeof(pfx_info));
        pfx_info.pivot_idx_from   = tcam_index;
        pfx_info.alpm_data        = alpm_data;
        pfx_info.alpm_sip_data    = alpm_sip_data;
        pfx_info.log_bkt_from     = bucket_index;
        pfx_info.log_bkt_to       = new_bucket_index;
        pfx_info.vrf              = vrf;
        pfx_info.mem              = mem;
        rv = _soc_alpm_bucket_split(u, &pfx_info, index_ptr);
        if (rv != SOC_E_NONE) {
            new_bucket_index = pfx_info.log_bkt_to;
            goto _rollback;
        }
        bkt_payload->index  = *index_ptr;
        /* New Pivot */
        soc_alpm_dbg_cnt[u].bkt_split++;
    }

    /* from here on, we don't expect any failure */

    /* propagate the insertion */
    if (default_route) {
        /* also free unused pfx_payload pointer */
        sal_free(pfx_payload);
    }

    if (SOC_URPF_STATUS_GET(u)) {
        if (soc_mem_field32_get(u, mem, alpm_sip_data, DEFAULTROUTEf)) {
            aux_flags |= SOC_ALPM_AUX_DEF_ROUTE;
        }
        if (soc_mem_field32_get(u, mem, alpm_sip_data, SRC_DISCARDf)) {
            aux_flags |= SOC_ALPM_AUX_SRC_DISCARD;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(u, INSERT_PROPAGATE, &aux_entry,
                        TRUE, &hit, &tcam_index, &bucket_index, aux_flags));

    /* For debug, update the old pivot's counter */
    PIVOT_BUCKET_ENT_CNT_UPDATE(old_pivot);
    return rv;

_rollback:
    if (!default_route && pfx_trie_inserted) {
        (void)trie_delete(pfx_trie, prefix, length, &lpmp);
    }

    if (bkt_trie_inserted) {
        (void)trie_delete(bkt_trie, prefix, length, &lpmp2);
    }

    if (pfx_payload) {
        sal_free(pfx_payload);
    }

    if (bkt_payload) {
        sal_free(bkt_payload);
    }

    if (split && new_bucket_index != -1) {
        /* for non-split case, the new logical bucket is taking effect and thus
         * should not be released
         */
         (void)alpm_bucket_release(u, new_bucket_index, v6);
    }
    return rv;
}



/* Build an LPM entry, from a key */
/* Mostly used for the match */
static int
_soc_alpm_lpm_ent_key_init(int unit, uint32 *key, int len, int vrf, int ipv6,
                           defip_entry_t *lpm_entry, int init)
{
    uint32 mask;

    /* Zero buffers. */
    if (init) {
        sal_memset(lpm_entry, 0, sizeof(defip_entry_t));
    }

    /* and with mask to ensure global vrf gets set to 0 */
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f,
                              vrf & SOC_VRF_MAX(unit));
    if (vrf == (SOC_VRF_MAX(unit) + 1)) {
        /* for global routes */
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f, 0);
    } else {
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f,
                              SOC_VRF_MAX(unit));
    }

    if (ipv6) {
        /* Set address to the buffer. */
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR0f, key[0]);

        /* Set address to the buffer. */
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR1f, key[1]);
        /* Set mode to ipv6 */
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, MODE0f, 1);
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, MODE1f, 1);

        /* Set Virtual Router id */
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_1f, vrf &
                                  SOC_VRF_MAX(unit));
        if (vrf == (SOC_VRF_MAX(unit) + 1)) {
            /* for global routes */
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK1f, 0);
        } else {
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK1f,
                                  SOC_VRF_MAX(unit));
        }
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VALID1f, 1);

        if (len >= 32) {
            mask = 0xffffffff;
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, mask);
            mask = ~SHR_SHIFT_RIGHT(0xffffffff, len - 32);
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, mask);
        } else {
            mask = ~(0xffffffff >> len);
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, mask);
            /* make sure lower word of mask is 0 */
            SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, 0);
        }
    } else {
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR0f, key[0]);

        /* Set address to the buffer. */
        assert(len <= 32);
        mask = (len == 32) ? 0xffffffff : ~(0xffffffff >> len);
        SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, mask);
    }
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, VALID0f, 1);

    /* Set Mode Masks */
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, MODE_MASK0f,
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK0f)) - 1);
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, MODE_MASK1f,
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK1f)) - 1);

    /* Set Entry Type Masks */
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE_MASK0f,
                        (1 << soc_mem_field_length(unit, L3_DEFIPm,
                         ENTRY_TYPE_MASK0f)) - 1);
    SOC_MEM_OPT_F32_SET(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE_MASK1f,
                        (1 << soc_mem_field_length(unit, L3_DEFIPm,
                         ENTRY_TYPE_MASK1f)) - 1);
    /*
     * Note ipv4 entries are expected to reside in part 0 of the entry.
     *      ipv6 entries both parts should be filled.
     *      Hence if entry is ipv6 copy part 0 to part 1
     */
    return (SOC_E_NONE);
}


int
_soc_alpm_delete_in_bkt(int u, soc_mem_t mem, int delete_bucket,
                        int bank_disable, void *bufp2, uint32 *e,
                        int *key_index, int v6)
{
    int rv;

    rv = soc_mem_alpm_delete(u, mem, delete_bucket, MEM_BLOCK_ALL,
                             bank_disable, bufp2, e, key_index);
    if (SOC_SUCCESS(rv)) {
        return rv;
    }
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        return soc_mem_alpm_delete(u, mem, delete_bucket + 1, MEM_BLOCK_ALL,
                                bank_disable, bufp2, e, key_index);
    }
    return rv;
}


#ifdef CHECK_CLEAN_BKT
static int
_soc_alpm_bkt_count(int u, soc_mem_t mem, int bkt_idx, int v6, void *bufp)
{
    int bank, entry;
    int addr, rv;
    int count = 0;

    for (bank = 0; bank < 4; bank++) {
      for (entry = 0; entry < (v6 ? 4 : 6); entry++) {
        addr = (entry << 16) | (bkt_idx << 2) | (bank & 3);
        rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY, addr, bufp);
        SOC_IF_ERROR_RETURN(rv);
        if (soc_mem_field32_get(u, mem, bufp, VALIDf)) {
            count++;
        }
      }
    }
    return count;
}
#endif /* CHECK_CLEAN_BKT */

/*
 * Delete entry from ALPM database.
 * Returns SOC_E_NONE, if found and updated.
 * SOC_E_FAIL, if insert failed.
 * If default route replace with internal default
 */
static int
_soc_alpm_delete(int u, void *key_data, int bucket_index,
                 int tcam_index, int key_index, int propagate)  /* TCAM entry data */
{
    alpm_pivot_t *pivot_pyld;
    /* Search Result buffer */
    defip_alpm_ipv4_1_entry_t alpmv4_entry, alpmv4_sip_entry, alpmv42;
    defip_alpm_ipv6_64_1_entry_t alpmv6_entry, alpmv6_sip_entry, alpmv62;
    defip_aux_scratch_entry_t aux_entry;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t mem;
    void      *bufp, *bufp2, *sip_bufp = NULL;
    int       vrf_id;
    int       ipv6;
    int       rv = SOC_E_NONE, rv2;
    uint32    prop_pfx[5], prefix[5];
    int       v6, vrf;
    uint32    length;
    int       delete_bucket;
    uint32    db_type, ent_type, bank_disable;
    int       hit, default_route = 0;
    trie_t    *trie, *pfx_trie;
    uint32    bpm_pfx_len;
    defip_entry_t lpm_entry, *lpm_key;
    payload_t *payload = NULL, *pfx_pyld = NULL, *tmp_pyld = NULL;
    trie_node_t *delp = NULL, *lpmp = NULL;
    trie_t    *pivot_trie = NULL;
    int       aux_flags = 0;

    ipv6 = v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0f);
    if (v6) {
        if (!(v6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE1f))) {
            return (SOC_E_PARAM);
        }
    }

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));

    /* For VRF_OVERRIDE (Global High) entries, software needs to maintain the
     * index, Prefix resides in TCAM directly. Handling of these functions are
     * taken care by main insert */
    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);
    if (vrf_id != SOC_L3_VRF_OVERRIDE) {
        bank_disable = soc_alpm_bank_dis(u, vrf);

        /* Create Prefix */
        rv = _alpm_prefix_create(u, key_data, prefix, &length, &default_route);

        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_delete: prefix create failed\n")));
            return rv;
        }
        if (!soc_alpm_mode_get(u)) {
            if (vrf_id != SOC_L3_VRF_GLOBAL) {
                /* allow default deletion only at end */
                if (VRF_TRIE_ROUTES_CNT(u, vrf_id, vrf, v6) > 1) {
                    if (default_route) {
                        LOG_ERROR(BSL_LS_SOC_ALPM,
                                  (BSL_META_U(u,
                                              "VRF %d: Cannot delete default "
                                              "route if other routes are present in "
                                              "this mode"), vrf));
                        return SOC_E_PARAM;
                    }
                }
            }
        }

        /* Get table memory. */
        SOC_ALPM_TABLE_MEM(u, v6, mem, VRF_FLEX_IS_SET(u, vrf_id, vrf, v6, 0));

        /* Assign entry buf based on table being used */
        SOC_ALPM_ENTRY_BUF(v6, mem, bufp, alpmv4_entry, alpmv6_entry);
        SOC_ALPM_ENTRY_BUF(v6, mem, bufp2, alpmv42, alpmv62);

        SOC_ALPM_LPM_LOCK(u);

        if (bucket_index == 0) {
            /* Search the entry */
            rv = _soc_alpm_find(u, key_data, mem, bufp, &tcam_index,
                                &bucket_index, &key_index,
                                SOC_ALPM_SW_LOOKUP(u));
        } else {
            /* key_data has all the info including search result */
            rv = _soc_alpm_mem_ent_init(u, key_data, bufp, 0, mem, 0, 0);
        }

        /* save results as they are lost later */
        sal_memcpy(bufp2, bufp, v6 ? sizeof(alpmv6_entry) :
                   sizeof(alpmv4_entry));

        if (SOC_FAILURE(rv)) {
            SOC_ALPM_LPM_UNLOCK(u);
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_delete: Unable to find prefix for delete\n")));
            return rv;
        }

        /* Entry found
         * 1> Delete entry from trie
         * 2> Find Best Prefix Match
         * 3> Update bpm using replacement prefix op
         * 4> Invalidate Entry in the ALPM database
         */
        /* Delete the prefix from trie*/
        /* Store index to invalidate the prefix */
        delete_bucket = bucket_index;
        if (SOC_ALPM_V6_SCALE_CHECK(u, ipv6)) {
            delete_bucket &= ~0x1;
        }
        pivot_pyld = ALPM_TCAM_PIVOT(u, tcam_index);
        if (pivot_pyld == NULL) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u, "del: pivot index %d bkt %d is not valid \n"),
                      tcam_index, bucket_index));
            SOC_ALPM_LPM_UNLOCK(u);
            return SOC_E_PARAM;
        }

        trie = PIVOT_BUCKET_TRIE(pivot_pyld);

        rv = trie_delete(trie, prefix, length,  &delp);
        payload = (payload_t *) delp;
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_alpm_delete: Error prefix not present in trie \n")));
            SOC_ALPM_LPM_UNLOCK(u);
            return rv;
        }

        /* prefix trie delete */
        if (v6) {
            pfx_trie = VRF_PREFIX_TRIE_IPV6(u, vrf);
        } else {
            pfx_trie = VRF_PREFIX_TRIE_IPV4(u, vrf);
        }

        if (v6) {
            pivot_trie = VRF_PIVOT_TRIE_IPV6(u, vrf);
        } else {
            pivot_trie = VRF_PIVOT_TRIE_IPV4(u, vrf);
        }

        if (!default_route) {
            rv = trie_delete(pfx_trie, prefix, length, &delp);
            pfx_pyld = (payload_t *) delp;

            if (SOC_FAILURE(rv)) {
                /* continue anyway to minimize state mismatch */
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_delete: Prefix not present in pfx"
                                      "trie: 0x%08x 0x%08x\n"),
                           prefix[0], prefix[1]));
                goto recover_bkt_trie;
            }

            if (propagate) {
                /* Find Best prefix match for the prefix */
                lpmp = NULL;
                rv = trie_find_lpm(pfx_trie, prefix, length, &lpmp);
                tmp_pyld = (payload_t *) lpmp;
                if (SOC_SUCCESS(rv)) {
                    payload_t *t = (payload_t *)(tmp_pyld->bkt_ptr);
                    if (t != NULL) {
                        /* this means a real default route exists */
                        bpm_pfx_len = t->len;
                    } else {
                        /* only internal default route exists */
                        /* this should happen only if the lpm is a default route */
                        bpm_pfx_len = 0;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                                          "_soc_alpm_delete: Could not find replacement"
                                          "bpm for prefix: 0x%08x 0x%08x\n"),
                               prefix[0], prefix[1]));
                    goto recover_pfx_trie;
                }

                /* Initialize an lpm enty to perform a match */
                /* not expected to fail */


                sal_memcpy(prop_pfx, prefix, sizeof(prefix));
                ALPM_TRIE_TO_NORMAL_IP(prop_pfx, length, v6);
                (void) _soc_alpm_lpm_ent_key_init(u, prop_pfx, bpm_pfx_len, vrf, ipv6,
                                        &lpm_entry, 1);

                /* Find Associated data for the bpm */
                rv = _soc_alpm_find(u, &lpm_entry, mem, bufp, &tcam_index,
                                     &bucket_index, &key_index,
                                     SOC_ALPM_SW_LOOKUP(u));
                if (SOC_SUCCESS(rv)) {
                    /* convert associated data to lpm entry format */
                    (void) _soc_alpm_lpm_ent_init(u, bufp, mem, ipv6, vrf_id,
                                                bucket_index, 0, &lpm_entry, 0);

                    /* set key to the prefix being deleted */
                    (void) _soc_alpm_lpm_ent_key_init(u, prop_pfx, length, vrf, ipv6,
                                           &lpm_entry, 0);

                    if (SOC_URPF_STATUS_GET(u)) {
                        SOC_ALPM_ENTRY_BUF(v6, mem, sip_bufp, alpmv4_sip_entry,
                                           alpmv6_sip_entry);
                        rv = _soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY,
                                 _soc_alpm_rpf_entry(u, key_index), sip_bufp);

                        if (soc_mem_field32_get(u, mem, sip_bufp, DEFAULTROUTEf)) {
                            aux_flags |= SOC_ALPM_AUX_DEF_ROUTE;
                        }
                        if (soc_mem_field32_get(u, mem, sip_bufp, SRC_DISCARDf)) {
                            aux_flags |= SOC_ALPM_AUX_SRC_DISCARD;
                        }
                    }

                } else if (bpm_pfx_len == 0) {
                    /* could not find a default route. Use internal default */
                    /* default route's associated data */

                    lpm_key = v6 ? VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf) :
                          VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf);
                    sal_memcpy(&lpm_entry, lpm_key, sizeof(lpm_entry));
                    /* set key to the prefix being deleted */
                    (void) _soc_alpm_lpm_ent_key_init(u, prop_pfx, length, vrf, ipv6,
                                           &lpm_entry, 0);
                    /* the sip ad does not matter here */
                    rv = SOC_E_NONE;
                }

                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                                          "_soc_alpm_delete: Could not find replacement"
                                          " prefix for prefix: 0x%08x 0x%08x, prop_pfx:"
                                          " 0x%08x 0x%08x, bpm_len %d \n"),
                               prefix[0], prefix[1],
                               prop_pfx[0], prop_pfx[1], bpm_pfx_len));
                    goto recover_pfx_trie;
                }

                lpm_key = &lpm_entry;
            }

        } else { /* !default_route */
            /* remove the real default route reference if any */
            lpmp = NULL;
            rv = trie_find_lpm(pfx_trie, prefix, length, &lpmp);
            tmp_pyld = (payload_t *) lpmp;
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_delete: Could not find default "
                                      "route in the trie for vrf %d\n"), vrf));
                goto recover_bkt_trie;
            }
            tmp_pyld->bkt_ptr = NULL;
            bpm_pfx_len = 0;
            lpm_key = v6 ? VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf) :
                      VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf);
        }

        if (propagate) {
            rv = _soc_alpm_fill_aux_entry_for_op(u, lpm_key, v6, db_type, ent_type,
                                                 bpm_pfx_len, &aux_entry);
            if (SOC_FAILURE(rv)) {
                goto recover_pfx_trie;
            }

            /* Perform Delete propagate */
            rv = _soc_alpm_aux_op(u, DELETE_PROPAGATE, &aux_entry, TRUE, &hit,
                                  &tcam_index, &bucket_index, aux_flags);

            if (SOC_FAILURE(rv)) {
                goto recover_pfx_trie;
            }
        }

        /* From here on don't try to recover trie state */
        sal_free(payload);
        if (!default_route) {
            sal_free(pfx_pyld);
        }

        /* For debug, update the pivot's counter */
        PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);

        if ((pivot_pyld->len != 0) && (trie->trie == NULL)) { /* Empty bucket */
            uint32 hw_key[5];
            sal_memcpy(hw_key, pivot_pyld->key, sizeof(hw_key));
            ALPM_TRIE_TO_NORMAL_IP(hw_key, pivot_pyld->len, ipv6);

            /* Initialize an lpm entry to perform a match */
            (void) _soc_alpm_lpm_ent_key_init(u, hw_key, pivot_pyld->len,
                              vrf, ipv6, &lpm_entry, 1);

            /* delete entry from TCAM */
            rv = soc_alpm_lpm_delete(u, &lpm_entry);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_delete: Unable to "
                                      "delete pivot 0x%08x 0x%08x \n"),
                           pivot_pyld->key[0], pivot_pyld->key[1]));
                /* continue to clean up still */
            }
        }

        /* Invalidate the route */
        rv = _soc_alpm_delete_in_bkt(u, mem, delete_bucket, bank_disable,
                                     bufp2, e, &key_index, v6);
        if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "_soc_th_alpm_delete_in_bkt: Unable to "
                                  "delete phy bkt 0x%08x \n"), delete_bucket)); 
            SOC_ALPM_LPM_UNLOCK(u);
            rv = SOC_E_FAIL;
            return rv;
        }

        /* delete corresponding RPF entry if it exists */
        if (SOC_URPF_STATUS_GET(u)) {
            /* hit is an unused variable */
            rv = _soc_alpm_delete_in_bkt(u, mem,
                                         SOC_ALPM_RPF_BKT_IDX(u, delete_bucket),
                                         bank_disable,
                                         bufp2, e, &hit, v6);
            if (!SOC_SUCCESS(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_th_alpm_delete_in_bkt: Unable to "
                                      "delete phy bkt 0x%08x \n"),
                           SOC_ALPM_RPF_BKT_IDX(u, delete_bucket))); 
                SOC_ALPM_LPM_UNLOCK(u);
                rv = SOC_E_FAIL;
                return rv;
            }
        }

        if ((pivot_pyld->len != 0) && (trie->trie == NULL)) { /* Empty bucket */
            /* Release Bucket */
            rv = alpm_bucket_release(u, PIVOT_BUCKET_INDEX(pivot_pyld), v6);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "_soc_alpm_delete: Unable to release"
                                      "empty bucket: %d\n"),
                           PIVOT_BUCKET_INDEX(pivot_pyld)));
                /* we lose this bucket */
            }

            /* remove pivot from pivot trie */
            rv = trie_delete(pivot_trie, pivot_pyld->key, pivot_pyld->len, &delp);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                      "could not delete pivot from pivot trie\n")));
            }

            /* Free up Bucket trie */
            trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));

            /* Free up Bucket handle */
            sal_free(PIVOT_BUCKET_HANDLE(pivot_pyld));

            /* free Pivot structure */
            sal_free(pivot_pyld);

#ifdef CHECK_CLEAN_BKT
            /* make sure bucket is indeed empty */
            assert(_soc_alpm_bkt_count(u, mem, delete_bucket, v6,
                                            bufp) == 0);
#endif /* CHECK_CLEAN_BKT */
        }
    }
    VRF_TRIE_ROUTES_DEC(u, vrf_id, vrf, v6);
    if (VRF_TRIE_ROUTES_CNT(u, vrf_id, vrf, v6) == 0) {
        /* vrf is empty. Release resources */
        rv = soc_alpm_vrf_delete(u, vrf, v6);
    }
    SOC_ALPM_LPM_UNLOCK(u);
    return rv;

/* Recovery from error: */
    /* add back pivot bucket trie entry */
    /* add back prefix trie entry */
recover_pfx_trie:
    rv2 = trie_insert(pfx_trie, prefix, NULL, length, (trie_node_t *)
                     pfx_pyld);
    if (SOC_FAILURE(rv2)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_soc_alpm_delete: Unable to reinsert"
                              "prefix 0x%08x 0x%08x into pfx trie\n"),
                   prefix[0], prefix[1]));
    }
recover_bkt_trie:
    rv2 = trie_insert(trie, prefix, NULL, length,  (trie_node_t *) payload);
    if (SOC_FAILURE(rv2)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_soc_alpm_delete: Unable to reinsert"
                              "prefix 0x%08x 0x%08x into bkt trie\n"),
                   prefix[0], prefix[1]));
    }

    SOC_ALPM_LPM_UNLOCK(u);
    return rv;
}

/**
 * IPMC_MISS signal is never asserted after an unsuccessful L3_DEFIP table
 * lookup for IPMC packets when IPMC (PIM-SM, PIM-BIDIR) forwarding is enabled
 * in TD2+. Due to this, the forwarding logic miss forwards IPMC
 * packets to an incorrect (or invalid) IPMC group.
 *
 * WAR: Add two default rules as last entries in L3_DEFIP (TCAM) table such that
 * all IPV4 and IPV6 multicast packets will, by default, hit these entries, if
 * any specific IPMC were not added in the L3_DEFIP table. Thus L3_DEFIP lookup
 * for IPMC packets will never result in a miss.
 *
 * Packets copied to CPU due to IPMC_MISS will have OPCODE set as 4
 * (IP Multicast) as opposed to 3 (L2 Multicast). This may cause some mismatches
 * in case of CPU bound packets compared to legacy XGS devices.
 */
int
soc_alpm_ipmc_war(int unit, int install)
{
    int i, ipv6 = 0;
    int index = -1;
    defip_entry_t lpm_entry;
    soc_mem_t mem = L3_DEFIPm;
    int count = 1;
    int num_rp = 63;

    if (!(soc_feature(unit, soc_feature_alpm) &&
          soc_feature(unit, soc_feature_td2p_a0_sw_war) &&
          soc_property_get(unit, spn_L3_ALPM_ENABLE, 0))) {
        return SOC_E_NONE;
    }

    if (!SOC_ALPM_LPM_INIT_CHECK(unit)) {
        return SOC_E_NONE;
    }

    if (!SOC_IS_TRIDENT2PLUS(unit)) {
        return SOC_E_NONE;
    }

    if (SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }

    if (soc_mem_index_count(unit, L3_DEFIPm) <= 0) {
        return SOC_E_NONE;
    }

    if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) <= 0) {
        count = 2;
    }

    num_rp = soc_mem_field_length(unit,
                ING_ACTIVE_L3_IIF_PROFILEm, RPA_ID_PROFILEf);

    /* First loop for ipv4, second loop for ipv6 */
    for (i = 0; i < count; i++) {
        /* Zero Buffer */
        sal_memset(&lpm_entry, 0, soc_mem_entry_words(unit, mem) * 4);
        ipv6 = i;

        soc_mem_field32_set(unit, mem, &lpm_entry, VALID0f, 1);
        soc_mem_field32_set(unit, mem, &lpm_entry, MODE_MASK0f, 3);
        soc_mem_field32_set(unit, mem, &lpm_entry, ENTRY_TYPE_MASK0f, 1);
        soc_mem_field32_set(unit, mem, &lpm_entry, MULTICAST_ROUTE0f, 1);
        soc_mem_field32_set(unit, mem, &lpm_entry, GLOBAL_ROUTE0f, 1);
        soc_mem_field32_set(unit, mem, &lpm_entry, RPA_ID0f, num_rp - 1);
        soc_mem_field32_set(unit, mem, &lpm_entry, EXPECTED_L3_IIF0f, 16383);

        if (ipv6) {
            soc_mem_field32_set(unit, mem, &lpm_entry, VALID1f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, MODE0f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, MODE1f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, MODE_MASK1f, 3);
            soc_mem_field32_set(unit, mem, &lpm_entry, ENTRY_TYPE_MASK1f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, MULTICAST_ROUTE1f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, GLOBAL_ROUTE1f, 1);
            soc_mem_field32_set(unit, mem, &lpm_entry, RPA_ID1f, num_rp - 1);
            /* 16383 indicate a max field value which is used as an invalid value */
            soc_mem_field32_set(unit, mem, &lpm_entry, EXPECTED_L3_IIF1f, 16383);

            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR0f, 0);
            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR_MASK0f, 0);
            /* IPv6 multicast route address starts from ff00 for default match */
            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR1f, 0xff000000);
            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR_MASK1f, 0xff000000);
        } else {
            /* IPv4 multicast route address starts from 224 for default match */
            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR0f, 0xe0000000);
            soc_mem_field32_set(unit, mem, &lpm_entry, IP_ADDR_MASK0f, 0xe0000000);
        }

        if (install) {
            if (soc_feature(unit, soc_feature_alpm_flex_stat)) {
                soc_mem_field32_set(unit, mem, &lpm_entry, ENTRY_VIEW0f, 0x1);
                VRF_FLEX_SET(unit, SOC_L3_VRF_GLOBAL, SOC_VRF_MAX(unit) + 1, ipv6, 1);
            }
            SOC_IF_ERROR_RETURN(soc_alpm_lpm_insert(unit, &lpm_entry, &index,
                                                    0, 0, INVALID_BPM_LEN));
        } else {
            SOC_IF_ERROR_RETURN(soc_alpm_lpm_delete(unit, &lpm_entry));
        }
    }

    return SOC_E_NONE;
}

/* Initialize ALPM Subsytem */
int
soc_alpm_init(int u)
{
    int i;
    int rv = SOC_E_NONE;

    if (SOC_ALPM_LPM_INIT_CHECK(u)) {
        /* this is a reinit. clean up old state */
        if (soc_alpm_deinit(u) < 0) {
            return SOC_E_INTERNAL;
        }
    }

    /* Initialize ALPM control */
    rv = _soc_alpm_ctrl_init(u);
    SOC_IF_ERROR_RETURN(rv);

    /* Initialize TCAM Memory management */
    rv = soc_alpm_lpm_init(u);
    SOC_IF_ERROR_RETURN(rv);

    /* Initialize ALPM memory management */
    rv = soc_alpm_shared_mem_init(u);

    alpm_vrf_handle[u] = sal_alloc((MAX_VRF_ID + 1) * sizeof(alpm_vrf_handle_t), "ALPM VRF Handles");
    if (alpm_vrf_handle[u] == NULL) {
        return SOC_E_MEMORY;
    }

    tcam_pivot[u] = sal_alloc(MAX_PIVOT_COUNT * sizeof(alpm_pivot_t *), "ALPM pivots");
    if (tcam_pivot[u] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(alpm_vrf_handle[u], 0, (MAX_VRF_ID + 1) * sizeof(alpm_vrf_handle_t));
    sal_memset(tcam_pivot[u], 0, MAX_PIVOT_COUNT * sizeof(alpm_pivot_t *));
    sal_memset(&soc_alpm_dbg_cnt[u], 0, sizeof(_alpm_int_dbg_cnt_t));

    /* Initialize PIVOT management */
    for (i = 0; i < MAX_PIVOT_COUNT; i++) {
        ALPM_TCAM_PIVOT(u, i) = NULL;
    }

    if (SOC_CONTROL(u)->alpm_bulk_retry == NULL) {
        SOC_CONTROL(u)->alpm_bulk_retry =
            sal_sem_create("ALPM bulk retry", sal_sem_BINARY, 0);
    }
    if (SOC_CONTROL(u)->alpm_lookup_retry == NULL) {
        SOC_CONTROL(u)->alpm_lookup_retry =
            sal_sem_create("ALPM lookup retry", sal_sem_BINARY, 0);
    }
    if (SOC_CONTROL(u)->alpm_insert_retry == NULL) {
        SOC_CONTROL(u)->alpm_insert_retry =
            sal_sem_create("ALPM insert retry", sal_sem_BINARY, 0);
    }
    if (SOC_CONTROL(u)->alpm_delete_retry == NULL) {
        SOC_CONTROL(u)->alpm_delete_retry =
            sal_sem_create("ALPM delete retry", sal_sem_BINARY, 0);
    }

    rv = soc_alpm_128_lpm_init(u);
    SOC_IF_ERROR_RETURN(rv);

    (void)soc_alpm_ipmc_war(u, TRUE);

    return rv;
}


/* clear alpm related internal state */
static int soc_alpm_state_clear(int u)
{
    int i, rv;
    alpm_pivot_t *tmp;

    /* clear all pivot state */
    for (i = 0; i < MAX_PIVOT_COUNT; i++) {
        tmp = ALPM_TCAM_PIVOT(u, i);
        if (tmp) {
            if (PIVOT_BUCKET_HANDLE(tmp)) {
                if (PIVOT_BUCKET_TRIE(tmp)) {
                    /* traverse bucket trie and clear it */
                    rv = trie_traverse(PIVOT_BUCKET_TRIE(tmp),
                            alpm_delete_node_cb, NULL, _TRIE_INORDER_TRAVERSE);
                    if (SOC_SUCCESS(rv)) {
                        trie_destroy(PIVOT_BUCKET_TRIE(tmp));
                    } else {
                        LOG_ERROR(BSL_LS_SOC_ALPM,
                                  (BSL_META_U(u,
                                              "Unable to clear trie state for unit %d\n"),
                                   u));
                        return rv;
                    }
                }
                sal_free(PIVOT_BUCKET_HANDLE(tmp));
            }
            sal_free(ALPM_TCAM_PIVOT(u, i));
            ALPM_TCAM_PIVOT(u, i) = NULL;
        }
    }

    /* clear all prefix tries */
    for (i = 0; i <= SOC_VRF_MAX(u) + 1; i++) {
        rv = trie_traverse(VRF_PREFIX_TRIE_IPV4(u, i), alpm_delete_node_cb,
                           NULL, _TRIE_INORDER_TRAVERSE);
        if (SOC_SUCCESS(rv)) {
            trie_destroy(VRF_PREFIX_TRIE_IPV4(u, i));
        } else {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "unit: %d Unable to clear v4 pfx trie for vrf %d\n"),
                       u, i));
            return rv;
        }
        rv = trie_traverse(VRF_PREFIX_TRIE_IPV6(u, i), alpm_delete_node_cb,
                           NULL, _TRIE_INORDER_TRAVERSE);
        if (SOC_SUCCESS(rv)) {
            trie_destroy(VRF_PREFIX_TRIE_IPV6(u, i));
        } else {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "unit: %d Unable to clear v6 pfx trie for vrf %d\n"),
                       u, i));
            return rv;
        }
        if (VRF_TRIE_DEFAULT_ROUTE_IPV4(u, i) != NULL) {
            sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV4(u, i));
        }
        if (VRF_TRIE_DEFAULT_ROUTE_IPV6(u, i) != NULL) {
            sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(u, i));
        }
        sal_memset(&alpm_vrf_handle[u][i], 0, sizeof(alpm_vrf_handle_t));
    }

    /* Reset VRF_OVERRIDE debug counter */
    sal_memset(&alpm_vrf_handle[u][MAX_VRF_ID], 0, sizeof(alpm_vrf_handle_t));
    /* Mark Global Hi (VRF_OVERRIDE) for IPv4, IPv6-64, IPv6-128 inited for debug usage */
    VRF_TRIE_INIT_DONE(u, MAX_VRF_ID, 0, 1);
    VRF_TRIE_INIT_DONE(u, MAX_VRF_ID, 1, 1);
    VRF_TRIE_INIT_DONE(u, MAX_VRF_ID, 2, 1);

    if (SOC_ALPM_BUCKET_BMAP(u) != NULL) {
        sal_free(SOC_ALPM_BUCKET_BMAP(u));
    }
    sal_memset(&soc_alpm_bucket[u], 0, sizeof(soc_alpm_bucket_t));
    sal_memset(&soc_alpm_dbg_cnt[u], 0, sizeof(_alpm_int_dbg_cnt_t));

    /* Free memory allocated for VRF handles and tcam pivots */
    if (alpm_vrf_handle[u])
        sal_free(alpm_vrf_handle[u]);
    if (tcam_pivot[u])
        sal_free(tcam_pivot[u]);
    alpm_vrf_handle[u] = NULL;
    tcam_pivot[u]      = NULL;
    return SOC_E_NONE;
}

/* De-Init ALPM Subsytem */
int
soc_alpm_deinit(int u)
{
    soc_alpm_lpm_deinit(u);

    SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(u));

    SOC_IF_ERROR_RETURN(soc_alpm_128_state_clear(u));

    SOC_IF_ERROR_RETURN(soc_alpm_state_clear(u));

    if (SOC_CONTROL(u)->alpm_bulk_retry) {
        sal_sem_destroy(SOC_CONTROL(u)->alpm_bulk_retry);
        SOC_CONTROL(u)->alpm_bulk_retry = NULL;
    }
    if (SOC_CONTROL(u)->alpm_lookup_retry) {
        sal_sem_destroy(SOC_CONTROL(u)->alpm_lookup_retry);
        SOC_CONTROL(u)->alpm_lookup_retry = NULL;
    }
    if (SOC_CONTROL(u)->alpm_insert_retry) {
        sal_sem_destroy(SOC_CONTROL(u)->alpm_insert_retry);
        SOC_CONTROL(u)->alpm_insert_retry = NULL;
    }
    if (SOC_CONTROL(u)->alpm_delete_retry) {
        sal_sem_destroy(SOC_CONTROL(u)->alpm_delete_retry);
        SOC_CONTROL(u)->alpm_delete_retry = NULL;
    }

    return SOC_E_NONE;
}


/*
 * soc_alpm_vrf_add
 * Add a VRF default route when a L3 VRF is added
 * Adds a 0.0 entry into VRF
 */
static int
soc_alpm_vrf_add(int u, int vrf, int v6)
{
    defip_entry_t *lpm_entry = NULL, tmp_lpm;
    int bucket_pointer;
    int index;
    int rv = SOC_E_NONE;
    uint32 key[5] = {0};
    uint32 length;
    alpm_bucket_handle_t *bucket_handle = NULL;
    alpm_pivot_t *pivot_pyld = NULL;
    payload_t   *pfx_pyld = NULL;
    trie_t  *prefix_root = NULL, *bucket_root = NULL;
    int prefix_root_inserted = 0;
    trie_t *pivot_root = NULL;
    int pivot_root_inserted = 0;
    soc_mem_t mem;

    SOC_ALPM_TABLE_MEM(u, v6, mem, VRF_FLEX_IS_SET(u, vrf, vrf, v6, 1));
    rv = soc_alpm_assign(u, vrf, mem, &bucket_pointer, NULL);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Allocate bucket handle */
    bucket_handle = sal_alloc(sizeof(alpm_bucket_handle_t), "ALPM Bucket Handle");
    if (bucket_handle == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_add: Unable to allocate memory for "
                              "PIVOT trie node \n")));
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    sal_memset(bucket_handle, 0, sizeof(*bucket_handle));

    /* Allocate PIVOT Payload */
    pivot_pyld = sal_alloc(sizeof(alpm_pivot_t), "Payload for Pivot");
    if (pivot_pyld == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_add: Unable to allocate memory for "
                              "PIVOT trie node \n")));
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    sal_memset(pivot_pyld, 0, sizeof(*pivot_pyld));
    PIVOT_BUCKET_HANDLE(pivot_pyld) = bucket_handle;

    /* Allocate Prefix Payload */
    pfx_pyld = sal_alloc(sizeof(payload_t), "Payload for pfx trie key");
    if (pfx_pyld == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_add: Unable to allocate memory for "
                              "pfx trie node \n")));
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    sal_memset(pfx_pyld, 0, sizeof(*pfx_pyld));

    /* Initialize VRF pivot trie */
    if (v6 == 0) {
        rv = trie_init(_MAX_KEY_LEN_48_, &VRF_PIVOT_TRIE_IPV4(u, vrf));
        pivot_root = VRF_PIVOT_TRIE_IPV4(u, vrf);
    } else {
        rv = trie_init(_MAX_KEY_LEN_144_, &VRF_PIVOT_TRIE_IPV6(u, vrf));
        pivot_root = VRF_PIVOT_TRIE_IPV6(u, vrf);
    }
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Initialize VRF prefix trie */
    if (v6 == 0) {
        rv = trie_init(_MAX_KEY_LEN_48_, &VRF_PREFIX_TRIE_IPV4(u, vrf));
        prefix_root = VRF_PREFIX_TRIE_IPV4(u, vrf);
    } else {
        rv = trie_init(_MAX_KEY_LEN_144_, &VRF_PREFIX_TRIE_IPV6(u, vrf));
        prefix_root = VRF_PREFIX_TRIE_IPV6(u, vrf);
    }
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Initialize bucket trie */
    if (v6) {
        rv = trie_init(_MAX_KEY_LEN_144_, &PIVOT_BUCKET_TRIE(pivot_pyld));
    } else {
        rv = trie_init(_MAX_KEY_LEN_48_, &PIVOT_BUCKET_TRIE(pivot_pyld));
    }
    bucket_root = PIVOT_BUCKET_TRIE(pivot_pyld);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    PIVOT_BUCKET_INDEX(pivot_pyld) = bucket_pointer;
    PIVOT_BUCKET_VRF(pivot_pyld) = vrf;
    PIVOT_BUCKET_IPV6(pivot_pyld) = v6;
    PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;

    /* 0 for default * route */
    length = 0;
    pivot_pyld->key[0] = pfx_pyld->key[0] = key[0];
    pivot_pyld->key[1] = pfx_pyld->key[1] = key[1];
    pivot_pyld->len = pfx_pyld->len    = length;
    rv = trie_insert(prefix_root, key, NULL, length, &(pfx_pyld->node));
    /* no internal default in bucket trie */
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    prefix_root_inserted = 1;

    /* insert into trie of pivots */
    rv = trie_insert(pivot_root, key, NULL, length, (trie_node_t *)pivot_pyld);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    pivot_root_inserted = 1;

    /* Initialize an lpm entry to perform a match */
    (void) _soc_alpm_lpm_ent_key_init(u, key, 0, vrf, v6, &tmp_lpm, 1);

    /* for global VRF */
    if (vrf == SOC_VRF_MAX(u) + 1) {
        soc_L3_DEFIPm_field32_set(u, &tmp_lpm, GLOBAL_ROUTE0f, 1);
    } else {
        /* Set default miss to global route only for non global routes */
        soc_L3_DEFIPm_field32_set(u, &tmp_lpm, DEFAULT_MISS0f, 1);
    }
    soc_L3_DEFIPm_field32_set(u, &tmp_lpm, ENTRY_VIEW0f,
                              VRF_FLEX_IS_SET(u, vrf, vrf, v6, 1));

    soc_L3_DEFIPm_field32_set(u, &tmp_lpm, ALG_BKT_PTR0f, bucket_pointer);

    /* Allocate default lpm entry */
    lpm_entry = sal_alloc(sizeof(defip_entry_t), "Default LPM entry");
    if (lpm_entry == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_add: unable to allocate memory for "
                              "IPv4 LPM entry\n")));
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(lpm_entry, &tmp_lpm, sizeof(tmp_lpm));

    /* add Entry into tcam as default routes for the VRF */
    /* urpf could modify the lpm entry. So use tmp one */
    rv = soc_alpm_lpm_insert(u, &tmp_lpm, &index, 0, 0, INVALID_BPM_LEN);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    if (v6 == 0) {
        VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf) = lpm_entry;
    } else {
        VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf) = lpm_entry;
    }

    /* get physical index */
    index = soc_alpm_physical_idx(u, L3_DEFIPm, index, v6);
    if (v6 == 0) {
        ALPM_TCAM_PIVOT(u, index) = pivot_pyld;
        PIVOT_TCAM_INDEX(pivot_pyld) = index;
    } else {
        ALPM_TCAM_PIVOT(u, index << 1) = pivot_pyld;
        PIVOT_TCAM_INDEX(pivot_pyld) = index << 1;
    }

    PIVOT_TCAM_BPMLEN(pivot_pyld) = length;
    VRF_TRIE_INIT_DONE(u, vrf, v6, 1);

    return SOC_E_NONE;

cleanup:
    if (lpm_entry) {
        sal_free(lpm_entry);
    }

    if (pivot_root_inserted) {
        trie_node_t *delp = NULL;
        (void) trie_delete(pivot_root, key, length, &delp);
    }

    if (prefix_root_inserted) {
        trie_node_t *delp = NULL;
        (void) trie_delete(prefix_root, key, length, &delp);
    }

    /* destroy inited tries */
    if (bucket_root) {
        (void) trie_destroy(bucket_root);
        PIVOT_BUCKET_TRIE(pivot_pyld) = NULL;
    }
    if (prefix_root) {
        (void) trie_destroy(prefix_root);
        if (v6 == 0) {
            VRF_PREFIX_TRIE_IPV4(u, vrf) = NULL;
        } else {
            VRF_PREFIX_TRIE_IPV6(u, vrf) = NULL;
        }
    }
    if (pivot_root) {
        (void) trie_destroy(pivot_root);
        if (v6 == 0) {
            VRF_PIVOT_TRIE_IPV4(u, vrf) = NULL;
        } else {
            VRF_PIVOT_TRIE_IPV6(u, vrf) = NULL;
        }
    }

    if (pfx_pyld) {
        sal_free(pfx_pyld);
    }
    if (pivot_pyld) {
        sal_free(pivot_pyld);
    }
    if (bucket_handle) {
        sal_free(bucket_handle);
    }

    (void) alpm_bucket_release(u, bucket_pointer, v6);
    return rv;
}


/*
 * soc_alpm_vrf_delete
 * Delete the VRF default route when a L3 VRF is destroyed
 */
static int
soc_alpm_vrf_delete(int u, int vrf, int v6)
{
    defip_entry_t *lpm_entry;
    int bucket_pointer;
    int idx;
    int rv = SOC_E_NONE;
    uint32 key[5] = {0}, result[SOC_MAX_MEM_FIELD_WORDS];
    payload_t *payload;
    alpm_pivot_t *pivot_pyld1;
    trie_node_t *delp;
    trie_t  *root;
    trie_t *pivot_root = NULL;

    if (v6 == 0) {
        lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf);
    } else {
        lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf);
    }

    /* find bucket pointers and release them */
    bucket_pointer = soc_L3_DEFIPm_field32_get(u, lpm_entry, ALG_BKT_PTR0f);
    rv = alpm_bucket_release(u, bucket_pointer, v6);

    /* add Entry into tcam as default routes for the VRF */
    rv = soc_alpm_lpm_match(u, lpm_entry, (void *) result, &idx);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_delete: unable to get internal"
                              " pivot idx for vrf %d/%d\n"), vrf, v6));
        idx = -1;
    }

    idx = soc_alpm_physical_idx(u, L3_DEFIPm, idx, v6);
    if (v6 == 0) {
        pivot_pyld1 = ALPM_TCAM_PIVOT(u, idx);
    } else {
        pivot_pyld1 = ALPM_TCAM_PIVOT(u, idx << 1);
    }

    rv = soc_alpm_lpm_delete(u, lpm_entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "soc_alpm_vrf_delete: unable to delete lpm entry "
                              " for internal default for vrf %d/%d\n"), vrf, v6));
    }

    sal_free(lpm_entry);

    if (v6 == 0) {
        VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf) = NULL;
        root = VRF_PREFIX_TRIE_IPV4(u, vrf);
        VRF_PREFIX_TRIE_IPV4(u, vrf) = NULL;
    } else {
        VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf)  = NULL;
        root = VRF_PREFIX_TRIE_IPV6(u, vrf);
        VRF_PREFIX_TRIE_IPV6(u, vrf) = NULL;
    }
    VRF_TRIE_INIT_DONE(u, vrf, v6, 0);

    rv = trie_delete(root, key, 0, &delp);
    payload = (payload_t *) delp;
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to delete internal default for vrf "
                              " %d/%d\n"), vrf, v6));
    }
    sal_free(payload);

    (void) trie_destroy(root);

    if (v6 == 0) {
        pivot_root = VRF_PIVOT_TRIE_IPV4(u, vrf);
        VRF_PIVOT_TRIE_IPV4(u, vrf) = NULL;
    } else {
        pivot_root = VRF_PIVOT_TRIE_IPV6(u, vrf);
        VRF_PIVOT_TRIE_IPV6(u, vrf) = NULL;
    }

    delp = NULL;
    rv = trie_delete(pivot_root, key, 0, &delp);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unable to delete internal pivot node for vrf"
                              " %d/%d\n"), vrf, v6));
    }
    (void) trie_destroy(pivot_root);

    (void) trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld1));
    sal_free(PIVOT_BUCKET_HANDLE(pivot_pyld1));
    sal_free(pivot_pyld1);

    return rv;
}

/*
 * soc_alpm_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
soc_alpm_insert(int u, void *lpm_entry, uint32 flags, int bkt_idx,
                int pivot_info)
{
    defip_alpm_ipv4_1_entry_t alpmv4_entry, alpmv4_sip_entry;
    defip_alpm_ipv6_64_1_entry_t alpmv6_entry, alpmv6_sip_entry;
    soc_mem_t mem;
    void      *bufp, *bufp2;
    int       vrf_id, vrf;
    int       index;
    int       ipv6;
    int       rv = SOC_E_NONE;
    uint32    default_route;
    int       ipmc_route = 0;
    int       flex_v6_64 = 0;

    ipv6 = soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, MODE0f);

    /* Get table memory. */
    SOC_ALPM_TABLE_MEM(u, ipv6, mem, flags & SOC_ALPM_STAT_FLEX);

    /* Assign entry buf based on table being used */
    SOC_ALPM_ENTRY_BUF(ipv6, mem, bufp, alpmv4_entry, alpmv6_entry);
    SOC_ALPM_ENTRY_BUF(ipv6, mem, bufp2, alpmv4_sip_entry, alpmv6_sip_entry);

    SOC_IF_ERROR_RETURN(
        _soc_alpm_mem_ent_init(u, lpm_entry, bufp, bufp2, mem, flags,
                               &default_route));

    /* If the entry is override, then insert into TCAM directly */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, lpm_entry, &vrf_id, &vrf));

    if (VRF_FLEX_COMPLETED(u, vrf_id, vrf, ipv6)) {
        if (VRF_FLEX_IS_SET(u, vrf_id, vrf, ipv6, 1) != !!(flags & SOC_ALPM_STAT_FLEX)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "soc_alpm_insert:Flex flag conflict\n"
                                  )));
            return SOC_E_PARAM;
        }
    } else {
        VRF_FLEX_SET(u, vrf_id, vrf, ipv6, flags & SOC_ALPM_STAT_FLEX);
    }

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipmc_route = soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, MULTICAST_ROUTE0f);
    }

    if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && ipv6 &&
        (flags & SOC_ALPM_STAT_FLEX)) {
        if (soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f) == 0xFFFFFFFF &&
            soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f) == 0xFFFFFFFF) {
            flex_v6_64 = 1;
        }
    }

    if ((vrf_id != SOC_L3_VRF_OVERRIDE) &&
        (vrf == 0) && soc_alpm_mode_get(u)) {
        /* cannot have 0 as a VRF in parallel mode */
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "VRF=0 cannot be added in Parallel mode\n")));
        return SOC_E_PARAM;
    }

    /* For VRF_OVERRIDE (Global High) entries, and IPMC route
     * software needs to maintain the index
     */
    /* Prefix resides in TCAM directly */
    if ((vrf_id == SOC_L3_VRF_OVERRIDE) || (ipmc_route) || (flex_v6_64)) {

        /* Insert into TCAM directly and return */
        /* search in the hash table */
        rv = soc_alpm_lpm_insert(u, lpm_entry, &index, 0,
                                 flags & SOC_ALPM_RPF_SRC_DISCARD, INVALID_BPM_LEN);
        if (SOC_SUCCESS(rv) && !(pivot_info & SOC_ALPM_LPM_LOOKUP_HIT)) {
            VRF_TRIE_ROUTES_INC(u, vrf_id, vrf, ipv6);
        }

        if (rv == SOC_E_FULL) {
            VRF_PIVOT_FULL_INC(u, MAX_VRF_ID, ipv6);
        }
        return(rv);
    }

    /* combined search mode protection */
    if (vrf_id != SOC_L3_VRF_GLOBAL) {
        if (!soc_alpm_mode_get(u)) {
            /* for emtpy DB allow insert only if route is default */
            if (VRF_TRIE_ROUTES_CNT(u, vrf_id, vrf, ipv6) == 0) {
                /* check if route is default, else throw error */
                if (!default_route) {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                                          "VRF %d: First route in a VRF has to "
                                          " be a default route in this mode\n"), vrf_id));
                    return SOC_E_PARAM;
                }
            }
        }
    }

    /* non override entries, goes into ALPM memory */
    if (!VRF_TRIE_INIT_COMPLETED(u, vrf, ipv6)) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,
                                "soc_alpm_insert:VRF %d is not "
                                "initialized\n"), vrf));
        rv = soc_alpm_vrf_add(u, vrf, ipv6);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                  "soc_alpm_insert:VRF %d/%d trie init \n"
                                  "failed\n"), vrf, ipv6));
            return rv;
        }
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,
                                "soc_alpm_insert:VRF %d/%d trie init "
                                "completed\n"), vrf, ipv6));
    }

    /* Find and update the entry */
    if (pivot_info & SOC_ALPM_LOOKUP_HIT) {
        rv = _soc_alpm_find_and_update(u, lpm_entry, bufp, bufp2, mem,
                                       bkt_idx);
    } else {
        /* entry is not found in database */
        /* Insert the entry into the database, if FULL, split the bucket */
        /* Insert prefix into trie */
        /* Split trie : Insertion into trie results into Split */
        /* Allocate a TCAM entry for PIVOT and bucket and move entries */
        if (bkt_idx == -1) {
            /* internally 0 is invalid bucket index */
            bkt_idx = 0;
        }
        rv = _soc_alpm_insert(u, ipv6, lpm_entry, mem, bufp, bufp2, &index,
                              SOC_ALPM_BKT_ENTRY_TO_IDX(bkt_idx), pivot_info);
        if (SOC_SUCCESS(rv)) {
            VRF_TRIE_ROUTES_INC(u, vrf_id, vrf, ipv6);
        }
    }

    if (rv == SOC_E_FULL) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,
                               "unit %d :soc_alpm_insert: "
                               "Route Insertion Failed :%s\n"),
                     u, soc_errmsg(rv)));
    } else if (rv != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_ALPM,
                 (BSL_META_U(u,
                             "unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),
                  u, soc_errmsg(rv)));
    }
    return(rv);
}


/*
 * soc_alpm_lookup
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
soc_alpm_lookup(int u,
                void *key_data,
                void *e,         /* return entry data if found */
                int *index_ptr,
                int *cookie)
{
    defip_alpm_ipv4_1_entry_t alpmv4_entry; /* Search Result buffer */
    defip_alpm_ipv6_64_1_entry_t alpmv6_entry; /* Search Result buffer */
    soc_mem_t mem;
    int       bucket_index;
    int       tcam_index = -1;
    void      *bufp;
    int       vrf_id, vrf;
    int       ipv6, pfx;
    int       rv = SOC_E_NONE;
    int       ipmc_route = 0;
    int       flex_v6_64 = 0;
    int       flex = 0;

    ipv6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0f);

    /* If the entry is override, then insert into TCAM directly */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));

    /* First launch lookup directly into tcam. If hit comes from override
     * vrf just return it. Else launch lookup into alpm pivots
     */
    /* For VRF_OVERRIDE (Global High) entries, software needs to maintain the
     * index */
    /* Prefix resides in TCAM directly */

    /* search in the hash table */
    *index_ptr = 0;
    rv = _soc_alpm_lpm_match(u, key_data, e, index_ptr, &pfx, &ipv6);
    /*
     * If entry is ipv4 copy to the "zero" half of the,
     * buffer, "zero" half of e if the  original entry
     * is in the "one" half.
     */
    flex = VRF_FLEX_IS_SET(u, vrf_id, vrf, ipv6, 0);
    if (SOC_SUCCESS(rv)) {
        if (!ipv6 && (*index_ptr & 0x1)) {
            rv = soc_alpm_lpm_ip4entry1_to_0(u, e, e, PRESERVE_HIT);
        }
        if (soc_feature(u, soc_feature_ipmc_defip)) {
            ipmc_route = soc_mem_field32_get(u, L3_DEFIPm, e, MULTICAST_ROUTE0f);
        }

        if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && ipv6 && flex) {
            if (soc_mem_field32_get(u, L3_DEFIPm, key_data, IP_ADDR_MASK0f) == 0xFFFFFFFF &&
                soc_mem_field32_get(u, L3_DEFIPm, key_data, IP_ADDR_MASK1f) == 0xFFFFFFFF) {
                flex_v6_64 = 1;
            }
        }
    }
    if ((vrf_id == SOC_L3_VRF_OVERRIDE) || (ipmc_route) || (flex_v6_64)) {
        *cookie = 0;
        if (SOC_SUCCESS(rv)) {
            *cookie |= SOC_ALPM_LPM_LOOKUP_HIT;
        }
        return rv;
    }

    /* either no hit in OVERRIDE or hit in pivot tcam */
    /* non override entries, go to into ALPM memory */
    if (!VRF_TRIE_INIT_COMPLETED(u, vrf, ipv6)) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,
                                "soc_alpm_lookup:VRF %d is not initialized\n"),
                     vrf));
        *cookie = 0;
        return SOC_E_NOT_FOUND;
    }
    /* Get table memory. */
    SOC_ALPM_TABLE_MEM(u, ipv6, mem, flex);

    /* Assign entry buf based on table being used */
    SOC_ALPM_ENTRY_BUF(ipv6, mem, bufp, alpmv4_entry, alpmv6_entry);

    SOC_ALPM_LPM_LOCK(u);
    /* Find and update the entry */
    rv = _soc_alpm_find(u, key_data, mem, bufp, &tcam_index, &bucket_index,
                        index_ptr, SOC_ALPM_SW_LOOKUP(u));
    SOC_ALPM_LPM_UNLOCK(u);

    if (SOC_FAILURE(rv)) {
        *cookie = tcam_index;
        *index_ptr = bucket_index << 2;
        return rv;
    }
    /* Copy ALPM memory to LPM entry */
    rv =  _soc_alpm_lpm_ent_init(u, bufp, mem, ipv6, vrf_id,
                                 bucket_index, *index_ptr, e, flex);
    *cookie = SOC_ALPM_LOOKUP_HIT | tcam_index;
    if (flex) {
        *cookie |= SOC_ALPM_COOKIE_FLEX;
    }
    return(rv);
}

/* Find a LPM Global low or VRF route in ALPM memory */
static int
_soc_alpm_find_best_match(int u,
                          void *key_data,
                          void *e,          /* return entry data if found */
                          int vrf_id,
                          int *tcam_index,
                          int *bucket_index,
                          int *key_index,
                          int do_urpf)
{
    int rv = SOC_E_NONE;
    int i, j, v6, hit = 0;
    uint32 db_type, ent_type, bank_disable;
    defip_aux_scratch_entry_t aux_entry;
    int global, mvrf, vrf;
    int index;
    soc_mem_t mem, lpm_mem;
    int hw_len, sw_len;
    int ent_num;

    /* Used for keeping Longest Matched entry in Bucket */
    uint32  alpm_bufp[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int     alpm_len = -1;
    int     alpm_index = 0;
    int     flex;

    soc_field_t ip_addr_fld[2] = {
        IP_ADDR0f,
        IP_ADDR1f,
    };

    lpm_mem = L3_DEFIPm;
    v6 = soc_mem_field32_get(u, lpm_mem, key_data, MODE0f);
    global = soc_mem_field32_get(u, lpm_mem, key_data, GLOBAL_ROUTE0f);
    mvrf = soc_mem_field32_get(u, lpm_mem, key_data, VRF_ID_0f);
    vrf = (vrf_id == SOC_L3_VRF_GLOBAL) ? (SOC_VRF_MAX(u) + 1) : mvrf;

    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u,
                "Prefare AUX Scratch for searching TCAM in "
                "%s region, Key data: v6 %d global %d vrf %d:\n"),
                vrf_id == SOC_L3_VRF_GLOBAL ? "Global" : "VRF",
                v6, global, mvrf));
    soc_alpm_db_ent_type_encoding(u, vrf, &db_type, &ent_type);
    if (do_urpf) {
        db_type ++;
    }
    if (vrf_id == SOC_L3_VRF_GLOBAL) {
        /* Crack these value to use _fill_aux routine */
        soc_mem_field32_set(u, lpm_mem, key_data, GLOBAL_ROUTE0f, 1);
        soc_mem_field32_set(u, lpm_mem, key_data, VRF_ID_0f, 0);
    }

    bank_disable = soc_alpm_bank_dis(u, vrf);

    SOC_IF_ERROR_RETURN(
        _soc_alpm_fill_aux_entry_for_op(u, key_data, v6, db_type, ent_type,
                                        0, &aux_entry));
    /* Recover to original value */
    if (vrf_id == SOC_L3_VRF_GLOBAL) {
        soc_mem_field32_set(u, lpm_mem, key_data, GLOBAL_ROUTE0f, global);
        soc_mem_field32_set(u, lpm_mem, key_data, VRF_ID_0f, mvrf);
    }

    /* Start shadow table engine to search */
    SOC_IF_ERROR_RETURN(
        _soc_alpm_aux_op(u, PREFIX_LOOKUP, &aux_entry, TRUE, &hit, tcam_index,
                         bucket_index, 0));

    if (hit == 0) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,"Could not find bucket\n")));
        return SOC_E_NOT_FOUND;
    }

    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u, "Hit in memory %s, index %d, "
                "bucket_index %d\n"), SOC_MEM_NAME(u, lpm_mem),
                soc_alpm_logical_idx(u, lpm_mem, (*tcam_index) >> 1, 1),
                *bucket_index));

    /* Found, check ALPM bucket */
    flex = VRF_FLEX_IS_SET(u, vrf_id, vrf, v6, 0);
    SOC_ALPM_TABLE_MEM(u, v6, mem, flex);
    rv = _soc_alpm_mask_len_get(u, v6, key_data, &sw_len);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (v6) {
        if (flex) {
            ent_num = ALPM_IPV6_64_BKT_COUNT_FLEX;
        } else {
            ent_num = ALPM_IPV6_64_BKT_COUNT;
        }
    } else {
        if (flex) {
            ent_num = ALPM_IPV4_BKT_COUNT_FLEX;
        } else {
            ent_num = ALPM_IPV4_BKT_COUNT;
        }
    }
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        ent_num <<= 1;
    }

    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u, "Start searching mem %s bucket %d(count %d) "
                "for Length %d\n"),
                SOC_MEM_NAME(u, mem), *bucket_index, ent_num, sw_len));

    for (i = 0; i < ent_num; i++) {
        uint32 bufp[SOC_MAX_MEM_FIELD_WORDS] = {0};
        uint32 mask[2] = {0};
        uint32 hkey[2] = {0};
        uint32 skey[2] = {0};
        int valid;

        rv = _soc_alpm_mem_index(u, mem, *bucket_index, i, bank_disable, &index);
        if (rv == SOC_E_FULL) {
            continue;
        }

        SOC_IF_ERROR_RETURN(_soc_mem_alpm_read(u, mem, MEM_BLOCK_ANY,
                                               index, (void *)bufp));
        valid = soc_mem_field32_get(u, mem, bufp, VALIDf);
        hw_len = soc_mem_field32_get(u, mem, bufp, LENGTHf);

        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u, "Bucket %5d Index %6d: Valid %d, Length %d\n"),
                    *bucket_index, index, valid, hw_len));

        if (!valid || (hw_len > sw_len)) {
            continue;
        }

        /* Length in ALPM <= given length */
        SHR_BITSET_RANGE(mask, (v6 ? 64 : 32) - hw_len, hw_len);
        (void)soc_mem_field_get(u, mem, (uint32 *)bufp, KEYf, (uint32 *)hkey);
        skey[1] = soc_mem_field32_get(u, lpm_mem, key_data, ip_addr_fld[1]);
        skey[0] = soc_mem_field32_get(u, lpm_mem, key_data, ip_addr_fld[0]);

        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u, "\tmask %08x %08x\n\t key %08x %08x\n"
                    "\thost %08x %08x\n"),
                    mask[1], mask[0], hkey[1], hkey[0], skey[1], skey[0]));

        for (j = v6 ? 1 : 0; j >= 0; j--) {
            if ((skey[j] & mask[j]) != (hkey[j] & mask[j])) {
                break;
            }
        }

        /* Hardware key doesn't match to software key */
        if (j >= 0) {
            continue;
        }

        /* Matched, store and keep searching,
         * could be Longer prefix in following entries */
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u, "Found a match in mem %s bucket %d, "
                    "index %d\n"), SOC_MEM_NAME(u, mem), *bucket_index, index));

        /* Keep longest matched route */
        if (alpm_len == -1 || alpm_len < hw_len) {
            alpm_len = hw_len;
            alpm_index = index;
            sal_memcpy(alpm_bufp, bufp, sizeof(bufp));
        }
    }

    /* Found */
    if (alpm_len != -1) {
        rv = _soc_alpm_lpm_ent_init(u, alpm_bufp, mem, v6, vrf_id, *bucket_index,
                                    alpm_index, e, flex);
        if (SOC_SUCCESS(rv)) {
            *key_index = alpm_index;
            if (bsl_check(bslLayerSoc, bslSourceAlpm, bslSeverityVerbose, u)) {
                LOG_VERBOSE(BSL_LS_SOC_ALPM,
                            (BSL_META_U(u, "Hit mem %s bucket %d, index %d\n"),
                            SOC_MEM_NAME(u, mem), *bucket_index, alpm_index));
            }
        }
        return rv;
    }

    /* Missed */
    *key_index = soc_alpm_logical_idx(u, lpm_mem, (*tcam_index) >> 1, 1);
    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u,"Miss in mem %s bucket %d, use associate data "
                "in mem %s LOG index %d\n"), SOC_MEM_NAME(u, mem), *bucket_index,
                SOC_MEM_NAME(u, lpm_mem), *key_index));
    
    /* Miss in bucket, use associate data */
    SOC_IF_ERROR_RETURN(_soc_mem_alpm_read(u, lpm_mem, MEM_BLOCK_ANY,
                                           *key_index, (void *)e));
    if ((!v6) && ((*tcam_index) & 1)) {
        /* move entry to 0 half */
        rv = soc_alpm_lpm_ip4entry1_to_0(u, e, e, PRESERVE_HIT);
    }

    return SOC_E_NONE;
}

/*
 * soc_alpm_find_best_match
 *
 * It's an indenpendent routine, to simalute HW process for incoming packets.
 * Considering possible mismatch between SW and HW, we need to go through
 * hardware for lookup regardless of software state.
 */
int
soc_alpm_find_best_match(int u,
                         void *key_data,
                         void *e,           /* return entry data if found */
                         int *index_ptr,
                         int do_urpf)
{
    int rv = SOC_E_NONE;
    int i, index_min, index_cnt;
    defip_entry_t lpm_ent;
    uint32 mem_mask[2];
    uint32 mem_addr[2];
    uint32 mask[2];
    uint32 addr[2];
    uint32 glb_hi, glb_rt;

    int vrf_id, vrf = 0;
    int vld[2] = {0};
    int tcam_index, bucket_index;
    soc_mem_t lpm_mem = L3_DEFIPm;
    int ent, v6, hw_v6, match = 0;
    int flex_v6_64;

    soc_field_t glb_hi_fld[] = {
        GLOBAL_HIGH0f,
        GLOBAL_HIGH1f
    };
    soc_field_t glb_rt_fld[] = {
        GLOBAL_ROUTE0f,
        GLOBAL_ROUTE1f
    };

    v6 = soc_mem_field32_get(u, lpm_mem, key_data, MODE0f);
    if (!SOC_URPF_STATUS_GET(u) && do_urpf) {
        return SOC_E_PARAM;
    }

    /* Since L3_DEFIP_* tables are not support for DMA,
     * we do PIO search instead */
    index_min = soc_mem_index_min(u, lpm_mem);
    index_cnt = soc_mem_index_count(u, lpm_mem);
    if (SOC_URPF_STATUS_GET(u)) {
        index_cnt >>= 1;
    }
    /* in Parallel mode search from second half */
    if (soc_alpm_mode_get(u)) {
        index_cnt >>= 1;
        index_min += index_cnt;
    }

    if (do_urpf) {
        index_min += soc_mem_index_count(u, lpm_mem) / 2;
    }

    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u, "Launch LPM searching from index %d count %d\n"),
                index_min, index_cnt));

    mask[0] = soc_mem_field32_get(u, lpm_mem, key_data, IP_ADDR_MASK0f);
    mask[1] = soc_mem_field32_get(u, lpm_mem, key_data, IP_ADDR_MASK1f);
    addr[0] = soc_mem_field32_get(u, lpm_mem, key_data, IP_ADDR0f);
    addr[1] = soc_mem_field32_get(u, lpm_mem, key_data, IP_ADDR1f);

    /* Scanning for Global High routes */
    for (i = index_min; i < index_min + index_cnt; i++) {
        SOC_IF_ERROR_RETURN(_soc_mem_alpm_read(u, lpm_mem, MEM_BLOCK_ANY,
                            i, (void *)&lpm_ent));
        vld[0] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, VALID0f);
        vld[1] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, VALID1f);
        if (vld[0] == 0 && vld[1] == 0) {
            continue;
        }

        hw_v6 = soc_mem_field32_get(u, lpm_mem, &lpm_ent, MODE0f);
        if (hw_v6 != v6) {
            continue;
        }

        for (ent = 0; ent < (v6 ? 1 : 2); ent++) {
            if (vld[ent] == 0) {
                continue;
            }
            glb_hi = soc_mem_field32_get(u, lpm_mem, &lpm_ent, glb_hi_fld[ent]);
            glb_rt = soc_mem_field32_get(u, lpm_mem, &lpm_ent, glb_rt_fld[ent]);
            flex_v6_64 = 0;
            if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && v6 &&
                soc_mem_field32_get(u, lpm_mem, &lpm_ent, ENTRY_VIEW0f)) {
                if (soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR_MASK0f) == 0xFFFFFFFF &&
                    soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR_MASK1f) == 0xFFFFFFFF) {
                    flex_v6_64 = 1;
                }
            }

            /* No need to compare key if not Global High route */
            if (!(glb_hi && glb_rt) && !flex_v6_64) {
                continue;
            }

            LOG_VERBOSE(BSL_LS_SOC_ALPM,
                        (BSL_META_U(u, "Match a Global High route: ent %d\n"),
                        ent));

            mem_mask[0] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR_MASK0f);
            mem_mask[1] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR_MASK1f);
            mem_addr[0] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR0f);
            mem_addr[1] = soc_mem_field32_get(u, lpm_mem, &lpm_ent, IP_ADDR1f);

            LOG_VERBOSE(BSL_LS_SOC_ALPM,
                        (BSL_META_U(u, "\thmsk %08x %08x\n\thkey %08x %08x\n"
                        "\tsmsk %08x %08x\n\tskey %08x %08x\n"),
                        mem_mask[1], mem_mask[0], mem_addr[1], mem_addr[0],
                        mask[1], mask[0], addr[1], addr[0]));

            /* match condition: mem mask equal to or shorten than match mask
             *                  mem masked addrs are equal.
             */
            if (v6) {
                match = (mem_mask[1] <= mask[1] && mem_mask[0] <= mask[0] &&
                        ((addr[0] & mem_mask[0]) == (mem_addr[0] & mem_mask[0])) &&
                        ((addr[1] & mem_mask[1]) == (mem_addr[1] & mem_mask[1])));
            } else {
                match = (mem_mask[ent] <= mask[0] &&
                         ((addr[0] & mem_mask[ent]) == (mem_addr[ent] & mem_mask[ent])));
            }
            if (match) {
                /* Found, all Key matched */
                LOG_VERBOSE(BSL_LS_SOC_ALPM,
                            (BSL_META_U(u,"Hit Global High route in index = %d(%d)\n"),
                            i, ent));
                sal_memcpy(e, &lpm_ent, sizeof(lpm_ent));
                if (!v6 && ent == 1) {
                    /* ent == 1 indicate it's sencond half of the entry */
                    rv = soc_alpm_lpm_ip4entry1_to_0(u, e, e, PRESERVE_HIT);
                }
                *index_ptr = i;
                return rv;
            }
        }
    }

    /* Global High lookup miss, use AUX to search Global Low and VRF route */
    LOG_VERBOSE(BSL_LS_SOC_ALPM,
                (BSL_META_U(u,"Global high lookup miss, use AUX engine to "
                "search for VRF and Global Low routes\n")));

    /* Fill in AUX Scratch and perform PREFIX Operation */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));
    rv = _soc_alpm_find_best_match(u, key_data, e, vrf_id, &tcam_index,
                                   &bucket_index, index_ptr, do_urpf);
    if (rv == SOC_E_NOT_FOUND) {
        vrf = SOC_L3_VRF_GLOBAL;
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,"Not found in VRF region, trying Global "
                    "region\n")));
        rv = _soc_alpm_find_best_match(u, key_data, e, vrf_id, &tcam_index,
                                       &bucket_index, index_ptr, do_urpf);
    }

    if (SOC_SUCCESS(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,"Hit in %s region in TCAM index %d, "
                    "buckekt_index %d\n"),
                    vrf == SOC_L3_VRF_GLOBAL ? "Global Low" : "VRF",
                    tcam_index, bucket_index));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,"Search miss for given address\n")));
    }

    return(rv);
}


/*
 * soc_alpm_delete
 */
int
soc_alpm_delete(int u, void *key_data, int bkt_idx, int pivot_info)
{
    int       vrf_id, vrf;
    int       ipv6;
    int       rv = SOC_E_NONE;
    int       propagate = 0;
    int       ipmc_route = 0;
    int       flex_v6_64 = 0;

    ipv6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0f);

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipmc_route = soc_mem_field32_get(u, L3_DEFIPm, key_data, MULTICAST_ROUTE0f);
    }
    /* If the entry is override, then Lookup into TCAM directly */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));

    if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && ipv6 &&
        VRF_FLEX_IS_SET(u, vrf_id, vrf, ipv6, 0)) {
        if (soc_mem_field32_get(u, L3_DEFIPm, key_data, IP_ADDR_MASK0f) == 0xFFFFFFFF &&
            soc_mem_field32_get(u, L3_DEFIPm, key_data, IP_ADDR_MASK1f) == 0xFFFFFFFF) {
            flex_v6_64 = 1;
        }
    }

    /* For VRF_OVERRIDE (Global High) entries, and IPMC route
     * software needs to maintain the index
     */
    /* Prefix resides in TCAM directly */
    if ((vrf_id == SOC_L3_VRF_OVERRIDE) || (ipmc_route) || (flex_v6_64)) {
        /* TCAM Direct lookup */
        /* search in the hash table */
        rv = soc_alpm_lpm_delete(u, key_data);
        if (SOC_SUCCESS(rv)) {
            VRF_TRIE_ROUTES_DEC(u, vrf_id, vrf, ipv6);
        }
        return(rv);
    } else { /* non override entries, go to into ALPM memory */
        int tcam_index;
        if (!VRF_TRIE_INIT_COMPLETED(u, vrf, ipv6)) {
            LOG_VERBOSE(BSL_LS_SOC_ALPM,
                        (BSL_META_U(u,
                                    "soc_alpm_delete:VRF %d/%d is not initialized\n"),
                         vrf, ipv6));
            return SOC_E_NONE;
        }
        if (bkt_idx == -1) {
            /* internally 0 is invalid bucket index */
            bkt_idx = 0;
        }
        /* deleting all skip propagation to reduce overall time */
        propagate = !(pivot_info & SOC_ALPM_DELETE_ALL);
        tcam_index = pivot_info & SOC_ALPM_TCAM_INDEX_MASK;
        rv = _soc_alpm_delete(u, key_data, SOC_ALPM_BKT_ENTRY_TO_IDX(bkt_idx),
                              tcam_index, bkt_idx,
                              propagate);
    }
    return(rv);
}


/* Initialize shared bucket management */
static int
soc_alpm_shared_mem_init(int u)
{
    int defip_table_size;

    /* Maximm number of IPv4 pivots */
    SOC_ALPM_BUCKET_COUNT(u) = SOC_TD2_ALPM_MAX_BKTS;

    if (SOC_URPF_STATUS_GET(u)) {
        /* remaining half buckets are the src buckets */
        SOC_ALPM_BUCKET_COUNT(u) >>= 1;
    }

    /* number bytes to manage the max buckets */
    SOC_ALPM_BUCKET_BMAP_SIZE(u) = SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(u));

    SOC_ALPM_BUCKET_BMAP(u) =
             sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(u),
                       "alpm_shared_bucket_bitmap");
    if (SOC_ALPM_BUCKET_BMAP(u) == NULL) {
        LOG_WARN(BSL_LS_SOC_ALPM,
                 (BSL_META_U(u,
                             "soc_alpm_shared_mem_init: Memory allocation for "
                             "bucket bitmap management failed\n")));
        return SOC_E_MEMORY;
    }

    /* Mark all buckets as free */
    sal_memset(SOC_ALPM_BUCKET_BMAP(u), 0, SOC_ALPM_BUCKET_BMAP_SIZE(u));

    /* Bucket 0 is reserved. Just assign it */
    alpm_bucket_assign(u, 1, &defip_table_size);
    return SOC_E_NONE;
}

STATIC int
_soc_alpm_lpm_free_entries(int u, soc_mem_t mem, int vrf)
{
    int curr_pfx, min_pfx;
    int total_fent = 0;

    if (mem == L3_DEFIP_ALPM_IPV6_128m) {
        return _soc_alpm_128_lpm_free_entries(u, vrf);
    }

    curr_pfx = ALPM_MAX_PFX_INDEX;
    min_pfx  = 0;
    if (soc_alpm_mode_get(u) == SOC_ALPM_MODE_PARALLEL) {
        if (vrf == (SOC_VRF_MAX(u) + 1)) { /* Global VRF */
            min_pfx  = ALPM_MAX_VRF_PFX_INDEX + 1;
        } else {
            curr_pfx = ALPM_MAX_VRF_PFX_INDEX;
        }
    }
    while (curr_pfx >= min_pfx) {
        total_fent += SOC_ALPM_LPM_STATE_FENT(u, curr_pfx);
        curr_pfx = SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx);
    }
    return total_fent;
}


/* Connect TCAM entry to a bucket ID */
STATIC int
_soc_alpm_bucket_link(int u, int v6, int pivot_index, int bucket_index)
{
    int             rv = SOC_E_NONE;
    int             lpm_index;
    soc_mem_t       lpm_mem = L3_DEFIPm;
    defip_entry_t   lpm_entry;

    /* Connect TCAM entry to new bucket */
    lpm_index = soc_alpm_logical_idx(u, lpm_mem, pivot_index >> 1, 1);

    rv = _soc_mem_alpm_read(u, lpm_mem, MEM_BLOCK_ANY, lpm_index, &lpm_entry);
    SOC_IF_ERROR_RETURN(rv);
    if ((!v6) && (pivot_index & 1)) {
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, &lpm_entry, ALG_BKT_PTR1f, bucket_index);
    } else {
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, &lpm_entry, ALG_BKT_PTR0f, bucket_index);
    }
    rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, lpm_index, &lpm_entry);
    SOC_IF_ERROR_RETURN(rv);

    if (SOC_URPF_STATUS_GET(u)) {
        int _urpf_index = lpm_index + (soc_mem_index_count(u, lpm_mem) >> 1);
        rv = _soc_mem_alpm_read(u, lpm_mem, MEM_BLOCK_ANY,
                                _urpf_index, &lpm_entry);
        SOC_IF_ERROR_RETURN(rv);

        if ((!v6) && (pivot_index & 1)) {
            soc_mem_field32_set(u, lpm_mem, &lpm_entry, ALG_BKT_PTR1f,
                                bucket_index + SOC_ALPM_BUCKET_COUNT(u));
        } else {
            soc_mem_field32_set(u, lpm_mem, &lpm_entry, ALG_BKT_PTR0f,
                                bucket_index + SOC_ALPM_BUCKET_COUNT(u));
        }

        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, _urpf_index, &lpm_entry);
        SOC_IF_ERROR_RETURN(rv);
    }

    return rv;
}


/* Merge entries from one logical bucket to another */
static int
_soc_alpm_bucket_merge(int u, soc_mem_t mem, int pivot_idx_from, int pivot_idx_to,
                       soc_alpm_merge_type_t merge_dir, int *shuffled)
{
    int                     *new_index = NULL;
    int                     rv = SOC_E_NONE;
    int                     bucket_index_from, bucket_index_to;
    int                     v6, i;
    int                     vrf = 0;
    alpm_mem_prefix_array_t *pfx_array = NULL;
    defip_entry_t           lpm_entry;
    alpm_pivot_t            *from_pivot, *to_pivot;
    alpm_pivot_t            *parent_pivot = NULL, *child_pivot = NULL;
    uint32                  bank_disable;
    enum {
        _MERGE_NONE,
        _MERGE_PREPARE,
        _MERGE_COPY,
        _MERGE_LPM_LINK,
        _MERGE_INVALIDATE,
        _MERGE_CHILD_TRIE_MERGED,
        _MERGE_CHILD_LPM_DELETED,
        _MERGE_CHILD_PIVOT_DESTROYED
    } doing = _MERGE_NONE, done = _MERGE_NONE;
    void                    *raw_bkt[RAW_BKT_NUM] = {0};
    v6 = (L3_DEFIP_ALPM_IPV6_64m == mem ||
          L3_DEFIP_ALPM_IPV6_64_1m == mem ||
          L3_DEFIP_ALPM_IPV6_128m == mem);

    from_pivot = ALPM_TCAM_PIVOT(u, pivot_idx_from);
    to_pivot   = ALPM_TCAM_PIVOT(u, pivot_idx_to);

    bucket_index_from = PIVOT_BUCKET_INDEX(from_pivot);
    bucket_index_to   = PIVOT_BUCKET_INDEX(to_pivot);

    vrf = PIVOT_BUCKET_VRF(from_pivot);
    bank_disable = soc_alpm_bank_dis(u, vrf);

    /**
     * Invalidate need to happen only after all the entries are
     * written to the new pivot and the new pivot is valid. Otherwise
     * there is a chance of MISS.
     *
     * hitless move:
     * - Copy entries to new bucket.
     * - activate new pivot
     * - delete entries from old bucket
     */

    doing = _MERGE_PREPARE;
    rv = _soc_alpm_raw_bucket_prepare(u, mem, PIVOT_BUCKET_TRIE(from_pivot),
                                      bucket_index_from, bucket_index_to,
                                      bank_disable, NULL, NULL,
                                      &pfx_array, raw_bkt, &new_index, NULL, 1);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _MERGE_COPY;
    rv = _soc_alpm_raw_bucket_copy(u, mem, bucket_index_to,
                                   bank_disable, raw_bkt[RAW_NEW_BKT_DIP],
                                   raw_bkt[RAW_NEW_BKT_SIP],
                                   new_index, pfx_array->count);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    /* Connect TCAM entry to new bucket.
     * This is a must step even for merge child to parent
     */
    doing = _MERGE_LPM_LINK;
    rv = _soc_alpm_bucket_link(u, v6, pivot_idx_from, bucket_index_to);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    /* Invalide old entries */
    doing = _MERGE_INVALIDATE;
    rv = _soc_alpm_raw_bucket_inval(u, mem, bucket_index_from,
                                    bank_disable,
                                    raw_bkt[RAW_OLD_BKT_DIP],
                                    raw_bkt[RAW_OLD_BKT_SIP],
                                    raw_bkt[RAW_RB_BKT_DIP],
                                    raw_bkt[RAW_RB_BKT_SIP]);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    if (merge_dir != MERGE_PHY) {
        uint32          hw_key[5] = {0};
        int             child_vrf;

        if (merge_dir == MERGE_LOG_PARENT_TO_CHILD) {
            parent_pivot = ALPM_TCAM_PIVOT(u, pivot_idx_from);
            child_pivot  = ALPM_TCAM_PIVOT(u, pivot_idx_to);
        } else if (merge_dir == MERGE_LOG_CHILD_TO_PARENT) {
            child_pivot  = ALPM_TCAM_PIVOT(u, pivot_idx_from);
            parent_pivot = ALPM_TCAM_PIVOT(u, pivot_idx_to);
        }

        /* SW bucket merge: not expect to fail */
        doing = _MERGE_CHILD_TRIE_MERGED;
        rv = trie_merge(PIVOT_BUCKET_TRIE(parent_pivot),
                        PIVOT_BUCKET_TRIE(child_pivot)->trie,
                        child_pivot->key,
                        child_pivot->len);
        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

        sal_memcpy(hw_key, child_pivot->key, sizeof(hw_key));
        ALPM_TRIE_TO_NORMAL_IP(hw_key, child_pivot->len, v6);

        child_vrf = PIVOT_BUCKET_VRF(child_pivot);

        (void) _soc_alpm_lpm_ent_key_init(u, hw_key, child_pivot->len,
                                          child_vrf, v6, &lpm_entry, 1);

        /* Delete child lpm entry from TCAM */
        doing = _MERGE_CHILD_LPM_DELETED;
        rv = soc_alpm_lpm_delete(u, &lpm_entry);
        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);
        if (shuffled) {
            *shuffled = 1;
        }

        /* Destroy child pivot from trie */
        doing = _MERGE_CHILD_PIVOT_DESTROYED;
        rv = _soc_alpm_split_pivot_create_undo
                    (u, v6, child_vrf, child_pivot->key, child_pivot->len, NULL, 0);

        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);
    }

    /* Update TCAM mgmt info: set new logical bucket */
    if (merge_dir != MERGE_LOG_CHILD_TO_PARENT && ALPM_TCAM_PIVOT(u, pivot_idx_from)) {
        PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(u, pivot_idx_from)) = bucket_index_to;
    }

    alpm_bucket_release(u, bucket_index_from, v6);

    /* Update new index */
    for (i = 0; i < pfx_array->count; i++) {
        pfx_array->prefix[i]->index = new_index[i];
    }

    _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);
    return rv;

_rollback:

    /* fall through on all cases */
    switch (done)  {
    case _MERGE_CHILD_LPM_DELETED:
        if (merge_dir != MERGE_PHY) {
            /* not expecting any failure after lpm deleted */
        }

    case _MERGE_CHILD_TRIE_MERGED:
        if (merge_dir != MERGE_PHY) {
            uint32      pivot[5], length;
            trie_split2(PIVOT_BUCKET_TRIE(parent_pivot),
                        child_pivot->key,
                        child_pivot->len,
                        pivot, &length,
                        &(PIVOT_BUCKET_TRIE(child_pivot)->trie),
                        PIVOT_BUCKET_COUNT(child_pivot),
                        1);
        }

    case _MERGE_INVALIDATE:
        _soc_alpm_raw_bucket_inval_undo(u, mem, bucket_index_from,
                                        bank_disable,
                                        raw_bkt[RAW_RB_BKT_DIP],
                                        raw_bkt[RAW_RB_BKT_SIP]);

    case _MERGE_LPM_LINK:
        /* Connect TCAM back to original bucket */
        _soc_alpm_bucket_link(u, v6, pivot_idx_from, bucket_index_from);

    case _MERGE_COPY:
        /* Delete entries in new bucket */
        _soc_alpm_raw_bucket_copy_undo(u, mem, bucket_index_to, bank_disable,
                                       raw_bkt[RAW_NEW_BKT_DIP],
                                       raw_bkt[RAW_NEW_BKT_SIP], new_index,
                                       pfx_array->count);

    case _MERGE_PREPARE:
        _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);

    case _MERGE_NONE:
    default:
        break;
    }

    /* Error dump */
    switch (doing)  {
    case _MERGE_LPM_LINK:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_soc_alpm_bucket_link failed, pivot %d log_bkt %d\n"),
                              pivot_idx_from, bucket_index_to));
        break;

    case _MERGE_INVALIDATE:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "_soc_alpm_bucket_inval failed\n")));
        break;
    case _MERGE_CHILD_TRIE_MERGED:
        break;

    case _MERGE_CHILD_PIVOT_DESTROYED:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Pivot destroy fail, something not expected\n")));
    default:
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Merge fails at step %d\n"), doing));
        break;
    }

    return rv;
}


/* Move partial entries from one logical bucket to another */
static int
_soc_alpm_bucket_repartition(int u, soc_mem_t mem, int pivot_idx_from,
                             int pivot_idx_to, trie_node_t **new_trie,
                             int *shuffled)
{
    trie_node_t             *split_trie_root = NULL;
    uint32                  length, bpm_len = 0;
    uint32                  pivot[5];
    defip_entry_t           lpm_entry;
    alpm_mem_prefix_array_t *pfx_array = NULL;
    int                     *new_index = NULL;
    int                     rv = SOC_E_NONE, i;
    int                     tcam_index, pivot_index;
    int                     bucket_index_from, bucket_index_to;
    int                     vrf = 0;
    uint32                  src_discard, src_default;
    void                    *raw_bkt[RAW_BKT_NUM] = {0};
    alpm_pivot_t            *new_pivot, *parent_pivot, *child_pivot;
    uint32                  bank_disable = 0;
    int         v6 = (L3_DEFIP_ALPM_IPV6_64m == mem ||
                      L3_DEFIP_ALPM_IPV6_64_1m == mem ||
                      L3_DEFIP_ALPM_IPV6_128m == mem);
    enum {
        _REPART_TRIE_NONE,
        _REPART_TRIE_SPLIT,
        _REPART_LPM_INIT,
        _REPART_PIVOT_CREATE,
        _REPART_PREPARE,
        _REPART_COPY,
        _REPART_LPM_INSERT,
        _REPART_INVALIDATE,
        _REPART_CHILD_TRIE_MERGED,
        _REPART_CHILD_LPM_DELETED,
        _REPART_CHILD_PIVOT_DESTROYED
    } doing = _REPART_TRIE_NONE, done = _REPART_TRIE_NONE;

    parent_pivot = ALPM_TCAM_PIVOT(u, pivot_idx_from);
    child_pivot  = ALPM_TCAM_PIVOT(u, pivot_idx_to);
    bucket_index_from = PIVOT_BUCKET_INDEX(parent_pivot);
    bucket_index_to   = PIVOT_BUCKET_INDEX(child_pivot);

    doing = _REPART_TRIE_SPLIT;
    rv = trie_split2(PIVOT_BUCKET_TRIE(parent_pivot),
                     child_pivot->key,
                     child_pivot->len,
                     pivot, &length,
                     &split_trie_root,
                     1024, 0);
    if (SOC_FAILURE(rv)) {
        /* Non repartable is nothing fatal, just return and keep searching */
        if (rv == SOC_E_NOT_FOUND) {
            rv = SOC_E_NONE;
        }
        /* Must return */
        return rv;
    }
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    vrf = PIVOT_BUCKET_VRF(parent_pivot);
    doing = _REPART_LPM_INIT;

    rv = _soc_alpm_split_lpm_init(u, mem, vrf, bucket_index_to, NULL, NULL,
                                  pivot, length, &lpm_entry, &bpm_len,
                                  &src_discard, &src_default);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);


    doing = _REPART_PIVOT_CREATE;
    rv = _soc_alpm_split_pivot_create(u, v6, vrf, bucket_index_to,
                                      split_trie_root, pivot, length,
                                      bpm_len, &new_pivot);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    bank_disable = soc_alpm_bank_dis(u, vrf);

    doing = _REPART_PREPARE;
    rv = _soc_alpm_raw_bucket_prepare(u, mem, PIVOT_BUCKET_TRIE(new_pivot),
                                      bucket_index_from, bucket_index_to,
                                      bank_disable, NULL, NULL,
                                      &pfx_array, raw_bkt, &new_index, NULL, 1);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _REPART_COPY;
    rv = _soc_alpm_raw_bucket_copy(u, mem, bucket_index_to,
                                   bank_disable, raw_bkt[RAW_NEW_BKT_DIP],
                                   raw_bkt[RAW_NEW_BKT_SIP],
                                   new_index, pfx_array->count);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    doing = _REPART_LPM_INSERT;
    rv = soc_alpm_lpm_insert(u, &lpm_entry, &tcam_index,
                             src_default, src_discard, bpm_len);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);
    if (shuffled) {
        *shuffled = 1;
    }

    /* TCAM index from lpm code is logical.  Convert to global idx */
    tcam_index = soc_alpm_physical_idx(u, L3_DEFIPm, tcam_index, v6);
    pivot_index = tcam_index << (v6 ? 1 : 0);
    PIVOT_TCAM_INDEX(new_pivot)         = pivot_index;
    ALPM_TCAM_PIVOT(u, pivot_index)     = new_pivot;

    /* Invalide old entries */
    doing = _REPART_INVALIDATE;
    rv = _soc_alpm_raw_bucket_inval(u, mem, bucket_index_from,
                                    bank_disable,
                                    raw_bkt[RAW_OLD_BKT_DIP],
                                    raw_bkt[RAW_OLD_BKT_SIP],
                                    raw_bkt[RAW_RB_BKT_DIP],
                                    raw_bkt[RAW_RB_BKT_SIP]);
    _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

    /* Delete old lpm entry */
    {
        uint32          hw_key[5] = {0};
        int             child_vrf;

        /* SW bucket merge: not expect to fail */
        doing = _REPART_CHILD_TRIE_MERGED;
        rv = trie_merge(PIVOT_BUCKET_TRIE(new_pivot),
                        PIVOT_BUCKET_TRIE(child_pivot)->trie,
                        child_pivot->key,
                        child_pivot->len);
        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

        sal_memcpy(hw_key, child_pivot->key, sizeof(hw_key));
        ALPM_TRIE_TO_NORMAL_IP(hw_key, child_pivot->len, v6);

        child_vrf = PIVOT_BUCKET_VRF(child_pivot);

        (void) _soc_alpm_lpm_ent_key_init(u, hw_key, child_pivot->len,
                                      child_vrf, v6, &lpm_entry, 1);

        doing = _REPART_CHILD_LPM_DELETED;
        rv = soc_alpm_lpm_delete(u, &lpm_entry);
        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);

        pivot_index = PIVOT_TCAM_INDEX(new_pivot);

        /* Destroy child pivot */
        doing = _REPART_CHILD_PIVOT_DESTROYED;
        rv = _soc_alpm_split_pivot_create_undo
                    (u, v6, child_vrf, child_pivot->key, child_pivot->len, NULL, 0);

        _ALPM_IF_ERROR_GOTO(rv, _rollback, done, doing);
    }


    /* Update new index */
    for (i = 0; i < pfx_array->count; i++) {
        pfx_array->prefix[i]->index = new_index[i];
    }

    _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);
    *new_trie = (trie_node_t *) new_pivot;

/*    LOG_CLI((" Do repartition for two buckets OK\n"));*/

    return rv;

_rollback:

    /* fall through on all cases */
    switch (done)  {
    case _REPART_CHILD_LPM_DELETED:
        /* Not expect any failure after trie_merge */

    case _REPART_CHILD_TRIE_MERGED: {
            uint32      pivot2[5], length2;
            trie_split2(PIVOT_BUCKET_TRIE(parent_pivot),
                        child_pivot->key,
                        child_pivot->len,
                        pivot2, &length2,
                        &(PIVOT_BUCKET_TRIE(child_pivot)->trie),
                        PIVOT_BUCKET_COUNT(child_pivot), 1);
        }

    case _REPART_INVALIDATE:
        _soc_alpm_raw_bucket_inval_undo(u, mem, bucket_index_from,
                                        bank_disable,
                                        raw_bkt[RAW_RB_BKT_DIP],
                                        raw_bkt[RAW_RB_BKT_SIP]);

    case _REPART_LPM_INSERT:
        soc_alpm_lpm_delete(u, &lpm_entry);

    case _REPART_COPY:
        /* Delete entries in new bucket */
        _soc_alpm_raw_bucket_copy_undo(u, mem, bucket_index_to, bank_disable,
                                       raw_bkt[RAW_NEW_BKT_DIP],
                                       raw_bkt[RAW_NEW_BKT_SIP], new_index,
                                       pfx_array->count);

    case _REPART_PREPARE:
        _soc_alpm_raw_bucket_prepare_undo(u, &pfx_array, raw_bkt, &new_index);

    case _REPART_PIVOT_CREATE:
        _soc_alpm_split_pivot_create_undo(u, v6, vrf, pivot, length, NULL, 0);

    case _REPART_LPM_INIT:
        /* nothing to undo */

    case _REPART_TRIE_SPLIT:
        trie_merge(PIVOT_BUCKET_TRIE(parent_pivot), split_trie_root, pivot, length);
    default:
        break;
    }

    LOG_ERROR(BSL_LS_SOC_ALPM,
              (BSL_META_U(u,
                          "Repartition fails at step %d\n"), doing));

    return rv;
}


static int
_soc_alpm_repartition_cb(trie_node_t *ptrie, trie_node_t *trie,
                         trie_traverse_states_e_t *state, void *info,
                         trie_node_t **new_trie)
{
    int             rv;
    int             max_count;
    int             pivot_idx_from, pivot_idx_to, pivot_idx_parent, pivot_idx_child;
    int             bucket_index_from;
    int             do_repartition = 0; /* 0: merge. 1: repartition */
    int             child_count = 0, parent_count = 0;
    soc_alpm_merge_info_t *repart = (soc_alpm_merge_info_t *)info;
    alpm_pivot_t *parent_pivot  = (alpm_pivot_t *)ptrie;
    alpm_pivot_t *child_pivot   = (alpm_pivot_t *)trie;
    soc_alpm_merge_type_t merge_dir = MERGE_PHY;

#ifdef ALPM_REPART_DEBUG
{
    trie_t          *pivot_trie;
    trie_node_t     *lpm = NULL;
    switch (repart->v6) {
        case L3_DEFIP_MODE_V4:
            pivot_trie = VRF_PIVOT_TRIE_IPV4(repart->u, repart->vrf);
            break;
        case L3_DEFIP_MODE_64:
            pivot_trie = VRF_PIVOT_TRIE_IPV6(repart->u, repart->vrf);
            break;
        default:
            pivot_trie = VRF_PIVOT_TRIE_IPV6_128(repart->u, repart->vrf);
            break;
    }
    trie_find_lpm2(pivot_trie, child_pivot->key, child_pivot->len, &lpm);
    ptrie = lpm;
    parent_pivot  = (alpm_pivot_t *)ptrie;
}
#endif

    if (!parent_pivot) { /* no parent, this is a root */
        return SOC_E_NONE;
    }

    /* Reset state everytime */
    *state = TRIE_TRAVERSE_STATE_NONE;

    max_count = repart->max_count;

    if (PIVOT_BUCKET_TRIE(parent_pivot)->trie == NULL) {
        merge_dir    = MERGE_LOG_PARENT_TO_CHILD;
    }

    if (PIVOT_BUCKET_TRIE(child_pivot)->trie == NULL) {
        merge_dir   = MERGE_LOG_CHILD_TO_PARENT;
    }

    if (merge_dir == MERGE_PHY) {
        parent_count = PIVOT_BUCKET_COUNT(parent_pivot);
        child_count = PIVOT_BUCKET_COUNT(child_pivot);
        if (parent_count + child_count < max_count) {
            if (parent_count > child_count) {
                merge_dir   = MERGE_LOG_CHILD_TO_PARENT;
            } else {
                merge_dir   = MERGE_LOG_PARENT_TO_CHILD;
            }
        } else {
            /* Merge is not qualified, try repartition */
            merge_dir = MERGE_LOG_PARENT_TO_CHILD;
            do_repartition = 1;
        }
    }

    pivot_idx_parent = PIVOT_TCAM_INDEX(parent_pivot);
    pivot_idx_child  = PIVOT_TCAM_INDEX(child_pivot);

    if (merge_dir == MERGE_LOG_CHILD_TO_PARENT) {
        pivot_idx_from = pivot_idx_child;
        pivot_idx_to   = pivot_idx_parent;
        bucket_index_from = PIVOT_BUCKET_INDEX(child_pivot);
    } else {
        /* repartition always go here */
        pivot_idx_from = pivot_idx_parent;
        pivot_idx_to   = pivot_idx_child;
        bucket_index_from = PIVOT_BUCKET_INDEX(parent_pivot);
    }

    if (do_repartition == 0) { /* Bucket merge */
        if (repart->mem == L3_DEFIP_ALPM_IPV6_128m) {
            rv = _soc_alpm_128_bucket_merge(repart->u, repart->mem,
                                            pivot_idx_from, pivot_idx_to, merge_dir,
                                            &repart->shuffled);
        } else {
            rv = _soc_alpm_bucket_merge(repart->u, repart->mem,
                                        pivot_idx_from, pivot_idx_to, merge_dir,
                                        &repart->shuffled);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(repart->u,
                                  "Bucket merge fail %d\n"), rv));
            return rv;
        }

        repart->log_bkt = bucket_index_from;
        *state = TRIE_TRAVERSE_STATE_DELETED;
        if (++repart->merge_count >= REPART_THRESHOLD) {
            *state = TRIE_TRAVERSE_STATE_DONE;
        }
        soc_alpm_dbg_cnt[repart->u].bkt_merge2++;
    } else { /* Repartition */
        /* Repartition is an intermediate stage, not a goal.
         * Also note, repartition may result in trie modified
         * (old node deleted, new node added).
         */
        if (repart->mem == L3_DEFIP_ALPM_IPV6_128m) {
            rv = _soc_alpm_128_bucket_repartition(repart->u, repart->mem,
                                                  pivot_idx_from, pivot_idx_to,
                                                  new_trie, &repart->shuffled);
        } else {
            rv = _soc_alpm_bucket_repartition(repart->u, repart->mem,
                                              pivot_idx_from, pivot_idx_to,
                                              new_trie, &repart->shuffled);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(repart->u,
                                  "Bucket repartition fail %d\n"), rv));
        }
        /* keep searching if not repartable */
        if (repart->shuffled) {
            soc_alpm_dbg_cnt[repart->u].bkt_repart++;
        }
    }

    return rv;
}


static int
_soc_alpm_repartition(int u, soc_mem_t mem, int vrf, int *bucket_index, int *shuffled)
{
    int                             rv = SOC_E_NONE;
    soc_alpm_merge_info_t           info;
    trie_t                          *pivot_trie = NULL;

    sal_memset(&info, 0, sizeof(info));
    info.log_bkt     = -1; /* Initialize to non-eligible bucket_index */
    info.u           = u;
    info.mem         = mem;
    info.orig_vrf    = vrf;
    info.v6          = SOC_ALPM_MEM_V6(mem);
    info.vrf         = vrf;
    info.max_count   = _soc_alpm_bkt_entry_cnt(u, mem);

    pivot_trie =
        (info.v6 == L3_DEFIP_MODE_V4 ? VRF_PIVOT_TRIE_IPV4(u, info.vrf) :
         info.v6 == L3_DEFIP_MODE_64 ? VRF_PIVOT_TRIE_IPV6(u, info.vrf) :
                                   VRF_PIVOT_TRIE_IPV6_128(u, info.vrf));
    if (pivot_trie == NULL || pivot_trie->trie == NULL) {
        return SOC_E_FULL;
    }
    /* Choose post-order traverse for repartition.
     * All orders are delete safe.
     * But only the post-order is modify safe.
     */
    rv = trie_repartition(pivot_trie, _soc_alpm_repartition_cb,
                          &info, _TRIE_POSTORDER_TRAVERSE);
    if (info.shuffled && shuffled) {
        *shuffled = 1;
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u, "Logic bucket repartition fails."\
                                 "rv %d vrf %d v6 %d\n"), rv, info.vrf, info.v6));
        return rv;
    } else if (info.log_bkt != -1) {
        /* This indicates one and only one merge happened.
         * Note repartition don't set bucket_index.
         */
        *bucket_index = info.log_bkt;
        return SOC_E_NONE;
    }
    return SOC_E_FULL;
}

static int
_soc_alpm_merge_cb(trie_node_t *ptrie, trie_node_t *trie,
                   trie_traverse_states_e_t *state, void *info)
{
    int             rv;
    int             max_count;
    int             pivot_idx_from, pivot_idx_to, pivot_idx_parent, pivot_idx_child;
    int             bucket_index_from;
    int             child_count = 0, parent_count = 0;
    soc_alpm_merge_info_t *merge = (soc_alpm_merge_info_t *)info;
    alpm_pivot_t *parent_pivot = (alpm_pivot_t *)ptrie;
    alpm_pivot_t *child_pivot = (alpm_pivot_t *)trie;
    soc_alpm_merge_type_t merge_dir = MERGE_PHY;

    if (!parent_pivot) {
        return SOC_E_NONE;
    }

    /* Reset state everytime */
    *state = TRIE_TRAVERSE_STATE_NONE;

    max_count = merge->max_count;

    if (PIVOT_BUCKET_TRIE(parent_pivot)->trie == NULL) {
        merge_dir    = MERGE_LOG_PARENT_TO_CHILD;
    }

    if (PIVOT_BUCKET_TRIE(child_pivot)->trie == NULL) {
        merge_dir   = MERGE_LOG_CHILD_TO_PARENT;
    }

    if (merge_dir == MERGE_PHY) {
        parent_count = PIVOT_BUCKET_COUNT(parent_pivot);
        child_count = PIVOT_BUCKET_COUNT(child_pivot);
        if (parent_count + child_count < max_count) {
            if (parent_count > child_count) {
                merge_dir   = MERGE_LOG_CHILD_TO_PARENT;
            } else {
                merge_dir   = MERGE_LOG_PARENT_TO_CHILD;
            }
        } else {
            return SOC_E_NONE;
        }
    }

    pivot_idx_parent = PIVOT_TCAM_INDEX(parent_pivot);
    pivot_idx_child  = PIVOT_TCAM_INDEX(child_pivot);

    if (merge_dir == MERGE_LOG_CHILD_TO_PARENT) {
        pivot_idx_from = pivot_idx_child;
        pivot_idx_to   = pivot_idx_parent;
        bucket_index_from = PIVOT_BUCKET_INDEX(child_pivot);
    } else {
        pivot_idx_from = pivot_idx_parent;
        pivot_idx_to   = pivot_idx_child;
        bucket_index_from = PIVOT_BUCKET_INDEX(parent_pivot);
    }

    /* Do logical bucket merge */
    if (merge->mem == L3_DEFIP_ALPM_IPV6_128m) {
        rv = _soc_alpm_128_bucket_merge(merge->u, merge->mem,
                                        pivot_idx_from, pivot_idx_to, merge_dir,
                                        &merge->shuffled);
    } else {
        rv = _soc_alpm_bucket_merge(merge->u, merge->mem,
                                    pivot_idx_from, pivot_idx_to, merge_dir,
                                    &merge->shuffled);
    }

    if (SOC_FAILURE(rv)) { /* traverse stops on callback failure */
        LOG_CLI(("Merge fails %d (not expecting this )\n", rv));
        return rv;
    }

    merge->log_bkt = bucket_index_from;
    *state = TRIE_TRAVERSE_STATE_DELETED;
    if (++merge->merge_count >= MERGE_THRESHOLD) {
        *state = TRIE_TRAVERSE_STATE_DONE;
    }
    soc_alpm_dbg_cnt[merge->u].bkt_merge++;

    return SOC_E_NONE;
}


static int
_soc_alpm_merge(int u, soc_mem_t mem, int vrf, int *bucket_index, int *shuffled)
{
    int                             rv = SOC_E_NONE;
    soc_alpm_merge_info_t           info;
    trie_t                          *pivot_trie = NULL;

    sal_memset(&info, 0, sizeof(info));
    info.log_bkt     = -1; /* Initialize to non-eligible bucket_index */
    info.u           = u;
    info.mem         = mem;
    info.orig_vrf    = vrf;
    info.v6          = SOC_ALPM_MEM_V6(mem);
    info.vrf         = vrf;
    info.max_count   = _soc_alpm_bkt_entry_cnt(u, mem);

    pivot_trie =
        (info.v6 == L3_DEFIP_MODE_V4 ? VRF_PIVOT_TRIE_IPV4(u, info.vrf) :
         info.v6 == L3_DEFIP_MODE_64 ? VRF_PIVOT_TRIE_IPV6(u, info.vrf) :
                                   VRF_PIVOT_TRIE_IPV6_128(u, info.vrf));
    if (pivot_trie == NULL || pivot_trie->trie == NULL) {
        return SOC_E_FULL;
    }
    rv = trie_traverse2(pivot_trie, _soc_alpm_merge_cb,
                        &info, _TRIE_POSTORDER_TRAVERSE);
    if (info.shuffled && shuffled) {
        *shuffled = 1;
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u, "Logic bucket merge fails."\
                                 "rv %d vrf %d v6 %d\n"), rv, info.vrf, info.v6));
        return rv;
    } else if (info.log_bkt != -1) {
        /* This indicates one and only one merge happened. */
        *bucket_index = info.log_bkt;
        return SOC_E_NONE;
    }
    return SOC_E_FULL;
}



static int
_soc_alpm_assign(int u, soc_mem_t mem, int vrf, int *bucket_index, int *o_shuffled)
{
    int                 rv = SOC_E_FULL;
    int                 shuffled = 0;
    int                 lpm_free_count = 0;

    /* Logical bucket merge on condition. */
    rv = _soc_alpm_merge(u, mem, vrf, bucket_index, &shuffled);

    if (rv == SOC_E_FULL) {
        /* Logical bucket repartition on condition. */
        lpm_free_count = _soc_alpm_lpm_free_entries(u, mem, vrf);
        if (lpm_free_count >= 1 && lpm_free_count <= 5) {
            rv = _soc_alpm_repartition(u, mem, vrf, bucket_index, &shuffled);
        }
    }

    if (o_shuffled) {
        *o_shuffled = shuffled;
    }
    return rv;
}

/* Get the free bucket to allocate to the PIVOT */
int
alpm_bucket_assign(int u, int v6, int *bucket_pointer)
{
    int i, step_count = 1, used = 0;

    /*
     * in combined search mode, with urpf disabled, need to allocate even and 
     * odd bucket pointers
     */

    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        step_count = 2;
    }
    for (i = 0; i < SOC_ALPM_BUCKET_COUNT(u); i += step_count) {
        SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(u), i, step_count, used);
        if (0 == used) {
            break;
        }
    }
    if (i == SOC_ALPM_BUCKET_COUNT(u)) {
        return SOC_E_FULL;
    }
    SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(u), i, step_count);
    *bucket_pointer = i;
    SOC_ALPM_BUCKET_NEXT_FREE(u) = i;
    return SOC_E_NONE;
}

int
soc_alpm_assign(int u, int vrf, soc_mem_t mem, int *bucket_pointer, int *shuffled)
{
    int rv;
    int step_count = 1;
    int v6 = SOC_ALPM_MEM_V6(mem);

    rv = alpm_bucket_assign(u, v6, bucket_pointer);

    if (SOC_SUCCESS(rv)) {
        if (_soc_alpm_lpm_free_entries(u, mem, vrf) == 0) {
            rv = _soc_alpm_merge(u, mem, vrf, &step_count, shuffled);
            if (rv == SOC_E_FULL && !v6) {
                rv = SOC_E_NONE;
            } else if (SOC_FAILURE(rv)) {
                (void) alpm_bucket_release(u, *bucket_pointer, v6);
            }
        }
    } else if (rv == SOC_E_FULL) {
        rv =  _soc_alpm_assign(u, mem, vrf, bucket_pointer, shuffled);
        /* Reclaim the physical bucket in case it has been freed */
        if (SOC_SUCCESS(rv)) {
            if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
                step_count = 2;
            }
            SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(u), *bucket_pointer, step_count);
            SOC_ALPM_BUCKET_NEXT_FREE(u) = *bucket_pointer;
        }
    }
    return rv;
}


/* Release the bucket to the pool*/
int
alpm_bucket_release(int u, int bucket_pointer, int v6)
{
    int step_count = 1, used = 0;

    /* Check if the bucket pointer is valid */
    if ((bucket_pointer < 1) ||
                (bucket_pointer > SOC_ALPM_BUCKET_MAX_INDEX(u))) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "Unit %d\n, freeing invalid bucket index %d\n"),
                   u, bucket_pointer));
        return SOC_E_PARAM;
    }
    /* in combined search mode, with urpf disabled, need to free even and odd
     * bucket pointers
     */
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        step_count = 2;
    }
    SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(u), bucket_pointer, step_count, 
                      used);
    if (!used) {
        return SOC_E_PARAM;
    }
    SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(u), bucket_pointer, step_count);
    _soc_trident2_alpm_bkt_view_set(u, bucket_pointer << 2, INVALIDm);
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        _soc_trident2_alpm_bkt_view_set(u, (bucket_pointer + 1) << 2,
                                        INVALIDm);
    }

    if (SOC_URPF_STATUS_GET(u)) {
        _soc_trident2_alpm_bkt_view_set(u,
                        _soc_alpm_rpf_entry(u, bucket_pointer << 2), INVALIDm);
        if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
            _soc_trident2_alpm_bkt_view_set(u,
                        _soc_alpm_rpf_entry(u, (bucket_pointer + 1) << 2), INVALIDm);
        }
    }
    return SOC_E_NONE;
}

int
alpm_bucket_is_assigned(int u, int bucket_ptr, int ipv6, int *used)
{
    int step_count = 1;

    /* Check if the bucket pointer is valid */
    if ((bucket_ptr < 1) || (bucket_ptr > SOC_ALPM_BUCKET_MAX_INDEX(u))) {
        return SOC_E_PARAM;
    }
    
    /* in combined search mode, with urpf disabled, need to allocate even and 
     * odd bucket pointers */
    if (ipv6) {
        if (!soc_alpm_mode_get(u) && !SOC_URPF_STATUS_GET(u)) {
            step_count = 2;
        }
    }
    
    SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(u), bucket_ptr, step_count, *used);
    return SOC_E_NONE;
}

/**********************************************
* TCAM Management functions *
 */

/*
 * TCAM based PIVOT implementation. Each table entry can hold two IPV4 PIVOTs or
 * one IPV6 PIVOT entry. VRF independent routes placed at the beginning or 
 * at the end of table based on application provided entry vrf id 
 * (SOC_L3_VRF_OVERRIDE/SOC_L3_VRF_GLOBAL).   
 *
 *              ALPM_MAX_PFX_INDEX
 * lpm_prefix_index[98].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    0                      ==
 * lpm_prefix_index[98].end   ---> ===============================
 *
 * lpm_prefix_index[97].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 64  ==
 * lpm_prefix_index[97].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[x].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==                           ==
 * lpm_prefix_index[x].end   --->  ===============================
 *
 *
 *              ALPM_IPV6_PFX_ZERO
 * lpm_prefix_index[33].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 0   ==
 * lpm_prefix_index[33].end   ---> ===============================
 *
 *
 * lpm_prefix_index[32].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 * lpm_prefix_index[32].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[0].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 * lpm_prefix_index[0].end   --->  ===============================
 */


/*
 *      Extract key data from an entry at the given index.
 */
static
void _soc_alpm_lpm_hash_entry_get(int u, void *e,
                                int index, _soc_alpm_lpm_hash_entry_t r_entry,
                                int *v)
{
    if (index & TD2_ALPM_HASH_IPV6_MASK) {
        SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, e, r_entry);
        if (v) {
            *v =  SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f) &&
                  SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);
        }
    } else {
        if (index & 0x1) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, e, r_entry);
            if (v) {
                *v =  SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);
            }
        } else {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, e, r_entry);
            if (v) {
                *v =  SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
            }
        }
    }
}

/*
 * Function:
 *      _soc_alpm_lpm_hash_compare_key
 * Purpose:
 *      Comparison function for AVL shadow table operations.
 */
static
int _soc_alpm_lpm_hash_compare_key(_soc_alpm_lpm_hash_entry_t key1,
                                 _soc_alpm_lpm_hash_entry_t key2)
{
    int idx;

    for (idx = 0; idx < 5; idx++) { 
        SOC_MEM_COMPARE_RETURN(key1[idx], key2[idx]);
    }
    return (0);
}

#ifdef FB_LPM_DEBUG      
#define H_INDEX_MATCH(str, tab_index, match_index)      \
    LOG_ERROR(BSL_LS_SOC_ALPM, \
              (BSL_META("%s index: H %d A %d\n"),              \
               str, (int)tab_index, match_index))
#else
#define H_INDEX_MATCH(str, tab_index, match_index)
#endif

#define LPM_NO_MATCH_INDEX 0x4000
#define LPM_HASH_INSERT(u, entry_data, tab_index, rvt_index0, rvt_index1)     \
    soc_alpm_lpm_hash_insert(u, entry_data, tab_index, LPM_NO_MATCH_INDEX, 0, \
                             rvt_index0, rvt_index1)

#define LPM_HASH_REVERT(u, entry_data, tab_index, rvt_index0, rvt_index1)     \
    soc_alpm_lpm_hash_revert(u, entry_data, tab_index, \
                             rvt_index0, rvt_index1)

#define LPM_HASH_DELETE(u, key_data, tab_index)         \
    soc_alpm_lpm_hash_delete(u, key_data, tab_index)

#define LPM_HASH_LOOKUP(u, key_data, pfx, tab_index)    \
    soc_alpm_lpm_hash_lookup(u, key_data, pfx, tab_index)

#define LPM_HASH_VERIFY(u, key_data, tab_index)    \
    soc_alpm_lpm_hash_verify(u, key_data, tab_index)

#define INDEX_DELETE(hash, hash_idx, del_idx)                   \
    hash->table[hash_idx] =                                     \
        hash->link_table[del_idx & TD2_ALPM_HASH_INDEX_MASK];     \
    hash->link_table[del_idx & TD2_ALPM_HASH_INDEX_MASK] =        \
        TD2_ALPM_HASH_INDEX_NULL

#define INDEX_DELETE_LINK(hash, prev_idx, del_idx)              \
    hash->link_table[prev_idx & TD2_ALPM_HASH_INDEX_MASK] =       \
        hash->link_table[del_idx & TD2_ALPM_HASH_INDEX_MASK];     \
    hash->link_table[del_idx & TD2_ALPM_HASH_INDEX_MASK] =        \
        TD2_ALPM_HASH_INDEX_NULL


static void soc_alpm_lpm_hash_insert(int u, void *entry_data, uint32 tab_index,
                                     uint32 old_index, int pfx,
                                     uint32 *rvt_index0, uint32 *rvt_index1)
{
    _soc_alpm_lpm_hash_entry_t    key_hash;

    if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, MODE0f)) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID1f) &&
            SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID0f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_insert(
                        SOC_ALPM_LPM_STATE_HASH(u),
                        _soc_alpm_lpm_hash_compare_key,
                        key_hash,
                        pfx,
                        old_index,
                        ((uint16)tab_index << 1) | TD2_ALPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID0f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_insert(SOC_ALPM_LPM_STATE_HASH(u),
                                    _soc_alpm_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    ((uint16)tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID1f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_insert(SOC_ALPM_LPM_STATE_HASH(u),
                                    _soc_alpm_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    (((uint16)tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}




static void soc_alpm_lpm_hash_revert(int u, void *entry_data, uint32 tab_index,
                                     uint32 rvt_index0, uint32 rvt_index1)
{
    _soc_alpm_lpm_hash_entry_t    key_hash;

    if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, MODE0f)) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID1f) &&
            SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID0f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_revert(
                        SOC_ALPM_LPM_STATE_HASH(u),
                        _soc_alpm_lpm_hash_compare_key,
                        key_hash,
                        ((uint16)tab_index << 1) | TD2_ALPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID0f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_revert(SOC_ALPM_LPM_STATE_HASH(u),
                                    _soc_alpm_lpm_hash_compare_key,
                                    key_hash,
                                    ((uint16)tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALID1f)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            _soc_alpm_lpm_hash_revert(SOC_ALPM_LPM_STATE_HASH(u),
                                    _soc_alpm_lpm_hash_compare_key,
                                    key_hash,
                                    (((uint16)tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}


static void soc_alpm_lpm_hash_delete(int u, void *key_data, uint32 tab_index)
{
    _soc_alpm_lpm_hash_entry_t    key_hash;
    int                         pfx = -1;
    int                         rv;
    uint16                      index;

    if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODE0f)) {
        SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
        index = (tab_index << 1) | TD2_ALPM_HASH_IPV6_MASK;
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
        index = tab_index;
    }

    rv = _soc_alpm_lpm_hash_delete(SOC_ALPM_LPM_STATE_HASH(u),
                                 _soc_alpm_lpm_hash_compare_key,
                                 key_hash, pfx, index);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "\ndel  index: H %d error %d\n"), index, rv));
    }
}

static int soc_alpm_lpm_hash_lookup(int u, void *key_data, int pfx, int *key_index)
{
    _soc_alpm_lpm_hash_entry_t    key_hash;
    int                         is_ipv6;
    int                         rv;
    uint16                      index = TD2_ALPM_HASH_INDEX_NULL;

    is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODE0f);
    if (is_ipv6) {
        SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
    }

    rv = _soc_alpm_lpm_hash_lookup(SOC_ALPM_LPM_STATE_HASH(u),
                                 _soc_alpm_lpm_hash_compare_key,
                                 key_hash, pfx, &index);
    if (SOC_FAILURE(rv)) {
        *key_index = 0xFFFFFFFF;
        return(rv);
    }

    *key_index = index;

    return(SOC_E_NONE);
}

static int soc_alpm_lpm_hash_verify(int u, void *key_data, uint32 tab_index)
{
    _soc_alpm_lpm_hash_entry_t    key_hash;
    int                         pfx = -1;
    int                         rv;
    uint16                      index;

    if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODE0f)) {
        SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
        index = (tab_index << 1) | TD2_ALPM_HASH_IPV6_MASK;
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
        index = tab_index;
    }

    rv = _soc_alpm_lpm_hash_verify(SOC_ALPM_LPM_STATE_HASH(u),
                                 _soc_alpm_lpm_hash_compare_key,
                                 key_hash, pfx, index);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "\nveri index: H %d error %d\n"), index, rv));
    }
    return rv;
}

/* 
 * Function:
 *      _soc_alpm_lpm_hash_compute
 * Purpose:
 *      Compute CRC hash for key data.
 * Parameters:
 *      data - Key data
 *      data_nbits - Number of data bits
 * Returns:
 *      Computed 16 bit hash
 */
static
uint16 _soc_alpm_lpm_hash_compute(uint8 *data, int data_nbits)
{
    return (_shr_crc16b(0, data, data_nbits));
}

/* 
 * Function:
 *      _soc_alpm_lpm_hash_create
 * Purpose:
 *      Create an empty hash table
 * Parameters:
 *      unit  - Device unit
 *      entry_count - Limit for number of entries in table
 *      index_count - Hash index max + 1. (index_count <= count)
 *      fb_lpm_hash_ptr - Return pointer (handle) to new Hash Table
 * Returns:
 *      SOC_E_NONE       Success
 *      SOC_E_MEMORY     Out of memory (system allocator)
 */

static
int _soc_alpm_lpm_hash_create(int unit,
                            int entry_count,
                            int index_count,
                            _soc_alpm_lpm_hash_t **fb_lpm_hash_ptr)
{
    _soc_alpm_lpm_hash_t  *hash;
    int                 index;

    if (index_count > entry_count) {
        return SOC_E_MEMORY;
    }
    hash = sal_alloc(sizeof (_soc_alpm_lpm_hash_t), "lpm_hash");
    if (hash == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(hash, 0, sizeof (*hash));

    hash->unit = unit;
    hash->entry_count = entry_count;
    hash->index_count = index_count;

    /*
     * Pre-allocate the hash table storage.
     */
    hash->table = sal_alloc(hash->index_count * sizeof(*(hash->table)),
                            "hash_table");

    if (hash->table == NULL) {
        sal_free(hash);
        return SOC_E_MEMORY;
    }
    /*
     * In case where all the entries should hash into the same bucket
     * this will prevent the hash table overflow
     */
    hash->link_table = sal_alloc(
                            hash->entry_count * sizeof(*(hash->link_table)),
                            "link_table");
    if (hash->link_table == NULL) {
        sal_free(hash->table);
        sal_free(hash);
        return SOC_E_MEMORY;
    }

    /*
     * Set the entries in the hash table to TD2_ALPM_HASH_INDEX_NULL
     * Link the entries beyond hash->index_max for handling collisions
     */
    for(index = 0; index < hash->index_count; index++) {
        hash->table[index] = TD2_ALPM_HASH_INDEX_NULL;
    }
    for(index = 0; index < hash->entry_count; index++) {
        hash->link_table[index] = TD2_ALPM_HASH_INDEX_NULL;
    }
    *fb_lpm_hash_ptr = hash;
    return SOC_E_NONE;
}

/* 
 * Function:
 *      _soc_alpm_lpm_hash_destroy
 * Purpose:
 *      Destroy the hash table
 * Parameters:
 *      fb_lpm_hash - Pointer (handle) to Hash Table
 * Returns:
 *      SOC_E_NONE       Success
 */
static
int _soc_alpm_lpm_hash_destroy(_soc_alpm_lpm_hash_t *fb_lpm_hash)
{
    if (fb_lpm_hash != NULL) {
        sal_free(fb_lpm_hash->table);
        sal_free(fb_lpm_hash->link_table);
        sal_free(fb_lpm_hash);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_alpm_lpm_hash_lookup
 * Purpose:
 *      Look up a key in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to lookup
 *      pfx     - Prefix length for lookup acceleration.
 *      key_index - (OUT)       Index where the key was found.
 * Returns:
 *      SOC_E_NONE      Key found
 *      SOC_E_NOT_FOUND Key not found
 */

static
int _soc_alpm_lpm_hash_lookup(_soc_alpm_lpm_hash_t          *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t    entry,
                            int                         pfx,
                            uint16                      *key_index)
{
    int u = hash->unit, v, i = 0;

    uint16 hash_val;
    uint16 index;

    hash_val = _soc_alpm_lpm_hash_compute((uint8 *)entry,
                                        (32 * 5)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("lhash", entry[0], hash_val);
    H_INDEX_MATCH("lkup ", entry[0], index);
    while(index != TD2_ALPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        uint32  e[SOC_MAX_MEM_FIELD_WORDS];
        _soc_alpm_lpm_hash_entry_t  r_entry;
        int     rindex;

        rindex = (index & TD2_ALPM_HASH_INDEX_MASK) >> 1;
        /*
         * Check prefix length and skip index if not valid for given length
        if ((SOC_ALPM_LPM_STATE_START(u, pfx) <= rindex) &&
            (SOC_ALPM_LPM_STATE_END(u, pfx) >= rindex)) {
         */
        SOC_IF_ERROR_RETURN(
            _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY, rindex, e));
        SOC_FB_LPM_HASH_ENTRY_GET(u, e, index, r_entry, &v);
        if (v == 0) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(u,
                                "Empty entry[%d] in link_table. hash_val %d ridx %d \n"),
                                index, hash_val, rindex));
        }
        if ((*key_cmp_fn)(entry, r_entry) == 0) {
            *key_index = (index & TD2_ALPM_HASH_INDEX_MASK) >> 
                            ((index & TD2_ALPM_HASH_IPV6_MASK) ? 1 : 0);
            H_INDEX_MATCH("found", entry[0], index);
            return(SOC_E_NONE);
        }
        /*
        }
        */
        index = hash->link_table[index & TD2_ALPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("lkup1", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    H_INDEX_MATCH("not_found", entry[0], index);
    return(SOC_E_NOT_FOUND);
}

/*
 * Function:
 *      _soc_alpm_lpm_hash_insert
 * Purpose:
 *      Insert/Update a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to lookup
 *      pfx     - Prefix length for lookup acceleration.
 *      old_index - Index where the key was moved from.
 *                  TD2_ALPM_HASH_INDEX_NULL if new entry.
 *      new_index - Index where the key was moved to.
 * Returns:
 *      SOC_E_NONE      Key found
 */
/*
 *      Should be caled before updating the LPM table so that the
 *      data in the hash table is consistent with the LPM table
 */
static
int _soc_alpm_lpm_hash_insert(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 old_index,
                            uint16 new_index,
                            uint32 *rvt_index)
{

#define INDEX_ADD(hash, hash_idx, new_idx)                      \
    hash->link_table[new_idx & TD2_ALPM_HASH_INDEX_MASK] =        \
        hash->table[hash_idx];                                  \
    hash->table[hash_idx] = new_idx

#define INDEX_ADD_LINK(hash, t_index, new_idx)                  \
    hash->link_table[new_idx & TD2_ALPM_HASH_INDEX_MASK] =        \
        hash->link_table[t_index & TD2_ALPM_HASH_INDEX_MASK];     \
    hash->link_table[t_index & TD2_ALPM_HASH_INDEX_MASK] = new_idx

#define INDEX_UPDATE(hash, hash_idx, old_idx, new_idx)          \
    hash->table[hash_idx] = new_idx;                            \
    hash->link_table[new_idx & TD2_ALPM_HASH_INDEX_MASK] =        \
        hash->link_table[old_idx & TD2_ALPM_HASH_INDEX_MASK];     \
    hash->link_table[old_idx & TD2_ALPM_HASH_INDEX_MASK] = TD2_ALPM_HASH_INDEX_NULL

#define INDEX_UPDATE_LINK(hash, prev_idx, old_idx, new_idx)             \
    hash->link_table[prev_idx & TD2_ALPM_HASH_INDEX_MASK] = new_idx;      \
    hash->link_table[new_idx & TD2_ALPM_HASH_INDEX_MASK] =                \
        hash->link_table[old_idx & TD2_ALPM_HASH_INDEX_MASK];             \
    hash->link_table[old_idx & TD2_ALPM_HASH_INDEX_MASK] = TD2_ALPM_HASH_INDEX_NULL


    int u = hash->unit, v, i = 0;

    uint16 hash_val;
    uint16 index;
    uint16 prev_index;

    if (rvt_index) {
        *rvt_index = TD2_ALPM_HASH_INDEX_NULL;
    }
    hash_val = _soc_alpm_lpm_hash_compute((uint8 *)entry,
                                        (32 * 5)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("ihash", entry[0], hash_val);
    H_INDEX_MATCH("ins  ", entry[0], new_index);
    H_INDEX_MATCH("ins1 ", index, new_index);
    prev_index = TD2_ALPM_HASH_INDEX_NULL;
    if (old_index != TD2_ALPM_HASH_INDEX_NULL) {
        while(index != TD2_ALPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
            uint32  e[SOC_MAX_MEM_FIELD_WORDS];
            _soc_alpm_lpm_hash_entry_t  r_entry;
            int     rindex;
            
            rindex = (index & TD2_ALPM_HASH_INDEX_MASK) >> 1;

            /*
             * Check prefix length and skip index if not valid for given length
            if ((SOC_ALPM_LPM_STATE_START(u, pfx) <= rindex) &&
                (SOC_ALPM_LPM_STATE_END(u, pfx) >= rindex)) {
             */
            SOC_IF_ERROR_RETURN(
                _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY, rindex, e));
            SOC_FB_LPM_HASH_ENTRY_GET(u, e, index, r_entry, &v);
            if (v == 0) {
                if ((index & TD2_ALPM_HASH_IPV6_MASK) ||
                    !((new_index & 1) && (new_index == (index + 1)))) {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                                    "Empty entry[%d] in link_table. hash_val %d ridx %d nidx %d\n"),
                                    index, hash_val, rindex, new_index));
                }
            }
            if (v && ((*key_cmp_fn)(entry, r_entry) == 0)) {
                /* assert(old_index == index);*/
                if (new_index != index) {
                    H_INDEX_MATCH("imove", prev_index, new_index);
                    if (prev_index == TD2_ALPM_HASH_INDEX_NULL) {
                        INDEX_UPDATE(hash, hash_val, index, new_index);
                    } else {
                        INDEX_UPDATE_LINK(hash, prev_index, index, new_index);
                    }
                }
                if (rvt_index) {
                    *rvt_index = index;
                }
                H_INDEX_MATCH("imtch", index, new_index);
                return(SOC_E_NONE);
            }
            /*
            }
            */
            prev_index = index;
            index = hash->link_table[index & TD2_ALPM_HASH_INDEX_MASK];
            H_INDEX_MATCH("ins2 ", index, new_index);
        }
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
        return SOC_E_INTERNAL;
    }
    INDEX_ADD(hash, hash_val, new_index);  /* new entry */
    return(SOC_E_NONE);
}

/*
 * Function:
 *      _soc_alpm_lpm_hash_revert
 * Purpose:
 *      Revert a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to delete
 *      pfx     - Prefix length for lookup acceleration.
 *      delete_index - Index to delete.
 * Returns:
 *      SOC_E_NONE      Success
 *      SOC_E_NOT_FOUND Key index not found.
 */
static
int _soc_alpm_lpm_hash_revert(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            uint16 new_index,
                            uint32 rvt_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint16 index;
    uint16 prev_index;

    hash_val = _soc_alpm_lpm_hash_compute((uint8 *)entry,
                                        (32 * 5)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("rhash", entry[0], hash_val);
    H_INDEX_MATCH("rvt  ", entry[0], index);
    prev_index = TD2_ALPM_HASH_INDEX_NULL;
    while(index != TD2_ALPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (new_index == index) {
            H_INDEX_MATCH("dfoun", entry[0], index);
            if (rvt_index == TD2_ALPM_HASH_INDEX_NULL) {
                if (prev_index == TD2_ALPM_HASH_INDEX_NULL) {
                    INDEX_DELETE(hash, hash_val, new_index);
                } else {
                    INDEX_DELETE_LINK(hash, prev_index, new_index);
                    }
            } else {
                if (prev_index == TD2_ALPM_HASH_INDEX_NULL) {
                    INDEX_UPDATE(hash, hash_val, new_index, rvt_index);
                } else {
                    INDEX_UPDATE_LINK(hash, prev_index, new_index, rvt_index);
                }
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & TD2_ALPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("rvt1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}



/*
 * Function:
 *      _soc_alpm_lpm_hash_delete
 * Purpose:
 *      Delete a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to delete
 *      pfx     - Prefix length for lookup acceleration.
 *      delete_index - Index to delete.
 * Returns:
 *      SOC_E_NONE      Success
 *      SOC_E_NOT_FOUND Key index not found.
 */
static
int _soc_alpm_lpm_hash_delete(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 delete_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint16 index;
    uint16 prev_index;

    hash_val = _soc_alpm_lpm_hash_compute((uint8 *)entry,
                                        (32 * 5)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("dhash", entry[0], hash_val);
    H_INDEX_MATCH("del  ", entry[0], index);
    prev_index = TD2_ALPM_HASH_INDEX_NULL;
    while(index != TD2_ALPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (delete_index == index) {
            H_INDEX_MATCH("dfoun", entry[0], index);
            if (prev_index == TD2_ALPM_HASH_INDEX_NULL) {
                INDEX_DELETE(hash, hash_val, delete_index);
            } else {
                INDEX_DELETE_LINK(hash, prev_index, delete_index);
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & TD2_ALPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("del1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}


/*
 * Function:
 *      _soc_alpm_lpm_hash_verify
 * Purpose:
 *      Verify a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to delete
 *      pfx     - Prefix length for lookup acceleration.
 *      verify_index   - Index to verify.
 * Returns:
 *      SOC_E_NONE      Success
 *      SOC_E_NOT_FOUND Key index not found.
 */
static
int _soc_alpm_lpm_hash_verify(_soc_alpm_lpm_hash_t *hash,
                            _soc_alpm_lpm_hash_compare_fn key_cmp_fn,
                            _soc_alpm_lpm_hash_entry_t entry,
                            int    pfx,
                            uint16 verify_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint16 index;

    hash_val = _soc_alpm_lpm_hash_compute((uint8 *)entry,
                                        (32 * 5)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("vhash", entry[0], hash_val);
    H_INDEX_MATCH("veri ", entry[0], index);
    while(index != TD2_ALPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (verify_index == index) {
            H_INDEX_MATCH("vfoun", entry[0], index);
            return(SOC_E_NONE);
        }
        index = hash->link_table[index & TD2_ALPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("veri1", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}
#else
#define LPM_HASH_INSERT(u, entry_data, tab_index, rvt_index0, rvt_index1)
#define LPM_HASH_REVERT(u, entry_data, tab_index, rvt_index0, rvt_index1)
#define LPM_HASH_DELETE(u, key_data, tab_index)
#define LPM_HASH_LOOKUP(u, key_data, pfx, tab_index)
#define LPM_HASH_VERIFY(u, key_data, tab_index)
#endif /* FB_LPM_HASH_SUPPORT */

int
_ipmask2pfx(uint32 ipv4m, int *mask_len)
{
    *mask_len = 0;
    while (ipv4m & (1 << 31)) {
        *mask_len += 1;
        ipv4m <<= 1;
    }

    return ((ipv4m) ? SOC_E_PARAM : SOC_E_NONE);
}

/* src and dst can be same */
int
soc_alpm_lpm_ip4entry0_to_0(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VALID0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VALID0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE0f, ipv4a);
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, MODE_MASK0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE0f, ipv4a);
    }

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK0f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT0f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT0f, ipv4a);

            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR0f, ipv4a);
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE0f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD0f, ipv4a);
    }
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID0f, ipv4a);
    }
    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, HIT0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, HIT0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_HIGH0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_HIGH0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_HIT_IDX0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_HIT_IDX0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_BKT_PTR0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_BKT_PTR0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DEFAULT_MISS0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULT_MISS0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_BASE_COUNTER_IDX0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_POOL_NUMBER0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_OFFSET_MODE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_VIEW0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_VIEW0f, ipv4a);

    return(SOC_E_NONE);
}

int
soc_alpm_lpm_ip4entry1_to_1(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VALID1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VALID1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE1f, ipv4a);
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, MODE_MASK1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE1f, ipv4a);
    }

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR1f, ipv4a);
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD1f, ipv4a);
    }
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID1f, ipv4a);
    }
    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, HIT1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, HIT1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_HIGH1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_HIGH1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_HIT_IDX1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_HIT_IDX1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_BKT_PTR1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_BKT_PTR1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DEFAULT_MISS1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULT_MISS1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_BASE_COUNTER_IDX1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_POOL_NUMBER1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_OFFSET_MODE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_VIEW1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_VIEW1f, ipv4a);

    return(SOC_E_NONE);
}

/* src and dst can be same */
int
soc_alpm_lpm_ip4entry0_to_1(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VALID0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VALID1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE1f, ipv4a);
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, MODE_MASK0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE_MASK1f, ipv4a);
    }
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE1f, ipv4a);
    }

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK1f, ipv4a);

    if (!SOC_IS_HURRICANE(u)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR1f, ipv4a);
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID1f, ipv4a);
    }
    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, HIT0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, HIT1f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_HIGH0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_HIGH1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_HIT_IDX0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_HIT_IDX1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_BKT_PTR0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_BKT_PTR1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DEFAULT_MISS0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULT_MISS1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_BASE_COUNTER_IDX0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_POOL_NUMBER0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_OFFSET_MODE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_VIEW0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_VIEW1f, ipv4a);

    return(SOC_E_NONE);
}

/* src and dst can be same */
int
soc_alpm_lpm_ip4entry1_to_0(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VALID1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VALID0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE0f, ipv4a);
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, MODE_MASK0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MODE_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MODE_MASK0f, ipv4a);
    }
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE0f, ipv4a);
    }

    if (soc_feature(u, soc_feature_ipmc_defip)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK0f, ipv4a);

    if (!SOC_IS_HURRICANE(u)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR0f, ipv4a);
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE0f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID0f, ipv4a);
    }
    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, HIT1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, HIT0f, ipv4a);
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_HIGH1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_HIGH0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_HIT_IDX1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_HIT_IDX0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ALG_BKT_PTR1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ALG_BKT_PTR0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DEFAULT_MISS1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULT_MISS0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_BASE_COUNTER_IDX1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_POOL_NUMBER1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, FLEX_CTR_OFFSET_MODE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_VIEW1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_VIEW0f, ipv4a);

    return(SOC_E_NONE);
}

void
soc_alpm_lpm_state_dump(int u)
{
    int i;
    int max_pfx_len;
    max_pfx_len = ALPM_MAX_PFX_INDEX;

#ifndef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    if (!bsl_check(bslLayerSoc, bslSourceAlpm, bslSeverityVerbose, u)) {
        return;
    }
#endif

    for(i = max_pfx_len; i >= 0 ; i--) {
        if ((i != ALPM_MAX_PFX_INDEX) && (SOC_ALPM_LPM_STATE_START(u, i) == -1)) {
            continue;
        }

#ifdef ALPM_WARM_BOOT_DEBUG
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                              "PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),
                   i,
                   SOC_ALPM_LPM_STATE_PREV(u, i),
                   SOC_ALPM_LPM_STATE_NEXT(u, i),
                   SOC_ALPM_LPM_STATE_START(u, i),
                   SOC_ALPM_LPM_STATE_END(u, i),
                   SOC_ALPM_LPM_STATE_VENT(u, i),
                   SOC_ALPM_LPM_STATE_FENT(u, i)));
#else
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(u,
                                "PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),
                     i,
                     SOC_ALPM_LPM_STATE_PREV(u, i),
                     SOC_ALPM_LPM_STATE_NEXT(u, i),
                     SOC_ALPM_LPM_STATE_START(u, i),
                     SOC_ALPM_LPM_STATE_END(u, i),
                     SOC_ALPM_LPM_STATE_VENT(u, i),
                     SOC_ALPM_LPM_STATE_FENT(u, i)));
#endif        
    }
}


/*
 * Function:
 *      _lpm_fb_urpf_entry_replicate
 * Purpose:
 *      Replicate entry to the second half of the tcam if URPF check is ON.
 * Parameters:
 *      u           - (IN)  SOC unit number.
 *      index:      - (IN)  corresponding dip target index
 *      e           - (IN|OUT) input dip entry, could be modified for sip
 *      src_index   - (IN)  if ALPM_INVALID_INDEX use input e,
 *                          otherwise copy from src_index.
 *      convert     - (IN)  Used if src_index != ALPM_INVALID_INDEX.
 *      src_default - (IN)  Used if src_index = ALPM_INVALID_INDEX.
 *                            0 - caculate using entry_data's mask (legacy way)
 *                            1 - force set (split case) 
 *      src_discard - (IN)  Used if src_index = ALPM_INVALID_INDEX.
 * Returns:
 *      BCM_E_XXX
 */
static 
int _lpm_fb_urpf_entry_replicate(int u, int src_index, int index, uint32 *e,
                                 int convert, int src_default, int src_discard,
                                 int bpm_len, int v4_half)
{
    int src_tcam_offset;  /* Defip memory size/2 urpf source lookup offset */
    int ipv6;             /* IPv6 entry.                                   */
    uint32 mask0;         /* Mask 0 field value.                           */
    uint32 mask1;         /* Mask 1 field value.                           */
    int def_gw_flag;      /* Entry is default gateway.                     */ 
    defip_entry_t urpf;

    if(!SOC_URPF_STATUS_GET(u)) {
        return (SOC_E_NONE);
    }

    src_tcam_offset = (soc_mem_index_count(u, L3_DEFIPm) >> 1);

    ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, MODE0f);
 
    if (src_index != ALPM_INVALID_INDEX) {
#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, L3_DEFIPm,
                                MEM_BLOCK_ANY,
                                src_index + src_tcam_offset));
#endif /* FB_LPM_TABLE_CACHED */

        SOC_IF_ERROR_RETURN(_soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY,
                            src_index + src_tcam_offset,
                            &urpf));
        switch (convert) {
        case CONVERT_NONE:
            sal_memcpy(e, &urpf, sizeof(defip_entry_t));
            break;
        case 0:
            mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, RPE0f);
            mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, DST_DISCARD0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, mask0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, mask1);
            break;
        case 1:
            mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, RPE0f);
            mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, DST_DISCARD0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, mask0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD1f, mask1);
            break;
        case 2:
            mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, RPE1f);
            mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, DST_DISCARD1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, mask0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, mask1);
            break;
        case 3:
            mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, RPE1f);
            mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, &urpf, DST_DISCARD1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, mask0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD1f, mask1);
            break;
        default:
            return SOC_E_INTERNAL;
        }

        /* Write entry to the second half of the tcam. */
        /* Better use index instead of (index + src_tcam_offset) for src_index, 
         * because the entry at index is just written, and we can simply form
         * aux based on it, it's simple. 
         * But if we use (index + src_tcam_offset), we have to do convert again.
         */
        return _soc_alpm_write_pivot_aux(u, index, index + src_tcam_offset, e,
                                         CONVERT_NONE, bpm_len, v4_half);
    }

    src_default = src_default ? 1 : 0;
    src_discard = src_discard ? 1 : 0;

    /* Set/Reset default gateway flag based on ip mask value. */
    mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK0f);
    mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK1f);

    if (!ipv6) {
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f)) {
            def_gw_flag = (!mask0) ? 1 : 0;
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, src_default | def_gw_flag);
            if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, src_discard);
            }
        }

        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f)) {
            def_gw_flag = (!mask1) ? 1 : 0;
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, src_default | def_gw_flag);
            if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD1f)) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD1f, src_discard);
            }
        }
    } else {
        def_gw_flag = ((!mask0) &&  (!mask1)) ? 1 : 0;
        mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
        mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);
        if (mask0 && mask1) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, src_default | def_gw_flag);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, src_default | def_gw_flag);
            if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, src_discard);
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD1f, src_discard);
            }
        }
    }
    /* Write entry to the second half of the tcam. */
    return _soc_alpm_write_pivot_aux(u, index, index + src_tcam_offset, e,
                                     CONVERT_NONE, bpm_len, v4_half);
}

static int
_soc_alpm_write_pivot_with_bpm(int u, int src_index, int index, void *entry,
                               uint32 op, int src_default_or_convert,
                               int src_discard, int bpm_len, int v4_half)
{

    int convert = CONVERT_NONE;
    int src_index_uprf = ALPM_INVALID_INDEX;
    int sdefault = 0, sdiscard = 0;
    uint32  rvt_index0 = 0, rvt_index1 = 0;
    int rv;

    switch (op & (~WRITE_PIVOT_WITHOUT_INSERT_HASH)) {
    case WRITE_PIVOT_WITH_SRC_INDEX:
        src_index_uprf = src_index;
        break;
    case WRITE_PIVOT_WITH_SRC_INDEX_CONVERT:
        src_index_uprf = src_index;
        convert        = src_default_or_convert;
        break;
    case WRITE_PIVOT_WITH_URPF_PARAMS:
        sdefault = src_default_or_convert;
        sdiscard = src_discard;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    if (!(op & WRITE_PIVOT_WITHOUT_INSERT_HASH)) {
        LPM_HASH_INSERT(u, entry, index, &rvt_index0, &rvt_index1);
    }

    /* Do not use src_index_urpf because it can be invalid.
     * For aux entry, reading src from HW is required for BPM_LENGTH.
     */
    rv = (_soc_alpm_write_pivot_aux(u, src_index, index, entry, convert, bpm_len, v4_half));

    if (rv >= 0) {
        rv = (_lpm_fb_urpf_entry_replicate(u, src_index_uprf, index, entry,
                                          convert, sdefault, sdiscard, bpm_len, v4_half));
    }
    if (!(op & WRITE_PIVOT_WITHOUT_INSERT_HASH) && rv < 0) {
        LPM_HASH_REVERT(u, entry, index, rvt_index0, rvt_index1);
    }

    return rv;
}
static int
_soc_alpm_write_pivot(int u, int src_index, int index, void *entry,
                      uint32 op, int src_default_or_convert, int src_discard)
{
    return _soc_alpm_write_pivot_with_bpm(u, src_index, index, entry, op,
                        src_default_or_convert, src_discard, INVALID_BPM_LEN, 0);
}
static void
_soc_alpm_move_pivot(int u, int src, int dst, int src_half, int dst_half)
{
    alpm_pivot_t *src_pivot;

    src = soc_alpm_physical_idx((u), L3_DEFIPm, (src), 1) << 1;
    dst = soc_alpm_physical_idx((u), L3_DEFIPm, (dst), 1) << 1;
    src += src_half ? 1 : 0; 
    dst += dst_half ? 1 : 0;

    src_pivot = ALPM_TCAM_PIVOT(u, src);
    if (src_pivot) { 
        PIVOT_TCAM_INDEX(src_pivot) = dst;
    }

    ALPM_TCAM_PIVOT(u, dst) = src_pivot;
    ALPM_TCAM_PIVOT(u, src) = NULL;

    return;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
static
int _lpm_fb_entry_shift(int u, int from_ent, int to_ent)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    if (from_ent == to_ent) {
        return SOC_E_NONE;
    }

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, L3_DEFIPm,
                                       MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */

    SOC_IF_ERROR_RETURN(
        _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY, from_ent, e));

    SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, to_ent, e,
                                              WRITE_PIVOT_WITH_SRC_INDEX, 0, 0));

    /* from_ent != to_ent */
    _soc_alpm_move_pivot(u, from_ent, to_ent, 0, 0);
    _soc_alpm_move_pivot(u, from_ent, to_ent, 1, 1);
    return (SOC_E_NONE);
}


/*
 *      Shift prefix entries 1 entry UP, while preserving  
 *      last half empty IPv4 entry if any.
 */
static
int _lpm_fb_shift_pfx_up(int u, int pfx, int ipv6)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    uint32      v0, v1;

    to_ent = SOC_ALPM_LPM_STATE_END(u, pfx) + 1;

    ipv6 = SOC_ALPM_PFX_IS_V6_64(u, pfx);
    if (!ipv6) {
        from_ent = SOC_ALPM_LPM_STATE_END(u, pfx);
#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, L3_DEFIPm,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
        SOC_IF_ERROR_RETURN(
            _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY, from_ent, e));
        v0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
        v1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, to_ent, e,
                                          WRITE_PIVOT_WITH_SRC_INDEX, 0, 0));

            /* track pivot state movement */
            /* from_ent != to_ent */
            _soc_alpm_move_pivot(u, from_ent, to_ent, v1, 0);
            to_ent--;
        }
    }

    from_ent = SOC_ALPM_LPM_STATE_START(u, pfx);
    if (from_ent != to_ent) {
        SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, from_ent, to_ent));
        VRF_PIVOT_SHIFT_INC(u, MAX_VRF_ID, ipv6);
    } 
    SOC_ALPM_LPM_STATE_START(u, pfx) += 1;
    SOC_ALPM_LPM_STATE_END(u, pfx) += 1;
    return (SOC_E_NONE);
}

/*
 *      Shift prefix entries 1 entry DOWN, while preserving  
 *      last half empty IPv4 entry if any.
 */
static
int _lpm_fb_shift_pfx_down(int u, int pfx, int ipv6)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    int         prev_ent;
    uint32      v0, v1;

    to_ent = SOC_ALPM_LPM_STATE_START(u, pfx) - 1;

    /* Don't move empty prefix . */ 
    if (SOC_ALPM_LPM_STATE_VENT(u, pfx) == 0) {
        SOC_ALPM_LPM_STATE_START(u, pfx) = to_ent;
        SOC_ALPM_LPM_STATE_END(u, pfx) = to_ent - 1;
        return (SOC_E_NONE);
    }

    ipv6 = SOC_ALPM_PFX_IS_V6_64(u, pfx);
    if ((!ipv6) && (SOC_ALPM_LPM_STATE_END(u, pfx) != SOC_ALPM_LPM_STATE_START(u, pfx))) {

        from_ent = SOC_ALPM_LPM_STATE_END(u, pfx);

#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, L3_DEFIPm,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
        SOC_IF_ERROR_RETURN(
            _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY, from_ent, e));
        v0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
        v1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            prev_ent = from_ent - 1;
            SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, prev_ent, to_ent));
            VRF_PIVOT_SHIFT_INC(u, MAX_VRF_ID, ipv6);


            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, prev_ent, e,
                                            WRITE_PIVOT_WITH_SRC_INDEX, 0, 0));

            /* from_ent != prev_ent */
            _soc_alpm_move_pivot(u, from_ent, prev_ent, v1, 0);
        } else {
            /* Last entry is full -> just shift it to start - 1  position. */

            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, to_ent, e,
                                            WRITE_PIVOT_WITH_SRC_INDEX, 0, 0));

            /* Track full-entry move */
            /* from_ent != to_ent */
            _soc_alpm_move_pivot(u, from_ent, to_ent, 0, 0);
            _soc_alpm_move_pivot(u, from_ent, to_ent, 1, 1);
        }

    } else  { 

        from_ent = SOC_ALPM_LPM_STATE_END(u, pfx);
        SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, from_ent, to_ent));
        VRF_PIVOT_SHIFT_INC(u, MAX_VRF_ID, ipv6);

    }
    SOC_ALPM_LPM_STATE_START(u, pfx) -= 1;
    SOC_ALPM_LPM_STATE_END(u, pfx) -= 1;

    return (SOC_E_NONE);
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
static
int _lpm_free_slot_create(int u, int pfx, int ipv6, void *e, int *free_slot)
{
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         curr_pfx;
    int         from_ent;
    uint32      v0, v1;
    int         rv;


    if (SOC_ALPM_LPM_STATE_VENT(u, pfx) == 0) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length ALPM_MAX_PFX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = ALPM_MAX_PFX_INDEX;
        if (soc_alpm_mode_get(u) == SOC_ALPM_MODE_PARALLEL) {
            if (pfx <= ALPM_MAX_VRF_PFX_INDEX) {
                curr_pfx = ALPM_MAX_VRF_PFX_INDEX;
            }
        }
        while (SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx) > pfx) {
            curr_pfx = SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx);
        if (next_pfx != -1) {
            SOC_ALPM_LPM_STATE_PREV(u, next_pfx) = pfx;
        }
        SOC_ALPM_LPM_STATE_NEXT(u, pfx) = SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx);
        SOC_ALPM_LPM_STATE_PREV(u, pfx) = curr_pfx;
        SOC_ALPM_LPM_STATE_NEXT(u, curr_pfx) = pfx;

        SOC_ALPM_LPM_STATE_FENT(u, pfx) =  (SOC_ALPM_LPM_STATE_FENT(u, curr_pfx) + 1) / 2;
        SOC_ALPM_LPM_STATE_FENT(u, curr_pfx) -= SOC_ALPM_LPM_STATE_FENT(u, pfx);
        SOC_ALPM_LPM_STATE_START(u, pfx) =  SOC_ALPM_LPM_STATE_END(u, curr_pfx) +
                                       SOC_ALPM_LPM_STATE_FENT(u, curr_pfx) + 1;
        SOC_ALPM_LPM_STATE_END(u, pfx) = SOC_ALPM_LPM_STATE_START(u, pfx) - 1;
        SOC_ALPM_LPM_STATE_VENT(u, pfx) = 0;
    } else if (!ipv6) {
        /* For IPv4 Check if alternate entry is free */
        from_ent = SOC_ALPM_LPM_STATE_START(u, pfx);
#ifdef FB_LPM_TABLE_CACHED
        if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
            return rv;
        }
#endif /* FB_LPM_TABLE_CACHED */
        if ((rv = _soc_mem_alpm_read(u, L3_DEFIPm,
                                     MEM_BLOCK_ANY, from_ent, e)) < 0) {
            return rv;
        }
        v0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
        v1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            return(SOC_E_NONE);
        }

        from_ent = SOC_ALPM_LPM_STATE_END(u, pfx);
#ifdef FB_LPM_TABLE_CACHED
        if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
            return rv;
        }
#endif /* FB_LPM_TABLE_CACHED */
        if ((rv = _soc_mem_alpm_read(u, L3_DEFIPm,
                                     MEM_BLOCK_ANY, from_ent, e)) < 0) {
            return rv;
        }
        v0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID0f);
        v1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALID1f);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            return(SOC_E_NONE);
        }
    }

    free_pfx = pfx;
    while(SOC_ALPM_LPM_STATE_FENT(u, free_pfx) == 0) {
        free_pfx = SOC_ALPM_LPM_STATE_NEXT(u, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while(SOC_ALPM_LPM_STATE_FENT(u, free_pfx) == 0) {
        free_pfx = SOC_ALPM_LPM_STATE_PREV(u, free_pfx);
        if (free_pfx == -1) {
            if (SOC_ALPM_LPM_STATE_VENT(u, pfx) == 0) {
                /* We failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = SOC_ALPM_LPM_STATE_PREV(u, pfx);
                next_pfx = SOC_ALPM_LPM_STATE_NEXT(u, pfx);
                if (-1 != prev_pfx) {
                    SOC_ALPM_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    SOC_ALPM_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
                }
            }
            return(SOC_E_FULL);
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = SOC_ALPM_LPM_STATE_NEXT(u, free_pfx); 
        SOC_IF_ERROR_RETURN(_lpm_fb_shift_pfx_down(u, next_pfx, ipv6));
        SOC_ALPM_LPM_STATE_FENT(u, free_pfx) -= 1;
        SOC_ALPM_LPM_STATE_FENT(u, next_pfx) += 1;
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        SOC_IF_ERROR_RETURN(_lpm_fb_shift_pfx_up(u, free_pfx, ipv6));
        SOC_ALPM_LPM_STATE_FENT(u, free_pfx) -= 1;
        prev_pfx = SOC_ALPM_LPM_STATE_PREV(u, free_pfx); 
        SOC_ALPM_LPM_STATE_FENT(u, prev_pfx) += 1;
        free_pfx = prev_pfx;
    }

    SOC_ALPM_LPM_STATE_VENT(u, pfx) += 1;
    SOC_ALPM_LPM_STATE_FENT(u, pfx) -= 1;
    SOC_ALPM_LPM_STATE_END(u, pfx) += 1;
    *free_slot = SOC_ALPM_LPM_STATE_END(u, pfx) <<  ((ipv6) ? 0 : 1);
    sal_memcpy(e, soc_mem_entry_null(u, L3_DEFIPm),
               soc_mem_entry_words(u,L3_DEFIPm) * 4);

    return(SOC_E_NONE);
}

/*
 *      Delete a slot and adjust entry pointers if required.
 *      e - has the contents of entry at slot(useful for IPV4 only)
 */
static
int _lpm_free_slot_delete(int u, int pfx, int ipv6, void *e, int slot)
{
    int         prev_pfx;
    int         next_pfx;
    int         from_ent;
    int         to_ent;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    uint32      sf[SOC_MAX_MEM_FIELD_WORDS];
    uint32      st[SOC_MAX_MEM_FIELD_WORDS];
    void        *et;
    int         rv = SOC_E_NONE;
    int         bpm_len;
    int         src_half = 0, dst_half = 0;

    from_ent = SOC_ALPM_LPM_STATE_END(u, pfx);
    to_ent = slot;
    if (!ipv6) { /* IPV4 */
        to_ent >>= 1;
#ifdef FB_LPM_TABLE_CACHED
        if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
            return rv;
        }
#endif /* FB_LPM_TABLE_CACHED */
        if ((rv = _soc_mem_alpm_read(u, L3_DEFIPm,
                                     MEM_BLOCK_ANY, from_ent, ef)) < 0) {
            return rv;
        }
        /* get bpm_len of entry that is about to move */
        if ((rv = READ_L3_DEFIP_AUX_TABLEm(u, MEM_BLOCK_ANY, 
                soc_alpm_physical_idx(u, L3_DEFIPm, from_ent, 1), sf)) < 0) {
            return rv;
        }
        if ((rv = READ_L3_DEFIP_AUX_TABLEm(u, MEM_BLOCK_ANY, 
                soc_alpm_physical_idx(u, L3_DEFIPm, to_ent, 1), st)) < 0) {
            return rv;
        }
        et =  (to_ent == from_ent) ? ef : e;
        src_half = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, ef, VALID1f);
        dst_half = (slot & 1);
        if (src_half) {
            bpm_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm, 
                                                        sf, BPM_LENGTH1f);
            if (dst_half) {
                rv = soc_alpm_lpm_ip4entry1_to_1(u, ef, et, PRESERVE_HIT);
                soc_mem_field32_set(u, L3_DEFIP_AUX_TABLEm, st, BPM_LENGTH1f,
                                    bpm_len);
            } else {
                rv = soc_alpm_lpm_ip4entry1_to_0(u, ef, et, PRESERVE_HIT);
                soc_mem_field32_set(u, L3_DEFIP_AUX_TABLEm, st, BPM_LENGTH0f,
                                    bpm_len);
            }
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, ef, VALID1f, 0);
        } else {
            bpm_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm, 
                                                        sf, BPM_LENGTH0f);
            if (dst_half) {
                rv = soc_alpm_lpm_ip4entry0_to_1(u, ef, et, PRESERVE_HIT);
                soc_mem_field32_set(u, L3_DEFIP_AUX_TABLEm, st, BPM_LENGTH1f,
                                    bpm_len);
            } else {
                rv = soc_alpm_lpm_ip4entry0_to_0(u, ef, et, PRESERVE_HIT);
                soc_mem_field32_set(u, L3_DEFIP_AUX_TABLEm, st, BPM_LENGTH0f,
                                    bpm_len);
            }
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, ef, VALID0f, 0);
            SOC_ALPM_LPM_STATE_VENT(u, pfx) -= 1;
            SOC_ALPM_LPM_STATE_FENT(u, pfx) += 1;
            SOC_ALPM_LPM_STATE_END(u, pfx) -= 1;
        }

        /* update bpm_len at target */
        if ((rv = WRITE_L3_DEFIP_AUX_TABLEm(u, MEM_BLOCK_ALL, 
                soc_alpm_physical_idx(u, L3_DEFIPm, to_ent, 1), st)) < 0) {
            return rv;
        }

        if (to_ent == from_ent) {
            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, from_ent, ef,
                                  WRITE_PIVOT_WITH_SRC_INDEX_CONVERT,
                                  ((src_half << 1) | dst_half), 0));
        } else {
            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, to_ent, et,
                                  WRITE_PIVOT_WITH_SRC_INDEX_CONVERT, 
                                  ((src_half << 1) | dst_half), 0));

            /* Use WRITE_PIVOT_WITH_URPF_PARAMS because it's faster */
            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, from_ent, ef,
                                  WRITE_PIVOT_WITH_URPF_PARAMS, 0, 0));
        }

        _soc_alpm_move_pivot(u, from_ent, to_ent, src_half, dst_half);

    } else { /* IPV6 */
        SOC_ALPM_LPM_STATE_VENT(u, pfx) -= 1;
        SOC_ALPM_LPM_STATE_FENT(u, pfx) += 1;
        SOC_ALPM_LPM_STATE_END(u, pfx) -= 1;
        if (to_ent != from_ent) {
#ifdef FB_LPM_TABLE_CACHED
            if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                               MEM_BLOCK_ANY, from_ent)) < 0) {
                return rv;
            }
#endif /* FB_LPM_TABLE_CACHED */
            if ((rv = _soc_mem_alpm_read(u, L3_DEFIPm,
                                         MEM_BLOCK_ANY, from_ent, ef)) < 0) {
                return rv;
            }

            SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, to_ent, ef,
                                  WRITE_PIVOT_WITH_SRC_INDEX, 0, 0));
        }

        /* set from_ent invalid */
        sal_memcpy(ef, soc_mem_entry_null(u, L3_DEFIPm),
                   soc_mem_entry_words(u,L3_DEFIPm) * 4);

        SOC_IF_ERROR_RETURN(_soc_alpm_write_pivot(u, from_ent, from_ent, ef,
                             WRITE_PIVOT_WITH_URPF_PARAMS, 0, 0));

        _soc_alpm_move_pivot(u, from_ent, to_ent, 0, 0);
        _soc_alpm_move_pivot(u, from_ent, to_ent, 1, 1);
    }

    if (SOC_ALPM_LPM_STATE_VENT(u, pfx) == 0) {
        /* remove from the list */
        prev_pfx = SOC_ALPM_LPM_STATE_PREV(u, pfx); /* Always present */
        assert(prev_pfx != -1);
        next_pfx = SOC_ALPM_LPM_STATE_NEXT(u, pfx);
        SOC_ALPM_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
        SOC_ALPM_LPM_STATE_FENT(u, prev_pfx) += SOC_ALPM_LPM_STATE_FENT(u, pfx);
        SOC_ALPM_LPM_STATE_FENT(u, pfx) = 0;
        if (next_pfx != -1) {
            SOC_ALPM_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
        }
        SOC_ALPM_LPM_STATE_NEXT(u, pfx) = -1;
        SOC_ALPM_LPM_STATE_PREV(u, pfx) = -1;
        SOC_ALPM_LPM_STATE_START(u, pfx) = -1;
        SOC_ALPM_LPM_STATE_END(u, pfx) = -1;
    }

    return(rv);
}


/*
 * Function:
 *      soc_alpm_lpm_vrf_get
 * Purpose:
 *      Service routine used to translate hw specific vrf id to API format.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_entry - (IN)Route info buffer from hw.
 *      vrf_id    - (OUT)Virtual router id.
 * Returns:
 *      BCM_E_XXX
 */
int
soc_alpm_lpm_vrf_get(int unit, void *lpm_entry, int *vrf_id, int *vrf)
{
    int mem_vrf;

    /* Get Virtual Router id if supported. */
    if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, VRF_ID_MASK0f)){
        mem_vrf = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f);

        /* Special vrf's handling. */
        if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f)) {
            *vrf_id = mem_vrf;
        } else if (!SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, 
                                                    GLOBAL_HIGH0f)) {
            *vrf_id = SOC_L3_VRF_GLOBAL;
        } else {
            *vrf_id = SOC_L3_VRF_OVERRIDE;
        }
        if (vrf) {
            if (*vrf_id == SOC_L3_VRF_GLOBAL) {
                *vrf = SOC_VRF_MAX(unit) + 1;
            } else {
                *vrf = mem_vrf;
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf_id = SOC_L3_VRF_DEFAULT;
    }
    return (SOC_E_NONE);
}

/*
 * _soc_alpm_lpm_prefix_length_get (Extract vrf weighted  prefix lenght from the 
 * hw entry based on ip, mask & vrf)
 */
static int
_soc_alpm_lpm_prefix_length_get(int u, void *entry, int *pfx_len) 
{
    int         pfx;
    int         rv;
    int         ipv6;
    uint32      ipv4a;
    int         vrf_id;
    int         mode;

    ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, MODE0f);

    if (ipv6) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK1f);
        if (pfx) {
            if (ipv4a != 0xffffffff)  {
                return(SOC_E_PARAM);
            }
            pfx += 32;
        } else {
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
                return(rv);
            }
        }
        pfx += ALPM_IPV6_PFX_ZERO; /* First 33 are for IPV4 addresses */
    } else {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
    }

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, entry, &vrf_id, NULL));

    /* In ALPM the arragnement of VRF is at he begining followed by VRF
     * override and global */
    mode = soc_alpm_mode_get(u);
    switch (vrf_id) {
        case SOC_L3_VRF_GLOBAL:
            if (mode == SOC_ALPM_MODE_PARALLEL) {
                *pfx_len = pfx + (ALPM_MAX_PFX_ENTRIES / 3);
            } else {
                *pfx_len = pfx;
            }
            break;
        case SOC_L3_VRF_OVERRIDE:
            *pfx_len = pfx +  2 * (ALPM_MAX_PFX_ENTRIES / 3);
            break;
        default:
            if (mode == SOC_ALPM_MODE_PARALLEL) {
                *pfx_len = pfx;
            } else {
                *pfx_len = pfx +  (ALPM_MAX_PFX_ENTRIES / 3);
            }
            break;
    }
    return (SOC_E_NONE);
}


/*
 * _soc_alpm_lpm_match (Exact match for the key. Will match both IP address
 * and mask)
 */
static int
_soc_alpm_lpm_match(int u,
               void *key_data,
               void *e,         /* return entry data if found */
               int *index_ptr,  /* return key location */
               int *pfx_len,    /* Key prefix length. vrf + 32 + prefix len for IPV6*/
               int *ipv6)       /* Entry is ipv6. */
{
    int         rv;
    int         v6;
    int         key_index;
    int         pfx = 0;

    v6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODE0f);

    if (v6) {
        if (!(v6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODE1f))) {
            return (SOC_E_PARAM);
        }
    }
    *ipv6 = v6;

    /* Calculate vrf weighted prefix lengh. */
    _soc_alpm_lpm_prefix_length_get(u, key_data, &pfx); 
    *pfx_len = pfx; 

#ifdef FB_LPM_HASH_SUPPORT
    if (LPM_HASH_LOOKUP(u, key_data, pfx, &key_index) == SOC_E_NONE) {
        *index_ptr = key_index;
        if ((rv = _soc_mem_alpm_read(u, L3_DEFIPm, MEM_BLOCK_ANY,
                     (*ipv6) ? *index_ptr : (*index_ptr >> 1), e)) < 0) {
            return rv;
        }
#ifndef  FB_LPM_CHECKER_ENABLE
        return(SOC_E_NONE);
#endif
    } else {
#ifndef  FB_LPM_CHECKER_ENABLE
        return(SOC_E_NOT_FOUND);
#endif
    }
#endif /* FB_LPM_HASH_SUPPORT */
}

/*
 * Initialize the start/end tracking pointers for each prefix length
 */
static int
soc_alpm_lpm_init(int u)
{
    int max_pfx_len;
    int i;
    int defip_table_size;
    int pfx_state_size;
    uint32 rval = 0;
    uint32 mode;

    if (! soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    if ((mode = soc_property_get(u, spn_L3_ALPM_ENABLE, 0))) {
        /* Set DEFIP mode to ALPM */
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(u, &rval));
        soc_reg_field_set(u, L3_DEFIP_RPF_CONTROLr, &rval, LPM_MODEf, 1);
        /* Parallel mode */
        if (mode == SOC_ALPM_MODE_PARALLEL) {
            soc_reg_field_set(u, L3_DEFIP_RPF_CONTROLr, &rval, LOOKUP_MODEf, 1);
        } else {
            /* combined search mode */
            soc_reg_field_set(u, L3_DEFIP_RPF_CONTROLr, &rval, LOOKUP_MODEf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(u, rval));
        SOC_ALPM_SW_LOOKUP(u) = soc_property_get(u, "l3_alpm_sw_prefix_lookup", 1);

        rval = 0;
        if (SOC_URPF_STATUS_GET(u)) {
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &rval, 
                            URPF_LOOKUP_CAM4f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &rval, 
                            URPF_LOOKUP_CAM5f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &rval, 
                            URPF_LOOKUP_CAM6f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &rval, 
                            URPF_LOOKUP_CAM7f, 0x1);
        }
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(u, rval));
        
        rval = 0;
        if (mode == SOC_ALPM_MODE_PARALLEL) {
            if (SOC_URPF_STATUS_GET(u)) {
                /* 4 DIPs, 4 SIPs */
                /* *DIP */
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM2_SELf, 1);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM3_SELf, 1);
                /* VRF SIP */
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM4_SELf, 2);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM5_SELf, 2);
                /* *SIP */
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM6_SELf, 3);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM7_SELf, 3);
            } else {
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM4_SELf, 1);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM5_SELf, 1);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM6_SELf, 1);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM7_SELf, 1);
            }
        } else {
            if (SOC_URPF_STATUS_GET(u)) {
                /* 4 DIPs, 4 SIPs */
                /* DIP */
                /* VRF SIP */
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM4_SELf, 2);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM5_SELf, 2);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM6_SELf, 2);
                soc_reg_field_set(u, L3_DEFIP_ALPM_CFGr, &rval, TCAM7_SELf, 2);
            } 
        }
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(u, rval));
        if (SOC_ALPM_128B_ENABLE(u)) {
            uint32 defip_key_sel_val = 0;

            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_KEY_SELr(u, &defip_key_sel_val));
            /* Supports mixed IPv4, IPv6 128 bit prefix IPv6 routes.
            * upto 16K IPv4 pivots only or
            * upto 2K 128 bit IPv6 and upto 8k v4 pivots, or */
            /* all tcams in paired mode */
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                V6_KEY_SEL_CAM0_1f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                V6_KEY_SEL_CAM2_3f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                V6_KEY_SEL_CAM4_5f, 0x1);
            soc_reg_field_set(u, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                V6_KEY_SEL_CAM6_7f, 0x1);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(u, defip_key_sel_val));
        }
    }

    max_pfx_len = ALPM_MAX_PFX_ENTRIES;

    SOC_ALPM_LPM_LOCK(u);
    pfx_state_size = sizeof(soc_alpm_lpm_state_t) * (max_pfx_len);

    soc_lpm_field_cache_state[u] = 
                sal_alloc(sizeof(soc_lpm_field_cache_t), "lpm_field_state");
    if (NULL == soc_lpm_field_cache_state[u]) {
        SOC_ALPM_LPM_UNLOCK(u);
        return (SOC_E_MEMORY);
    }
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    CLASS_ID0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    CLASS_ID1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    DST_DISCARD0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    DST_DISCARD1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP_COUNT0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP_COUNT1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP_PTR0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ECMP_PTR1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    GLOBAL_ROUTE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    GLOBAL_ROUTE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MULTICAST_ROUTE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MULTICAST_ROUTE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    RPA_ID0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    RPA_ID1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    EXPECTED_L3_IIF0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    EXPECTED_L3_IIF1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    L3MC_INDEX0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    L3MC_INDEX1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    HIT0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    HIT1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    IP_ADDR0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    IP_ADDR1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    IP_ADDR_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    IP_ADDR_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MODE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MODE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MODE_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    MODE_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    NEXT_HOP_INDEX0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    NEXT_HOP_INDEX1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    PRI0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    PRI1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    RPE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    RPE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VALID0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VALID1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VRF_ID_0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VRF_ID_1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VRF_ID_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    VRF_ID_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    GLOBAL_HIGH0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    GLOBAL_HIGH1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ALG_HIT_IDX0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ALG_HIT_IDX1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ALG_BKT_PTR0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    ALG_BKT_PTR1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    DEFAULT_MISS0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, 
                                    DEFAULT_MISS1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_BASE_COUNTER_IDX0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_BASE_COUNTER_IDX1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_POOL_NUMBER0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_POOL_NUMBER1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_OFFSET_MODE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    FLEX_CTR_OFFSET_MODE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    SRC_DISCARD0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    SRC_DISCARD1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_TYPE_MASK0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_TYPE_MASK1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_TYPE0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_TYPE1f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_VIEW0f);
    SOC_ALPM_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm,
                                    ENTRY_VIEW1f);

    SOC_ALPM_LPM_STATE(u) = sal_alloc(pfx_state_size, "LPM prefix info");
    if (NULL == SOC_ALPM_LPM_STATE(u)) {
        sal_free(soc_lpm_field_cache_state[u]);
        soc_lpm_field_cache_state[u] = NULL;
        SOC_ALPM_LPM_UNLOCK(u);
        return (SOC_E_MEMORY);
    }

#ifdef FB_LPM_TABLE_CACHED
    rv = soc_mem_cache_set(u, L3_DEFIPm, MEM_BLOCK_ALL, TRUE);
    if (SOC_FAILURE(rv)) {
        SOC_ALPM_LPM_UNLOCK(u);
        return rv;
    }
#endif /* FB_LPM_TABLE_CACHED */

    sal_memset(SOC_ALPM_LPM_STATE(u), 0, pfx_state_size);
    for(i = 0; i < max_pfx_len; i++) {
        SOC_ALPM_LPM_STATE_START(u, i) = -1;
        SOC_ALPM_LPM_STATE_END(u, i) = -1;
        SOC_ALPM_LPM_STATE_PREV(u, i) = -1;
        SOC_ALPM_LPM_STATE_NEXT(u, i) = -1;
        SOC_ALPM_LPM_STATE_VENT(u, i) = 0;
        SOC_ALPM_LPM_STATE_FENT(u, i) = 0;
    }

    defip_table_size = soc_mem_index_count(u, L3_DEFIPm);

    /* RPF supported, then the table is partitioned for DIP and SIP */
    if (SOC_URPF_STATUS_GET(u)) {
        defip_table_size >>= 1;
    }

    if (mode == SOC_ALPM_MODE_PARALLEL) {
        /* For Parallel search, the TCAM is hard paritioned.
        * First half are for VRF specific, the bottom half for override and 
        * global. Start of Global entries start at the bottom half of TCAM
        */
        SOC_ALPM_LPM_STATE_END(u, ALPM_MAX_PFX_INDEX) = (defip_table_size>>1) - 1;

        /* Free entries for VRF */
        SOC_ALPM_LPM_STATE_FENT(u, (ALPM_MAX_VRF_PFX_INDEX)) = defip_table_size >> 1;

        /* Remaining entries are assigned to Global entries */
        SOC_ALPM_LPM_STATE_FENT(u, (ALPM_MAX_PFX_INDEX)) = 
          (defip_table_size - SOC_ALPM_LPM_STATE_FENT(u, (ALPM_MAX_VRF_PFX_INDEX)));
    } else {
        /* Remaining entries are assigned to Global entries */
        SOC_ALPM_LPM_STATE_FENT(u, (ALPM_MAX_PFX_INDEX)) = defip_table_size;
    }

#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_ALPM_LPM_STATE_HASH(u) != NULL) {
        if (_soc_alpm_lpm_hash_destroy(SOC_ALPM_LPM_STATE_HASH(u)) < 0) {
            SOC_ALPM_LPM_UNLOCK(u);
            return SOC_E_INTERNAL;
        }
        SOC_ALPM_LPM_STATE_HASH(u) = NULL;
    }

    if (_soc_alpm_lpm_hash_create(u, defip_table_size * 2, defip_table_size,
                                &SOC_ALPM_LPM_STATE_HASH(u)) < 0) {
        SOC_ALPM_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#endif


    SOC_ALPM_LPM_UNLOCK(u);
    return(SOC_E_NONE);
}

/*
 * De-initialize the start/end tracking pointers for each prefix length
 */
static int
soc_alpm_lpm_deinit(int u)
{
    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    SOC_ALPM_LPM_LOCK(u);

#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_ALPM_LPM_STATE_HASH(u) != NULL) {
        _soc_alpm_lpm_hash_destroy(SOC_ALPM_LPM_STATE_HASH(u));
        SOC_ALPM_LPM_STATE_HASH(u) = NULL;
    }
#endif

    if (SOC_ALPM_LPM_INIT_CHECK(u)) {
        sal_free(soc_lpm_field_cache_state[u]);
        soc_lpm_field_cache_state[u] = NULL;
        sal_free(SOC_ALPM_LPM_STATE(u));
        SOC_ALPM_LPM_STATE(u) = NULL;
    }

    SOC_ALPM_LPM_UNLOCK(u);

    return(SOC_E_NONE);
}

/*
 * Implementation using soc_mem_read/write using entry rippling technique
 * Advantage: A completely sorted table is not required. Lookups can be slow
 * as it will perform a linear search on the entries for a given prefix length.
 * No device access necessary for the search if the table is cached. Auxiliary
 * hash table can be maintained to speed up search(Not implemented) instead of
 * performing a linear search.
 * Small number of entries need to be moved around (97 worst case)
 * for performing insert/update/delete. However CPU needs to do all
 * the work to move the entries.
 */

/*
 * soc_alpm_lpm_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 * src_default: for sip entry if any
 *              0 - caculate using entry_data's mask (legacy way)
 *              1 - force set (split case)
 * src_discard: for sip entry if any
 */
static int
soc_alpm_lpm_insert(int u, void *entry_data, int *lpm_index,
                    int src_default, int src_discard, int bpm_len)
{
    int         pfx;
    int         index;
    int         ipv6, v4_half = 0;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;
    int         found = 0;
    int         vrf, vrf_id;

    sal_memcpy(e, soc_mem_entry_null(u, L3_DEFIPm),
               soc_mem_entry_words(u, L3_DEFIPm) * 4);

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, entry_data, &vrf_id, &vrf));

    SOC_ALPM_LPM_LOCK(u);
    rv = _soc_alpm_lpm_match(u, entry_data, e, &index, &pfx, &ipv6);
    if (rv == SOC_E_NOT_FOUND) {
        rv = _lpm_free_slot_create(u, pfx, ipv6, e, &index);
        if (rv < 0) {
            SOC_ALPM_LPM_UNLOCK(u);
            return(rv);
        }
    } else {
        found = 1;
    }

    *lpm_index = index;

    if (rv == SOC_E_NONE) {
        if (!ipv6) {
            /* IPV4 entry */
            if (index & 1) {
                rv = soc_alpm_lpm_ip4entry0_to_1(u, entry_data, e, 
                                                 PRESERVE_HIT);
                v4_half = 3;
            } else {
                rv = soc_alpm_lpm_ip4entry0_to_0(u, entry_data, e, 
                                                 PRESERVE_HIT);
                v4_half = 2;
            }

            if (rv < 0) {
                SOC_ALPM_LPM_UNLOCK(u);
                return(rv);
            }
            entry_data = (void *)e;
            index >>= 1;
        }
        soc_alpm_lpm_state_dump(u);
        LOG_INFO(BSL_LS_SOC_ALPM,
                 (BSL_META_U(u,
                             "\nsoc_alpm_lpm_insert: %d %d\n"),
                             index, pfx));
        rv = _soc_alpm_write_pivot_with_bpm(u, index, index, entry_data,
                             WRITE_PIVOT_WITH_URPF_PARAMS | 
                             (found ? WRITE_PIVOT_WITHOUT_INSERT_HASH : 0),
                             src_default, src_discard, bpm_len, v4_half);
        if (!found && SOC_SUCCESS(rv)) {
            VRF_PIVOT_REF_INC(u, vrf_id, vrf, ipv6);
        }
    }

    SOC_ALPM_LPM_UNLOCK(u);

    return(rv);
}

/*
 * soc_alpm_lpm_delete
 */
static int
soc_alpm_lpm_delete(int u, void *key_data)
{
    int         pfx;
    int         index;
    int         ipv6;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;
    int         vrf, vrf_id;

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(u, key_data, &vrf_id, &vrf));    
    SOC_ALPM_LPM_LOCK(u);
    rv = _soc_alpm_lpm_match(u, key_data, e, &index, &pfx, &ipv6);
    if (rv == SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_ALPM,
                 (BSL_META_U(u,
                             "\nsoc_alpm_lpm_delete: %d %d\n"),
                             index, pfx));
        LPM_HASH_DELETE(u, key_data, index);
        rv = _lpm_free_slot_delete(u, pfx, ipv6, e, index);
    }
    if (SOC_SUCCESS(rv)) {
        VRF_PIVOT_REF_DEC(u, vrf_id, vrf, ipv6);
    }
    soc_alpm_lpm_state_dump(u);
    SOC_ALPM_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_alpm_lpm_match (Exact match for the key. Will match both IP
 *                   address and mask)
 */
static int
soc_alpm_lpm_match(int u,
               void *key_data,
               void *e,         /* return entry data if found */
               int *index_ptr)  /* return key location */
{
    int        pfx;
    int        rv;
    int        ipv6;

    SOC_ALPM_LPM_LOCK(u);
    rv = _soc_alpm_lpm_match(u, key_data, e, index_ptr, &pfx, &ipv6);
    SOC_ALPM_LPM_UNLOCK(u);
    return(rv);
}


/* Fill Aux entry for operation */
static int
_soc_alpm_fill_aux_entry_for_op(int unit,
               void *key_data,
               int ipv6,            /* Entry is ipv6. */
               int db_type,         /* database type */
               int ent_type,
               int replace_len,     /* used for Delete propagate */
               defip_aux_scratch_entry_t *aux_entry)
{
    uint32 ipv4a;
    uint32 ip6_word[4] = {0, 0, 0, 0};
    int    pfx = 0;
    int    rv = SOC_E_NONE;

    sal_memset(aux_entry, 0, sizeof(defip_aux_scratch_entry_t));

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, VALID0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, VALIDf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, MODE0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, MODEf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, ENTRY_TYPE0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, ENTRY_TYPEf, ent_type);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, GLOBAL_ROUTE0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, GLOBAL_ROUTEf, ipv4a);

    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, RPA_ID0f);
        soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, RPA_IDf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, EXPECTED_L3_IIF0f);
        soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, EXPECTED_L3_IIFf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
        soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
        soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, L3MC_INDEX0f);
        soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, L3MC_INDEXf, ipv4a);
    }

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, ECMP0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, ECMPf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, ECMP_PTR0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, ECMP_PTRf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, NEXT_HOP_INDEX0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, NEXT_HOP_INDEXf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, PRI0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, PRIf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, RPE0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, RPEf, ipv4a);

    ipv4a =soc_mem_field32_get(unit, L3_DEFIPm, key_data, VRF_ID_0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, VRFf, ipv4a);

    /* Set Database type */
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, DB_TYPEf, db_type);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, DST_DISCARD0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, DST_DISCARDf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, DEFAULT_MISS0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, DEFAULT_MISSf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, CLASS_ID0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, CLASS_IDf, ipv4a);


    /* Set IP Address and IP length */
    if (ipv6) {
        ip6_word[2] = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR0f);
        ip6_word[3] = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR1f);
    } else {
        ip6_word[0] = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR0f);
    }
    soc_mem_field_set(unit, L3_DEFIP_AUX_SCRATCHm, (uint32*)aux_entry,
                      IP_ADDRf, (uint32*)ip6_word);

    if (ipv6) {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR_MASK1f);
        if (pfx) {
            if (ipv4a != 0xffffffff)  {
                return(SOC_E_PARAM);
            }
            pfx += 32;
        } else {
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
                return(rv);
            }
        }
    } else {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, key_data, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
    }
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, IP_LENGTHf, pfx);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, REPLACE_LENf,
                        replace_len);

    ipv4a = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, key_data, FLEX_CTR_BASE_COUNTER_IDX0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, FLEX_CTR_BASE_COUNTER_IDXf, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, key_data, FLEX_CTR_OFFSET_MODE0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, FLEX_CTR_OFFSET_MODEf, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, key_data, FLEX_CTR_POOL_NUMBER0f);
    soc_mem_field32_set(unit, L3_DEFIP_AUX_SCRATCHm, aux_entry, FLEX_CTR_POOL_NUMBERf, ipv4a);

    return (SOC_E_NONE);
}

#define _SOC_ALPM_AUX_OP_RETRY_TO           1000000
#define _SOC_ALPM_AUX_OP_RETRY_COUNT        100
#define _TD2_PIPES_PER_DEV                  2

/* Using binary search with DMA read to retrieve corrupted index */
int
_soc_alpm_aux_error_index(int u, int pipe, int *index, soc_mem_t *mem)
{
    char *dmabuf = NULL;
    soc_mem_t mem_to_search[2] = {L3_DEFIP_AUX_TABLEm, L3_DEFIP_DATA_ONLYm};
    int min, max;
    uint32 ser_flags;
    int i = 0;
    int found_index = FALSE;
    int mem_wsz = 0;

    switch (pipe) {
        case 0:
            ser_flags = _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
            break;
        case 1:
            ser_flags = _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
            break;
        default:
            ser_flags = _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
            break;
    }

    ser_flags |= _SOC_SER_FLAG_DMA_ERR_RETURN;
    ser_flags |= _SOC_SER_FLAG_DISCARD_READ;
    ser_flags |= _SOC_SER_FLAG_MULTI_PIPE;

    for (i = 0; i < 2; i++) {
        min = soc_mem_index_min(u, mem_to_search[i]);
        if (mem_to_search[i] == L3_DEFIP_DATA_ONLYm) {
            max = soc_mem_index_max(u, L3_DEFIP_DATA_ONLYm) +
                  soc_mem_index_max(u,L3_DEFIP_PAIR_128_DATA_ONLYm) * 2;
        } else {
            max = soc_mem_index_max(u, mem_to_search[i]);
        }

        mem_wsz = sizeof(uint32) * soc_mem_entry_words(u, mem_to_search[i]);
        dmabuf = soc_cm_salloc(u, max * mem_wsz, "auxtbl_dmabuf");
        if (dmabuf == NULL) {
            return SOC_E_MEMORY;
        }

        while (min <= max) {
            int mid = min + (max - min) / 2;
            if (soc_mem_ser_read_range(u, mem_to_search[i], MEM_BLOCK_ALL, min, mid, ser_flags,
                                       dmabuf) < 0) {
                if (max == min) {
                    *index = min;
                    found_index = TRUE;
                    break;
                }
                max = mid;
            } else {
                min = mid + 1;
            }
        }

        soc_cm_sfree(u, dmabuf);

        if (found_index) {
            *mem = mem_to_search[i];
            break;
        }
    }

    if (min > max) {
        /* Not found */
        LOG_WARN(BSL_LS_SOC_ALPM,
            (BSL_META_U(u, "No corrupted index in L3_DEFIP_AUX_TABLE or"
                           "L3_DEFIP_DATA_ONLY Pipe %d\n"), pipe));
        return SOC_E_NOT_FOUND;
    } else {
        LOG_WARN(BSL_LS_SOC_ALPM,
            (BSL_META_U(u, "Found corrupted Index %d in %s Table Pipe %d\n"),
            *index, SOC_MEM_NAME(u, *mem), pipe));
        return SOC_E_NONE;
    }
}

/* Perform an ALPM hardware operation */
int
_soc_alpm_aux_hw_op(int u, _soc_aux_op_t aux_op,
                    defip_aux_scratch_entry_t *aux_entry, int update_scratch,
                    int *hit, int *tcam_index, int *bucket_index)
{
    soc_timeout_t  to;
    int pipe, rv = SOC_E_NONE;
    int _soc_alpm_aux_op_retry = 0;

    uint32 rval[_TD2_PIPES_PER_DEV];
    int value[_TD2_PIPES_PER_DEV];
    int err[_TD2_PIPES_PER_DEV];

    uint32 baddr;
    int block;
    uint8 acc_type;
    soc_mem_t mem = INVALIDm;

    /* Write to Aux Scratch before performing the operation */
    if (update_scratch) {
        SOC_IF_ERROR_RETURN(
            WRITE_L3_DEFIP_AUX_SCRATCHm(u, MEM_BLOCK_ANY, 0, aux_entry));
    }

#ifdef AUX_OPTIMIZATIONS_OFF
    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(u, &rval[0]));
    if (soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr, rval[0], STARTf)) {
        /* Aux op in porgress */
        return SOC_E_BUSY;
    }
#endif /* OPTIMIZATIONS_OFF */

_retry:
    switch (aux_op) {
    case INSERT_PROPAGATE:
        value[0] = 0;
        break;
    case DELETE_PROPAGATE:
        value[0] = 1;
        break;
    case PREFIX_LOOKUP:
        value[0] = 2;
        break;
    case HITBIT_REPLACE:
        value[0] = 3;
        break;
    default:
        return SOC_E_PARAM;
    }

    rval[0] = 0;

    /* set opcode */
    soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[0], OPCODEf, value[0]);

    /* Start opertation */
    soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[0], STARTf, 1);

    /* Write to Aux ctrl reg */
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(u, rval[0]));
    soc_timeout_init(&to, 50000, 5);

    /* Check for Operation complete */
    sal_memset(value, 0, sizeof(value));
    sal_memset(err, 0, sizeof(err));
    baddr = soc_reg_addr_get(u, L3_DEFIP_AUX_CTRLr, REG_PORT_ANY, 0,
                             SOC_REG_ADDR_OPTION_NONE,
                             &block, &acc_type);
    do {
        for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
            SOC_IF_ERROR_RETURN
                (_soc_reg32_get(u, block, pipe + 1, baddr, &rval[pipe]));
            value[pipe] = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr,
                                            rval[pipe], DONEf);
            err[pipe] = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr,
                                          rval[pipe], ERRORf);
            if (value[pipe] != 1) {
                break;
            }
        }

        /* All DONE */
        if (pipe >= _TD2_PIPES_PER_DEV) {
            rv = SOC_E_NONE;
            break;
        }

        if (soc_timeout_check(&to)) {
            sal_memset(value, 0, sizeof(value));
            sal_memset(err, 0, sizeof(err));
            for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
                SOC_IF_ERROR_RETURN
                    (_soc_reg32_get(u, block, pipe + 1, baddr, &rval[pipe]));
                value[pipe] = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr,
                                                rval[pipe], DONEf);
                err[pipe] = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr,
                                              rval[pipe], ERRORf);
                if (value[pipe] != 1) {
                    break;
                }
            }

            if (pipe >= _TD2_PIPES_PER_DEV) {
                rv = SOC_E_NONE;
            } else {
                LOG_WARN(BSL_LS_SOC_ALPM,
                         (BSL_META_U(u, "unit %d : DEFIP AUX Operation "
                         "timeout, Pipe %d\n"), u, pipe));
                rv = SOC_E_TIMEOUT;
            }
            break;
        }
    } while (1);

    if (SOC_SUCCESS(rv)) {
        for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
            if (err[pipe]) {
                soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[pipe], STARTf, 0);
                soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[pipe], ERRORf, 0);
                soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[pipe], DONEf, 0);

                SOC_IF_ERROR_RETURN
                    (_soc_reg32_set(u, block, pipe + 1, baddr, rval[pipe]));
                LOG_WARN(BSL_LS_SOC_ALPM,
                         (BSL_META_U(u, "DEFIP AUX Operation encountered "
                                     "parity error in Pipe %d!!\n"), pipe));
                _soc_alpm_aux_op_retry++;
                if (SOC_CONTROL(u)->alpm_bulk_retry) {
                    sal_sem_take(SOC_CONTROL(u)->alpm_bulk_retry, _SOC_ALPM_AUX_OP_RETRY_TO);
                }
                if (_soc_alpm_aux_op_retry < _SOC_ALPM_AUX_OP_RETRY_COUNT) {
                    int err_index;
                    if (_soc_alpm_aux_error_index(u, pipe, &err_index, &mem) >= 0) {
                        /* Call inline correction with err_index and pipe */
                        rv = soc_mem_alpm_aux_table_correction(u, pipe, err_index, mem);
                        if (rv < 0) {
                            LOG_ERROR(BSL_LS_SOC_ALPM,
                                (BSL_META_U(u, "_soc_th_alpm_aux_error_index "
                                "pipe %d index %d failed\n"), pipe, err_index));
                        }
                    }
                    LOG_WARN(BSL_LS_SOC_ALPM,
                        (BSL_META_U(u, "Retry DEFIP AUX Operation in "
                        "Pipe %d.\n"), pipe));
                    goto _retry;
                } else {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                        (BSL_META_U(u, "unit %d: Aborting DEFIP AUX Operation "
                        "due to un-correctable error !!\n"), u));
                    return SOC_E_INTERNAL;
                }
            }
        }

        if (aux_op == PREFIX_LOOKUP) {
            if (hit && tcam_index) {
                /* Get the hit bit and Bucket index for lookup operations */
                *hit = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr, rval[0], HITf);
                *tcam_index = soc_reg_field_get(u, L3_DEFIP_AUX_CTRLr,
                                                rval[0], BKT_INDEXf);
                SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(u, &rval[1]));
                *bucket_index = soc_reg_field_get(u, L3_DEFIP_AUX_CTRL_1r,
                                                  rval[1], BKT_PTRf);
            }
        }
    }
#ifdef AUX_OPTIMIZATIONS_OFF
    /* clear AUX ctrl register */
    soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[0], STARTf, 0);
    soc_reg_field_set(u, L3_DEFIP_AUX_CTRLr, &rval[0], DONEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(u, rval[0]));
#endif /* OPTIMIZATIONS_OFF */
    return rv;
}

int
_soc_alpm_aux_op(int u, _soc_aux_op_t aux_op,
                 defip_aux_scratch_entry_t *aux_entry, int update_scratch,
                 int *hit, int *tcam_index, int *log_bkt, int aux_flags)
{
    int rv = SOC_E_NONE;
    int cb_cnt = 1;

    /*
     * SW propagation:
     * Callback only being proceeded when cache of LPM TCAM tables is
     * turn on.
     */
    if (ALPM_CTRL(u).trie_propagate) {
        rv = _soc_alpm_aux_sw_op(u, aux_op, aux_entry,
                                 &cb_cnt, aux_flags);
    }

    /* Hareware assists only needed when matched pivot is hit. */
    if (SOC_SUCCESS(rv) && cb_cnt > 0) {
        uint32 val;

        if (SOC_IS_TRIDENT2(u) && (aux_op == INSERT_PROPAGATE)) {
            /*
             * Do DELETE propagation to update existing default datas
             * Since INSERT Propagate doesn't handle '==' case
             */
            rv = _soc_alpm_aux_hw_op(u, DELETE_PROPAGATE, aux_entry,
                                     update_scratch, hit, tcam_index, log_bkt);
            if (SOC_SUCCESS(rv)) {
                rv = _soc_alpm_aux_hw_op(u, aux_op, aux_entry, FALSE,
                                         hit, tcam_index, log_bkt);
            }
        } else {
            rv = _soc_alpm_aux_hw_op(u, aux_op, aux_entry, update_scratch,
                                     hit, tcam_index, log_bkt);
        }

        if ((aux_op == INSERT_PROPAGATE || aux_op == DELETE_PROPAGATE) &&
            SOC_SUCCESS(rv) && SOC_URPF_STATUS_GET(u)) {
            soc_mem_t mem = L3_DEFIP_AUX_SCRATCHm;
            val = soc_mem_field32_get(u, mem, aux_entry, DB_TYPEf);
            soc_mem_field32_set(u, mem, aux_entry, DB_TYPEf, val + 1);

            val = (aux_flags & SOC_ALPM_AUX_DEF_ROUTE) ? 1 : 0;
            soc_mem_field32_set(u, mem, aux_entry, DEFAULTROUTEf, val);

            val = (aux_flags & SOC_ALPM_AUX_SRC_DISCARD) ? 1 : 0;
            soc_mem_field32_set(u, mem, aux_entry, SRC_DISCARDf, val);

            if (SOC_IS_TRIDENT2(u) && (aux_op == INSERT_PROPAGATE)) {
                /*
                 * Do DELETE propagation to update existing default datas
                 * Since INSERT Propagate doesn't handle '==' case.
                 */
                rv = _soc_alpm_aux_hw_op(u, DELETE_PROPAGATE, aux_entry,
                                         TRUE, NULL, NULL, NULL);
                if (SOC_SUCCESS(rv)) {
                    rv = _soc_alpm_aux_hw_op(u, aux_op, aux_entry, FALSE,
                                             NULL, NULL, NULL);
                }
            } else {
                rv = _soc_alpm_aux_hw_op(u, aux_op, aux_entry, TRUE,
                                         NULL, NULL, NULL);
            }
        }
    }

    return rv;
}

/* Perform an ALPM mem read operation */
int
_soc_mem_alpm_read(int unit, soc_mem_t mem, int copyno,
                   int index, void *entry_data)
{
    int rv = SOC_E_NONE;
    int read_no_cache = FALSE;

    switch (mem) {
        case L3_DEFIPm:
        case L3_DEFIP_PAIR_128m:
        case L3_DEFIP_ALPM_RAWm:
        case L3_DEFIP_ALPM_IPV4m:
        case L3_DEFIP_ALPM_IPV4_1m:
        case L3_DEFIP_ALPM_IPV6_64m:
        case L3_DEFIP_ALPM_IPV6_64_1m:
        case L3_DEFIP_ALPM_IPV6_128m:
            if (!ALPM_CTRL(unit).hit_bits_skip) {
                /* Be sure to get hit information */
                read_no_cache = TRUE;
            }
            break;
        default:
            break;
    }

    if (read_no_cache) {
        rv = soc_mem_read_no_cache(unit, 0, mem, 0, copyno, index, entry_data);
    } else {
        rv = soc_mem_read(unit, mem, copyno, index, entry_data);
    }

    return rv;
}

/* Initialize the ALPM memory from a TCAM entry */
static int
_soc_alpm_mem_ent_init(int unit, void *lpm_entry, 
                       void *alpm_entry, void *alpm_sip_entry, soc_mem_t mem,
                       uint32 flags, uint32 *is_default)
{
    uint32 ipv4a;
    uint32 ip6_word[4] = {0, 0};
    int    pfx = 0;
    int    rv = SOC_E_NONE;
    int ipv6;
    uint32 default_route = 0;

    ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0f);

    /* Zero Buffer */
    sal_memset(alpm_entry, 0, soc_mem_entry_words(unit, mem) * 4);

    if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, HIT0f);
        soc_mem_field32_set(unit, mem, alpm_entry, HITf, ipv4a);
    }

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, VALID0f);
    soc_mem_field32_set(unit, mem, alpm_entry, VALIDf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, ECMP0f);
    soc_mem_field32_set(unit, mem, alpm_entry, ECMPf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, ECMP_PTR0f);
    soc_mem_field32_set(unit, mem, alpm_entry, ECMP_PTRf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, NEXT_HOP_INDEX0f);
    soc_mem_field32_set(unit, mem, alpm_entry, NEXT_HOP_INDEXf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, PRI0f);
    soc_mem_field32_set(unit, mem, alpm_entry, PRIf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, RPE0f);
    soc_mem_field32_set(unit, mem, alpm_entry, RPEf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, DST_DISCARD0f);
    soc_mem_field32_set(unit, mem, alpm_entry, DST_DISCARDf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, SRC_DISCARD0f);
    soc_mem_field32_set(unit, mem, alpm_entry, SRC_DISCARDf, ipv4a);

    ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, CLASS_ID0f);
    soc_mem_field32_set(unit, mem, alpm_entry, CLASS_IDf, ipv4a);

    /* Set IP Address and IP length */
    ip6_word[0] = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IP_ADDR0f);
    if (ipv6) {
        ip6_word[1] = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IP_ADDR1f);
    }
    soc_mem_field_set(unit, mem, (uint32*)alpm_entry, KEYf, (uint32*)ip6_word);

    if (ipv6) {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f);
        if (pfx) {
            if (ipv4a != 0xffffffff)  {
                return(SOC_E_PARAM);
            }
            pfx += 32;
        } else {
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
                return(rv);
            }
        }
    } else {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
            return(rv);
        }
    }
    if ((pfx == 0) && (ip6_word[0] == 0) && (ip6_word[1] == 0)) {
        default_route = 1;
    }
    if (is_default != NULL) {
        *is_default = default_route;
    }
    soc_mem_field32_set(unit, mem, alpm_entry, LENGTHf, pfx);

    if (mem == L3_DEFIP_ALPM_IPV4_1m || mem == L3_DEFIP_ALPM_IPV6_64_1m) {
        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_BASE_COUNTER_IDX0f);
        soc_mem_field32_set(unit, mem, alpm_entry, FLEX_CTR_BASE_COUNTER_IDXf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_OFFSET_MODE0f);
        soc_mem_field32_set(unit, mem, alpm_entry, FLEX_CTR_OFFSET_MODEf, ipv4a);

        ipv4a = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_POOL_NUMBER0f);
        soc_mem_field32_set(unit, mem, alpm_entry, FLEX_CTR_POOL_NUMBERf, ipv4a);
    }

    if (alpm_sip_entry == NULL) {
        return (SOC_E_NONE);
    }
    if (SOC_URPF_STATUS_GET(unit)) {
        sal_memset(alpm_sip_entry, 0, soc_mem_entry_words(unit, mem) * 4);
        sal_memcpy(alpm_sip_entry, alpm_entry, soc_mem_entry_words(unit, mem) * 4);
        soc_mem_field32_set(unit, mem, alpm_sip_entry, DST_DISCARDf, 0);
        soc_mem_field32_set(unit, mem, alpm_sip_entry, RPEf, 0);

        soc_mem_field32_set(unit, mem, alpm_sip_entry, SRC_DISCARDf,
                            flags & SOC_ALPM_RPF_SRC_DISCARD);
        soc_mem_field32_set(unit, mem, alpm_sip_entry, DEFAULTROUTEf,
                            default_route);
    }

    return (SOC_E_NONE);
}

/* Initialize the TCAM entry from ALPM memory */
static int
_soc_alpm_lpm_ent_init(int unit, void *alpm_entry, soc_mem_t mem, int ipv6, 
                       int vrf_id, int bucket, int index, void *lpm_entry,
                       int flex)
{
    uint32 ipv4a;
    uint32 ip6_word[4] = {0, 0};
    uint32 pfx = 0;

    /* Zero Buffer */
    sal_memset(lpm_entry, 0, soc_mem_entry_words(unit, L3_DEFIPm) * 4);

    if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
        ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, HITf);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, HIT0f, ipv4a);
        if (ipv6) {
            soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, HIT1f, ipv4a);
        }
    }

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, VALIDf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VALID0f, ipv4a);
    if (ipv6) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VALID1f, ipv4a);
    }

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, ECMPf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ECMP0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, ECMP_PTRf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ECMP_PTR0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, NEXT_HOP_INDEXf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, NEXT_HOP_INDEX0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, PRIf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, PRI0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, RPEf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, RPE0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, DST_DISCARDf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, DST_DISCARD0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, SRC_DISCARDf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, SRC_DISCARD0f, ipv4a);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, CLASS_IDf);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, CLASS_ID0f, ipv4a);

    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ALG_BKT_PTR0f, bucket);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ALG_HIT_IDX0f, index);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE_MASK0f, 3);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE_MASK0f, 1);

    if (ipv6) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE0f, 1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE1f, 1);
    }

    /* Set IP Address and IP length */
    soc_mem_field_get(unit, mem, alpm_entry, KEYf, ip6_word);
    if (ipv6) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR1f, 
                            ip6_word[1]);
    }
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR0f, ip6_word[0]);

    ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, LENGTHf);
    if (ipv6) {
        if (ipv4a >= 32) {
            pfx = 0xffffffff;
            soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, pfx);
            pfx = ~SHR_SHIFT_RIGHT(0xffffffff, ipv4a - 32);
            soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, pfx);
        } else {
            pfx = ~(0xffffffff >> ipv4a);
            soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f, pfx);
        }
    } else {
        assert(ipv4a <= 32);
        pfx = ~SHR_SHIFT_RIGHT(0xffffffff, ipv4a);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f, pfx);
    }

    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f, flex);

    if (vrf_id == SOC_L3_VRF_OVERRIDE) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_HIGH0f, 1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f, 0);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f, 0);
    } else  if (vrf_id == SOC_L3_VRF_GLOBAL) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f, 0);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f, 0);
    } else {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f, vrf_id);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f,
                            SOC_VRF_MAX(unit));
    }

    if (mem == L3_DEFIP_ALPM_IPV4_1m || mem == L3_DEFIP_ALPM_IPV6_64_1m) {
        ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, FLEX_CTR_BASE_COUNTER_IDXf);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_BASE_COUNTER_IDX0f, ipv4a);

        ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, FLEX_CTR_OFFSET_MODEf);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_OFFSET_MODE0f, ipv4a);

        ipv4a = soc_mem_field32_get(unit, mem, alpm_entry, FLEX_CTR_POOL_NUMBERf);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_POOL_NUMBER0f, ipv4a);
    }

    return (SOC_E_NONE);
}

int 
soc_alpm_warmboot_pivot_add(int unit,
                            int ipv6,
                            void *lpm_entry,
                            int tcam_idx, 
                            int bckt_idx)
{
    int                  rv = SOC_E_NONE;
    uint32               key[2] = {0, 0};
    alpm_pivot_t         *pivot = NULL;
    alpm_bucket_handle_t *bucket_handle = NULL;
    int                  vrf_id = 0, vrf = 0;
    uint32               length2;
    trie_t               *pivot_root = NULL;
    uint32               prefix[5] = {0};
    uint32               bpm_len = 0;
    int                  default_route = 0;
    defip_aux_table_entry_t aux_ent;

    rv = _alpm_prefix_create(unit, lpm_entry, prefix, &length2, &default_route);
    SOC_IF_ERROR_RETURN(rv);    

    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf));
    tcam_idx = soc_alpm_physical_idx(unit, L3_DEFIPm, tcam_idx, ipv6);

    /* Allocate bucket handle */
    bucket_handle = sal_alloc(sizeof(alpm_bucket_handle_t), "ALPM Bucket Handle");
    if (bucket_handle == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for "
                              "PIVOT trie node \n")));
        return SOC_E_NONE;
    }
    sal_memset(bucket_handle, 0, sizeof(*bucket_handle));

    /* Allocate pivot Payload */
    pivot = sal_alloc(sizeof(alpm_pivot_t), "Payload for Pivot");
    if (pivot == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for "
                              "PIVOT trie node \n")));
        sal_free(bucket_handle);
        return SOC_E_MEMORY;
    }
    sal_memset(pivot, 0, sizeof(*pivot));

    /* Save bucket_handle in pivot->bucket_trie */
    PIVOT_BUCKET_HANDLE(pivot) = bucket_handle;
    
    if (ipv6) {
        trie_init(_MAX_KEY_LEN_144_, &PIVOT_BUCKET_TRIE(pivot));

        /* Set address to the buffer. */
        key[0] = soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR0f);

        /* Set address to the buffer. */
        key[1] = soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR1f);
    } else {
        trie_init(_MAX_KEY_LEN_48_, &PIVOT_BUCKET_TRIE(pivot));

        /* Set address to the buffer. */
        key[0] = soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR0f);
    }
    
    /* Save bucket index into pivot->bucket->bucket_index */
    PIVOT_BUCKET_INDEX(pivot) = bckt_idx;
   
    /* Save tcam index into pivot->tcam_index */
    PIVOT_TCAM_INDEX(pivot) = tcam_idx;

    if (vrf_id != SOC_L3_VRF_OVERRIDE) {
        if (ipv6 == 0) {
            pivot_root = VRF_PIVOT_TRIE_IPV4(unit, vrf);
            if (pivot_root == NULL) {
                trie_init(_MAX_KEY_LEN_48_, &VRF_PIVOT_TRIE_IPV4(unit, vrf));
                pivot_root = VRF_PIVOT_TRIE_IPV4(unit, vrf);
            }
        } else {
            pivot_root = VRF_PIVOT_TRIE_IPV6(unit, vrf);
            if (pivot_root == NULL) {
                trie_init(_MAX_KEY_LEN_144_, &VRF_PIVOT_TRIE_IPV6(unit, vrf));
                pivot_root = VRF_PIVOT_TRIE_IPV6(unit, vrf);
            }
        }

        sal_memcpy(pivot->key, prefix, sizeof(prefix));
        pivot->len    = length2;
        
        /* insert into pivot trie */
        rv = soc_mem_read(unit, L3_DEFIP_AUX_TABLEm, SOC_BLOCK_ANY,
                          tcam_idx >> 1, &aux_ent);
        if (SOC_SUCCESS(rv)) {
            bpm_len = soc_mem_field32_get(unit, L3_DEFIP_AUX_TABLEm, &aux_ent,
                                ipv6 ? BPM_LENGTH0f :
                                (tcam_idx & 1) ? BPM_LENGTH1f : BPM_LENGTH0f);
        }

        rv = trie_insert(pivot_root, pivot->key, NULL, pivot->len, 
                         (trie_node_t *)pivot);
        if (SOC_FAILURE(rv)) {
            sal_free(bucket_handle);
            sal_free(pivot);
            return rv;
        }
    }

    PIVOT_TCAM_BPMLEN(pivot) = bpm_len;
    /* Insert pivot into global pivot array indexed by tcam_idx */
    ALPM_TCAM_PIVOT(unit, tcam_idx) = pivot;
    PIVOT_BUCKET_VRF(pivot) = vrf;
    PIVOT_BUCKET_IPV6(pivot) = ipv6;
    
    PIVOT_BUCKET_ENT_CNT_UPDATE(pivot);
    if (key[0] == 0 && key[1] == 0) {
        PIVOT_BUCKET_DEF(pivot) = TRUE;
    }

    return rv;
}


static int
_soc_alpm_warmboot_insert(int unit,
                          int ipv6,
                          int  flex,
                          void *lpm_entry, 
                          void *alpm_entry, /* ALPM (SRAM) entry data */
                          int tcam_idx,     /* TCAM index             */
                          int bckt_idx,     /* Bucket index           */
                          int alpm_idx )    /* ALPM entry index       */
{
    int          vrf_id;  /* Internal API representation of VRF id           */
    int          vrf;      /* VRF in 0 to (MAX_VRF + 1) range;                */
                           /* VRF = (MAX_VRF + 1), software rep of GLOBAL VRF */
    int          rv = SOC_E_NONE;
    int          default_route = 0;
    uint32       prefix[5] = {0, 0, 0, 0, 0};
    uint32       length;
    defip_entry_t temp_lpm_entry;
    trie_t       *bkt_trie = NULL;
    trie_t       *pfx_trie = NULL;
    trie_node_t  *delp = NULL;
    payload_t    *bkt_payload = NULL;
    payload_t    *pfx_payload = NULL;
    alpm_pivot_t *pivot_pyld = NULL;
    soc_mem_t    mem;
    defip_entry_t *def_entry = NULL;
    trie_node_t  *lpmp = NULL;
    payload_t    *tmp_pyld = NULL;

    /* Check function params */
    if ((NULL == lpm_entry) || (NULL == alpm_entry)) {
        return SOC_E_PARAM;
    }

    /* Check IPv6 */
    if (ipv6) {
        if (!(ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE1f))) {
            return (SOC_E_PARAM);
        }
    }

    /* Get VRF */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf));

    /* Get ALPM table memory. */
    SOC_ALPM_TABLE_MEM(unit, ipv6, mem, flex);

    /* Convert alpm_entry into lpm_entry to extract prefix */ 
    SOC_IF_ERROR_RETURN(_soc_alpm_lpm_ent_init(unit, alpm_entry,  mem, ipv6, 
                                               vrf_id, bckt_idx, tcam_idx,
                                               &temp_lpm_entry, 0));
   
    /* Create Prefix */
    rv = _alpm_prefix_create(unit, &temp_lpm_entry, prefix, &length, &default_route);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "prefix create failed\n")));
        return rv;
    }
   
    /* Get TCAM pivot */ 
    pivot_pyld = ALPM_TCAM_PIVOT(unit, tcam_idx);
    
    /* Get bucket trie from pivot */ 
    bkt_trie = PIVOT_BUCKET_TRIE(pivot_pyld);
   
    /* Allocate bucket payload */
    bkt_payload = sal_alloc(sizeof(payload_t), "Payload for Key");
    if (NULL == bkt_payload) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for trie node.\n")));
        return SOC_E_MEMORY;
    }

    /* Allocate prefix trie payload */
    pfx_payload = sal_alloc(sizeof(payload_t), "Payload for pfx trie key");
    if (NULL == pfx_payload) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for pfx trie node\n")));
        sal_free(bkt_payload);
        return SOC_E_MEMORY;
    }
  
    /* Clear allocated payloads */ 
    sal_memset(bkt_payload, 0, sizeof(*bkt_payload));
    sal_memset(pfx_payload, 0, sizeof(*pfx_payload));

    bkt_payload->key[0] = prefix[0]; 
    bkt_payload->key[1] = prefix[1]; 
    bkt_payload->key[2] = prefix[2]; 
    bkt_payload->key[3] = prefix[3]; 
    bkt_payload->key[4] = prefix[4]; 
    bkt_payload->len    = length;
    bkt_payload->index  = alpm_idx;
 
#ifdef ALPM_WARM_BOOT_DEBUG_EXTENSIVE
    LOG_ERROR(BSL_LS_SOC_ALPM,
              (BSL_META_U(unit,
                          "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, length = 0x%x alpm_idx = 0x%x\n"),
               bkt_payload->key[0], bkt_payload->key[1], bkt_payload->key[2],
               bkt_payload->key[3], bkt_payload->key[4],
               length, alpm_idx));
#endif

    /* Create duplicate for prefix trie */
    sal_memcpy(pfx_payload, bkt_payload, sizeof(*bkt_payload));
    pfx_payload->bkt_ptr = bkt_payload;

    /* Insert prefix into bucket-trie */
    rv = trie_insert(bkt_trie, prefix, NULL, length, 
                    (trie_node_t *)bkt_payload);
    if (SOC_FAILURE(rv)) {
        goto free_memory;
    }

    /* Insert prefix into prefix-trie */
    if (ipv6) {
        pfx_trie = VRF_PREFIX_TRIE_IPV6(unit, vrf);
    } else {
        pfx_trie = VRF_PREFIX_TRIE_IPV4(unit, vrf);
    }
    
    if (!default_route) {
        /* Default route already in prefix trie */
        rv = trie_insert(pfx_trie, prefix, NULL, length, 
                        (trie_node_t *)pfx_payload);
        if (SOC_FAILURE(rv)) {
            goto delete_bkt_trie;
        }
    } else {
        /* update the default route */
        lpmp = NULL;
        rv = trie_find_lpm(pfx_trie, 0, 0, &lpmp);
        tmp_pyld = (payload_t *)lpmp;
        if (SOC_SUCCESS(rv)) {
            tmp_pyld->bkt_ptr = bkt_payload;
        } else {
            goto delete_bkt_trie;
        }
    }

    /* Update bucket index for VRF default route entry */
    if (VRF_TRIE_INIT_COMPLETED(unit, vrf, ipv6)) {
        if (ipv6 == 0) {
            def_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(unit, vrf);
        } else {
            def_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(unit, vrf);
        }

        if (0 == soc_L3_DEFIPm_field32_get(unit, def_entry, ALG_BKT_PTR0f)) {
            default_route = 0;
            rv = _alpm_prefix_create(unit, lpm_entry, prefix,
                                     &length, &default_route);
            if (SOC_SUCCESS(rv) && default_route) {
                soc_L3_DEFIPm_field32_set(unit, def_entry,
                                          ALG_BKT_PTR0f, bckt_idx);
            }
        }
    }

    /* Return success */
    return rv;

delete_bkt_trie:
    (void) trie_delete(bkt_trie, prefix, length, &delp);
    bkt_payload = (payload_t *) delp;

free_memory:
    sal_free(bkt_payload);
    sal_free(pfx_payload);
    return rv;
}

/*
 * soc_alpm_warmboot_vrf_add
 */
static int
soc_alpm_warmboot_vrf_add(int unit, 
                          int v6,
                          int vrf,
                          int idx,
                          int bkt_ptr)
{

    int           rv = SOC_E_NONE;
    uint32        length;
    uint32        key[5] = {0};
    trie_t        *pfx_root = NULL;
    payload_t     *pfx_pyld = NULL;
    defip_entry_t *lpm_entry = NULL;

    /* Initialize VRF PIVOT IPV4 trie */
    lpm_entry = sal_alloc(sizeof(defip_entry_t), "Default LPM entry");
    if (lpm_entry == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "unable to allocate memory for LPM entry\n")));
        return SOC_E_MEMORY;
    }

    /* Initialize an lpm entry to perform a match */
    (void) _soc_alpm_lpm_ent_key_init(unit, key, 0, vrf, v6, lpm_entry, 1);

    /* for global VRF */
    if (vrf == SOC_VRF_MAX(unit) + 1) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, GLOBAL_ROUTE0f, 1);
    } else {
        /* Set default miss to global route only for non global routes */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, DEFAULT_MISS0f, 1);
    }

    if (v6 == 0) {
        VRF_TRIE_DEFAULT_ROUTE_IPV4(unit, vrf) = lpm_entry;
        trie_init(_MAX_KEY_LEN_48_, &VRF_PREFIX_TRIE_IPV4(unit, vrf));
        pfx_root = VRF_PREFIX_TRIE_IPV4(unit, vrf);
    } else {
        VRF_TRIE_DEFAULT_ROUTE_IPV6(unit, vrf) = lpm_entry;
        trie_init(_MAX_KEY_LEN_144_, &VRF_PREFIX_TRIE_IPV6(unit, vrf));
        pfx_root = VRF_PREFIX_TRIE_IPV6(unit, vrf);
    }
  
    pfx_pyld = sal_alloc(sizeof(payload_t), "Payload for pfx trie key");
    if (pfx_pyld == NULL) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for pfx trie node \n")));
        return SOC_E_MEMORY;
    }

    sal_memset(pfx_pyld,   0, sizeof(*pfx_pyld));

    length = 0;
    pfx_pyld->key[0] = key[0]; 
    pfx_pyld->key[1] = key[1]; 
    pfx_pyld->len    = length;

    rv = trie_insert(pfx_root, key, NULL, length, &(pfx_pyld->node));
    if (SOC_FAILURE(rv)) {
        sal_free(pfx_pyld);
        return rv;
    }

    VRF_TRIE_INIT_DONE(unit, vrf, v6, 1);
    
    return rv;
}
/*
 * soc_alpm_warmboot_prefix_insert
 */
int
soc_alpm_warmboot_prefix_insert(int unit, 
                                int  ipv6,
                                int  flex,
                                void *lpm_entry, 
                                void *alpm_entry, 
                                int tcam_idx, 
                                int bckt_idx, 
                                int alpm_idx)
{
    int       vrf_id;  /* Internal API representation of VRF id */
    int       vrf;      /* VRF in (0 - (MAX_VRF + 1)) range */
    int       rv = SOC_E_NONE; 

    tcam_idx = soc_alpm_physical_idx(unit, L3_DEFIPm, tcam_idx, ipv6);
 
    /* Get VRF */
    SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf));
    
    /* For VRF_OVERRIDE (Global High) entries,  Prefix resides in TCAM directly */
    if (vrf_id == SOC_L3_VRF_OVERRIDE) {
        return(rv);
    }
    /* Non override entries, goes into ALPM memory */
    if (!VRF_TRIE_INIT_COMPLETED(unit, vrf, ipv6)) {
        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(unit,
                                "VRF %d is not initialized\n"),
                     vrf));
        rv = soc_alpm_warmboot_vrf_add(unit, ipv6, vrf, tcam_idx, bckt_idx);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(unit,
                                  "VRF %d/%d trie init \n"
                                  "failed\n"), vrf, ipv6));
            return rv;
        }

        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                    (BSL_META_U(unit,
                                "VRF %d/%d trie init completed\n"),
                     vrf, ipv6));
    }
 
    rv = _soc_alpm_warmboot_insert(unit, ipv6, flex, lpm_entry, alpm_entry,
                                   tcam_idx, bckt_idx, alpm_idx);
    if (rv != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_ALPM,
                 (BSL_META_U(unit,
                             "unit %d : Route Insertion Failed :%s\n"),
                  unit, soc_errmsg(rv)));

        return(rv);
    }
    VRF_TRIE_ROUTES_INC(unit, vrf_id, vrf, ipv6);
    return(rv);
}

/*
 * alpm_warmboot_bucket_bitmap_set
 * This function will set bit position in bucket bitmap
 */
int  
soc_alpm_warmboot_bucket_bitmap_set(int u,       /* Unit         */
                                    int v6,      /* Flag v4/v6   */
                                    int bkt_idx) /* Bucket index */
{
    int step_count = 1;

    /*
     * In combined search mode, with urpf disabled, need to allocate even and 
     * odd bucket pointers
     */
    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        step_count = 2;
    }

    if (SOC_ALPM_BUCKET_BMAP(u) == NULL) {
        return SOC_E_INTERNAL;
    }

    /* Set bit position */
    SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(u), bkt_idx, step_count);
    
    return SOC_E_NONE;
}

int
soc_alpm_warmboot_lpm_reinit_done(int unit)
{
    int idx;
    int prev_idx = ALPM_MAX_PFX_INDEX;

    int defip_table_size = soc_mem_index_count(unit, L3_DEFIPm);
    
    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    } 
    
    /* returns 0 for combined search mode and 1 for parallel search mode */
    if (!soc_alpm_mode_get(unit)) {
        /* Combined search mode */
        SOC_ALPM_LPM_STATE_PREV(unit, ALPM_MAX_PFX_INDEX) = -1;

        for (idx = ALPM_MAX_PFX_INDEX; idx > -1; idx--) {
            if (-1 == SOC_ALPM_LPM_STATE_START(unit, idx)) {
                continue;
            }

            SOC_ALPM_LPM_STATE_PREV(unit, idx) = prev_idx;
            SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = idx;

            SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                    \
                              SOC_ALPM_LPM_STATE_START(unit, idx) -      \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;
            prev_idx = idx;
            
        }

        SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = -1;
        SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                       \
                              defip_table_size -                   \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;
    } else {
        /* Parallel search mode 
         * 
         *    Configured VRF Range 
         *    {0 - (ALPM_MAX_PFX_INDEX/3)}
         *    Global VRF Range 
         *    {((ALPM_MAX_PFX_INDEX/1) + 1) - (ALPM_MAX_PFX_INDEX/2) }
         *    Override VRF Range 
         *    {((ALPM_MAX_PFX_INDEX/2) + 1) - (ALPM_MAX_PFX_INDEX) }
         */

        /*
         * Global and Overide VRF range are treated as single block and
         * both blocks will be linked 
         */
        SOC_ALPM_LPM_STATE_PREV(unit, ALPM_MAX_PFX_INDEX) = -1;
        for (idx = ALPM_MAX_PFX_INDEX; idx > (ALPM_MAX_PFX_INDEX/3) ; idx--) {
            if (-1 == SOC_ALPM_LPM_STATE_START(unit, idx)) {
                continue;
            }

            SOC_ALPM_LPM_STATE_PREV(unit, idx) = prev_idx;
            SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = idx;

            SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                    \
                              SOC_ALPM_LPM_STATE_START(unit, idx) -      \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;
                              
            prev_idx = idx;
        }
        SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = -1;
        SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                       \
                              defip_table_size -                        \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;

        /* Configured VRF range */
        prev_idx = (ALPM_MAX_PFX_INDEX/3);
        SOC_ALPM_LPM_STATE_PREV(unit, (ALPM_MAX_PFX_INDEX/3)) = -1;
        for (idx = ((ALPM_MAX_PFX_INDEX/3)-1); idx > -1; idx--) {
            if (-1 == SOC_ALPM_LPM_STATE_START(unit, idx)) {
                continue;
            }

            SOC_ALPM_LPM_STATE_PREV(unit, idx) = prev_idx;
            SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = idx;

            SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                    \
                              SOC_ALPM_LPM_STATE_START(unit, idx) -      \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;
            prev_idx = idx;
        }
        SOC_ALPM_LPM_STATE_NEXT(unit, prev_idx) = -1;
        SOC_ALPM_LPM_STATE_FENT(unit, prev_idx) =                         \
                              (defip_table_size >> 1) -                   \
                              SOC_ALPM_LPM_STATE_END(unit, prev_idx) - 1;
    }

    return (SOC_E_NONE);
}

int
soc_alpm_warmboot_lpm_reinit(int unit,
                             int ipv6,
                             int idx,
                             void *lpm_entry)
{
    int pfx_len;
    int flex;
    defip_entry_t lpm_entry_hi;
    int vrf_id, vrf;
    
    if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f) ||
        soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID1f)) {
    
            LPM_HASH_INSERT(unit, lpm_entry, idx, NULL, NULL);
    }

    if (ipv6) {
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
            SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf));
            if (!VRF_FLEX_COMPLETED(unit, vrf_id, vrf, ipv6)) {
                flex = soc_L3_DEFIPm_field32_get(unit, lpm_entry, ENTRY_VIEW0f);
                VRF_FLEX_SET(unit, vrf_id, vrf, ipv6, flex);
            }
            VRF_PIVOT_REF_INC(unit, vrf_id, vrf, ipv6);
        }
    } else {
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
            SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf));
            if (!VRF_FLEX_COMPLETED(unit, vrf_id, vrf, ipv6)) {
                flex = soc_L3_DEFIPm_field32_get(unit, lpm_entry, ENTRY_VIEW0f);
                VRF_FLEX_SET(unit, vrf_id, vrf, ipv6, flex);
            }
            VRF_PIVOT_REF_INC(unit, vrf_id, vrf, ipv6);
        }
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID1f)) {
            soc_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, &lpm_entry_hi, TRUE);
            SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(unit, &lpm_entry_hi, &vrf_id, &vrf));
            if (!VRF_FLEX_COMPLETED(unit, vrf_id, vrf, ipv6)) {
                flex = soc_L3_DEFIPm_field32_get(unit, lpm_entry, ENTRY_VIEW1f);
                VRF_FLEX_SET(unit, vrf_id, vrf, ipv6, flex);
            }
            VRF_PIVOT_REF_INC(unit, vrf_id, vrf, ipv6);
        }
    }

    if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
        
        SOC_IF_ERROR_RETURN
            (_soc_alpm_lpm_prefix_length_get(unit, lpm_entry, &pfx_len));

        if (SOC_ALPM_LPM_STATE_VENT(unit, pfx_len) == 0) {
            SOC_ALPM_LPM_STATE_START(unit, pfx_len) = idx;
            SOC_ALPM_LPM_STATE_END(unit, pfx_len) = idx;
        } else {
            SOC_ALPM_LPM_STATE_END(unit, pfx_len) = idx;
        }

        SOC_ALPM_LPM_STATE_VENT(unit, pfx_len)++;

        if (ipv6) {
            /* IPv6 entry */
            return (SOC_E_NONE);
        }
    } else {
        /* Check upper half of lpm_entry */
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID1f)) {
            if (ipv6) {
                return SOC_E_NONE;
            }

            /* Get prefix length */
            SOC_IF_ERROR_RETURN
                (_soc_alpm_lpm_prefix_length_get(unit, &lpm_entry_hi, &pfx_len));

            if (SOC_ALPM_LPM_STATE_VENT(unit, pfx_len) == 0) {
                SOC_ALPM_LPM_STATE_START(unit, pfx_len) = idx;
                SOC_ALPM_LPM_STATE_END(unit, pfx_len) = idx;
            } else {
                SOC_ALPM_LPM_STATE_END(unit, pfx_len) = idx;
            }
      
            SOC_ALPM_LPM_STATE_VENT(unit, pfx_len)++;
        }
    } 
    return (SOC_E_NONE);
}

/* Helper structure for Debug Counter */
typedef struct _alpm_dbg_cnt_s {
    int v4;         /* Counter for IPv4 */
    int v6_64;      
    int v6_128;     
    int v4t;        /* Totaled counter for IPv4 per VRF */
    int v6_64t;
    int v6_128t;
    int used;
} _alpm_dbg_cnt_t;

typedef enum _alpm_pvt_cnt_type_e {
    SOC_ALPM_DEBUG_CNT_ADD = 0,
    SOC_ALPM_DEBUG_CNT_DEL,
    SOC_ALPM_DEBUG_CNT_SPLIT,
    SOC_ALPM_DEBUG_CNT_USED,
    SOC_ALPM_DEBUG_CNT_LPM_SHIFT,
    SOC_ALPM_DEBUG_CNT_LPM_FULL
} _alpm_pvt_cnt_type_t;


#define _ALPM_DBG_CNT_ADD(dst_dc, src_dc)       \
    do {                                        \
        (dst_dc)->v4 += (src_dc)->v4;           \
        (dst_dc)->v4t += (src_dc)->v4t;         \
        (dst_dc)->v6_64 += (src_dc)->v6_64;     \
        (dst_dc)->v6_64t += (src_dc)->v6_64t;   \
        (dst_dc)->v6_128 += (src_dc)->v6_128;   \
        (dst_dc)->v6_128t += (src_dc)->v6_128t; \
    } while (0)

/* Common debug counter print */
#define PRINT_SPLIT_LINE(u)    \
    LOG_CLI((BSL_META_U(u, "-----------------------------------------------\n")))
#define PRINT_VRF_HEADER(u)    \
    LOG_CLI((BSL_META_U(u, "\n      VRF  v4      v6-64   v6-128  |   AVG\n")))
#define PRINT_VRF_NOT_FOUND(u) \
    LOG_CLI((BSL_META_U(u, "%9s\n"), "Specific VRF not found"))

/* Print Bucket Occupancy */
#define BKT_PERCENT(used, tot) \
    (tot) ? (used) * 100 / (tot) : 0, (tot) ? (used) * 1000 / (tot) % 10 : 0
#define PRINT_BKT_OCCU_ITEM(u, vrf, tmp_dc) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"), \
                 (vrf), BKT_PERCENT(tmp_dc->v4, tmp_dc->v4t), \
                 BKT_PERCENT(tmp_dc->v6_64, tmp_dc->v6_64t), \
                 BKT_PERCENT(tmp_dc->v6_128, tmp_dc->v6_128t), \
                 BKT_PERCENT((tmp_dc->v4+tmp_dc->v6_64+tmp_dc->v6_128),\
                 (tmp_dc->v4t+tmp_dc->v6_64t+tmp_dc->v6_128t)), \
                 (vrf) == SOC_VRF_MAX(u)+1 ? "GLo" : "")); \
    } while (0)
#define PRINT_BKT_OCCU_ITEM_TOT(u, all_dc) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"), \
                 "AVG", BKT_PERCENT((all_dc)->v4, (all_dc)->v4t), \
                 BKT_PERCENT((all_dc)->v6_64, (all_dc)->v6_64t), \
                 BKT_PERCENT((all_dc)->v6_128, (all_dc)->v6_128t), \
                 BKT_PERCENT(((all_dc)->v4+(all_dc)->v6_64+(all_dc)->v6_128),\
                 ((all_dc)->v4t+(all_dc)->v6_64t+(all_dc)->v6_128t)))); \
    } while (0)

/* Print VRF based debug counter */
#define PRINT_VRF_ITEM(u, vrf, v4, v6_64, v6_128, total) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "%9d  %-7d %-7d %-7d |   %-7d %s\n"), \
                 (vrf == MAX_VRF_ID ? -1 : vrf), (v4), (v6_64),\
                 (v6_128), (total), (vrf) == MAX_VRF_ID ? "GHi" : \
                 (vrf) == SOC_VRF_MAX(u)+1 ? "GLo" : "")); \
    } while (0)
#define PRINT_VRF_ITEM_TOT(u, v4, v6_64, v6_128, total) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "%9s  %-7d %-7d %-7d |   %-7d \n"), "Total", (v4), (v6_64),\
                 (v6_128), (total))); \
    } while (0)

#define PRINT_BUCKET_HEADER(u) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "\n  BKT/VRF  Min     Max     Cur     |   Comment\n"))); \
    } while (0)
#define PRINT_BUCKET_ITEM(u, bkt, vrf, min, max, cur, def) \
    do { \
        LOG_CLI((BSL_META_U(u, \
                            "%5d/%-4d %-7d %-7d %-7d |   %-7s\n"), bkt, vrf, \
                 min, max, cur, def ? "Def" : \
                 (vrf) == SOC_VRF_MAX(u)+1 ? "GLo" : "")); \
    } while (0)

/* Get max available IPv4, IPv6-64, IPv6-128 entry numbers */
static void
_soc_alpm_pivot_max_get(int u, alpm_vrf_counter_t *pvt_max)
{
    pvt_max->v4 = soc_mem_index_count(u, L3_DEFIPm) * 2;
    pvt_max->v6_128 = soc_mem_index_count(u, L3_DEFIP_PAIR_128m);

    if (SOC_ALPM_128B_ENABLE(u)) {
        pvt_max->v6_64 = pvt_max->v6_128;
    } else {
        pvt_max->v6_64 = pvt_max->v4 >> 1;
    }
    
    if (SOC_URPF_STATUS_GET(u)) {
        pvt_max->v4 >>= 1;
        pvt_max->v6_128 >>= 1;
        pvt_max->v6_64 >>= 1;
    }
}

static void
_soc_alpm_pivot_debug_dump(int u, int vrf_id, alpm_vrf_handle_t *vh,
                                   _alpm_pvt_cnt_type_t pct)
{
    alpm_vrf_counter_t *vc;
    int i, tv4c, tv6_64c, tv6_128c;
    int found = 0;

    alpm_vrf_counter_t pvt_max;

    /* Print Pivot debug info header */
    switch (pct) {
    case SOC_ALPM_DEBUG_CNT_ADD:
        LOG_CLI((BSL_META_U(u,
                            "\nAdd Counter:\n")));
        break;
    case SOC_ALPM_DEBUG_CNT_DEL:
        LOG_CLI((BSL_META_U(u,
                            "\nDelete Counter:\n"))); 
        break;
    case SOC_ALPM_DEBUG_CNT_SPLIT:
        LOG_CLI((BSL_META_U(u,
                            "\nInternal Debug Counter - Split:\n")));
        break;
    case SOC_ALPM_DEBUG_CNT_USED:
        _soc_alpm_pivot_max_get(u, &pvt_max);
        LOG_CLI((BSL_META_U(u,
                            "\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"), 
                 pvt_max.v4, pvt_max.v6_64, pvt_max.v6_128));        
        break;
    case SOC_ALPM_DEBUG_CNT_LPM_SHIFT:
        LOG_CLI((BSL_META_U(u,
                            "\nInternal Debug Counter - LPM Shift:\n"))); 
        break;
    case SOC_ALPM_DEBUG_CNT_LPM_FULL:
        LOG_CLI((BSL_META_U(u,
                            "\nInternal Debug Counter - LPM Full:\n"))); 
        break;
    default:
        break;
    }
    PRINT_VRF_HEADER(u);
    PRINT_SPLIT_LINE(u); 

    tv4c = tv6_64c = tv6_128c = 0;    
    for (i = 0; i < MAX_VRF_ID + 1; i++) {
        int v4c, v6_64c, v6_128c;
        if (vh[i].init_done == 0 && i != MAX_VRF_ID) {
            continue;
        }
        if (vrf_id != -1 && vrf_id != i) {
            continue;
        }

        found = 1;

        switch (pct) {
        case SOC_ALPM_DEBUG_CNT_ADD:
            vc = &vh[i].add;
            break;
        case SOC_ALPM_DEBUG_CNT_DEL:
            vc = &vh[i].del;
            break;
        case SOC_ALPM_DEBUG_CNT_SPLIT:
            vc = &vh[i].bkt_split;
            break;
        case SOC_ALPM_DEBUG_CNT_LPM_SHIFT:
            vc = &vh[i].lpm_shift;
            break;
        case SOC_ALPM_DEBUG_CNT_LPM_FULL:
            vc = &vh[i].lpm_full;
            break;            
        case SOC_ALPM_DEBUG_CNT_USED:
            vc = &vh[i].pivot_used;
            break;
        default:
            vc = &vh[i].pivot_used;
            break;
        }
            
        v4c = vc->v4;
        v6_64c = vc->v6_64;
        v6_128c = vc->v6_128;

        tv4c += v4c;
        tv6_64c += v6_64c;
        tv6_128c += v6_128c;

        PRINT_VRF_ITEM(u, i, v4c, v6_64c, v6_128c, (v4c+v6_64c+v6_128c));
    }
    if (found == 0) {
        PRINT_VRF_NOT_FOUND(u);
    } else {        
        PRINT_SPLIT_LINE(u);
        PRINT_VRF_ITEM_TOT(u, tv4c, tv6_64c, tv6_128c, (tv4c+tv6_64c+tv6_128c));
    }

    return ;
}

int
soc_alpm_debug_show(int u, int vrf_id, uint32 flags)
{
    int i, alloc_sz, found = 0;
    _alpm_dbg_cnt_t *alpm_dc_arr;
    _alpm_dbg_cnt_t bkt_ent_max;
    _alpm_dbg_cnt_t all_dc;

    if (vrf_id > (SOC_VRF_MAX(u) + 1)) {
        return SOC_E_PARAM;
    }

    alloc_sz = MAX_VRF_ID * sizeof(_alpm_dbg_cnt_t);
    alpm_dc_arr = sal_alloc(alloc_sz, "_alpm_dbg_cnt");
    if (alpm_dc_arr == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(alpm_dc_arr, 0, alloc_sz);

    /* Collect data */
    bkt_ent_max.v4 = ALPM_IPV4_BKT_COUNT;
    bkt_ent_max.v6_64 = ALPM_IPV6_64_BKT_COUNT;
    bkt_ent_max.v6_128 = ALPM_IPV6_128_BKT_COUNT;    

    if (!soc_alpm_mode_get(u) && !SOC_URPF_STATUS_GET(u)) {
        bkt_ent_max.v6_64  <<= 1;
        bkt_ent_max.v6_128 <<= 1;
    }

    /* 1. Bucket Occupancy */
    if (flags & (SOC_ALPM_DEBUG_SHOW_FLAG_BKT)) {
        LOG_CLI((BSL_META_U(u,
                            "\nBucket Occupancy:\n")));
        PRINT_BUCKET_HEADER(u);
        PRINT_SPLIT_LINE(u);
    }
    for (i = 0; i < MAX_PIVOT_COUNT; i++) {
        alpm_pivot_t *p = ALPM_TCAM_PIVOT(u, i);
        if (p != NULL) {
            _alpm_dbg_cnt_t *tmp_dc;
            int vrf = PIVOT_BUCKET_VRF(p);

            if (vrf < 0 || vrf > (SOC_VRF_MAX(u) + 1)) {
                continue;
            }
            
            if (vrf_id != -1 && vrf_id != vrf) {
                continue;
            }

            if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_BKT) {
                found = 1;
                PRINT_BUCKET_ITEM(u, i, vrf, 
                                  PIVOT_BUCKET_MIN(p), 
                                  PIVOT_BUCKET_MAX(p), 
                                  PIVOT_BUCKET_COUNT(p), 
                                  PIVOT_BUCKET_DEF(p));
            }

            tmp_dc = &alpm_dc_arr[vrf];
            if (PIVOT_BUCKET_IPV6(p) == L3_DEFIP_MODE_128) {
                tmp_dc->v6_128 += PIVOT_BUCKET_COUNT(p);
                tmp_dc->v6_128t += bkt_ent_max.v6_128;
            } else if (PIVOT_BUCKET_IPV6(p) == L3_DEFIP_MODE_64) {
                tmp_dc->v6_64 += PIVOT_BUCKET_COUNT(p);
                tmp_dc->v6_64t += bkt_ent_max.v6_64;
            } else {
                tmp_dc->v4 += PIVOT_BUCKET_COUNT(p);
                tmp_dc->v4t += bkt_ent_max.v4;
            }
            tmp_dc->used = TRUE;
        }
    }

    if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_BKT) {
        if (found == 0) {
            PRINT_VRF_NOT_FOUND(u);
        }
    }

    sal_memset(&all_dc, 0, sizeof(all_dc));
    found = 0;

    if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG) {
        LOG_CLI((BSL_META_U(u,
                            "\nBucket Usage:\n")));
        PRINT_VRF_HEADER(u);
        PRINT_SPLIT_LINE(u);
        for (i = 0; i < MAX_VRF_ID; i++) {
            _alpm_dbg_cnt_t *tmp_dc;
            if (alpm_dc_arr[i].used != TRUE) {
                continue;
            }
            if (vrf_id != -1 && vrf_id != i) {
                continue;
            }

            found = 1;
            tmp_dc = &alpm_dc_arr[i];
            _ALPM_DBG_CNT_ADD(&all_dc, tmp_dc);
            PRINT_BKT_OCCU_ITEM(u, i, tmp_dc);
        }
        if (found == 0) {
            PRINT_VRF_NOT_FOUND(u);
        } else {
            PRINT_SPLIT_LINE(u);
            PRINT_BKT_OCCU_ITEM_TOT(u, &all_dc);
        }
    }

    /* 2. Pivot Occupancy */
    if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_PVT) {
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                           SOC_ALPM_DEBUG_CNT_USED);
    }

    /* 3. Add/Delete counters per VRF/ip type */
    if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_CNT) {
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                   SOC_ALPM_DEBUG_CNT_ADD);
        
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                   SOC_ALPM_DEBUG_CNT_DEL);
    }

    /* 4. Total No. of bucket splits per VRF/ip type */
    if (flags & SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG) {
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                   SOC_ALPM_DEBUG_CNT_SPLIT);
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                   SOC_ALPM_DEBUG_CNT_LPM_FULL);
        _soc_alpm_pivot_debug_dump(u, vrf_id, alpm_vrf_handle[u], 
                                   SOC_ALPM_DEBUG_CNT_LPM_SHIFT);        
    }

    sal_free(alpm_dc_arr);
    
    return SOC_E_NONE;
}

void
soc_alpm_debug_cntr_show(int u)
{
    LOG_CLI(("====ALPM=Internal=Debug=Counter====\n"));
    LOG_CLI(("%-24s%d\n", "Bucket Split:", soc_alpm_dbg_cnt[u].bkt_split));
    LOG_CLI(("%-24s%d\n", "Bucket Merge:", soc_alpm_dbg_cnt[u].bkt_merge));
    LOG_CLI(("%-24s%d\n", "Bucket MergeDuringRp:", soc_alpm_dbg_cnt[u].bkt_merge2));
    LOG_CLI(("%-24s%d\n", "Bucket Repartition:", soc_alpm_dbg_cnt[u].bkt_repart));
    LOG_CLI(("%-24s%d\n", "Propagation Insert CB:",
            soc_alpm_dbg_cnt[u].ppg_cb_insert));
    LOG_CLI(("%-24s%d\n", "Propagation Delete CB:",
            soc_alpm_dbg_cnt[u].ppg_cb_delete));
}

int
soc_alpm_debug_brief_show(int u, int phy_bkt, int index)
{
    int i = 0, j = 0, rv = SOC_E_NONE;
    uint32 rval = 0, alpm = 0, mode = 0;
    uint32 urpf = 0, bit128 = 0, fv = 0, bnkbits = 0xf;
    int v4used = 0, v4max = 0;
    int v6_64used = 0, v6_64max = 0;
    int v6_128used = 0, v6_128max = 0;
    int bucket_used = 0, bucket_max = 0;
    char *lpm_tbl_ptr = NULL;
    defip_entry_t *lpm_entry = NULL;
    defip_pair_128_entry_t *lpm_entry_p128 = NULL;
    int ipv6, bkt_ptr, gh, gr, use_alpm;
    int *bktref = NULL;
    int glb_hi_cnt = 0, glb_lo_cnt = 0;
    soc_mem_t lpm_mem;
    int alloc_size = 0;
    uint32 e[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t fldv, fldb, fldg, fldr;
    int alpm_bkt_bits = SOC_ALPM_BUCKET_BITS(u);

    char *mode_str[] = {
        "LPM",
        "ALPM-Combined",
        "ALPM-Parallel",
        "ALPM-Invalid",
        "ALPM-Invalid"
    };

    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(u, &rval));
    alpm = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, LPM_MODEf);
    if (alpm != 0) {
        mode = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, LOOKUP_MODEf);
    }
    urpf = soc_reg_field_get(u, L3_DEFIP_RPF_CONTROLr, rval, DEFIP_RPF_ENABLEf);

    LOG_CLI(("\n"));
    LOG_CLI(("%-24s%s\n", "Mode:", mode_str[alpm + mode]));
    LOG_CLI(("%-24s%s\n", "uRPF:", urpf ? "Enable" : "Disable"));
    bit128 = !!soc_mem_index_count(u, L3_DEFIP_PAIR_128m);
    LOG_CLI(("%-24s%d\n", "IPv6 Bits:", bit128 ? 128 : 64));

    if (alpm == 0) {
        return SOC_E_NONE;
    }
    LOG_CLI(("%-24s%s\n", "IPv4 Wide:", "Single"));

    if (mode == 0 && !urpf) {
        fv = 1;
    } else {
        fv = 0;
    }
    LOG_CLI(("%-24s%s\n", "IPv6 Wide:", fv ? "Double" : "Single"));

    LOG_CLI(("%-24s%d\n", "Banks:", 4));

    /* Calculate utilization */
    v4max = 2 * soc_mem_index_count(u, L3_DEFIPm);
    v6_64max = bit128 ? soc_mem_index_count(u, L3_DEFIP_PAIR_128m) :
                        soc_mem_index_count(u, L3_DEFIPm);
    v6_128max = soc_mem_index_count(u, L3_DEFIP_PAIR_128m);
    bucket_max = SOC_TD2_ALPM_MAX_BKTS;
    if (urpf) {
        v4max       >>= 1;
        v6_64max    >>= 1;
        v6_128max   >>= 1;
        bucket_max  >>= 1;
    }

    bktref = sal_alloc(bucket_max * sizeof(int), "bktref");
    if (bktref == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(bktref, 0, bucket_max * sizeof(int));

    /* DMA LPM table to software copy */
    lpm_mem = L3_DEFIPm;
    alloc_size = WORDS2BYTES(soc_mem_entry_words(u, lpm_mem)) *
                 soc_mem_index_count(u, lpm_mem);
    lpm_tbl_ptr = soc_cm_salloc(u, alloc_size, "lpm_tbl");
    if (lpm_tbl_ptr == NULL) {
        rv = SOC_E_MEMORY;
        goto _free_mem;
    }
    /* Reset allocated buffer. */
    sal_memset(lpm_tbl_ptr, 0, alloc_size);

    /* Read table to the buffer. */
    if (v4max && soc_mem_read_range(u, lpm_mem, MEM_BLOCK_ANY,
                           soc_mem_index_min(u, lpm_mem),
                           soc_mem_index_max(u, lpm_mem), lpm_tbl_ptr) < 0) {
        rv = (SOC_E_INTERNAL);
        goto _free_mem;
    }

    for (i = 0; i < (v4max / 2); i++) {
        /* Calculate entry offset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(u, lpm_mem,
                                         defip_entry_t *, lpm_tbl_ptr, i);
        ipv6 = soc_mem_field32_get(u, lpm_mem, lpm_entry, MODE0f);

        for (j = 0; j < (ipv6 ? 1 : 2); j++) {
            if (j == 1) {
                fldv = VALID1f;
            } else {
                fldv = VALID0f;
            }
            /* Make sure entry is valid. */
            if (!soc_mem_field32_get(u, lpm_mem, lpm_entry, fldv)) {
                continue;
            }

            if (ipv6) {
                v6_64used++;
            } else {
                v4used++;
            }

            if (j == 0) {
                fldg = GLOBAL_HIGH0f;
                fldb = ALG_BKT_PTR0f;
                fldr = GLOBAL_ROUTE0f;
            } else {
                fldg = GLOBAL_HIGH1f;
                fldb = ALG_BKT_PTR1f;
                fldr = GLOBAL_ROUTE1f;
            }

            gh = soc_mem_field32_get(u, lpm_mem, lpm_entry, fldg);
            gr = soc_mem_field32_get(u, lpm_mem, lpm_entry, fldr);
            if (gr) {
                if (gh) {
                    glb_hi_cnt++;
                    use_alpm = 0;
                } else {
                    glb_lo_cnt++;
                    use_alpm = 1;
                }
            } else {
                use_alpm = 1;
            }

            if (use_alpm) {
                bkt_ptr = soc_mem_field32_get(u, lpm_mem, lpm_entry, fldb);
                if (bkt_ptr > bucket_max) {
                    LOG_CLI(("Invalid phy_bkt pointer in %s index %d\n",
                             SOC_MEM_NAME(u, lpm_mem), i));
                } else {
                    if (bktref[bkt_ptr]) {
                        LOG_CLI(("Invalid bucket sharing in %s index %d\n",
                                 SOC_MEM_NAME(u, lpm_mem), i));
                    } else {
                        bktref[bkt_ptr]++;
                        bucket_used++;
                    }
                }
            }
        }
    }

    soc_cm_sfree(u, lpm_tbl_ptr);

    /* l3_defip_pair_128 */
    lpm_tbl_ptr = NULL;
    lpm_mem = L3_DEFIP_PAIR_128m;
    alloc_size = WORDS2BYTES(soc_mem_entry_words(u, lpm_mem)) *
                 soc_mem_index_count(u, lpm_mem);
    lpm_tbl_ptr = soc_cm_salloc(u, alloc_size, "lpm_tbl_p128");
    if (lpm_tbl_ptr == NULL) {
        rv = SOC_E_MEMORY;
        goto _free_mem;
    }
    /* Reset allocated buffer. */
    sal_memset(lpm_tbl_ptr, 0, alloc_size);

    /* Read table to the buffer. */
    if (v6_128max && soc_mem_read_range(u, lpm_mem, MEM_BLOCK_ANY,
                           soc_mem_index_min(u, lpm_mem),
                           soc_mem_index_max(u, lpm_mem), lpm_tbl_ptr) < 0) {
        rv = (SOC_E_INTERNAL);
        goto _free_mem;
    }

    for (i = 0; i < v6_128max; i++) {
        /* Calculate entry offset */
        lpm_entry_p128 =
            soc_mem_table_idx_to_pointer(u, lpm_mem,
                                defip_pair_128_entry_t *, lpm_tbl_ptr, i);
        /* Verify if read LPM entry is IPv6-128 entry */
        if (0x03 != soc_mem_field32_get(u, lpm_mem, lpm_entry_p128, MODE1_UPRf)) {
            continue;
        };

        /* Verify if LPM entry is valid */
        if (!soc_mem_field32_get(u, lpm_mem, lpm_entry_p128,
                                    VALID1_LWRf) ||
            !soc_mem_field32_get(u, lpm_mem, lpm_entry_p128,
                                    VALID0_LWRf) ||
            !soc_mem_field32_get(u, lpm_mem, lpm_entry_p128,
                                    VALID1_UPRf) ||
            !soc_mem_field32_get(u, lpm_mem, lpm_entry_p128,
                                    VALID0_UPRf)) {
            continue;
        };

        v6_128used++;

        gh = soc_mem_field32_get(u, lpm_mem, lpm_entry_p128, GLOBAL_HIGHf);
        gr = soc_mem_field32_get(u, lpm_mem, lpm_entry_p128, GLOBAL_ROUTEf);
        if (gr) {
            if (gh) {
                glb_hi_cnt++;
                use_alpm = 0;
            } else {
                glb_lo_cnt++;
                use_alpm = 1;
            }
        } else {
            use_alpm = 1;
        }

        if (use_alpm) {
            bkt_ptr = soc_mem_field32_get(u, lpm_mem, lpm_entry_p128, ALG_BKT_PTRf);
            if (bkt_ptr > bucket_max) {
                LOG_CLI(("Invalid phy_bkt pointer in %s index %d\n",
                         SOC_MEM_NAME(u, lpm_mem), i));
            } else {
                if (bktref[bkt_ptr]) {
                    LOG_CLI(("Invalid bucket sharing in %s index %d\n",
                             SOC_MEM_NAME(u, lpm_mem), i));
                } else {
                    bktref[bkt_ptr]++;
                    bucket_used++;
                }
            }
        }
    }

    LOG_CLI(("\n"));
    LOG_CLI(("%-24s%d/%-4d\n", "IPv4 Pivots:", v4used, v4max));
    LOG_CLI(("%-24s%d/%-4d\n", "IPv6-64 Pivots:", v6_64used, v6_64max));
    LOG_CLI(("%-24s%d/%-4d\n", "IPv6-128 Pivots:", v6_128used, v6_128max));
    LOG_CLI(("%-24s%d/%-4d\n", "Buckets:", bucket_used, bucket_max));
    LOG_CLI(("%-24s%d\n", "Global High:", glb_hi_cnt));
    LOG_CLI(("%-24s%d\n", "Global Low:", glb_lo_cnt));
    LOG_CLI(("\n"));
    soc_alpm_debug_cntr_show(u);
    LOG_CLI(("\n\n"));

_free_mem:
    if (bktref) {
        sal_free(bktref);
        bktref = NULL;
    }

    if (lpm_tbl_ptr) {
        soc_cm_sfree(u, lpm_tbl_ptr);
        lpm_tbl_ptr = NULL;
    }

    if (phy_bkt != -1) {
        int bank, entmax, tmp;

        lpm_mem = _soc_trident2_alpm_bkt_view_get(u, phy_bkt << 2);
        LOG_CLI(("Bucket = %4d (View %s)\n", phy_bkt, lpm_mem == -1 ? "INVALID" :
                SOC_MEM_NAME(u, lpm_mem)));
        LOG_CLI(("------------------------------------------------------------\n"));
        LOG_CLI(("Index distribution (* indicates occupied)\n"));

        switch (lpm_mem) {
        case L3_DEFIP_ALPM_IPV4m:
            entmax = 6;
            break;
        case L3_DEFIP_ALPM_IPV4_1m:
            entmax = 4;
            break;
        case L3_DEFIP_ALPM_IPV6_64m:
            entmax = 4;
            break;
        case L3_DEFIP_ALPM_IPV6_64_1m:
            entmax = 3;
            break;
        case L3_DEFIP_ALPM_IPV6_128m:
            entmax = 2;
            break;
        default:
            lpm_mem = L3_DEFIP_ALPM_IPV4m;
            entmax = 6;
            break;
        }

        for (bank = 0; bank < 4; bank++) {
            if (!(bnkbits & (1 << bank))) {
                continue;
            }

            LOG_CLI(("Bank%d: ", bank));
            for (i = 0; i < entmax; i++) {
                int rv1;
                tmp = (_shr_popcount(bnkbits) + 1) / 2;
                j = (i << (tmp + alpm_bkt_bits)) |
                    (phy_bkt << tmp) | bank;
                rv1 = _soc_mem_alpm_read(u, lpm_mem, MEM_BLOCK_ANY, j, e);
                if (rv1 == 0 &&
                    soc_mem_field32_get(u, lpm_mem, e, VALIDf)) {
                    tmp = 1;
                } else {
                    tmp = 0;
                }
                if (tmp) {
                    LOG_CLI(("%d(*) ", j));
                } else {
                    LOG_CLI(("%d( ) ", j));
                }
            }
            LOG_CLI(("\n"));
        }
    }

    LOG_CLI(("\n"));
    if (index != -1) {
        int log_bkt;
        LOG_CLI(("Index = %8d\n", index));
        LOG_CLI(("------------------------------------------------------------\n"));
        lpm_mem = _soc_trident2_alpm_bkt_view_get(u, index);
        LOG_CLI(("%-24s%s\n", "Type", lpm_mem == -1 ? "INVALID" :
                SOC_MEM_NAME(u, lpm_mem)));
        LOG_CLI(("%-24s%d\n", "Bucket:", (index >> 2) & 0x3fff));
        LOG_CLI(("%-24s%d\n", "Bank:", index & (_shr_popcount(bnkbits) - 1)));
        log_bkt = (_shr_popcount(bnkbits) + 1) / 2 + alpm_bkt_bits;
        LOG_CLI(("%-24s%d\n", "Entry:", index >> log_bkt));
        LOG_CLI(("\n"));
    }

    return rv;
}

/*
 * Function:
 *     soc_alpm_lpm_sw_dump
 * Purpose:
 *     Displays FB LPM information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
soc_alpm_lpm_sw_dump(int unit)
{
    soc_alpm_lpm_state_p  lpm_state;
    int                   i, j;
    alpm_pivot_t          *pivot_pyld = NULL;
    _soc_alpm_lpm_hash_t  *hash;
    int idx;

    /* Dump v4/v6 lpm state */
    LOG_CLI((BSL_META_U(unit, "V4 LPM STATE DUMP.\n")));
    lpm_state = soc_alpm_lpm_state[unit];

    if (lpm_state != NULL) {
        for (i = 0; i < ALPM_MAX_PFX_ENTRIES; i++) {
            if (lpm_state[i].vent != 0) {
                LOG_CLI((BSL_META_U(unit, "Prefix %d\n"), i));
                LOG_CLI((BSL_META_U(unit, "  Start : %d\n"), lpm_state[i].start));
                LOG_CLI((BSL_META_U(unit, "  End   : %d\n"), lpm_state[i].end));
                LOG_CLI((BSL_META_U(unit, "  Prev  : %d\n"), lpm_state[i].prev));
                LOG_CLI((BSL_META_U(unit, "  Next  : %d\n"), lpm_state[i].next));
                LOG_CLI((BSL_META_U(unit, "  Valid : %d\n"), lpm_state[i].vent));
                LOG_CLI((BSL_META_U(unit, "  Free  : %d\n"), lpm_state[i].fent));
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "V4 LPM STATE DUMP DONE.\n\n")));

    /* Dump LPM HASH table */
    LOG_CLI((BSL_META_U(unit, "LPM HASH TABLE DUMP.\n")));
    hash = SOC_ALPM_LPM_STATE_HASH(unit);
    for(idx = 0; idx < hash->index_count; idx++) {
        j = 0;
        if (hash->table[idx] != TD2_ALPM_HASH_INDEX_NULL) {
            LOG_CLI((
                BSL_META_U(unit, "\nhash_idx = 0x%04x Data = 0x%04x"),
                idx, hash->table[idx]));
        }
        i = hash->table[idx];
        while ((i != TD2_ALPM_HASH_INDEX_NULL) && (j++ < hash->entry_count)) {
            if ((j - 1) % 8 == 0) {
                LOG_CLI((BSL_META_U(unit, "\nlink_idx =")));
            }
            LOG_CLI((BSL_META_U(unit, " 0x%04x"), i));
            i = hash->link_table[i & TD2_ALPM_HASH_INDEX_MASK];
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nLPM HASH TABLE DUMP DONE.\n\n")));

    /* Dump TCAM pivot table */
    LOG_CLI((BSL_META_U(unit, "TCAM PIVOT DUMP.\n")));
    for(idx = 0; idx < MAX_PIVOT_COUNT; idx++) {
        /* Get TCAM pivot */
        pivot_pyld = ALPM_TCAM_PIVOT(unit, idx);
        if (NULL == pivot_pyld) {
            continue;
        }
        if (PIVOT_BUCKET_HANDLE(pivot_pyld) == NULL) {
            continue;
        }

        /* Get bucket trie from pivot */
        LOG_CLI((
            BSL_META_U(unit, "tcam_idex = 0x%04x\n"), pivot_pyld->tcam_index));
        LOG_CLI((BSL_META_U(unit, "  len = 0x%02x\t"), pivot_pyld->len));
        LOG_CLI((BSL_META_U(unit, "bpm_len = 0x%02x\t"), pivot_pyld->bpm_len));
        LOG_CLI((
            BSL_META_U(unit, "bkt_idx = 0x%04x\n"),
            PIVOT_BUCKET_INDEX(pivot_pyld)));
        LOG_CLI((
            BSL_META_U(unit, "  vrf = 0x%04x\t"), PIVOT_BUCKET_VRF(pivot_pyld)));
        LOG_CLI((
            BSL_META_U(unit, "v6 = 0x%x\t"), PIVOT_BUCKET_IPV6(pivot_pyld)));
        LOG_CLI((BSL_META_U(unit, "Key : {")));
        if (pivot_pyld->len < 32) {
            LOG_CLI((
                BSL_META_U(unit, "0x%x/%d"),
                pivot_pyld->key[0], pivot_pyld->len));
        } else {
            for (i=0; i < (((pivot_pyld->len)/32) + 1); i++)
                LOG_CLI((BSL_META_U(unit, " 0x%x"), pivot_pyld->key[i]));
                LOG_CLI((BSL_META_U(unit, "/%d"), pivot_pyld->len));
        }
        LOG_CLI((BSL_META_U(unit, "}\n")));
    }
    LOG_CLI((BSL_META_U(unit, "TCAM PIVOT DUMP DONE.\n\n")));

    /* Print VRF count/default route infomation */
    LOG_CLI((BSL_META_U(unit, "VRF ROUTE DUMP.\n")));
    for (idx = 0; idx < (SOC_VRF_MAX(unit) + 2); idx++) {
       if (alpm_vrf_handle[unit][idx].count_v4 != 0) {
           LOG_CLI((
               BSL_META_U(unit, "V4-32 VRF = %d, Routes = %d Default Route:\n"),
               idx, alpm_vrf_handle[unit][idx].count_v4));
           if (VRF_TRIE_DEFAULT_ROUTE_IPV4(unit, idx)) {
               soc_mem_entry_dump(unit, L3_DEFIPm,
                                  VRF_TRIE_DEFAULT_ROUTE_IPV4(unit, idx),
                                  BSL_LSS_CLI);
           }
       }

       if (alpm_vrf_handle[unit][idx].count_v6_64 !=0) {
           LOG_CLI((
               BSL_META_U(unit, "V6-64 VRF = %d, Routes = %d Default Route:\n"),
               idx, alpm_vrf_handle[unit][idx].count_v6_64));
           if (VRF_TRIE_DEFAULT_ROUTE_IPV6(unit, idx)) {
               soc_mem_entry_dump(unit, L3_DEFIPm,
                                  VRF_TRIE_DEFAULT_ROUTE_IPV6(unit, idx),
                                  BSL_LSS_CLI);
           }
       }

       if (alpm_vrf_handle[unit][idx].count_v6_128 != 0) {
           LOG_CLI((
               BSL_META_U(unit, "V6-128 VRF = %d, Routes = %d Default Route:\n"),
               idx, alpm_vrf_handle[unit][idx].count_v6_128));
           if (VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit, idx)) {
               soc_mem_entry_dump(unit, L3_DEFIP_PAIR_128m,
                                  VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit, idx),
                                  BSL_LSS_CLI);
           }
       }
    }
    LOG_CLI((BSL_META_U(unit, "VRF ROUTE DUMP DONE.\n")));

    return;
}

/*
 * Function:
 *     soc_alpm_bucket_sw_dump
 * Purpose:
 *     Displays ALPM bucket information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
soc_alpm_bucket_sw_dump(int unit)
{
    int                 i = 0, bkt= 0, used = 0;
    soc_mem_t           lpm_mem;

    LOG_CLI((BSL_META_U(unit, "\n")));

    LOG_CLI((BSL_META_U(unit,
            "=================================================\n")));
    LOG_CLI((BSL_META_U(unit,
            "  %s[%s]\n"), "VRF Route bucket idx", "mem_view"));
    LOG_CLI((BSL_META_U(unit,
            "=================================================\n")));

    for (i = 0; i < SOC_ALPM_BUCKET_COUNT(unit); i++) {
        SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(unit), i, 1, used);
        if (0 != used) {
            lpm_mem = _soc_trident2_alpm_bkt_view_get(unit, i << 2);

            LOG_CLI((
                BSL_META_U(unit, "  0x%04x[%24s]\n"),
                i, lpm_mem == -1 ? "INVALID" : SOC_MEM_NAME(unit, lpm_mem)));
            bkt++;
        }
    }

    if (bkt == 0) {
        LOG_CLI((BSL_META_U(unit, "- None -\n")));
    }

    return;
}


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     soc_alpm_sw_dump
 * Purpose:
 *     Displays ALPM state maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
soc_alpm_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit, "\n  ALPM Info -\n")));

    soc_alpm_lpm_sw_dump(unit);

    soc_alpm_bucket_sw_dump(unit);

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      _soc_alpm_sanity_check
 * Purpose:
 *      Sanity check for ALPM with L3_DEFIP
 * Parameters:
 *      u           - Device unit
 *      min         - Min index of L3_DEFIP
 *      max         - Max index of L3_DEFIP
 *      check_sw    - Check software
 *      bkt_ptr_arr - bucket ptr array
 * Returns:
 *      SOC_E_XXX defined in error.h
 */
STATIC int
_soc_alpm_sanity_check(int u, int min, int max, int check_sw, int *bkt_ptr_arr)
{
    int             rv = SOC_E_NONE;
    int             lpm_idx, lpm_idx2, fd_lpm_idx2, alpm_idx, prev_idx = 0;
    int             i, j, ipv6, prev_v6 = 0, prev_i = 0;
    int             vrf_id, vrf;
    int             fd_lpm_idx, fd_bkt_ptr, fd_alpm_idx;
    int             ent, ent_num, bkt_ptr;
    int             error1 = 0, error2 = 0, error = 0, success = 0;
    int             used;
    int             bpm_len, ip_len;
    int             trie_bpm_len, default_route;
    int             alloc_size;
    uint32          bank_disable = 0;
    uint32          e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32          shadow[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32          prefix[5], length;
    trie_t          *pfx_trie  = NULL;
    trie_node_t     *lpmp      = NULL;
    payload_t       *tmp_pyld  = NULL;
    uint32          data0[10] = {0};
    uint32          data1[10] = {0};
    char            *buffer = NULL;
    defip_entry_t   *lpm_entry, lpm_entry2, lpm_bmp;
    soc_mem_t       bkt_mem_type;
    soc_mem_t       alpm_mem;
    int             flex;
    int             bpm_check = 0;
    int             flex_v6_64;

    /* Calculate table size. */
    alloc_size = sizeof(defip_entry_t) * (max - min + 1);

    /* Allocate memory buffer. */
    buffer = soc_cm_salloc(u, alloc_size, "lpm_tbl");
    if (buffer == NULL) {
        return (SOC_E_MEMORY);
    }

    /* Reset allocated buffer. */
    sal_memset(buffer, 0, alloc_size);

    SOC_ALPM_LPM_LOCK(u);

    /* Read table to the buffer. */
    if (soc_mem_read_range(u, L3_DEFIPm, MEM_BLOCK_ANY,
                           min, max, buffer) < 0) {
        soc_cm_sfree(u, buffer);
        SOC_ALPM_LPM_UNLOCK(u);
        return (SOC_E_INTERNAL);
    }

    for (lpm_idx = min; lpm_idx <= max; lpm_idx++) {
        lpm_entry = soc_mem_table_idx_to_pointer(u, L3_DEFIPm,
                                                 defip_entry_t *, buffer,
                                                 lpm_idx - min);

        ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, MODE0f);

        for (i = 0; i <= (ipv6 ? 0 : 1); i++) {
            if (error1) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                          "\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),
                          prev_idx, prev_i, prev_v6, error1));
                error = 1;
            } else if (success) {
                LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(u,
                        "\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),
                         prev_idx, prev_i, prev_v6, success));
            }
            error1 = error2 = success = 0;
            prev_v6  = ipv6;
            prev_idx = lpm_idx;
            prev_i   = i;

            /* cache coherency check */
            if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, VALID0f) ||
                SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, VALID1f)) {
                defip_entry_t hw_ent, sw_ent;
                soc_mem_t cmem = L3_DEFIPm;
                sal_memset(&hw_ent, 0, sizeof(hw_ent));
                sal_memset(&sw_ent, 0, sizeof(sw_ent));
                rv = _soc_mem_alpm_read_cache(u, cmem, SOC_BLOCK_ANY,
                                              lpm_idx, &sw_ent);
                if (SOC_SUCCESS(rv)) {
                    rv = soc_mem_read_no_cache(u, 0, cmem, 0, SOC_BLOCK_ANY,
                                               lpm_idx, &hw_ent);
                    if (SOC_SUCCESS(rv)) {
                        if (soc_mem_field_valid(u, cmem, EVEN_PARITYf)) {
                            soc_mem_field32_set(u, cmem, &hw_ent,
                                                EVEN_PARITYf, 0);
                            soc_mem_field32_set(u, cmem, &sw_ent,
                                                EVEN_PARITYf, 0);
                        } else if (soc_mem_field_valid(u, cmem, PARITY0f)) {
                            soc_mem_field32_set(u, cmem, &hw_ent, PARITY0f, 0);
                            soc_mem_field32_set(u, cmem, &sw_ent, PARITY0f, 0);
                            soc_mem_field32_set(u, cmem, &hw_ent, PARITY1f, 0);
                            soc_mem_field32_set(u, cmem, &sw_ent, PARITY1f, 0);
                        }
                        soc_mem_field32_set(u, cmem, &hw_ent, HIT0f, 0);
                        soc_mem_field32_set(u, cmem, &sw_ent, HIT0f, 0);
                        soc_mem_field32_set(u, cmem, &hw_ent, HIT1f, 0);
                        soc_mem_field32_set(u, cmem, &sw_ent, HIT1f, 0);
                        if (sal_memcmp(&sw_ent, &hw_ent,
                                       soc_mem_entry_bytes(u, cmem)) != 0) {
                            LOG_ERROR(BSL_LS_SOC_ALPM,
                                (BSL_META_U(u, "\t%s index:[%d], ipv6(%d)"
                                " cache mismatch\n"), SOC_MEM_NAME(u, cmem),
                                lpm_idx, ipv6));
                            error1 ++;
                        }
                    }
                }

                /* We can just use defip_entry_t because it's always longer
                 * than aux_entry_t */
                cmem = L3_DEFIP_AUX_TABLEm;
                sal_memset(&hw_ent, 0, sizeof(hw_ent));
                sal_memset(&sw_ent, 0, sizeof(sw_ent));
                lpm_idx2 = soc_alpm_physical_idx(u, L3_DEFIPm, lpm_idx, 1);
                rv = _soc_mem_alpm_read_cache(u, cmem, SOC_BLOCK_ANY,
                                              lpm_idx2, &sw_ent);
                if (SOC_SUCCESS(rv)) {
                    rv = soc_mem_read_no_cache(u, 0, cmem, 0, SOC_BLOCK_ANY,
                                               lpm_idx2, &hw_ent);
                    if (SOC_SUCCESS(rv)) {
                        if (soc_mem_field_valid(u, cmem, EVEN_PARITYf)) {
                            soc_mem_field32_set(u, cmem, &hw_ent,
                                                EVEN_PARITYf, 0);
                            soc_mem_field32_set(u, cmem, &sw_ent,
                                                EVEN_PARITYf, 0);
                        } else if (soc_mem_field_valid(u, cmem, PARITYf)) {
                            soc_mem_field32_set(u, cmem, &hw_ent, PARITYf, 0);
                            soc_mem_field32_set(u, cmem, &sw_ent, PARITYf, 0);
                        }
                        if (sal_memcmp(&sw_ent, &hw_ent,
                                       soc_mem_entry_bytes(u, cmem)) != 0) {
                            LOG_ERROR(BSL_LS_SOC_ALPM,
                                (BSL_META_U(u, "\t%s index:[%d], ipv6(%d)"
                                " cache mismatch\n"), SOC_MEM_NAME(u, cmem),
                                lpm_idx2, ipv6));
                            error1 ++;
                        }
                    }
                }
            }

            if (i == 1) {
                soc_alpm_lpm_ip4entry1_to_0(u, lpm_entry, lpm_entry, PRESERVE_HIT);
            }
            bkt_ptr = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, ALG_BKT_PTR0f);
            if (!SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, VALID0f)) {
                continue;
            }
            flex = soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f);
            flex_v6_64 = 0;
            if (soc_feature(u, soc_feature_alpm_flex_stat_v6_64) && ipv6 && flex) {
                if (soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK0f) == 0xFFFFFFFF &&
                    soc_mem_field32_get(u, L3_DEFIPm, lpm_entry, IP_ADDR_MASK1f) == 0xFFFFFFFF) {
                    flex_v6_64 = 1;
                }
            }

            if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, GLOBAL_HIGH0f) ||
                (soc_feature(u, soc_feature_ipmc_defip) &&
                 SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, MULTICAST_ROUTE0f)) ||
                 flex_v6_64) {
                if (bkt_ptr != 0) {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u, "\tWrong bkt_ptr %d\n"), 
                              bkt_ptr));
                    error1 ++;
                }
                continue;
            }
            if (bkt_ptr == 0) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tWrong bkt_ptr2 %d\n"), 
                          bkt_ptr));
                error1 ++;
            }

            j = 0;
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, ECMP0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, ECMP_PTR0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, NEXT_HOP_INDEX0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, PRI0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, RPE0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, DST_DISCARD0f);
            data0[j++] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, CLASS_ID0f);

            /* Check if bucket pointer duplicated */
            if (bkt_ptr_arr) {
                if (bkt_ptr_arr[bkt_ptr] == -1) {
                    bkt_ptr_arr[bkt_ptr] = lpm_idx;
                    if (SOC_ALPM_V6_SCALE_CHECK(u, ipv6)) {
                        bkt_ptr_arr[bkt_ptr + 1] = lpm_idx;
                    }
                    success ++;
                } else {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u, "\tConflict bucket pointer %d: "
                              "was %d now %d\n"), bkt_ptr,
                              bkt_ptr_arr[bkt_ptr], lpm_idx));
                    error1 ++;
                }
            }

            if (ipv6) {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    ent_num = ALPM_IPV6_64_BKT_COUNT_FLEX;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                    ent_num = ALPM_IPV6_64_BKT_COUNT;
                }
            } else {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                    ent_num = ALPM_IPV4_BKT_COUNT_FLEX;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV4m;
                    ent_num = ALPM_IPV4_BKT_COUNT;
                }
            }
            if (SOC_ALPM_V6_SCALE_CHECK(u, ipv6)) {
                ent_num <<= 1;
            }

            (void) soc_alpm_lpm_vrf_get(u, lpm_entry, &vrf_id, &vrf);

            /* read shadow table for bpm length */
            lpm_idx2 = soc_alpm_physical_idx(u, L3_DEFIPm, lpm_idx, 1);
            rv = soc_mem_read(u, L3_DEFIP_AUX_TABLEm, MEM_BLOCK_ANY,
                              lpm_idx2, shadow);
            bpm_len = -1;
            bpm_check = 0;
            if (SOC_SUCCESS(rv)) {
                if (i == 0) {
                    bpm_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm,
                                                  shadow, BPM_LENGTH0f);
                    ip_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm,
                                                  shadow, IP_LENGTH0f);
                } else {
                    bpm_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm,
                                                  shadow, BPM_LENGTH1f);
                    ip_len = soc_mem_field32_get(u, L3_DEFIP_AUX_TABLEm,
                                                  shadow, IP_LENGTH1f);
                }
                if (bpm_len > ip_len) {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u, "\tAUX Table corrupted\n")));
                    error1 ++;
                } else {
                    success ++;
                }

                bpm_check = 1;
                if (bpm_len == 0 &&
                    SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, lpm_entry, DEFAULT_MISS0f)) {
                    if (soc_alpm_mode_get(u) == SOC_ALPM_MODE_COMBINED) {
                        error1 ++;
                    } else {
                        bpm_check = 0;
                    }
                }
            }

            if (bpm_check) {
                sal_memcpy(&lpm_bmp, lpm_entry, sizeof(defip_entry_t));
                _soc_alpm_pfx_len_to_mask(u, &lpm_bmp, bpm_len, ipv6);

                fd_lpm_idx = fd_bkt_ptr = fd_alpm_idx = -1;
                rv = _soc_alpm_find(u, &lpm_bmp, alpm_mem, e, &fd_lpm_idx,
                                    &fd_bkt_ptr, &fd_alpm_idx, FALSE);

                if (fd_lpm_idx > 0) {
                    fd_lpm_idx = soc_alpm_logical_idx(u, L3_DEFIPm,
                                                      fd_lpm_idx >> 1, 1);
                }

                if (SOC_SUCCESS(rv)) {
                    j = 0;
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, ECMPf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, ECMP_PTRf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, NEXT_HOP_INDEXf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, PRIf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, RPEf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, DST_DISCARDf);
                    data1[j++] = soc_mem_field32_get(u, alpm_mem, e, CLASS_IDf);

                    if (sal_memcmp(data0, data1, sizeof(data0)) != 0) {
                        LOG_ERROR(BSL_LS_SOC_ALPM,
                                  (BSL_META_U(u,
                                  "\tData mismatch: "
                                  "lpm_idx %d find_lpm_idx %d "
                                  "find_alpm_idx %d find_bkt_ptr %d\n"),
                                  lpm_idx, fd_lpm_idx,
                                  fd_alpm_idx, fd_bkt_ptr));
                        error1 ++;
                    } else {
                        success ++;
                    }
                } else {
                    if (rv == SOC_E_NOT_FOUND &&
                        (vrf_id == SOC_L3_VRF_GLOBAL || soc_alpm_mode_get(u))) {
                        LOG_VERBOSE(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                              "\t_soc_alpm_find rv=%d: "
                              "lpm_idx %d find_lpm_idx %d "
                              "find_alpm_idx %d find_bkt_ptr %d,"
                              "(Possible: global route or !combined "
                              "without default)\n"),
                              rv, lpm_idx, fd_lpm_idx,
                              fd_alpm_idx, fd_bkt_ptr));
                    } else {
                        LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                              "\t_soc_alpm_find rv=%d: "
                              "lpm_idx %d find_lpm_idx %d "
                              "find_alpm_idx %d find_bkt_ptr %d\n"),
                              rv, lpm_idx, fd_lpm_idx,
                              fd_alpm_idx, fd_bkt_ptr));
                        error1 ++;
                    }
                }
            }

            bank_disable = soc_alpm_bank_dis(u, vrf);
            /* Traverse bucket */
            for (ent = 0; ent < ent_num; ent++) {
                (void) _soc_alpm_mem_index(u, alpm_mem, bkt_ptr, ent, bank_disable, &alpm_idx);

                rv = _soc_mem_alpm_read(u, alpm_mem,
                                        MEM_BLOCK_ANY, alpm_idx, e);
                if (SOC_FAILURE(rv)) {
                    continue;
                }

                if (!soc_mem_field32_get(u, alpm_mem, e, VALIDf)) {
                    continue;
                }

                (void) _soc_alpm_lpm_ent_init(u, e, alpm_mem, ipv6, vrf_id,
                                              bkt_ptr, 0, &lpm_entry2, 0);

                if (check_sw) {
                    rv = _soc_alpm_sw_prefix_check(u, alpm_mem, ipv6, &lpm_entry2, vrf_id, vrf);
                    if (SOC_FAILURE(rv)) {
                        error1 ++;
                        error2 ++;
                        LOG_ERROR(BSL_LS_SOC_ALPM,
                                  (BSL_META_U(u,
                                  "_soc_alpm_sw_prefix_check rv=%d: v6 = %d, vrf_id = %d, vrf = %d\n"),
                                  rv, ipv6, vrf_id, vrf));
                    }
                }

                fd_lpm_idx2 = fd_lpm_idx = fd_bkt_ptr = fd_alpm_idx = -1;
                rv = _soc_alpm_find(u, &lpm_entry2, alpm_mem, e,
                                    &fd_lpm_idx, &fd_bkt_ptr,
                                    &fd_alpm_idx, FALSE);

                if (SOC_SUCCESS(rv)) {
                    fd_lpm_idx2 = soc_alpm_logical_idx(u,
                        L3_DEFIPm, fd_lpm_idx >> 1, 1);
                }

                if (SOC_FAILURE(rv) ||
                    fd_bkt_ptr  != bkt_ptr ||
                    fd_lpm_idx2 != lpm_idx  ||
                    fd_lpm_idx >> 1  != lpm_idx2 || /* for map remap */
                    fd_alpm_idx != alpm_idx) {

                    error1 ++;
                    error2 ++;
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u,
                              "\t_soc_alpm_find2 rv=%d: "
                              "fl [%d,%d] fa %d fb %d "
                              "l [%d,%d] a %d b %d\n"),
                              rv,
                              fd_lpm_idx >> 1, fd_lpm_idx2, fd_alpm_idx, fd_bkt_ptr,
                              lpm_idx2, lpm_idx, alpm_idx, bkt_ptr));
                }
            }
            if (error2 == 0) {
                success ++;
            }

            if (!check_sw) {
                continue;
            }

            /*
             * Check hardware table above.
             * Check software table below.
 */

            /* lpm hash */

#ifdef FB_LPM_HASH_SUPPORT
            rv = LPM_HASH_VERIFY(u, lpm_entry,
                                 ipv6 ? lpm_idx : ((lpm_idx << 1) + i));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tLPM Hash check failed: rv %d "
                          "lpm_idx %d\n"),
                          rv, lpm_idx));
                error1 ++;
            } else {
                success ++;
            }
#endif

            /* bucket bitmap */
            rv = alpm_bucket_is_assigned(u, bkt_ptr, ipv6, &used);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tInvalid bucket pointer %d "
                          "detected, in memory %s index %d\n"), bkt_ptr,
                          SOC_MEM_NAME(u, L3_DEFIPm), lpm_idx));
                error1 ++;
            } else if (used == 0) {
                /* If bucket was already freed */
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tFreed bucket pointer %d "
                          "detected, in memory %s index %d\n"), bkt_ptr,
                          SOC_MEM_NAME(u, L3_DEFIPm), lpm_idx));
                error1 ++;
            } else {
                success ++;
            }

            /* bucket view map */
            bkt_mem_type = _soc_trident2_alpm_bkt_view_get(u, bkt_ptr << 2);
            if (alpm_mem != bkt_mem_type) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tMismatched alpm view "
                           "in bucket %d, expected %s, was %s\n"), bkt_ptr,
                           SOC_MEM_NAME(u, alpm_mem),
                           SOC_MEM_NAME(u, bkt_mem_type)));
                error1 ++;
            } else {
                success ++;
            }

            /* Check bpm len information */
            rv = _alpm_prefix_create(u, lpm_entry, prefix,
                                     &length, &default_route);
            if (SOC_SUCCESS(rv)) {
                if (ipv6) {
                    pfx_trie = VRF_PREFIX_TRIE_IPV6(u, vrf);
                } else {
                    pfx_trie = VRF_PREFIX_TRIE_IPV4(u, vrf);
                }
                lpmp = NULL;
                if (pfx_trie) {
                    rv = trie_find_lpm(pfx_trie, prefix, length, &lpmp);
                }
                if (SOC_SUCCESS(rv) && lpmp){
                    tmp_pyld = (payload_t *) lpmp;
                    if (tmp_pyld->bkt_ptr != NULL) {
                        trie_bpm_len = ((payload_t *)(tmp_pyld->bkt_ptr))->len;
                        if (trie_bpm_len != bpm_len) {
                            LOG_ERROR(BSL_LS_SOC_ALPM,
                                      (BSL_META_U(u, "\tBPM len mismatch: lpm_idx %d"
                                      " alpm_idx %d bpm_len %d trie_bpm_len %d\n"),
                                      lpm_idx, alpm_idx, bpm_len, trie_bpm_len));
                            error1 ++;
                        } else {
                            success ++;
                        }
                    } else {
                        success ++;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_ALPM,
                              (BSL_META_U(u, "\ttrie_find_lpm failed: "
                              "lpm_idx %d alpm_idx %d lpmp %p\n"),
                              lpm_idx, alpm_idx, lpmp));
                    error1 ++;
                }
            } else {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u, "\tPrefix creating failed: "
                          "lpm_idx %d alpm_idx %d\n" ), lpm_idx, alpm_idx));
                error1 ++;
            }
        }
    }

    if (error1) {
        LOG_ERROR(BSL_LS_SOC_ALPM,
                  (BSL_META_U(u,
                  "\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),
                  prev_idx, prev_i, prev_v6, error1));
        error = 1;
    } else if (success) {
        LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(u,
                "\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),
                 prev_idx, prev_i, prev_v6, success));
    }

    SOC_ALPM_LPM_UNLOCK(u);
    soc_cm_sfree(u, buffer);

    return (error ? SOC_E_FAIL: SOC_E_NONE);
}

extern int _soc_alpm_128_sanity_check(int u, int min, int max,
                                      int check_sw, int *bkt_ptr_arr);

/*
 * Function:
 *      soc_alpm_sanity_check
 * Purpose:
 *      Sanity check for ALPM
 * Parameters:
 *      u       - Device unit
 *      mem     -   L3_DEFIP/L3_DEFIP_PAIR_128 memory
 *      index   -   -2 indicates full check to both table,
 *                  -1 indicates full check to given table
 *                  other indicates single entry check to given table
 *      check_sw -  Check SW tables as well. 
 */
int
soc_alpm_sanity_check(int u, soc_mem_t mem, int index, int check_sw)
{
    int         rv           = SOC_E_NONE;
    int         rv2          = SOC_E_NONE;
    int         index_min    = -1, index_max  = -1;
    int         index_min2   = -1, index_max2 = -1;
    int         *bkt_ptr_arr = NULL;
#ifdef ALPM_SANITY_SENSITIVE
    int         i;
    soc_mem_t   bkt_view;
#endif

    if ((mem == L3_DEFIPm || index == -2) && 
        soc_mem_index_max(u, L3_DEFIPm) != -1) {
        index_min  = soc_mem_index_min(u, L3_DEFIPm);
        index_max  = soc_mem_index_max(u, L3_DEFIPm);
        /* Don't check urpf */
        if (SOC_URPF_STATUS_GET(u)) {
            index_max >>= 1;
        }
        if (index < -2 || index > index_max) {
            return SOC_E_PARAM;
        }
        if (index >= index_min && index <= index_max) {
            index_min = index;
            index_max = index;
        }
    }

    if ((mem == L3_DEFIP_PAIR_128m || index == -2) && 
        soc_mem_index_max(u, L3_DEFIP_PAIR_128m) != -1) {
        index_min2 = soc_mem_index_min(u, L3_DEFIP_PAIR_128m);
        index_max2 = soc_mem_index_max(u, L3_DEFIP_PAIR_128m);
        /* Don't check urpf */
        if (SOC_URPF_STATUS_GET(u)) {
            index_max2 >>= 1;
        }
        if (index < -2 || index > index_max2) {
            return SOC_E_PARAM;
        }
        if (index >= index_min2 && index <= index_max2) {
            index_min2 = index;
            index_max2 = index;
        }
    }

    if (index_max == -1 && index_max2 == -1) {
        return SOC_E_PARAM;
    }

    L3_LOCK(u);
    bkt_ptr_arr = sal_alloc(sizeof(int) * MAX_PIVOT_COUNT, "Bucket ptr array");
    if (bkt_ptr_arr == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(bkt_ptr_arr, 0xff, sizeof(int) * MAX_PIVOT_COUNT);

    if (index_max != -1) {
        LOG_CLI(("Processing ALPM sanity check L3_DEFIP "
                 "from index:%d to index:%d\n",
                 index_min, index_max));
        rv = _soc_alpm_sanity_check(u, index_min, index_max, check_sw, bkt_ptr_arr);
        LOG_CLI(("ALPM sanity check L3_DEFIP %s. \n\n",
                 SOC_SUCCESS(rv) ? "passed" : "failed"));
    }

    if (index_max2 != -1) {
        LOG_CLI(("Processing ALPM sanity check L3_DEFIP_PAIR_128 "
                 "from index:%d to index:%d\n",
                 index_min2, index_max2));
        rv2 = _soc_alpm_128_sanity_check(u, index_min2, index_max2, check_sw, bkt_ptr_arr);
        LOG_CLI(("ALPM sanity check L3_DEFIP_PAIR_128 %s. \n",
                 SOC_SUCCESS(rv2) ? "passed" : "failed"));
    }

#ifdef ALPM_SANITY_SENSITIVE
    /* This check is very sensitive, turn it off for now. */
    /* In case full check to both table, the bkt ptr is fully collected */
    if (index == -2 &&  SOC_SUCCESS(rv) && SOC_SUCCESS(rv2)) {
        for (i = 0; i < MAX_PIVOT_COUNT; i++) {
            if (bkt_ptr_arr[i] != -1) {
                continue;
            }
            bkt_view = _soc_trident2_alpm_bkt_view_get(u, i << 2);
            if (bkt_view != INVALIDm){
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(u,
                          "\tBucket view corrupted: bkt_ptr %d, view %s.\n"),
                          i, SOC_MEM_NAME(u, bkt_view)));
            }
        }
    }
#endif

    sal_free(bkt_ptr_arr);
    L3_UNLOCK(u);
    return SOC_SUCCESS(rv) ? rv2: rv;
}


/* Get entry No. per bucket based on various setting */
int
_soc_alpm_bkt_entry_cnt(int u, soc_mem_t mem)
{
    int bkt_cnt = 0;
    int v6;

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
        bkt_cnt = ALPM_IPV4_BKT_COUNT;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        bkt_cnt = ALPM_IPV4_BKT_COUNT_FLEX;
        v6 = 0;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        bkt_cnt = ALPM_IPV6_64_BKT_COUNT;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        bkt_cnt = ALPM_IPV6_64_BKT_COUNT_FLEX;
        v6 = 1;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        bkt_cnt = ALPM_IPV6_128_BKT_COUNT;
        v6 = 1;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    if (SOC_ALPM_V6_SCALE_CHECK(u, v6)) {
        bkt_cnt <<= 1;
    }

    if (soc_alpm_mode_get(u) == SOC_ALPM_MODE_PARALLEL &&
        SOC_URPF_STATUS_GET(u)) {
        bkt_cnt >>= 1;
    }

    return bkt_cnt;
}

/*
 * Function:
 *      soc_alpm_route_capacity_get
 * Purpose:
 *      Get the capacity for ALPM from tables:
 * L3_DEFIP_ALPM_IPV4, L3_DEFIP_ALPM_IPV6_64, L3_DEFIP_ALPM_IPV6_128
 * L3_DEFIP, L3_DEFIP_PAIR_128
 * Parameters:
 *      unit         - Device unit
 *      route_type   - 0: v4, 1: v6-64, 2: v6-128
 *      cap_type     - 0: max capacity, 1: min guaranteed capacity
 *      max_entries  - Maximum result returned.
 *      min_entries  - Minimum result returned.
 */
int
soc_alpm_route_capacity_get(int unit, soc_mem_t mem,
                            int *max_entries, int *min_entries)
{
    int rv = SOC_E_NONE;
    int num_ent, num_pivot, num_bkt_max, num_bkt_min;

    if (max_entries == NULL && min_entries == NULL) {
        return SOC_E_PARAM;
    }

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
    case L3_DEFIP_ALPM_IPV4_1m:
        num_ent = soc_mem_index_count(unit, L3_DEFIPm) * 2;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
        num_ent = soc_mem_index_count(unit, L3_DEFIP_PAIR_128m);
        if (num_ent == 0) {
            num_ent = soc_mem_index_count(unit, L3_DEFIPm);
        }
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        num_ent = soc_mem_index_count(unit, L3_DEFIP_PAIR_128m);
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    num_pivot = num_ent >> SOC_URPF_STATUS_GET(unit);
    num_bkt_max = _soc_alpm_bkt_entry_cnt(unit, mem);
    num_bkt_min = num_bkt_max / 2;

    if (max_entries != NULL) {
        *max_entries = num_pivot * num_bkt_max;
    }
    if (min_entries != NULL) {
        *min_entries = num_pivot * num_bkt_min;
    }

    return rv; 
}

#endif /* ALPM_ENABLE */
