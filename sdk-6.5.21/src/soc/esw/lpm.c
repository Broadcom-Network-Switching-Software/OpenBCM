/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Firebolt LPM TCAM table insert/delete/lookup routines
 * soc_fb_lpm_init - Called from bcm_l3_init
 * soc_fb_lpm_insert - Insert/Update an IPv4/IPV6 route entry into LPM table
 * soc_fb_lpm_delete - Delete an IPv4/IPV6 route entry from LPM table
 * soc_fb_lpm_match  - (Exact match for the key. Will match both IP address
 *                      and mask)
 * soc_fb_lpm_lookup (LPM search) - Not Implemented
 *
 * Hit bit preservation - May loose HIT bit state when entries are moved
 * around due to race condition. This happens if the HIT bit gets set in
 * hardware after reading the entry to be moved and before the move is
 * completed. If the HIT bit for an entry is already set when the move is
 * initiated then it is preserved.
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/format.h>
#include <shared/util.h>
#include <shared/avl.h>
#include <shared/l3.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_UTT_SUPPORT
#include <soc/esw/utt.h>
#endif /* BCM_UTT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
static soc_kt_lpm_ipv6_info_t *kt_lpm_ipv6_info[SOC_MAX_NUM_DEVICES];
#define L3_DEFIP_TCAM_SIZE 1024
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
/* Matches with _BCM_TR3_DEFIP_TCAM_DEPTH in triumph3.h */
#define _SOC_TR3_DEFIP_TCAM_DEPTH 1024 
#endif
#ifdef BCM_FIREBOLT_SUPPORT
#if defined(INCLUDE_L3)
soc_field_t defip_mode0_bit[SOC_MAX_NUM_DEVICES];
soc_field_t defip_mode1_bit[SOC_MAX_NUM_DEVICES];
soc_field_t defip_mode_mask0_bit[SOC_MAX_NUM_DEVICES];
soc_field_t defip_mode_mask1_bit[SOC_MAX_NUM_DEVICES];
/*
 * TCAM based LPM implementation. Each table entry can hold two IPV4 entries or
 * one IPV6 entry. VRF independent routes placed at the beginning or 
 * at the end of table based on application provided entry vrf id 
 * (SOC_L3_VRF_OVERRIDE/SOC_L3_VRF_GLOBAL).   
 *
 *              MAX_PFX_INDEX
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
 *              IPV6_PFX_ZERO
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

#if defined(FB_LPM_HASH_SUPPORT)
#undef FB_LPM_AVL_SUPPORT
#elif defined(FB_LPM_AVL_SUPPORT)
#else
#define FB_LPM_HASH_SUPPORT 1
#endif

#ifdef FB_LPM_CHECKER_ENABLE
#define FB_LPM_AVL_SUPPORT 1
#define FB_LPM_HASH_SUPPORT 1
#endif

#define SOC_MEM_COMPARE_RETURN(a, b) {          \
        if ((a) < (b)) { return -1; }           \
        if ((a) > (b)) { return  1; }           \
}

/* Can move to SOC Control structures */
soc_lpm_state_p soc_lpm_state[SOC_MAX_NUM_DEVICES];
soc_lpm128_table_t *soc_lpm128_state_table[SOC_MAX_NUM_DEVICES];
int *soc_lpm128_index_to_pfx_group[SOC_MAX_NUM_DEVICES];
soc_lpm_stat_p soc_lpm_stat[SOC_MAX_NUM_DEVICES];
soc_ext_lpm_stat_p soc_ext_lpm_stat[SOC_MAX_NUM_DEVICES];

#define SOC_LPM_CACHE_FIELD_CREATE(m, f) soc_field_info_t * m##f
#define SOC_LPM_CACHE_FIELD_ASSIGN(m_u, s, m, f)  {\
         if (SOC_MEM_IS_VALID(m_u,L3_DEFIP_LEVEL1m)) { \
                (s)->m##f = soc_mem_fieldinfo_get(m_u, L3_DEFIP_LEVEL1m, f); \
         } else { \
                (s)->m##f = soc_mem_fieldinfo_get(m_u, m, f); \
         } \
     }

typedef struct soc_lpm_field_cache_s {
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, CLASS_ID0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, CLASS_ID1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DST_DISCARD0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DST_DISCARD1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_COUNT0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_COUNT1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_PTR0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_PTR1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_ROUTE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_ROUTE1f);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MULTICAST_ROUTE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MULTICAST_ROUTE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, EXPECTED_L3_IIF0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, EXPECTED_L3_IIF1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPA_ID0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPA_ID1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, L3MC_INDEX0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, L3MC_INDEX1f);
#endif
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, HIT0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, HIT1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_MASK1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, defip_mode0_field);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, defip_mode1_field);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, defip_mode_mask0_field);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, defip_mode_mask1_field);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, NEXT_HOP_INDEX0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, NEXT_HOP_INDEX1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, PRI0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, PRI1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VALID0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VALID1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_MASK1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE_MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ENTRY_TYPE_MASK1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, D_ID0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, D_ID1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, D_ID_MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, D_ID_MASK1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DEFAULTROUTE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DEFAULTROUTE1f);
#if defined(BCM_TRIDENT3_SUPPORT)
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_MODE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_MODE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DESTINATION0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DESTINATION1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DATA_TYPE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DATA_TYPE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_TYPE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_TYPE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_TYPE_MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, KEY_TYPE_MASK1f);
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_OFFSET_MODE1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_POOL_NUMBER0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_POOL_NUMBER1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX1f);
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, HIT0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, HIT1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, VALID0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, VALID1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, KEY0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, KEY1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, MASK0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, MASK1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, ASSOC_DATA0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, ASSOC_DATA1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, FIXED_DATA0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, FIXED_DATA1f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, ALPM1_DATA0f);
    SOC_LPM_CACHE_FIELD_CREATE(L3_DEFIP_LEVEL1m, ALPM1_DATA1f);
#endif
} soc_lpm_field_cache_t, *soc_lpm_field_cache_p;
static soc_lpm_field_cache_p soc_lpm_field_cache_state[SOC_MAX_NUM_DEVICES];

#define SOC_TH3_MEM_OPT_F64_GET(m_unit, m_mem, m_entry_data, m_field, pval64) \
        soc_meminfo_fieldinfo_field_get((m_entry_data), (&SOC_MEM_INFO(m_unit, m_mem)), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1m##m_field), (pval64))

#define SOC_TH3_MEM_OPT_F64_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field_set((m_entry_data), (&SOC_MEM_INFO(m_unit, m_mem)), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1m##m_field), (m_val))

#define SOC_TH3_MEM_OPT_F32_GET(m_unit, m_mem, m_entry_data, m_field) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1m##m_field))

#define SOC_TH3_MEM_OPT_F32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1m##m_field)), (m_val))


#define SOC_MEM_OPT_F32_GET(m_unit, m_mem, m_entry_data, m_field) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPm##m_field))

#define SOC_MEM_OPT_F32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPm##m_field, (m_val))

#if defined(BCM_TOMAHAWK3_SUPPORT)
#define SOC_MEM_OPT_F32_VALID0_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID0f) : soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mVALID0f))

#define SOC_MEM_OPT_F32_VALID0_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID0f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mVALID0f)), (m_val))

#define SOC_MEM_OPT_F32_VALID1_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID1f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mVALID1f)))

#define SOC_MEM_OPT_F32_VALID1_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID1f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mVALID1f)), (m_val))

#define SOC_MEM_OPT_F32_HIT0_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT0f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mHIT0f)))

#define SOC_MEM_OPT_F32_HIT0_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT0f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mHIT0f)), (m_val))

#define SOC_MEM_OPT_F32_HIT1_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT1f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mHIT1f)))

#define SOC_MEM_OPT_F32_HIT1_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), ((m_mem == L3_DEFIPm) ? \
                                          (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT1f) : (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIP_LEVEL1mHIT1f)), (m_val))
#else

#define SOC_MEM_OPT_F32_VALID0_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID0f)

#define SOC_MEM_OPT_F32_VALID0_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID0f, (m_val))

#define SOC_MEM_OPT_F32_VALID1_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID1f)

#define SOC_MEM_OPT_F32_VALID1_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmVALID1f), (m_val))

#define SOC_MEM_OPT_F32_HIT0_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT0f))

#define SOC_MEM_OPT_F32_HIT0_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT0f), (m_val))

#define SOC_MEM_OPT_F32_HIT1_GET(m_unit, m_mem, m_entry_data) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT1f))

#define SOC_MEM_OPT_F32_HIT1_SET(m_unit, m_mem, m_entry_data, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), (soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPmHIT1f), (m_val))
#endif
#define SOC_MEM_OPT_FIELD_VALID(m_unit, m_mem, m_field) \
                ((soc_lpm_field_cache_state[(m_unit)]->L3_DEFIPm##m_field) != NULL)

#if defined(FB_LPM_AVL_SUPPORT)
typedef struct soc_lpm_avl_state_s {
    uint32      mask0;
    uint32      mask1;
    uint32      ip0;
    uint32      ip1;
    uint32      vrf_id;
    uint32      index;
} soc_lpm_avl_state_t, *soc_lpm_avl_state_p;

static shr_avl_t   *avl[SOC_MAX_NUM_DEVICES];   /* AVL tree to track entries */

#define SOC_LPM_STATE_AVL(u)         (avl[(u)])
/*
 * Function:
 *      soc_fb_lpm_avl_compare_key
 * Purpose:
 *      Comparison function for AVL shadow table operations.
 */
int
soc_fb_lpm_avl_compare_key(void *user_data,
                          shr_avl_datum_t *datum1,
                          shr_avl_datum_t *datum2)
{
    soc_lpm_avl_state_p d1, d2;
    
    /* COMPILER_REFERENCE(user_data);*/
    d1 = (soc_lpm_avl_state_p)datum1;
    d2 = (soc_lpm_avl_state_p)datum2;

    SOC_MEM_COMPARE_RETURN(d1->vrf_id, d2->vrf_id);
    SOC_MEM_COMPARE_RETURN(d1->mask1, d2->mask1);
    SOC_MEM_COMPARE_RETURN(d1->mask0, d2->mask0);
    SOC_MEM_COMPARE_RETURN(d1->ip1, d2->ip1);
    SOC_MEM_COMPARE_RETURN(d1->ip0, d2->ip0);

    return(0);
}

#define LPM_AVL_INSERT_IPV6(u, entry_data, tab_index)                        \
{                                                                            \
    soc_lpm_avl_state_t     d;                                               \
    d.ip0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);        \
    d.ip1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);        \
    d.mask0 =                                                                \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
    d.mask1 =                                                                \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);       \
    if ((!(SOC_IS_HURRICANE(u))) && \
	 (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                      \
        d.vrf_id = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_0f); \
    } else {                                                                 \
        d.vrf_id = 0;                                                        \
    }                                                                        \
    d.index = tab_index;                                                     \
    if (shr_avl_insert(SOC_LPM_STATE_AVL(u),                                 \
                       soc_fb_lpm_avl_compare_key,                           \
                       (shr_avl_datum_t *)&d)) {                             \
    }                                                                        \
}

#define LPM_AVL_INSERT_IPV4_0(u, entry_data, tab_index)                      \
{                                                                            \
    soc_lpm_avl_state_t     d;                                               \
    d.ip0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);        \
    d.mask0 =                                                                \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
    d.ip1 = 0;                                                               \
    d.mask1 = 0x80000001;                                                    \
    if ((!(SOC_IS_HURRICANE(u))) && \
	 (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                      \
        d.vrf_id = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_0f); \
    } else {                                                                 \
        d.vrf_id = 0;                                                        \
    }                                                                        \
    d.index = (tab_index << 1);                                              \
    if (shr_avl_insert(SOC_LPM_STATE_AVL(u),                                 \
                       soc_fb_lpm_avl_compare_key,                           \
                       (shr_avl_datum_t *)&d)) {                             \
    }                                                                        \
}

#define LPM_AVL_INSERT_IPV4_1(u, entry_data, tab_index)                      \
{                                                                            \
    soc_lpm_avl_state_t     d;                                               \
    d.ip0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);        \
    d.mask0 =                                                                \
        SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);       \
    d.ip1 = 0;                                                               \
    d.mask1 = 0x80000001;                                                    \
    if ((!(SOC_IS_HURRICANE(u))) && \
	 (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f))) {                      \
        d.vrf_id = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_1f); \
    } else {                                                                 \
        d.vrf_id = 0;                                                        \
    }                                                                        \
    d.index = (tab_index << 1) + 1;                                          \
    if (shr_avl_insert(SOC_LPM_STATE_AVL(u),                                 \
                       soc_fb_lpm_avl_compare_key,                           \
                       (shr_avl_datum_t *)&d)) {                             \
    }                                                                        \
}

#define LPM_AVL_INSERT(u, entry_data, tab_index)                        \
        soc_fb_lpm_avl_insert(u, entry_data, tab_index)
#define LPM_AVL_DELETE(u, entry_data, tab_index)                        \
        soc_fb_lpm_avl_delete(u, entry_data)
#define LPM_AVL_LOOKUP(u, key_data, pfx, tab_index)                     \
        soc_fb_lpm_avl_lookup(u, key_data, tab_index)


void soc_fb_lpm_avl_insert(int u, void *entry_data, uint32 tab_index)
{
    uint8 mode;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        mem  = L3_DEFIP_LEVEL1m;
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f,_key_data);
        mode = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        mode = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);
    }
    if (mode) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            LPM_AVL_INSERT_IPV6(u, entry_data, tab_index)
        }
    } else {
        /* IPV4 entry */                                                
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            LPM_AVL_INSERT_IPV4_1(u, entry_data, tab_index)
        }
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            LPM_AVL_INSERT_IPV4_0(u, entry_data, tab_index)
        }
    }
}

void soc_fb_lpm_avl_delete(int u, void *key_data)
{
    soc_lpm_avl_state_t     d;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f, _key_data);
        d.ip0 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        d.vrf_id = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf);
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, MASK0f, _key_data);
        d.mask0 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        if (soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf)) {
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY1f, _key_data);
            d.ip1 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
           SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, MASK1f, _key_data);
            d.mask1 =
                SOC_MEM_OPT_F32_GET(u, L3_DEFIP_LEVEL1m, key_data, IP_ADDR1f);
        } else {
            d.ip1 = 0;
            d.mask1 = 0x80000001;
        }
    } else
#endif
    {
        d.ip0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, IP_ADDR0f);
        d.mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, IP_ADDR_MASK0f);

        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
            d.vrf_id = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, VRF_ID_0f);
        } else {
            d.vrf_id = 0;
        }
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field)) {
            d.ip1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, IP_ADDR1f);
            d.mask1 =
                SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, IP_ADDR_MASK1f);
        } else {
            d.ip1 = 0;
            d.mask1 = 0x80000001;
        }
    }
    if (shr_avl_delete(SOC_LPM_STATE_AVL(u),
                       soc_fb_lpm_avl_compare_key,
                       (shr_avl_datum_t *)&d)) {
    }
}

int soc_fb_lpm_avl_lookup(int u, void *key_data, int *tab_index)
{
    soc_lpm_avl_state_t     d;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f, _key_data);
        d.ip0 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        d.vrf_id = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf);

        /* Entry Type: IPv4/IPv6 */
        if (soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf) != 0) {
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY1f, _key_data);
            d.ip1 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, MASK1f, _key_data);
            d.mask1 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        } else {
            d.ip1 = 0;
            d.mask1 = 0x80000001;
        }
    } else
#endif
    {

        d.ip0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, IP_ADDR0f);
        d.mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm,  key_data, IP_ADDR_MASK0f);

        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
            d.vrf_id = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, VRF_ID_0f);
        } else {
            d.vrf_id = 0;
        }

        /* Entry Type: IPv4/IPv6 */
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field)) {
            d.ip1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm,  key_data, IP_ADDR1f);
            d.mask1 =
                SOC_MEM_OPT_F32_GET(u, L3_DEFIPm,  key_data, IP_ADDR_MASK1f);
        } else {
            d.ip1 = 0;
            d.mask1 = 0x80000001;
        }
    }
    if (shr_avl_lookup(SOC_LPM_STATE_AVL(u),
                       soc_fb_lpm_avl_compare_key,
                       (shr_avl_datum_t *)&d)) {
        *tab_index = d.index;
    } else {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}
#else
#define LPM_AVL_INSERT(u, entry_data, tab_index)
#define LPM_AVL_DELETE(u, entry_data, tab_index)
#define LPM_AVL_LOOKUP(u, key_data, pfx, tab_index)
#endif /* FB_LPM_AVL_SUPPORT */

/*
 * LPM functionality has 4 HW lookups to perform any of the below lookups
 *          V4/V6-128B DIP
 *          V4/V6-64B  DIP
 *          V4/V6-128B SIP
 *          V4/V6-64B  SIP
 * 1) Legcay devices always had 2 TCAM units per LPM lookup.
 *    Moving across parts of lookup could be done by adding TCAM depth.
 *
 *                   LOOKUP in Legacy devices
 *                  --------------------------
 *                  |                        |
 *                  |   -------    -------   |
 *                  |   |     |    |     |   |
 *                  |   |  0  |    |  1  |   |
 *                  |   |     |    |     |   |
 *                  |   -------    -------   |
 *                  |                        |
 *                  --------------------------
 * 2) With UTT feature, each LPM lookup can have more than 2 TCAM units.
 *    This number changes based on chip and number of v6 entries layout.
 *    To make movement across lookups and minimize the changes in the
 *    scaling algo, each lookup will be visualized as 2 logical TCAMs.
 *    Logical TCAM's size in a lookup is same
 *    but can be different across lookups.
 *    Layout for logical Lookup 0 with "n" TCAMs and
 *    logical lookup 1 with "m" TCAMs is shown below.
 *    Logical tcam numbers will still be in range [0-7].
 *
 *                          LOGICAL LOOKUP's in UTT devices
 *                -------------------------------------------------
 *                |                                               |
 *                |     LOGICAL TCAM 0        LOGICAL TCAM 1      |
 *                |  --------------------  ---------------------  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      0      | |  |  |    (n/2)    |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      1      | |  |  |   (n/2+1)   |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |         .        |  |         .         |  |
 *                |  |         .        |  |         .         |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |   (n/2-1)   | |  |  |     n-1     |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  --------------------  ---------------------  |
 *                |                                               |
 *                -------------------------------------------------
 *
 *                -------------------------------------------------
 *                |                                               |
 *                |     LOGICAL TCAM 2        LOGICAL TCAM 3      |
 *                |  --------------------  ---------------------  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      n      | |  |  |   (n+m/2)   |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |     n+1     | |  |  |  (n+m/2+1)  |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |         .        |  |         .         |  |
 *                |  |         .        |  |         .         |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |  (n+m/2-1)  | |  |  |    n+m-1    |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  --------------------  ---------------------  |
 *                |                                               |
 *                -------------------------------------------------
 */
#define LPM128_TCAM_NUM(u, i)              (_soc_fb_lpm128_tcam_num(u, i))
#define LPM128_IN_ODD_TCAM(u, i)           (LPM128_TCAM_NUM(u, i) & 1)
#define LPM128_IN_EVEN_TCAM(u, i)          (!LPM128_IN_ODD_TCAM(u, i))
#define LPM128_IN_ODD_TCAM_FIRST(u, i)     \
                  (_soc_fb_lpm128_in_odd_tcam_first(u, i))
#define LPM128_IN_EVEN_TCAM_FIRST(u, i)    \
                  (_soc_fb_lpm128_in_even_tcam_first(u, i))

STATIC INLINE int
_soc_fb_lpm128_tcam_num(int u, int index)
{
#ifdef BCM_UTT_SUPPORT
   if (soc_feature(u, soc_feature_utt)) {
       return soc_utt_lpm128_logical_tcam_num(u, index);
   } else
#endif
   {
       return (index / SOC_L3_DEFIP_TCAM_DEPTH_GET(u));
   }
}

STATIC INLINE int
_soc_fb_lpm128_in_odd_tcam_first(int u, int index)
{
    int cond = 0;
#ifdef BCM_UTT_SUPPORT
   if (soc_feature(u, soc_feature_utt)) {
       cond = soc_utt_lpm128_is_logical_tcam_first_entry(u, index);
   } else
#endif
   {
       cond = ((index % SOC_L3_DEFIP_TCAM_DEPTH_GET(u)) == 0);
   }
   return (LPM128_IN_ODD_TCAM(u, index) && cond);
}

STATIC INLINE int
_soc_fb_lpm128_in_even_tcam_first(int u, int index)
{
    int cond = 0;
#ifdef BCM_UTT_SUPPORT
   if (soc_feature(u, soc_feature_utt)) {
       cond = soc_utt_lpm128_is_logical_tcam_first_entry(u, index);
   } else
#endif
   {
       cond = ((index % SOC_L3_DEFIP_TCAM_DEPTH_GET(u)) == 0);
   }
   return (LPM128_IN_EVEN_TCAM(u, index) && cond);
}


/*
 * Function:
 *      soc_fb_lpm_paired_offset
 * Purpose:
 *      Computes paired index second part.
 */
STATIC int
soc_fb_lpm_paired_offset(int u, int index)
{
#ifdef BCM_UTT_SUPPORT
    if (soc_feature(u, soc_feature_utt)) {
        return soc_utt_lpm128_paired_offset(u, index);
    } else
#endif
    {
        return SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    }
}

/* #define FB_LPM_DEBUG */
#ifdef FB_LPM_HASH_SUPPORT
#define _SOC_LPM128_HASH_KEY_LEN_IN_WORDS 10
typedef struct _soc_fb_lpm_hash_s {
    int         unit;
    int         entry_count;    /* Number entries in hash table */
    int         index_count;    /* Hash index max value + 1 */
    uint32      *table;         /* Hash table with 32 bit index */
    uint32      *link_table;    /* To handle collisions */
#ifdef FB_LPM_DEBUG
    uint32      *check_table;   /* To help sanity check */
#endif
} _soc_fb_lpm_hash_t;

typedef uint32 _soc_fb_lpm_hash_entry_t[6];
typedef uint32 _soc_fb_lpm128_hash_entry_t[_SOC_LPM128_HASH_KEY_LEN_IN_WORDS];
typedef int (*_soc_fb_lpm_hash_compare_fn)(_soc_fb_lpm_hash_entry_t key1,
                                           _soc_fb_lpm_hash_entry_t key2);
typedef int (*_soc_fb_lpm128_hash_compare_fn)(_soc_fb_lpm128_hash_entry_t key1,
                                           _soc_fb_lpm128_hash_entry_t key2);
static _soc_fb_lpm_hash_t *_fb_lpm_hash_tab[SOC_MAX_NUM_DEVICES];
static _soc_fb_lpm_hash_t *_fb_lpm128_hash_tab[SOC_MAX_NUM_DEVICES];

static uint32 fb_lpm_hash_index_mask[SOC_MAX_NUM_DEVICES] = {0};

#define SOC_LPM_STATE_HASH(u)           (_fb_lpm_hash_tab[(u)])
#define SOC_LPM128_STATE_HASH(u)           (_fb_lpm128_hash_tab[(u)])

#if defined(BCM_TOMAHAWK3_SUPPORT)
#define SOC_TH3_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr, odata) {       \
        uint32 _key_data[2] = {0, 0};                                                             \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data_upr, KEY1f, _key_data);  \
        odata[0] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data_upr, MASK1f, _key_data); \
        odata[1] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data_upr, KEY0f, _key_data);  \
        odata[2] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, entry_data_upr,       \
                                                                              IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data_upr, MASK0f, _key_data);        \
        odata[3] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY1f, _key_data);      \
        odata[4] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK1f, _key_data);     \
        odata[5] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);      \
        odata[6] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, entry_data_upr,       \
                                                                              IP_ADDRf); \
        odata[8] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, entry_data_upr,       \
                                                                              VRF_IDf);  \
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK0f, _key_data);     \
        odata[7] = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf); \
    }

#define SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)             \
    uint32 _key_data[2] = {0, 0};                                                     \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data); \
    odata[0] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    soc_fb_lpm_hash_vrf_0_get(u, entry_data, &odata[5]);                     \
    odata[4] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf); \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK0f, _key_data); \
    odata[1] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY1f, _key_data); \
    odata[2] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);   \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK1f, _key_data);\
    odata[3] =                                                                  \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);   \

#define SOC_TH3_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, odata)           \
    uint32 _key_data[2] = {0, 0};                                                     \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data); \
    odata[0] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    odata[4] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf); \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK0f, _key_data); \
    odata[1] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    odata[2] = 0;                                                            \
    odata[3] = 0x80000001;                                                   \
    soc_fb_lpm_hash_vrf_0_get(u, entry_data, &odata[5]);

#define SOC_TH3_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, odata)           \
    uint32 _key_data[2] = {0, 0};                                                     \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY1f, _key_data);       \
    odata[0] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    odata[4] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf); \
    SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, MASK1f, _key_data);     \
    odata[1] =                                                               \
        soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);\
    odata[2] = 0;                                                            \
    odata[3] = 0x80000001;                                                   \
    soc_fb_lpm_hash_vrf_1_get(u, entry_data, &odata[5]);
#else
#define SOC_TH3_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, odata)
#define SOC_TH3_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, odata)
#define SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)
#define SOC_TH3_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr, odata)
#endif

#define SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr, odata)         \
    if (SOC_IS_TOMAHAWK3(u)) {                                                          \
        SOC_TH3_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr, odata)        \
    } else {                                                                            \
        odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data_upr, IP_ADDR1f);        \
        odata[1] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data_upr, IP_ADDR_MASK1f);   \
        odata[2] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data_upr, IP_ADDR0f);        \
        odata[3] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data_upr, IP_ADDR_MASK0f);   \
        odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);            \
        odata[5] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);       \
        odata[6] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);            \
        odata[7] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);\
        if ( (!(SOC_IS_HURRICANE(u))) &&                                         \
            (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                \
            odata[8] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_0f); \
            soc_fb_lpm_hash_vrf_0_get(u, entry_data, &odata[9]);                 \
        } else {                                                                 \
            odata[8] = 0;                                                        \
            odata[9] = 0;                                                        \
        }                                                                        \
    }

#define SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)                     \
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {                                 \
        SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)                    \
    } else {                                                                     \
        odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);     \
        odata[1] =                                                               \
            SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
        odata[2] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);     \
        odata[3] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);\
        if ( (!(SOC_IS_HURRICANE(u))) &&                                         \
            (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                \
            odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_0f); \
            soc_fb_lpm_hash_vrf_0_get(u, entry_data, &odata[5]);                 \
        } else {                                                                 \
            odata[4] = 0;                                                        \
            odata[5] = 0;                                                        \
        }                                                                        \
    }

#define SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, odata)               \
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {                                 \
        SOC_TH3_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, odata)                 \
    } else {                                                                     \
        odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR0f);     \
        odata[1] =                                                               \
            SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK0f);       \
        odata[2] = 0;                                                            \
        odata[3] = 0x80000001;                                                   \
        if ((!(SOC_IS_HURRICANE(u))) &&                                          \
            (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f))) {                \
            odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_0f); \
            soc_fb_lpm_hash_vrf_0_get(u, entry_data, &odata[5]);                 \
        } else {                                                                 \
            odata[4] = 0;                                                        \
            odata[5] = 0;                                                        \
        }                                                                        \
    }

#define SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, odata)               \
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {                                 \
        SOC_TH3_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, odata)               \
    } else {                                                                     \
        odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR1f);     \
        odata[1] =                                                               \
            SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_MASK1f);       \
        odata[2] = 0;                                                            \
        odata[3] = 0x80000001;                                                   \
        if ((!(SOC_IS_HURRICANE(u))) &&  \
            (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f))) {                \
            odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_ID_1f); \
            soc_fb_lpm_hash_vrf_1_get(u, entry_data, &odata[5]);                 \
        } else {                                                                 \
            odata[4] = 0;                                                        \
            odata[5] = 0;                                                        \
        }                                                                        \
    }
#ifdef FB_LPM_DEBUG
#define INDEX_ADD_DEBUG(hash, hash_idx, new_idx)                        \
    if (hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] != 0) {     \
        LOG_CLI(("INDEX ADD new_idx %d hash_idx %d check_idx %d\n",     \
        new_idx, hash_idx, hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK]));\
        soc_fb_lpm_debug_collect(hash->unit);                           \
    } else {                                                            \
        hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] = 1;        \
    }

#define INDEX_UPDATE_DEBUG(hash, hash_idx, old_idx, new_idx)            \
    if (hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] != 0 ||     \
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK] == 0) {     \
        LOG_CLI(("INDEX UPDATE new_idx %d old_idx %d hash_idx %d check_idx %d %d\n", \
        new_idx, old_idx, hash_idx, hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK],\
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK]));          \
        soc_fb_lpm_debug_collect(hash->unit);                           \
    } else {                                                            \
        hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] = 1;        \
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK] = 0;        \
    }

#define INDEX_UPDATE_LINK_DEBUG(hash, prev_idx, old_idx, new_idx)       \
    if (hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] != 0 ||     \
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK] == 0 ||     \
        hash->check_table[prev_idx & FB_LPM_HASH_INDEX_MASK] == 0) {    \
        LOG_CLI(("INDEX UPDATE_LINK new_idx %d old_idx %d prev_idx %d " \
                 "check_idx %d %d %d\n",                                \
        new_idx, old_idx, prev_idx,                                     \
        hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK],            \
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK],            \
        hash->check_table[prev_idx & FB_LPM_HASH_INDEX_MASK]));         \
        soc_fb_lpm_debug_collect(hash->unit);                           \
    } else {                                                            \
        hash->check_table[new_idx & FB_LPM_HASH_INDEX_MASK] = 1;        \
        hash->check_table[old_idx & FB_LPM_HASH_INDEX_MASK] = 0;        \
    }

#define INDEX_DELETE_DEBUG(hash, hash_idx, del_idx)                     \
    if (hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK] == 0) {     \
        LOG_CLI(("INDEX DELETE del_idx %d hash_idx %d check_idx %d\n",  \
        del_idx, hash_idx,                                              \
        hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK]));          \
        soc_fb_lpm_debug_collect(hash->unit);                           \
    } else {                                                            \
        hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK] = 0;        \
    } 

#define INDEX_DELETE_LINK_DEBUG(hash, prev_idx, del_idx)                \
    if (hash->check_table[prev_idx & FB_LPM_HASH_INDEX_MASK] == 0 ||    \
        hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK] == 0) {     \
        LOG_CLI(("INDEX DELETE LINK prev_idx %d del_idx %d check_idx %d %d\n", \
        prev_idx, del_idx,                                              \
        hash->check_table[prev_idx & FB_LPM_HASH_INDEX_MASK],           \
        hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK]));          \
        soc_fb_lpm_debug_collect(hash->unit);                           \
    } else {                                                            \
        hash->check_table[del_idx & FB_LPM_HASH_INDEX_MASK] = 0;        \
    }
#else
#define INDEX_ADD_DEBUG(hash, hash_idx, new_idx)
#define INDEX_UPDATE_DEBUG(hash, hash_idx, old_idx, new_idx)
#define INDEX_UPDATE_LINK_DEBUG(hash, prev_idx, old_idx, new_idx)
#define INDEX_DELETE_DEBUG(hash, hash_idx, del_idx)
#define INDEX_DELETE_LINK_DEBUG(hash, prev_idx, del_idx)
#endif

#define INDEX_ADD(hash, hash_idx, new_idx)                      \
    INDEX_ADD_DEBUG(hash, hash_idx, new_idx)                    \
    hash->link_table[new_idx & FB_LPM_HASH_INDEX_MASK] =        \
        hash->table[hash_idx];                                  \
    hash->table[hash_idx] = new_idx

#define INDEX_ADD_LINK(hash, t_index, new_idx)                  \
    hash->link_table[new_idx & FB_LPM_HASH_INDEX_MASK] =        \
        hash->link_table[t_index & FB_LPM_HASH_INDEX_MASK];     \
    hash->link_table[t_index & FB_LPM_HASH_INDEX_MASK] = new_idx

#define INDEX_UPDATE(hash, hash_idx, old_idx, new_idx)          \
    INDEX_UPDATE_DEBUG(hash, hash_idx, old_idx, new_idx)        \
    hash->table[hash_idx] = new_idx;                            \
    hash->link_table[new_idx & FB_LPM_HASH_INDEX_MASK] =        \
        hash->link_table[old_idx & FB_LPM_HASH_INDEX_MASK];     \
    hash->link_table[old_idx & FB_LPM_HASH_INDEX_MASK] = FB_LPM_HASH_INDEX_NULL

#define INDEX_UPDATE_LINK(hash, prev_idx, old_idx, new_idx)             \
    INDEX_UPDATE_LINK_DEBUG(hash, prev_idx, old_idx, new_idx)           \
    hash->link_table[prev_idx & FB_LPM_HASH_INDEX_MASK] = new_idx;      \
    hash->link_table[new_idx & FB_LPM_HASH_INDEX_MASK] =                \
        hash->link_table[old_idx & FB_LPM_HASH_INDEX_MASK];             \
    hash->link_table[old_idx & FB_LPM_HASH_INDEX_MASK] = FB_LPM_HASH_INDEX_NULL

#define INDEX_DELETE(hash, hash_idx, del_idx)                   \
    INDEX_DELETE_DEBUG(hash, hash_idx, del_idx)                 \
    hash->table[hash_idx] =                                     \
        hash->link_table[del_idx & FB_LPM_HASH_INDEX_MASK];     \
    hash->link_table[del_idx & FB_LPM_HASH_INDEX_MASK] =        \
        FB_LPM_HASH_INDEX_NULL

#define INDEX_DELETE_LINK(hash, prev_idx, del_idx)              \
    INDEX_DELETE_LINK_DEBUG(hash, prev_idx, del_idx)            \
    hash->link_table[prev_idx & FB_LPM_HASH_INDEX_MASK] =       \
        hash->link_table[del_idx & FB_LPM_HASH_INDEX_MASK];     \
    hash->link_table[del_idx & FB_LPM_HASH_INDEX_MASK] =        \
        FB_LPM_HASH_INDEX_NULL

/*
 * Function:
 *      soc_fb_lpm_hash_vrf_0_get and soc_fb_lpm_hash_vrf_1_get
 * Purpose:
 *      Service routine used to determine the vrf type 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_entry - (IN)Route info buffer from hw.
 *      vrf_id    - (OUT)Virtual router id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
soc_fb_lpm_hash_vrf_0_get(int unit, void *lpm_entry, uint32 *vrf)
{
    int vrf_id;

    /* Get Virtual Router id if supported. */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        int vrf_level;
        SOC_TH3_MEM_OPT_F64_GET(unit, L3_DEFIP_LEVEL1m, lpm_entry, FIXED_DATA0f, _key_data);
        vrf_level = soc_format_field32_get(unit, FIXED_DATAfmt, _key_data, SUB_DB_PRIORITYf);

        /* Special vrf's handling. */
        if (1 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_SPECIFIC;
        } else if (2 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_GLOBAL;
        } else if (0 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_OVERRIDE;
        } else {
            *vrf = _SOC_HASH_L3_VRF_DEFAULT;
        }
    } else 
#endif
    if (SOC_MEM_OPT_FIELD_VALID(unit,L3_DEFIPm, VRF_ID_MASK0f)){
        vrf_id = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f);

        /* Special vrf's handling. */
        if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f)) {
            *vrf = _SOC_HASH_L3_VRF_SPECIFIC;
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = _SOC_HASH_L3_VRF_GLOBAL;
        } else {
            *vrf = _SOC_HASH_L3_VRF_OVERRIDE;
            if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_ROUTE0f)) {
                if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f)) {
                    *vrf = _SOC_HASH_L3_VRF_GLOBAL;
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf = _SOC_HASH_L3_VRF_DEFAULT;
    }
}

STATIC void
soc_fb_lpm_hash_vrf_1_get(int unit, void *lpm_entry, uint32 *vrf)
{
    int vrf_id;

    /* Get Virtual Router id if supported. */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        int vrf_level;
        SOC_TH3_MEM_OPT_F64_GET(unit, L3_DEFIP_LEVEL1m, lpm_entry, FIXED_DATA1f, _key_data);
        vrf_level = soc_format_field32_get(unit, FIXED_DATAfmt, _key_data, SUB_DB_PRIORITYf);

        /* Special vrf's handling. */
        if (1 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_SPECIFIC;
        } else if (2 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_GLOBAL;
        } else if (0 == vrf_level) {
            *vrf = _SOC_HASH_L3_VRF_OVERRIDE;
            if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_ROUTE0f)) {
                if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f)) {
                    *vrf = _SOC_HASH_L3_VRF_GLOBAL;
                }
            }
        } else {
            *vrf = _SOC_HASH_L3_VRF_DEFAULT;
        }
    } else
#endif
    if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, VRF_ID_MASK1f)){
        vrf_id = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_1f);

        /* Special vrf's handling. */
        if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK1f)) {
            *vrf = _SOC_HASH_L3_VRF_SPECIFIC;    
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = _SOC_HASH_L3_VRF_GLOBAL;
        } else {
            *vrf = _SOC_HASH_L3_VRF_OVERRIDE;    
            if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_ROUTE1f)) {
                if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE1f)) {
                    *vrf = _SOC_HASH_L3_VRF_GLOBAL; 
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf = _SOC_HASH_L3_VRF_DEFAULT;
    }
}


#define SOC_FB_LPM_HASH_ENTRY_GET _soc_fb_lpm_hash_entry_get
#define SOC_FB_LPM128_HASH_ENTRY_GET _soc_fb_lpm128_hash_entry_get

static
void _soc_fb_lpm_hash_entry_get(int u, void *e,
                                int index, _soc_fb_lpm_hash_entry_t r_entry,
                                int *v);
static
unsigned int _soc_fb_lpm_hash_compute(uint8 *data, int data_nbits);
static
int _soc_fb_lpm_hash_create(int unit,
                            int entry_count,
                            int index_count,
                            _soc_fb_lpm_hash_t **fb_lpm_hash_ptr);
static
int _soc_fb_lpm_hash_destroy(_soc_fb_lpm_hash_t *fb_lpm_hash);
static
int _soc_fb_lpm_hash_lookup(_soc_fb_lpm_hash_t          *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t    entry,
                            int                         pfx,
                            uint32                      *key_index);
static
int _soc_fb_lpm_hash_insert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            int    pfx,
                            uint32 old_index,
                            uint32 new_index,
                            uint32 *rvt_index);
static
int _soc_fb_lpm_hash_revert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            uint32 new_index,
                            uint32 rvt_index);

static
int _soc_fb_lpm128_hash_insert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t entry,
                            int    pfx,
                            uint32 old_index,
                            uint32 new_index,
                            uint32 *rvt_index);

static
int _soc_fb_lpm128_hash_revert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t entry,
                            uint32 new_index,
                            uint32 rvt_index);


static
int _soc_fb_lpm_hash_delete(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            int    pfx,
                            uint32 delete_index);

/*
 *      Extract key data from an entry at the given index.
 */
static
void _soc_fb_lpm_hash_entry_get(int u, void *e,
                                int index, _soc_fb_lpm_hash_entry_t r_entry,
                                int *v)
{
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    if (index & FB_LPM_HASH_IPV6_MASK) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
            SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, e, r_entry);
        } else
#endif
        {
            SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, e, r_entry);
        }
        if (v) {
            *v =  SOC_MEM_OPT_F32_VALID0_GET(u, mem, e) &&
                  SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
        }
    } else {
        if (index & 0x1) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, e, r_entry);
            if (v) {
                *v =  SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
            }
        } else {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, e, r_entry);
            if (v) {
                *v =  SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
            }
        }
    }
}


/*
 * Function:
 *      _soc_fb_lpm_hash_compare_key
 * Purpose:
 *      Comparison function for AVL shadow table operations.
 */
static
int _soc_fb_lpm_hash_compare_key(_soc_fb_lpm_hash_entry_t key1,
                                 _soc_fb_lpm_hash_entry_t key2)
{
    int idx;

    for (idx = 0; idx < 6; idx++) { 
        SOC_MEM_COMPARE_RETURN(key1[idx], key2[idx]);
    }
    return (0);
}

/*
 * Function:
 *      _soc_fb_lpm_hash_compare_key
 * Purpose:
 *      Comparison function for AVL shadow table operations.
 */
static
int _soc_fb_lpm128_hash_compare_key(_soc_fb_lpm128_hash_entry_t key1,
                                 _soc_fb_lpm128_hash_entry_t key2)
{
    int idx;

    for (idx = 0; idx < _SOC_LPM128_HASH_KEY_LEN_IN_WORDS; idx++) { 
        SOC_MEM_COMPARE_RETURN(key1[idx], key2[idx]);
    }
    return (0);
}


#define SOC_LPM_STATE_S(lpm_state, pfx) (lpm_state[pfx].start)
#define SOC_LPM_STATE_E(lpm_state, pfx) (lpm_state[pfx].end)
#define SOC_LPM_STATE_P(lpm_state, pfx) (lpm_state[pfx].prev)
#define SOC_LPM_STATE_N(lpm_state, pfx) (lpm_state[pfx].next)
#define SOC_LPM_STATE_V(lpm_state, pfx) (lpm_state[pfx].vent)
#define SOC_LPM_STATE_F(lpm_state, pfx) (lpm_state[pfx].fent)

#ifdef FB_LPM_DEBUG      
#define H_INDEX_MATCH(str, tab_index, match_index)      \
    LOG_INFO(BSL_LS_SOC_LPM, \
              (BSL_META("%s index: H %d A %d\n"),                  \
               str, (int)tab_index, match_index))
int *dbg_soc_lpm128_index_to_pfx_group[SOC_MAX_NUM_DEVICES];
soc_lpm128_state_p dbg_soc_lpm128_state[SOC_MAX_NUM_DEVICES];
soc_lpm128_state_p dbg_soc_lpm128_unreserved_state[SOC_MAX_NUM_DEVICES];
defip_entry_t *dbg_l3_defip[SOC_MAX_NUM_DEVICES];
static _soc_fb_lpm_hash_t *_dbg_fb_lpm_hash_tab[SOC_MAX_NUM_DEVICES];
static _soc_fb_lpm_hash_t *_dbg_fb_lpm128_hash_tab[SOC_MAX_NUM_DEVICES];
soc_lpm_state_p dbg_soc_lpm_state[SOC_MAX_NUM_DEVICES];

#define DBG_SOC_LPM128_STATE(u)             \
        (dbg_soc_lpm128_state[(u)])
#define DBG_SOC_LPM128_UNRESERVED_STATE(u)  \
        (dbg_soc_lpm128_unreserved_state[(u)])
#define DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) \
        dbg_soc_lpm128_index_to_pfx_group[(u)]
#define DBG_SOC_LPM128_INDEX_TO_PFX_GROUP(u, index)\
        dbg_soc_lpm128_index_to_pfx_group[(u)][(index)]
#define DBG_SOC_L3_DEFIP(u)\
        dbg_l3_defip[(u)]
#define DBG_SOC_LPM_STATE_HASH(u)           (_dbg_fb_lpm_hash_tab[(u)])
#define DBG_SOC_LPM128_STATE_HASH(u)        (_dbg_fb_lpm128_hash_tab[(u)])
#define DBG_SOC_LPM_STATE(u)                (dbg_soc_lpm_state[(u)])

int *dbg_soc_lpm128_index_to_pfx_group2[SOC_MAX_NUM_DEVICES];
soc_lpm128_state_p dbg_soc_lpm128_state2[SOC_MAX_NUM_DEVICES];
soc_lpm128_state_p dbg_soc_lpm128_unreserved_state2[SOC_MAX_NUM_DEVICES];
static _soc_fb_lpm_hash_t *_dbg_fb_lpm_hash_tab2[SOC_MAX_NUM_DEVICES];
static _soc_fb_lpm_hash_t *_dbg_fb_lpm128_hash_tab2[SOC_MAX_NUM_DEVICES];
soc_lpm_state_p dbg_soc_lpm_state2[SOC_MAX_NUM_DEVICES];
int *dbg_soc_lpm_hash_tmp[SOC_MAX_NUM_DEVICES];

#define DBG_SOC_LPM128_STATE2(u)             \
        (dbg_soc_lpm128_state2[(u)])
#define DBG_SOC_LPM128_UNRESERVED_STATE2(u)  \
        (dbg_soc_lpm128_unreserved_state2[(u)])
#define DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u) \
        dbg_soc_lpm128_index_to_pfx_group2[(u)]
#define DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(u, index)\
        dbg_soc_lpm128_index_to_pfx_group2[(u)][(index)]
#define DBG_SOC_LPM_STATE_HASH2(u)           (_dbg_fb_lpm_hash_tab2[(u)])
#define DBG_SOC_LPM128_STATE_HASH2(u)        (_dbg_fb_lpm128_hash_tab2[(u)])
#define DBG_SOC_LPM_STATE2(u)                (dbg_soc_lpm_state2[(u)])

#define LPM_TBL_SIZE 16384
#define DBG_SOC_LPM_HASH_TMP(u)          (dbg_soc_lpm_hash_tmp[(u)])
#define DBG_SOC_LPM_HASH_TMP_A(u)        (&dbg_soc_lpm_hash_tmp[(u)][0])
#define DBG_SOC_LPM_HASH_TMP_B(u)        (&dbg_soc_lpm_hash_tmp[(u)][LPM_TBL_SIZE])

#define LPM_HASH_INSERT2(u, entry_data, tab_index, rvt_index0, rvt_index1)   \
    soc_fb_lpm_hash_insert2(u, entry_data, tab_index, LPM_NO_MATCH_INDEX, 0, \
                           rvt_index0, rvt_index1)

void soc_fb_lpm_hash_insert2(int u, void *entry_data, uint32 tab_index,
                            uint32 old_index, int pfx,
                            uint32 *rvt_index0, uint32 *rvt_index1)
{
    _soc_fb_lpm_hash_entry_t    key_hash;
    soc_mem_t mem = L3_DEFIPm;
    int ipv6 = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
        mem = L3_DEFIP_LEVEL1m;
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);
        ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf);
    } else
#endif
    {
        ipv6 = SOC_MEM_OPT_F32_GET(u, mem, entry_data, defip_mode0_field);
    }

    if (ipv6) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
                SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            } else
#endif
            {
                SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            }
            _soc_fb_lpm_hash_insert(
                        DBG_SOC_LPM_STATE_HASH2(u),
                        _soc_fb_lpm_hash_compare_key,
                        key_hash,
                        pfx,
                        old_index,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_insert(DBG_SOC_LPM_STATE_HASH2(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_insert(DBG_SOC_LPM_STATE_HASH2(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}


/* The symptom is sw still has the index, but the correspoinding entry 
 * in hw is empty. So let's add an instrument that when doing a 
 * mem write, and when the to be written entry is empty, then its index
 * should be deleted from sw. 
 */
static
int _soc_fb_lpm_hash_verify(_soc_fb_lpm_hash_t *hash,
                            uint32 new_index, int valid);
void soc_fb_lpm_hash_verify(int u, void *entry_data, uint32 tab_index)
{
    int valid = 0;
    _soc_fb_lpm_hash_t *hash = SOC_LPM128_STATE_HASH(u); 

    soc_mem_t mem = L3_DEFIPm;
    int ipv6 = 0;
    int mode = 0;
    int offset = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
        mem = L3_DEFIP_LEVEL1m;
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);
        ipv6 = mode = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        mode = ipv6 = SOC_MEM_OPT_F32_GET(u, mem, entry_data, defip_mode0_field);
    }

    if (tab_index >= SOC_LPM_STATE_START(u, MAX_PFX_INDEX(u))) {
        hash = SOC_LPM_STATE_HASH(u);
    }

    if (ipv6) {
        /* IPV6 entry */
        valid = (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
                 SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data));

        if ((mode == 3) &&
            LPM128_IN_ODD_TCAM(u, tab_index)) {
            offset = soc_fb_lpm_paired_offset(u, tab_index);
            tab_index -= offset;

            /* Corner case for transient address */
            valid = SOC_MEM_OPT_F32_GET(u, mem, entry_data, VALID1f);
        }
        _soc_fb_lpm_hash_verify( hash,
                    (tab_index << 1) | FB_LPM_HASH_IPV6_MASK, valid);
    } else {
        /* IPV4 entry */
        valid = SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data);
        _soc_fb_lpm_hash_verify(hash, (tab_index << 1), valid);

        valid = SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data);
        _soc_fb_lpm_hash_verify(hash, ((tab_index << 1) + 1), valid);
    }
}


extern void
lpm128_state_dump(int u, int before);

extern void
lpm128_idx_to_pfx_dump(int u, int before);
STATIC INLINE void
_soc_fb_lpm_state_dump(int u, soc_lpm_state_p lpm_state_ptr);
extern void
lpm_hash_dump(int u, int before, int step, uint32 usec);
extern void
lpm128_hash_dump(int u, int before, int step, uint32 usec);
extern void
lpm_count_dump(int u, int before);
extern int
dbg_soc_fb_lpm_reinit_done(int unit, int ipv6);
extern int
dbg_soc_fb_lpm_reinit(int unit, int idx, uint32 *lpm_entry);
extern int
dbg_soc_fb_lpm128_reinit_done(int unit, int ipv6);
extern int
dbg_soc_fb_lpm128_reinit(int u, int idx, uint32 *lpm_entry,
                     uint32 *lpm_entry_upr);
extern STATIC INLINE int
_soc_fb_lpm_state_cmp(int u, soc_lpm_state_p lpm_state_ptr,
                             soc_lpm_state_p lpm_state_ptr2);

extern int sh_process_command(int u,char * c);

#undef WRITE_L3_DEFIPm
#define WRITE_L3_DEFIPm debug_write_l3_defip

void
debug_dump_l3_defip(int u, int index)
{
    soc_mem_entry_dump_if_changed(u, L3_DEFIPm, 
                                  &(DBG_SOC_L3_DEFIP(u)[index]), "LPM");
}

int 
debug_write_l3_defip(int u, int copyno, int index, void *entry_data)
{
    int rv;
    uint32          data0[20] = {0};
    uint32          data1[20] = {0};    
    uint32      read[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int i = 0, j = 0;
    int offset = 0;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    offset = (soc_mem_index_count(u, mem) >> 1);

    rv = soc_mem_read(u, mem, copyno, index, entry_data);
    /* As long as the target entry is empty or half empty, 
     * trigger a sanity check to the sw hash. The idea is 
     * the empty entry should not be resided in the sw hash, because 
     * the sw hash is always updated prior to hw change, whateve the operation
     * is add or delete or replace.
     */
    if(SOC_URPF_STATUS_GET(u)) {
        if (soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            return (SOC_E_NONE);
        } else {
            if (index >= offset) {
                soc_fb_lpm_hash_verify(u, entry_data, index - offset);
            } else {
                soc_fb_lpm_hash_verify(u, entry_data, index);
            }
        }
    } else {
        soc_fb_lpm_hash_verify(u, entry_data, index);
    }
    rv = soc_mem_write(u, mem, copyno, index, entry_data);
    if (rv < 0) {
        LOG_CLI(("L3 Defip write error \n"));
    }
    sal_memcpy(&(DBG_SOC_L3_DEFIP(u)[index]), entry_data, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    rv = soc_mem_read(u, mem, copyno, index, read);
    if (rv < 0) {
        LOG_CLI(("L3 Defip read error \n"));
    }
    data0[i++] = SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, MODE0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR_MASK0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VRF_ID_0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VALID1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, MODE1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR_MASK1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VRF_ID_1f);


    data1[j++] = SOC_MEM_OPT_F32_VALID0_GET(u, mem, read);
    data1[j++] = soc_mem_field32_get(u, mem, read, MODE0f);
    data1[j++] = soc_mem_field32_get(u, mem, read, IP_ADDR0f);
    data1[j++] = soc_mem_field32_get(u, mem, read, IP_ADDR_MASK0f);
    data1[j++] = soc_mem_field32_get(u, mem, read, VRF_ID_0f);
    data1[j++] = soc_mem_field32_get(u, mem, read, VALID1f);
    data1[j++] = soc_mem_field32_get(u, mem, read, MODE1f);
    data1[j++] = soc_mem_field32_get(u, mem, read, IP_ADDR1f);
    data1[j++] = soc_mem_field32_get(u, mem, read, IP_ADDR_MASK1f);
    data1[j++] = soc_mem_field32_get(u, mem, read, VRF_ID_1f);
   
    if (sal_memcmp(data0, data1, sizeof(data0))) {
        LOG_CLI(("Low level write error \n"));
    }
    return rv;
}

#undef READ_L3_DEFIPm
#define READ_L3_DEFIPm debug_read_l3_defip

static void
soc_fb_lpm_debug_collect(uint32 u);

int 
debug_read_l3_defip(int u, int copyno, int index, void *entry_data)
{
    int rv;
    uint32          data0[20] = {0};
    uint32          data1[20] = {0};    
    int i = 0, j = 0;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    rv = soc_mem_read(u, mem, copyno, index, entry_data);
    if (rv < 0) {
        LOG_CLI(("L3 Defip read  error \n"));
    }
    data0[i++] = SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, MODE0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR_MASK0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VRF_ID_0f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VALID1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, MODE1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, IP_ADDR_MASK1f);
    data0[i++] = soc_mem_field32_get(u, mem, entry_data, VRF_ID_1f);


    data1[j++] = SOC_MEM_OPT_F32_VALID0_GET(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]));
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), MODE0f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR0f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR_MASK0f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), VRF_ID_0f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), VALID1f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), MODE1f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR1f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR_MASK1f);
    data1[j++] = soc_mem_field32_get(u, mem, &(DBG_SOC_L3_DEFIP(u)[index]), VRF_ID_1f);

    if (sal_memcmp(data0, data1, sizeof(data0))) {
        soc_mem_entry_dump(u, L3_DEFIPm, entry_data, BSL_LSS_CLI);
        soc_mem_entry_dump(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), BSL_LSS_CLI);
        soc_fb_lpm_debug_collect(u);
        LOG_CLI(("Low level read error \n"));
    }
    return rv;
}

static void
soc_fb_lpm_hash_sanity(uint32 u, int print, int *err_count)
{

    uint32 min, idx, i, j;
    _soc_fb_lpm_hash_t  *hash[2];
    int *tmp[2];
    tmp[0] = DBG_SOC_LPM_HASH_TMP_A(u);
    tmp[1] = DBG_SOC_LPM_HASH_TMP_B(u);
    int unmatch = 0;

    /* Traverse the table, for each head, traverse the link list.
     * Pick the smallest one as the head.
     * Record the head to each member.
     * In the end, the final arrawy will record all the valid index
     * For those indexes with same hash result, they share same head.
     * Generate two arrays 
     */

    sal_memset(DBG_SOC_LPM_HASH_TMP(u), -1, 2 * LPM_TBL_SIZE * sizeof(int));
    hash[0] = DBG_SOC_LPM_STATE_HASH2(u);
    hash[1] = SOC_LPM_STATE_HASH(u);

    for (i = 0; i < _fb_lpm_hash_tab[u]->index_count; i++) {
        for (j = 0; j < 2; j++) {
            if (hash[j]->table[i] != FB_LPM_HASH_INDEX_NULL) {
                min = idx = hash[j]->table[i];
                while (idx != FB_LPM_HASH_INDEX_NULL) {
                    if (idx < min) {
                        min = idx;
                    }
                    idx = hash[j]->link_table[idx & fb_lpm_hash_index_mask[u]];
                }
                idx = hash[j]->table[i];
                while (idx != FB_LPM_HASH_INDEX_NULL) {
                    if (tmp[j][idx & fb_lpm_hash_index_mask[u]] != -1) {
                        if (print) {
                            LOG_CLI(("duplicate idx %d\n", idx));
                        }
                        if (err_count) {
                            (*err_count) ++;
                        }
                    }
                    tmp[j][idx & fb_lpm_hash_index_mask[u]] = min;
                    idx = hash[j]->link_table[idx & fb_lpm_hash_index_mask[u]];
                }
            }
        }
    }

    for (i = 0; i < LPM_TBL_SIZE; i++) {
        if (tmp[0][i] != tmp[1][i]) {
            unmatch = 1;
            if (print) {
                LOG_CLI(("  LPM hash %d: %d != %d \n", i, tmp[0][i], tmp[1][i]));
            }
        }
    }
    if (unmatch == 0) {
        if (print) {
            LOG_CLI(("  LPM hash table compare pass \n"));
        }
    } else {
        if (print) {
            LOG_CLI(("  LPM hash table compare error \n"));
        }
        if (err_count) {
            (*err_count) ++;
        }
    }

    /* LPM 128 starts here */

    sal_memset(DBG_SOC_LPM_HASH_TMP(u), -1, 2 * LPM_TBL_SIZE * sizeof(int));
    hash[0] = DBG_SOC_LPM128_STATE_HASH2(u);
    hash[1] = SOC_LPM128_STATE_HASH(u);

    for (i = 0; i < _fb_lpm_hash_tab[u]->index_count; i++) {
        for (j = 0; j < 2; j++) {
            if (hash[j]->table[i] != FB_LPM_HASH_INDEX_NULL) {
                min = idx = hash[j]->table[i];
                while (idx != FB_LPM_HASH_INDEX_NULL) {
                    if (idx < min) {
                        min = idx;
                    }
                    idx = hash[j]->link_table[idx & FB_LPM_HASH_INDEX_MASK];
                }
                idx = hash[j]->table[i];
                while (idx != FB_LPM_HASH_INDEX_NULL) {
                    if (tmp[j][idx & FB_LPM_HASH_INDEX_MASK] != -1) {
                        if (print) {
                            LOG_CLI(("duplicate idx %d\n", idx));
                        }
                        if (err_count) {
                            (*err_count) ++;
                        }
                    }
                    tmp[j][idx & FB_LPM_HASH_INDEX_MASK] = min;
                    idx = hash[j]->link_table[idx & FB_LPM_HASH_INDEX_MASK];
                }
            }
        }
    }

    unmatch = 0;
    for (i = 0; i < LPM_TBL_SIZE; i++) {
        if (tmp[0][i] != tmp[1][i]) {
            unmatch = 1;
            if (print) {
                LOG_CLI(("  LPM 128 hash %d: %d != %d \n", i, tmp[0][i], tmp[1][i]));
            }
        }
    }
    if (unmatch == 0) {
        if (print) {
            LOG_CLI(("  LPM 128 hash table compare pass \n"));
        }
    } else {
        if (print) {
            LOG_CLI(("  LPM 128 hash table compare error \n"));
        }
        if (err_count) {
            (*err_count) ++;
        }
    }
}


static void 
soc_fb_lpm_debug_parse(uint32 u, int print, int *err_count)
{
    int i, defip_size, paired_size;
    int rv;
    soc_lpm128_state_p lpm_state_ptr = NULL;
    int pfx_state_size = sizeof(soc_lpm128_state_t) * (MAX_PFX128_ENTRIES);
/*    int pfx_state_size2 = sizeof(soc_lpm_state_t) * (MAX_PFX_ENTRIES(u));*/
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    /* Parse the l3_defip table to table & link_table. And then compare */
    /* Reset all parsed data */
    /* Reset 1 lpmstate */

    soc_fb_lpm_table_sizes_get(u, &paired_size, &defip_size);

    for(i = 0; i < MAX_PFX_ENTRIES(u); i++) {
        SOC_LPM_STATE_S(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_E(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_P(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_N(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_V(DBG_SOC_LPM_STATE2(u), i) = 0;
        SOC_LPM_STATE_F(DBG_SOC_LPM_STATE2(u), i) = 0;
    }    
    SOC_LPM_STATE_F(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = defip_size;
    SOC_LPM_STATE_S(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = paired_size;
    SOC_LPM_STATE_E(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = paired_size - 1;


    /* Reset 2 lpm128 state ( Reserved case TBD) */
    lpm_state_ptr = DBG_SOC_LPM128_STATE2(u);
    for(i = 0; i < MAX_PFX128_ENTRIES; i++) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_VENT(u, lpm_state_ptr, i) = 0;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, i) = 0;
    }
    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        int num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            num_ipv6_128b_entries >>= 1;
        }
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              2 * num_ipv6_128b_entries;
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              paired_size;
    }

    /* reset 3 lpm128 hash*/
    for(i = 0; i < DBG_SOC_LPM128_STATE_HASH2(u)->index_count; i++) {
        DBG_SOC_LPM128_STATE_HASH2(u)->table[i] = FB_LPM_HASH_INDEX_NULL;
    }
    for(i = 0; i < DBG_SOC_LPM128_STATE_HASH2(u)->entry_count; i++) {
        DBG_SOC_LPM128_STATE_HASH2(u)->link_table[i] = FB_LPM_HASH_INDEX_NULL;
        DBG_SOC_LPM128_STATE_HASH2(u)->check_table[i] = 0;
    }    

    /* reset 4 lpm hash*/
    for(i = 0; i < DBG_SOC_LPM_STATE_HASH2(u)->index_count; i++) {
        DBG_SOC_LPM_STATE_HASH2(u)->table[i] = FB_LPM_HASH_INDEX_NULL;
    }
    for(i = 0; i < DBG_SOC_LPM_STATE_HASH2(u)->entry_count; i++) {
        DBG_SOC_LPM_STATE_HASH2(u)->link_table[i] = FB_LPM_HASH_INDEX_NULL;
        DBG_SOC_LPM_STATE_HASH2(u)->check_table[i] = 0;
    }    

    /* reset 5 lpm/lpm128 count*/
    SOC_LPM128_STAT_128BV6_COUNT2(u)          =  0;
    SOC_LPM128_STAT_V4_COUNT2(u)              =  0;
    SOC_LPM128_STAT_64BV6_COUNT2(u)           =  0;
    SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT2(u)   =  0;
    SOC_LPM_V4_COUNT2(u)                      =  0;
    SOC_LPM_64BV6_COUNT2(u)                   =  0;
    SOC_LPM_V4_HALF_ENTRY_COUNT2(u)           =  0;

    /* reset 6 idx 2 group */
    sal_memset(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u), -1,
               defip_size * sizeof(int));

    for (i = 0; i < paired_size; i++) {

        /* Make sure entry is valid. */
        if (!SOC_MEM_OPT_F32_VALID0_GET(u, mem, &(DBG_SOC_L3_DEFIP(u)[i])) &&
            !SOC_MEM_OPT_F32_VALID1_GET(u, mem, &(DBG_SOC_L3_DEFIP(u)[i]))) {
            continue;
        }

        if (soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[i]), MODE0f) == 3) {
            if (LPM128_IN_ODD_TCAM(u, i)) {
                continue;
            }
            if (soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[i]), MODE1f) != 3 ||
                soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[i+1024]), MODE0f) != 3 ||
                soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[i+1024]), MODE1f) != 3) {
                assert(0);
            }
            rv = dbg_soc_fb_lpm128_reinit(u, i, &(DBG_SOC_L3_DEFIP(u)[i]), &(DBG_SOC_L3_DEFIP(u)[i+1024]));
            assert(rv == 0);
        } else {
            rv = dbg_soc_fb_lpm128_reinit(u, i, &(DBG_SOC_L3_DEFIP(u)[i]), NULL);
            assert(rv == 0);
        }
    }

    for (i = paired_size; i < soc_mem_index_count(u, L3_DEFIPm); i++) {
        /* Make sure entry is valid. */
        if (!SOC_MEM_OPT_F32_VALID0_GET(u, mem, &(DBG_SOC_L3_DEFIP(u)[i])) &&
            !SOC_MEM_OPT_F32_VALID1_GET(u, mem, &(DBG_SOC_L3_DEFIP(u)[i]))) {
            continue;
        }

        rv = dbg_soc_fb_lpm_reinit(u, i, &(DBG_SOC_L3_DEFIP(u)[i]));
        assert(rv == 0);
    }
    
    dbg_soc_fb_lpm128_reinit_done(u, 1);
    dbg_soc_fb_lpm_reinit_done(u, 0);
    dbg_soc_fb_lpm_reinit_done(u, 1);

    if (SOC_LPM_STATE(u)) {
        if (_soc_fb_lpm_state_cmp(u, SOC_LPM_STATE(u), DBG_SOC_LPM_STATE2(u))) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "  LPM state compare error \n")));
            if (err_count) {
                (*err_count)++;
            }
        }
    }

    if (SOC_LPM128_STATE(u)) {
        if (sal_memcmp(SOC_LPM128_STATE(u),
                   DBG_SOC_LPM128_STATE2(u),
                   pfx_state_size)) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "  LPM128 state compare error \n")));
            if (err_count) {
                (*err_count)++;
            }
        }
    }

    if (SOC_LPM128_UNRESERVED_STATE(u)) {
        if (sal_memcmp(SOC_LPM128_UNRESERVED_STATE(u),
                   DBG_SOC_LPM128_UNRESERVED_STATE2(u),
                   pfx_state_size)) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "  LPM128 unres state compare error \n")));
        }
    } 

    if (SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u)) {
        if (sal_memcmp(SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u),
                   DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u),
                   (paired_size * sizeof(int)))) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "  LPM128 idx 2 pfx compare error \n")));
            if (err_count) {
                (*err_count)++;
            }
        }
    }

    if (SOC_LPM128_STAT_128BV6_COUNT2(u) != SOC_LPM128_STAT_128BV6_COUNT(u) ||
        SOC_LPM128_STAT_V4_COUNT2(u) != SOC_LPM128_STAT_V4_COUNT(u) ||
        SOC_LPM128_STAT_64BV6_COUNT2(u) != SOC_LPM128_STAT_64BV6_COUNT(u) ||
        SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT2(u) != SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u) ||
        SOC_LPM_V4_COUNT2(u) != SOC_LPM_V4_COUNT(u) ||
        SOC_LPM_64BV6_COUNT2(u) != SOC_LPM_64BV6_COUNT(u) ||
        SOC_LPM_V4_HALF_ENTRY_COUNT2(u) != SOC_LPM_V4_HALF_ENTRY_COUNT(u)) {

        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "  Count mismatch \n")));
        if (err_count) {
            (*err_count)++;
        }
    }

    if (print) {
        lpm128_state_dump(u, 2);
        lpm128_idx_to_pfx_dump(u, 2);
        _soc_fb_lpm_state_dump(u, DBG_SOC_LPM_STATE2(u));
        lpm_hash_dump(u, 2, 100, 0);
        lpm128_hash_dump(u, 2, 100, 0);
        lpm_count_dump(u, 2);
    }
}
static void
soc_fb_lpm_debug_collect(uint32 u)
{
    int err_count1 = 0, err_count2 = 0;
    LOG_CLI((BSL_META_U(u, "\n==Before==\n")));
    lpm128_state_dump(u, 1);
    lpm128_idx_to_pfx_dump(u, 1);
    _soc_fb_lpm_state_dump(u, DBG_SOC_LPM_STATE(u));
    lpm_hash_dump(u, 1, 100, 0);
    lpm128_hash_dump(u, 1, 100, 0);
    lpm_count_dump(u, 1);


    LOG_CLI((BSL_META_U(u, "\n==Current==\n")));
    lpm128_state_dump(u, 0);
    lpm128_idx_to_pfx_dump(u, 0);
    _soc_fb_lpm_state_dump(u, 0);
    lpm_hash_dump(u, 0, 100, 0);
    lpm128_hash_dump(u, 0, 100, 0);
    lpm_count_dump(u, 0);

    LOG_CLI((BSL_META_U(u, "\n==Parsed==\n")));
    soc_fb_lpm_debug_parse(u, 1, &err_count1);
    soc_fb_lpm_hash_sanity(u, 1, &err_count2);
    LOG_CLI((" Err1 %d Err2 %d\n", err_count1, err_count2));

    if (DBG_SOC_L3_DEFIP(u)) {
        int rv;
        uint32          data0[20] = {0};
        uint32          data1[20] = {0};    
        uint32      read[SOC_MAX_MEM_FIELD_WORDS] = {0};
        int i = 0, j = 0, mismatch = 0, index;

        for (index = 0; index < soc_mem_index_count(u, L3_DEFIPm); index++) {
            i = j = 0;
            rv = soc_mem_read(u, L3_DEFIPm, MEM_BLOCK_ANY, index, read);
            if (rv < 0) {
                LOG_CLI(("L3 Defip read error %d \n", index));
                continue;
            }
            data0[i++] = SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]));
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), MODE0f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR0f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR_MASK0f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), VRF_ID_0f);
            data0[i++] = SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]));
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), MODE1f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR1f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), IP_ADDR_MASK1f);
            data0[i++] = soc_mem_field32_get(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), VRF_ID_1f);


            data1[j++] = SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, read);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, MODE0f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, IP_ADDR0f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, IP_ADDR_MASK0f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, VRF_ID_0f);
            data1[j++] = SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, read);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, MODE1f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, IP_ADDR1f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, IP_ADDR_MASK1f);
            data1[j++] = soc_mem_field32_get(u, L3_DEFIPm, read, VRF_ID_1f);
           
            if (sal_memcmp(data0, data1, sizeof(data0))) {
                mismatch = 1;
                LOG_CLI(("Defip mismatch %d \n", index));
                soc_mem_entry_dump(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[index]), BSL_LSS_CLI);
            }            
        }
        if (!mismatch) {
            LOG_CLI(("Defip all match\n"));
        }
    }

    LOG_CLI(("L3 defip table dump\n"));
    sh_process_command(u, "d chg l3_defip");
    sh_process_command(u, "ser log");
    sh_process_command(u, "soc");

    /* Please collect back trace */
    assert(0);
}

static
int _soc_fb_lpm_hash_verify(_soc_fb_lpm_hash_t *hash,
                            uint32 new_index,
                            int valid)
{
    int u = hash->unit;
    int match = 0;
    uint32 index = 0;

    for (index = 0; index < hash->index_count; index++) {
        if (hash->table[index] != FB_LPM_HASH_INDEX_NULL) {
            if (new_index == hash->table[index]) {
                match = 1;
                break;
            }
        }
    }

    for (index = 0; (index < hash->entry_count) && !match; index++) {
        if (hash->link_table[index] != FB_LPM_HASH_INDEX_NULL) {
            if (new_index == hash->link_table[index]) {
                match = 1;
                break;
            }
        }
    }

    if (match == valid) {
        return(SOC_E_NONE);
    }


    LOG_ERROR(BSL_LS_SOC_LPM,
              (BSL_META_U(u,
                        "Verify index %d valid %d. Index %d. table %d. Link_table %d\n"),
                        new_index, valid, index, hash->table[index], hash->link_table[index]));
    LOG_CLI(("Fatal5\n"));

    soc_fb_lpm_debug_collect(u);
    return (SOC_E_INTERNAL);    
}

/* 0 : equal. 1: not equal */
STATIC INLINE int
_soc_fb_lpm_state_cmp(int u, soc_lpm_state_p lpm_state_ptr,
                             soc_lpm_state_p lpm_state_ptr2)
{
    int curr_pfx = MAX_PFX_INDEX(u);
    int result = 0;
    while (curr_pfx > 0) {
        if (sal_memcmp(&(SOC_LPM_STATE(u)[curr_pfx]),
                       &(DBG_SOC_LPM_STATE2(u)[curr_pfx]),
                       sizeof(soc_lpm_state_t))) {
            if (SOC_LPM_STATE_S(lpm_state_ptr2, curr_pfx) == -1 &&
                SOC_LPM_STATE_V(lpm_state_ptr, curr_pfx) == 0) {
                /* good */
            } else {
                LOG_CLI(("PFX0 = %d P = %d N = %d START = %d "
                         "END = %d VENT = %d FENT = %d\n",
                         curr_pfx,
                         SOC_LPM_STATE_P(lpm_state_ptr, curr_pfx),
                         SOC_LPM_STATE_N(lpm_state_ptr, curr_pfx),
                         SOC_LPM_STATE_S(lpm_state_ptr, curr_pfx),
                         SOC_LPM_STATE_E(lpm_state_ptr, curr_pfx),
                         SOC_LPM_STATE_V(lpm_state_ptr, curr_pfx),
                         SOC_LPM_STATE_F(lpm_state_ptr, curr_pfx)));
                LOG_CLI(("PFX2 = %d P = %d N = %d START = %d "
                         "END = %d VENT = %d FENT = %d\n",
                         curr_pfx,
                         SOC_LPM_STATE_P(lpm_state_ptr2, curr_pfx),
                         SOC_LPM_STATE_N(lpm_state_ptr2, curr_pfx),
                         SOC_LPM_STATE_S(lpm_state_ptr2, curr_pfx),
                         SOC_LPM_STATE_E(lpm_state_ptr2, curr_pfx),
                         SOC_LPM_STATE_V(lpm_state_ptr2, curr_pfx),
                         SOC_LPM_STATE_F(lpm_state_ptr2, curr_pfx)));
                
                result = 1;
            }
              
        }
        curr_pfx--;
    }
    return result;
}

#else
#define H_INDEX_MATCH(str, tab_index, match_index)
STATIC int
LOCAL_READ_L3_DEFIPm(int u,int blk, int index, void *data) {
                  return ((SOC_MEM_IS_VALID(u, L3_DEFIPm))?
                  READ_L3_DEFIPm(u, blk, index, data):
                  READ_L3_DEFIP_LEVEL1m(u, blk, index, data));
}
#undef READ_L3_DEFIPm
#define READ_L3_DEFIPm LOCAL_READ_L3_DEFIPm

STATIC int
LOCAL_WRITE_L3_DEFIPm(int u,int blk, int index, void *data) {
                  return ((SOC_MEM_IS_VALID(u, L3_DEFIPm))?
                  WRITE_L3_DEFIPm(u, blk, index, data):
                  WRITE_L3_DEFIP_LEVEL1m(u, blk, index, data));
}
#undef WRITE_L3_DEFIPm
#define WRITE_L3_DEFIPm LOCAL_WRITE_L3_DEFIPm
#endif

#define LPM_NO_MATCH_INDEX FB_LPM_HASH_IPV6_MASK
#define LPM_HASH_INSERT(u, entry_data, tab_index, rvt_index0, rvt_index1)   \
    soc_fb_lpm_hash_insert(u, entry_data, tab_index, LPM_NO_MATCH_INDEX, 0, \
                           rvt_index0, rvt_index1)

#define LPM_HASH_REVERT(u, entry_data, tab_index, rvt_index0, rvt_index1)   \
    soc_fb_lpm_hash_revert(u, entry_data, tab_index, rvt_index0, rvt_index1)

#define LPM_HASH_DELETE(u, key_data, tab_index)         \
    soc_fb_lpm_hash_delete(u, key_data, tab_index)

#define LPM_HASH_LOOKUP(u, key_data, pfx, tab_index)    \
    soc_fb_lpm_hash_lookup(u, key_data, pfx, tab_index)

void soc_fb_lpm_hash_insert(int u, void *entry_data, uint32 tab_index,
                            uint32 old_index, int pfx,
                            uint32 *rvt_index0, uint32 *rvt_index1)
{
    _soc_fb_lpm_hash_entry_t    key_hash;
    int                         is_ipv6;
    soc_mem_t                   mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f,_key_data);
        is_ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        mem = L3_DEFIP_LEVEL1m;
    } else
#endif
    {
        is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);
    }

    if (is_ipv6) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
                SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            } else
#endif
            {
                SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            }
            _soc_fb_lpm_hash_insert(
                        SOC_LPM_STATE_HASH(u),
                        _soc_fb_lpm_hash_compare_key,
                        key_hash,
                        pfx,
                        old_index,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_insert(SOC_LPM_STATE_HASH(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_insert(SOC_LPM_STATE_HASH(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}



void soc_fb_lpm_hash_revert(int u, void *entry_data, uint32 tab_index,
                            uint32 rvt_index0, uint32 rvt_index1)
{
    _soc_fb_lpm_hash_entry_t    key_hash;
    soc_mem_t                   mem = L3_DEFIPm;
    int ipv6 = 0;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f,_key_data);
        ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        mem = L3_DEFIP_LEVEL1m;
    } else
#endif
    {
        ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);
    }

    if (ipv6) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
                SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            } else
#endif
            {
                SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
            }
            _soc_fb_lpm_hash_revert(
                        SOC_LPM_STATE_HASH(u),
                        _soc_fb_lpm_hash_compare_key,
                        key_hash,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_revert(SOC_LPM_STATE_HASH(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            _soc_fb_lpm_hash_revert(SOC_LPM_STATE_HASH(u),
                                    _soc_fb_lpm_hash_compare_key,
                                    key_hash,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}

void soc_fb_lpm_hash_delete(int u, void *key_data, uint32 tab_index)
{
    _soc_fb_lpm_hash_entry_t    key_hash;
    int                         pfx = -1;
    int                         rv;
    int                         is_ipv6;
    uint32                      index;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f,_key_data);
        is_ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);
    }

    if (is_ipv6) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
                SOC_TH3_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
            } else
#endif
            {
                SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
            }
        index = (tab_index << 1) | FB_LPM_HASH_IPV6_MASK;
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
        index = tab_index;
    }

    rv = _soc_fb_lpm_hash_delete(SOC_LPM_STATE_HASH(u),
                                 _soc_fb_lpm_hash_compare_key,
                                 key_hash, pfx, index);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "\ndel  index: H %d error %d\n"), index, rv));
    }
}

int soc_fb_lpm_hash_lookup(int u, void *key_data, int pfx, int *key_index)
{
    _soc_fb_lpm_hash_entry_t    key_hash;
    int                         is_ipv6;
    int                         rv;
    uint32                      index = FB_LPM_HASH_INDEX_NULL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f,_key_data);
        is_ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);
    }

    if (is_ipv6) {
        SOC_FB_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
    }

    rv = _soc_fb_lpm_hash_lookup(SOC_LPM_STATE_HASH(u),
                                 _soc_fb_lpm_hash_compare_key,
                                 key_hash, pfx, &index);
    if (SOC_FAILURE(rv)) {
        *key_index = 0xFFFFFFFF;
        return(rv);
    }

    *key_index = index;

    return(SOC_E_NONE);
}

/* 
 * Function:
 *      _soc_fb_lpm_hash_compute
 * Purpose:
 *      Compute CRC hash for key data.
 * Parameters:
 *      data - Key data
 *      data_nbits - Number of data bits
 * Returns:
 *      Computed 16 bit hash
 */
static
unsigned int _soc_fb_lpm_hash_compute(uint8 *data, int data_nbits)
{
    return (_shr_crc32b(0, data, data_nbits));
}

/* 
 * Function:
 *      _soc_fb_lpm_hash_create
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
int _soc_fb_lpm_hash_create(int unit,
                            int entry_count,
                            int index_count,
                            _soc_fb_lpm_hash_t **fb_lpm_hash_ptr)
{
    _soc_fb_lpm_hash_t  *hash;
    int                 index;

    if (index_count > entry_count) {
        return SOC_E_MEMORY;
    }

    hash = sal_alloc(sizeof (_soc_fb_lpm_hash_t), "lpm_hash");
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

#ifdef FB_LPM_DEBUG
    hash->check_table = sal_alloc(
                            hash->entry_count * sizeof(*(hash->check_table)),
                            "link_table");
    if (hash->check_table == NULL) {
        sal_free(hash->link_table);
        sal_free(hash->table);
        sal_free(hash);
        return SOC_E_MEMORY;
    }
    for(index = 0; index < hash->entry_count; index++) {
        hash->check_table[index] = 0;
    }
#endif

    /*
     * Set the entries in the hash table to FB_LPM_HASH_INDEX_NULL
     * Link the entries beyond hash->index_max for handling collisions
     */
    for(index = 0; index < hash->index_count; index++) {
        hash->table[index] = FB_LPM_HASH_INDEX_NULL;
    }
    for(index = 0; index < hash->entry_count; index++) {
        hash->link_table[index] = FB_LPM_HASH_INDEX_NULL;
    }
    *fb_lpm_hash_ptr = hash;
    return SOC_E_NONE;
}

/* 
 * Function:
 *      _soc_fb_lpm_hash_destroy
 * Purpose:
 *      Destroy the hash table
 * Parameters:
 *      fb_lpm_hash - Pointer (handle) to Hash Table
 * Returns:
 *      SOC_E_NONE       Success
 */
static
int _soc_fb_lpm_hash_destroy(_soc_fb_lpm_hash_t *fb_lpm_hash)
{
    if (fb_lpm_hash != NULL) {
        sal_free(fb_lpm_hash->table);
        sal_free(fb_lpm_hash->link_table);
#ifdef FB_LPM_DEBUG
        sal_free(fb_lpm_hash->check_table);
#endif
        sal_free(fb_lpm_hash);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_fb_lpm_hash_lookup
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
int _soc_fb_lpm_hash_lookup(_soc_fb_lpm_hash_t          *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t    entry,
                            int                         pfx,
                            uint32                      *key_index)
{
    int u = hash->unit, v, i = 0;

    uint16 hash_val;
    uint32 index;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
                                        (32 * 6)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("lhash", entry[0], hash_val);
    H_INDEX_MATCH("lkup ", entry[0], index);
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        uint32  e[SOC_MAX_MEM_FIELD_WORDS];
        _soc_fb_lpm_hash_entry_t  r_entry;
        int     rindex;
        int rv = SOC_E_NONE;

        rindex = (index & fb_lpm_hash_index_mask[u]) >> 1;
        /*
         * Check prefix length and skip index if not valid for given length
        if ((SOC_LPM_STATE_START(u, pfx) <= rindex) &&
            (SOC_LPM_STATE_END(u, pfx) >= rindex)) {
         */
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, rindex, e);
        SOC_IF_ERROR_RETURN(rv);
        SOC_FB_LPM_HASH_ENTRY_GET(u, e, index, r_entry, &v);
        if (v == 0) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                "Empty entry[%d] in link_table. hash_val %d ridx %d\n"),
                                index, hash_val, rindex));
        }
        if ((*key_cmp_fn)(entry, r_entry) == 0) {
            *key_index = (index & fb_lpm_hash_index_mask[u]) >>
                            ((index & FB_LPM_HASH_IPV6_MASK) ? 1 : 0);
            H_INDEX_MATCH("found", entry[0], index);
            return(SOC_E_NONE);
        }
        /*
        }
        */
        index = hash->link_table[index & fb_lpm_hash_index_mask[u]];
        H_INDEX_MATCH("lkup1", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    H_INDEX_MATCH("not_found", entry[0], index);
    return(SOC_E_NOT_FOUND);
}

/*
 * Function:
 *      _soc_fb_lpm_hash_insert
 * Purpose:
 *      Insert/Update a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to lookup
 *      pfx     - Prefix length for lookup acceleration.
 *      old_index - Index where the key was moved from.
 *                  FB_LPM_HASH_INDEX_NULL if new entry.
 *      new_index - Index where the key was moved to.
 * Returns:
 *      SOC_E_NONE      Key found
 */
/*
 *      Should be caled before updating the LPM table so that the
 *      data in the hash table is consistent with the LPM table
 */
static
int _soc_fb_lpm_hash_insert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            int    pfx,
                            uint32 old_index,
                            uint32 new_index,
                            uint32 *rvt_index)
{
    int u = hash->unit, v, i = 0;
    uint32 hash_val;
    uint32 index;
    uint32 prev_index;
    int rv = SOC_E_NONE;

    if (rvt_index) {
        *rvt_index = FB_LPM_HASH_INDEX_NULL;
    }
    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
                                        (32 * 6)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("ihash", entry[0], hash_val);
    H_INDEX_MATCH("ins  ", entry[0], new_index);
    H_INDEX_MATCH("ins1 ", index, new_index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    if (old_index != FB_LPM_HASH_INDEX_NULL) {
        while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
            uint32  e[SOC_MAX_MEM_FIELD_WORDS];
            _soc_fb_lpm_hash_entry_t  r_entry;
            int     rindex;
            
            rindex = (index & fb_lpm_hash_index_mask[u]) >> 1;

            /*
             * Check prefix length and skip index if not valid for given length
            if ((SOC_LPM_STATE_START(u, pfx) <= rindex) &&
                (SOC_LPM_STATE_END(u, pfx) >= rindex)) {
             */
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, rindex, e);
            SOC_IF_ERROR_RETURN(rv);
            SOC_FB_LPM_HASH_ENTRY_GET(u, e, index, r_entry, &v);
            if (v == 0) {
                if ((index & FB_LPM_HASH_IPV6_MASK) ||
                    !((new_index & 1) && (new_index == (index + 1)))) {
                    LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                    "Empty entry[%d] in link_table. hash_val %d ridx %d nidx %d\n"),
                                    index, hash_val, rindex, new_index));
                }
            }
            if (v && ((*key_cmp_fn)(entry, r_entry) == 0)) {
                /* assert(old_index == index);*/
                if (new_index != index) {
                    H_INDEX_MATCH("imove", prev_index, new_index);
                    if (prev_index == FB_LPM_HASH_INDEX_NULL) {
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
            index = hash->link_table[index & fb_lpm_hash_index_mask[u]];
            H_INDEX_MATCH("ins2 ", index, new_index);
        }
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
        return SOC_E_INTERNAL;
    }
    INDEX_ADD(hash, hash_val, new_index);  /* new entry */
    return(SOC_E_NONE);
}

/*
 * Function:
 *      _soc_fb_lpm_hash_revert
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
int _soc_fb_lpm_hash_revert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            uint32 new_index,
                            uint32 rvt_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint32 index;
    uint32 prev_index;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
                                        (32 * 6)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("rhash", entry[0], hash_val);
    H_INDEX_MATCH("rvt  ", entry[0], index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (new_index == index) {
            H_INDEX_MATCH("rfoun", entry[0], index);
            if (rvt_index == FB_LPM_HASH_INDEX_NULL) {
                if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                    INDEX_DELETE(hash, hash_val, new_index);
                } else {
                    INDEX_DELETE_LINK(hash, prev_index, new_index);
                }
            } else {
                if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                    INDEX_UPDATE(hash, hash_val, new_index, rvt_index);
                } else {
                    INDEX_UPDATE_LINK(hash, prev_index, new_index, rvt_index);
                }
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & fb_lpm_hash_index_mask[u]];
        H_INDEX_MATCH("rvt1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}

/*
 * Function:
 *      _soc_fb_lpm_hash_delete
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
int _soc_fb_lpm_hash_delete(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm_hash_entry_t entry,
                            int    pfx,
                            uint32 delete_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint32 index;
    uint32 prev_index;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
                                        (32 * 6)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("dhash", entry[0], hash_val);
    H_INDEX_MATCH("del  ", entry[0], index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (delete_index == index) {
            H_INDEX_MATCH("dfoun", entry[0], index);
            if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                INDEX_DELETE(hash, hash_val, delete_index);
            } else {
                INDEX_DELETE_LINK(hash, prev_index, delete_index);
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & fb_lpm_hash_index_mask[u]];
        H_INDEX_MATCH("del1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
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
#endif /* FB_LPM_HASH_SUPPORT */

static
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
soc_fb_lpm_ip4entry0_to_0(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(u, soc_feature_flex_flow)) {
        uint32      key_type;

        if (soc_mem_field_valid(u, L3_DEFIPm, KEY_TYPE0f)) {
            key_type = soc_mem_field32_get(u, L3_DEFIPm, src, KEY_TYPE0f);
        } else {
            key_type = 0;
        }
   
        if (BCMI_LPM_FLEX_VIEW(key_type)) {
            return soc_td3_lpm_flex_ip4entry0_to_0(u, src, dst, key_type, copy_hit);
        }
    }
#endif


    ipv4a = SOC_MEM_OPT_F32_VALID0_GET(u, mem, src);
    SOC_MEM_OPT_F32_VALID0_SET(u, mem, dst, ipv4a);

    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_HIT0_GET(u, mem, src);
        SOC_MEM_OPT_F32_HIT0_SET(u, mem, dst, ipv4a);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32      val[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, KEY0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, KEY0f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, MASK0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, MASK0f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ASSOC_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ASSOC_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, FIXED_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, FIXED_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ALPM1_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ALPM1_DATA0f, val);
        return(SOC_E_NONE);
    }
#endif
    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode0_field);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode0_field, ipv4a);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, defip_mode_mask0_field)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode_mask0_field);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode_mask0_field, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE0f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

    if (soc_mem_field_valid(u, L3_DEFIPm, DATA_TYPE0f)) {
        ipv4a = soc_mem_field32_get(u, L3_DEFIPm, src, DATA_TYPE0f);
        soc_mem_field32_set(u, L3_DEFIPm, dst, DATA_TYPE0f, ipv4a);
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(u, soc_feature_ipmc_defip)) {
        if (soc_mem_field_valid(u, L3_DEFIPm, MULTICAST_ROUTE0f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE0f, ipv4a);
        }

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF0f, ipv4a);

        if (soc_mem_field_valid(u, L3_DEFIPm, L3MC_INDEX0f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX0f, ipv4a);
        }
    }
#endif
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
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_generic_dest)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DESTINATION0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DESTINATION0f, ipv4a);

        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX0f, ipv4a);
        }
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE0f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,DEFAULTROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULTROUTE0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ENTRY_TYPE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE_MASK0f, ipv4a);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, KEY_TYPE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, KEY_TYPE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, KEY_TYPE0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, KEY_TYPE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, KEY_TYPE_MASK0f, ipv4a);
    }
#endif

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, D_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD0f, ipv4a);
    }
#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID0f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(u, soc_feature_advanced_flex_counter)) {
        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_OFFSET_MODE0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE0f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_POOL_NUMBER0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER0f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_BASE_COUNTER_IDX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX0f,
                                ipv4a);
        }
    }
#endif

    return(SOC_E_NONE);
}

int
soc_fb_lpm_ip4entry1_to_1(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    ipv4a = SOC_MEM_OPT_F32_VALID1_GET(u, mem, src);
    SOC_MEM_OPT_F32_VALID1_SET(u, mem, dst, ipv4a);

    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_HIT1_GET(u, mem, src);
        SOC_MEM_OPT_F32_HIT1_SET(u, mem, dst, ipv4a);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 val[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, KEY1f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, KEY1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, MASK1f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, MASK1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ASSOC_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ASSOC_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, FIXED_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, FIXED_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ALPM1_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ALPM1_DATA1f, val);
        return(SOC_E_NONE);
    }
#endif
    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode1_field);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode1_field, ipv4a);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, defip_mode_mask1_field)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode_mask1_field);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode_mask1_field, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE1f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

    if (soc_mem_field_valid(u, L3_DEFIPm, DATA_TYPE1f)) {
        ipv4a = soc_mem_field32_get(u, L3_DEFIPm, src, DATA_TYPE1f);
        soc_mem_field32_set(u, L3_DEFIPm, dst, DATA_TYPE1f, ipv4a);
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(u, soc_feature_ipmc_defip)) {
        if (soc_mem_field_valid(u, L3_DEFIPm, MULTICAST_ROUTE1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE1f, ipv4a);
        }

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF1f, ipv4a);

        if (soc_mem_field_valid(u, L3_DEFIPm, L3MC_INDEX1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX1f, ipv4a);
        }
    }
#endif

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
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_generic_dest)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DESTINATION1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DESTINATION1f, ipv4a);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX1f, ipv4a);
        }
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,DEFAULTROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULTROUTE1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ENTRY_TYPE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, D_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD1f, ipv4a);
    }
#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID1f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(u, soc_feature_advanced_flex_counter)) {
        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, FLEX_CTR_OFFSET_MODE1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_OFFSET_MODE1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE1f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_POOL_NUMBER1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER1f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_BASE_COUNTER_IDX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX1f,
                                ipv4a);
        }
    }
#endif

    return(SOC_E_NONE);
}

/* src and dst can be same */
int
soc_fb_lpm_ip4entry0_to_1(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(u, soc_feature_flex_flow)) {
        uint32      key_type;

        if (soc_mem_field_valid(u, mem, KEY_TYPE0f)) {
            key_type = soc_mem_field32_get(u, L3_DEFIPm, src, KEY_TYPE0f);
        } else {
            key_type = 0;
        }
    
        if (BCMI_LPM_FLEX_VIEW(key_type)) {

            return soc_td3_lpm_flex_ip4entry0_to_1(u, src, dst, key_type, copy_hit);
        }
    }
#endif

    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_HIT0_GET(u, mem, src);
        SOC_MEM_OPT_F32_HIT1_SET(u, mem, dst, ipv4a);
    }
    ipv4a = SOC_MEM_OPT_F32_VALID0_GET(u, mem, src);
    SOC_MEM_OPT_F32_VALID1_SET(u, mem, dst, ipv4a);


#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 val[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, KEY0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, KEY1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, MASK0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, MASK1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ASSOC_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ASSOC_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, FIXED_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, FIXED_DATA1f, val);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ALPM1_DATA0f, val);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ALPM1_DATA1f, val);
        return(SOC_E_NONE);
    }
#endif

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode0_field);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode1_field, ipv4a);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, defip_mode_mask0_field)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode_mask0_field);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode_mask1_field, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE1f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

    if (soc_mem_field_valid(u, L3_DEFIPm, DATA_TYPE0f)) {
        ipv4a = soc_mem_field32_get(u, L3_DEFIPm, src, DATA_TYPE0f);
        soc_mem_field32_set(u, L3_DEFIPm, dst, DATA_TYPE1f, ipv4a);
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(u, soc_feature_ipmc_defip)) {
        if (soc_mem_field_valid(u, L3_DEFIPm, MULTICAST_ROUTE1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE1f, ipv4a);
        }

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID1f, ipv4a);

        if (soc_mem_field_valid(u, L3_DEFIPm, L3MC_INDEX1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX1f, ipv4a);
        }

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f, ipv4a);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */    

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT1f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR1f, ipv4a);
    } else {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_generic_dest)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DESTINATION0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DESTINATION1f, ipv4a);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX1f, ipv4a);
        }
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI1f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE0f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE1f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE0f) &&
        SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,DEFAULTROUTE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULTROUTE1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ENTRY_TYPE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE_MASK1f, ipv4a);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, KEY_TYPE0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, KEY_TYPE0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, KEY_TYPE1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, KEY_TYPE_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, KEY_TYPE_MASK1f, ipv4a);
    }
#endif

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, D_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID1f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID_MASK0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD1f, ipv4a);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID0f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID1f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(u, soc_feature_advanced_flex_counter)) {
        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_OFFSET_MODE0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE1f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_POOL_NUMBER0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER1f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_BASE_COUNTER_IDX0f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX1f,
                                ipv4a);
        }
    }
#endif

    return(SOC_E_NONE);
}

/* src and dst can be same */
int
soc_fb_lpm_ip4entry1_to_0(int u, void *src, void *dst, int copy_hit)
{
    uint32      ipv4a;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    ipv4a = SOC_MEM_OPT_F32_VALID1_GET(u, mem, src);
    SOC_MEM_OPT_F32_VALID0_SET(u, mem, dst, ipv4a);

    if (copy_hit) {
        ipv4a = SOC_MEM_OPT_F32_HIT1_GET(u, mem, src);
        SOC_MEM_OPT_F32_HIT0_SET(u, mem, dst, ipv4a);
    }
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, KEY1f, _key_data);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, KEY0f, _key_data);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, MASK1f, _key_data);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, MASK0f, _key_data);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ASSOC_DATA1f, _key_data);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ASSOC_DATA0f, _key_data);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, FIXED_DATA1f, _key_data);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, FIXED_DATA0f, _key_data);
        SOC_TH3_MEM_OPT_F64_GET(u, mem, src, ALPM1_DATA1f, _key_data);
        SOC_TH3_MEM_OPT_F64_SET(u, mem, dst, ALPM1_DATA0f, _key_data);
        return(SOC_E_NONE);
    }
#endif


    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode1_field);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode0_field, ipv4a);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, defip_mode_mask0_field)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, defip_mode_mask1_field);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, defip_mode_mask0_field, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, GLOBAL_ROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, GLOBAL_ROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, GLOBAL_ROUTE0f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

    if (soc_mem_field_valid(u, L3_DEFIPm, DATA_TYPE0f)) {
        ipv4a = soc_mem_field32_get(u, L3_DEFIPm, src, DATA_TYPE1f);
        soc_mem_field32_set(u, L3_DEFIPm, dst, DATA_TYPE0f, ipv4a);
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(u, soc_feature_ipmc_defip)) {
        if (soc_mem_field_valid(u, L3_DEFIPm, MULTICAST_ROUTE1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, MULTICAST_ROUTE1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, MULTICAST_ROUTE0f, ipv4a);
        }

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPA_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPA_ID0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, EXPECTED_L3_IIF1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, EXPECTED_L3_IIF0f, ipv4a);

        if (soc_mem_field_valid(u, L3_DEFIPm, L3MC_INDEX1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, L3MC_INDEX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, L3MC_INDEX0f, ipv4a);
        }
    }
#endif

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, IP_ADDR_MASK1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, IP_ADDR_MASK0f, ipv4a);

	if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP0f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP0f, ipv4a);
	}

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ECMP_COUNT1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_COUNT1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_COUNT0f, ipv4a);

        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ECMP_PTR1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ECMP_PTR0f, ipv4a);
    } else {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_generic_dest)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DESTINATION1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DESTINATION0f, ipv4a);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, NEXT_HOP_INDEX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, NEXT_HOP_INDEX0f, ipv4a);
        }
    }

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, PRI1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, PRI0f, ipv4a);

    ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, RPE1f);
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, RPE0f, ipv4a);

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE0f) &&
        SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DEFAULTROUTE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,DEFAULTROUTE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DEFAULTROUTE0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_ID_1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, VRF_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, VRF_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, ENTRY_TYPE1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, ENTRY_TYPE_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, ENTRY_TYPE_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, D_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID0f, ipv4a);
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, D_ID_MASK1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, D_ID_MASK0f, ipv4a);
    }

    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, DST_DISCARD1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, DST_DISCARD0f, ipv4a);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, CLASS_ID1f)) {
        ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src, CLASS_ID1f);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, CLASS_ID0f, ipv4a);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(u, soc_feature_advanced_flex_counter)) {
        if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, FLEX_CTR_OFFSET_MODE1f)) {
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_OFFSET_MODE1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_OFFSET_MODE0f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_POOL_NUMBER1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_POOL_NUMBER0f,
                                ipv4a);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, src,
                                        FLEX_CTR_BASE_COUNTER_IDX1f);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, dst, FLEX_CTR_BASE_COUNTER_IDX0f,
                                ipv4a);
        }
    }
#endif

    return(SOC_E_NONE);
}


STATIC INLINE void
_soc_fb_lpm_state_dump(int u, soc_lpm_state_p lpm_state_ptr)
{
    int curr_pfx = MAX_PFX_INDEX(u);
    if (lpm_state_ptr == NULL) {
        lpm_state_ptr = SOC_LPM_STATE(u);
    }
    while (lpm_state_ptr && curr_pfx > 0) {
        LOG_CLI(("PFX = %d P = %d N = %d START = %d "
                 "END = %d VENT = %d FENT = %d\n",
                 curr_pfx,
                 SOC_LPM_STATE_P(lpm_state_ptr, curr_pfx),
                 SOC_LPM_STATE_N(lpm_state_ptr, curr_pfx),
                 SOC_LPM_STATE_S(lpm_state_ptr, curr_pfx),
                 SOC_LPM_STATE_E(lpm_state_ptr, curr_pfx),
                 SOC_LPM_STATE_V(lpm_state_ptr, curr_pfx),
                 SOC_LPM_STATE_F(lpm_state_ptr, curr_pfx)));

        curr_pfx = SOC_LPM_STATE_N(lpm_state_ptr, curr_pfx);
    }
}

void
soc_fb_lpm_state_dump(int u)
{

    if (!bsl_check(bslLayerSoc, bslSourceLpm, bslSeverityVerbose, u)) {
        return;
    }
    _soc_fb_lpm_state_dump(u, 0);
}

/*
 *      Gets entry index offset in second half of the tcam if URPF check is ON. 
 */
int soc_lpm_fb_urpf_entry_replicate_index_offset(int unit, int *offset)
{
    int src_tcam_offset;  /* Defip memory size/2 urpf source lookup offset */

    if(!SOC_URPF_STATUS_GET(unit)) {
        return (SOC_E_NONE);
    }

    *offset = 0;

    if (soc_feature(unit, soc_feature_l3_defip_hole)) {
        src_tcam_offset = (soc_mem_index_count(unit, L3_DEFIPm) >> 1);
    } else if (SOC_IS_APOLLO(unit)) {
        src_tcam_offset = (soc_mem_index_count(unit, L3_DEFIPm) >> 1) + 0x0400;
    } else {
        src_tcam_offset = (soc_mem_index_count(unit, L3_DEFIPm) >> 1);
    }

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 0) == 1) {
            /* When tcams are paired ipv4 lpm and ipv6 64b lpm
             * starts at index of tcam 4. We have to add size of the
             * tcam used for DIP lookup to get the urpf index */
            src_tcam_offset = kt_lpm_ipv6_info[unit]->ipv6_64b.depth;
        }
    }
#endif

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using only 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (For ex: Katana2).
     */
    if (!soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        *offset += src_tcam_offset;
    }

    return SOC_E_NONE;
}

/*
 *      Replicate entry to the second half of the tcam if URPF check is ON. 
 */
static 
int _lpm_fb_urpf_entry_replicate(int u, int index, int temp_index, uint32 *e)
{
    int src_tcam_offset;  /* Defip memory size/2 urpf source lookup offset */
    int ipv6;             /* IPv6 entry.                                   */
    uint32 mask0;         /* Mask 0 field value.                           */
    uint32 mask1;         /* Mask 1 field value.                           */
    int def_gw_flag;      /* Entry is default gateway.                     */ 
    int rv;               /* Return type.                                  */
    uint32 lpm_entry[SOC_MAX_MEM_WORDS]; /* Temporary lpm entry. */
    soc_mem_t mem = L3_DEFIPm;
#if defined(BCM_TRIDENT3_SUPPORT)
    int data_type0 = 0, data_type1 = 0;
#endif

    if(!SOC_URPF_STATUS_GET(u)) {
        return (SOC_E_NONE);
    }

    if (soc_feature(u, soc_feature_l3_defip_hole)) {
         src_tcam_offset = (soc_mem_index_count(u, mem) >> 1);
    } else if (SOC_IS_APOLLO(u)) {
         src_tcam_offset = (soc_mem_index_count(u, mem) >> 1) + 0x0400;
    } 
    else {
         src_tcam_offset = (soc_mem_index_count(u, mem) >> 1);
    }

    ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, defip_mode0_field);

    #if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(u)) {
        if (soc_property_get(u, spn_IPV6_LPM_128B_ENABLE, 0) == 1) {
            /* When tcams are paired ipv4 lpm and ipv6 64b lpm
             * starts at index of tcam 4. We have to add size of the
             * tcam used for DIP lookup to get the urpf index */
            src_tcam_offset = kt_lpm_ipv6_info[u]->ipv6_64b.depth;
        }
    }
    #endif

    /* Reset destination discard bit. */
    if (SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, DST_DISCARD0f)) {
        /* For KT2, both dip and sip lookup is done using only 1 l3_defip entry.
         * l3_defip table is not divided into 2 to support urpf. So, dest
         * discard bits should not be reset in KT2. For other devices, if urpf
         * is enabled, destination discard bits has to be reset since only sip
         * lookup happens in the second half of L3_DEFIPm.
         */
        if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, 0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD1f, 0);
        }
    }

    /* Set/Reset default gateway flag based on ip mask value. */
    mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK0f);
    mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK1f);


    if (!ipv6) {
        if (SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, e)) {
            /* 
             * For KT2, L3_DEFIP table is not divided in 2 parts to 
             * support URPF unlike previous devices
             * RPExf and DEFAULTROUTExf are not overlaid in KT2 
             * like other devices.
             */
            if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, (!mask0) ? 1 : 0);
            }
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e,
                DEFAULTROUTE0f, (!mask0) ? 1 : 0);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(u, L3_DEFIPm, DATA_TYPE0f)) {
                data_type0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, DATA_TYPE0f);
                /* SIP lookup for flex entries use a different data_type
                 * as defined in CANCUN.Setting this only for fixed flows
                 * where data_type is within the regsfile defined values */
                if (data_type0 <= 3) {
                    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DATA_TYPE0f, 1);
                }
            }
#endif
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, e)) {
            /* 
             * For KT2, L3_DEFIP table is not divided in 2 parts to 
             * support URPF unlike previous devices
             * RPExf and DEFAULTROUTExf are not overlaid in KT2 
             * like other devices.
             */
            if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, (!mask1) ? 1 : 0);
            }
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e,
                DEFAULTROUTE1f, (!mask1) ? 1 : 0);

#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(u, L3_DEFIPm, DATA_TYPE1f)) {
                data_type1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, DATA_TYPE1f);
                /* SIP lookup for flex entries use a different data_type
                 * as defined in CANCUN.Setting this only for fixed flows
                 * where data_type is within the regsfile defined values */
                if (data_type1 <= 3) {
                    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DATA_TYPE1f, 1);
                }
            }
#endif
        }
    } else {
        def_gw_flag = ((!mask0) &&  (!mask1)) ? 1 : 0;

        /* 
         * For KT2, L3_DEFIP table is not divided in 2 parts to 
         * support URPF unlike previous devices
         * RPExf and DEFAULTROUTExf are not overlaid in KT2 
         * like other devices.
         */
        if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, def_gw_flag);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE1f, def_gw_flag);
        }
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DEFAULTROUTE0f, def_gw_flag);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DEFAULTROUTE1f, def_gw_flag);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(u, L3_DEFIPm, DATA_TYPE0f)) {
            data_type0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, DATA_TYPE0f);
            /* SIP lookup for flex entries use a different data_type
             * as defined in CANCUN.Setting this only for fixed flows
             * where data_type is within the regsfile defined values */
            if (data_type0 <= 3) {
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DATA_TYPE0f, 1);
                SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DATA_TYPE1f, 1);
            }
        }
#endif
    }

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using only 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (For ex: Katana2).
     */
    if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        index += src_tcam_offset;
        if (temp_index != -1) {
           temp_index += src_tcam_offset;
        }
    }

    /*
     * In order to avoid atomicity issue, write the tcam entry to temp_index
     * before writing into the actual index.
     * "temp_index" is VALID only if 'lpm_atomic_write' feature is enabled.
     */  
    if (temp_index != -1) {
        sal_memset(&lpm_entry, 0x0, sizeof(lpm_entry));
        rv =  READ_L3_DEFIPm(u, MEM_BLOCK_ANY, index, &lpm_entry);
        SOC_IF_ERROR_RETURN(rv);
        rv =  WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_index, &lpm_entry);
        SOC_IF_ERROR_RETURN(rv);
    }
    if (SOC_IS_TRIDENT3(u)) {
        soc_mem_field32_set(u, L3_DEFIPm, e, FLEX_CTR_BASE_COUNTER_IDX0f, 0);
        soc_mem_field32_set(u, L3_DEFIPm, e, FLEX_CTR_BASE_COUNTER_IDX1f, 0);
    }

    rv =  WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, e);
    SOC_IF_ERROR_RETURN(rv);

    /*
     * Clear temp_index
     */
    if (temp_index != -1) {
       sal_memset(&lpm_entry, 0x0, sizeof(lpm_entry));
       rv =  WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_index, &lpm_entry);
    } 

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
static
int _lpm_fb_entry_shift(int u, int from_ent, int to_ent)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
#ifdef FB_LPM_TABLE_CACHED
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                       MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */

    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e);
    SOC_IF_ERROR_RETURN(rv);
    LPM_HASH_INSERT(u, e, to_ent, &rvt_index0, &rvt_index1);
    rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e);
    if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (rv >= 0)) {
        rv = _lpm_fb_urpf_entry_replicate(u, to_ent, -1, e);
    }
    if (rv < 0) {
        LPM_HASH_REVERT(u, e, to_ent, rvt_index0, rvt_index1);
    }
    LPM_AVL_INSERT(u, e, to_ent);
    return (rv);
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
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    soc_mem_t mem = L3_DEFIPm;
    int rv = SOC_E_NONE;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    to_ent = SOC_LPM_STATE_END(u, pfx) + 1;

    /* Don't move empty prefix . */ 
    if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
        SOC_LPM_STATE_START(u, pfx) = to_ent + 1;
        SOC_LPM_STATE_END(u, pfx) = to_ent;
        return (SOC_E_NONE);
    }

    ipv6 = SOC_LPM_PFX_IS_V6_64(u, pfx);

    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity) && !ipv6) {
        from_ent = SOC_LPM_STATE_END(u, pfx);
#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e);
        SOC_IF_ERROR_RETURN(rv);
        v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
        v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            LPM_HASH_INSERT(u, e, to_ent, &rvt_index0, &rvt_index1);
            rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e);
            if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (rv >= 0)) {
                rv = _lpm_fb_urpf_entry_replicate(u, to_ent, -1, e);
            }
            if (rv < 0) {
                LPM_HASH_REVERT(u, e, to_ent, rvt_index0, rvt_index1);
                return rv;
            }
            LPM_AVL_INSERT(u, e, to_ent);
            to_ent--;
        }
    }

    from_ent = SOC_LPM_STATE_START(u, pfx);
    if (from_ent != to_ent) {
        SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, from_ent, to_ent));
    } 
    SOC_LPM_STATE_START(u, pfx) += 1;
    SOC_LPM_STATE_END(u, pfx) += 1;
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
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    to_ent = SOC_LPM_STATE_START(u, pfx) - 1;

    /* Don't move empty prefix . */ 
    if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
        SOC_LPM_STATE_START(u, pfx) = to_ent;
        SOC_LPM_STATE_END(u, pfx) = to_ent - 1;
        return (SOC_E_NONE);
    }

    ipv6 = SOC_LPM_PFX_IS_V6_64(u, pfx);

    if ((!ipv6) && (SOC_LPM_STATE_END(u, pfx) != SOC_LPM_STATE_START(u, pfx))) {

        from_ent = SOC_LPM_STATE_END(u, pfx);

#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e);
        SOC_IF_ERROR_RETURN(rv);
        v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
        v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

        if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity) && 
            ((v0 == 0) || (v1 == 0))) {
            /* Last entry is half full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            prev_ent = from_ent - 1;
            SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, prev_ent, to_ent));

            LPM_HASH_INSERT(u, e, prev_ent, &rvt_index0, &rvt_index1);
            rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, prev_ent , e);
            if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (rv >= 0)) {
                rv = _lpm_fb_urpf_entry_replicate(u, prev_ent, -1, e);
            }
            if (rv < 0) {
                LPM_HASH_REVERT(u, e, prev_ent, rvt_index0, rvt_index1);
                return rv;
            }
            LPM_AVL_INSERT(u, e, prev_ent);
        } else {
            /* Last entry is full -> just shift it to start - 1  position. */
            LPM_HASH_INSERT(u, e, to_ent, &rvt_index0, &rvt_index1);
            rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e);
            if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (rv >= 0)) {
                rv = _lpm_fb_urpf_entry_replicate(u, to_ent, -1, e);
            }
            if (rv < 0) {
                LPM_HASH_REVERT(u, e, to_ent, rvt_index0, rvt_index1);
                return rv;
            }
            LPM_AVL_INSERT(u, e, to_ent);
        }

    } else  {

        from_ent = SOC_LPM_STATE_END(u, pfx);
        SOC_IF_ERROR_RETURN(_lpm_fb_entry_shift(u, from_ent, to_ent));

    }

    SOC_LPM_STATE_START(u, pfx) -= 1;
    SOC_LPM_STATE_END(u, pfx) -= 1;

    return (SOC_E_NONE);
}

/*
 * @name  - _lpm_entry_in_paired_tcam
 * @param - unit - unit number
 * @param - index - logical index into L3_DEFIP
 *
 * @purpose - Check if the physical index corresponding to the logical index
 *            is in paired tcam 
 * @returns - TRUE/FALSE
 */
STATIC int
_lpm_entry_in_paired_tcam(int unit, int index) 
{
    int num_tcams = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int new_index;
    int num_ipv6_128b_entries = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);
    soc_mem_t mem = L3_DEFIPm;

#ifdef BCM_UTT_SUPPORT
    if (soc_feature(unit, soc_feature_utt)) {
        return soc_utt_lpm_entry_in_paired_tcam(unit, index);
    }
#endif

    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    if (index >= soc_mem_index_count(unit, mem)) {
        return FALSE; 
    }

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using only 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (For ex: Katana2).
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        new_index = soc_l3_defip_urpf_index_map(unit, 0, index); 
        num_ipv6_128b_entries /= 2;
    } else {
        new_index = soc_l3_defip_index_map(unit, 0, index); 
    } 

    num_tcams = (num_ipv6_128b_entries /  tcam_size) +
                ((num_ipv6_128b_entries % tcam_size) ? 1 : 0);

    if (new_index < num_tcams * tcam_size * 2) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *      _lpm_free_slot_remove_empty_pfx
 * Purpose:
 *      Remove freed prefix from the prefix state link
 * Parameters:
 *      u - Device unit
 *      pfx - Translated prefix (Global prefix value)
 * Returns:
 *      None
 */  
STATIC void 
_lpm_free_slot_remove_empty_pfx(int u, int pfx)
{
    int prev_pfx;
    int next_pfx;

    if (pfx == MAX_PFX_INDEX(u)) {
        return ;
    }
    
    if ((SOC_LPM_STATE_VENT(u, pfx) == 0) &&
        (SOC_LPM_STATE_FENT(u, pfx) == 0)) {
        /* remove from the list */
        prev_pfx = SOC_LPM_STATE_PREV(u, pfx); /* Always present */
        next_pfx = SOC_LPM_STATE_NEXT(u, pfx);
        SOC_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
        if (next_pfx != -1) {
            SOC_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
        }
        SOC_LPM_STATE_NEXT(u, pfx) = -1;
        SOC_LPM_STATE_PREV(u, pfx) = -1;
        SOC_LPM_STATE_START(u, pfx) = -1;
        SOC_LPM_STATE_END(u, pfx) = -1;
    }

    return;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
STATIC
int _lpm_free_slot_create(int u, int pfx, int ipv6, void *e, int *free_slot)
{
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         curr_pfx;
    int         from_ent;
    uint32      v0, v1;
    int         rv;
    int         v6_index_in_paired_tcam = 0;
    int         start_index = 0;
    int         to_ent;
    int         unpaired_start = -1;
    int         occupied_index = -1;
    uint32      entry[SOC_MAX_MEM_FIELD_WORDS];
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int num_ipv6_128b_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    if ((SOC_LPM_STATE_VENT(u, pfx) == 0) && 
        (SOC_LPM_STATE_FENT(u, pfx) == 0)) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length MAX_PFX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = MAX_PFX_INDEX(u);
        while (SOC_LPM_STATE_NEXT(u, curr_pfx) > pfx) {
            curr_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
        if (next_pfx != -1) {
            SOC_LPM_STATE_PREV(u, next_pfx) = pfx;
        }
        SOC_LPM_STATE_NEXT(u, pfx) = SOC_LPM_STATE_NEXT(u, curr_pfx);
        SOC_LPM_STATE_PREV(u, pfx) = curr_pfx;
        SOC_LPM_STATE_NEXT(u, curr_pfx) = pfx;

        SOC_LPM_STATE_START(u, pfx) =  SOC_LPM_STATE_END(u, curr_pfx) +
                                   (SOC_LPM_STATE_FENT(u, curr_pfx) / 2) + 1;
        SOC_LPM_STATE_END(u, pfx) = SOC_LPM_STATE_START(u, pfx) - 1;
        SOC_LPM_STATE_VENT(u, pfx) = 0;

        if (soc_feature(u, soc_feature_l3_shared_defip_table) ||
            (soc_feature(u, soc_feature_l3_kt2_shared_defip_table))) {
            /* if v6, dont allow the entry to be added in paired tcam */
            start_index = SOC_LPM_STATE_START(u, pfx);
            if ((ipv6 != 0) && _lpm_entry_in_paired_tcam(u, start_index)) {
                /* entry location is paired tcam. 
                 * so find a new slot for this prefix. 
                 */
                v6_index_in_paired_tcam = 1;
                SOC_LPM_STATE_START(u, pfx) = -1;
                SOC_LPM_STATE_END(u, pfx) = -1;
                SOC_LPM_STATE_FENT(u, pfx) = 0;
                if (SOC_LPM_STATE_FENT(u, curr_pfx)) {
                    /* fent = 0, index in paired tcam for V6, if prev has
                     * fent, then figureout the start of unpaired tcam
                     * and try to get that index
                     */
                    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
                     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
                     * L3_DEFIP table is not divided into 2 to support URPF.
                     */
                    if (SOC_URPF_STATUS_GET(u) &&
                        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
                        num_ipv6_128b_entries >>= 1;
                    }
#ifdef BCM_UTT_SUPPORT
                    if (soc_feature(u, soc_feature_utt)) {
                        SOC_IF_ERROR_RETURN
                            (soc_utt_lpm_unreserved_entry_count(u, &unpaired_start));
                    } else
#endif
                    {
                        unpaired_start = (tcam_depth -
                                         (num_ipv6_128b_entries % tcam_depth)) << 1;
                    }
                    /* Need not check if num_ipv6_128b_entries % tcam_depth == 0
                     * as _lpm_entry_in_paired_tcam will be true only if it is 
                     * not zero. 
                     */
                    if (unpaired_start <= soc_mem_index_max(u, mem)) {
                        occupied_index = SOC_LPM_STATE_END(u, curr_pfx) +
                                         SOC_LPM_STATE_FENT(u, curr_pfx);
                        if ((SOC_LPM_STATE_END(u, curr_pfx) < unpaired_start) &&
                            (occupied_index - unpaired_start >= 0)) {
                            SOC_LPM_STATE_FENT(u, pfx) = occupied_index -
                                                    unpaired_start + 1;
                            SOC_LPM_STATE_FENT(u, curr_pfx) -=
                                SOC_LPM_STATE_FENT(u, pfx);
                            _lpm_free_slot_remove_empty_pfx(u, curr_pfx);
                            SOC_LPM_STATE_START(u, pfx) = unpaired_start;
                            SOC_LPM_STATE_END(u, pfx) = unpaired_start - 1;
                            v6_index_in_paired_tcam = 0;
                        }
                    }
                }
            } else {
                SOC_LPM_STATE_FENT(u, pfx) =  (SOC_LPM_STATE_FENT(u, curr_pfx) + 1) / 2;
                SOC_LPM_STATE_FENT(u, curr_pfx) -= SOC_LPM_STATE_FENT(u, pfx);
                _lpm_free_slot_remove_empty_pfx(u, curr_pfx);
            }
        } else {
            SOC_LPM_STATE_FENT(u, pfx) =  (SOC_LPM_STATE_FENT(u, curr_pfx) + 1) / 2;
            SOC_LPM_STATE_FENT(u, curr_pfx) -= SOC_LPM_STATE_FENT(u, pfx);
            _lpm_free_slot_remove_empty_pfx(u, curr_pfx);
        }
    } else if (!ipv6) {
        /* For IPv4 Check if alternate entry is free */
        from_ent = SOC_LPM_STATE_START(u, pfx);
        if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
            /* Just occupied a new entry */
            SOC_LPM_STATE_VENT(u, pfx) += 1;
            SOC_LPM_STATE_FENT(u, pfx) -= 1;
            SOC_LPM_STATE_END(u, pfx) += 1;
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                *free_slot = (from_ent << 1) + 1;
            } else 
#endif
            {
                SOC_LPM_STATE_HFENT(u, pfx) = 1;
                *free_slot = from_ent << 1;
            }
            sal_memcpy(e, soc_mem_entry_null(u, mem),
                       soc_mem_entry_words(u,mem) * 4);
            return (SOC_E_NONE);
        }
        
#ifdef FB_LPM_TABLE_CACHED
        if ((rv = soc_mem_cache_invalidate(u, mem,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
            return rv;
        }
#endif /* FB_LPM_TABLE_CACHED */

        if (SOC_LPM_STATE_HFENT(u, pfx)) {
           from_ent = SOC_LPM_STATE_END(u, pfx);
#ifdef FB_LPM_TABLE_CACHED
           if ((rv = soc_mem_cache_invalidate(u, mem,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
              return rv;
           }
#endif /* FB_LPM_TABLE_CACHED */
           if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e)) < 0) {
              return rv;
           }
           v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
           v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

           if ((v0 == 0) || (v1 == 0)) {
              *free_slot = (from_ent << 1) + ((v0 == 0) ? 0 : 1);
              SOC_LPM_STATE_HFENT(u, pfx) = 0;
              return(SOC_E_NONE);
           }
        }
    }

    /* VENT[pfx] == 0 || FENT[pfx] == 0 */
    free_pfx = pfx;
    while (SOC_LPM_STATE_FENT(u, free_pfx) == 0) {
        free_pfx = SOC_LPM_STATE_NEXT(u, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }
    while (v6_index_in_paired_tcam == 0 && 
           SOC_LPM_STATE_FENT(u, free_pfx) == 0) {
        free_pfx = SOC_LPM_STATE_PREV(u, free_pfx);
        if (free_pfx == -1) {
            if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
                /* failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = SOC_LPM_STATE_PREV(u, pfx);
                next_pfx = SOC_LPM_STATE_NEXT(u, pfx);
                if (-1 != prev_pfx) {
                    SOC_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    SOC_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
                }
            }
            return(SOC_E_FULL);
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = SOC_LPM_STATE_NEXT(u, free_pfx); 
        if (soc_feature(u, soc_feature_l3_shared_defip_table) ||
            (soc_feature(u, soc_feature_l3_kt2_shared_defip_table))) {
            /*
             * If the next_pfx is of type 64b V6 and if the free_pfx is in 
             * paired_tcam space, we should not move the entry.
             * We will move the entry from the next prefix. 
             * The assumption here is order of V4 and V6 entries are independent
             */
            if (SOC_LPM_PFX_IS_V6_64(u, next_pfx) == TRUE) {
                to_ent = SOC_LPM_STATE_START(u, next_pfx) - 1;
                if (_lpm_entry_in_paired_tcam(u, to_ent)) {

                    /* encountered V6 prefix which cannot be moved to 
                     * paired tcam. When we move a pfx, the from entry will
                     * not be removed assuming that the location will be used
                     * by next prefix. In this case we did not write the V6 
                     * entry. so we need to delete the from entry. 
                     * Ideally we should delete hash and set valid bits to 0 
                     * in _lpm_fb_entry_shift for the from_entry. But in most 
                     * of the cases, the from entry will be used by next prefix
                     * so we can reduce one read/write of L3_DEFIP if we dont 
                     * delete the entry in _lpm_fb_entry_shift
                     * free_pfx contains the previous pfx from which entry was 
                     * moved 
                     */
                     
                    from_ent = SOC_LPM_STATE_END(u, free_pfx) + 1;
                    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                                       from_ent, entry);
                    SOC_IF_ERROR_RETURN(rv);
                    SOC_MEM_OPT_F32_VALID0_SET(u, mem, entry, 0);
                    SOC_MEM_OPT_F32_VALID0_SET(u, mem, entry, 0);
                    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, 
                                                        from_ent, entry));
                    if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
                        SOC_IF_ERROR_RETURN(_lpm_fb_urpf_entry_replicate(u,
                                        from_ent, -1, entry));
                    }
                    if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
                        /* failed to allocate entries for a newly allocated prefix.*/
                        prev_pfx = SOC_LPM_STATE_PREV(u, pfx);
                        next_pfx = SOC_LPM_STATE_NEXT(u, pfx);
                        if (-1 != prev_pfx) {
                            SOC_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
                        }
                        if (-1 != next_pfx) {
                            SOC_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
                        }
                    }
                    return (SOC_E_FULL);
                }
            }
        }
        SOC_IF_ERROR_RETURN(_lpm_fb_shift_pfx_down(u, next_pfx, ipv6));
        SOC_LPM_STATE_FENT(u, free_pfx) -= 1;
        SOC_LPM_STATE_FENT(u, next_pfx) += 1;
        _lpm_free_slot_remove_empty_pfx(u, free_pfx);
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        SOC_IF_ERROR_RETURN(_lpm_fb_shift_pfx_up(u, free_pfx, ipv6));
        SOC_LPM_STATE_FENT(u, free_pfx) -= 1;
        prev_pfx = SOC_LPM_STATE_PREV(u, free_pfx); 
        SOC_LPM_STATE_FENT(u, prev_pfx) += 1;
        _lpm_free_slot_remove_empty_pfx(u, free_pfx);
        free_pfx = prev_pfx;
    }

    if (soc_feature(u, soc_feature_l3_shared_defip_table) ||
        (soc_feature(u, soc_feature_l3_kt2_shared_defip_table))) {
        if (v6_index_in_paired_tcam != 0 && free_pfx == pfx) {
            /* could not find a entry in  unpaired tcam */
            if (SOC_LPM_STATE_VENT(u, pfx) == 0) {
                prev_pfx = SOC_LPM_STATE_PREV(u, pfx);
                next_pfx = SOC_LPM_STATE_NEXT(u, pfx);
                if (SOC_LPM_STATE_FENT(u, pfx)) {
                    SOC_LPM_STATE_START(u, pfx) =
                        SOC_LPM_STATE_START(u, next_pfx) - 1;
                    SOC_LPM_STATE_END(u, pfx) = SOC_LPM_STATE_START(u, pfx) - 1;
                } else {
                    /* failed to allocate entries for a newly allocated prefix.*/
                    if (-1 != prev_pfx) {
                        SOC_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
                    }
                    if (-1 != next_pfx) {
                        SOC_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
                    }
                    return SOC_E_FULL;
                }
            }
        }
    }

    SOC_LPM_STATE_VENT(u, pfx) += 1;
    SOC_LPM_STATE_FENT(u, pfx) -= 1;
    SOC_LPM_STATE_END(u, pfx) += 1;
    if (ipv6 == 0) {
       SOC_LPM_STATE_HFENT(u, pfx) = 
           soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity) ? 0 : 1;
    }
    *free_slot = SOC_LPM_STATE_END(u, pfx) <<  ((ipv6) ? 0 : 1);
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        *free_slot += (ipv6) ? 0 : 1; 
    }
#endif
    LOG_DEBUG(BSL_LS_SOC_LPM, (BSL_META_U(u,
      "ADD - pfx[%d]: start:%d end:%d vent:%d fent:%d hfent:%d free_slot:%d\n\r"),
             pfx, soc_lpm_state[u][pfx].start, soc_lpm_state[u][pfx].end,
             soc_lpm_state[u][pfx].vent, soc_lpm_state[u][pfx].fent,
             soc_lpm_state[u][pfx].hfent, *free_slot));

    sal_memcpy(e, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u,mem) * 4);

    return(SOC_E_NONE);
}

/*
 *      Delete a slot and adjust entry pointers if required.
 *      e - has the contents of entry at slot(useful for IPV4 only)
 */
static
int _lpm_free_slot_delete(int u, int pfx, int ipv6, void *e, int slot)
{
    int         from_ent;
    int         to_ent;
    int         temp_index = -1;
    int         temp_hw_index = -1;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    void        *et;
    int         rv;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    uint32      e_temp[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         half_entry_free = 0;
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }


    from_ent = SOC_LPM_STATE_END(u, pfx);
    to_ent = slot;
    if (!ipv6) { /* IPV4 */
        to_ent >>= 1;
#ifdef FB_LPM_TABLE_CACHED
        if ((rv = soc_mem_cache_invalidate(u, mem,
                                           MEM_BLOCK_ANY, from_ent)) < 0) {
            return rv;
        }
#endif /* FB_LPM_TABLE_CACHED */
        if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
            return rv;
        }
        et = (to_ent == from_ent) ? ef : e;

        half_entry_free = SOC_LPM_STATE_HFENT(u, pfx);
        if (soc_feature(u, soc_feature_lpm_atomic_write)) {
           if (((to_ent == from_ent) && (half_entry_free == 0)) ||
               (to_ent != from_ent)) {
              /* Reset half free entry value. */ 
              SOC_LPM_STATE_HFENT(u, pfx) = 0;
              /* Create new slot */
              rv = _lpm_free_slot_create(u, pfx, ipv6, e_temp, &temp_index);
              if (SOC_FAILURE(rv)) {
                 return rv;
              }

              /* Increment the Half entry count due to recent temp entry. */
              SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));

              temp_hw_index = temp_index >> 1;
              /*
               * In case, getting the free slot from higher prefix,
               * entries get moved up and from_ent index gets modified.
               */
              if (temp_hw_index == from_ent) {
                  /*
                   * Check End index has half entry then 'from_ent - 1'
                   * will be moved to start index and 'from_ent' moves by 1.
                   */
                 if (half_entry_free) {
                    if ((from_ent - 1) == to_ent) {
                       to_ent = SOC_LPM_STATE_START(u, pfx);
                    }
                    from_ent = temp_hw_index - 1;
                 } else {
                    if (to_ent == from_ent) {
                       to_ent = SOC_LPM_STATE_START(u, pfx);
                    }
                    from_ent = SOC_LPM_STATE_END(u, pfx) - 1;
                    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef);
                    if (SOC_FAILURE(rv)) {
                       LOG_ERROR(BSL_LS_SOC_LPM, (BSL_META_U(u,
                          "Failed to read entry from index:%d\n\r"), from_ent));
                       return SOC_E_INTERNAL;
                    }
                 }
              }

              if (slot & 1) {
                 rv = soc_fb_lpm_ip4entry0_to_0(u, e, e_temp, PRESERVE_HIT);
              } else {
                 rv = soc_fb_lpm_ip4entry1_to_0(u, e, e_temp, PRESERVE_HIT);
              }
              SOC_IF_ERROR_RETURN(rv);

              /* Write left over original entry to new slot 'temp_index' */ 
              rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, e_temp);
              SOC_IF_ERROR_RETURN(rv);

              if (to_ent != from_ent) {
                 /* Copy end slot entry to temp buffer */
                 if (half_entry_free) {
                    rv = soc_fb_lpm_ip4entry0_to_1(u, ef, e_temp, PRESERVE_HIT);
                 } else {
                    rv = soc_fb_lpm_ip4entry1_to_1(u, ef, e_temp, PRESERVE_HIT);
                 }                 
                 SOC_IF_ERROR_RETURN(rv);
              }
           } else {
              /* In case of single entry. */
              SOC_LPM_STATE_VENT(u, pfx) -= 1;
              SOC_LPM_STATE_FENT(u, pfx) += 1;
              SOC_LPM_STATE_END(u, pfx) -= 1;
           }

           /*
            *  Write temp buffer (left over original entry + adjusted end slot)
            *  to the original entry.
            */
           LPM_HASH_INSERT(u, e_temp, to_ent, &rvt_index0, &rvt_index1);
           rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e_temp);
           if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (SOC_SUCCESS(rv))) {
              rv = _lpm_fb_urpf_entry_replicate(u, to_ent, temp_hw_index, e_temp);
              if (SOC_FAILURE(rv)) {
                 LPM_HASH_REVERT(u, e_temp, to_ent, rvt_index0, rvt_index1);
                 return rv;
              }
           }
           LPM_AVL_INSERT(u, e_temp, to_ent);

           /* Update END slot */ 
           if (to_ent != from_ent) {
              sal_memset(e_temp, 0x0, sizeof(e_temp));
              if (half_entry_free == 0) {
                 rv = soc_fb_lpm_ip4entry0_to_0(u, ef, e_temp, PRESERVE_HIT);
                 SOC_IF_ERROR_RETURN(rv);

                 /* Write temp buffer to new temp slot */
                 rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, e_temp);
                 SOC_IF_ERROR_RETURN(rv);
              } else {
                 /* Remove End Slot */
                 SOC_LPM_STATE_VENT(u, pfx) -= 1;
                 SOC_LPM_STATE_FENT(u, pfx) += 1;
                 SOC_LPM_STATE_END(u, pfx) -= 1;
              }
              /* Write temp buffer to End slot */ 
              LPM_HASH_INSERT(u, e_temp, from_ent, &rvt_index0, &rvt_index1);
              rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e_temp);
              if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (SOC_SUCCESS(rv))) {
                 rv = _lpm_fb_urpf_entry_replicate(u, from_ent, temp_hw_index, e_temp);
              }

              if (SOC_FAILURE(rv)) {
                 LPM_HASH_REVERT(u, e_temp, from_ent, rvt_index0, rvt_index1);
                 return rv;
              }
              LPM_AVL_INSERT(u, e_temp, from_ent);
           }
          
           if (temp_index != -1) {
              /* Delete temp slot */
              SOC_LPM_STATE_VENT(u, pfx) -= 1;
              SOC_LPM_STATE_FENT(u, pfx) += 1;
              SOC_LPM_STATE_END(u, pfx) -= 1;
              /* Decrement the global Half entry count due to recent temp entry. */
              SOC_LPM_COUNT_DEC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));

              sal_memcpy(e_temp, soc_mem_entry_null(u, mem),
                         soc_mem_entry_words(u,mem) * 4);
              /* Write temp buffer to End slot */ 
              rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, e_temp);
              SOC_IF_ERROR_RETURN(rv);
           }
      
           if (half_entry_free) {
              /* Remaining half entry got deleted. Decrement half entry count.*/
              SOC_LPM_COUNT_DEC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
           } else {
              /* Incremented half entry. Created after deleting existing half.*/
              SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
           }
        } else {
           if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, ef)) {
               if (slot & 1) {
                   rv = soc_fb_lpm_ip4entry1_to_1(u, ef, et, PRESERVE_HIT);
               } else {
                   rv = soc_fb_lpm_ip4entry1_to_0(u, ef, et, PRESERVE_HIT);
               }
               SOC_MEM_OPT_F32_VALID1_SET(u, mem, ef, 0);
               /* first half of full entry is deleted, generating half entry.*/
               if (((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
                   (soc_feature(u, soc_feature_shared_defip_stat_support))) && 
                   !(soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity))) {
                   SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
               }
#ifdef BCM_TOMAHAWK2_SUPPORT               
               if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                   SOC_LPM_STATE_VENT(u, pfx) -= 1;
                   SOC_LPM_STATE_FENT(u, pfx) += 1;
                   SOC_LPM_STATE_END(u, pfx) -= 1;
               }
#endif               
           } else {
               if (slot & 1) {
                   rv = soc_fb_lpm_ip4entry0_to_1(u, ef, et, PRESERVE_HIT);
               } else {
                   rv = soc_fb_lpm_ip4entry0_to_0(u, ef, et, PRESERVE_HIT);
               }
               SOC_MEM_OPT_F32_VALID0_SET(u, mem, ef, 0);
               /* Remaining half entry got deleted. Decrement half entry count.*/ 
               if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
                   (soc_feature(u, soc_feature_shared_defip_stat_support))) {
                   SOC_LPM_COUNT_DEC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
               } 
               SOC_LPM_STATE_VENT(u, pfx) -= 1;
               SOC_LPM_STATE_FENT(u, pfx) += 1;
               SOC_LPM_STATE_END(u, pfx) -= 1;
           }

           if (to_ent != from_ent) {
               LPM_HASH_INSERT(u, et, to_ent, &rvt_index0, &rvt_index1);
               if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, et)) < 0) {
                   LPM_HASH_REVERT(u, et, to_ent, rvt_index0, rvt_index1);
                   return rv;
               }
               if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
                   if ((rv = _lpm_fb_urpf_entry_replicate(u, to_ent, -1, et)) < 0) {
                       LPM_HASH_REVERT(u, et, to_ent, rvt_index0, rvt_index1);
                       return rv;
                   }
               }
               LPM_AVL_INSERT(u, et, to_ent);
           }
           LPM_HASH_INSERT(u, ef, from_ent, &rvt_index0, &rvt_index1);
           if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
               LPM_HASH_REVERT(u, ef, from_ent, rvt_index0, rvt_index1);
               return rv;
           }
           if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
               if ((rv = _lpm_fb_urpf_entry_replicate(u, from_ent, -1, ef)) < 0) {
                   LPM_HASH_REVERT(u, ef, from_ent, rvt_index0, rvt_index1);
                   return rv;
               }
           }
           LPM_AVL_INSERT(u, ef, from_ent);
        }
     
        /* Update half entry for the given prefix */        
        if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
            SOC_LPM_STATE_HFENT(u, pfx) = (half_entry_free) ? 0 : 1;
        }
        LOG_DEBUG(BSL_LS_SOC_LPM, (BSL_META_U(u,
          "DEL - pfx[%d]: start:%d end:%d vent:%d fent:%d hfent:%d slot:%d\n\r"),
             pfx, soc_lpm_state[u][pfx].start, soc_lpm_state[u][pfx].end,
             soc_lpm_state[u][pfx].vent, soc_lpm_state[u][pfx].fent,
             soc_lpm_state[u][pfx].hfent, slot));

    } else { /* IPV6 */
        SOC_LPM_STATE_VENT(u, pfx) -= 1;
        SOC_LPM_STATE_FENT(u, pfx) += 1;
        SOC_LPM_STATE_END(u, pfx) -= 1;
        if (to_ent != from_ent) {
#ifdef FB_LPM_TABLE_CACHED
            if ((rv = soc_mem_cache_invalidate(u, mem,
                                               MEM_BLOCK_ANY, from_ent)) < 0) {
                return rv;
            }
#endif /* FB_LPM_TABLE_CACHED */
            if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
                return rv;
            }
            LPM_HASH_INSERT(u, ef, to_ent, &rvt_index0, &rvt_index1);
            if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, ef)) < 0) {
                LPM_HASH_REVERT(u, ef, to_ent, rvt_index0, rvt_index1);
                return rv;
            }
            if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
                if ((rv = _lpm_fb_urpf_entry_replicate(u, to_ent, -1, ef)) < 0) {
                    LPM_HASH_REVERT(u, ef, to_ent, rvt_index0, rvt_index1);
                    return rv;
                }
            }
            LPM_AVL_INSERT(u, ef, to_ent);
        }

        sal_memcpy(ef, soc_mem_entry_null(u, mem),
                   soc_mem_entry_words(u,mem) * 4);
        LPM_HASH_INSERT(u, ef, from_ent, &rvt_index0, &rvt_index1);
        if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
            LPM_HASH_REVERT(u, ef, from_ent, rvt_index0, rvt_index1);
            return rv;
        }
        if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
            if ((rv = _lpm_fb_urpf_entry_replicate(u, from_ent, -1, ef)) < 0) {
                LPM_HASH_REVERT(u, ef, from_ent, rvt_index0, rvt_index1);
                return rv;
            }
        }
        LPM_AVL_INSERT(u, ef, from_ent);
    }

    if ((SOC_LPM_STATE_VENT(u, pfx) == 0) &&
        (SOC_LPM_STATE_FENT(u, pfx) == 0)) {
        _lpm_free_slot_remove_empty_pfx(u, pfx);
    }

    return(rv);
}


/*
 * Function:
 *      _soc_fb_lpm_vrf_get
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
soc_fb_lpm_vrf_get(int unit, void *lpm_entry, int *vrf)
{
    int vrf_id;
    int vrf_mask;
    int ipv6mc_key=0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        uint32 _key_data[2] = {0, 0};
        int global_low = 0, vrf_id_mask;
        SOC_TH3_MEM_OPT_F64_GET(unit, L3_DEFIP_LEVEL1m, lpm_entry, KEY0f,_key_data);
        vrf_id = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf);
        SOC_TH3_MEM_OPT_F64_GET(unit, L3_DEFIP_LEVEL1m, lpm_entry, MASK0f,_key_data);
        vrf_id_mask = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf);
        SOC_TH3_MEM_OPT_F64_GET(unit, L3_DEFIP_LEVEL1m, lpm_entry, FIXED_DATA0f,_key_data);
        global_low = soc_format_field32_get(unit, FIXED_DATAfmt, _key_data, SUB_DB_PRIORITYf);
        if (global_low == 2) {
           global_low = 1;
        } else {
           global_low = 0;
        }
        if (vrf_id_mask) {
            *vrf = vrf_id;
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = SOC_L3_VRF_GLOBAL;
        } else {
            *vrf = SOC_L3_VRF_OVERRIDE;
            if (global_low) {
                *vrf = SOC_L3_VRF_GLOBAL;
            }
        }
        return (SOC_E_NONE);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        int key_type = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, KEY_TYPE0f);
        ipv6mc_key = (key_type == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    }
#endif

    /* Get Virtual Router id if supported. */
    if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, VRF_ID_MASK0f) || ipv6mc_key) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (ipv6mc_key) {
            vrf_id = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IPMC_V6_VRF_IDf);
            vrf_mask = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, IPMC_V6_VRF_ID_MASKf);
        } else
#endif
        {
            vrf_id = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_0f);
            vrf_mask = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASK0f);
        }
        /* Special vrf's handling. */
        if (vrf_mask) {
            *vrf = vrf_id;    
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = SOC_L3_VRF_GLOBAL;
        } else {
            *vrf = SOC_L3_VRF_OVERRIDE;    
            if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_ROUTE0f)) {
                if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f)) {
                    *vrf = SOC_L3_VRF_GLOBAL; 
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf = SOC_L3_VRF_DEFAULT;
    }
    return (SOC_E_NONE);
}

STATIC int
_soc_fb_lpm_prefix_get_by_len(int u, int ipv6, int vrf_type, int len, int *pfx, int mc)
{
    int         pfx_len = len;
    int         n = 0, d = MAX_LPM_ZONE(u);

    if (ipv6) {
        if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
            (soc_feature(u, soc_feature_l3_kt2_shared_defip_table)))) {
            /*
             * if using shared defip table, 64B V6 addresses have lower
             * prefix length than V4 address. This is because, V4 entries 
             * can be added to the paired tcam but not 64B V6 addresses.
             * So V4 entries need to occupy the lower addresses to be contigous 
             * in software.
             */     
         
            pfx_len += IPV6_PFX_ZERO; /* First 33 are for IPV4 addresses */
        }
    } else {
        if (soc_feature(u, soc_feature_l3_shared_defip_table) ||
            (soc_feature(u, soc_feature_l3_kt2_shared_defip_table))) {
            /*
             * V4 prefixes should be at lower indices that 64B prefixes
             * as V4 entries can be added to the paired tcam as well.
             * The max V4 prefix length is 295.
             * Max 64B prefix length is 262.
             */
            pfx_len += IPV4_PFX_ZERO(u);
        } 
    }

    switch (vrf_type) { 
    case SOC_L3_VRF_GLOBAL:
        n = 0;
        break;
    case SOC_L3_VRF_OVERRIDE:
        n = 2;
        break;
    default:
        n = 1;
        break;
    }

    if (soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc) {
        n += 3;
    }

    *pfx = pfx_len + n * (MAX_PFX_ENTRIES(u) / d);

    return (SOC_E_NONE);
}


/*
 * _soc_fb_lpm_prefix_length_get (Extract vrf weighted  prefix lenght from the 
 * hw entry based on ip, mask & vrf)
 */
STATIC int
_soc_fb_lpm_prefix_length_get(int u, void *entry, int *pfx_len) 
{
    int         pfx;
    int         rv;
    int         ipv6;
    uint32      ipv4a = 0;
    int         vrf_id;
    int         mc = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
    soc_mem_t   mem = L3_DEFIPm;
#endif
  
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 _key_data[2] = {0, 0};
#endif

#ifdef BCM_TRIDENT3_SUPPORT
    uint32      key_type = 0;

    if (soc_mem_field_valid(u, mem, KEY_TYPE0f)) {
        key_type = soc_mem_field32_get(u, mem, entry, KEY_TYPE0f);
    }

    if (soc_mem_field_valid(u, mem, DATA_TYPE0f)) {
        mc = (soc_mem_field32_get(u, mem, entry, DATA_TYPE0f) == 2);
    }

    if (soc_feature(u, soc_feature_flex_flow) &&
        BCMI_LPM_FLEX_VIEW(key_type)) {

            SOC_IF_ERROR_RETURN(
                 soc_td3_lpm_flex_prefix_length_get(u, entry,
                                         &ipv6, &vrf_id, &pfx, key_type));
    }
    else
#endif
    {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, KEY0f,_key_data);
            ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, MASK0f,_key_data);
            ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        } else 
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_separate_key_for_ipmc_route) && (key_type != 0)) {
            ipv6 = (key_type == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        } else
#endif
        {
            ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, defip_mode0_field);
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(u, soc_feature_separate_key_for_ipmc_route) &&
            key_type == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE) {
            SOC_IF_ERROR_RETURN(
                soc_td3_lpm_ipv6mc_prefix_length_get(u, entry, &pfx));
        } else
#endif
        if (ipv6) {
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
                return(rv);
            }
            if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
                ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK1f);
            }
#if defined(BCM_TOMAHAWK3_SUPPORT)
            else {
                SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, MASK1f,_key_data);
                ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
            }
#endif
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
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
                SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, MASK0f,_key_data);
                ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
            } else
#endif
            {
                ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_MASK0f);
            }
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) < 0) {
                return(rv);
            }
        }

        SOC_IF_ERROR_RETURN(soc_fb_lpm_vrf_get(u, entry, &vrf_id));
    }

    return _soc_fb_lpm_prefix_get_by_len(u, ipv6, vrf_id, pfx, pfx_len, mc);
}


/*
 * _soc_fb_lpm_match (Exact match for the key. Will match both IP address
 * and mask)
 */
static
int
_soc_fb_lpm_match(int u,
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

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        uint32 _key_data[2] = {0,0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f,_key_data);
        v6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        if (v6) {
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY1f,_key_data);
            v6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
            if (!v6) {
                return (SOC_E_PARAM);
            }
        }
    } else
#endif
    {

        v6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);

        if (v6) {
            if (!(v6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode1_field))) {
                return (SOC_E_PARAM);
            }
        }
    }
    *ipv6 = v6;

    /* Calculate vrf weighted prefix lengh. */
    _soc_fb_lpm_prefix_length_get(u, key_data, &pfx); 
    *pfx_len = pfx; 

#ifdef FB_LPM_HASH_SUPPORT
    if (LPM_HASH_LOOKUP(u, key_data, pfx, &key_index) == SOC_E_NONE) {
        *index_ptr = key_index;
        if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
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

#if defined(FB_LPM_AVL_SUPPORT)
    if (LPM_AVL_LOOKUP(u, key_data, pfx, &key_index) == SOC_E_NONE) {
#ifdef  FB_LPM_CHECKER_ENABLE
        if (*index_ptr != key_index) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "\nsoc_fb_lpm_locate: HASH %d AVL %d\n"), *index_ptr, key_index));
        }
        assert(*index_ptr == key_index);
#endif
        *index_ptr = key_index;
        if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                         (*ipv6) ? *index_ptr : (*index_ptr >> 1), e)) < 0) {
            return rv;
        }
        return(SOC_E_NONE);
    } else {
        return(SOC_E_NOT_FOUND);
    }
#endif /* FB_LPM_AVL_SUPPORT */
}

int soc_fb_lpm_tcam_pair_count_get(int u, int *tcam_pair_count)
{
    int num_ipv6_128b_entries;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);

    if (tcam_pair_count == NULL) {
        return SOC_E_PARAM;
    }

#ifdef BCM_UTT_SUPPORT
    if (soc_feature(u, soc_feature_utt)) {
        *tcam_pair_count  = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(u);
        return SOC_E_NONE;
    }
#endif

    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    if (num_ipv6_128b_entries) {
        *tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        *tcam_pair_count = 0;
    }
    
    return SOC_E_NONE;
}

STATIC int 
_soc_lpm_max_v4_route_get(int u, int paired, uint32 *entries)
{
    int is_reserved = 0;
    int paired_table_size = 0;
    int defip_table_size = 0;
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    if (!paired && !soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF.
         */
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            *entries = (soc_mem_index_count(u, mem));
        } else {
            *entries = (soc_mem_index_count(u, mem) << 1);
        }
        return SOC_E_NONE;
    }

    if (paired && !soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries = 0;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_table_size, 
                                                   &defip_table_size));
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }
    if (paired) {
        if (is_reserved) {
            *entries = paired_table_size - (max_v6_entries << 1);
        } else {
            *entries = paired_table_size;
        }
    } else {
        *entries = defip_table_size;
    }
    *entries <<= 1;
    return SOC_E_NONE;
}

STATIC int 
_soc_lpm_max_64bv6_route_get(int u, int paired, uint16 *entries)
{
    int is_reserved = 0;
    int paired_table_size = 0;
    int defip_table_size = 0;
    int tcam_pair_count = 0;
    int max_pair_count = 0;
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(u);

#ifdef BCM_UTT_SUPPORT
    if (soc_feature(u, soc_feature_utt)) {
        return soc_utt_lpm_max_64bv6_route_get(u, paired, entries);
    }
#endif

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    if (!paired) {
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF.
         */
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            max_v6_entries >>= 1;
            tcam_pair_count = (max_v6_entries / tcam_depth) + 
                              (max_v6_entries % tcam_depth ? 1 : 0);
            max_pair_count = max_tcams >> 2;
            *entries = ((max_pair_count - tcam_pair_count) * tcam_depth);
        } else {
            tcam_pair_count = (max_v6_entries / tcam_depth) + 
                              (max_v6_entries % tcam_depth ? 1 : 0);
            max_pair_count = max_tcams >> 1;
            *entries = ((max_pair_count - tcam_pair_count) * tcam_depth);
        }
        *entries <<= 1;
        return SOC_E_NONE;
    }

    if (paired && soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_table_size, 
                                                       &defip_table_size));
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF.
         */
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            max_v6_entries >>= 1;
        }
        *entries = (paired_table_size >> 1);
        if (is_reserved) {
            *entries -= (max_v6_entries);
        }
        return SOC_E_NONE;
    }
    *entries = 0;
    return SOC_E_NONE;
}

STATIC int
_soc_lpm_max_128bv6_route_get(int u, uint16 *entries)
{
    int is_reserved = 0;
    int paired_table_size = 0;
    int defip_table_size = 0;
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF.
         */
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
            max_v6_entries >>= 1;
        }
        *entries = max_v6_entries;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_table_size, 
                                                   &defip_table_size));
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }
    if (is_reserved) {
        *entries = max_v6_entries;
    } else {
        *entries = paired_table_size >> 1;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb_lpm_state_config
 * Purpose:
 *      Config LPM prefix state table by property
 * Parameters:
 *      u - Device unit
 *      ipv6_64_depth - L3_DEFIP memory table depth
 *      start   - start memory index to configure
 * Returns:
 *      SOC_E_NONE
 */ 
int
soc_fb_lpm_state_config(int u, int ipv6_64_depth, int start)
{
    int i;
    int pfx, curr_pfx, next_pfx, prev_pfx;
    int ipver, vrf_type, plen, entry_number, max_pfx_number;
    int entry_total_count = 0;
    char *cfg_str;
    char sub_str[128];

    char *tokstr;
    char *ipver_str, *vrf_str, *len_str, *num_str;

#define MAX_PFX_CONF_NUM    64

    if (SOC_WARM_BOOT(u)) {
        return SOC_E_NONE;
    }
    
    if (soc_property_get(u, spn_LPM_LAYOUT_PREFIX_ENABLE, 0) == FALSE) {
        return SOC_E_NONE;
    }

    /* LPM prefix layout enable */
    for (i = 0; i <= MAX_PFX_INDEX(u); i++) {
        SOC_LPM_STATE_START(u, i) = -1;
        SOC_LPM_STATE_END(u, i) = -1;
        SOC_LPM_STATE_PREV(u, i) = -1;
        SOC_LPM_STATE_NEXT(u, i) = -1;
        SOC_LPM_STATE_VENT(u, i) = 0;
        SOC_LPM_STATE_FENT(u, i) = 0;
    }

    for (i = 1; i <= MAX_PFX_CONF_NUM; i++) {
        cfg_str = soc_property_suffix_num_str_get(u, i, spn_LPM_LAYOUT, "prefix");
        if (cfg_str == NULL) {
            continue;
        }

        /* lpm_layout_prefix<idx>=
         *   <IP version>:<VRF type>:<prefix length>:<route number>
         */
        if (sal_strlen(cfg_str) < sizeof(sub_str)) {
            /* coverity [secure_coding : FALSE] */
            sal_strcpy(sub_str, cfg_str);
        } else {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid string \"%s\"\n"),
                       i, cfg_str));
            continue;              
        }

        ipver_str = sal_strtok_r(sub_str, ":", &tokstr);
        vrf_str = sal_strtok_r(NULL, ":", &tokstr);
        len_str = sal_strtok_r(NULL, ":", &tokstr);
        num_str = sal_strtok_r(NULL, ":", &tokstr);

        if (ipver_str == NULL || vrf_str == NULL || 
            len_str == NULL || num_str == NULL) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid string \"%s\"\n"),
                       i, cfg_str));
            continue;          
        }

        /* Parsing physical port number */
        ipver = sal_ctoi(ipver_str, NULL);
        if (ipver != 4 && ipver != 6) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid IP version %d\n"),
                       i, ipver));
            continue;
        }

        /* Parsing VRF configuration */
        vrf_type = sal_ctoi(vrf_str, NULL);
        if (vrf_type != 0 && vrf_type != 1 && vrf_type != 2) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid VRF value %d\n"),
                       i, vrf_type));
            continue;
        } 
        if (vrf_type == 1) {
            vrf_type = SOC_L3_VRF_OVERRIDE;
        } else if (vrf_type == 2) {
            vrf_type = SOC_L3_VRF_GLOBAL;
        }

        /* Parsing prefix length */
        plen = sal_ctoi(len_str, NULL);

        /* Only prefix length <= 64 allowed */
        if (!((ipver == 4 && plen >= 0 && plen <= 32) || 
              (ipver == 6 && plen >= 0 && plen <= 64))) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid prefix length value %d\n"),
                       i, plen));
            continue;
        }

        /* Parsing prefix length */
        entry_number = sal_ctoi(num_str, NULL);
        if (entry_number < 0) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "Prefix index %d: Invalid entry number value %d\n"),
                       i, entry_number));
            continue;
        }

        if (ipver == 4) {
            entry_number = (entry_number + 1) / 2;
        }
        entry_total_count += entry_number;
        if (entry_total_count > ipv6_64_depth) {
            LOG_WARN(BSL_LS_SOC_LPM,
                     (BSL_META_U(u,
                                 "LPM Prefix Layout: Exceeded max capacity %d(cur %d) in "
                                 "L3_DEFIP, ignore rest configuration.\n"), 
                      ipv6_64_depth, entry_total_count));
            entry_number -= entry_total_count - ipv6_64_depth;
            entry_total_count = ipv6_64_depth;
            
            /* Force to break by setting i to SOC_PREFIX_COUNT_NUMBER */
            i = MAX_PFX_CONF_NUM;
        }

        /* Layout LPM prefix state in 1st round, we only create link and number
         * here, after all prefix configuration are ready, then we orginaze the 
         * start and end pointer */
        (void)_soc_fb_lpm_prefix_get_by_len(u, ipver == 6, vrf_type, plen, &pfx, 0);

        curr_pfx = MAX_PFX_INDEX(u);
        while (SOC_LPM_STATE_NEXT(u, curr_pfx) > pfx) {
            curr_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
        }

        SOC_LPM_STATE_FENT(u, pfx) = entry_number;
        if (curr_pfx != pfx) {
            next_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
            if (next_pfx != -1) {
                SOC_LPM_STATE_PREV(u, next_pfx) = pfx;
            }
            SOC_LPM_STATE_NEXT(u, pfx) = SOC_LPM_STATE_NEXT(u, curr_pfx);
            SOC_LPM_STATE_PREV(u, pfx) = curr_pfx;
            SOC_LPM_STATE_NEXT(u, curr_pfx) = pfx;
        }
    }

    /* ipv6_64_depth equals to soc_mem_index_count(mem)
     * So the rest memory we need to get back to MAX_PFX_INDEX */
    max_pfx_number = ipv6_64_depth - entry_total_count;

    /* Layout LPM prefix state in 2nd round */
    curr_pfx = MAX_PFX_INDEX(u);
    while (curr_pfx != -1) {
        if (curr_pfx == MAX_PFX_INDEX(u)) {
            /* We never use prefix MAX_PFX_INDEX, so we can set its start 
             * to 0, it won't break the lpm logic */ 
            SOC_LPM_STATE_START(u, curr_pfx) = start;
            SOC_LPM_STATE_END(u, curr_pfx) = start - 1;
            SOC_LPM_STATE_VENT(u, curr_pfx) = 0;
            SOC_LPM_STATE_FENT(u, curr_pfx) = max_pfx_number;
            
            curr_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
            continue;
        }

        /* Setup start index of each prefix base on free entry number. */
        prev_pfx = SOC_LPM_STATE_PREV(u, curr_pfx);
        SOC_LPM_STATE_START(u, curr_pfx) = 
            SOC_LPM_STATE_START(u, prev_pfx) + SOC_LPM_STATE_FENT(u, prev_pfx);
        SOC_LPM_STATE_END(u, curr_pfx) = 
            SOC_LPM_STATE_END(u, prev_pfx) + SOC_LPM_STATE_FENT(u, prev_pfx);
        SOC_LPM_STATE_VENT(u, curr_pfx) = 0;
        
        curr_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
    }

    return SOC_E_NONE;
}

int
soc_fb_lpm_stat_init(int u)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_NONE;
    }

    if (SOC_LPM_STAT_INIT_CHECK(u)) {
        sal_free(soc_lpm_stat[u]);
        soc_lpm_stat[u] = NULL;
    }

    soc_lpm_stat[u] = sal_alloc(sizeof(soc_lpm_stat_t), "LPM STATS");
    if (soc_lpm_stat[u] == NULL) {
        return SOC_E_MEMORY;
    }

    SOC_LPM_V4_COUNT(u) = 0;
    SOC_LPM_64BV6_COUNT(u) = 0;
    SOC_LPM_128BV6_COUNT(u) = 0;
    SOC_LPM_V4_HALF_ENTRY_COUNT(u) = 0;
    SOC_IF_ERROR_RETURN(_soc_lpm_max_v4_route_get(u, 0,
                                           &SOC_LPM_MAX_V4_COUNT(u))); 
    SOC_IF_ERROR_RETURN(_soc_lpm_max_64bv6_route_get(u, 0,
                                           &SOC_LPM_MAX_64BV6_COUNT(u))); 
    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        SOC_IF_ERROR_RETURN(_soc_lpm_max_128bv6_route_get(u,
                                           &SOC_LPM_MAX_128BV6_COUNT(u)));
    } else {
        SOC_LPM_MAX_128BV6_COUNT(u) = 0;
    }
    return SOC_E_NONE;
}

int
soc_fb_lpm128_stat_init(int u)
{
    if (SOC_LPM128_STATE(u) == NULL) {
        return SOC_E_INTERNAL;
    }
    SOC_LPM128_STAT_V4_COUNT(u) = 0;
    SOC_LPM128_STAT_64BV6_COUNT(u) = 0;
    SOC_LPM128_STAT_128BV6_COUNT(u) = 0;
    SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u) = 0;
    /* Now setup max values */

    if (soc_feature(u, soc_feature_defip_2_tcams_with_separate_rpf)) {
        int rpf_scale = 1;
        rpf_scale = (SOC_URPF_STATUS_GET(u) ? 1: 2);

        SOC_LPM128_STAT_MAX_V4_COUNT(u) = R2P_V4_MAX * rpf_scale;
        SOC_LPM128_STAT_MAX_64BV6_COUNT(u) = R2P_64V6_MAX * rpf_scale;
        SOC_LPM128_STAT_MAX_128BV6_COUNT(u) =  R2P_128V6_MAX * rpf_scale;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_soc_lpm_max_v4_route_get(u, 1,
                                       &SOC_LPM128_STAT_MAX_V4_COUNT(u)));
    SOC_IF_ERROR_RETURN(_soc_lpm_max_64bv6_route_get(u, 1,
                                      &SOC_LPM128_STAT_MAX_64BV6_COUNT(u)));
    SOC_IF_ERROR_RETURN(_soc_lpm_max_128bv6_route_get(u,
                                     &SOC_LPM128_STAT_MAX_128BV6_COUNT(u)));

    return SOC_E_NONE;
}

/*
 * Initialize the start/end tracking pointers for each prefix length
 */
int
soc_fb_lpm_init(int u)
{
    int max_pfx_len;
    int defip_table_size = 0;
    int pfx_state_size;
    int i;    
#if defined(BCM_KATANA_SUPPORT)
    uint32 l3_defip_tbl_size;
    int ipv6_lpm_128b_enable;
#endif
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int start_index = 0;
    int hash_table_size = 0;
    soc_field_t defip_mode0_field = MODE0f;
    soc_field_t defip_mode1_field = MODE1f;
    soc_field_t defip_mode_mask0_field = KEY_MODE_MASK0f;
    soc_field_t defip_mode_mask1_field = KEY_MODE_MASK1f;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(u)) {
        defip_mode0_field = defip_mode0_bit[u] = KEY_MODE0f;
        defip_mode1_field = defip_mode1_bit[u] = KEY_MODE1f;
        defip_mode_mask0_field = defip_mode_mask0_bit[u] = KEY_MODE_MASK0f;
        defip_mode_mask1_field = defip_mode_mask1_bit[u] = KEY_MODE_MASK1f;
    } else 
#endif
    {
        defip_mode0_field = defip_mode0_bit[u] = MODE0f;
        defip_mode1_field = defip_mode1_bit[u] = MODE1f;
        defip_mode_mask0_field = defip_mode_mask0_bit[u] = MODE_MASK0f;
        defip_mode_mask1_field = defip_mode_mask1_bit[u] = MODE_MASK1f;
    }
    if (! soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    if (!SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    max_pfx_len = MAX_PFX_ENTRIES(u);
    pfx_state_size = sizeof(soc_lpm_state_t) * (max_pfx_len);

    if (! SOC_LPM_INIT_CHECK(u)) {
        soc_lpm_field_cache_state[u] = 
                sal_alloc(sizeof(soc_lpm_field_cache_t), "lpm_field_state");
        if (NULL == soc_lpm_field_cache_state[u]) {
            return (SOC_E_MEMORY);
        }
        sal_memset(soc_lpm_field_cache_state[u], 0 , sizeof(soc_lpm_field_cache_t));
        if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, CLASS_ID0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, CLASS_ID1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DST_DISCARD0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DST_DISCARD1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP_COUNT0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP_COUNT1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP_PTR0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ECMP_PTR1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, GLOBAL_ROUTE0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, GLOBAL_ROUTE1f);

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(u, L3_DEFIPm, DESTINATION0f)) {
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DESTINATION0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DESTINATION1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DATA_TYPE0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DATA_TYPE1f);
        }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(u, soc_feature_ipmc_defip)) {
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, MULTICAST_ROUTE0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, MULTICAST_ROUTE1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, EXPECTED_L3_IIF0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, EXPECTED_L3_IIF1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, RPA_ID0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, RPA_ID1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, L3MC_INDEX0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u], L3_DEFIPm, L3MC_INDEX1f);
            }
#endif
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, HIT0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, HIT1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, IP_ADDR0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, IP_ADDR1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, IP_ADDR_MASK0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, IP_ADDR_MASK1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, defip_mode0_field);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, defip_mode1_field);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, defip_mode_mask0_field);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, defip_mode_mask1_field);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, NEXT_HOP_INDEX0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, NEXT_HOP_INDEX1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, PRI0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, PRI1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, RPE0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, RPE1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VALID0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VALID1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VRF_ID_0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VRF_ID_1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VRF_ID_MASK0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, VRF_ID_MASK1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ENTRY_TYPE0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ENTRY_TYPE1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ENTRY_TYPE_MASK0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, ENTRY_TYPE_MASK1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, D_ID0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, D_ID1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, D_ID_MASK0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, D_ID_MASK1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DEFAULTROUTE0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, DEFAULTROUTE1f);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(u)) {
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, KEY_TYPE0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, KEY_TYPE1f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, KEY_TYPE_MASK0f);
                SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIPm, KEY_TYPE_MASK1f);
            }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
            if (soc_feature(u, soc_feature_advanced_flex_counter)) {
                if (SOC_MEM_FIELD_VALID(u, L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f)) {
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_OFFSET_MODE0f);
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_OFFSET_MODE1f);
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_POOL_NUMBER0f);
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_POOL_NUMBER1f);
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX0f);
                    SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u],
                                               L3_DEFIPm, FLEX_CTR_BASE_COUNTER_IDX1f);
                }
            }
#endif
        }
#if defined(BCM_TOMAHAWK3_SUPPORT)
        else if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, HIT0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, HIT1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, VALID0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, VALID1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, ASSOC_DATA0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, ASSOC_DATA1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, FIXED_DATA0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, FIXED_DATA1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, ALPM1_DATA0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, ALPM1_DATA1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, KEY0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, KEY1f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, MASK0f);
            SOC_LPM_CACHE_FIELD_ASSIGN(u, soc_lpm_field_cache_state[u] ,L3_DEFIP_LEVEL1m, MASK1f);
        }
#endif
        SOC_LPM_STATE(u) = sal_alloc(pfx_state_size, "LPM prefix info");
        if (NULL == SOC_LPM_STATE(u)) {
            sal_free(soc_lpm_field_cache_state[u]);
            soc_lpm_field_cache_state[u] = NULL;
            return (SOC_E_MEMORY);
        }
#ifdef FB_LPM_DEBUG
        DBG_SOC_LPM_STATE(u) = sal_alloc(pfx_state_size, "DBG LPM prefix info");
        if (NULL == DBG_SOC_LPM_STATE(u)) {
            soc_fb_lpm_deinit(u);
            return (SOC_E_MEMORY);
        }
        DBG_SOC_LPM_STATE2(u) = sal_alloc(pfx_state_size, "DBG LPM prefix info 2");
        if (NULL == DBG_SOC_LPM_STATE2(u)) {
            soc_fb_lpm_deinit(u);
            return (SOC_E_MEMORY);
        }
        DBG_SOC_LPM_HASH_TMP(u) = sal_alloc(2 * LPM_TBL_SIZE * sizeof(int), "hash");
        if (NULL == DBG_SOC_LPM_HASH_TMP(u)) {
            soc_fb_lpm_deinit(u);
            return (SOC_E_MEMORY);
        }
#endif
    }

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN (soc_mem_cache_set(u, mem, MEM_BLOCK_ALL, TRUE));
#endif /* FB_LPM_TABLE_CACHED */
    SOC_LPM_LOCK(u);

    sal_memset(SOC_LPM_STATE(u), 0, pfx_state_size);
    for(i = 0; i < max_pfx_len; i++) {
        SOC_LPM_STATE_START(u, i) = -1;
        SOC_LPM_STATE_END(u, i) = -1;
        SOC_LPM_STATE_PREV(u, i) = -1;
        SOC_LPM_STATE_NEXT(u, i) = -1;
        SOC_LPM_STATE_VENT(u, i) = 0;
        SOC_LPM_STATE_FENT(u, i) = 0;
    }
#ifdef FB_LPM_DEBUG
    sal_memset(DBG_SOC_LPM_STATE2(u), 0, pfx_state_size);
    for(i = 0; i < max_pfx_len; i++) {
        SOC_LPM_STATE_S(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_E(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_P(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_N(DBG_SOC_LPM_STATE2(u), i) = -1;
        SOC_LPM_STATE_V(DBG_SOC_LPM_STATE2(u), i) = 0;
        SOC_LPM_STATE_F(DBG_SOC_LPM_STATE2(u), i) = 0;
    }
#endif

    defip_table_size = soc_mem_index_count(u, mem);
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(u)) {
          fb_lpm_hash_index_mask[u] = 0x7FFF;
    } else {
          fb_lpm_hash_index_mask[u] = 0x3FFF;
    }
#else
    fb_lpm_hash_index_mask[u] = 0x3FFF;
#endif

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(u, soc_feature_l3_defip_v4_64k)) {
        
        fb_lpm_hash_index_mask[u] = 0xFFFF;
    } else if (SOC_IS_TRIDENT3X(u)) {
        fb_lpm_hash_index_mask[u] = 0x7FFF;
    }
#endif

    if (SOC_IS_HELIX4(u)) {
        fb_lpm_hash_index_mask[u] = 0xFFFF;
    } else if (SOC_IS_TOMAHAWK2(u)) {
        fb_lpm_hash_index_mask[u] = 0x7FFF;
    }

    if (SOC_URPF_STATUS_GET(u)) {
        if (soc_feature(u, soc_feature_l3_defip_hole)) {
              defip_table_size = SOC_APOLLO_B0_L3_DEFIP_URPF_SIZE;
        } else if (SOC_IS_APOLLO(u)) {
            defip_table_size = SOC_APOLLO_L3_DEFIP_URPF_SIZE;
        } else {
            /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
             * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
             * L3_DEFIP table is not divided into 2 to support URPF.
             */
            if (!soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
                defip_table_size >>= 1;
            }
        }
    }

    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        SOC_IF_ERROR_RETURN(soc_fb_lpm_tcam_pair_count_get(u, &tcam_pair_count));
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF.
         */
        if (SOC_URPF_STATUS_GET(u) &&
            !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
#ifdef BCM_UTT_SUPPORT
            if (soc_feature(u, soc_feature_utt)) {
                tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(u);
                start_index = (tcam_pair_count * tcam_depth * 2);
                defip_table_size -= start_index;
            } else
#endif
            {
                switch(tcam_pair_count) {
                    case 1:
                    case 2:
                        defip_table_size -= (2 * tcam_depth);
                        start_index = 2 * tcam_depth;
                        break;
                    case 3:
                    case 4:
                        defip_table_size -= (4 * tcam_depth);
                        start_index = 4 * tcam_depth;
                        break;
                    case 5:
                    case 6:
                        defip_table_size -= (6 * tcam_depth);
                        start_index = 6 * tcam_depth;
                        break;
                    case 7:
                    case 8:
                        defip_table_size -= (8 * tcam_depth);
                        start_index = 8 * tcam_depth;
                        break;
                    default:
                        break;
                }
            }
        } else {
            defip_table_size -= (tcam_pair_count * tcam_depth * 2);
            start_index = (2 * tcam_pair_count * tcam_depth);
        }
    } else {
        start_index = 0;
    }

    SOC_LPM_STATE_FENT(u, (MAX_PFX_INDEX(u))) = defip_table_size;
    SOC_LPM_STATE_START(u, MAX_PFX_INDEX(u)) = start_index;
    SOC_LPM_STATE_END(u, (MAX_PFX_INDEX(u))) = start_index - 1;
#ifdef FB_LPM_DEBUG
    SOC_LPM_STATE_F(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = defip_table_size;
    SOC_LPM_STATE_S(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = start_index;
    SOC_LPM_STATE_E(DBG_SOC_LPM_STATE2(u), MAX_PFX_INDEX(u)) = start_index - 1;
#endif
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(u)) {
        l3_defip_tbl_size = soc_mem_index_count(u, L3_DEFIPm);
        
        ipv6_lpm_128b_enable = soc_property_get(u, spn_IPV6_LPM_128B_ENABLE, 0);
    
        /* IPv6 128b and IPv6 64b entries are stored in mutually exclusive
         * entries in L3_DEFIP table. IPv6 128b are stored in L3_DEFIP_PAIR_128
         * table which is an alias of the L3_DEFIP TCAM pairs. While storing the
         * IPv6 64b entriesin L3_DEFIP table it should be taken care not to
         * overlap with the  entry locations reserved for IPv6 128b for a given
         * 128b pairing mode */

        /* De-allocate any existing kt_lpm_ipv6_info structures. */
        if (kt_lpm_ipv6_info[u] != NULL) {
            kt_lpm_ipv6_info[u] = NULL;
            sal_free(kt_lpm_ipv6_info[u]);
        }

        /* Allocate kt_lpm_ipv6_info structure. */
        kt_lpm_ipv6_info[u] = sal_alloc(sizeof(soc_kt_lpm_ipv6_info_t),
                                        "kt_lpm_ipv6_info");
        if (kt_lpm_ipv6_info[u] == NULL) {
            return (SOC_E_MEMORY);
        }

        /* Reset kt_lpm_ipv6_info structure. */
        sal_memset(kt_lpm_ipv6_info[u], 0, sizeof(soc_kt_lpm_ipv6_info_t));

        if (SOC_URPF_STATUS_GET(u)) {
            /* urpf check enabled */
            if (ipv6_lpm_128b_enable == 1) {
                /*
                 * Saber - 56246 has only 2 L3_DEFIP tcams
                 * Both IPV6 and RPF cannot be supported
                 * on this device.
                 */
                if (soc_feature(u, soc_feature_l3_max_2_defip_tcams)) {
                    LOG_ERROR(BSL_LS_SOC_LPM,
                              (BSL_META_U(u,
                               "Cannot enable IPV6 and URPF "
                               "only 2 L3_DEFIP tcams present\n")));
                    return SOC_E_CONFIG;
                } else {
                    /* C6 :
                     * L3_DEFIP_KEY_SEL=101100_011 ( 0x163 )
                     * TCAMs 0-1 paired for DIP IPv6 LPM 128b
                     * TCAMs 2-3 paired for SIP Ipv6 LPM 128b*/
                    WRITE_L3_DEFIP_KEY_SELr(u, 0x163);
                }
                kt_lpm_ipv6_info[u]->ipv6_128b.depth = L3_DEFIP_TCAM_SIZE;
                /* TCAM 4 is used for DIP IPv6 LPM 64b
                 * TCAM 5 is used for SIP IPv6 LPM 64b */
                kt_lpm_ipv6_info[u]->ipv6_64b.dip_start_offset =
                    L3_DEFIP_TCAM_SIZE * 4;
                kt_lpm_ipv6_info[u]->ipv6_64b.depth = L3_DEFIP_TCAM_SIZE;
            } else {
                /*
                 * Saber - 56246 has only 2 L3_DEFIP tcams
                 * on this device.
                 */
                if (soc_feature(u, soc_feature_l3_max_2_defip_tcams)) {
                    WRITE_L3_DEFIP_KEY_SELr(u, 0x10);
                } else {
                    /* C5 :
                     * L3_DEFIP_KEY_SEL=111000_000 ( 0x1c0 )
                     * No TCAM pairing - No IPv6 LPM 128b entries */
                    WRITE_L3_DEFIP_KEY_SELr(u, 0x1c0);
                }
                /* TCAMs 0-2 are shared for DIP IPv4 LPM and IPv6 LPM 64b
                 * TCAMs 3-5 are shared for SIP IPv4 LPM and IPv6 LPM 64b*/
                kt_lpm_ipv6_info[u]->ipv6_64b.depth = l3_defip_tbl_size / 2;
            }
            kt_lpm_ipv6_info[u]->ipv6_128b.sip_start_offset =
                kt_lpm_ipv6_info[u]->ipv6_128b.dip_start_offset +
                kt_lpm_ipv6_info[u]->ipv6_128b.depth;
            kt_lpm_ipv6_info[u]->ipv6_64b.sip_start_offset =
                kt_lpm_ipv6_info[u]->ipv6_64b.dip_start_offset +
                kt_lpm_ipv6_info[u]->ipv6_64b.depth;
        } else {
            /* urpf check disabled */
            if (ipv6_lpm_128b_enable == 1) {
                /* C3:
                 * L3_DEFIP_KEY_SEL = 000000_011
                 * TCAMs 0-3 paired for DIP IPv6 LPM 128b */
                WRITE_L3_DEFIP_KEY_SELr(u, 0x3);
                kt_lpm_ipv6_info[u]->ipv6_128b.depth = L3_DEFIP_TCAM_SIZE * 2;
                /* TCAMs 4-5 is used for DIP IPv6 LPM 64b */
                kt_lpm_ipv6_info[u]->ipv6_64b.dip_start_offset =
                    L3_DEFIP_TCAM_SIZE * 4;
                kt_lpm_ipv6_info[u]->ipv6_64b.depth = L3_DEFIP_TCAM_SIZE * 2;
            } else {
                /* C1:
                 * L3_DEFIP_KEY_SEL = 000000_000
                 * No TCAM pairing - No Ipv6 LPM 128b entries */
                WRITE_L3_DEFIP_KEY_SELr(u, 0x0);
                /* TCAMs 0-6 are used for DIP IPv6 LPM 64b */
                kt_lpm_ipv6_info[u]->ipv6_64b.depth = l3_defip_tbl_size;
            };
        }
        SOC_LPM_STATE_FENT(u, (MAX_PFX_INDEX(u))) =
                kt_lpm_ipv6_info[u]->ipv6_64b.depth;
        SOC_LPM_STATE_END(u, (MAX_PFX_INDEX(u))) =
                kt_lpm_ipv6_info[u]->ipv6_64b.dip_start_offset - 1;

        SOC_IF_ERROR_RETURN(soc_fb_lpm_state_config(u,
                            kt_lpm_ipv6_info[u]->ipv6_64b.depth,
                            kt_lpm_ipv6_info[u]->ipv6_64b.dip_start_offset));
    } else 
#endif /* BCM_KATANA_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(soc_fb_lpm_state_config(u, defip_table_size, 
                                                    start_index));
    }



#if defined(FB_LPM_AVL_SUPPORT)
    if (SOC_LPM_STATE_AVL(u) != NULL) {
        if (shr_avl_destroy(SOC_LPM_STATE_AVL(u)) < 0) {
            SOC_LPM_UNLOCK(u);
            return SOC_E_INTERNAL;
        }
        SOC_LPM_STATE_AVL(u) = NULL;
    }
    if (shr_avl_create(&SOC_LPM_STATE_AVL(u), INT_TO_PTR(u),
                       sizeof(soc_lpm_avl_state_t), defip_table_size * 2) < 0) {
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#endif /* FB_LPM_AVL_SUPPORT */

#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_LPM_STATE_HASH(u) != NULL) {
        if (_soc_fb_lpm_hash_destroy(SOC_LPM_STATE_HASH(u)) < 0) {
            SOC_LPM_UNLOCK(u);
            return SOC_E_INTERNAL;
        }
        SOC_LPM_STATE_HASH(u) = NULL;
    }

    if (SOC_MEM_IS_VALID(u, L3_DEFIPm)) {
        hash_table_size = soc_mem_index_count(u, L3_DEFIPm);
    } else {
        hash_table_size = soc_mem_index_count(u, L3_DEFIP_LEVEL1m);
    }

    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &SOC_LPM_STATE_HASH(u)) < 0) {
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#ifdef FB_LPM_DEBUG
    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &DBG_SOC_LPM_STATE_HASH(u)) < 0) {
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }

    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &DBG_SOC_LPM_STATE_HASH2(u)) < 0) {
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#endif
#endif
    if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
        (soc_feature(u, soc_feature_shared_defip_stat_support))) {
        if (soc_fb_lpm_stat_init(u) < 0) {
       /* COVERITY
        * soc_fb_lpm_deinit is called in error condition.
        * so irrespective of its return value we are going to release lpm lock
        * and return E_INTERNAL. Hence we can ignore the return value
        */
        /* coverity[check_return] */
            soc_fb_lpm_deinit(u);
            SOC_LPM_UNLOCK(u);
            return SOC_E_INTERNAL;
        }
    }
    SOC_LPM_UNLOCK(u);

    return(SOC_E_NONE);
}
/*
 * De-initialize the start/end tracking pointers for each prefix length
 */
int
soc_fb_lpm_deinit(int u)
{
    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    SOC_LPM_LOCK(u);

#if defined(FB_LPM_AVL_SUPPORT)
    if (SOC_LPM_STATE_AVL(u) != NULL) {
        shr_avl_destroy(SOC_LPM_STATE_AVL(u));
        SOC_LPM_STATE_AVL(u) = NULL;
    }
#endif /* FB_LPM_AVL_SUPPORT */

#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_LPM_STATE_HASH(u) != NULL) {
        _soc_fb_lpm_hash_destroy(SOC_LPM_STATE_HASH(u));
        SOC_LPM_STATE_HASH(u) = NULL;
    }
#ifdef FB_LPM_DEBUG
    if (DBG_SOC_LPM_STATE_HASH(u) != NULL) {
        _soc_fb_lpm_hash_destroy(DBG_SOC_LPM_STATE_HASH(u));
        DBG_SOC_LPM_STATE_HASH(u) = NULL;
    }
    if (DBG_SOC_LPM_STATE_HASH2(u) != NULL) {
        _soc_fb_lpm_hash_destroy(DBG_SOC_LPM_STATE_HASH2(u));
        DBG_SOC_LPM_STATE_HASH2(u) = NULL;
    }

    if (DBG_SOC_LPM_STATE(u) != NULL) {
        sal_free(DBG_SOC_LPM_STATE(u));
        DBG_SOC_LPM_STATE(u) = NULL;
    }
    if (DBG_SOC_LPM_STATE2(u) != NULL) {
        sal_free(DBG_SOC_LPM_STATE2(u));
        DBG_SOC_LPM_STATE2(u) = NULL;
    }
    if (DBG_SOC_LPM_HASH_TMP(u) != NULL) {
        sal_free(DBG_SOC_LPM_HASH_TMP(u));
        DBG_SOC_LPM_HASH_TMP(u) = NULL;
    }
#endif
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(u)) {
        /* De-allocate any existing kt_lpm_ipv6_info structures. */
        if (kt_lpm_ipv6_info[u] != NULL) {
            kt_lpm_ipv6_info[u] = NULL;
            sal_free(kt_lpm_ipv6_info[u]);
        }
    }
#endif

    if (SOC_LPM_INIT_CHECK(u)) {
        sal_free(soc_lpm_field_cache_state[u]);
        soc_lpm_field_cache_state[u] = NULL;
        sal_free(SOC_LPM_STATE(u));
        SOC_LPM_STATE(u) = NULL;
    }

    if (SOC_LPM_STAT_INIT_CHECK(u)) {
        sal_free(soc_lpm_stat[u]);
        soc_lpm_stat[u] = NULL;
    }

    SOC_LPM_UNLOCK(u);

    return(SOC_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_fb_lpm_reinit_done(int unit, int ipv6)
{
    int idx;
    int prev_idx = MAX_PFX_INDEX(unit);
    uint32      v0 = 0, v1 = 0;
    int from_ent = -1;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

    int defip_table_size;

    if (SOC_IS_HAWKEYE(unit)) {
        return SOC_E_NONE;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
         mem = L3_DEFIP_LEVEL1m;
    }
#endif

    defip_table_size = soc_mem_index_count(unit, mem);

    if (SOC_URPF_STATUS_GET(unit)) {
        if (soc_feature(unit, soc_feature_l3_defip_hole)) {
              defip_table_size = SOC_APOLLO_B0_L3_DEFIP_URPF_SIZE;
        } else if (SOC_IS_APOLLO(unit)) {
            defip_table_size = SOC_APOLLO_L3_DEFIP_URPF_SIZE;
        } else {
            /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
             * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
             * L3_DEFIP table is not divided into 2 to support URPF.
             */
            if (!soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
                defip_table_size >>= 1;
            }
        }
    } 

    SOC_LPM_STATE_PREV(unit, MAX_PFX_INDEX(unit)) = -1;

    for (idx = MAX_PFX_INDEX(unit); idx >= 0; idx--) {
        if ((idx == MAX_PFX_INDEX(unit)) && (SOC_LPM_STATE_START(unit, idx) <= 0)) {
            continue;
        }
            
        if (-1 == SOC_LPM_STATE_START(unit, idx)) {
            continue;
        }

        if (prev_idx != idx) {
            SOC_LPM_STATE_PREV(unit, idx) = prev_idx;
            SOC_LPM_STATE_NEXT(unit, prev_idx) = idx;
        }
        SOC_LPM_STATE_FENT(unit, prev_idx) =                    \
                          SOC_LPM_STATE_START(unit, idx) -      \
                          SOC_LPM_STATE_END(unit, prev_idx) - 1;
        prev_idx = idx;
        
        if (idx == MAX_PFX_INDEX(unit) || (ipv6 && SOC_LPM_PFX_IS_V4(unit, idx)) ||
           (!ipv6 && SOC_LPM_PFX_IS_V6_64(unit, idx))) {
            continue;
        }

        if (!(soc_feature(unit, soc_feature_l3_shared_defip_table) ||
            (soc_feature(unit, soc_feature_l3_kt2_shared_defip_table)))) {
            continue;
        }

        if (SOC_LPM_PFX_IS_V4(unit, idx)) {
            from_ent = SOC_LPM_STATE_END(unit, idx);
#ifdef FB_LPM_TABLE_CACHED
        SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(unit, L3_DEFIPm,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
            rv = READ_L3_DEFIPm(unit, MEM_BLOCK_ANY, from_ent, e);
            SOC_IF_ERROR_RETURN(rv);
            if (!soc_feature(unit, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                v0 = SOC_MEM_OPT_F32_VALID0_GET(unit, mem, e);
                v1 = SOC_MEM_OPT_F32_VALID1_GET(unit, mem, e);
                SOC_LPM_V4_COUNT(unit) += (SOC_LPM_STATE_VENT(unit, idx) << 1);
                if ((v0 == 0) || (v1 == 0)) {
                    SOC_LPM_V4_COUNT(unit) -= 1;
                }
                if ((v0 && (!(v1))) || (!(v0) && v1)) {
                    SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(unit));
                    SOC_LPM_STATE_HFENT(unit, idx) = 1;
                } 
            } else {
                SOC_LPM_V4_COUNT(unit) += SOC_LPM_STATE_VENT(unit, idx);
            }
        } else {
            SOC_LPM_64BV6_COUNT(unit) += SOC_LPM_STATE_VENT(unit, idx);
        }
    }

    SOC_LPM_STATE_NEXT(unit, prev_idx) = -1;
    SOC_LPM_STATE_FENT(unit, prev_idx) =                       \
                          defip_table_size -                   \
                          SOC_LPM_STATE_END(unit, prev_idx) - 1;

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        SOC_LPM_STATE_FENT(unit, MAX_PFX_INDEX(unit)) =
                kt_lpm_ipv6_info[unit]->ipv6_64b.depth;
    }
#endif

    return (SOC_E_NONE);
}

int
soc_fb_lpm_reinit(int unit, int idx, uint32 *lpm_entry)
{
    int pfx_len;
#ifdef BCM_TOMAHAWK2_SUPPORT
    uint32 lpm_entry_tmp[SOC_MAX_MEM_WORDS];
    sal_memset(&lpm_entry_tmp, 0,sizeof(lpm_entry_tmp));
#endif
    if (SOC_IS_HAWKEYE(unit)) {
        return SOC_E_NONE;
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_half_of_l3_defip_ipv4_capacity) && 
        SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VALID1f) && 
        !SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, defip_mode1_field)) {
        soc_fb_lpm_ip4entry1_to_0(unit, lpm_entry, &lpm_entry_tmp, TRUE);
        SOC_IF_ERROR_RETURN
            (_soc_fb_lpm_prefix_length_get(unit, &lpm_entry_tmp, &pfx_len));
    } else 
#endif
    {
        SOC_IF_ERROR_RETURN
            (_soc_fb_lpm_prefix_length_get(unit, lpm_entry, &pfx_len));
    }
    if (SOC_LPM_STATE_VENT(unit, pfx_len) == 0) {
        SOC_LPM_STATE_START(unit, pfx_len) = idx;
        SOC_LPM_STATE_END(unit, pfx_len) = idx;
    } else {
        SOC_LPM_STATE_END(unit, pfx_len) = idx;
    }

    SOC_LPM_STATE_VENT(unit, pfx_len)++;
    LPM_HASH_INSERT(unit, lpm_entry, idx, NULL, NULL);
    LPM_AVL_INSERT(unit, lpm_entry, idx);

    return (SOC_E_NONE);
}


#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *     soc_fb_lpm_sw_dump
 * Purpose:
 *     Displays FB LPM information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
soc_fb_lpm_sw_dump(int unit)
{
    soc_lpm_state_p  lpm_state;
    int              i;

    LOG_CLI((BSL_META_U(unit,
                        "\n    FB LPM State -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "        Prefix entries : %d\n"), MAX_PFX_ENTRIES(unit)));

    lpm_state = soc_lpm_state[unit];
    if (lpm_state == NULL) {
        return;
    }

    for (i = 0; i < MAX_PFX_INDEX(unit); i++) {
        if (lpm_state[i].vent != 0 || lpm_state[i].fent != 0 || 
            lpm_state[i].prev != -1 || lpm_state[i].next != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "      Prefix %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "        Start : %d\n"), lpm_state[i].start));
            LOG_CLI((BSL_META_U(unit,
                                "        End   : %d\n"), lpm_state[i].end));
            LOG_CLI((BSL_META_U(unit,
                                "        Prev  : %d\n"), lpm_state[i].prev));
            LOG_CLI((BSL_META_U(unit,
                                "        Next  : %d\n"), lpm_state[i].next));
            LOG_CLI((BSL_META_U(unit,
                                "        Valid Entries : %d\n"), lpm_state[i].vent));
            LOG_CLI((BSL_META_U(unit,
                                "        Free  Entries : %d\n"), lpm_state[i].fent));
            if (soc_feature(unit, soc_feature_l3_shared_defip_table) ||
                (soc_feature(unit, soc_feature_l3_kt2_shared_defip_table))) {
                if (SOC_LPM_PFX_IS_V4(unit, i)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "        Type  : IPV4\n")));
                } else if (SOC_LPM_PFX_IS_V6_64(unit, i)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "        Type  : 64B IPV6\n")));
                }
            }
        }
    }
    
    LOG_CLI((BSL_META_U(unit,
                        "      Prefix %d\n"), i));
    LOG_CLI((BSL_META_U(unit,
                        "        Start : %d\n"), lpm_state[i].start));
    LOG_CLI((BSL_META_U(unit,
                        "        End   : %d\n"), lpm_state[i].end));
    LOG_CLI((BSL_META_U(unit,
                        "        Prev  : %d\n"), lpm_state[i].prev));
    LOG_CLI((BSL_META_U(unit,
                        "        Next  : %d\n"), lpm_state[i].next));
    LOG_CLI((BSL_META_U(unit,
                        "        Valid Entries : %d\n"), lpm_state[i].vent));
    LOG_CLI((BSL_META_U(unit,
                        "        Free  Entries : %d\n"), lpm_state[i].fent));
    if (soc_feature(unit, soc_feature_l3_shared_defip_table) ||
        (soc_feature(unit, soc_feature_l3_kt2_shared_defip_table))) {
        LOG_CLI((BSL_META_U(unit,
                            "        Type  : MAX\n")));
    }
    return;
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
 * soc_fb_lpm_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
soc_fb_lpm_insert(int u, void *entry_data)
{
    int         pfx;
    int         index, copy_index;
    int         ipv6;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;
    int         found = 0;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    soc_mem_t   mem = L3_DEFIPm;

    if (SOC_MEM_IS_VALID(u, mem)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    sal_memcpy(e, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u,mem) * 4);

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm_match(u, entry_data, e, &index, &pfx, &ipv6);
    if (rv == SOC_E_NOT_FOUND) {
        rv = _lpm_free_slot_create(u, pfx, ipv6, e, &index);
        if (rv < 0) {
            SOC_LPM_UNLOCK(u);
            return(rv);
        }
    } else {
        found = 1;
    }

    copy_index = index;
    if (rv == SOC_E_NONE) {
        /* Entry already present. Update the entry */
        if (!ipv6) {
            /* IPV4 entry */
            if (index & 1) {
                rv = soc_fb_lpm_ip4entry0_to_1(u, entry_data, e, PRESERVE_HIT);
            } else {
                rv = soc_fb_lpm_ip4entry0_to_0(u, entry_data, e, PRESERVE_HIT);
            }

            if (rv < 0) {
                SOC_LPM_UNLOCK(u);
                return(rv);
            }

            entry_data = (void *)e;
            index >>= 1;
        }
        soc_fb_lpm_state_dump(u);
        LOG_INFO(BSL_LS_SOC_LPM,
                 (BSL_META_U(u,
                             "\nsoc_fb_lpm_insert: %d %d\n"),
                             index, pfx));
        if (!found) {
            LPM_HASH_INSERT(u, entry_data, index, &rvt_index0, &rvt_index1);
            if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
                (soc_feature(u, soc_feature_shared_defip_stat_support))) {
                if (ipv6) {
                    SOC_LPM_COUNT_INC(SOC_LPM_64BV6_COUNT(u));
                } else {
                    SOC_LPM_COUNT_INC(SOC_LPM_V4_COUNT(u));
                    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                        /* half entry counts should change for new entry only */
                        if (copy_index & 1) {
                            SOC_LPM_COUNT_DEC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
                        } else {
                            SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
                        }
                    }
                }
            }
        }
        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, entry_data);
        if ((SOC_MEM_IS_VALID(u, L3_DEFIPm)) && (rv >= 0)) {
            rv = _lpm_fb_urpf_entry_replicate(u, index, -1, entry_data);
        }
        if (!found && rv < 0) {
            LPM_HASH_REVERT(u, entry_data, index, rvt_index0, rvt_index1);
        }
        LPM_AVL_INSERT(u, entry_data, index);
    }

    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_insert_index
 */
int
soc_fb_lpm_insert_index(int u, void *entry_data, int index)
{
    int         pfx;
    int         ipv6;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    uint32      e_temp[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         rv = SOC_E_NONE;
    int         new_add = 0;
    int         copy_index;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    int         temp_index = -1;
    soc_mem_t   mem = L3_DEFIPm;
    int mode1;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m)) {
        uint32 _key_data[2] = {0, 0};
        mem = L3_DEFIP_LEVEL1m;
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);
        ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY1f, _key_data);
        mode1 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);
        mode1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode1_field);
    }

    if (index == -2) {
        return soc_fb_lpm_insert(u, entry_data);
    }

    SOC_LPM_LOCK(u);
    if (ipv6) {
        if (!mode1) {
            SOC_LPM_UNLOCK(u);
            return (SOC_E_PARAM);
        }
    }
    /* Calculate vrf weighted prefix lengh. */
    _soc_fb_lpm_prefix_length_get(u, entry_data, &pfx); 

    if (index == -1) {
        /*
         * Validate free entries in V4 space. In case of atomic write,
         * one entry is reserved in V4 space (two v4 routes).
         * Insert should not be allowed more than (max_v4_count - 2).
         */
        if (soc_feature(u, soc_feature_lpm_atomic_write)) {
            int max_entries, used_entries;

            used_entries = SOC_LPM_V4_COUNT(u) - SOC_LPM_V4_HALF_ENTRY_COUNT(u);
            used_entries = ((used_entries + 1) >> 1) + SOC_LPM_V4_HALF_ENTRY_COUNT(u);
            used_entries += SOC_LPM_64BV6_COUNT(u);
            used_entries -= SOC_LPM_STATE_HFENT(u, pfx); /* Entry only half used */

            max_entries = SOC_LPM_MAX_64BV6_COUNT(u);
            if (used_entries  >= (max_entries - 1)) {
                SOC_LPM_UNLOCK(u);
                return SOC_E_FULL;
            }
        } 

        sal_memcpy(e, soc_mem_entry_null(u, mem),
                   soc_mem_entry_words(u,mem) * 4);
        rv = _lpm_free_slot_create(u, pfx, ipv6, e, &index);
        if (SOC_FAILURE(rv)) {
            SOC_LPM_UNLOCK(u);
            return rv;
        }
   
        /*
         * In case of IPv4, while inserting second entry (upper part) into the
         * valid TCAM index, for a momentary time, the entry gets disabled,
         * which results in lookup miss for the existing (lower part) valid entry.
         * In order to avoid atomicity issue, Create a new slot and insert the valid
         * entry to the temporary created index.
         */ 
        if (soc_feature(u, soc_feature_lpm_atomic_write) &&
            (ipv6 == 0) && (index & 1)) {
           rv = _lpm_free_slot_create(u, pfx, ipv6, e_temp, &temp_index);
           if (SOC_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_SOC_LPM, (BSL_META_U(u,
                     "Temporary backup entry slot create failure: %d,"
                     "original_index:%d temp_index:%d pfx:%d\n\r"),
                      rv, index, temp_index, pfx));
              goto cleanup; 
           }
           
           /*
            * Increment Half entry created by reserve entry, which gets
            * decremented while deleting the slot in _lpm_free_slot_delete.
           */
           SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));

           /* If temp_index happens to be same as index, then thew new
            * slot creattion resulted in pfx_move_down and the last half entry
            * (which originally is expected to be at 'index' would have moved up
            * in the table by one physical entry level. The 'index' now needs to
            * be corrected for this movement.
 */
           if ((temp_index >> 1) == (index >> 1)) {
               /*
                * (index >> 1) gives physical index; and we decr this by 1
                * and then convert it back to 'slot' range (by multiplying with 2).
                * Lastly offset with 1 because we are dealing with the upper half.
 */
               index = (((index >> 1) - 1) << 1) + (index & 1);
           }
           /* Copy index to temp_index */
           rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, (index >> 1), e_temp);
           if (SOC_FAILURE(rv)) {
              goto cleanup; 
           }
           rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, (temp_index >> 1), e_temp);
           if (SOC_FAILURE(rv)) {
              goto cleanup; 
           }
        }
        new_add = 1;
    } else {
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                         ipv6 ? index : (index >> 1), e);
    }

    copy_index = index;
    if (rv == SOC_E_NONE) {
        /* Entry already present. Update the entry */
        if (!ipv6) {
            /* IPV4 entry */
            if (index & 1) {
                rv = soc_fb_lpm_ip4entry0_to_1(u, entry_data, e, PRESERVE_HIT);
            } else {
                rv = soc_fb_lpm_ip4entry0_to_0(u, entry_data, e, PRESERVE_HIT);
            }

            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            entry_data = (void *)e;
            index >>= 1;
        }
        soc_fb_lpm_state_dump(u);
        LOG_INFO(BSL_LS_SOC_LPM,
                 (BSL_META_U(u,
                             "\nsoc_fb_lpm_insert_index: %d %d\n"),
                             index, pfx));
        if (new_add) {
            LPM_HASH_INSERT(u, entry_data, index, &rvt_index0, &rvt_index1);
            if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
                (soc_feature(u, soc_feature_shared_defip_stat_support))) {
                if (ipv6) {
                    SOC_LPM_COUNT_INC(SOC_LPM_64BV6_COUNT(u));
                } else {
                    SOC_LPM_COUNT_INC(SOC_LPM_V4_COUNT(u));
                    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                        if (copy_index & 1) {
                            SOC_LPM_COUNT_DEC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
                        } else {
                            SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT(u));
                        } 
                    }
                }
            }
        }
        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, entry_data);
        if ((!SOC_IS_TOMAHAWK3(u)) && SOC_SUCCESS(rv)) {
            rv = _lpm_fb_urpf_entry_replicate(u, index,
                     ((temp_index == -1) ? (-1):(temp_index >> 1)) , entry_data);
        }
        if (new_add && SOC_FAILURE(rv)) {
            LPM_HASH_REVERT(u, entry_data, index, rvt_index0, rvt_index1);
        }
        LPM_AVL_INSERT(u, entry_data, index);

cleanup:
        /*
         * Delete the temporary slot created to maintain atomicity.
         */
        if (temp_index != -1) {
           rv = _lpm_free_slot_delete(u, pfx, ipv6, e_temp, temp_index);
           if (SOC_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_SOC_LPM, (BSL_META_U(u,
                     "Temporary backup entry slot delete failure: %d,"
                     "original_index:%d temp_index:%d pfx:%d\n\r"),
                      rv, (index << 1), temp_index, pfx));
           }
        } 
    }

    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_delete
 */
int
soc_fb_lpm_delete(int u, void *key_data)
{
    int         pfx;
    int         index;
    int         ipv6;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm_match(u, key_data, e, &index, &pfx, &ipv6);
    if (rv == SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_LPM,
                 (BSL_META_U(u,
                             "\nsoc_fb_lpm_delete: %d %d\n"),
                             index, pfx));
        LPM_HASH_DELETE(u, key_data, index);
        rv = _lpm_free_slot_delete(u, pfx, ipv6, e, index);
        LPM_AVL_DELETE(u, key_data, index);
        if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
            (soc_feature(u, soc_feature_shared_defip_stat_support))) {
            if (ipv6) {
                SOC_LPM_COUNT_DEC(SOC_LPM_64BV6_COUNT(u));
            } else {
                SOC_LPM_COUNT_DEC(SOC_LPM_V4_COUNT(u));
            }
        }
    }
    soc_fb_lpm_state_dump(u);
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_delete_index
 */
int
soc_fb_lpm_delete_index(int u, void *key_data, int index)
{
    int         pfx;
    int         ipv6;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;

    if (index == -1) {
        return soc_fb_lpm_delete(u, key_data);
    } 

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f, _key_data);
        ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY1f, _key_data);
    } else
#endif
    {
        ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);
    }

    SOC_LPM_LOCK(u);
    /* Calculate vrf weighted prefix lengh. */
    _soc_fb_lpm_prefix_length_get(u, key_data, &pfx); 
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                     ipv6 ? index : (index >> 1), e);

    if (rv == SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_LPM,
                 (BSL_META_U(u,
                             "\nsoc_fb_lpm_delete_index: %d %d\n"),
                             index, pfx));
        LPM_HASH_DELETE(u, key_data, index);
        rv = _lpm_free_slot_delete(u, pfx, ipv6, e, index);
        LPM_AVL_DELETE(u, key_data, index);
        if ((soc_feature(u, soc_feature_l3_shared_defip_table)) ||
            (soc_feature(u, soc_feature_shared_defip_stat_support))) {
            if (ipv6) {
                SOC_LPM_COUNT_DEC(SOC_LPM_64BV6_COUNT(u));
            } else {
                SOC_LPM_COUNT_DEC(SOC_LPM_V4_COUNT(u));
            }
        }
    }
    soc_fb_lpm_state_dump(u);
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_match (Exact match for the key. Will match both IP
 *                   address and mask)
 */
int
soc_fb_lpm_match(int u,
               void *key_data,
               void *e,         /* return entry data if found */
               int *index_ptr)  /* return key location */
{
    int        pfx;
    int        rv;
    int        ipv6;

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm_match(u, key_data, e, index_ptr, &pfx, &ipv6);
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_lookup (LPM search). Masked match.
 */
int
soc_fb_lpm_lookup(int u,
               void *key_data,
               void *entry_data,
               int *index_ptr)
{
    return(SOC_E_UNAVAIL);
}

/*
 * Function:
 *	soc_fb_lpm_ipv4_delete_index
 * Purpose:
 *	Delete an entry from a LPM table based on index,
 * Parameters:
 *	u - StrataSwitch unit #
 *	Index for IPv4 entry is 0 .. 2 * max_index + 1. 
 *      If entry appears at offset N and entry 0 of the table
 *      then index = 2 * N + 0.
 *      If entry appears at offset N and entry 1 of the table
 *      then index = 2 * N + 1.
 *                            =========================================
 *     Offset N - 1           =  IPV4 Entry 1    =  IPV4 Entry 0      =
 *                            =========================================
 *     Offset N               =  IPV4 Entry 1    =  IPV4 Entry 0      =
 *                            =========================================
 *     Offset N + 1           =  IPV4 Entry 1    =  IPV4 Entry 0      =
 *                            =========================================
 */
int
soc_fb_lpm_ipv4_delete_index(int u, int index)
{
    int         pfx;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;

    SOC_LPM_LOCK(u);
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, index / 2, e); 
    if (rv == SOC_E_NONE) {
        if ((soc_mem_field32_get(u, L3_DEFIPm, e,
                                (index % 2) ? VALID1f : VALID0f)) &&
            (soc_mem_field32_get(u, L3_DEFIPm, e,
                                (index % 2) ? defip_mode1_bit[u] : defip_mode0_bit[u]) == 0)) {
            uint32      ipv4a;
            ipv4a = soc_mem_field32_get(u, L3_DEFIPm, e,
                                        (index % 2) ?
                                        IP_ADDR_MASK1f : IP_ADDR_MASK0f);
            rv = _ipmask2pfx(ipv4a, &pfx);
        } else {
            rv = SOC_E_PARAM;
        }
        if (rv == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_LPM,
                     (BSL_META_U(u,
                                 "\nsoc_fb_lpm_ipv4_delete_index: %d %d\n"),
                      index, pfx));
            LPM_HASH_DELETE(u, e, index);
            rv = _lpm_free_slot_delete(u, pfx, FALSE, e, index);
            LPM_AVL_DELETE(u, e, index);
        }
        soc_fb_lpm_state_dump(u);
    }
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * Function:
 *	soc_fb_lpm_ipv6_delete_index
 * Purpose:
 *	Delete an entry from a LPM table based on index,
 * Parameters:
 *	u - StrataSwitch unit #
 *	Index for IPv6 entry is 0 .. max_index. 
 *                            =========================================
 *     Offset N - 1           =  IPV6 Entry                           =
 *                            =========================================
 *     Offset N               =  IPV6 Entry                           =
 *                            =========================================
 *     Offset N + 1           =  IPV6 Entry                           =
 *                            =========================================
 */
int
soc_fb_lpm_ipv6_delete_index(int u, int index)
{
    int         pfx;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;

    SOC_LPM_LOCK(u);
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, index , e); 
    if (rv == SOC_E_NONE) {
        if ((SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, e)) &&
            (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, defip_mode0_field)) &&
            (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, defip_mode1_field)) &&
            (SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, e))) {
            uint32      ipv4a;
 
            ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK0f);
            if ((rv = _ipmask2pfx(ipv4a, &pfx)) == SOC_E_NONE) {
                ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK1f);
                if (pfx) {
                    if (ipv4a != 0xffffffff)  {
                        rv =SOC_E_PARAM;
                    }
                    pfx += 32;
                } else {
                    rv = _ipmask2pfx(ipv4a, &pfx);
                }
            }
        } else {
            rv = SOC_E_PARAM;
        }
        if (rv == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_LPM,
                     (BSL_META_U(u,
                                 "\nsoc_fb_lpm_ipv4_delete_index: %d %d\n"),
                      index, pfx));
            LPM_HASH_DELETE(u, e, index);
            rv = _lpm_free_slot_delete(u, pfx, TRUE,e, index);
            LPM_AVL_DELETE(u, e, index);
        }
        soc_fb_lpm_state_dump(u);
    }
    SOC_LPM_UNLOCK(u);
    return(rv);
}

int soc_fb_get_largest_prefix(int u, int ipv6, void *e, 
                              int *index, int *pfx_len, int* count)
{
    int curr_pfx = MAX_PFX_INDEX(u);
    int next_pfx = SOC_LPM_STATE_NEXT(u, curr_pfx);
    int rv = SOC_E_NOT_FOUND;
    uint32 v0, v1;
    soc_mem_t mem = L3_DEFIPm;

    while (next_pfx != -1) {
        if (!ipv6 && SOC_LPM_PFX_IS_V4(u, next_pfx)) {
            break; 
        } else if(ipv6 && SOC_LPM_PFX_IS_V6_64(u, next_pfx)) {
           break;
        } 
        next_pfx = SOC_LPM_STATE_NEXT(u, next_pfx);  
    }

    if (next_pfx == -1) {
        return rv;
    }

    *index = SOC_LPM_STATE_END(u, next_pfx);
    *pfx_len = next_pfx;

#ifdef FB_LPM_TABLE_CACHED
    if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                       MEM_BLOCK_ANY, *index)) < 0) {
        return rv;
    }
#endif /* FB_LPM_TABLE_CACHED */
    if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, *index, e)) < 0) {
        return rv;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

   
    *count = (v0 == 1) ? 1 : 0;
    if (!ipv6 && v1) {
       *count = 2;    
    }
    return rv;
}


                        /* LPM 128 code starts here */

#define LPM128_HASH_INSERT(u, entry_data, entry_data_upr, tab_index, rvt_index0, rvt_index1)  \
    soc_fb_lpm128_hash_insert(u, entry_data, entry_data_upr, tab_index, LPM_NO_MATCH_INDEX, 0,\
                              rvt_index0, rvt_index1)

#define LPM128_HASH_REVERT(u, entry_data, entry_data_upr, tab_index, rvt_index0, rvt_index1)  \
    soc_fb_lpm128_hash_revert(u, entry_data, entry_data_upr, tab_index, rvt_index0, rvt_index1) 

#define LPM128_HASH_DELETE(u, key_data, key_data_upr, tab_index)         \
    soc_fb_lpm128_hash_delete(u, key_data, key_data_upr, tab_index)

#define LPM128_HASH_LOOKUP(u, key_data, key_data_upr, pfx, tab_index)    \
    soc_fb_lpm128_hash_lookup(u, key_data, key_data_upr, pfx, tab_index)



STATIC
void _soc_fb_lpm128_hash_entry_get(int u, void *e, void *eupr,
                                   int index, _soc_fb_lpm128_hash_entry_t r_entry,
                                   int *v)
{
    soc_mem_t mem = L3_DEFIPm;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    if (index & FB_LPM_HASH_IPV6_MASK) {
        SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, e, eupr, r_entry);
        if (v) {
            *v = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e) &&
                 SOC_MEM_OPT_F32_VALID1_GET(u, mem, e) &&
                 SOC_MEM_OPT_F32_VALID0_GET(u, mem, eupr) &&
                 SOC_MEM_OPT_F32_VALID1_GET(u, mem, eupr);
        }
    } else {
        if (index & 0x1) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, e, r_entry);
            if (v) {
                *v = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
            }
        } else {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, e, r_entry);
            if (v) {
                *v = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
            }
        }
    }
}

/*
 * Function:
 *      _soc_fb_lpm128_hash_insert
 * Purpose:
 *      Insert/Update a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to lookup
 *      pfx     - Prefix length for lookup acceleration.
 *      old_index - Index where the key was moved from.
 *                  FB_LPM_HASH_INDEX_NULL if new entry.
 *      new_index - Index where the key was moved to.
 * Returns:
 *      SOC_E_NONE      Key found
 */
/*
 *      Should be caled before updating the LPM table so that the
 *      data in the hash table is consistent with the LPM table
 */
static
int _soc_fb_lpm128_hash_insert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm128_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t entry,
                            int    pfx,
                            uint32 old_index,
                            uint32 new_index,
                            uint32 *rvt_index)
{
    int u = hash->unit, v, i = 0;
    uint16 hash_val;
    uint32 index;
    uint32 prev_index;
    int offset = 0;
    int rv = SOC_E_NONE;

    if (rvt_index) {
        *rvt_index = FB_LPM_HASH_INDEX_NULL;
    }
    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
               (32 * _SOC_LPM128_HASH_KEY_LEN_IN_WORDS)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("ihash", entry[0], hash_val);
    H_INDEX_MATCH("ins  ", entry[0], new_index);
    H_INDEX_MATCH("ins1 ", index, new_index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    if (old_index != FB_LPM_HASH_INDEX_NULL) {
        while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
            uint32  e[SOC_MAX_MEM_FIELD_WORDS];
            uint32  eupr[SOC_MAX_MEM_FIELD_WORDS];
            _soc_fb_lpm128_hash_entry_t  r_entry = {0};
            int     rindex;

            rindex = (index & FB_LPM_HASH_INDEX_MASK) >> 1;

            /*
             * Check prefix length and skip index if not valid for given length
            if ((SOC_LPM_STATE_START(u, pfx) <= rindex) &&
                (SOC_LPM_STATE_END(u, pfx) >= rindex)) {
             */
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, rindex, e);
            SOC_IF_ERROR_RETURN(rv);
            if (index & FB_LPM_HASH_IPV6_MASK) {
                offset = soc_fb_lpm_paired_offset(u, rindex);
                rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, rindex + offset, eupr);
                SOC_IF_ERROR_RETURN(rv);
                SOC_FB_LPM128_HASH_ENTRY_GET(u, e, eupr, index, r_entry, &v);
            } else {
                SOC_FB_LPM128_HASH_ENTRY_GET(u, e, NULL, index, r_entry, &v);
            }
            if (v == 0) {
                if ((index & FB_LPM_HASH_IPV6_MASK) ||
                    !((new_index & 1) && (new_index == (index + 1)))) {
                    LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                    "Empty entry[%d] in link_table. hash_val %d ridx %d nidx %d\n"),
                                    index, hash_val, rindex, new_index));
                }
            }
            if (v && ((*key_cmp_fn)(entry, r_entry) == 0)) {
                /* assert(old_index == index);*/
                if (new_index != index) {
                    H_INDEX_MATCH("imove", prev_index, new_index);
                    if (prev_index == FB_LPM_HASH_INDEX_NULL) {
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
            index = hash->link_table[index & FB_LPM_HASH_INDEX_MASK];
            H_INDEX_MATCH("ins2 ", index, new_index);
        }
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
        return SOC_E_INTERNAL;
    }
    INDEX_ADD(hash, hash_val, new_index);  /* new entry */
    return(SOC_E_NONE);
}

/*
 * Function:
 *      _soc_fb_lpm128_hash_lookup
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
int _soc_fb_lpm128_hash_lookup(_soc_fb_lpm_hash_t          *hash,
                            _soc_fb_lpm128_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t    entry,
                            int                         pfx,
                            uint32                      *key_index)
{
    int u = hash->unit, v, i = 0;

    uint16 hash_val;
    uint32 index;
    int offset = 0;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
               (32 * _SOC_LPM128_HASH_KEY_LEN_IN_WORDS)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("lhash", entry[0], hash_val);
    H_INDEX_MATCH("lkup ", entry[0], index);
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        uint32  e[SOC_MAX_MEM_FIELD_WORDS];
        uint32  eupr[SOC_MAX_MEM_FIELD_WORDS];
        _soc_fb_lpm128_hash_entry_t  r_entry = {0};
        int     rindex;
        int rv = SOC_E_NONE;

        rindex = (index & FB_LPM_HASH_INDEX_MASK) >> 1;
        /*
         * Check prefix length and skip index if not valid for given length
        if ((SOC_LPM_STATE_START(u, pfx) <= rindex) &&
            (SOC_LPM_STATE_END(u, pfx) >= rindex)) {
         */
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, rindex, e);
        SOC_IF_ERROR_RETURN(rv);
        if (index & FB_LPM_HASH_IPV6_MASK) {
            offset = soc_fb_lpm_paired_offset(u, rindex);
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, \
                                               rindex + offset, eupr);
            SOC_IF_ERROR_RETURN(rv);
            SOC_FB_LPM128_HASH_ENTRY_GET(u, e, eupr, index, r_entry, &v);
        } else {
            SOC_FB_LPM128_HASH_ENTRY_GET(u, e, NULL, index, r_entry, &v);
        }
        if (v == 0) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                "Empty entry[%d] in link_table. hash_val %d ridx %d\n"),
                                index, hash_val, rindex));
        }
        if ((*key_cmp_fn)(entry, r_entry) == 0) {
            *key_index = (index & FB_LPM_HASH_INDEX_MASK) >>
                            ((index & FB_LPM_HASH_IPV6_MASK) ? 1 : 0);
            H_INDEX_MATCH("found", entry[0], index);
            return(SOC_E_NONE);
        }
        /*
        }
        */
        index = hash->link_table[index & FB_LPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("lkup1", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    H_INDEX_MATCH("not_found", entry[0], index);
    return(SOC_E_NOT_FOUND);
}

/*
 * Function:
 *      _soc_fb_lpm128_hash_revert
 * Purpose:
 *      Revert a key index in the hash table
 * Parameters:
 *      hash - Pointer (handle) to Hash Table
 *      key_cmp_fn - Compare function which should compare key
 *      entry   - The key to delete
 *      pfx     - Prefix length for lookup acceleration.
 *      new_index - Index to delete.
 * Returns:
 *      SOC_E_NONE      Success
 *      SOC_E_NOT_FOUND Key index not found.
 */
static
int _soc_fb_lpm128_hash_revert(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm128_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t entry,
                            uint32 new_index,
                            uint32 rvt_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint32 index;
    uint32 prev_index;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
               (32 * _SOC_LPM128_HASH_KEY_LEN_IN_WORDS)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("rhash", entry[0], hash_val);
    H_INDEX_MATCH("rvt  ", entry[0], index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (new_index == index) {
            H_INDEX_MATCH("rfoun", entry[0], index);
            if (rvt_index == FB_LPM_HASH_INDEX_NULL) {
                if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                    INDEX_DELETE(hash, hash_val, new_index);
                } else {
                    INDEX_DELETE_LINK(hash, prev_index, new_index);
                }
            } else {
                if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                    INDEX_UPDATE(hash, hash_val, new_index, rvt_index);
                } else {
                    INDEX_UPDATE_LINK(hash, prev_index, new_index, rvt_index);
                }
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & FB_LPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("rvt1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}

/*
 * Function:
 *      _soc_fb_lpm128_hash_delete
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
int _soc_fb_lpm128_hash_delete(_soc_fb_lpm_hash_t *hash,
                            _soc_fb_lpm128_hash_compare_fn key_cmp_fn,
                            _soc_fb_lpm128_hash_entry_t entry,
                            int    pfx,
                            uint32 delete_index)
{
    int u = hash->unit, i = 0;
    uint16 hash_val;
    uint32 index;
    uint32 prev_index;

    hash_val = _soc_fb_lpm_hash_compute((uint8 *)entry,
               (32 * _SOC_LPM128_HASH_KEY_LEN_IN_WORDS)) % hash->index_count;
    index = hash->table[hash_val];
    H_INDEX_MATCH("dhash", entry[0], hash_val);
    H_INDEX_MATCH("del  ", entry[0], index);
    prev_index = FB_LPM_HASH_INDEX_NULL;
    while(index != FB_LPM_HASH_INDEX_NULL && i++ < hash->entry_count) {
        if (delete_index == index) {
            H_INDEX_MATCH("dfoun", entry[0], index);
            if (prev_index == FB_LPM_HASH_INDEX_NULL) {
                INDEX_DELETE(hash, hash_val, delete_index);
            } else {
                INDEX_DELETE_LINK(hash, prev_index, delete_index);
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & FB_LPM_HASH_INDEX_MASK];
        H_INDEX_MATCH("del1 ", entry[0], index);
    }
    if (i > hash->entry_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                            "Hash loop\n")));
    }
    return(SOC_E_NOT_FOUND);
}

void soc_fb_lpm128_hash_insert(int u, void *entry_data, void *entry_data_upr,
                               uint32 tab_index, uint32 old_index, int pfx,
                               uint32 *rvt_index0, uint32 *rvt_index1)
{
    _soc_fb_lpm128_hash_entry_t    key_hash = {0};
    soc_mem_t mem = L3_DEFIPm;
    int mode;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        mem = L3_DEFIP_LEVEL1m;
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);
        mode = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        mode = SOC_MEM_OPT_F32_GET(u, mem, entry_data, defip_mode0_field);
    }

    if (mode && mode != 3) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "Invalid mode while inserting lpm128 hash for pfx - %d\n"), pfx));
        return;
    }
    if (mode) {
        /* IPV6 entry */
        if (entry_data_upr == NULL) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "upper data is NULL for pfx - %d\n"), pfx));
            return;
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data_upr) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data_upr)) {
            SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr,
                                              key_hash);
            (void) _soc_fb_lpm128_hash_insert(
                        SOC_LPM128_STATE_HASH(u),
                        _soc_fb_lpm128_hash_compare_key,
                        key_hash,
                        pfx,
                        old_index,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            (void) _soc_fb_lpm128_hash_insert(SOC_LPM128_STATE_HASH(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            (void)_soc_fb_lpm128_hash_insert(SOC_LPM128_STATE_HASH(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}



void soc_fb_lpm128_hash_revert(int u, void *entry_data, void *entry_data_upr,
                               uint32 tab_index, uint32 rvt_index0, uint32 rvt_index1)
{
    _soc_fb_lpm128_hash_entry_t    key_hash = {0};
    soc_mem_t mem = L3_DEFIPm;
    int mode;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        uint32 _key_data[2] = {0, 0};
        mem = L3_DEFIP_LEVEL1m;
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_data, KEY0f, _key_data);
        mode = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        mode = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);
    }

    if (mode && mode != 3) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "Invalid mode while inserting lpm128 hash\n")));
        return;
    }
    if (mode) {
        /* IPV6 entry */
        if (entry_data_upr == NULL) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "upper data is NULL\n")));
            return;
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data) &&
            SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data_upr) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data_upr)) {
            SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr,
                                              key_hash);
            (void) _soc_fb_lpm128_hash_revert(
                        SOC_LPM128_STATE_HASH(u),
                        _soc_fb_lpm128_hash_compare_key,
                        key_hash,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            (void) _soc_fb_lpm128_hash_revert(SOC_LPM128_STATE_HASH(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            (void)_soc_fb_lpm128_hash_revert(SOC_LPM128_STATE_HASH(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}



void soc_fb_lpm128_hash_delete(int u, void *key_data, void *key_data_upr, 
                               uint32 tab_index)
{
    _soc_fb_lpm128_hash_entry_t    key_hash = {0};
    int                         pfx = -1;
    int                         rv;
    uint32                      index;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 _key_data[2] = {0, 0};
#endif
    int is_ipv6 = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f, _key_data);
        is_ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);
    }

    if (is_ipv6) {
        SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, key_data, key_data_upr, key_hash);
        index = (tab_index << 1) | FB_LPM_HASH_IPV6_MASK;
    } else {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
        index = tab_index;
    }

    rv = _soc_fb_lpm128_hash_delete(SOC_LPM128_STATE_HASH(u),
                                 _soc_fb_lpm128_hash_compare_key,
                                 key_hash, pfx, index);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "\ndel  index: H %d error %d\n"), index, rv));
    }
}

int soc_fb_lpm128_hash_lookup(int u, void *key_data, void *key_data_upr, 
                              int pfx, int *key_index)
{
    _soc_fb_lpm128_hash_entry_t    key_hash = {0};
    int                         is_ipv6;
    int                         rv;
    uint32                      index = FB_LPM_HASH_INDEX_NULL;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 _key_data[2] = {0, 0};
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, key_data, KEY0f, _key_data);
        is_ipv6 = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
    } else
#endif
    {
        is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, defip_mode0_field);
    }
    if (is_ipv6 == 3) {
        SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, key_data, key_data_upr, key_hash);
    } else if(is_ipv6 == 0) {
        SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, key_data, key_hash);
    } else {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "LPM128 hash lookup for pfx - %d failed\n"), pfx));
        return SOC_E_PARAM;
    }

    rv = _soc_fb_lpm128_hash_lookup(SOC_LPM128_STATE_HASH(u),
                                 _soc_fb_lpm128_hash_compare_key,
                                 key_hash, pfx, &index);
    if (SOC_FAILURE(rv)) {
        *key_index = 0xFFFFFFFF;
        return(rv);
    }

    *key_index = index;
    return(SOC_E_NONE);
}


/*
 * _soc_fb_lpm_prefix_length_get (Extract vrf weighted  prefix lenght from the 
 * hw entry based on ip, mask & vrf)
 */
static int
_soc_fb_lpm128_prefix_length_get(int u, void *entry, void* entry_upr, 
                                 int *pfx_len)
{
    int         pfx_len1 = 0;
    int         pfx_len2 = 0;
    int         pfx_len3 = 0;
    int         pfx_len4 = 0;
    int         rv;
    int         mode;
    uint32      ipv4a;
    int         vrf_id;
    int pfx;
    int offset = 0;
    int value;
    soc_mem_t mem = L3_DEFIPm;
    int n = 0, p = 3;
    int mc = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 _key_data[2] = {0, 0};
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, KEY0f, _key_data);
        mode = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf);
        SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, MASK0f, _key_data);
        ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
    } else
#endif
    {
        mode = SOC_MEM_OPT_F32_GET(u, mem, entry, defip_mode0_field);
        ipv4a = SOC_MEM_OPT_F32_GET(u, mem, entry, IP_ADDR_MASK0f);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(u)) {
        mc = (SOC_MEM_OPT_F32_GET(u, mem, entry, DATA_TYPE0f) == 2);
    }
#endif

    if (mode && (mode != 3)) {
       return SOC_E_PARAM;
    }

    if (ipv4a) {
        if ((rv = _ipmask2pfx(ipv4a, &pfx_len1)) < 0) {
            return(rv);
        }
    }
    if (mode) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(u)) {
            _key_data[0] = _key_data[1] = 0;
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry, MASK1f, _key_data);
            ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, mem, entry, IP_ADDR_MASK1f);
        }
        if (ipv4a) {
            if ((rv = _ipmask2pfx(ipv4a, &pfx_len2)) < 0) {
                return(rv);
            }
        }

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(u)) {
            _key_data[0] = _key_data[1] = 0;
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_upr, MASK0f, _key_data);
            ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, mem, entry_upr, IP_ADDR_MASK0f);
        }
        if (ipv4a) {
            if ((rv = _ipmask2pfx(ipv4a, &pfx_len3)) < 0) {
                return(rv);
            }
        }

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(u)) {
            SOC_TH3_MEM_OPT_F64_GET(u, L3_DEFIP_LEVEL1m, entry_upr, MASK1f, _key_data);
            ipv4a = soc_format_field32_get(u, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        } else
#endif
        {
            ipv4a = SOC_MEM_OPT_F32_GET(u, mem, entry_upr, IP_ADDR_MASK1f);
        }
        if (ipv4a) {
            if ((rv = _ipmask2pfx(ipv4a, &pfx_len4)) < 0) {
                return(rv);
            }
        }
    }

    pfx = pfx_len1 + pfx_len2 + pfx_len3 + pfx_len4;

    if (mode == 0) {
        offset = 0;
    } else {
        offset = (pfx > 64) ? (MAX_PFX_64_OFFSET + MAX_PFX_32_OFFSET) :
                 MAX_PFX_32_OFFSET;
    }

    pfx += offset;

    SOC_IF_ERROR_RETURN(soc_fb_lpm_vrf_get(u, entry, &vrf_id));

    switch(mode) {
        case 0:
            value = MAX_PFX_32_OFFSET;
            break; 
        case 3:
            if (offset > MAX_PFX_32_OFFSET) {
                value = MAX_PFX_128_OFFSET;
            } else {
                value = MAX_PFX_64_OFFSET;
            }  
            break;
        /* COVERITY
         * Default switch case
         */
        /* coverity[dead_error_begin] */
        default:
            return SOC_E_INTERNAL;
    }

    switch (vrf_id) { 
      case SOC_L3_VRF_GLOBAL:
          n = 0;
          break;
      case SOC_L3_VRF_OVERRIDE:
          n = 2;
          break;
      default:
          n = 1;
          break;
    }

    if (soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc) {
        n += 3;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    p = 6;
#endif

    *pfx_len = pfx + n * (value / p);

    return (SOC_E_NONE);
}
/*
 * _soc_fb_lpm_match (Exact match for the key. Will match both IP address
 * and mask)
 */

STATIC int
_soc_fb_lpm128_match(int u,
               void *key_data,
               void *key_data_upr,
               void *e,         /* return entry data if found */
               void *eupr,         /* return entry data if found */
               int *index_ptr,  /* return key location */
               int *pfx_len,    /* Key prefix length. vrf + 32 + prefix len for IPV6*/
               soc_lpm_entry_type_t *type_out)
{
    int         rv;
    int         key_index;
    int         pfx = 0;
    int         offset = 0;
    soc_lpm_entry_type_t type;

    /* Calculate vrf weighted prefix lengh. */
    SOC_IF_ERROR_RETURN
    (_soc_fb_lpm128_prefix_length_get(u, key_data, key_data_upr, &pfx));
    *pfx_len = pfx;

     if (SOC_LPM128_PFX_IS_V6_128(u, pfx)) {
         type = socLpmEntryType128BV6;
     } else if (SOC_LPM128_PFX_IS_V6_64(u, pfx)) {
         type = socLpmEntryType64BV6;
     } else {
         type = socLpmEntryTypeV4;
     }

#ifdef FB_LPM_HASH_SUPPORT
    rv = LPM128_HASH_LOOKUP(u, key_data, key_data_upr, pfx, &key_index);
    if (rv == SOC_E_NONE) {
        *index_ptr = key_index;
        if (type == socLpmEntryTypeV4) {
            key_index = key_index >> 1; 
        }

        if (e != NULL) {
           if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, key_index, e)) < 0) {
              return rv;
           }
        }
        if (eupr != NULL) {
           if (type == socLpmEntryType128BV6) {
               offset = soc_fb_lpm_paired_offset(u, key_index);
              if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, 
                                      (key_index + offset),
                      eupr)) < 0) {
                 return rv;
              }
           }
       }

    }
#endif /* FB_LPM_HASH_SUPPORT */
    if (type_out != NULL) {
       *type_out = type;
    }
    return rv;
}

/*
 * Initialize the start/end tracking pointers for each prefix length
 */

int
soc_fb_lpm128_init(int u)
{
    int max_pfx_len;
    int defip_table_size = 0;
    int pfx_state_size;
    int i;
    int num_ipv6_128b_entries;
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int is_reserved = 0;
    int unreserved_start_offset = 0; 
    soc_lpm128_state_p lpm_state_ptr = NULL;
    int hash_table_size = 0;
    soc_mem_t defip_mem = (SOC_MEM_IS_VALID(u, L3_DEFIP_LEVEL1m) ?
                           L3_DEFIP_LEVEL1m : L3_DEFIPm);

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_UNAVAIL;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }


    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(u);

    SOC_IF_ERROR_RETURN(soc_fb_lpm_tcam_pair_count_get(u, &tcam_pair_count));

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (tcam_pair_count && SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(u, soc_feature_utt)) {
            tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(u);
            defip_table_size = (tcam_pair_count * tcam_depth * 2);
        } else
#endif
        {
            switch(tcam_pair_count) {
                case 1:
                case 2:
                    defip_table_size = 2 * tcam_depth;
                    break;
                case 3:
                case 4:
                    defip_table_size = 4 * tcam_depth;
                    break;
                case 5:
                case 6:
                    defip_table_size = 6 * tcam_depth;
                    break;
                case 7:
                case 8:
                    defip_table_size = 8 * tcam_depth;
                    break;

                default:
                    defip_table_size = 0;
                   break;
            }
        }
    } else {
        defip_table_size = (tcam_pair_count * tcam_depth * 2);
    }

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        num_ipv6_128b_entries >>= 1;
    }

    if(SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u)) {
        /* defip table size might have changed */
        sal_free(SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u));
        SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) = NULL;
    }

    SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) =
        sal_alloc(defip_table_size * sizeof(int), 
                  "SOC LPM128 GROUP TO PFX");
    if (NULL == SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u)) {
        return SOC_E_MEMORY;
    }
    
    sal_memset(SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u), -1,
               defip_table_size * sizeof(int));

    if (!SOC_LPM128_STATE_TABLE_INIT_CHECK(u)) {
        SOC_LPM128_STATE_TABLE(u) = sal_alloc(sizeof(soc_lpm128_table_t),
                                              "soc LPM STATE table");
        if (NULL == SOC_LPM128_STATE_TABLE(u)) {
            soc_fb_lpm128_deinit(u);
            return SOC_E_MEMORY;
        }
    }
    sal_memset(SOC_LPM128_STATE_TABLE(u), 0x0, sizeof(soc_lpm128_table_t));

    max_pfx_len = MAX_PFX128_ENTRIES;
    pfx_state_size = sizeof(soc_lpm128_state_t) * (max_pfx_len);

    if (!SOC_LPM128_INIT_CHECK(u)) {
        SOC_LPM128_STATE(u) = sal_alloc(pfx_state_size, "LPM prefix info");
        if (NULL == SOC_LPM128_STATE(u)) {
            soc_fb_lpm128_deinit(u);
            return (SOC_E_MEMORY);
        }
    }

    if (!SOC_LPM128_UNRESERVED_INIT_CHECK(u)) {
        if (is_reserved && ((num_ipv6_128b_entries * 2) != defip_table_size)) {
            SOC_LPM128_UNRESERVED_STATE(u) = sal_alloc(pfx_state_size,
                                                       "LPM prefix info");
            if (NULL == SOC_LPM128_UNRESERVED_STATE(u)) {
                soc_fb_lpm128_deinit(u);
                return (SOC_E_MEMORY);
            }
#ifdef FB_LPM_DEBUG
            DBG_SOC_LPM128_UNRESERVED_STATE(u) = sal_alloc(pfx_state_size,
                                                       "Debug LPM prefix info");
            if (NULL == DBG_SOC_LPM128_UNRESERVED_STATE(u)) {
                return (SOC_E_MEMORY);
            }

            DBG_SOC_LPM128_UNRESERVED_STATE2(u) = sal_alloc(pfx_state_size,
                                                       "Debug LPM prefix info");
            if (NULL == DBG_SOC_LPM128_UNRESERVED_STATE2(u)) {
                return (SOC_E_MEMORY);
            }            
#endif
        } else {
            SOC_LPM128_UNRESERVED_STATE(u) = NULL;
#ifdef FB_LPM_DEBUG
            DBG_SOC_LPM128_UNRESERVED_STATE(u) = NULL;
            DBG_SOC_LPM128_UNRESERVED_STATE2(u) = NULL;
#endif
        }
    } else {
        if (is_reserved && ((num_ipv6_128b_entries * 2) == defip_table_size)) {
           sal_free(SOC_LPM128_UNRESERVED_STATE(u));
           SOC_LPM128_UNRESERVED_STATE(u) = NULL;
#ifdef FB_LPM_DEBUG
           sal_free(DBG_SOC_LPM128_UNRESERVED_STATE(u));
           DBG_SOC_LPM128_UNRESERVED_STATE(u) = NULL;
           sal_free(DBG_SOC_LPM128_UNRESERVED_STATE2(u));
           DBG_SOC_LPM128_UNRESERVED_STATE2(u) = NULL;
#endif
        }
    }
#ifdef FB_LPM_DEBUG

    DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) =
        sal_alloc(defip_table_size * sizeof(int),
                  "Debug SOC LPM128 GROUP TO PFX");
    if (NULL == DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u)) {
        return SOC_E_MEMORY;
    }
    sal_memset(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u), -1,
               defip_table_size * sizeof(int));

    DBG_SOC_LPM128_STATE(u) = sal_alloc(pfx_state_size, "Debug LPM prefix info");
    if (NULL == DBG_SOC_LPM128_STATE(u)) {
        return (SOC_E_MEMORY);
    }


    DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u) =
        sal_alloc(defip_table_size * sizeof(int),
                  "Debug SOC LPM128 GROUP TO PFX");
    if (NULL == DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u)) {
        return SOC_E_MEMORY;
    }
    sal_memset(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u), -1,
               defip_table_size * sizeof(int));

    DBG_SOC_LPM128_STATE2(u) = sal_alloc(pfx_state_size, "Debug LPM prefix info");
    if (NULL == DBG_SOC_LPM128_STATE2(u)) {
        return (SOC_E_MEMORY);
    }


    DBG_SOC_L3_DEFIP(u) = sal_alloc(sizeof(defip_entry_t) *
                                 soc_mem_index_count(u, defip_mem), "Debug L3_DEFIP");
    if (NULL == DBG_SOC_L3_DEFIP(u)) {
        return (SOC_E_MEMORY);
    }

#endif

    SOC_LPM_LOCK(u);

    SOC_LPM128_STATE_TABLE_TOTAL_COUNT(u) = defip_table_size;

    lpm_state_ptr = SOC_LPM128_STATE(u);
    for(i = 0; i < max_pfx_len; i++) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_VENT(u, lpm_state_ptr, i) = 0;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, i) = 0;
        SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, i) = 0;
    }

    if (is_reserved) {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              2 * num_ipv6_128b_entries;
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              defip_table_size;
    }
#ifdef FB_LPM_DEBUG
    lpm_state_ptr = DBG_SOC_LPM128_STATE2(u);
    for(i = 0; i < max_pfx_len; i++) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, i) = -1;
        SOC_LPM128_STATE_VENT(u, lpm_state_ptr, i) = 0;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, i) = 0;
    }
    if (is_reserved) {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              2 * num_ipv6_128b_entries;
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
                              defip_table_size;
    }
#endif
    
    unreserved_start_offset = 0;
    if (is_reserved && ((num_ipv6_128b_entries * 2) != defip_table_size)) {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(u, soc_feature_utt)) {
            SOC_IF_ERROR_RETURN
                (soc_utt_lpm_scaled_unreserved_start_index(u,
                                               &unreserved_start_offset));
        } else
#endif
        {
            unreserved_start_offset =
                ((num_ipv6_128b_entries / tcam_depth) * tcam_depth * 2) +
                (num_ipv6_128b_entries % tcam_depth);
        }
    }

    if (unreserved_start_offset) {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
        for(i = 0; i < max_pfx_len; i++) {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_PREV(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_VENT(u, lpm_state_ptr, i) = 0;
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, i) = 0;
        }
        /* Assign all fent entries to FENT only */
       SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           defip_table_size - (2 * num_ipv6_128b_entries);

       SOC_LPM128_STATE_START1(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           unreserved_start_offset;
       SOC_LPM128_STATE_END1(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           unreserved_start_offset - 1;       
    }
#ifdef FB_LPM_DEBUG
    if (unreserved_start_offset) {
        lpm_state_ptr = DBG_SOC_LPM128_UNRESERVED_STATE2(u);
        for(i = 0; i < max_pfx_len; i++) {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_PREV(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, i) = -1;
            SOC_LPM128_STATE_VENT(u, lpm_state_ptr, i) = 0;
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, i) = 0;
        }
        /* Assign all fent entries to FENT only */
       SOC_LPM128_STATE_FENT(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           defip_table_size - (2 * num_ipv6_128b_entries);

       SOC_LPM128_STATE_START1(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           unreserved_start_offset;
       SOC_LPM128_STATE_END1(u, lpm_state_ptr, (MAX_PFX128_INDEX)) =
           unreserved_start_offset - 1;       
   }
#endif

#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_LPM128_STATE_HASH(u) != NULL) {
        if (_soc_fb_lpm_hash_destroy(SOC_LPM128_STATE_HASH(u)) < 0) {
            SOC_LPM_UNLOCK(u);
            soc_fb_lpm128_deinit(u);
            return SOC_E_INTERNAL;
        }
        SOC_LPM128_STATE_HASH(u) = NULL;
    }

    hash_table_size = soc_mem_index_count(u, defip_mem);
    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &SOC_LPM128_STATE_HASH(u)) < 0) {
        soc_fb_lpm128_deinit(u);
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#ifdef FB_LPM_DEBUG
    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &DBG_SOC_LPM128_STATE_HASH(u)) < 0) {
        soc_fb_lpm128_deinit(u);
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }

    if (_soc_fb_lpm_hash_create(u, hash_table_size * 2, hash_table_size,
                                &DBG_SOC_LPM128_STATE_HASH2(u)) < 0) {
        soc_fb_lpm128_deinit(u);
        SOC_LPM_UNLOCK(u);
        return SOC_E_MEMORY;
    }
#endif

#endif
    if (soc_fb_lpm128_stat_init(u) < 0) {
        soc_fb_lpm128_deinit(u);
        SOC_LPM_UNLOCK(u);
        return SOC_E_INTERNAL;
    }
    SOC_LPM_UNLOCK(u);

    return(SOC_E_NONE);
}

/*
 * De-initialize the start/end tracking pointers for each prefix length
 */
int
soc_fb_lpm128_deinit(int u)
{
    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return(SOC_E_UNAVAIL);
    }

    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_UNAVAIL;
    }

    SOC_LPM_LOCK(u);
    if (SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) != NULL) {
        sal_free(SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u));
        SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) = NULL;
    }

    if (SOC_LPM128_STATE_TABLE_INIT_CHECK(u) && SOC_LPM128_INIT_CHECK(u)) {
        sal_free(SOC_LPM128_STATE(u));
        SOC_LPM128_STATE(u) = NULL;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved) &&
        SOC_LPM128_STATE_TABLE_INIT_CHECK(u) &&
        SOC_LPM128_UNRESERVED_INIT_CHECK(u)) {
        sal_free(SOC_LPM128_UNRESERVED_STATE(u));
        SOC_LPM128_UNRESERVED_STATE(u) = NULL;
    }

    if (SOC_LPM128_STATE_TABLE_INIT_CHECK(u)) {
        sal_free(SOC_LPM128_STATE_TABLE(u));
        SOC_LPM128_STATE_TABLE(u) = NULL;
    }
#ifdef FB_LPM_HASH_SUPPORT
    if (SOC_LPM128_STATE_HASH(u) != NULL) {
        _soc_fb_lpm_hash_destroy(SOC_LPM128_STATE_HASH(u));
        SOC_LPM128_STATE_HASH(u) = NULL;
    }
#endif

#ifdef FB_LPM_DEBUG
    if (DBG_SOC_LPM128_STATE_HASH(u)) {
        _soc_fb_lpm_hash_destroy(DBG_SOC_LPM128_STATE_HASH(u));
        DBG_SOC_LPM128_STATE_HASH(u) = NULL;
    }

    if (DBG_SOC_LPM128_STATE_HASH2(u)) {
        _soc_fb_lpm_hash_destroy(DBG_SOC_LPM128_STATE_HASH2(u));
        DBG_SOC_LPM128_STATE_HASH2(u) = NULL;
    }

    if (DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) != NULL) {
        sal_free(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u));
        DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u) = NULL;
    }
    if (DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u) != NULL) {
        sal_free(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u));
        DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE2(u) = NULL;
    }
        
    if (DBG_SOC_LPM128_STATE(u)) {
        sal_free(DBG_SOC_LPM128_STATE(u));
        DBG_SOC_LPM128_STATE(u) = NULL;
    }

    if (DBG_SOC_LPM128_STATE2(u)) {
        sal_free(DBG_SOC_LPM128_STATE2(u));
        DBG_SOC_LPM128_STATE2(u) = NULL;
    }

    if (DBG_SOC_LPM128_UNRESERVED_STATE(u)) {
        sal_free(DBG_SOC_LPM128_UNRESERVED_STATE(u));
        DBG_SOC_LPM128_UNRESERVED_STATE(u) = NULL;
    }

    if (DBG_SOC_LPM128_UNRESERVED_STATE2(u)) {
        sal_free(DBG_SOC_LPM128_UNRESERVED_STATE2(u));
        DBG_SOC_LPM128_UNRESERVED_STATE2(u) = NULL;
    }

    if (DBG_SOC_L3_DEFIP(u)) {
        sal_free(DBG_SOC_L3_DEFIP(u));
        DBG_SOC_L3_DEFIP(u) = NULL;
    }
#endif

    SOC_LPM_UNLOCK(u);

    return(SOC_E_NONE);
}


/*
 * Expect pfx1 < pfx2,
 */
STATIC INLINE int
_lpm128_pfx_conflicting(int pfx1, int pfx2)
{
    return  (SOC_LPM128_PFX_IS_V4(u, pfx1) && SOC_LPM128_PFX_IS_V6(u, pfx2));
}

STATIC int
_lpm128_fb_urpf_entry_replicate(int u, int index, int temp_index, uint32 *e, uint32 *eupr)
{
    int src_tcam_offset;  /* Defip memory size/2 urpf source lookup offset */
    int new_index = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(u);
    int ipv6;             /* IPv6 entry.                                   */
    uint32 mask0;         /* Mask 0 field value.                           */
    uint32 mask1;         /* Mask 1 field value.                           */
    int def_gw_flag;      /* Entry is default gateway.                     */
    uint32 defip_temp_e[SOC_MAX_MEM_WORDS]; /* Temporary lpm entry. */
    int rv = SOC_E_NONE;
    int offset = 0;
#if defined(BCM_TRIUMPH3_SUPPORT)
    int         pfx;
    uint32      mask2 = 0;         /* Mask 0 field value.                           */
    uint32      mask3 = 0;         /* Mask 1 field value.                           */
    soc_lpm_entry_type_t type = socLpmEntryType128BV6;
#endif

    if(!SOC_URPF_STATUS_GET(u)) {
        return (SOC_E_NONE);
    }
    ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, defip_mode0_field);

    /* Set/Reset default gateway flag based on ip mask value. */
    mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK0f);
    mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_MASK1f);

    if (!ipv6) {
        if (SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, e)) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e,
            DEFAULTROUTE0f, (!mask0) ? 1 : 0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, e)) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e,
                 DEFAULTROUTE1f, (!mask1) ? 1 : 0);
        }
    } else {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(u)) {
            /* Calculate vrf weighted prefix lengh. */
            SOC_IF_ERROR_RETURN
                (_soc_fb_lpm128_prefix_length_get(u, e, eupr, &pfx));
            if (SOC_LPM128_PFX_IS_V6_128(u, pfx)) {
                type = socLpmEntryType128BV6;
            } else if (SOC_LPM128_PFX_IS_V6_64(u, pfx)) {
                type = socLpmEntryType64BV6;
            } else {
                type = socLpmEntryTypeV4;
            }
            if (socLpmEntryType64BV6 == type) {
                /* Set/Reset default gateway flag based on ip mask value. */
                mask2 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, eupr, IP_ADDR_MASK0f);
                mask3 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, eupr, IP_ADDR_MASK1f);
            }
        }
        if (SOC_IS_TRIUMPH3(u) && (socLpmEntryType64BV6 == type)) {
            def_gw_flag = ((!mask0) &&  (!mask1) && (!mask2) &&  (!mask3)) ? 1 : 0;
        } else
#endif
        {
            def_gw_flag = ((!mask0) &&  (!mask1)) ? 1 : 0;
        }
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DEFAULTROUTE0f, def_gw_flag);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DEFAULTROUTE1f, def_gw_flag);
    }

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        return WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, e);
    }
    if (soc_feature(u, soc_feature_defip_2_tcams_with_separate_rpf)) {
        src_tcam_offset = SOC_DEFIP_HOLE_RPF_OFFSET;
    } else {
        src_tcam_offset = (max_tcams * tcam_depth) / 2;
    }

    new_index = index + src_tcam_offset;
    
    /*
     * In order to avoid atomicity issue, write the tcam entry to temp_index
     * before writing into the actual index.
     * "temp_index" is VALID only if 'lpm_atomic_write' feature is enabled.
     */  
    if (temp_index != -1) {
       temp_index += src_tcam_offset;
       rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                          new_index, &defip_temp_e);
       SOC_IF_ERROR_RETURN(rv);
       rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                          temp_index, &defip_temp_e);
       SOC_IF_ERROR_RETURN(rv);
    }

    /* Write entry to the second half of the tcam. */
    if (SOC_IS_TRIDENT3X(u)) {
        soc_mem_t   mem = L3_DEFIPm;
        soc_mem_field32_set(u, mem, e, FLEX_CTR_BASE_COUNTER_IDX0f, 0);
        soc_mem_field32_set(u, mem, e, FLEX_CTR_BASE_COUNTER_IDX1f, 0);
        if (eupr) {
            soc_mem_field32_set(u, mem, eupr, FLEX_CTR_BASE_COUNTER_IDX0f, 0);
            soc_mem_field32_set(u, mem, eupr, FLEX_CTR_BASE_COUNTER_IDX1f, 0);
        }
    } 

    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, new_index, e));
    if (eupr) {
        offset = soc_fb_lpm_paired_offset(u, new_index);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                            new_index + offset, eupr));
    }

    if (temp_index != -1) {
       sal_memset(&defip_temp_e, 0x0, sizeof(defip_temp_e));
       SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                          temp_index, &defip_temp_e));
    }
    return SOC_E_NONE;
}

#ifdef FB_LPM_DEBUG
#define DBG_LPM128_HASH_INSERT(u, entry_data, entry_data_upr, tab_index, rvt_index0, rvt_index1)  \
    dbg_soc_fb_lpm128_hash_insert(u, entry_data, entry_data_upr, tab_index, LPM_NO_MATCH_INDEX, 0,\
                              rvt_index0, rvt_index1)

void dbg_soc_fb_lpm128_hash_insert(int u, void *entry_data, void *entry_data_upr,
                               uint32 tab_index, uint32 old_index, int pfx,
                               uint32 *rvt_index0, uint32 *rvt_index1)
{
    _soc_fb_lpm128_hash_entry_t    key_hash = {0};
    int mode = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, defip_mode0_field);

    if (mode && mode != 3) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "Invalid mode while inserting lpm128 hash for pfx - %d\n"), pfx));
        return;
    }
    if (mode) {
        /* IPV6 entry */
        if (SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, entry_data) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, entry_data) &&
            SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, entry_data_upr) &&
            SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, entry_data_upr)) {
            SOC_FB_LPM128_HASH_ENTRY_IPV6_GET(u, entry_data, entry_data_upr,
                                              key_hash);
            (void) _soc_fb_lpm128_hash_insert(
                        DBG_SOC_LPM128_STATE_HASH2(u),
                        _soc_fb_lpm128_hash_compare_key,
                        key_hash,
                        pfx,
                        old_index,
                        (tab_index << 1) | FB_LPM_HASH_IPV6_MASK,
                        rvt_index0);
        }
    } else {
        /* IPV4 entry */
        if (SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_0_GET(u, entry_data, key_hash);
            (void) _soc_fb_lpm128_hash_insert(DBG_SOC_LPM128_STATE_HASH2(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    (tab_index << 1),
                                    rvt_index0);
        }
        if (SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, entry_data)) {
            SOC_FB_LPM_HASH_ENTRY_IPV4_1_GET(u, entry_data, key_hash);
            (void)_soc_fb_lpm128_hash_insert(DBG_SOC_LPM128_STATE_HASH2(u),
                                    _soc_fb_lpm128_hash_compare_key,
                                    key_hash,
                                    pfx,
                                    old_index,
                                    ((tab_index << 1) + 1),
                                    rvt_index1);
        }
    }
}

void
lpm128_idx_to_pfx_dump(int u, int before)
{
    int i = 0;
    int defip_table_size;
    int pfx1, pfx2;
    int prev_pfx1 = -2;
    int prev_pfx2 = -2;
    int repeat_cnt = 0;
    int offset = 0;

    if (soc_fb_lpm_table_sizes_get(u, &defip_table_size, NULL) < 0 ) {
        return ;
    }

    LOG_CLI((BSL_META_U(u,
                        "|===============||===============|\n")));
    for (i = 0; i < defip_table_size; i++) {
        offset = soc_fb_lpm_paired_offset(u, i);
        if (LPM128_IN_ODD_TCAM(u, i)) {
            i += offset - 1;
            continue;
        }
        if (before == 2) {
            pfx1 = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(u, i);
            pfx2 = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(u, i + offset);
        } else if (before == 1) {
            pfx1 = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP(u, i);
            pfx2 = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP(u, i + offset);
        } else {
            pfx1 = SOC_LPM128_INDEX_TO_PFX_GROUP(u, i);
            pfx2 = SOC_LPM128_INDEX_TO_PFX_GROUP(u, i + offset);
        }

        if (pfx1 == prev_pfx1 && pfx2 == prev_pfx2) {
            repeat_cnt++;
            continue;
        }

        if (repeat_cnt) {
            repeat_cnt++;
            LOG_CLI((BSL_META_U(u,
                            "| repeats %5d || repeats %5d |\n"),
                            repeat_cnt, repeat_cnt));
        }

        repeat_cnt = 0;
        LOG_CLI((BSL_META_U(u,
                            "| %5d [%5d] || %5d [%5d] |\n"),
                            pfx1, i, pfx2, i + offset));
        prev_pfx1 = pfx1;
        prev_pfx2 = pfx2;
    }
    if (repeat_cnt) {
        repeat_cnt++;
        LOG_CLI((BSL_META_U(u,
                        "| repeats %5d || repeats %5d |\n"),
                        repeat_cnt, repeat_cnt));
    }
    LOG_CLI((BSL_META_U(u,
                        "|===============||===============|\n")));
    return ;
}

void
lpm_hash_dump(int u, int before, int step, uint32 usec)
{
    int i, j;
    _soc_fb_lpm_hash_t * hash = before == 2 ? DBG_SOC_LPM_STATE_HASH2(u) :
                                before == 1 ? DBG_SOC_LPM_STATE_HASH(u) :
                                              SOC_LPM_STATE_HASH(u);

    if (step < 5) {
        step = 5;
    }
                                              
    LOG_CLI((" Lpm table\n"));
    for (i = 0, j = 0; i < hash->index_count; i++) {
        if (hash->table[i] != FB_LPM_HASH_INDEX_NULL) {
            LOG_CLI((" t[%d] %d\n", i, hash->table[i]));
            if ((j % step) == 0) {
                sal_usleep (usec);
            }
            j++;
        }
    }

    LOG_CLI((" Lpm link_table \n"));
    for (i = 0, j = 0; i < hash->entry_count; i++) {
        if (hash->link_table[i] != FB_LPM_HASH_INDEX_NULL) {
            LOG_CLI((" l[%d] %d\n", i, hash->link_table[i]));
            if ((j % step) == 0) {
                sal_usleep(usec);
            }
            j++;
        }
    }

    return ;
}


void
lpm_count_dump(int u, int before)
{
    if (before == 2) {
        LOG_CLI(("Lpm128 %d %d %d %d Lpm %d %d %d. before=%d\n",
        SOC_LPM128_STAT_128BV6_COUNT2(u),
        SOC_LPM128_STAT_V4_COUNT2(u),
        SOC_LPM128_STAT_64BV6_COUNT2(u),
        SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT2(u),
        SOC_LPM_V4_COUNT2(u),
        SOC_LPM_64BV6_COUNT2(u),
        SOC_LPM_V4_HALF_ENTRY_COUNT2(u),
        before));
    } else if (before == 1) {
        LOG_CLI(("Lpm128 %d %d %d %d Lpm %d %d %d. before=%d\n",
        SOC_LPM128_STAT_128BV6_COUNT1(u),
        SOC_LPM128_STAT_V4_COUNT1(u),
        SOC_LPM128_STAT_64BV6_COUNT1(u),
        SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT1(u),
        SOC_LPM_V4_COUNT1(u),
        SOC_LPM_64BV6_COUNT1(u),
        SOC_LPM_V4_HALF_ENTRY_COUNT1(u),
        before));
    } else {
        LOG_CLI(("Lpm128 %d %d %d %d Lpm %d %d %d. before=%d\n",
        SOC_LPM128_STAT_128BV6_COUNT(u),
        SOC_LPM128_STAT_V4_COUNT(u),
        SOC_LPM128_STAT_64BV6_COUNT(u),
        SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u),
        SOC_LPM_V4_COUNT(u),
        SOC_LPM_64BV6_COUNT(u),
        SOC_LPM_V4_HALF_ENTRY_COUNT(u),
        before));
    }
}

void
lpm128_hash_dump(int u, int before, int step, uint32 usec)
{
    int i, j;
    _soc_fb_lpm_hash_t * hash = before == 2 ? DBG_SOC_LPM128_STATE_HASH2(u) :
                                before == 1 ? DBG_SOC_LPM128_STATE_HASH(u) :
                                              SOC_LPM128_STATE_HASH(u);
    if (step < 5) {
        step = 5;
    }
                                              
    LOG_CLI((" Lpm 128 table\n"));
    for (i = 0, j = 0; i < hash->index_count; i++) {
        if (hash->table[i] != FB_LPM_HASH_INDEX_NULL) {
            LOG_CLI((" t8[%d] %d\n", i, hash->table[i]));
            if ((j % step) == 0) {
                sal_usleep (usec);
            }
            j++;
        }
    }

    LOG_CLI((" Lpm 128 link_table \n"));
    for (i = 0, j = 0; i < hash->entry_count; i++) {
        if (hash->link_table[i] != FB_LPM_HASH_INDEX_NULL) {
            LOG_CLI((" l8[%d] %d\n", i, hash->link_table[i]));
            if ((j % step) == 0) {
                sal_usleep(usec);
            }
            j++;
        }
    }

    return ;
}


void
lpm128_state_dump(int u, int before)
{
    soc_lpm128_state_p lpm_state_ptr;
    int start1;
    int start2;
    int end1;
    int end2;
    int vent;
    int fent;
    int prev;
    int next;
    int total_prefixes = 0;
    int curr_pfx = MAX_PFX128_INDEX;

    LOG_CLI((BSL_META_U(u, "before = %d:\n"), before));

    if (SOC_LPM128_UNRESERVED_STATE(u)) {
        LOG_CLI((BSL_META_U(u,
                            "Reserved prefixes=====================\n")));
    } else {
        LOG_CLI((BSL_META_U(u,
                            "Prefixes=====================\n")));
    }

    lpm_state_ptr = before == 2 ? DBG_SOC_LPM128_STATE2(u) :
                    before == 1 ? DBG_SOC_LPM128_STATE(u) :
                                  SOC_LPM128_STATE(u);
    while (lpm_state_ptr && curr_pfx > 0) {
        start1 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, curr_pfx);
        start2 = SOC_LPM128_STATE_START2(u, lpm_state_ptr, curr_pfx);
        end1   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, curr_pfx);
        end2   = SOC_LPM128_STATE_END2(u, lpm_state_ptr, curr_pfx);
        vent   = SOC_LPM128_STATE_VENT(u, lpm_state_ptr, curr_pfx);
        fent   = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, curr_pfx);
        prev   = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, curr_pfx);
        next   = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, curr_pfx);
        LOG_CLI((BSL_META_U(u,
                            "PFX:%3d S1:%4d E1:%4d S2:%4d E2:%4d "\
                            "V:%4d F:%4d P:%3d N:%3d \n"),
                 curr_pfx, start1, end1, start2, end2,
                 vent, fent, prev, next ));
        curr_pfx = next;
        total_prefixes += vent;
    }

    lpm_state_ptr = before == 2 ? DBG_SOC_LPM128_UNRESERVED_STATE2(u) :
                    before == 1 ? DBG_SOC_LPM128_UNRESERVED_STATE(u) :
                                  SOC_LPM128_UNRESERVED_STATE(u);
    curr_pfx = MAX_PFX128_INDEX;
    if (lpm_state_ptr) {
        LOG_CLI((BSL_META_U(u,
                            "Total number of reserved prefixes - %d\n"), total_prefixes));
        total_prefixes = 0;

        LOG_CLI((BSL_META_U(u,
                            "Unreserved prefixes=====================\n")));
    }
    while (lpm_state_ptr && curr_pfx > 0) {
        start1 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, curr_pfx);
        start2 = SOC_LPM128_STATE_START2(u, lpm_state_ptr, curr_pfx);
        end1   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, curr_pfx);
        end2   = SOC_LPM128_STATE_END2(u, lpm_state_ptr, curr_pfx);
        vent   = SOC_LPM128_STATE_VENT(u, lpm_state_ptr, curr_pfx);
        fent   = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, curr_pfx);
        prev   = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, curr_pfx);
        next   = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, curr_pfx);
        LOG_CLI((BSL_META_U(u,
                            "PFX:%3d S1:%4d E1:%4d S2:%4d E2:%4d "\
                            "V:%4d F:%4d P:%3d N:%3d \n"),
                 curr_pfx, start1, end1, start2, end2,
                 vent, fent, prev, next ));
        curr_pfx = next;
        total_prefixes += vent;
    }

    if (lpm_state_ptr) {
        LOG_CLI((BSL_META_U(u,
                            "Total number of unreserved prefixes - %d\n"), total_prefixes));
    } else {
        LOG_CLI((BSL_META_U(u,
                            "Total number of prefixes - %d\n"), total_prefixes));
    }
    return;
}
int
dbg_soc_fb_lpm_reinit_done(int unit, int ipv6)
{
    int idx;
    int prev_idx = MAX_PFX_INDEX(unit);
    uint32      v0 = 0, v1 = 0;
    int from_ent = -1;
    soc_lpm_state_p state2 = DBG_SOC_LPM_STATE2(unit);
    int defip_table_size = soc_mem_index_count(unit, L3_DEFIPm);

    if (SOC_URPF_STATUS_GET(unit)) {
        if (soc_feature(unit, soc_feature_l3_defip_hole)) {
              defip_table_size = SOC_APOLLO_B0_L3_DEFIP_URPF_SIZE;
        } else if (SOC_IS_APOLLO(unit)) {
            defip_table_size = SOC_APOLLO_L3_DEFIP_URPF_SIZE;
        } else {
            /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
             * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
             * L3_DEFIP table is not divided into 2 to support URPF.
             */
            if (!soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
                defip_table_size >>= 1;
            }
        }
    } 

    SOC_LPM_STATE_P(state2, MAX_PFX_INDEX(unit)) = -1;

    for (idx = MAX_PFX_INDEX(unit); idx >= 0; idx--) {
        if ((idx == MAX_PFX_INDEX(unit)) && (SOC_LPM_STATE_S(state2, idx) <= 0)) {
            continue;
        }
            
        if (-1 == SOC_LPM_STATE_S(state2, idx)) {
            continue;
        }

        if (prev_idx != idx) {
            SOC_LPM_STATE_P(state2, idx) = prev_idx;
            SOC_LPM_STATE_N(state2, prev_idx) = idx;
        }
        SOC_LPM_STATE_F(state2, prev_idx) =                    \
                          SOC_LPM_STATE_S(state2, idx) -      \
                          SOC_LPM_STATE_E(state2, prev_idx) - 1;
        prev_idx = idx;
        
        if (idx == MAX_PFX_INDEX(unit) || (ipv6 && SOC_LPM_PFX_IS_V4(unit, idx)) ||
           (!ipv6 && SOC_LPM_PFX_IS_V6_64(unit, idx))) {
            continue;
        }

        if (!(soc_feature(unit, soc_feature_l3_shared_defip_table) ||
            (soc_feature(unit, soc_feature_l3_kt2_shared_defip_table)))) {
            continue;
        }

        if (SOC_LPM_PFX_IS_V4(unit, idx)) {
            from_ent = SOC_LPM_STATE_E(state2, idx);
            if (!soc_feature(unit, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                v0 = SOC_MEM_OPT_F32_VALID0_GET(unit, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(unit)[from_ent]));
                v1 = SOC_MEM_OPT_F32_VALID1_GET(unit, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(unit)[from_ent]));
                SOC_LPM_V4_COUNT2(unit) += (SOC_LPM_STATE_V(state2, idx) << 1);
                if ((v0 == 0) || (v1 == 0)) {
                    SOC_LPM_V4_COUNT2(unit) -= 1;
                }
                if ((v0 && (!(v1))) || (!(v0) && v1)) {
                    SOC_LPM_COUNT_INC(SOC_LPM_V4_HALF_ENTRY_COUNT2(unit));
                } 
            } else {
                SOC_LPM_V4_COUNT2(unit) += SOC_LPM_STATE_V(state2, idx);
            }
        } else {
            SOC_LPM_64BV6_COUNT2(unit) += SOC_LPM_STATE_V(state2, idx);
        }
    }

    SOC_LPM_STATE_N(state2, prev_idx) = -1;
    SOC_LPM_STATE_F(state2, prev_idx) =                       \
                          defip_table_size -                   \
                          SOC_LPM_STATE_E(state2, prev_idx) - 1;

    return (SOC_E_NONE);
}



int
dbg_soc_fb_lpm_reinit(int unit, int idx, uint32 *lpm_entry)
{
    int pfx_len;

    if (SOC_IS_HAWKEYE(unit)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (_soc_fb_lpm_prefix_length_get(unit, lpm_entry, &pfx_len));

    if (DBG_SOC_LPM_STATE2(unit)[pfx_len].vent == 0) {
        DBG_SOC_LPM_STATE2(unit)[pfx_len].start = idx;
        DBG_SOC_LPM_STATE2(unit)[pfx_len].end = idx;
    } else {
        DBG_SOC_LPM_STATE2(unit)[pfx_len].end = idx;
    }

    DBG_SOC_LPM_STATE2(unit)[pfx_len].vent++;
    LPM_HASH_INSERT2(unit, lpm_entry, idx, NULL, NULL);
    LPM_AVL_INSERT(unit, lpm_entry, idx);

    return (SOC_E_NONE);
}


STATIC int
dbg_soc_fb_lpm128_setup_pfx_state(int u, soc_lpm128_state_p lpm_state_ptr)
{
    int prev_idx = MAX_PFX128_INDEX;
    int idx = 0;
    int from_ent = -1;
    int v0 = 0, v1 = 0;

    if (NULL == lpm_state_ptr) {
       return SOC_E_INTERNAL;
    }

    SOC_LPM128_STATE_PREV(u, lpm_state_ptr, MAX_PFX128_INDEX) = -1;

    for (idx = MAX_PFX128_INDEX; idx > 0; idx--) {
        if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, idx) == -1) {
           continue;
        }

        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, idx) = prev_idx;
        SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_idx) = idx;

        prev_idx = idx;
        if (SOC_LPM128_PFX_IS_V6_64(u, idx)) {
           SOC_LPM128_STAT_64BV6_COUNT2(u) += SOC_LPM128_STATE_VENT(u,
                                                           lpm_state_ptr, idx);
        }

        if (SOC_LPM128_PFX_IS_V6_128(u, idx)) {
           SOC_LPM128_STAT_128BV6_COUNT2(u) += SOC_LPM128_STATE_VENT(u,
                                                            lpm_state_ptr, idx);
        }

        if (SOC_LPM128_PFX_IS_V4(u, idx)) {
            if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, idx) != -1) {
               from_ent = SOC_LPM128_STATE_END2(u, lpm_state_ptr, idx);
            } else {
               from_ent = SOC_LPM128_STATE_END1(u, lpm_state_ptr, idx);
            }
            if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                v0 = SOC_MEM_OPT_F32_VALID0_GET(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[from_ent]));
                v1 = SOC_MEM_OPT_F32_VALID1_GET(u, L3_DEFIPm, &(DBG_SOC_L3_DEFIP(u)[from_ent]));
                SOC_LPM128_STAT_V4_COUNT2(u) += (SOC_LPM128_STATE_VENT(u,
                                                          lpm_state_ptr, idx)) << 1;
                if ((v0 == 0) || (v1 == 0)) {
                    SOC_LPM128_STAT_V4_COUNT2(u) -= 1;
                }
                if ((v0 && (!(v1))) || ((!(v0) && v1))) {
                    SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT2(u));
                }
            } else {
                SOC_LPM128_STAT_V4_COUNT2(u) += SOC_LPM128_STATE_VENT(u,
                                                          lpm_state_ptr, idx);
            }
        }
    }

    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_idx) = -1;

    return SOC_E_NONE;
}


int
dbg_soc_fb_lpm128_reinit_done(int unit, int ipv6)
{
    int paired_table_size = 0;
    int defip_table_size = 0;
    int i = 0;
    int pfx;
    int is_reserved = 0;
    int start_idx = -1;
    int end_idx = -1;
    int fent_incr = 0;
    int found = 0;
    int first_v4_index = -1;
    int split_done = 0;
    int next_pfx = -1;
    soc_lpm128_state_p lpm_state_ptr = DBG_SOC_LPM128_STATE2(unit);
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int offset = 0;
#ifdef BCM_UTT_SUPPORT
    int l_tcam_start = 0;
#endif

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_NONE;
    }

    if (!ipv6) {
        return SOC_E_NONE;
    }

    if (max_v6_entries == 0) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(unit, &paired_table_size,
                                                   &defip_table_size));
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }

    lpm_state_ptr = DBG_SOC_LPM128_STATE2(unit);
    if (lpm_state_ptr == NULL) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (dbg_soc_fb_lpm128_setup_pfx_state(unit, lpm_state_ptr));

    pfx = MAX_PFX128_INDEX;
    next_pfx = SOC_LPM128_STATE_NEXT(unit, lpm_state_ptr, pfx);
    start_idx = 0;
    if (!is_reserved) {
        end_idx = paired_table_size;
        if (next_pfx == -1) {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = end_idx;
        }
    } else {
        end_idx = max_v6_entries;
        if (next_pfx == -1) {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = end_idx << 1;
        }
    }

    for (i = start_idx; (next_pfx != -1) && i < end_idx; i++) {
        if (LPM128_IN_ODD_TCAM_FIRST(unit, i)) {
            if (is_reserved) {
#ifdef BCM_UTT_SUPPORT
                /*
                 * Reserved section will have only V6 entries.
                 * Add logical TCAM depth to move to next valid V6 entry.
                 */
                if (soc_feature(unit, soc_feature_utt)) {
                    offset = soc_fb_lpm_paired_offset(unit, i);
                    i += offset;
                } else
#endif
                {
                    i += max_v6_entries;
                }
            }
            if (first_v4_index != -1 && !split_done) {
                offset = soc_fb_lpm_paired_offset(unit, first_v4_index);
                i = first_v4_index + offset;
                split_done = 1;
            }
        }
        if (DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(unit, i) != -1) {
            pfx = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(unit, i);
            if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
                if (first_v4_index == -1) {
                   first_v4_index = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
                }
                fent_incr = 1;
            } else {
                fent_incr = 2;
            }
            if (pfx != MAX_PFX128_INDEX) {
                found = 1;
            }
        } else {
            if ((SOC_LPM128_PFX_IS_V6(u, pfx)) && LPM128_IN_ODD_TCAM(unit, i)) {
                continue;
            }
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) += fent_incr;
        }
    }

    if (found) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, MAX_PFX128_INDEX) = 0;
    }
    lpm_state_ptr = DBG_SOC_LPM128_UNRESERVED_STATE2(unit);
    if (lpm_state_ptr == NULL) {
       return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (dbg_soc_fb_lpm128_setup_pfx_state(unit, lpm_state_ptr));
    pfx = MAX_PFX128_INDEX;
    found = 0;
#ifdef BCM_UTT_SUPPORT
    /*
     * Get start of unreserved index.
     */
    if (soc_feature(unit, soc_feature_utt)) {
        start_idx = 0;
        SOC_IF_ERROR_RETURN
            (soc_utt_lpm_scaled_unreserved_start_index(unit,
                                                       &start_idx));
    } else
#endif
    {
        start_idx = ((max_v6_entries / tcam_depth) * tcam_depth * 2) +
                    max_v6_entries % tcam_depth;
    }
    next_pfx = SOC_LPM128_STATE_NEXT(unit, lpm_state_ptr, pfx);
    if (next_pfx == -1) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = (paired_table_size -
                                                         (max_v6_entries << 1));
    }
    for (i = start_idx; (next_pfx != -1) && i < paired_table_size; i++) {
        if (LPM128_IN_ODD_TCAM_FIRST(unit, i)) {
#ifdef BCM_UTT_SUPPORT
            /*
             * Add delta of v6 entries reserved in last lookup.
             * Difference between logical tcam start addr of unreserved section
             * and unreserved start index will give num v6 entries reserved in
             * last lookup.
             */
            if (soc_feature(unit, soc_feature_utt)) {
                l_tcam_start = soc_utt_lpm128_logical_tcam_start_index(unit,
                                                                       start_idx);
                i += (start_idx - l_tcam_start);
            } else
#endif
            {
                i += (max_v6_entries % tcam_depth);
            }
        }
        if (DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(unit, i) != -1) {
            pfx = DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(unit, i);
            if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
                fent_incr = 1;
            } else {
                fent_incr = 2;
            }
            if (pfx != MAX_PFX128_INDEX) {
                found = 1;
            }
        } else {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) += fent_incr;
        }
    }

    if (found) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, MAX_PFX128_INDEX) = 0;
    }

    return SOC_E_NONE;
}

int
dbg_soc_fb_lpm128_reinit(int u, int idx, uint32 *lpm_entry,
                     uint32 *lpm_entry_upr)
{
    int pfx = 0;
    int is_reserved = 0;
    soc_lpm128_state_p lpm_state_ptr = DBG_SOC_LPM128_STATE2(u);
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    int offset = 0;

    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_NONE;
    }

    if (max_v6_entries == 0) {
        return SOC_E_NONE;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN
        (_soc_fb_lpm128_prefix_length_get(u, lpm_entry, lpm_entry_upr,
                                          &pfx));

    if (!is_reserved || (SOC_LPM128_PFX_IS_V6_128(u, pfx))) {
        lpm_state_ptr = DBG_SOC_LPM128_STATE2(u);
    } else {
        lpm_state_ptr = DBG_SOC_LPM128_UNRESERVED_STATE2(u);
    }

    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = idx;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = idx;
    } else {
        if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = idx;
        } else {
            if (((SOC_LPM128_PFX_IS_V6(u, pfx))) && LPM128_IN_ODD_TCAM(u, idx)) {
                return SOC_E_NONE;
            }
            if ((SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) + 1) != idx &&
                (SOC_LPM128_PFX_IS_V4(u, pfx))) {
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = idx;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = idx;
            } else {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = idx;
            }
        }
    }

    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) += 1;
    DBG_LPM128_HASH_INSERT(u, lpm_entry, lpm_entry_upr, idx, NULL, NULL);
    DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(u, idx) = pfx;
    if (SOC_LPM128_PFX_IS_V6(u, pfx)) {
        offset = soc_fb_lpm_paired_offset(u, idx);
        DBG_SOC_LPM128_INDEX_TO_PFX_GROUP2(u, idx + offset) = pfx;
    }

    return (SOC_E_NONE);
}


int lpm_state_verify(int u, int *err)
{
    int i = 0, idx = 0;
    int paired_size = 0, defip_size = 0;
    int free_count = 0;
    soc_lpm_state_p lpm_state_ptr = SOC_LPM_STATE(u);
    int32 curr_pfx = MAX_PFX_INDEX(u);
    int32 *pfxmap = NULL;
    int32 *pfxmap2 = NULL;
    int start;
    int end;
    int vent = 0;
    int vent_count = 0;
    int next_pfx = -1;
    int fent = 0;
    int error_count = 0;

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_size, &defip_size));

    if (lpm_state_ptr == NULL) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "lpm state ptr is NULL, just return\n")));
        return SOC_E_MEMORY;
    }

    pfxmap = sal_alloc(defip_size * sizeof(int), "debug pfxmap");
    if (pfxmap == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(pfxmap, -1, sizeof(int) * defip_size);

    pfxmap2 = sal_alloc(defip_size * sizeof(int), "debug pfxmap2");
    if (pfxmap2 == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(pfxmap2, -1, sizeof(int) * defip_size);

    free_count = SOC_LPM_STATE_F(lpm_state_ptr, MAX_PFX_INDEX(u));

    while (curr_pfx != -1) {
        next_pfx = SOC_LPM_STATE_N(lpm_state_ptr, curr_pfx);
        if (curr_pfx == MAX_PFX_INDEX(u)) {
            if (next_pfx == -1) {
                goto done;
            }
            curr_pfx = next_pfx;
            continue;
        }

        if (next_pfx != -1) {
            if (SOC_LPM_STATE_P(lpm_state_ptr, next_pfx) != curr_pfx ||
                curr_pfx <= next_pfx) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Corrupted list0 pfx:%d != next's prev:%d,"\
                                      " next_pfx:%d \n"), curr_pfx,
                                      SOC_LPM_STATE_P(lpm_state_ptr, next_pfx),
                                      next_pfx));
                error_count++;
            }
        }

        start = SOC_LPM_STATE_S(lpm_state_ptr, curr_pfx);
        end   = SOC_LPM_STATE_E(lpm_state_ptr, curr_pfx);
        fent  = SOC_LPM_STATE_F(lpm_state_ptr, curr_pfx);
        vent  = SOC_LPM_STATE_V(lpm_state_ptr, curr_pfx);
        if (start <= -1 || end <= -1 || fent <= -1 || (vent <= 0 && fent <= 0)) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "pfx: %d Negative values detected\n"), curr_pfx));
            error_count++;
        }

        if (vent != end + 1 - start) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "pfx: %d Vent wrong\n"), curr_pfx));
        }

        for (i = start; i <= end + fent; i++) {
            if (pfxmap[i-paired_size] != -1) {
                /* There is a overwrite */
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Conflict0 for pfx: %d with pfx: %d\n"),
                                      curr_pfx, pfxmap[i-paired_size]));
                error_count++;
            } else {
                pfxmap[i-paired_size] = curr_pfx;
                if (i <= end) {
                    pfxmap2[i-paired_size] = curr_pfx;
                }
            }
        }

        vent_count += vent;
        free_count += fent;
        curr_pfx = next_pfx;
    }

    if ((free_count + vent_count) != defip_size) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "Count mismatch! free: %d vent: %d defipsize: %d\n"),
                   free_count, vent_count, defip_size));
        error_count++;
    }

    for (i = 0; i < defip_size; i++) {

        idx = i + paired_size;
        /* The following does sanity check on lpm hash table, with the 
         * help from lpm state (say it's trustable) and a little help
         * from cached l3_defip 
         */
        if (pfxmap2[i] != -1 && SOC_LPM_PFX_IS_V6_64(u, pfxmap2[i])) {
            /* left must exist in hash table, right must not exist. */
            if (SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)] == 0 ||
                SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1]) {
                error_count++;
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Check table error0: %d "
                                      "  pfx %d, check_table %d %d \n"),
                                      i, pfxmap2[i],
                                      SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)],
                                      SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1]));
            }
        } else if (pfxmap2[i] != -1 && SOC_LPM_PFX_IS_V4(u, pfxmap2[i])) {
            /* Valid v4 */
            /* left half must exist in hash table */
            if (SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)] == 0) {
                error_count++;
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Check table error2: %d "
                                      "  pfx %d, check_table %d \n"),
                                      i, pfxmap2[i],
                                      SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)]));
            }

            /* right half must exist in hash table when VALID1 is 1 */
            if (SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1] == 0) {
                /* further check if it's half entry, report error if not half entry */
                if (soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[idx]), VALID1f)) {
                    error_count++;
                    LOG_ERROR(BSL_LS_SOC_LPM,
                              (BSL_META_U(u,
                                          "Check table error3: %d "
                                          "  pfx %d, check_table %d \n"),
                                          i, pfxmap2[i],
                                          SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1]));
                }
            }
        } else {
            /* Not valid entry, must not exist in hash table */
            if (SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)] ||
                SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1]) {
                error_count++;
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Check table error4: %d "
                                      "  pfx %d, check_table %d %d \n"),
                                      i, pfxmap2[i],
                                      SOC_LPM_STATE_HASH(u)->check_table[(idx << 1)],
                                      SOC_LPM_STATE_HASH(u)->check_table[(idx << 1) + 1]));
            }
        }

    }


done:
    sal_free(pfxmap);
    sal_free(pfxmap2);
    if (err) {
        *err = (*err) + error_count;
    }

    return SOC_E_NONE;
}


int lpm128_state_verify(int u)
{
    int i = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int paired_size = 0, defip_size = 0;
    int free_count = 0;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);
    int32 curr_pfx = MAX_PFX128_INDEX;
    int32 *pfxmap = NULL;
    int32 *pfxmap2 = NULL;
    int start1, start2;
    int end1, end2;
    int vent = 0;
    int vent_count = 0;
    int split_count = 0;
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    int is_reserved = 0;
    int next_pfx = -1;
    int prev_split_pfx = -1;
    int max_entries = 0;
    int end_idx = 0;
    int fent = 0;
    int first_v4_start = -1;
    int error_count = 0;
    int pfx_state_size = sizeof(soc_lpm128_state_t) * (MAX_PFX128_ENTRIES);
    int pfx_state_size2 = sizeof(soc_lpm_state_t) * (MAX_PFX_ENTRIES(u));
    int offset = 0;
#ifdef BCM_UTT_SUPPORT
    int rem_fent;
    int l_tcam_start = 0;
    int end1_l_tcam_start_idx;
    int start1_l_tcam_start_idx;
#endif

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_size, &defip_size));

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    if (is_reserved) {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
        if (NULL == lpm_state_ptr) {
            if ((max_v6_entries * 2) == paired_size) {
                return SOC_E_NONE;
            }
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "reserved and unreserved state ptr is NULL\n")));
            return SOC_E_MEMORY;
        }
    }

    if (lpm_state_ptr == NULL) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "lpm128 state ptr is NULL, just return\n")));
        return SOC_E_MEMORY;
    }

    pfxmap = sal_alloc(paired_size * sizeof(int), "debug pfxmap");
    if (pfxmap == NULL) {
        return SOC_E_MEMORY;
    }
    pfxmap2 = sal_alloc(paired_size * sizeof(int), "debug pfxmap2");
    if (pfxmap2 == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(pfxmap, -1, sizeof(int) * paired_size);
    sal_memset(pfxmap2, -1, sizeof(int) * paired_size);

    free_count = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, MAX_PFX128_INDEX);

    while (curr_pfx != -1) {
        next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, curr_pfx);
        if (curr_pfx == MAX_PFX128_INDEX) {
            if (next_pfx == -1) {
                goto done;
            }
            curr_pfx = next_pfx;
            continue;
        }

        if (next_pfx != -1) {
            if (SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx) != curr_pfx ||
                curr_pfx <= next_pfx) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Corrupted list pfx:%d != next's prev:%d,"\
                                      " next_pfx:%d \n"), curr_pfx,
                                      SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx),
                                      next_pfx));
                error_count++;
            }
        }

        start1 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, curr_pfx);
        end1 = SOC_LPM128_STATE_END1(u, lpm_state_ptr, curr_pfx);
        fent = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, curr_pfx);
        vent = SOC_LPM128_STATE_VENT(u, lpm_state_ptr, curr_pfx);
        if (start1 <= -1 || end1 <= -1 || fent <= -1 || vent <= 0) {
            LOG_ERROR(BSL_LS_SOC_LPM,
                      (BSL_META_U(u,
                                  "pfx: %d Negative values detected\n"), curr_pfx));
            error_count++;
        }

        start2 = SOC_LPM128_STATE_START2(u, lpm_state_ptr, curr_pfx);
        end2 = SOC_LPM128_STATE_END2(u, lpm_state_ptr, curr_pfx);

        if (SOC_LPM128_PFX_IS_V6(u, curr_pfx)) {
            if (fent & 1) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "V6 pfx: %d fent odd number \n"), curr_pfx));
                error_count++;
            }
            end_idx  = end1;
        } else {
            if (start2 != -1) {
                end_idx  = end2;
            } else {
                end_idx = end1;
            }
        }

        if (start2 != -1) {
            if (split_count > 1) {
                 LOG_ERROR(BSL_LS_SOC_LPM,
                           (BSL_META_U(u,
                                       "More than one split. pfx: %d prev split pfx: %d\n"),
                            curr_pfx, prev_split_pfx));
                error_count++;
            } else {
                split_count++;
                prev_split_pfx = curr_pfx;
            }
            if (end2 == -1) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "pfx: %d end2 is -1\n"), curr_pfx));
                error_count++;
            }

            for (i = start2; i <= end_idx + fent; i++) {
                if (pfxmap[i] != -1) {
                    /* There is a overwrite */
                    LOG_ERROR(BSL_LS_SOC_LPM,
                              (BSL_META_U(u,
                                          "S2 Conflict for pfx: %d with pfx: %d\n"),
                                          curr_pfx, pfxmap[i]));
                    error_count++;
                } else {
                    pfxmap[i] = curr_pfx;
                    if (i <= end_idx) {
                        pfxmap2[i] = curr_pfx;
                    }
                }
            }
        }

        if (SOC_LPM128_PFX_IS_V4(u, curr_pfx)) {
            if (first_v4_start == -1){
                first_v4_start = start1;
            }
        }

        if (start2 != -1) {
            end_idx = end1;
        } else {
            if (SOC_LPM128_PFX_IS_V4(u, curr_pfx)) {

                int v6_count_in_last_tcam = 0;
#ifdef BCM_UTT_SUPPORT
                if (soc_feature(u, soc_feature_utt)) {
                    l_tcam_start =
                        soc_utt_lpm128_logical_tcam_start_index(u,
                                                                first_v4_start);
                    v6_count_in_last_tcam = first_v4_start - l_tcam_start;
                } else
#endif
                {
                    v6_count_in_last_tcam = first_v4_start % tcam_depth;
                }
                /* even if there's no range2 for valid entries, there still could be
                 * a range2 for free entries, in which case both start1 and end1 should
                 * be in same tcam number as the first v4.
                 * Condition: end1 + fent is in a larger tcam_number than end1,
                 *            both start1 and end1 in same tcam number as first_v4_start
                 *            and there left v6 entries (valid or free).
                 */
                end_idx = end1 + fent;
                if (LPM128_TCAM_NUM(u, end_idx) > LPM128_TCAM_NUM(u, end1)  &&
                    LPM128_TCAM_NUM(u, start1) == LPM128_TCAM_NUM(u, first_v4_start) &&
                    LPM128_TCAM_NUM(u, end1) == LPM128_TCAM_NUM(u, first_v4_start))  {
                    end_idx += v6_count_in_last_tcam;
                }

            } else {
                /* For ipv6, get the last entry (free or valid) index */
                int k;
                if (next_pfx != -1) {
                    int start1_next_pfx;
                    int end_idx_comp;

                    start1_next_pfx = SOC_LPM128_STATE_START1(u, lpm_state_ptr, next_pfx);
                    if (LPM128_TCAM_NUM(u, end1) == LPM128_TCAM_NUM(u, (start1_next_pfx - 1))) {
                        end_idx = end1 + (fent / 2);
                    } else {
#ifdef BCM_UTT_SUPPORT
                        if (soc_feature(u, soc_feature_utt)) {

                            /* V6 pfx entry so end1 will always be in even tcam */
                            end1_l_tcam_start_idx =
                                soc_utt_lpm128_logical_tcam_start_index(u, end1);
                            start1_l_tcam_start_idx =
                                soc_utt_lpm128_logical_tcam_start_index(u, (start1_next_pfx - 1));

                            k = 0;
                            /*
                             * This take of adding extra TCAM offset if
                             * start1_next_pfx - 1 was in odd TCAM
                             */
                            while (end1_l_tcam_start_idx < start1_l_tcam_start_idx) {
                                offset = soc_fb_lpm_paired_offset(u, end1_l_tcam_start_idx);
                                end1_l_tcam_start_idx += (2 * offset);
                                k += offset;
                            }
                        } else
#endif
                        {
                            k = LPM128_TCAM_NUM(u, (start1_next_pfx - 1)) - LPM128_TCAM_NUM(u, end1);
                            k = k / 2 * tcam_depth;
                        }
                        end_idx = end1 + (fent / 2) + k;
                    }

                    if (LPM128_IN_ODD_TCAM(u, (start1_next_pfx - 1))) {
                        offset = soc_fb_lpm_paired_offset(u, (start1_next_pfx - 1));
                        end_idx_comp = (start1_next_pfx - 1) - offset;
                    } else {
                        end_idx_comp = (start1_next_pfx - 1);
                    }
                    if (end_idx != end_idx_comp) {
                        LOG_ERROR(BSL_LS_SOC_LPM,
                                  (BSL_META_U(u,
                                  "End_idx compare wrong: end_idx %d ,"\
                                  "end_idx_comp %d\n"), end_idx,
                                  end_idx_comp
                                  ));
                        error_count++;
                    }

                } else {
#ifdef BCM_UTT_SUPPORT
                    if (soc_feature(u, soc_feature_utt)) {

                        /* V6 pfx entry so end1 will always be in even tcam */
                        end1_l_tcam_start_idx =
                            soc_utt_lpm128_logical_tcam_start_index(u, end1);

                        offset = soc_fb_lpm_paired_offset(u, end1);
                        /* Compute free entries in last lookup */
                        k = offset - (end1 - end1_l_tcam_start_idx);
                        rem_fent = fent - (k * 2);

                        /* Add ODD TCAM depth */
                        k += offset;
                        end1_l_tcam_start_idx += (2 * offset);
                        while (rem_fent > 0) {
                            offset =
                                soc_fb_lpm_paired_offset(u, end1_l_tcam_start_idx);
                            end1_l_tcam_start_idx += (offset * 2);
                            rem_fent -= (offset * 2);
                            /*
                             * For the last lookup odd TCAM
                             * offset shouldn't be added
                             */
                            if (rem_fent > 0) {
                                k += offset;
                            }
                            k += offset;
                        }

                    } else
#endif
                    {

                        k = LPM128_TCAM_NUM(u, (end1 + fent / 2)) - LPM128_TCAM_NUM(u, end1);
                        k = fent / 2 + k * tcam_depth;
                    }
                    end_idx = end1 + k;

                    offset = soc_fb_lpm_paired_offset(u, end_idx);
                    if (end_idx != (paired_size - offset - 1)) {
                        LOG_ERROR(BSL_LS_SOC_LPM,
                                  (BSL_META_U(u,
                                  "End_idx caculation wrong: end_idx %d ,"\
                                  "should be %d\n"), end_idx,
                                  (paired_size - offset - 1)
                                  ));
                        error_count++;
                    }
                }
            }
        }


        for (i = start1; i <= end_idx; i++) {
            if (SOC_LPM128_PFX_IS_V4(u, curr_pfx) &&
                LPM128_IN_ODD_TCAM_FIRST(u, i) &&
                (LPM128_TCAM_NUM(u, i) - 1 == LPM128_TCAM_NUM(u, first_v4_start))) {

#ifdef BCM_UTT_SUPPORT
                if (soc_feature(u, soc_feature_utt)) {
                    l_tcam_start =
                        soc_utt_lpm128_logical_tcam_start_index(u,
                                                                first_v4_start);
                    i += (first_v4_start - l_tcam_start);
                } else
#endif
                {
                    i += first_v4_start % tcam_depth;
                }
            } else if (SOC_LPM128_PFX_IS_V6(u, curr_pfx) &&
                       LPM128_IN_ODD_TCAM_FIRST(u, i)) {

                offset = soc_fb_lpm_paired_offset(u, i);
                i += offset;
            }

            if (pfxmap[i] != -1) {
                /* There is a overwrite */
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Conflict for pfx: %d with pfx: %d\n"),
                                      curr_pfx, pfxmap[i]));
                error_count++;
            } else {
                pfxmap[i] = curr_pfx;
                if (SOC_LPM128_PFX_IS_V6(u, curr_pfx)) {
                    offset = soc_fb_lpm_paired_offset(u, i);
                    pfxmap[i + offset] = curr_pfx;
                }

                if (i <= end1) {
                    pfxmap2[i] = curr_pfx;
                    if (SOC_LPM128_PFX_IS_V6(u, curr_pfx)) {
                        offset = soc_fb_lpm_paired_offset(u, i);
                        pfxmap2[i + offset] = curr_pfx;
                    }
                }
            }
        }

        if (SOC_LPM128_PFX_IS_V4(u, curr_pfx)) {
            vent_count += vent;
        } else {
            vent_count += (vent * 2);
        }
        free_count += SOC_LPM128_STATE_FENT(u, lpm_state_ptr, curr_pfx);
        curr_pfx = next_pfx;
    }

    if (is_reserved) {
        max_entries = paired_size - (2 * max_v6_entries);
    } else {
        max_entries = paired_size;
    }
    if ((free_count + vent_count) != max_entries) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "count mismatch!!! free: %d vent: %d tablesize: %d\n"),
                   free_count, vent_count, paired_size));
        error_count++;
    }

    if (is_reserved) {
        int reserved_v6_last;
        if (max_v6_entries == 0) {
            reserved_v6_last = -1;
        }else {
#ifdef BCM_UTT_SUPPORT
            /*
             * Get start of unreserved index.
             */
            if (soc_feature(u, soc_feature_utt)) {
                reserved_v6_last = 0;
                SOC_IF_ERROR_RETURN
                    (soc_utt_lpm_scaled_unreserved_start_index(u,
                                                               &reserved_v6_last));
                reserved_v6_last--;
            } else
#endif
            {
                reserved_v6_last = ((max_v6_entries - 1) / tcam_depth) * tcam_depth * 2 +
                         ((max_v6_entries - 1) % tcam_depth);
            }
        }
        for (i = 0; i < (reserved_v6_last + 1); i++) {
            if (LPM128_IN_ODD_TCAM_FIRST(u, i)) {
                offset = soc_fb_lpm_paired_offset(u, i);
                i += offset;
            }
            offset = soc_fb_lpm_paired_offset(u, i);
            if (pfxmap[i] != -1 ||
                pfxmap[i + offset] != -1) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "overflow into reserved space pfx: %d\n"),
                                      pfxmap[i]));
                error_count++;
            }
        }

        for (; i < paired_size; i++) {
            if (LPM128_IN_ODD_TCAM_FIRST(u, i) &&
                (reserved_v6_last != -1) &&
                (LPM128_TCAM_NUM(u, i) - 1 == LPM128_TCAM_NUM(u, reserved_v6_last))) {

#ifdef BCM_UTT_SUPPORT
                /*
                 * Add delta of v6 entries reserved in last lookup.
                 * Difference between logical tcam start addr of unreserved section
                 * and unreserved start index will give num v6 entries reserved in
                 * last lookup.
                 */
                if (soc_feature(u, soc_feature_utt)) {
                    l_tcam_start =
                        soc_utt_lpm128_logical_tcam_start_index(u,
                                                                (reserved_v6_last + 1));
                    i += (reserved_v6_last + 1 - l_tcam_start);
                } else
#endif
                {
                    i += (reserved_v6_last + 1) % tcam_depth;
                }
            }

            if (pfxmap2[i] != SOC_LPM128_INDEX_TO_PFX_GROUP(u, i) ||
                pfxmap[i]  == -1) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Reserverd: index missmatch: %d "
                                      "  pfxmap %d, pfxmap2 %d, pfx %d \n"),
                                      i, pfxmap[i], pfxmap2[i],
                                      SOC_LPM128_INDEX_TO_PFX_GROUP(u, i)));
                error_count++;
            }
        }
    } else {
        for (i = 0; i < paired_size; i++) {
            if (pfxmap2[i] != SOC_LPM128_INDEX_TO_PFX_GROUP(u, i) ||
                pfxmap[i]  == -1) {
                LOG_ERROR(BSL_LS_SOC_LPM,
                          (BSL_META_U(u,
                                      "Unreserverd: index missmatch: %d "
                                      "  pfxmap %d, pfxmap2 %d, pfx %d \n"),
                                      i, pfxmap[i], pfxmap2[i],
                                      SOC_LPM128_INDEX_TO_PFX_GROUP(u, i)));
                error_count++;
            }

            /* The following does sanity check on lpm128 hash table, with the 
             * help from lpm128 state (say it's trustable) and a little help
             * from cached l3_defip 
             */
            if (pfxmap2[i] != -1 && SOC_LPM128_PFX_IS_V6(u, pfxmap2[i])) {
                /* Valid v6 */
                if (LPM128_IN_EVEN_TCAM(u,i)) {
                    /* left must exist in hash table, right must not exist */
                    if (SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)] == 0 ||
                        SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]) {
                        error_count++;
                        LOG_ERROR(BSL_LS_SOC_LPM,
                                  (BSL_META_U(u,
                                              "Unreserverd: check table error0: %d "
                                              "  pfx %d, check_table %d %d \n"),
                                              i, pfxmap2[i],
                                              SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)],
                                              SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]));
                    }
                } else {
                    /* both must not exist in hash table */
                    if (SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)] ||
                        SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]) {
                        error_count++;
                        LOG_ERROR(BSL_LS_SOC_LPM,
                                  (BSL_META_U(u,
                                              "Unreserverd: check table error1: %d "
                                              "  pfx %d, check_table %d %d \n"),
                                              i, pfxmap2[i],
                                              SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)],
                                              SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]));
                    }
                }
            } else if (pfxmap2[i] != -1 && SOC_LPM128_PFX_IS_V4(u, pfxmap2[i])) {
                /* Valid v4 */
                /* left half must exist in hash table */
                if (SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)] == 0) {
                    error_count++;
                    LOG_ERROR(BSL_LS_SOC_LPM,
                              (BSL_META_U(u,
                                          "Unreserverd: check table error2: %d "
                                          "  pfx %d, check_table %d \n"),
                                          i, pfxmap2[i],
                                          SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)]));
                }

                /* right half must exist in hash table when VALID1 is 1 */
                if (SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1] == 0) {
                    /* further check if it's half entry, report error if not half entry */
                    if (soc_L3_DEFIPm_field32_get(u, &(DBG_SOC_L3_DEFIP(u)[i]), VALID1f)) {
                        error_count++;
                        LOG_ERROR(BSL_LS_SOC_LPM,
                                  (BSL_META_U(u,
                                              "Unreserverd: check table error3: %d "
                                              "  pfx %d, check_table %d \n"),
                                              i, pfxmap2[i],
                                              SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]));
                    }
                }
            } else {
                /* Not valid entry, must not exist in hash table */
                if (SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)] ||
                    SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]) {
                    error_count++;
                    LOG_ERROR(BSL_LS_SOC_LPM,
                              (BSL_META_U(u,
                                          "Unreserverd: check table error4: %d "
                                          "  pfx %d, check_table %d %d \n"),
                                          i, pfxmap2[i],
                                          SOC_LPM128_STATE_HASH(u)->check_table[(i << 1)],
                                          SOC_LPM128_STATE_HASH(u)->check_table[(i << 1) + 1]));
                }
            }
        }
    }

    (void) lpm_state_verify(u, &error_count);
/* Parse for every operation is just too time intensive. */
/*
    soc_fb_lpm_debug_parse(u, 0, &error_count);
    soc_fb_lpm_hash_sanity(u, 0, &error_count); */

done:
    sal_free(pfxmap);
    sal_free(pfxmap2);

    if (error_count) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "Total ERROR: %d \n"),
                              error_count));
        soc_fb_lpm_debug_collect(u);
        return SOC_E_INTERNAL;
    }
    
    if (SOC_LPM_STATE(u)) {
        sal_memcpy(DBG_SOC_LPM_STATE(u),
                   SOC_LPM_STATE(u),
                   pfx_state_size2);
    }

    if (SOC_LPM128_STATE(u)) {
        sal_memcpy(DBG_SOC_LPM128_STATE(u),
                   SOC_LPM128_STATE(u),
                   pfx_state_size);
    }

    if (SOC_LPM128_UNRESERVED_STATE(u)) {
        sal_memcpy(DBG_SOC_LPM128_UNRESERVED_STATE(u),
                   SOC_LPM128_UNRESERVED_STATE(u),
                   pfx_state_size);
    }

    if (SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u)) {
        sal_memcpy(DBG_SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u),
                   SOC_LPM128_INDEX_TO_PFX_GROUP_TABLE(u),
                   (paired_size * sizeof(int)));
    }

    if (SOC_LPM_STATE_HASH(u)) {
        sal_memcpy(DBG_SOC_LPM_STATE_HASH(u)->table, 
                   SOC_LPM_STATE_HASH(u)->table, 
                   _fb_lpm_hash_tab[u]->index_count * sizeof(*(_fb_lpm_hash_tab[u]->table)));
        sal_memcpy(DBG_SOC_LPM_STATE_HASH(u)->link_table, 
                   SOC_LPM_STATE_HASH(u)->link_table, 
                   _fb_lpm_hash_tab[u]->entry_count * sizeof(*(_fb_lpm_hash_tab[u]->link_table)));
    }

    if (SOC_LPM128_STATE_HASH(u)) {
        sal_memcpy(DBG_SOC_LPM128_STATE_HASH(u)->table, 
                   SOC_LPM128_STATE_HASH(u)->table, 
                   _fb_lpm128_hash_tab[u]->index_count * sizeof(*(_fb_lpm128_hash_tab[u]->table)));
        sal_memcpy(DBG_SOC_LPM128_STATE_HASH(u)->link_table, 
                   SOC_LPM128_STATE_HASH(u)->link_table, 
                   _fb_lpm128_hash_tab[u]->entry_count * sizeof(*(_fb_lpm128_hash_tab[u]->link_table)));
    }

    SOC_LPM128_STAT_128BV6_COUNT1(u)          =  SOC_LPM128_STAT_128BV6_COUNT(u);
    SOC_LPM128_STAT_V4_COUNT1(u)              =  SOC_LPM128_STAT_V4_COUNT(u);
    SOC_LPM128_STAT_64BV6_COUNT1(u)           =  SOC_LPM128_STAT_64BV6_COUNT(u);
    SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT1(u)   =  SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
    SOC_LPM_V4_COUNT1(u)                      =  SOC_LPM_V4_COUNT(u);
    SOC_LPM_64BV6_COUNT1(u)                   =  SOC_LPM_64BV6_COUNT(u);
    SOC_LPM_V4_HALF_ENTRY_COUNT1(u)           =  SOC_LPM_V4_HALF_ENTRY_COUNT(u);
    
    return SOC_E_NONE;
}
#endif


STATIC int
_lpm128_smallest_v6_prefix(int u)
{
    int pfx;
    int smallest_pfx = -1;
    soc_lpm128_state_p lpm_state_ptr;

    pfx = MAX_PFX128_INDEX;
    lpm_state_ptr = SOC_LPM128_STATE(u);
    while(lpm_state_ptr && pfx != -1) {
        pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
        if ((pfx == -1) || SOC_LPM128_PFX_IS_V4(u, pfx)) {
            break;
        }

        /* This happens when a v6 group is about to freed, its free
         * entries are now available for next v4 to occupy.
         * This also happens when a v6 group is created just now,
         * in that case skip it and continue to find.
         */
        if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
            continue;
        }

        smallest_pfx = pfx;
    }

    pfx = MAX_PFX128_INDEX;
    lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    while(lpm_state_ptr && pfx != -1) {
        pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
        if ((pfx == -1) || SOC_LPM128_PFX_IS_V4(u, pfx)) {
            break;
        }

        /* This happens when a v6 group is about to freed, its free
         * entries are now available for next v4 to occupy.
         * This also happens when a v6 group is created just now,
         * in that case skip it and continue to find.
         */
        if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
            continue;
        }

        smallest_pfx = pfx;
    }

    return smallest_pfx;
}

/*
 * Scenario 0:
 * No Reserved or No V6 Pfx exists
 *                    -----------             -----------
 *                    | LTCAM 0 | v4_odd -->  | LTCAM 1 |
 *                    |         | domain      |         |
 *                    |         | start       |         |
 *                    |         |             |         |
 *                    |         |             |         |
 *                    |         |             |         |
 *                    |         |             |         |
 *                    |         |             |         |
 *                    -----------             -----------
 *
 * Scenario 1:
 * Reserved 128B space
 *                    -----------             -----------
 *                    | LTCAM 0 |             | LTCAM 1 |
 *                    |         |             |         |
 *                    |         |             |         |
 *                    |         |             |         |
 *                 ---|---------|-------------|---------|---
 *                    |         | v4_odd -->  |         |
 *                    |         | domain      |         |
 *                    |         | start       |         |
 *                    -----------             -----------
 *
 * Scenario 2:
 * Smallest V6 pfx end and fent are within one logical lookup
 *
 *                    -----------             -----------
 *                    | LTCAM 0 |             | LTCAM 1 |
 *        pfx start   |         |             |         |
 *                 ---|---------|-------------|---------|---
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *        pfx end  |--|---------|-------------|---------|--|
 *                 |  |         |             |         |  |
 *        pfx fent |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 ---|---------|-------------|---------|---
 *                    |         |  v4_odd --> |         |
 *                    |         |  domain     |         |
 *                    |         |  start      |         |
 *                    -----------             -----------
 *
 * Scenario 3:
 * Smallest V6 pfx end and fent are within one logical lookup.
 * Start index in a different logical lookup
 *                    -----------             -----------
 *                    | LTCAM 0 |             | LTCAM 1 |
 *        pfx start   |         |             |         |
 *                 ---|---------|-------------|---------|---
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  -----------             -----------  |
 *                 |                                       |
 *                 |                                       |
 *                 |  -----------             -----------  |
 *                 |  | LTCAM 2 |             | LTCAM 3 |  |
 *        pfx end  ---|---------|-------------|---------|---
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *        pfx fent |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 ---|---------|-------------|---------|---
 *                    |         |  v4_odd --> |         |
 *                    |         |  domain     |         |
 *                    |         |  start      |         |
 *                    -----------             -----------
 *
 * Scenario 4:
 * Smallest V6 pfx end and fent are in different logical lookup.
 *                    -----------             -----------
 *                    | LTCAM 0 |             | LTCAM 1 |
 *        pfx start   |         |             |         |
 *                 ---|---------|-------------|---------|---
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *        pfx end  ---|---------|-------------|---------|---
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 |  -----------             -----------  |
 *                 |                                       |
 *                 |                                       |
 *                 |  -----------             -----------  |
 *                 |  | LTCAM 2 |             | LTCAM 3 |  |
 *                 |  |         |             |         |  |
 *        pfx fent |  |         |             |         |  |
 *                 |  |         |             |         |  |
 *                 ---|---------|-------------|---------|---
 *                    |         |  v4_odd --> |         |
 *                    |         |  domain     |         |
 *                    |         |  start      |         |
 *                    |         |             |         |
 *                    -----------             -----------
 */
STATIC int
_lpm128_v4_domain_odd_start(int u)
{
    int pfx_id = -1;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int fent = 0;
    int end = 0;
    int tcam_count = 0;
    int rem = 0;
    int fent_in_last_tcam = 0;
    int max_128b_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);
    int v4_domain_odd_start;
    int offset = 0;
#ifdef BCM_UTT_SUPPORT
    int l_tcam_start = 0;
#endif

    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        max_128b_entries >>= 1;
    }

    /* Scenario 0 */
    offset = soc_fb_lpm_paired_offset(u, 0);
    v4_domain_odd_start = offset;

    pfx_id = _lpm128_smallest_v6_prefix(u);
    if (pfx_id == -1) {
        if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
#ifdef BCM_UTT_SUPPORT
            /* Scenario 1 */
            if (soc_feature(u, soc_feature_utt)) {
                SOC_IF_ERROR_RETURN
                (soc_utt_lpm_scaled_unreserved_start_index(u,
                                                           &v4_domain_odd_start));
                offset = soc_fb_lpm_paired_offset(u, v4_domain_odd_start);
                v4_domain_odd_start += offset;

            } else
#endif
            {
                v4_domain_odd_start = (max_128b_entries / tcam_depth) * tcam_depth * 2 +
                                      (max_128b_entries % tcam_depth);
                v4_domain_odd_start += tcam_depth;
            }
        }
    } else {
        if (SOC_LPM128_UNRESERVED_STATE(u)) {
            lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
            if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx_id) == -1) {
                lpm_state_ptr = SOC_LPM128_STATE(u);
            }
        }

        end  = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx_id);
        fent = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx_id);
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(u, soc_feature_utt)) {
            offset = soc_fb_lpm_paired_offset(u, end + 1);
            l_tcam_start = soc_utt_lpm128_logical_tcam_start_index(u, end + 1);
            rem = offset - (end + 1 - l_tcam_start);
            if (rem == offset) {
                rem = 0;
            }
        } else
#endif
        {
            rem = tcam_depth - (end + 1) % tcam_depth;
            if (tcam_depth == rem) {
                rem  = 0;
            }
        }

        /*
         * during the middle of conflicting process, free slot move up
         * could result fent into 1, but that entry should still be
         * considered as part of v4 domain.
         */

        if (fent == 1) {
            fent = 0;
        }

        /* Scenario 4: Free entries are spanning across lookups   */
        if (fent >= 2  * rem) {
#ifdef BCM_UTT_SUPPORT
            if (soc_feature(u, soc_feature_utt)) {
                SOC_IF_ERROR_RETURN
                    (soc_utt_lpm128_multiple_lookup_v4_domain_odd_start(u, end + 1,
                                                                        fent, rem ,
                                                                        &v4_domain_odd_start));
            } else
#endif
            {
                fent -= (2 * rem);
                tcam_count = (end + rem + 1) / tcam_depth;;
                if (tcam_count % 2) {
                    tcam_count++;
                }

                fent_in_last_tcam = (fent / 2) % tcam_depth;
                fent -= (2 * fent_in_last_tcam);
                tcam_count += (fent / tcam_depth);
                v4_domain_odd_start = ((tcam_count + 1) * tcam_depth) + fent_in_last_tcam;
            }
        } else {
            /* Scenario 2 or 3 */
            offset = soc_fb_lpm_paired_offset(u, end + 1);
            v4_domain_odd_start = end + offset + (fent / 2) + 1;
        }

    }

    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(u,
                          "V4 DOMAIN START %d \n"), v4_domain_odd_start));
    return v4_domain_odd_start;
}

STATIC INLINE int
_lpm128_next_index(int u, int pfx,  /* pfx that index belongs to */
                   int index)
{
    int next_idx = 0;
    int offset = 0;
    int v4_domain_start;

    next_idx = index + 1;
    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        if (LPM128_IN_ODD_TCAM_FIRST(u, next_idx)) {
            v4_domain_start = _lpm128_v4_domain_odd_start(u);
            if (LPM128_TCAM_NUM(u, next_idx) ==
                LPM128_TCAM_NUM(u, v4_domain_start)) {
                next_idx = v4_domain_start;
            }
        }
    } else if (SOC_LPM128_PFX_IS_V6(u, pfx)) {
        if (LPM128_IN_ODD_TCAM(u, next_idx)) {
            offset = soc_fb_lpm_paired_offset(u, next_idx);
            next_idx += offset;
        }
    }

    return next_idx;
}


STATIC INLINE int
_lpm128_prev_index(int u, int pfx,  /* pfx that index belongs to */
                   soc_lpm128_state_p lpm_state_ptr,
                   int index)
{
    int prev_pfx;
    int prev_idx;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int v4_domain_start;
    int is_v4 = 0;
    int offset = 0;

    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx);
        if (prev_pfx == -1  ||
            SOC_LPM128_PFX_IS_V4(u, prev_pfx) ||
            index > SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)) {
            is_v4 = 1;
        }

        /* This happens when a v6 group is about to freed, its free
         * entries are now available for v4 to occupy.
         */
        if (prev_pfx != -1 &&
            SOC_LPM128_STATE_VENT(u, lpm_state_ptr, prev_pfx) == 0) {
            is_v4 = 1;
        }
    }

    prev_idx = index - 1;
    if (is_v4) {
        v4_domain_start = _lpm128_v4_domain_odd_start(u);
        if (LPM128_TCAM_NUM(u, v4_domain_start) ==
            LPM128_TCAM_NUM(u, prev_idx) &&
            prev_idx < v4_domain_start) {
#ifdef BCM_UTT_SUPPORT
            if (soc_feature(u, soc_feature_utt)) {
                prev_idx =
                    soc_utt_lpm128_logical_tcam_start_index(u, prev_idx) - 1;
            } else
#endif
            {
                prev_idx = LPM128_TCAM_NUM(u, prev_idx) * tcam_depth - 1;
            }
        }
    } else {
        if (LPM128_IN_ODD_TCAM(u, prev_idx)) {
            offset = soc_fb_lpm_paired_offset(u, prev_idx);
            prev_idx -= offset;
        }
    }

    return prev_idx;
}

/*
 *  Copy (or move if erase set) entry from from_ent to to_ent.
 */
STATIC int
_lpm128_fb_entry_shift(int u, int pfx,
                       int from_ent, void *fe, void *feupr,
                       int to_ent, int erase)
{
    uint32 v0 = 0;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32      eupr[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         offset = 0;
    int         rv = SOC_E_NONE;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    if (from_ent == to_ent) {
        return SOC_E_NONE;
    }

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                       MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */

    if (fe) {
        sal_memcpy(e, fe, soc_mem_entry_words(u,mem) * 4);
    } else {
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e);
        SOC_IF_ERROR_RETURN(rv);
    }
    if (SOC_LPM128_PFX_IS_V6(u, pfx)) {

        if (feupr) {
            sal_memcpy(eupr, feupr, soc_mem_entry_words(u, mem) * 4);
        } else {
            offset = soc_fb_lpm_paired_offset(u, from_ent);
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                           from_ent + offset, eupr);
            SOC_IF_ERROR_RETURN(rv);
        }
        LPM128_HASH_INSERT(u, e, eupr, to_ent, &rvt_index0, &rvt_index1);
        /* Disable valid bit at to_ent + offset so that packets dont
         * get forwared to some transient addresses
         */
        v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, eupr);
        if (v0) {
            SOC_MEM_OPT_F32_VALID0_SET(u, mem, eupr, 0);
            offset = soc_fb_lpm_paired_offset(u, to_ent);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                                to_ent + offset, eupr));
        }
        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e);
        if (v0) {
            SOC_MEM_OPT_F32_VALID0_SET(u, mem, eupr, v0);
        }
        if (rv >= 0) {
            offset = soc_fb_lpm_paired_offset(u, to_ent);
            rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent + offset, eupr);
        }

        if (rv >= 0) {
        /* coverity[copy_paste_error : FALSE] */
            rv = _lpm128_fb_urpf_entry_replicate(u, to_ent, -1, e, eupr);
        }
        if (rv < 0) {
            LPM128_HASH_REVERT(u, e, eupr, to_ent, rvt_index0, rvt_index1);
            return rv;
        }
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, to_ent + offset) = pfx;

    } else {
        LPM128_HASH_INSERT(u, e, NULL, to_ent, &rvt_index0, &rvt_index1);
        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e);
        if (rv >= 0) {
            rv = _lpm128_fb_urpf_entry_replicate(u, to_ent, -1, e, NULL);
        }
        if (rv < 0) {
            LPM128_HASH_REVERT(u, e, NULL, to_ent, rvt_index0, rvt_index1);
            return rv;
        }
    }

    SOC_LPM128_INDEX_TO_PFX_GROUP(u, to_ent) = pfx;

    if (erase) {
        sal_memcpy(e, soc_mem_entry_null(u, mem),
                   soc_mem_entry_words(u,mem) * 4);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e));
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, from_ent) = -1;
        if (SOC_LPM128_PFX_IS_V6(u, pfx)) {
            offset = soc_fb_lpm_paired_offset(u, from_ent);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                from_ent + offset, e));
            SOC_LPM128_INDEX_TO_PFX_GROUP(u, from_ent + offset) = -1;
            SOC_IF_ERROR_RETURN(_lpm128_fb_urpf_entry_replicate(u, from_ent, -1,
                                                                e, e));
        } else {
            SOC_IF_ERROR_RETURN(_lpm128_fb_urpf_entry_replicate(u, from_ent, -1,
                                                                e, NULL));
        }
    }
    return (SOC_E_NONE);
}

STATIC int
_lpm128_fb_shift_v4_pfx_up(int u, soc_lpm128_state_p lpm_state_ptr, int pfx)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         end;
    int         end_next;
    int         end_cross = 0;
    int         start1;
    int         start1_next;
    int         start1_cross;
    int         from_end2;
    uint32      v0, v1;
    int         half_entry;

    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif


    start1 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
        end = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx);
        from_end2 = 1;
    } else {
        end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
        from_end2 = 0;
    }
    end_next    = _lpm128_next_index(u, pfx, end);
    start1_next = _lpm128_next_index(u, pfx, start1);
    end_cross    = (end_next - end) > 1;
    start1_cross = (start1_next - start1) > 1;

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                 MEM_BLOCK_ANY, end));
#endif /* FB_LPM_TABLE_CACHED */
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end, e);
    SOC_IF_ERROR_RETURN(rv);
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

    half_entry = (v0 == 0 || v1 == 0) &&
                 (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) > 1) && 
                 !soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity);

    if (half_entry) {
        /* Last entry is half full -> keep it last. */
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx, end, e, NULL,
                                                   end_next, 0));
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx, start1, NULL, NULL,
                                                   end, 0));
    } else {
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx, start1, NULL, NULL,
                                                   end_next, 0));
    }

    /* No need to erase start1 entry, as it will be done by caller. */

    if (from_end2) {
        assert (!end_cross);
        if (start1_cross) {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)  = start1_next;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)    = end_next;
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx)  = -1;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)    = -1;
        } else {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)  = start1_next;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)    = end_next;
        }
    } else {
        if (end_cross) {
            if (start1_cross) {
                assert (start1_next == end_next);
                SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)  = start1_next;
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)    = end_next;
            } else {
                SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)  = start1_next;
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx)  = end_next;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)    = end_next;
            }
        } else {
            assert (!start1_cross);
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx)  = start1_next;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)    = end_next;
        }
    }
    return SOC_E_NONE;
}

/*
 *      Shift prefix entries 1 entry UP, while preserving
 *      last half empty IPv4 entry if any.
 */
STATIC int
_lpm128_fb_shift_pfx_up(int u, soc_lpm128_state_p lpm_state_ptr, int pfx)
{
    int         start;
    int         start_next;
    int         end;
    int         end_next;

    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        return _lpm128_fb_shift_v4_pfx_up(u, lpm_state_ptr, pfx);
    }

    start = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
    end   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    end_next   = _lpm128_next_index(u, pfx, end);
    start_next = _lpm128_next_index(u, pfx, start);

    SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx, start, NULL, NULL,
                                               end_next, 0));

    SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start_next;
    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_next;
    return (SOC_E_NONE);
}


STATIC int
_lpm128_fb_shift_v4_pfx_down(int u, soc_lpm128_state_p lpm_state_ptr, int pfx)
{
    int         end;
    int         end_prev;
    int         end_cross;
    int         from_end2;
    int         v0, v1;
    int         prev_pfx = 0;
    int         start1;
    int         start1_cross = 0;
    int         start1_prev = -1;
    int         half_entry;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         erase;
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif


    prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx);
    if (prev_pfx == -1) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "v4 move down: previous pfx is -1 for pfx: %d\n"), pfx));
        return SOC_E_INTERNAL;
    }

    start1 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
        from_end2 = 1;
        end   = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx);
    } else {
        from_end2 = 0;
        end   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    }

    end_prev    = _lpm128_prev_index(u, pfx, lpm_state_ptr, end);
    start1_prev = _lpm128_prev_index(u, pfx, lpm_state_ptr, start1);

    end_cross    = (end - end_prev) > 1;
    start1_cross = (start1 - start1_prev) > 1;

    /* Check if vent is 0. Then no need to move, just update start and end */
    /* This case is hit if the prefix group is created just now */
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)   = start1_prev - 1;
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, start1_prev) = pfx;
        return SOC_E_NONE;
    }

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                 MEM_BLOCK_ANY, end));
#endif /* FB_LPM_TABLE_CACHED */
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end, e);
    SOC_IF_ERROR_RETURN(rv);
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

    half_entry = (v0 == 0 || v1 == 0) &&
                 (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) > 1) && 
                  !soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity);

    /*
     * When this v4_pfx's end crosses tcam boundary, we need to erase 
     * immediately instead of leaving it to a further write. Because a further
     * write can only overwrite one entry, but for a end cross entry, we are 
     * actually moving 2 entries. This could happen on both directions:
     * Giving v6 free entries to v4 or giving v4 free entries to v6.
     * When pfx is V4 and prev_pfx is V6, and V6 needs to give away index, we
     * also need to erase immediately.
     */
    erase = end_cross || (SOC_LPM128_PFX_IS_V6(u, prev_pfx));
    if (half_entry == 0) {
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx,
                                                   end, e, NULL,
                                                   start1_prev, erase));
    } else {
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx,
                                                   end_prev, NULL, NULL,
                                                   start1_prev, 0));

        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx,
                                                   end, e, NULL,
                                                   end_prev, erase));
    }

    if (from_end2) {
        assert (!start1_cross);
        if (end_cross) {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = -1;
        } else {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = end_prev;
        }
    } else {
        if (start1_cross) {
            if (end_cross) {
                assert (start1_prev == end_prev);
                SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)   = end_prev;
            } else {
                SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)   = start1_prev;
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = start1;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = end_prev;
            }
        } else {
            assert (!end_cross);
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start1_prev;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)   = end_prev;
        }
    }
    return SOC_E_NONE;
}

/*
 *      Shift prefix entries 1 entry DOWN, while preserving
 *      last half empty IPv4 entry if any.
 */
STATIC int
_lpm128_fb_shift_pfx_down(int u, soc_lpm128_state_p lpm_state_ptr, int pfx)
{
    int         end;
    int         end_prev;
    int         start;
    int         start_prev;

    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        return _lpm128_fb_shift_v4_pfx_down(u, lpm_state_ptr, pfx);
    }

    start = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
    end   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    start_prev = _lpm128_prev_index(u, pfx, lpm_state_ptr, start);
    end_prev   = _lpm128_prev_index(u, pfx, lpm_state_ptr, end);
    /* Don't move empty prefix .
     * This condition is hit if pfx is created for the first time
     */
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start_prev;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = start_prev - 1;
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, start_prev) = pfx;
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, pfx, end, NULL, NULL,
                                               start_prev, 0));

    SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start_prev;
    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx)   = end_prev;

    return (SOC_E_NONE);
}

STATIC int
_lpm128_free_slot_move_up(int u, int pfx, int free_pfx,
                          soc_lpm128_state_p lpm_state_ptr);
STATIC int
_lpm128_free_slot_move_down(int u, int pfx, int free_pfx,
                            soc_lpm128_state_p lpm_state_ptr);

STATIC int
_lpm128_move_v4_entry_away_for_v6(int u,
                                  int v6_pfx,     /* last v6 pfx */
                                  int v6_domain_end,
                                  soc_lpm128_state_p lpm_state_ptr)
{
    int free_pfx    = -1;
    int start_ent   = -1;
    int end_ent     = -1;
    int new_end     = -1;
    int from_end2   = 0;
    int v4_odd_start;      /* index of v4 entry that should be given to v6_pfx */
    int v4_pfx = -1;       /* v4 pfx other index belongs to */
    int it_pfx;
    int entry_count;
    int half_entry = 0;
    uint32  e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32  v0 = 0, v1 = 0;
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;
    int offset = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif


    offset = soc_fb_lpm_paired_offset(u, v6_domain_end);
    v4_odd_start = v6_domain_end + offset;
    if (LPM128_IN_EVEN_TCAM(u, v4_odd_start)) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "creating free slot: other_idx: %d is in even tcam "\
                              " v6_pfx: %d\n"),
                   v4_odd_start, v6_pfx));
        return SOC_E_INTERNAL;
    }

    /*
     * v4_odd_start is empty, simply swap the fent
     */
    if (SOC_LPM128_INDEX_TO_PFX_GROUP(u, v4_odd_start) == -1) {
        it_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, v6_pfx);
        v4_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, v6_pfx);

        while (it_pfx != -1) {
            if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, it_pfx) >
                v4_odd_start) {
                break;
            }
            v4_pfx = it_pfx;
            it_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, it_pfx);
        }
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v4_pfx) -= 1;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx) += 1;
        return SOC_E_NONE;
    }

    v4_pfx = SOC_LPM128_INDEX_TO_PFX_GROUP(u, v4_odd_start);
    assert (SOC_LPM128_PFX_IS_V4(u, v4_pfx));
    free_pfx = v4_pfx;
    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
        free_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx);
        if (free_pfx == -1) {
            free_pfx = v4_pfx;
            break;
        }
    }

    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
       free_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, free_pfx);
       if (free_pfx == -1) {
           return SOC_E_FULL;
       }
    }

    if (free_pfx >= v6_pfx) {
        LOG_ERROR(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,
                              "moving v4 entry to create space for V6: v6_pfx: %d "\
                              "free_pfx: %d v4_pfx: %d\n"), v6_pfx, free_pfx,
                   v4_pfx));
        return SOC_E_FULL;
    }

    if (free_pfx < v4_pfx) {
        SOC_IF_ERROR_RETURN(_lpm128_free_slot_move_up(u, v4_pfx, free_pfx,
                                                      lpm_state_ptr));
    }

    if (free_pfx > v4_pfx) {
        SOC_IF_ERROR_RETURN(_lpm128_free_slot_move_down(u, v4_pfx, free_pfx,
                                                        lpm_state_ptr));
    }

    if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, v4_pfx) != -1) {
        start_ent = SOC_LPM128_STATE_START2(u, lpm_state_ptr, v4_pfx);
        end_ent   = SOC_LPM128_STATE_END2(u, lpm_state_ptr, v4_pfx);
        from_end2 = 1;
    } else {
        start_ent = SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx);
        end_ent   = SOC_LPM128_STATE_END1(u, lpm_state_ptr, v4_pfx);
        from_end2 = 0;
    }

    /* During free_slot_move_down, v4_odd_start could be moved to
     * to create free space. In this case, job done.
     * And no need to erase, since the goal is to create free slot
     * for ipv6 to occupy.
     */
    if (SOC_LPM128_INDEX_TO_PFX_GROUP(u, v4_odd_start) == -1) {
        assert (free_pfx > v4_pfx && end_ent < v4_odd_start);
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v4_pfx) -= 1;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx) += 1;

        return SOC_E_NONE;
    }

    new_end = _lpm128_next_index(u, v4_pfx, end_ent);

#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                 MEM_BLOCK_ANY, end_ent));
#endif /* FB_LPM_TABLE_CACHED */
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end_ent, e);
    SOC_IF_ERROR_RETURN(rv);
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
    
    entry_count = end_ent - v4_odd_start + 1;
    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        half_entry = (v0 == 0 || v1 == 0) && (entry_count > 1);
    }
    if (half_entry == 0)  {
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, v4_pfx,
                                                   v4_odd_start, NULL, NULL,
                                                   new_end, 0));
    } else {
       /* Last entry is half full -> keep it last. */
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, v4_pfx,
                                                   end_ent, e, NULL,
                                                   new_end, 0));
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, v4_pfx,
                                                   v4_odd_start, NULL, NULL,
                                                   end_ent, 0));
    }

    if (from_end2 == 0) {
        if (start_ent == v4_odd_start) {
            SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx) += 1;
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, v4_pfx) += 1;
        } else {
            /* must split */
            assert (start_ent < v4_odd_start && end_ent >= v4_odd_start);
            assert (LPM128_TCAM_NUM(u, start_ent) <
                    LPM128_TCAM_NUM(u, end_ent));
            assert (LPM128_IN_ODD_TCAM_FIRST(u, v4_odd_start));

            SOC_LPM128_STATE_END1(u, lpm_state_ptr, v4_pfx) = v4_odd_start - 1;
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, v4_pfx) = v4_odd_start + 1;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, v4_pfx) = end_ent + 1;
        }
    } else {
        assert (start_ent == v4_odd_start);
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, v4_pfx) += 1;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, v4_pfx) += 1;
    }

    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v4_pfx) -= 1;
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx) += 1;

    return SOC_E_NONE;
}


STATIC int
_lpm128_move_v4_entry_down_for_v6(int u, int free_cnt,
                                  int start_index,
                                  int other_pfx,
                                  soc_lpm128_state_p lpm_state_ptr)
{
    int start;
    int end = -1;
    int entry_count = 0;
    int from, to;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32      v0 = 0, v1 = 0;
    int half_entry = 0;
    int erase;
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif


    start = SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx);
    end   = SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx);
    if (end == -1) {
        return SOC_E_INTERNAL;
    }
    entry_count = end - start + 1;
#ifdef FB_LPM_TABLE_CACHED
    SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                 MEM_BLOCK_ANY, end));
#endif /* FB_LPM_TABLE_CACHED */
    rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end, e);
    SOC_IF_ERROR_RETURN(rv);
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        half_entry = (v0 == 0 || v1 == 0) && (entry_count > 1);
    }
    if (half_entry) {
        from = end - 1;
    } else {
        from = end;
    }
    to = start_index;

    /* Normally do erase, unless half_entry can overwrite */
    erase = !half_entry || start != (to + 1);

    while (from >= start && to < start) {
        SOC_IF_ERROR_RETURN(_lpm128_fb_entry_shift(u, other_pfx, from, NULL, NULL,
                                                   to, erase));
        from--;
        to++;
    }

    if (half_entry) {
        if (to < start) {
            SOC_IF_ERROR_RETURN
            (_lpm128_fb_entry_shift(u, other_pfx, end, e, NULL,
                                    to, 1));
        } else {
            from++;
            SOC_IF_ERROR_RETURN
            (_lpm128_fb_entry_shift(u, other_pfx, end, e, NULL,
                                    from, 1));
        }
    }

    SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) -= free_cnt;
    SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx) -= free_cnt;

    if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) ==
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, other_pfx) + 1) {
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, other_pfx) =
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx);
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) = -1;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx) = -1;
    }

    return SOC_E_NONE;
}

STATIC int
_lpm128_move_next_pfx_down_during_delete(int u, int pfx,
                                         soc_lpm128_state_p lpm_state_ptr)
{
    int end = -1;
    int from_end2 = 0;
    int half_entry = 0;
    uint32 v0, v1;
    int rv;
    int end_prev = 0;
    int end1_index = -1;
    int end2_index = -1;
    int start2_index = -1;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int fent2 = 0;
    int range2_move = 0;
    int bottom_pfx = 0;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int from, to, prev_to, prev_from;
    int other_start;
    int other_pfx = 0;
    int other_start_v4;
    int vent;
    int fent_start = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
    int fent = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx);
    int next_pfx =  SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
    int start   = -1;
    int v4_pfx  = -1;
    int erase;
    soc_mem_t mem = L3_DEFIPm;
    int offset = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif


    /* take care of deleting 64BV6 prefix group previous to V4 prefix group */
    if (_lpm128_pfx_conflicting(next_pfx, pfx)) {
        v4_pfx = next_pfx;

        other_start_v4 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx);
        offset = soc_fb_lpm_paired_offset(u, other_start_v4);
        other_start_v4 = other_start_v4 + offset;
        other_pfx    = v4_pfx;
        bottom_pfx   = v4_pfx;
        while (other_pfx != -1) {
            if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, other_pfx) >=
                other_start_v4 ||
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) >=
                other_start_v4) {
                break;
            }
            bottom_pfx = other_pfx;
            other_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, other_pfx);
        }

        range2_move = other_pfx != -1 &&
                      SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx) != -1;

        /*
         * V6 pfx to be delete and
         * V4 pfx starts in the same logical TCAM
         */
        if (LPM128_TCAM_NUM(u, fent_start) ==
            LPM128_TCAM_NUM(u, SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx))) {
            fent2 = fent >> 1;
            offset = soc_fb_lpm_paired_offset(u, fent_start);
            other_start = fent_start + offset;
        } else {
            /*
             * V6 pfx to be delete and
             * V4 pfx starts are in different logical TCAM's
             * V6 free entries are span across logical TCAM's
             */
#ifdef BCM_UTT_SUPPORT
            if (soc_feature(u, soc_feature_utt)) {
                SOC_IF_ERROR_RETURN
                    (soc_utt_lpm128_move_v4_down_for_v6_get_fent(u,
                                                          fent_start,
                                                          fent,
                                                          &fent2));
                other_start = SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx);
                other_start  = soc_utt_lpm128_logical_tcam_start_index(u,
                                                                       other_start);
            } else
#endif
            {
                fent2 = ((fent >> 1) + fent_start) % tcam_depth;
                other_start = SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) /
                              tcam_depth * tcam_depth;
            }
        }

        if (range2_move) {
            /* move other_pfx to make contiguous */
            SOC_IF_ERROR_RETURN(
                _lpm128_move_v4_entry_down_for_v6(u, fent2,
                    other_start, other_pfx, lpm_state_ptr));
        }
    }

    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx) != -1) {
        end = SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx);
        from_end2 = 1;
    } else {
        end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx);
        from_end2 = 0;
    }
    vent = SOC_LPM128_STATE_VENT(u, lpm_state_ptr, next_pfx);
    end_prev = _lpm128_prev_index(u, next_pfx, lpm_state_ptr, end);

#ifdef FB_LPM_TABLE_CACHED
    if ((rv = soc_mem_cache_invalidate(u, mem,
                                       MEM_BLOCK_ANY, end)) < 0) {
        return rv;
    }
#endif /* FB_LPM_TABLE_CACHED */
    if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end, e)) < 0) {
        return rv;
    }
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        half_entry = (v0 == 0 || v1 == 0) && (vent > 1);
    }
    if (half_entry) {
        from = end_prev;
    } else {
        from = end;
    }
    to   = fent_start;
    prev_to = to;

    start = SOC_LPM128_STATE_START1(u, lpm_state_ptr, next_pfx);

    /* Normally do erase, unless half_entry can overwrite */
    erase = !half_entry || start != _lpm128_next_index(u, next_pfx, to);

    while (from >= start && to < start) {
        SOC_IF_ERROR_RETURN
            (_lpm128_fb_entry_shift(u, next_pfx, from, NULL, NULL, to, erase));

        if ((SOC_LPM128_PFX_IS_V4(u, next_pfx) && (to - prev_to > 1) &&
            start2_index == -1)) {
            start2_index = to;
        }

        if (start2_index != -1) {
            end2_index = to;
        } else {
            end1_index = to;
        }

        prev_to = to;

        from = _lpm128_prev_index(u, next_pfx, lpm_state_ptr, from);
        to   = _lpm128_next_index(u, next_pfx, to);
    }

    if (half_entry) {
        if (to < start) {
            SOC_IF_ERROR_RETURN
                (_lpm128_fb_entry_shift(u, next_pfx, end, NULL, NULL, to, 1));
            if ((SOC_LPM128_PFX_IS_V4(u, next_pfx) && (to - prev_to > 1) &&
                start2_index == -1)) {
                start2_index = to;
            }
            if (start2_index != -1) {
                end2_index = to;
            } else {
                end1_index = to;
            }
        } else {
            prev_from = from;
            from = _lpm128_next_index(u, next_pfx, from);
            SOC_IF_ERROR_RETURN
                (_lpm128_fb_entry_shift(u, next_pfx, end, NULL, NULL, from, 1));
            if ((SOC_LPM128_PFX_IS_V4(u, next_pfx) && (from - prev_from > 1) &&
                start2_index == -1)) {
                start2_index = from;
            }
        }
    }

    if (to < SOC_LPM128_STATE_START1(u, lpm_state_ptr, next_pfx)) {
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx) = end1_index;
        SOC_LPM128_STATE_START2(u, lpm_state_ptr, next_pfx) = start2_index;
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx) = end2_index;
    } else {
        if (from_end2) {
            if (from > SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx)) {
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx) = from;
            } else {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx) = from;
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, next_pfx) = -1;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx) = -1;
            }
        } else {
            if (start2_index != -1) {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx) = end1_index;
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, next_pfx) = start2_index;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, next_pfx) = from;
            } else {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, next_pfx) = from;
            }
        }
    }
    SOC_LPM128_STATE_START1(u, lpm_state_ptr, next_pfx) = fent_start;
    if (_lpm128_pfx_conflicting(next_pfx, pfx)) {
        if (range2_move) {
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, other_pfx) += fent2;
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v4_pfx) += fent - fent2;
        } else {
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, bottom_pfx) += fent2;
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v4_pfx) += fent - fent2;
        }
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, next_pfx) += fent;
    }

    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) = 0;
    return SOC_E_NONE;
}


STATIC int
_lpm128_assign_free_fent_from_v6_to_v4(int u, int v6_pfx, int new_v4_pfx,
                                       soc_lpm128_state_p lpm_state_ptr)
{
    int v6_fent = 0;
    int bottom_pfx = -1;
    int other_start = -1;
    int fent2 = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int v4_pfx;
    int fent_start;
    int other_start_v4;
    int other_pfx;
    int range2_move;
    int offset = 0;

    v4_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, new_v4_pfx);
    assert (_lpm128_pfx_conflicting(new_v4_pfx, v6_pfx));
    assert (v4_pfx != -1);

    v6_fent    = SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx);
    fent_start = SOC_LPM128_STATE_END1(u, lpm_state_ptr, v6_pfx);
    fent_start = _lpm128_next_index(u, v6_pfx, fent_start);

    other_start_v4 = SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx);
    offset = soc_fb_lpm_paired_offset(u, other_start_v4);
    other_start_v4 = other_start_v4 + offset;
    other_pfx    = v4_pfx;
    bottom_pfx   = v4_pfx;
    while (other_pfx != -1) {
        if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, other_pfx) >=
            other_start_v4 ||
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) >=
            other_start_v4) {
            break;
        }
        bottom_pfx = other_pfx;
        other_pfx  = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, other_pfx);
    }

    range2_move = other_pfx != -1 &&
                  SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx) != -1;

    if (LPM128_TCAM_NUM(u, fent_start) ==
        LPM128_TCAM_NUM(u, SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx))) {
        fent2 = v6_fent >> 1;
        offset = soc_fb_lpm_paired_offset(u, fent_start);
        other_start = fent_start + offset;
    } else {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(u, soc_feature_utt)) {
            SOC_IF_ERROR_RETURN
                (soc_utt_lpm128_move_v4_down_for_v6_get_fent(u,
                                                             fent_start,
                                                             v6_fent,
                                                             &fent2));
            other_start = SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx);
            other_start  = soc_utt_lpm128_logical_tcam_start_index(u,
                                                                   other_start);
        } else
#endif
        {
            fent2 = ((v6_fent >> 1) + fent_start) % tcam_depth;
            other_start = SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) /
                          tcam_depth * tcam_depth;
        }
    }

    if (range2_move) {
        /* move other_pfx to make contiguous */
        SOC_IF_ERROR_RETURN(
            _lpm128_move_v4_entry_down_for_v6(u, fent2,
                other_start, other_pfx, lpm_state_ptr));
    }

    if (range2_move) {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, new_v4_pfx) += v6_fent - fent2;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, other_pfx)  += fent2;
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, new_v4_pfx) += v6_fent - fent2;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, bottom_pfx) += fent2;
    }

    return SOC_E_NONE;
}

STATIC int
_lpm128_free_slot_move_down(int u, int pfx, int free_pfx,
                            soc_lpm128_state_p lpm_state_ptr)
{
    int next_pfx = -1;
    int fent_incr = 0;
    int other_start_v4;
    int other_start;
    int v4_start;
    int other_pfx;
    int bottom_pfx;
    int range2_move;
    int dest_pfx = -1;
    int offset = 0;

    while (free_pfx > pfx) {
        /* copy start of prev_pfx to start - 1 */
        next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx);
        SOC_IF_ERROR_RETURN(_lpm128_fb_shift_pfx_down(u, lpm_state_ptr,
                                                      next_pfx));
        if (SOC_LPM128_PFX_IS_V6(u, next_pfx)) {
            fent_incr = 2; /* V6 to V6 */
        } else {
            fent_incr = 1; /* V4 to V6, V4 to V4 */
        }

        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) -= fent_incr;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, next_pfx) += fent_incr;

        if (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) &&
            _lpm128_pfx_conflicting(next_pfx, free_pfx)) {
            /* V4 to V6 case
             * next_pfx is v4 and got a free index for V6.
             * Since each V6 uses 2 slots, give away the other slot
             *
             * starting from search_index, till start_index search to get the
             * dest_pfx. Following cases are possible
             * 1. if the dest_pfx has start2 then other_index should be equal
             *    to start2 - 1. then simply copy end2 to start2 -1 and
             *    decrement start2 and end2 , increase fent for dest_pfx
             * 2. if start2 for dest_pfx is -1. Then simply increase fent
                  for dest pfx
             *  |  v6       |  v6    |
             *  |  v4 START |  v6    |  Occupy empty v6 to become a v4 entry
             *  |  v4       |  v4    |
             */
            int v4_pfx = next_pfx;
            int v6_pfx = free_pfx;

            assert(SOC_LPM128_PFX_IS_V4(u, pfx));
            v4_start = SOC_LPM128_STATE_START1(u, lpm_state_ptr, v4_pfx);
            offset = soc_fb_lpm_paired_offset(u, v4_start);
            other_start = v4_start + offset;
            other_start_v4 = other_start + 1;
            if (LPM128_IN_EVEN_TCAM_FIRST(u, other_start_v4)) {
                /* This is a corner case where v4_start is the last index in
                 * odd tcam, and other_start_v4 becomes the first in even tcam
                 * (so is the START2 of v4_pfx). In this case, simply do
                 * nothing, and things will work just fine.
                 */
            }

            other_pfx  = v4_pfx;
            bottom_pfx = v4_pfx;
            while (other_pfx != -1) {
                if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, other_pfx) >=
                    other_start_v4 ||
                    SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx) >=
                    other_start_v4) {
                    break;
                }
                bottom_pfx = other_pfx;
                other_pfx  = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, other_pfx);
            }

            range2_move = other_pfx != -1 &&
                          SOC_LPM128_STATE_END2(u, lpm_state_ptr, other_pfx) != -1;

            if (range2_move) {
                assert (other_start_v4 == other_start + 1);
                assert (other_start_v4 ==
                        SOC_LPM128_STATE_START2(u, lpm_state_ptr, other_pfx));
                SOC_IF_ERROR_RETURN(_lpm128_move_v4_entry_down_for_v6(u, 1,
                    other_start, other_pfx, lpm_state_ptr));
            }
            if (range2_move) {
                SOC_LPM128_STATE_FENT(u, lpm_state_ptr, other_pfx) += 1;
                SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx) -= 1;
                dest_pfx = other_pfx;
            } else {
                SOC_LPM128_STATE_FENT(u, lpm_state_ptr, bottom_pfx) += 1;
                SOC_LPM128_STATE_FENT(u, lpm_state_ptr, v6_pfx) -= 1;
                SOC_LPM128_INDEX_TO_PFX_GROUP(u, other_start) = -1;
                dest_pfx = bottom_pfx;
            }
        }

        if (dest_pfx != -1 && dest_pfx >= pfx) {
            free_pfx = dest_pfx;
            dest_pfx = -1;
        } else {
            free_pfx = next_pfx;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_lpm128_free_slot_move_up(int u, int pfx, int free_pfx,
                          soc_lpm128_state_p lpm_state_ptr)
{
    int prev_pfx = -1;
    int prev_last;
    int fent_incr = 0;

    while (free_pfx < pfx) {
        SOC_IF_ERROR_RETURN(_lpm128_fb_shift_pfx_up(u, lpm_state_ptr,
                                                    free_pfx));
        prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, free_pfx);

        if (SOC_LPM128_PFX_IS_V4(u, free_pfx)) {
            fent_incr = 1; /* V4 to V4, V6 to V4 case*/
        } else {
            fent_incr = 2; /* V6 to V6 case*/
        }

        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) -= fent_incr;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, prev_pfx) += fent_incr;

        if (_lpm128_pfx_conflicting(free_pfx, prev_pfx)) {
            /*
             * prev_pfx is V6 and free_pfx is V4.
             * At this stage there could be 1 entry free in V4. But we need
             * 2 entries to move V6 at index n and n + tcam_depth.
             * n is already free. so now find the prefix of n + tcam_depth and move
             * entries of that prefix to find free space
             *  |  v6 END|  v6    |
             *  |  v6    |  v4    |  move v4 away to become a v6 free entry.
             *  |  v4    |  v4    |
             */
            assert(SOC_LPM128_PFX_IS_V6(u, pfx));
            prev_last = _lpm128_next_index(u, prev_pfx,
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, prev_pfx));

            SOC_IF_ERROR_RETURN(_lpm128_move_v4_entry_away_for_v6(u, prev_pfx,
                                        prev_last, lpm_state_ptr));
        }
        free_pfx = prev_pfx;
    }
    return SOC_E_NONE;
}


STATIC INLINE int
_lpm128_pfx_group_create(int u, int pfx, soc_lpm128_state_p lpm_state_ptr)
{
    int start_index = -1;
    int next_pfx = -1;
    int prev_pfx = -1;

    /* Find prev and next pfx */
    prev_pfx = MAX_PFX128_INDEX;
    while (SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx) > pfx) {
        prev_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx);
    }
    next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx);

    /* Setup the entry in the linked list */
    if (next_pfx != -1) {
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx) = pfx;
    }
    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx) = next_pfx;
    SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx) = prev_pfx;
    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx) = pfx;

    if (next_pfx != -1 && _lpm128_pfx_conflicting(pfx, prev_pfx)) {
        if (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, prev_pfx)) {
            SOC_IF_ERROR_RETURN
            (_lpm128_assign_free_fent_from_v6_to_v4(u, prev_pfx, pfx,
                                                    lpm_state_ptr));
        }
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) =
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, prev_pfx);
    }
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, prev_pfx) = 0;

    if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, prev_pfx) != -1) {
        start_index = SOC_LPM128_STATE_END2(u, lpm_state_ptr, prev_pfx);
    } else {
        start_index = SOC_LPM128_STATE_END1(u, lpm_state_ptr, prev_pfx);
    }

    start_index = _lpm128_next_index(u, prev_pfx, start_index);
    SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = start_index;
    SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = start_index - 1;
    SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) = 0;

    return SOC_E_NONE;
}


STATIC int
_lpm128_pfx_group_destroy(int u, int pfx, soc_lpm128_state_p lpm_state_ptr )
{

    int         prev_pfx;
    int         next_pfx;

    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) != 0) {
        return SOC_E_NONE;
    }

    /* remove from the list */
    prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx);
    /* prev_pfx Always present */
    assert(prev_pfx != -1);
    next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);

    if (next_pfx != -1 && (_lpm128_pfx_conflicting(pfx, prev_pfx) ||
                           prev_pfx == MAX_PFX128_INDEX)) {
        SOC_IF_ERROR_RETURN
            (_lpm128_move_next_pfx_down_during_delete(u, pfx, lpm_state_ptr));
    } else {
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, prev_pfx) +=
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx);
    }

    if (next_pfx != -1) {
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx) = prev_pfx;
    }
    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx) = next_pfx;
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) = 0;
    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = -1;
    SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = -1;

    return SOC_E_NONE;
}



STATIC int
_lpm128_v4_half_entry_available(int u, soc_lpm128_state_p lpm_state_ptr,
                                int pfx, void *e, int *free_slot)
{
    int from_ent;
    uint32 v0, v1;
    int rv;
    soc_mem_t mem = L3_DEFIPm;

    if (SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) == 0) {
       return SOC_E_FULL;
    }
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
        from_ent = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx);
    } else {
        from_ent = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    }

    if (from_ent == -1) {
        return SOC_E_INTERNAL;
    }

#ifdef FB_LPM_TABLE_CACHED
    if ((rv = soc_mem_cache_invalidate(u, mem,
                                       MEM_BLOCK_ANY, from_ent)) < 0) {
        return rv;
    }
#endif /* FB_LPM_TABLE_CACHED */
    if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e)) < 0) {
        return rv;
    }
    v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
    v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);

    if ((v0 == 0) || (v1 == 0)) {
        *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
        return(SOC_E_NONE);
    }
    sal_memcpy(e, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u, mem) * 4);
    return SOC_E_FULL; /* To indicate that both the entries are in use*/
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
STATIC int
_lpm128_v4_free_slot_create(int u, int pfx, void *e, int *free_slot)
{
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         rv;
    int         end;
    int         end_next = -1;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);


    if (SOC_LPM128_UNRESERVED_STATE(u)) {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    }

    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        rv = _lpm128_pfx_group_create(u, pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    } else {
        /* Check if half of the entry is free. if yes add to it */
        rv = _lpm128_v4_half_entry_available(u, lpm_state_ptr,
                                             pfx, e, free_slot);
        if (SOC_SUCCESS(rv)) {
            SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) = 0;
            return SOC_E_NONE;
        } else if (rv != SOC_E_FULL) {
            return rv;
        }
    }

    free_pfx = pfx;
    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
        free_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on right side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
        free_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on left side as well */
            rv = SOC_E_FULL;
            goto cleanup;
        }
    }


    /*
     * Ripple entries to create free space for v4 pfx.
     */
    if (free_pfx < pfx) {
        rv = _lpm128_free_slot_move_up(u, pfx, free_pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }


    /*
     * Ripple entries to create free space for v4 pfx.
     */
    if (free_pfx > pfx) {
        rv = _lpm128_free_slot_move_down(u, pfx, free_pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }

    if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) == -1) {
        end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
        end_next = _lpm128_next_index(u, pfx, end);
        if (end_next - end > 1) {
            SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = end_next;
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = end_next;
        } else {
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_next;
        }
    } else {
        SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) += 1;
    }

    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) += 1;
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) -= 1;
    SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) = 
        soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity) ? 0 : 1;

    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
        *free_slot = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) << 1;
    } else {
        *free_slot = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) << 1;
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        *free_slot += 1;
    }
#endif
    return(SOC_E_NONE);

cleanup:
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        /* failed to allocate entries for a newly allocated prefix.*/
        prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx);
        next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
        if (-1 != prev_pfx) {
            SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx) =
                next_pfx;
        }
        if (-1 != next_pfx) {
            SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx) =
            prev_pfx;
        }
    }
    return rv;
}



/*
 *      Create a slot for the new entry rippling the entries if required
 */
STATIC int
_lpm128_free_slot_create(int u, int pfx, void *e, int *free_slot)
{
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx, free_pfx2;
    int         old_end;
    int         new_end;
    int         rv;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);

    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        return _lpm128_v4_free_slot_create(u, pfx, e, free_slot);
    }

    if (!SOC_LPM128_PFX_IS_V6_128(u, pfx)  && SOC_LPM128_UNRESERVED_STATE(u)) {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    }

    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        rv = _lpm128_pfx_group_create(u, pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }

    free_pfx = pfx;

    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
        free_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on left side */
            free_pfx = pfx;
            break;
        }
    }

    while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 0) {
        free_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on right side as well */
            rv = SOC_E_FULL;
            goto cleanup;
        }
    }

    /* For ipv6, need 2 free entries */
    if (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx) == 1) {
        free_pfx2 = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx);
        while (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, free_pfx2) == 0) {
            free_pfx2 = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, free_pfx2);
            if (free_pfx2 == -1) {
                /* No more free entries */
                rv = SOC_E_FULL;
                goto cleanup;
            }
        }
    }

    /*
     * Ripple entries to create free space for v6 pfx.
     */
    if (free_pfx > pfx) {
        rv = _lpm128_free_slot_move_down(u, pfx, free_pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }

    if (free_pfx < pfx) {
        rv = _lpm128_free_slot_move_up(u, pfx, free_pfx, lpm_state_ptr);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }

    old_end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    new_end = _lpm128_next_index(u, pfx, old_end);

    assert ((SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) >= 2) &&
            (SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) & 1) == 0);

    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = new_end;
    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) += 1;
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) -= 2;

    *free_slot = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    return(SOC_E_NONE);

cleanup:
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        /* failed to allocate entries for a newly allocated prefix.*/
        prev_pfx = SOC_LPM128_STATE_PREV(u, lpm_state_ptr, pfx);
        next_pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
        if (-1 != prev_pfx) {
            SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_pfx) =
                next_pfx;
        }
        if (-1 != next_pfx) {
            SOC_LPM128_STATE_PREV(u, lpm_state_ptr, next_pfx) =
            prev_pfx;
        }
    }
    return rv;
}


STATIC int
_lpm128_v4_free_slot_delete(int u, int pfx, soc_lpm128_state_p lpm_state_ptr,
                            void *key_data, void *e, int slot, int *is_deleted)
{
    int         end;
    int         end_prev;
    int         end_cross, end_cross_prev = 0;
    int         to_ent;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS]={0};
    void        *et;
    int         rv;
    int         from_end2;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    int         temp_index = -1;
    int         temp_hw_index = -1;
    int         half_entry_free = 0;
    soc_mem_t   mem = L3_DEFIPm;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

  
    *is_deleted = 0; 
    to_ent = slot;
    to_ent >>= 1;

    if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
        end = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx);
        from_end2 = 1;
    } else {
        end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
        from_end2 = 0;
    }

    half_entry_free = SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx);

    /*
     * Allocate Reserved entry.
     */ 
    if (soc_feature(u, soc_feature_lpm_atomic_write)) {
        if (((to_ent == end) && (half_entry_free == 0)) ||
             (to_ent != end)) {
            SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) = 0;

            rv = _lpm128_free_slot_create(u, pfx, ef, &temp_index);
            if (SOC_FAILURE(rv)) {
               return rv;
            }

            /* Increment Half entry count, since temp slot creates it. */
            SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));

            temp_hw_index = temp_index >> 1;

            rv = _soc_fb_lpm128_match(u, key_data, NULL, NULL, NULL,
                                      &slot, &pfx, NULL);
            if (SOC_FAILURE(rv)) {
               return rv;
            }

            to_ent = slot;
            to_ent >>= 1;

            if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
               end = SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx);
               from_end2 = 1;
            } else {
               end = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
               from_end2 = 0;
            }
        } 
    }

    /* Delete the HASH before updating the HW */
    if (key_data != NULL) {
       LPM128_HASH_DELETE(u, key_data, NULL, slot);
    }

    end_prev = _lpm128_prev_index(u, pfx, lpm_state_ptr, end);
    end_cross = (end - end_prev) > 1;

    if (temp_index != -1) {
       end = end_prev;
       end_prev = _lpm128_prev_index(u, pfx, lpm_state_ptr, end);
       end_cross_prev = (end - end_prev) > 1;
    }

#ifdef FB_LPM_TABLE_CACHED
    if ((rv = soc_mem_cache_invalidate(u, mem,
                                       MEM_BLOCK_ANY, end)) < 0) {
        return rv;
    }
#endif /* FB_LPM_TABLE_CACHED */
    if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, end, ef)) < 0) {
        return rv;
    }
    et =  (to_ent == end) ? ef : e;
    if (SOC_MEM_OPT_F32_VALID1_GET(u, mem, ef)) {
        if (slot & 1) {
            (void) soc_fb_lpm_ip4entry1_to_1(u, ef, et, PRESERVE_HIT);
        } else {
            (void) soc_fb_lpm_ip4entry1_to_0(u, ef, et, PRESERVE_HIT);
        }
        SOC_MEM_OPT_F32_VALID1_SET(u, mem, ef, 0);
#ifdef BCM_TOMAHAWK2_SUPPORT        
        if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
            /* Reset the end entry */
            sal_memcpy(ef, soc_mem_entry_null(u, mem),
                       soc_mem_entry_words(u,mem) * 4);
            /* Remaining half entry got deleted. Decrement half entry count.*/
            SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) -= 1;
            SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) += 1;
            SOC_LPM128_INDEX_TO_PFX_GROUP(u, end) = -1;
            *is_deleted = 1;
            if (from_end2) {
                if (end_cross) {
                    SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = -1;
                    SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
                    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_prev;
                } else {
                    SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = end_prev;
                }
            } else {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_prev;
            }
        } else 
#endif
        {
            /* first half of full entry is deleted, generating half entry.*/
            SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
        }
    } else {
        if (slot & 1) {
            (void) soc_fb_lpm_ip4entry0_to_1(u, ef, et, PRESERVE_HIT);
        } else {
            (void) soc_fb_lpm_ip4entry0_to_0(u, ef, et, PRESERVE_HIT);
        }

        /* Reset the end entry */
        sal_memcpy(ef, soc_mem_entry_null(u, mem),
                   soc_mem_entry_words(u,mem) * 4);
        /* Remaining half entry got deleted. Decrement half entry count.*/
        SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
        SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) -= 1;
        SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) += 1;
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, end) = -1;
        *is_deleted = 1;
        if (from_end2) {
            if (end_cross) {
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = -1;
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_prev;
            } else {
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = end_prev;
            }
        } else {
            SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end_prev;
        }
    }
            
    if (to_ent != end) {
        if (temp_index != -1) {
           rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, et);
           if (SOC_FAILURE(rv)) {
              return rv;
           }
        }

        LPM128_HASH_INSERT(u, et, NULL, to_ent, &rvt_index0, &rvt_index1);
        if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, et)) < 0) {
            LPM128_HASH_REVERT(u, et, NULL, to_ent, rvt_index0, rvt_index1);
            return rv;
        }

        rv = _lpm128_fb_urpf_entry_replicate(u, to_ent, temp_hw_index, et, NULL);
        if (SOC_FAILURE(rv)) {
            LPM128_HASH_REVERT(u, et, NULL, to_ent, rvt_index0, rvt_index1);
            return rv;
        }
    }

    if ((temp_index != -1) && (*is_deleted == 0)) {
       rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, ef);
       if (SOC_FAILURE(rv)) {
          return rv;
       }
    }

    LPM128_HASH_INSERT(u, ef, NULL, end, &rvt_index0, &rvt_index1);
    if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, end, ef)) < 0) {
        LPM128_HASH_REVERT(u, ef, NULL, end, rvt_index0, rvt_index1);
        return rv;
    }

    rv = _lpm128_fb_urpf_entry_replicate(u, end, temp_hw_index, ef, NULL);
    if (SOC_FAILURE(rv)) {
        LPM128_HASH_REVERT(u, ef, NULL, end, rvt_index0, rvt_index1);
        return rv;
    }

    /* Remove reserved entry */
    if (temp_index != -1) {
       /* Delete temp slot */
       SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) -= 1;
       SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) += 1;

       if (*is_deleted == 0) {
          if (from_end2) {
              if (end_cross && (end_cross_prev == 0)) {
                 SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx)   = -1;
                 SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = -1;
                 SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end;
              } else {
                 SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = end;
              }
          } else {
             SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = end;
          }
       }
            
       /* Decrement Half entry count, since temp slot creates it. */
       if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
           SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
       }

       /* Clear reserved entry */ 
       sal_memcpy(ef, soc_mem_entry_null(u, mem),
                   soc_mem_entry_words(u,mem) * 4);
       rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, temp_hw_index, ef);
       SOC_IF_ERROR_RETURN(rv);
    }

    if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        /* Update half free entry */
        SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) = (half_entry_free) ? 0 : 1;
    }
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_IF_ERROR_RETURN
        (_lpm128_pfx_group_destroy(u, pfx, lpm_state_ptr));
    }

    return(rv);
}


/*
 *      Delete a slot and adjust entry pointers if required.
 *      e - has the contents of entry at slot(useful for IPV4 only)
 */
STATIC int
_lpm128_free_slot_delete(int u, int pfx, void *key_data,
                         void *e, int slot, int *is_deleted)
{
    int         from_ent;
    int         to_ent;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    uint32      efupr[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;
    int fent_incr = 1;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    soc_mem_t mem = L3_DEFIPm;
    int offset = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    if (!SOC_LPM128_PFX_IS_V6_128(u, pfx)  && SOC_LPM128_UNRESERVED_STATE(u)) {
       lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    }

    if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
        return _lpm128_v4_free_slot_delete(u, pfx, lpm_state_ptr, key_data, e,
                                           slot, is_deleted);
    }

    fent_incr = 2;
    from_ent  = SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx);
    to_ent    = slot;
    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) -= 1;
    SOC_LPM128_STATE_FENT(u, lpm_state_ptr, pfx) += fent_incr;
    SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) -= 1;
    if (LPM128_IN_ODD_TCAM(u, SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx))) {
        offset = soc_fb_lpm_paired_offset(u,
                        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx));
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) -= offset;
    }

    if (to_ent != from_ent) {
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef);
        SOC_IF_ERROR_RETURN(rv);
        offset = soc_fb_lpm_paired_offset(u, from_ent);
        rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                           from_ent + offset, efupr);
        SOC_IF_ERROR_RETURN(rv);
        LPM128_HASH_INSERT(u, ef, efupr, to_ent, &rvt_index0, &rvt_index1);

        rv = (WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, ef));
        if (rv >= 0) {
            offset = soc_fb_lpm_paired_offset(u, to_ent);
            rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent + offset, efupr);
        }
        if (rv >= 0) {
            rv = _lpm128_fb_urpf_entry_replicate(u, to_ent, -1, ef, efupr);
        }
        if (rv < 0) {
            LPM128_HASH_REVERT(u, ef, efupr, to_ent, rvt_index0, rvt_index1);
            return rv;
        }
    }

    sal_memcpy(ef, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u,mem) * 4);
    sal_memcpy(efupr, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u,mem) * 4);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef));
    SOC_LPM128_INDEX_TO_PFX_GROUP(u, from_ent) = -1;
    offset = soc_fb_lpm_paired_offset(u, from_ent);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent + offset,
                                        efupr));
    SOC_LPM128_INDEX_TO_PFX_GROUP(u, from_ent + offset) = -1;
    SOC_IF_ERROR_RETURN(_lpm128_fb_urpf_entry_replicate(u, from_ent, -1, ef,
                                                        efupr));
    *is_deleted = 1;
    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_IF_ERROR_RETURN
        (_lpm128_pfx_group_destroy(u, pfx, lpm_state_ptr));
    }

    return(rv);
}



/*
 * Function:
 *    _bcm_fb_route_scale_check
 * Purpose:
 *    Check Route scale in DEFIP tables.
 * Parameters:
 *    unit    - (IN) SOC unit number;
 * Return:
 *    TRUE - Scale Reached
 *    False - Scale not reached
 */
STATIC int
_bcm_fb_route_scale_check(int unit, int half_entry)
{
    int rpf_scale = 1;
    int total_routes_added = 0;
    int v4_count = 0;
    rpf_scale = (SOC_URPF_STATUS_GET(unit) ? 1: 2);
    /* In case of Ranger2Plus, we have enabled more tcams
     * but number of routes that should be added in those tcams
     * are less. Moreover traditional LPM state can not
     * scale to exact numbers which are needed for this device.
     */
    if (soc_feature(unit, soc_feature_defip_2_tcams_with_separate_rpf)) {
        /*
         * Check for total scale if that is reached.
         * we are trying the exact scale that is mentioned
         * in device sheet and not how much it takes in hardware.
         */
        v4_count = SOC_LPM128_STAT_V4_COUNT(unit) -
                       SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(unit);
        /*
         * In case this prefix has half entry, then we can add one v4 route.
         */
        v4_count = v4_count - ((half_entry) ? 1 : 0) +
                       (SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(unit) * 2);

        total_routes_added = v4_count +
                             (SOC_LPM128_STAT_64BV6_COUNT(unit) * 2) +
                             (SOC_LPM128_STAT_128BV6_COUNT(unit) * 4);
        if (total_routes_added >= (R2P_V4_MAX * rpf_scale)) {
            return TRUE;
        }
    }
    return FALSE;
}


STATIC int
soc_fb_lpm128_can_insert_entry(int u, soc_lpm_entry_type_t type)
{
    int free_count = 0;

    if (type == socLpmEntryTypeV4) {
        SOC_IF_ERROR_RETURN(soc_lpm_free_v4_route_get(u, &free_count));
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
            free_count /= 2;
        }
#endif
    } else if (type == socLpmEntryType64BV6) {
        SOC_IF_ERROR_RETURN(soc_lpm_free_64bv6_route_get(u, &free_count));
    } else if (type == socLpmEntryType128BV6) {
        SOC_IF_ERROR_RETURN(soc_lpm_free_128bv6_route_get(u, &free_count));
    }
    if (free_count == 0) {
        return FALSE;
    }

    return TRUE;
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
 * soc_fb_lpm128_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
soc_fb_lpm128_insert(int u, void *entry_data, void *entry_data1,
                     int *hw_index)
{
    int         pfx;
    int         index, copy_index;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] =  {0};
    uint32      e1[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int         rv = SOC_E_NONE;
    int         found = 0;
    soc_lpm_entry_type_t type = socLpmEntryType128BV6;
    int         offset = 0;
    uint32      rvt_index0 = 0, rvt_index1 = 0;
    int           temp_index = -1;
    uint32 defip_temp_e[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem  = L3_DEFIP_LEVEL1m;
    }
#endif

    sal_memcpy(e, soc_mem_entry_null(u, mem),
               soc_mem_entry_words(u,mem) * 4);

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm128_match(u, entry_data, entry_data1, e, e1,
                              &index, &pfx, &type);

    if (rv == SOC_E_NOT_FOUND) {
        soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);

        if (SOC_LPM128_UNRESERVED_STATE(u)) {
           lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
        }

        /*
         * Validate free entries in V4 space. In case of atomic write,
         * one entry is reserved in V4 space (two v4 routes).
         * Insert should not be allowed more than (max_v4_count - 2).
         */
        if (soc_feature(u, soc_feature_lpm_atomic_write) &&
            soc_fb_lpm128_is_v4_64b_allowed_in_paired_tcam(u) &&
            (SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx) == 0)) {

            int max_entries, used_entries, reserved_entries;

            /* Used v4 l3_defip entries */
            used_entries = SOC_LPM128_STAT_V4_COUNT(u) - SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
            used_entries = ((used_entries + 1) >> 1) + SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
            
            /* Used l3_defip entries for (v6_64  + v6_128) */
            used_entries += ((SOC_LPM128_STAT_64BV6_COUNT(u) * 2) +
                             (SOC_LPM128_STAT_128BV6_COUNT(u) * 2));

            /* Max number of l3_defip entries */            
            max_entries = SOC_LPM128_STAT_MAX_128BV6_COUNT(u) * 2;

            /* Number of reserved entries */
            reserved_entries = (type == socLpmEntryTypeV4) ? 1 : 2;

            /* Reserve one paired entry to moves entries during updates */
            if (used_entries >= (max_entries - reserved_entries)) {
                SOC_LPM_UNLOCK(u);
                return SOC_E_FULL;
            }
        } 

        if (_bcm_fb_route_scale_check(u,
            SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, pfx))) {
            SOC_LPM_UNLOCK(u);
            return SOC_E_FULL;
        }

        if (!soc_fb_lpm128_can_insert_entry(u, type)) {
            SOC_LPM_UNLOCK(u);
            return SOC_E_FULL;
        }

        /* Will modify e if it's v4 and half entry available */
        rv = _lpm128_free_slot_create(u, pfx, e, &index);
        if (rv < 0) {
            SOC_LPM_UNLOCK(u);
            return(rv);
        }

        if (soc_feature(u, soc_feature_lpm_atomic_write) &&
            (type == socLpmEntryTypeV4) && (index & 1)) {
           rv = _lpm128_free_slot_create(u, pfx, &defip_temp_e, &temp_index);
           if (SOC_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_SOC_LPM, (BSL_META_U(u,
                     "Temporary v6 backup entry slot create failure: %d,"
                     "original_index:%d temp_index:%d pfx:%d\n\r"),
                      rv, index, temp_index, pfx));
              SOC_LPM_UNLOCK(u);
              return rv;
           }
           /* Increment Half entry count, since temp slot creates one. */
           SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));

           /*
            * Search again for the entry, it could have moved around as a part
            * of temp slot create.
            */ 
           rv = _soc_fb_lpm128_match(u, e, NULL, NULL, NULL,
                                      &index, &pfx, NULL);
           if (SOC_FAILURE(rv)) {
              return rv;
           }
           /*
            * Increment the entry to point to the second new half since the
            * search returns base index.
            */
           index += 1;

           /* Copy index to temp_index */
           rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, (index >> 1), &defip_temp_e);
           if (SOC_FAILURE(rv)) {
              goto cleanup; 
           }

           rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, (temp_index >> 1), &defip_temp_e);
           if (SOC_FAILURE(rv)) {
              goto cleanup; 
           }
        }
    } else {
        found = 1;
    }

    copy_index = index;
    if (rv == SOC_E_NONE) {
        /* Entry already present. Update the entry */
        if (type == socLpmEntryTypeV4) {
            /* IPV4 entry */
            if (index & 1) {
                rv = soc_fb_lpm_ip4entry0_to_1(u, entry_data, e, PRESERVE_HIT);
            } else {
                rv = soc_fb_lpm_ip4entry0_to_0(u, entry_data, e, PRESERVE_HIT);
            }

            if (rv < 0) {
                SOC_LPM_UNLOCK(u);
                return(rv);
            }

            entry_data = (void *)e;
            index >>= 1;
        }

        if (!found) {
            LPM128_HASH_INSERT(u, entry_data, entry_data1, index, &rvt_index0,
                               &rvt_index1);
            if (type == socLpmEntryTypeV4) {
                SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_COUNT(u));
                if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                    /* indexes should change for new entry only */
                    if (copy_index & 1) {
                        SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
                    } else {
                        SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
                    }
                }
            }

            if (type == socLpmEntryType64BV6) {
                SOC_LPM_COUNT_INC(SOC_LPM128_STAT_64BV6_COUNT(u));
            }

            if (type == socLpmEntryType128BV6) {
                SOC_LPM_COUNT_INC(SOC_LPM128_STAT_128BV6_COUNT(u));
            }
        } else {
            LOG_INFO(BSL_LS_SOC_LPM, (BSL_META_U(u,
                     "\nsoc_fb_lpm128_insert: %d %d ENTRY ALREADY PRESENT\n"),
                      index, pfx));
        }
        rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, entry_data);
        if (rv >= 0) {
            if (type != socLpmEntryTypeV4) {
                offset = soc_fb_lpm_paired_offset(u, index);
                rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index + offset,
                                     entry_data1);
                if (rv >= 0) {
                    rv = _lpm128_fb_urpf_entry_replicate(u, index, -1, entry_data,
                                                         entry_data1);
                }
                if (rv >= 0) {
                    SOC_LPM128_INDEX_TO_PFX_GROUP(u, index + offset) = pfx;
                }
            } else {
                rv = _lpm128_fb_urpf_entry_replicate(u, index,
                       ((temp_index == -1) ? -1 : (temp_index >> 1)),
                       entry_data, NULL);
            }
            if (rv >= 0) {
                SOC_LPM128_INDEX_TO_PFX_GROUP(u, index) = pfx;
            }
        }
        if (!found && rv < 0) {
            LPM128_HASH_REVERT(u, entry_data, entry_data1, index,
                               rvt_index0, rvt_index1);
        }
    }

cleanup:
    if (temp_index != -1) {
       int is_deleted;
       rv = _lpm128_free_slot_delete(u, pfx, NULL, &defip_temp_e,
                                     temp_index, &is_deleted);
       if (SOC_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_SOC_LPM, (BSL_META_U(u,
                 "Temporary backup entry slot delete failure: %d,"
                 "original_index:%d temp_index:%d pfx:%d\n\r"),
                  rv, (index << 1), temp_index, pfx));
       }
    }
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_delete
 */
int
soc_fb_lpm128_delete(int u, void *key_data, void* key_data_upr)
{
    int         pfx;
    int         index;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    uint32      eupr[SOC_MAX_MEM_FIELD_WORDS];
    int         rv = SOC_E_NONE;
    soc_lpm_entry_type_t type;
    int is_deleted = 0;

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm128_match(u, key_data, key_data_upr, e, eupr,
                              &index, &pfx, &type);
    if (rv == SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_LPM,
                 (BSL_META_U(u,
                             "\nsoc_fb_lpm128_delete: %d %d\n"),
                             index, pfx));
        if (type != socLpmEntryTypeV4) {
            LPM128_HASH_DELETE(u, key_data, key_data_upr, index);
        }
        rv = _lpm128_free_slot_delete(u, pfx, key_data, e, index, &is_deleted);
        if (rv >= 0) {
            if (type == socLpmEntryTypeV4) {
                SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_V4_COUNT(u));
            }

            if (type == socLpmEntryType64BV6) {
                SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_64BV6_COUNT(u));
            }

            if (type == socLpmEntryType128BV6) {
                SOC_LPM_COUNT_DEC(SOC_LPM128_STAT_128BV6_COUNT(u));
            }
        }

    }
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_match (Exact match for the key. Will match both IP
 *                   address and mask)
 */
int
soc_fb_lpm128_match(int u,
               void *key_data,
               void *key_data_upr,
               void *e,         /* return entry data if found */
               void *eupr,         /* return entry data if found */
               int *index_ptr,
               int *pfx,
               soc_lpm_entry_type_t *type)
{
    int        rv;

    SOC_LPM_LOCK(u);
    rv = _soc_fb_lpm128_match(u, key_data, key_data_upr, e, eupr, index_ptr,
                              pfx, type);
    SOC_LPM_UNLOCK(u);
    return(rv);
}

/*
 * soc_fb_lpm_lookup (LPM search). Masked match.
 */
int
soc_fb_lpm128_lookup(int u,
               void *key_data,
               void *entry_data,
               int *index_ptr)
{
    return(SOC_E_UNAVAIL);
}


int soc_fb_lpm128_get_smallest_movable_prefix(int u, int ipv6, void *e,
                                             void *eupr, int *index,
                                             int *pfx_len, int *count)
{
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);
    int pfx = MAX_PFX128_INDEX;
    int small_pfx = -1;
    int offset = 0;
    int v0, v1;
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif
    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    }

    if (pfx_len == NULL) {
        return SOC_E_PARAM;
    }

    if (lpm_state_ptr == NULL) {
        return SOC_E_NOT_FOUND;
    }

    while (pfx != -1) {
        if (ipv6 && (SOC_LPM128_PFX_IS_V6_64(u, pfx))) {
            small_pfx = pfx;
        }
        if (!ipv6 && (SOC_LPM128_PFX_IS_V4(u, pfx))) {
           small_pfx = pfx;
        }
        pfx = SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, pfx);
    }

    if (small_pfx == -1) {
       return SOC_E_NOT_FOUND;
    }

    if (ipv6) {
        if (index != NULL && e != NULL && eupr != NULL && count != NULL) {
            *index = SOC_LPM128_STATE_END1(u, lpm_state_ptr, small_pfx);
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, *index, e);
            SOC_IF_ERROR_RETURN(rv);
            offset = soc_fb_lpm_paired_offset(u, *index);
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,
                                               (*index + offset), eupr);
            SOC_IF_ERROR_RETURN(rv);
            *count = 1;
        }
    } else {
        if (index != NULL && e != NULL && count != NULL) {
            if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, small_pfx) != -1) {
                *index = SOC_LPM128_STATE_END2(u, lpm_state_ptr, small_pfx);
            } else {
                *index = SOC_LPM128_STATE_END1(u, lpm_state_ptr, small_pfx);
            }
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
               rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, *index, e);
               SOC_IF_ERROR_RETURN(rv);
               *count = 2;
            } else 
#endif
            {
                rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, *index, e);
                SOC_IF_ERROR_RETURN(rv);
                v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
                v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
                if (v0 == 0 || v1 == 0) {
                    *count = 1;
                } else {
                    *count = 2;
                }
            }
        }
    }

    *pfx_len = small_pfx;

    return SOC_E_NONE;
}

int soc_fb_lpm128_is_v4_64b_allowed_in_paired_tcam(int unit)
{
    if (soc_feature(unit, soc_feature_l3_lpm_128b_entries_reserved)) {
        return (SOC_LPM128_UNRESERVED_STATE(unit) != NULL) ? 1 : 0;
    }

    return 1;
}

int
soc_fb_lpm_table_sizes_get(int unit, int *paired_table_size,
                           int *defip_table_size)
{
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int max_paired_count = (SOC_L3_DEFIP_MAX_TCAMS_GET(unit)) / 2;
    int tcam_pair_count = 0;
    int paired_size = 0;
    int unpaired_size = 0;

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_UNAVAIL;
    }

    if (soc_feature(unit, soc_feature_defip_2_tcams_with_separate_rpf)) {
        /* Only 2 tcams are enabled.
         * RPF status does not matter for this size.
         * The RPF route memory is different than these 2 tcams.
         */
        if (paired_table_size != NULL) {
            *paired_table_size = 2048;
        }
        if (defip_table_size != NULL) {
            *defip_table_size  = 0;
        }
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_fb_lpm_tcam_pair_count_get(unit, &tcam_pair_count));
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(unit, soc_feature_utt)) {
            /*
             * Size needs to be in terms of L3_DEFIP view.
             * Each paried entry has 2 entries in L3_DEFIP view, so mul with 2.
             * Unparied entry will be based on urpf status
             */
            tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
            paired_size = tcam_pair_count * tcam_depth;
            paired_size  = paired_size << 1;
            unpaired_size = (max_paired_count - (tcam_pair_count * 2)) * tcam_depth;
        } else
#endif
        {
            switch(tcam_pair_count) {
                case 1:
                case 2:
                    unpaired_size = (max_paired_count - 2) * tcam_depth;
                    paired_size = 2 * tcam_depth;
                    break;
                case 3:
                case 4:
                    unpaired_size = (max_paired_count - 4) * tcam_depth;
                    paired_size = 4 * tcam_depth;
                    break;
                case 5:
                case 6:
                    unpaired_size = (max_paired_count - 6) * tcam_depth;
                    paired_size = 6 * tcam_depth;
                    break;
                case 7:
                case 8:
                    unpaired_size = (max_paired_count - 8) * tcam_depth;
                    paired_size = 8 * tcam_depth;
                    break;
                default:
                    unpaired_size = max_paired_count * tcam_depth;
                    paired_size = 0;
                   break;
            }
        }
    } else {
        unpaired_size = (max_paired_count - tcam_pair_count) * tcam_depth;
        unpaired_size <<= 1;
        paired_size = tcam_pair_count * tcam_depth;
        paired_size <<= 1;
    }

    if (paired_table_size != NULL) {
       *paired_table_size = paired_size;
    }

    if (defip_table_size != NULL) {
       *defip_table_size = unpaired_size;
    }

    return SOC_E_NONE;
}

int soc_lpm_max_v4_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV4_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV4_DEFIPm)) {
        *entries = SOC_EXT_LPM_MAX_V4_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_MAX_V4_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_MAX_V4_COUNT(u);
    }
    return SOC_E_NONE;
}

int soc_lpm_max_64bv6_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if ((SOC_MEM_IS_ENABLED(u, EXT_IPV6_64_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_64_DEFIPm)) ||
        (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm))) {
        *entries = SOC_EXT_LPM_MAX_64BV6_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_MAX_64BV6_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_MAX_64BV6_COUNT(u);
    }
    return SOC_E_NONE;
}

int soc_lpm_max_128bv6_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm)) {
        *entries = SOC_EXT_LPM_MAX_128BV6_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_MAX_128BV6_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_MAX_128BV6_COUNT(u);
    }
    return SOC_E_NONE;
}

int
soc_lpm_free_v4_route_get(int u, int *entries)
{
    int v4_count = 0;
    int b64_count = 0;
    int b128_count = 0;
    int is_reserved = 0;
    int v4_max_count = 0;

    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN(soc_lpm_max_v4_route_get(u, &v4_max_count));
    SOC_IF_ERROR_RETURN(soc_lpm_used_v4_route_get(u, &v4_count));
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV4_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV4_DEFIPm)) {
        *entries = v4_max_count - v4_count;
        return SOC_E_NONE;
    }
#endif
    b64_count = SOC_LPM_64BV6_COUNT(u) << 1;
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        b64_count += (SOC_LPM128_STAT_64BV6_COUNT(u) << 2);
        if (!is_reserved) {
            SOC_IF_ERROR_RETURN(soc_lpm_used_128bv6_route_get(u, &b128_count));
            b128_count <<= 2;
        }
    }
#ifdef BCM_TOMAHAWK2_SUPPORT    
    if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        *entries = v4_max_count - (b64_count + b128_count) - v4_count * 2;
    } else 
#endif    
    {
        *entries = v4_max_count - (b64_count + b128_count) - v4_count;
    }
    return SOC_E_NONE;
}

int
soc_lpm_free_64bv6_route_get(int u, int *entries)
{
    int v4_count = 0;
    int b64_count = 0;
    int b128_count = 0;
    int is_reserved = 0;
    int v64_max_count = 0;
    int paired_v64_max_count = 0;
    int paired_v4_count = 0;
    int paired_tcam_size = 0;
    int paired_v64_count = 0;
    int paired_count = 0;
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);

    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if ((SOC_MEM_IS_ENABLED(u, EXT_IPV6_64_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_64_DEFIPm)) ||
        (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm))) {
        SOC_IF_ERROR_RETURN(soc_lpm_max_64bv6_route_get(u, &v64_max_count));
        SOC_IF_ERROR_RETURN(soc_lpm_used_64bv6_route_get(u, &b64_count));
        *entries = v64_max_count - b64_count;
        return SOC_E_NONE;
    }
#endif
    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(u) &&
        !soc_feature(u, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }
    v64_max_count = SOC_LPM_MAX_64BV6_COUNT(u);
    /*
     * V4 routes may have some half entries. These entries have only
     * one v4 route and other side is empty. For Half entry V4
     * routes, one v4 route occupies one defip index.
     * No need to divide v4 routes in half entries by half.
     */
#ifdef BCM_TOMAHAWK2_SUPPORT     
    if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
        v4_count = SOC_LPM_V4_COUNT(u);
    } else 
#endif
    {
        v4_count = SOC_LPM_V4_COUNT(u) - SOC_LPM_V4_HALF_ENTRY_COUNT(u);
        v4_count = ((v4_count + 1) >> 1) + SOC_LPM_V4_HALF_ENTRY_COUNT(u);
    }
    b64_count = SOC_LPM_64BV6_COUNT(u);
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_tcam_size,
                                                                      NULL));
        if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
            paired_v4_count = SOC_LPM128_STAT_V4_COUNT(u) - SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
            paired_v4_count = ((paired_v4_count + 1) >> 1) + SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
        } else {
            paired_v4_count = SOC_LPM128_STAT_V4_COUNT(u);
        }
        
        paired_v64_count = SOC_LPM128_STAT_64BV6_COUNT(u) << 1;
        paired_v64_max_count = paired_tcam_size;
        if (!is_reserved) {
            SOC_IF_ERROR_RETURN(soc_lpm_used_128bv6_route_get(u, &b128_count));
            b128_count <<= 1;
            paired_count = paired_v64_max_count - paired_v64_count -
                           paired_v4_count - b128_count;
        } else {
            paired_v64_max_count -= (max_v6_entries << 1);
            paired_count = paired_v64_max_count - paired_v4_count -
                           paired_v64_count;
        }
        paired_count >>= 1;
    } else {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(u, soc_feature_utt)) {
            if (SOC_URPF_STATUS_GET(u)) {
                paired_v4_count =
                    ((SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(u) * tcam_depth)
                     - max_v6_entries) << 1;
            } else {
                paired_v4_count =
                    ((SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(u) * tcam_depth)
                     - max_v6_entries) << 1;
            }
        } else
#endif
        if (max_v6_entries % tcam_depth == 0) {
            paired_v4_count = 0;
        } else {
            paired_v4_count = (tcam_depth - (max_v6_entries % tcam_depth)) << 1;
        }
        if (v4_count > paired_v4_count) {
            v4_count -= paired_v4_count;
        } else {
            v4_count = 0;
        }
    }
    *entries = v64_max_count - b64_count - v4_count + paired_count;
    return SOC_E_NONE;
}

int
soc_lpm_free_128bv6_route_get(int u, int *entries)
{
    int v4_count = 0;
    int b64_count = 0;
    int b128_count = 0;
    int is_reserved = 0;
    int v128_max_count = 0;

    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN(soc_lpm_max_128bv6_route_get(u, &v128_max_count));
    SOC_IF_ERROR_RETURN(soc_lpm_used_128bv6_route_get(u, &b128_count));
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm)) {
        *entries = v128_max_count - b128_count;
        return SOC_E_NONE;
    }
#endif
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        if (!is_reserved) {
            /*
             * V4 routes may have some half entries. These entries have only
             * one v4 route and other side is empty. For Half entry V4
             * routes, one v4 route occupies one defip index.
             * No need to divide v4 routes in half entries by half.
             */
#ifdef BCM_TOMAHAWK2_SUPPORT             
            if (soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                v4_count = SOC_LPM128_STAT_V4_COUNT(u);
            } else 
#endif
            {
                v4_count = SOC_LPM128_STAT_V4_COUNT(u) - SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
                v4_count = ((v4_count + 1) >> 1) + SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u);
            }
            b64_count = SOC_LPM128_STAT_64BV6_COUNT(u);
        }
    }
    *entries = (v128_max_count << 1) - ((b64_count + b128_count) << 1) -
               v4_count;
    *entries >>= 1;
    return SOC_E_NONE;
}

int
soc_lpm_used_v4_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV4_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV4_DEFIPm)) {
        *entries = SOC_EXT_LPM_V4_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_V4_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_V4_COUNT(u);
    }
    return SOC_E_NONE;
}

int
soc_lpm_used_64bv6_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if ((SOC_MEM_IS_ENABLED(u, EXT_IPV6_64_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_64_DEFIPm)) ||
        (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm))) {
        *entries = SOC_EXT_LPM_64BV6_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_64BV6_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_64BV6_COUNT(u);
    }
    return SOC_E_NONE;
}

int
soc_lpm_used_128bv6_route_get(int u, int *entries)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return SOC_E_UNAVAIL;
    }
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_MEM_IS_ENABLED(u, EXT_IPV6_128_DEFIPm) &&
        soc_mem_index_count(u, EXT_IPV6_128_DEFIPm)) {
        *entries = SOC_EXT_LPM_128BV6_COUNT(u);
    } else
#endif
    {
        *entries = SOC_LPM_128BV6_COUNT(u);
    }
    if (soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        *entries += SOC_LPM128_STAT_128BV6_COUNT(u);
    }
    return SOC_E_NONE;
}

void
soc_lpm_stat_128b_count_update(int u, int incr)
{
    if (!(soc_feature(u, soc_feature_l3_shared_defip_table) ||
          soc_feature(u, soc_feature_shared_defip_stat_support))) {
        return;
    }

    if (incr) {
        SOC_LPM_COUNT_INC(SOC_LPM_128BV6_COUNT(u));
    } else {
        SOC_LPM_COUNT_DEC(SOC_LPM_128BV6_COUNT(u));
    }
}

/*
 * Returns number of entries reserved to provide atomicity.
 * Supported only if 'lpm_atomic_write' is enabled.
 */
int
soc_lpm_reserved_route_get(int u, int *v4_entries,
                           int *v6_64_entries,
                           int *v6_128_entries)
{
    int v4 = 0, v6_64 = 0, v6_128 = 0;
 
    if (soc_feature(u, soc_feature_lpm_atomic_write)) {
        int max_entries;
        uint32 unpaired_entries;

        SOC_IF_ERROR_RETURN(soc_lpm_max_v4_route_get(u, &max_entries));
        SOC_IF_ERROR_RETURN(_soc_lpm_max_v4_route_get(u, 0,
                                              &unpaired_entries));
        if (max_entries - unpaired_entries) {
           v6_64 = 2;
           v4 = 4;
           v6_128 = 1;
        } else {
           v6_64 = 1;
           v4 = 2;
        }
    }

    if (v4_entries != NULL) {
       *v4_entries = v4;
    }

    if (v6_64_entries != NULL) {
       *v6_64_entries = v6_64;
    }

    if (v6_128_entries != NULL) {
       *v6_128_entries = v6_128;
    } 

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_fb_lpm128_reinit(int u, int idx, uint32 *lpm_entry,
                     uint32 *lpm_entry_upr)
{
    int pfx = 0;
    int is_reserved = 0;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(u);
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);
    int offset = 0;

    if (!soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_NONE;
    }

    if (max_v6_entries == 0) {
        return SOC_E_NONE;
    }

    if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN
        (_soc_fb_lpm128_prefix_length_get(u, lpm_entry, lpm_entry_upr,
                                          &pfx));

    if (!is_reserved || (SOC_LPM128_PFX_IS_V6_128(u, pfx))) {
        lpm_state_ptr = SOC_LPM128_STATE(u);
    } else {
        lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(u);
    }

    if (SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) == 0) {
        SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx) = idx;
        SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = idx;
    } else {
        if (SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) != -1) {
            SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = idx;
        } else {
            if (((SOC_LPM128_PFX_IS_V6(u, pfx))) && LPM128_IN_ODD_TCAM(u, idx)) {
                return SOC_E_NONE;
            }
            if ((SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) + 1) != idx &&
                (SOC_LPM128_PFX_IS_V4(u, pfx))) {
                SOC_LPM128_STATE_START2(u, lpm_state_ptr, pfx) = idx;
                SOC_LPM128_STATE_END2(u, lpm_state_ptr, pfx) = idx;
            } else {
                SOC_LPM128_STATE_END1(u, lpm_state_ptr, pfx) = idx;
            }
        }
    }

    SOC_LPM128_STATE_VENT(u, lpm_state_ptr, pfx) += 1;
    LPM128_HASH_INSERT(u, lpm_entry, lpm_entry_upr, idx, NULL, NULL);
    SOC_LPM128_INDEX_TO_PFX_GROUP(u, idx) = pfx;
    if (SOC_LPM128_PFX_IS_V6(u, pfx)) {
        offset = soc_fb_lpm_paired_offset(u, idx);
        SOC_LPM128_INDEX_TO_PFX_GROUP(u, idx + offset) = pfx;
    }

    return (SOC_E_NONE);
}

STATIC int
_soc_fb_lpm128_setup_pfx_state(int u, soc_lpm128_state_p lpm_state_ptr)
{
    int prev_idx = MAX_PFX128_INDEX;
    int idx = 0;
    int from_ent = -1;
    int v0 = 0, v1 = 0;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS] = {0};
    int rv = SOC_E_NONE;
    soc_mem_t mem = L3_DEFIPm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(u)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    if (NULL == lpm_state_ptr) {
       return SOC_E_INTERNAL;
    }

    SOC_LPM128_STATE_PREV(u, lpm_state_ptr, MAX_PFX128_INDEX) = -1;

    for (idx = MAX_PFX128_INDEX; idx > 0; idx--) {
       if (SOC_LPM128_STATE_START1(u, lpm_state_ptr, idx) == -1) {
           continue;
       }

       SOC_LPM128_STATE_PREV(u, lpm_state_ptr, idx) = prev_idx;
       SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_idx) = idx;

       prev_idx = idx;
       if (SOC_LPM128_PFX_IS_V6_64(u, idx)) {
           SOC_LPM128_STAT_64BV6_COUNT(u) += SOC_LPM128_STATE_VENT(u,
                                                           lpm_state_ptr, idx);
       }

       if (SOC_LPM128_PFX_IS_V6_128(u, idx)) {
           SOC_LPM128_STAT_128BV6_COUNT(u) += SOC_LPM128_STATE_VENT(u,
                                                            lpm_state_ptr, idx);
       }

       if (SOC_LPM128_PFX_IS_V4(u, idx)) {
           if (SOC_LPM128_STATE_START2(u, lpm_state_ptr, idx) != -1) {
               from_ent = SOC_LPM128_STATE_END2(u, lpm_state_ptr, idx);
           } else {
               from_ent = SOC_LPM128_STATE_END1(u, lpm_state_ptr, idx);
           }
#ifdef FB_LPM_TABLE_CACHED
           SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, mem,
                                                     MEM_BLOCK_ANY, from_ent));
#endif /* FB_LPM_TABLE_CACHED */
            rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e);
            SOC_IF_ERROR_RETURN(rv);
            if (!soc_feature(u, soc_feature_half_of_l3_defip_ipv4_capacity)) {
                v0 = SOC_MEM_OPT_F32_VALID0_GET(u, mem, e);
                v1 = SOC_MEM_OPT_F32_VALID1_GET(u, mem, e);
                SOC_LPM128_STAT_V4_COUNT(u) += (SOC_LPM128_STATE_VENT(u,
                                                          lpm_state_ptr, idx)) << 1;
                if ((v0 == 0) || (v1 == 0)) {
                    SOC_LPM128_STAT_V4_COUNT(u) -= 1;
                }
                if ((v0 && (!(v1))) || ((!(v0) && v1))) {
                    SOC_LPM_COUNT_INC(SOC_LPM128_STAT_V4_HALF_ENTRY_COUNT(u));
                    SOC_LPM128_STATE_HFENT(u, lpm_state_ptr, idx) = 1;
                }
            } else {
                SOC_LPM128_STAT_V4_COUNT(u) += SOC_LPM128_STATE_VENT(u,
                                                          lpm_state_ptr, idx);
            }
        }
    }

    SOC_LPM128_STATE_NEXT(u, lpm_state_ptr, prev_idx) = -1;
#ifdef BCM_UTT_SUPPORT
    if (soc_feature(u, soc_feature_utt)) {
        SOC_LPM128_STATE_PREV(u, lpm_state_ptr, MAX_PFX128_INDEX) = -1;
    }
#endif

    return SOC_E_NONE;
}

int
soc_fb_lpm128_reinit_done(int unit, int ipv6)
{
    int paired_table_size = 0;
    int defip_table_size = 0;
    int i = 0;
    int pfx;
    int is_reserved = 0;
    int start_idx = -1;
    int end_idx = -1;
    int fent_incr = 0;
    int found = 0;
    int first_v4_index = -1;
    int split_done = 0;
    int next_pfx = -1;
    soc_lpm128_state_p lpm_state_ptr = SOC_LPM128_STATE(unit);
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int offset = 0;
#ifdef BCM_UTT_SUPPORT
    int l_tcam_start = 0;
#endif

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return SOC_E_NONE;
    }

    if (!ipv6) {
        return SOC_E_NONE;
    }

    if (max_v6_entries == 0) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_l3_lpm_128b_entries_reserved)) {
        is_reserved = 1;
    }

    SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(unit, &paired_table_size,
                                                   &defip_table_size));
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF.
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        max_v6_entries >>= 1;
    }

    lpm_state_ptr = SOC_LPM128_STATE(unit);
    if (lpm_state_ptr == NULL) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (_soc_fb_lpm128_setup_pfx_state(unit, lpm_state_ptr));

    pfx = MAX_PFX128_INDEX;
    next_pfx = SOC_LPM128_STATE_NEXT(unit, lpm_state_ptr, pfx);
    start_idx = 0;
    if (!is_reserved) {
        end_idx = paired_table_size;
        if (next_pfx == -1) {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = end_idx;
        }
    } else {
        end_idx = max_v6_entries * 2;
        if (next_pfx == -1) {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = end_idx;
        }
    }

    for (i = start_idx; (next_pfx != -1) && i < end_idx; i++) {
        if (LPM128_IN_ODD_TCAM_FIRST(unit, i)) {
            if (is_reserved) {
#ifdef BCM_UTT_SUPPORT
                /*
                 * Reserved section will have only V6 entries.
                 * Add logical TCAM depth to move to next valid V6 entry.
                 */
                if (soc_feature(unit, soc_feature_utt)) {
                    offset = soc_fb_lpm_paired_offset(unit, i);
                    i += offset;
                } else
#endif
                {
                    i += max_v6_entries;
                }
            }
            if (first_v4_index != -1 && !split_done) {
                offset = soc_fb_lpm_paired_offset(unit, first_v4_index);
                i = first_v4_index + offset;
                split_done = 1;
            }
        }
        if (SOC_LPM128_INDEX_TO_PFX_GROUP(unit, i) != -1) {
            pfx = SOC_LPM128_INDEX_TO_PFX_GROUP(unit, i);
            if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
                if (first_v4_index == -1) {
                   first_v4_index = SOC_LPM128_STATE_START1(u, lpm_state_ptr, pfx);
                }
                fent_incr = 1;
            } else {
                fent_incr = 2;
            }
            if (pfx != MAX_PFX128_INDEX) {
                found = 1;
            }
        } else {
            if ((SOC_LPM128_PFX_IS_V6(u, pfx)) && LPM128_IN_ODD_TCAM(unit, i)) {
                continue;
            }
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) += fent_incr;
        }
    }

    if (found) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, MAX_PFX128_INDEX) = 0;
    }
    lpm_state_ptr = SOC_LPM128_UNRESERVED_STATE(unit);
    if (lpm_state_ptr == NULL) {
       return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (_soc_fb_lpm128_setup_pfx_state(unit, lpm_state_ptr));
    pfx = MAX_PFX128_INDEX;
    found = 0;
#ifdef BCM_UTT_SUPPORT
    /*
     * Get start of unreserved index.
     */
    if (soc_feature(unit, soc_feature_utt)) {
        start_idx = 0;
        SOC_IF_ERROR_RETURN
            (soc_utt_lpm_scaled_unreserved_start_index(unit,
                                                       &start_idx));
    } else
#endif
    {
        start_idx = ((max_v6_entries / tcam_depth) * tcam_depth * 2) +
                    max_v6_entries % tcam_depth;
    }
    next_pfx = SOC_LPM128_STATE_NEXT(unit, lpm_state_ptr, pfx);
    if (next_pfx == -1) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) = (paired_table_size -
                                                         (max_v6_entries << 1));
    }
    for (i = start_idx; (next_pfx != -1) && i < paired_table_size; i++) {
        if (LPM128_IN_ODD_TCAM_FIRST(unit, i)) {
#ifdef BCM_UTT_SUPPORT
            /*
             * Add delta of v6 entries reserved in last lookup.
             * Difference between logical tcam start addr of unreserved section
             * and unreserved start index will give num v6 entries reserved in
             * last lookup.
             */
            if (soc_feature(unit, soc_feature_utt)) {
                l_tcam_start = soc_utt_lpm128_logical_tcam_start_index(unit,
                                                                       start_idx);
                i += (start_idx - l_tcam_start);
            } else
#endif
            {
                i += (max_v6_entries % tcam_depth);
            }
        }
        if (SOC_LPM128_INDEX_TO_PFX_GROUP(unit, i) != -1) {
            pfx = SOC_LPM128_INDEX_TO_PFX_GROUP(unit, i);
            if (SOC_LPM128_PFX_IS_V4(u, pfx)) {
                fent_incr = 1;
            } else {
                fent_incr = 2;
            }
            if (pfx != MAX_PFX128_INDEX) {
                found = 1;
            }
        } else {
            SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, pfx) += fent_incr;
        }
    }

    if (found) {
        SOC_LPM128_STATE_FENT(unit, lpm_state_ptr, MAX_PFX128_INDEX) = 0;
    }

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
soc_kt_lpm_ipv6_info_t *
soc_kt_lpm_ipv6_info_get(int unit)
{
    return kt_lpm_ipv6_info[unit];
}
#endif
