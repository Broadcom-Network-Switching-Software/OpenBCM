/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    lpv6.c
 * Purpose: L3 LPM V6 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#ifdef INCLUDE_L3  

#include <soc/drv.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/format.h>
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#endif
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>

#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/lpmv6.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/alpm.h>

#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#endif /* TRIDENT3_SUPPORT */

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_UTT_SUPPORT
#include <soc/esw/utt.h>
#endif /* BCM_UTT_SUPPORT */

#define _BCM_L3_MEM_BANKS_ALL     (-1)
#define _BCM_HOST_ENTRY_NOT_FOUND (-1)

/* L3 LPM state */
_bcm_defip_pair128_table_t *l3_defip_pair128[BCM_MAX_NUM_UNITS];

#define SOC_LPM_128_LPM_CACHE_FIELD_CREATE(m, f) soc_field_info_t * m##f
#define SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(m_u, s, m, f) \
                (s)->m##f = soc_mem_fieldinfo_get(m_u, m, f)
typedef struct soc_lpm_128_field_cache_s {
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, CLASS_IDf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, DST_DISCARDf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, ECMPf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, ECMP_COUNTf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, ECMP_PTRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, GLOBAL_ROUTEf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, HITf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IP_ADDR_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MODE_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, NEXT_HOP_INDEXf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, PRIf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, RPEf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VALID0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VALID1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VALID0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VALID1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, VRF_ID_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, GLOBAL_HIGHf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, ALG_HIT_IDXf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, ALG_BKT_PTRf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, DEFAULT_MISSf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, FLEX_CTR_BASE_COUNTER_IDXf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, FLEX_CTR_POOL_NUMBERf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, SRC_DISCARDf);    
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, DEFAULTROUTEf);    
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, HIT0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, HIT1f);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, EXPECTED_L3_IIFf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, RPA_IDf);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, L3MC_INDEXf);
#endif
#if defined(BCM_HELIX5_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_128m, DATA_RANGEf);
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_HIT0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_HIT1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_VALID0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_VALID1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_VALID0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_VALID1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_KEY0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_KEY1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_KEY0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_KEY1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_MASK0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_MASK1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_MASK0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, UPR_MASK1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_ASSOC_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_ASSOC_DATA1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_FIXED_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_FIXED_DATA1f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_ALPM1_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_CREATE(L3_DEFIP_PAIR_LEVEL1m, LWR_ALPM1_DATA1f);
#endif
} soc_lpm_128_field_cache_t, *soc_lpm_128_field_cache_p;

static soc_lpm_128_field_cache_p 
                        soc_lpm_128_field_cache_state[SOC_MAX_NUM_DEVICES];

#define SOC_TH3_MEM_OPT_FLD_SET(m_unit, m_entry_data, m_field, m_pval) \
        soc_meminfo_fieldinfo_field_set((m_entry_data), \
        (&SOC_MEM_INFO(m_unit, L3_DEFIP_PAIR_LEVEL1m)), \
        (soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field), (m_pval))

#define SOC_TH3_MEM_OPT_FLD_VALID(m_unit, m_mem, m_field) \
        ((soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field) != NULL)

#define SOC_TH3_MEM_OPT_FLD_LENGTH(m_unit, m_mem, m_field) \
        ((soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field)->len)

#define SOC_TH3_MEM_OPT_FLD32_GET(m_unit, m_mem, m_entry_data, m_field) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), \
        (m_entry_data), (soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field))

#define SOC_TH3_MEM_OPT_FLD32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), \
        (m_entry_data), (soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field), (m_val))

#define SOC_TH3_MEM_OPT_FLD_GET(m_unit, m_mem, m_entry_data, m_field, m_pval) \
        soc_meminfo_fieldinfo_field_get((m_entry_data), \
        (&SOC_MEM_INFO(m_unit, m_mem)), \
        (soc_lpm_128_field_cache_state[(m_unit)]->L3_DEFIP_PAIR_LEVEL1m##m_field), (m_pval))


#define SOC_MEM_OPT_FLD32_GET(m_unit, m_mem, m_entry_data, m_field) \
        soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(m_unit, m_mem)), \
        (m_entry_data), (soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field))

#define SOC_MEM_OPT_FLD32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
        soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(m_unit, m_mem)), \
        (m_entry_data), (soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field), (m_val))

#define SOC_MEM_OPT_FLD_GET(m_unit, m_mem, m_entry_data, m_field, m_pval) \
        soc_meminfo_fieldinfo_field_get((m_entry_data), \
        (&SOC_MEM_INFO(m_unit, m_mem)), \
        (soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field), (m_pval))

#define SOC_MEM_OPT_FLD_SET(m_unit, m_mem, m_entry_data, m_field, m_pval) \
        soc_meminfo_fieldinfo_field_set((m_entry_data), \
        (&SOC_MEM_INFO(m_unit, m_mem)), \
        (soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field), (m_pval))

#define SOC_MEM_OPT_FLD_VALID(m_unit, m_mem, m_field) \
        ((soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field) != NULL)

#define SOC_MEM_OPT_FLD_LENGTH(m_unit, m_mem, m_field) \
        ((soc_lpm_128_field_cache_state[(m_unit)]->m_mem##m_field)->len)


/*
 * Function:
 *      _bcm_l3_defip_mem_get
 * Purpose:
 *      Resolve route table memory for a given route entry.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      flags      - (IN)IPv6/IPv4 route.
 *      plen       - (IN)Prefix length.
 *      mem        - (OUT)Route table memory.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
    /* Default value */
    *mem = L3_DEFIPm;
    if(!SOC_MEM_IS_VALID(unit, L3_DEFIPm) &&
       (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m))) {
        *mem = L3_DEFIP_LEVEL1m;
    }

    if ((flags & BCM_L3_IP6) && (plen > 64)) {
        *mem = L3_DEFIP_PAIR_128m;
        if(!SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
            *mem = L3_DEFIP_PAIR_LEVEL1m;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_defip_pair128_hash
 * Purpose:
 *      Calculate an entry 16 bit hash.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      entry_hash  - (OUT)Entry hash
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_defip_pair128_hash(int unit, _bcm_defip_cfg_t *lpm_cfg, uint16 *hash) 
{
    uint32 key[BCM_DEFIP_PAIR128_HASH_SZ];

    if (hash == NULL) {
        return (BCM_E_PARAM);
    }

    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        SAL_IP6_ADDR_TO_UINT32(lpm_cfg->defip_ip6_addr, key);
        key[4] = lpm_cfg->defip_sub_len;
        key[5] = lpm_cfg->defip_vrf;

        *hash = _shr_crc16b(0, (void *)key,
                         WORDS2BITS((BCM_DEFIP_PAIR128_HASH_SZ)));
    } else {
        /* Call V4 hash function */
    }   
    return (BCM_E_NONE);
}

int
_bcm_defip_lkup_plen_get(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    int f, mc;

    f = ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);
    mc = (lpm_cfg->defip_flags & BCM_L3_IPMC);

    if (soc_feature(unit, soc_feature_td3_lpm_ipmc_war) && mc) {
        f += 3;
    }

    return lpm_cfg->defip_sub_len * f;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_defip_pair128_reinit
 * Purpose:
 *      Re-Initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 *      during warm boot. 
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      idx     - (IN)Entry index   
 *      lpm_cfg - (IN)Inserted entry. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_reinit(int unit, int idx, _bcm_defip_cfg_t *lpm_cfg)
{
    int   lkup_plen;         /* Vrf weighted lookup prefix. */
    uint16 hash = 0;         /* Entry hash value.           */

    /* Calculate vrf weighted prefix length. */
    lkup_plen = _bcm_defip_lkup_plen_get(unit, lpm_cfg);

    /* Calculate entry lookup hash. */
    BCM_IF_ERROR_RETURN(_bcm_defip_pair128_hash(unit, lpm_cfg, &hash));

    /* Set software structure info. */
    BCM_DEFIP_PAIR128_ENTRY_SET(unit, idx, lkup_plen, hash);

    /* Update software usage counter. */
    BCM_DEFIP_PAIR128_USED_COUNT(unit)++;
    soc_lpm_stat_128b_count_update(unit, 1);

    return (BCM_E_NONE);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_defip_pair128_deinit
 * Purpose:
 *      De-initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_defip_pair128_deinit(int unit)
{

    if (BCM_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_NONE);
    }

    /* De-allocate any existing structures. */
    if (soc_lpm_128_field_cache_state[unit] != NULL) {
        sal_free(soc_lpm_128_field_cache_state[unit]);
        soc_lpm_128_field_cache_state[unit] = NULL;
    }
    
    if (BCM_DEFIP_PAIR128_ARR(unit) != NULL) {
        sal_free(BCM_DEFIP_PAIR128_ARR(unit));
        BCM_DEFIP_PAIR128_ARR(unit) = NULL;
    }
    sal_free(BCM_DEFIP_PAIR128(unit));
    BCM_DEFIP_PAIR128(unit) = NULL;

    return (BCM_E_NONE);
}

int
_bcm_defip_pair128_field_cache_init(int unit)
{
    /* De-allocate existing structures. */
    if (soc_lpm_128_field_cache_state[unit] != NULL) {
        sal_free(soc_lpm_128_field_cache_state[unit]);
    }

    soc_lpm_128_field_cache_state[unit] = 
           sal_alloc(sizeof(soc_lpm_128_field_cache_t), "lpm_128_field_state");
    if (NULL == soc_lpm_128_field_cache_state[unit]) {
        return (BCM_E_MEMORY);
    }
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, CLASS_IDf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, DST_DISCARDf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, ECMPf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, ECMP_COUNTf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, ECMP_PTRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, GLOBAL_ROUTEf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, HITf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IP_ADDR_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MODE_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, NEXT_HOP_INDEXf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, PRIf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, RPEf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VALID0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VALID0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VALID1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VALID1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_MASK0_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_MASK0_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_MASK1_LWRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, VRF_ID_MASK1_UPRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, GLOBAL_HIGHf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, ALG_HIT_IDXf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, ALG_BKT_PTRf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, DEFAULT_MISSf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, 
                                    FLEX_CTR_BASE_COUNTER_IDXf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, 
                                    FLEX_CTR_POOL_NUMBERf);    
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, 
                                    SRC_DISCARDf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, DEFAULTROUTEf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, HIT0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, HIT1f);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, EXPECTED_L3_IIFf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, RPA_IDf);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, L3MC_INDEXf);
#endif
#if defined(BCM_HELIX5_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_128m, DATA_RANGEf);
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_HIT0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_HIT1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_VALID0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_VALID1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_VALID0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_VALID1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_KEY0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_KEY1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_KEY0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_KEY1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_MASK0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_MASK1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_MASK0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, UPR_MASK1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_ASSOC_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_ASSOC_DATA1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_FIXED_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_FIXED_DATA1f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_ALPM1_DATA0f);
    SOC_LPM_128_LPM_CACHE_FIELD_ASSIGN(unit, soc_lpm_128_field_cache_state[unit],
                                    L3_DEFIP_PAIR_LEVEL1m, LWR_ALPM1_DATA1f);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_defip_pair128_init
 * Purpose:
 *      Initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_defip_pair128_init(int unit)
{
    int rv;
    int mem_sz;   
    int ipv6_128_depth;
 
    /* De-allocate any existing structures. */
    if (BCM_DEFIP_PAIR128(unit) != NULL) {
        rv = _bcm_defip_pair128_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Allocate cam control structure. */
    mem_sz = sizeof(_bcm_defip_pair128_table_t);
    BCM_DEFIP_PAIR128(unit) = sal_alloc(mem_sz, "td2_l3_defip_pair128");
    if (BCM_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Reset cam control structure. */
    sal_memset(BCM_DEFIP_PAIR128(unit), 0, mem_sz);

    ipv6_128_depth = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    BCM_DEFIP_PAIR128_TOTAL(unit) = ipv6_128_depth;
    BCM_DEFIP_PAIR128_URPF_OFFSET(unit) = 0;
    BCM_DEFIP_PAIR128_IDX_MAX(unit) = ipv6_128_depth - 1;
    if (ipv6_128_depth) {
        mem_sz = ipv6_128_depth * sizeof(_bcm_defip_pair128_entry_t);
        BCM_DEFIP_PAIR128_ARR(unit) = 
                              sal_alloc(mem_sz, 
                                        "td2_l3_defip_pair128_entry_array");
        if (BCM_DEFIP_PAIR128_ARR(unit) == NULL) {
            BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
            return (BCM_E_MEMORY);
        }
        sal_memset(BCM_DEFIP_PAIR128_ARR(unit), 0, mem_sz);
    }

    rv = _bcm_defip_pair128_field_cache_init(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
        return rv;
    }
    
    return (BCM_E_NONE);
}

/*
 * Function:    
 *     _bcm_defip_pair128_ip6_mask_set
 * Purpose:  
 *     Set IP6 mask field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 *     ipmc  - (IN) multicast entry
 * Returns:      void
 */
void
_bcm_defip_pair128_ip6_mask_set(int unit, soc_mem_t mem, uint32 *entry, 
                                const ip6_addr_t ip6, int ipmc)
{
    uint32              ip6_field[4];
    
#if defined(BCM_TOMAHAWK3_SUPPORT)
    assert((mem == L3_DEFIP_PAIR_128m) || (mem == L3_DEFIP_PAIR_LEVEL1m));
#else
    assert(mem == L3_DEFIP_PAIR_128m);
#endif
    ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
        int vrf_mask = 0;
        uint32 _key_data[2] = {0,0};
        vrf_mask = (1 << soc_format_field_length(unit, L3_DEFIP_TCAM_KEYfmt, VRF_IDf)) - 1;
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field[3]);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf, vrf_mask);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)entry, LWR_MASK0f, _key_data);

        ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        _key_data[0] = _key_data[1] = 0;
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field[2]);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf, vrf_mask);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)entry, LWR_MASK1f, _key_data);

        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        _key_data[0] = _key_data[1] = 0;
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field[1]);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf, vrf_mask);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)entry, UPR_MASK0f, _key_data);

        ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        _key_data[0] = _key_data[1] = 0;
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field[0]);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf, vrf_mask);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)entry, UPR_MASK1f, _key_data);
    } else
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) && ipmc) {
        /* IPv6 IPMC entry with separate key */
        uint32 ip6_word[2] = {0,0};
        if (SOC_IS_FIREBOLT6(unit)) {
            /* set lower 46 bits */
            ip6_word[1] = (((ip6[10] & 0x3f) << 8) | (ip6[11] << 0));
            ip6_word[0] = ((ip6[12] << 24) | (ip6[13] << 16) | (ip6[14] << 8) | (ip6[15] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_45_0_MASKf, ip6_word);
            /* set middle 46 bits */
            ip6_word[1] = (((ip6[4] & 0xf) << 10) | (ip6[5] << 2) | (ip6[6] >> 2));
            ip6_word[0] = (((ip6[6] & 0x3f) << 26) | (ip6[7] << 18) | (ip6[8] << 10) |
                           (ip6[9] << 2) | (ip6[10] >> 2));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_91_46_MASKf, ip6_word);
            /* set upper 36 bits */
            ip6_word[1] = (ip6[0]>> 4);
            ip6_word[0] = (((ip6[0] & 0xf) << 28) | (ip6[1] << 20) | (ip6[2] << 12) | (ip6[3] << 4) | (ip6[4] >> 4));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_92_MASKf, ip6_word);
        } else {
            /* set lower 44 bits */
            ip6_word[1] = (((ip6[10] & 0xf) << 8) | (ip6[11] << 0));
            ip6_word[0] = ((ip6[12] << 24) | (ip6[13] << 16) | (ip6[14] << 8) | (ip6[15] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_43_0_MASKf, ip6_word);
            /* set middle 44 bits */
            ip6_word[1] = ((ip6[5] << 4) | (ip6[6] >> 4));
            ip6_word[0] = (((ip6[6] & 0xf) << 28) | (ip6[7] << 20) | (ip6[8] << 12) |
                           (ip6[9] << 4) | (ip6[10] >> 4));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_87_44_MASKf, ip6_word);
            /* set upper 40 bits */
            ip6_word[1] = ip6[0];
            ip6_word[0] = ((ip6[1] << 24) | (ip6[2] << 16) | (ip6[3] << 8) | (ip6[4] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_88_MASKf, ip6_word);
        }
    } else
#endif
    {
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK0_LWRf, &ip6_field[3]);
        ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK1_LWRf, &ip6_field[2]);
        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK0_UPRf, &ip6_field[1]);
        ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK1_UPRf, ip6_field);
    }
}

/*
 * Function:    
 *     _bcm_defip_pair128_ip6_mask_get
 * Purpose:  
 *     Read IP6 mask field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_ip6_mask_get(int unit, soc_mem_t mem, const void *entry, 
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];

    assert((mem == L3_DEFIP_PAIR_128m) || (mem == L3_DEFIP_PAIR_LEVEL1m));

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
        uint32 _key_data[2] = {0, 0};
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, LWR_MASK0f, _key_data);
        ip6_field[3] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, LWR_MASK1f, _key_data);
        ip6_field[2] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, UPR_MASK0f, _key_data);
        ip6_field[1] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, UPR_MASK1f, _key_data);
        ip6_field[0] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
    } else
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) &&
        soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
            entry, KEY_TYPE0_LWRf) == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE) {
        uint32 ip6_word[2] = {0, 0};
        if (SOC_IS_FIREBOLT6(unit)) {
            /* get first 46 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_45_0_MASKf, ip6_word);
            ip6_field[3] = ip6_word[0];
            ip6_field[2] = ip6_word[1] & 0x3fff;
            /* get next 46 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_91_46_MASKf, ip6_word);
            ip6_field[2] |= ((ip6_word[0] & 0x3ffff) << 14);
            ip6_field[1] = ip6_word[0] >> 18;
            ip6_field[1] |= ((ip6_word[1] & 0x3fff) << 14);
            /* get last 36 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_92_MASKf, ip6_word);
            ip6_field[1] |= (ip6_word[0] & 0xf) << 28;
            ip6_field[0] = (((ip6_word[1] & 0xf) << 28) | (ip6_word[0] >> 4));
        } else {
            /* get first 44 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_43_0_MASKf, ip6_word);
            ip6_field[3] = ip6_word[0];
            ip6_field[2] = ip6_word[1] & 0xfff;
            /* get next 44 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_87_44_MASKf, ip6_word);
            ip6_field[2] |= ((ip6_word[0] & 0xfffff) << 12);
            ip6_field[1] = ip6_word[0] >> 20;
            ip6_field[1] |= ((ip6_word[1] & 0xfff) << 12);
            /* get last 40 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_88_MASKf, ip6_word);
            ip6_field[1] |= (ip6_word[0] & 0xff) << 24;
            ip6_field[0] = (((ip6_word[1] & 0xff) << 24) | (ip6_word[0] >> 8));
        }
    } else
#endif
    {
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK0_LWRf, (uint32 *)&ip6_field[3]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK1_LWRf, (uint32 *)&ip6_field[2]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK0_UPRf, (uint32 *)&ip6_field[1]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR_MASK1_UPRf, ip6_field);
    }
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    ip6[8]  = (uint8) (ip6_field[2] >> 24);
    ip6[9]  = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] = (uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] = (uint8) (ip6_field[1] & 0xff);
    ip6[0] = (uint8) (ip6_field[0] >> 24);
    ip6[1] = (uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] = (uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] = (uint8) (ip6_field[0] & 0xff);
}

/*
 * Function:    
 *     _bcm_defip_pair128_ip6_addr_set
 * Purpose:  
 *     Set IP6 address field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 *     ipmc  - (IN) multicast entry.
 * Returns:      void
 */
void
_bcm_defip_pair128_ip6_addr_set(int unit, soc_mem_t mem, uint32 *entry,
                                const ip6_addr_t ip6, int ipmc)
{
    uint32              ip6_field[4];

    assert(mem == L3_DEFIP_PAIR_128m);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) && ipmc) {
        /* IPv6 IPMC entry with separate key */
        uint32 ip6_word[2] = {0,0};
        if (SOC_IS_FIREBOLT6(unit)) {
            /* set lower 46 bits */
            ip6_word[1] = (((ip6[10] & 0x3f) << 8) | (ip6[11] << 0));
            ip6_word[0] = ((ip6[12] << 24) | (ip6[13] << 16) | (ip6[14] << 8) | (ip6[15] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_45_0f, ip6_word);
            /* set middle 46 bits */
            ip6_word[1] = (((ip6[4] & 0xf) << 10) | (ip6[5] << 2) | (ip6[6] >> 2));
            ip6_word[0] = (((ip6[6] & 0x3f) << 26) | (ip6[7] << 18) | (ip6[8] << 10) |
                           (ip6[9] << 2) | (ip6[10] >> 2));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_91_46f, ip6_word);
            /* set upper 36 bits */
            ip6_word[1] = (ip6[0] >> 4);
            ip6_word[0] = (((ip6[0] & 0xf) << 28) | (ip6[1] << 20) | (ip6[2] << 12) | (ip6[3] << 4) | (ip6[4] >> 4));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_92f, ip6_word);
        } else {
            /* set lower 44 bits */
            ip6_word[1] = (((ip6[10] & 0xf) << 8) | (ip6[11] << 0));
            ip6_word[0] = ((ip6[12] << 24) | (ip6[13] << 16) | (ip6[14] << 8) | (ip6[15] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_43_0f, ip6_word);
            /* set middle 44 bits */
            ip6_word[1] = ((ip6[5] << 4) | (ip6[6] >> 4));
            ip6_word[0] = (((ip6[6] & 0xf) << 28) | (ip6[7] << 20) | (ip6[8] << 12) |
                           (ip6[9] << 4) | (ip6[10] >> 4));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_87_44f, ip6_word);
            /* set upper 40 bits */
            ip6_word[1] = ip6[0];
            ip6_word[0] = ((ip6[1] << 24) | (ip6[2] << 16) | (ip6[3] << 8) | (ip6[4] << 0));
            soc_mem_field_set(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_88f, ip6_word);
        }
    } else
#endif
    {
        ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR0_LWRf, &ip6_field[3]);
        ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR1_LWRf, &ip6_field[2]);
        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR0_UPRf, &ip6_field[1]);
        ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        SOC_MEM_OPT_FLD_SET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR1_UPRf, ip6_field);
    }
}

/*
 * Function:    
 *     _bcm_defip_pair128_ip6_addr_get
 * Purpose:  
 *     Read IP6 address field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_ip6_addr_get(int unit, soc_mem_t mem, const void *entry, 
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];

    assert((mem == L3_DEFIP_PAIR_128m) || (mem == L3_DEFIP_PAIR_LEVEL1m));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) &&
        soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
            entry, KEY_TYPE0_LWRf) == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE) {
        uint32 ip6_word[2];
        if (SOC_IS_FIREBOLT6(unit)) {
            /* get first 46 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_45_0f, ip6_word);
            ip6_field[3] = ip6_word[0];
            ip6_field[2] = ip6_word[1] & 0x3fff;
            /* get next 46 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_91_46f, ip6_word);
            ip6_field[2] |= ((ip6_word[0] & 0x3ffff) << 14);
            ip6_field[1] = ip6_word[0] >> 18;
            ip6_field[1] |= ((ip6_word[1] & 0x3fff) << 14);
            /* get last 36 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_92f, ip6_word);
            ip6_field[1] |= (ip6_word[0] & 0xf) << 28;
            ip6_field[0] = (((ip6_word[1] & 0xf) << 28) | (ip6_word[0] >> 4));
        } else {
            /* get first 44 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_43_0f, ip6_word);
            ip6_field[3] = ip6_word[0];
            ip6_field[2] = ip6_word[1] & 0xfff;
            /* get next 44 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_87_44f, ip6_word);
            ip6_field[2] |= ((ip6_word[0] & 0xfffff) << 12);
            ip6_field[1] = ip6_word[0] >> 20;
            ip6_field[1] |= ((ip6_word[1] & 0xfff) << 12);
            /* get last 40 bits */
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, entry, IPMC_V6_ADDR_127_88f, ip6_word);
            ip6_field[1] |= (ip6_word[0] & 0xff) << 24;
            ip6_field[0] = (((ip6_word[1] & 0xff) << 24) | (ip6_word[0] >> 8));
        }
    } else
#endif
    if (mem == L3_DEFIP_PAIR_128m) {
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR0_LWRf, (uint32 *)&ip6_field[3]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR1_LWRf, (uint32 *)&ip6_field[2]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR0_UPRf, (uint32 *)&ip6_field[1]);
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, entry, IP_ADDR1_UPRf, ip6_field);
    } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
        uint32 _key_data[2] = {0, 0};
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, LWR_KEY0f, _key_data);
        ip6_field[3] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, LWR_KEY1f, _key_data);
        ip6_field[2] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, UPR_KEY0f, _key_data);
        ip6_field[1] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)entry, UPR_KEY1f, _key_data);
        ip6_field[0] = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf);
#endif
    }
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    ip6[8] = (uint8) (ip6_field[2] >> 24);
    ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] =(uint8) (ip6_field[1] & 0xff);
    ip6[0] =(uint8) (ip6_field[0] >> 24);
    ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] =(uint8) (ip6_field[0] & 0xff);
    
}

/*
 * Function:
 *      _bcm_defip_pair128_get_key
 * Purpose:
 *      Parse route entry key from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      entry       - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_get_key(int unit, uint32 *hw_entry, _bcm_defip_cfg_t *lpm_cfg)
{
    bcm_ip6_t mask;                      /* Subnet mask.        */
    soc_mem_t mem = L3_DEFIP_PAIR_128m;

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        uint32 _key_data[2] = {0, 0};
        uint8 priority = 0;
        mem = L3_DEFIP_PAIR_LEVEL1m;

        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)hw_entry, LWR_KEY0f, _key_data);
        lpm_cfg->defip_vrf = soc_format_field32_get(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf);
        _key_data[0] = _key_data[1] = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)hw_entry, LWR_FIXED_DATA0f, _key_data);
        priority = soc_format_field32_get(unit, FIXED_DATAfmt, _key_data, SUB_DB_PRIORITYf);
        if (priority == 0) {
            lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
        } else if (priority == 2) {
            lpm_cfg->defip_vrf = BCM_L3_VRF_GLOBAL;
        }
#endif
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        uint32 key_type;

        key_type = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                hw_entry, KEY_TYPE0_LWRf);

        if (BCMI_LPM_FLEX_VIEW(key_type)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_defip_pair128_flex_get_key(unit, hw_entry, lpm_cfg, key_type));
            return BCM_E_NONE;
        }
    }
#endif

    /* Extract ip6 address. */
    _bcm_defip_pair128_ip6_addr_get(unit, mem, hw_entry, lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _bcm_defip_pair128_ip6_mask_get(unit, mem, hw_entry, mask);
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) &&
           (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
           hw_entry, KEY_TYPE0_LWRf) == BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE)) {
           uint32 key_mask;
           if (SOC_IS_FIREBOLT6(unit)) {
               key_mask = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_9_0_MASKf);
               key_mask |= (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_12_10_MASKf) << 10);
               if (!key_mask) {
                   lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
               } else {
                   lpm_cfg->defip_vrf = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_9_0f);
                   lpm_cfg->defip_vrf |= (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_12_10f) << 10);
               }

           } else {
               key_mask = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_3_0_MASKf);
               key_mask |= (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_11_4_MASKf) << 4);
               if (!key_mask) {
                   lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
               } else {
                   lpm_cfg->defip_vrf = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_3_0f);
                   lpm_cfg->defip_vrf |= (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_V6_VRF_ID_11_4f) << 4);
               }
           }
        }
        else
#endif
        {
            if (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK0_LWRf)) {
                lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
            } else {
                lpm_cfg->defip_vrf = SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_0_LWRf);
            }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_defip_pair128_match
 * Purpose:
 *      Lookup route entry from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      hw_entry    - (OUT)Matching hw entry. 
 *      hw_index    - (OUT)HW table index. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_match(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                         uint32 *hw_entry, int *hw_index)
{
    _bcm_defip_cfg_t candidate;        /* Match condidate.            */
    int   lkup_plen;                   /* Vrf weighted lookup prefix. */
    int   index;                       /* Table iteration index.      */
    uint16 hash = 0;                   /* Entry hash value.           */
    int   rv = BCM_E_NONE;             /* Internal operation status.  */
    soc_mem_t  mem = L3_DEFIP_PAIR_128m;

    /* Initialization */
    sal_memset(&candidate, 0, sizeof(_bcm_defip_cfg_t));
    
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        mem = L3_DEFIP_PAIR_LEVEL1m;
    }

    /* Calculate vrf weighted prefix length. */
    lkup_plen = _bcm_defip_lkup_plen_get(unit, lpm_cfg);

    /* Calculate entry lookup hash. */
    BCM_IF_ERROR_RETURN(_bcm_defip_pair128_hash(unit, lpm_cfg, &hash));

    /*    coverity[assignment : FALSE]    */
    for (index = 0; index <= BCM_DEFIP_PAIR128_IDX_MAX(unit); index++) {

        /* Route prefix length comparison. */
        if (lkup_plen != BCM_DEFIP_PAIR128_ARR(unit)[index].prefix_len) {
            continue;
        }

        /* Route lookup key hash comparison. */
        if (hash != BCM_DEFIP_PAIR128_ARR(unit)[index].entry_hash) {
            continue;
        }

        /* Hash & prefix length match -> read and compare hw entry itself. */
        rv = BCM_XGS3_MEM_READ(unit, mem, index, hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Make sure entry is valid.  */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
            if ((!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_VALID0f)) ||
                (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_VALID1f)) ||
                (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, UPR_VALID0f)) ||
                (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, UPR_VALID1f))
                ) {
                /* Too bad sw image doesn't match hardware. */
                continue;
            }
        } else
#endif
        if ((!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_LWRf)) ||
            (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_LWRf)) ||
            (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_UPRf)) ||
            (!SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_UPRf))
            ) {
            /* Too bad sw image doesn't match hardware. */
            continue;
        }

        /* Extract entry address vrf */
        rv =  _bcm_defip_pair128_get_key(unit, hw_entry, &candidate);
        if (BCM_FAILURE(rv)) {
            break;
        }


        /* Address & Vrf comparison. */
        if (lpm_cfg->defip_vrf != candidate.defip_vrf) {
            continue;
        }

        /* Subnet mask comparison. */
        if (lpm_cfg->defip_sub_len != candidate.defip_sub_len) {
            continue;
        }


        if (sal_memcmp(lpm_cfg->defip_ip6_addr, candidate.defip_ip6_addr,
                       sizeof(bcm_ip6_t))) {
            continue;
        }

        /* Matching entry found. */
        break;

    }

    BCM_IF_ERROR_RETURN(rv);

    if (index > BCM_DEFIP_PAIR128_IDX_MAX(unit))  {
        return (BCM_E_NOT_FOUND);
    }

    *hw_index = index;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_defip_pair128_entry_clear
 * Purpose:
 *      Clear/Reset a single route entry. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      hw_index  - (IN)Entry index in the tcam. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_entry_clear(int unit, int hw_index)  
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t key_field[4], mask_field[4];
    int field_count, i;
    uint64 val64;
    key_field[0] = KEY0_UPRf;
    key_field[1] = KEY1_UPRf;
    key_field[2] = KEY0_LWRf;
    key_field[3] = KEY1_LWRf;
    mask_field[0] = MASK0_UPRf;
    mask_field[1] = MASK1_UPRf;
    mask_field[2] = MASK0_LWRf;
    mask_field[3] = MASK1_LWRf;
    field_count = 4;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        key_field[0] = UPR_KEY0f;
        key_field[1] = UPR_KEY1f;
        key_field[2] = LWR_KEY0f;
        key_field[3] = LWR_KEY1f;
        mask_field[0] = UPR_MASK0f;
        mask_field[1] = UPR_MASK1f;
        mask_field[2] = LWR_MASK0f;
        mask_field[3] = LWR_MASK1f;
        mem = L3_DEFIP_PAIR_LEVEL1m;
    }
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
    COMPILER_64_ZERO(val64);
    for (i = 0; i < field_count; i++) {
        soc_mem_field64_set(unit, mem, hw_entry, key_field[i], val64);
        soc_mem_field64_set(unit, mem, hw_entry, mask_field[i], val64);
    }

    /* Reset hw entry. */
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, mem, hw_index,
                            &soc_mem_entry_null(unit, mem)));
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, mem, hw_index, hw_entry));

    /* Reset sw entry. */
    BCM_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);

    /* Reset urpf source lookup entry. */
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, mem,
                            (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)),
                            &soc_mem_entry_null(unit, mem)));
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, mem,
                 (hw_index+ BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), hw_entry));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_defip_pair128_parse
 * Purpose:
 *      Parse route entry from L3_DEFIP_PAIR_128 table to sw structure.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      hw_entry    - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 *      nh_ecmp_idx - (OUT)Next hop ecmp table index. 
 * Returns:
 *      BCM_E_XXX
 */
static INLINE int
_bcm_defip_pair128_parse(int unit, uint32 *hw_entry,
                         _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
        uint32 key_type;

        key_type = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                         hw_entry, KEY_TYPE0_LWRf);

        if (BCMI_LPM_FLEX_VIEW(key_type)) {
            return _bcm_td3_lpm_flex_ent_parse(unit, hw_entry, key_type,
                                                L3_DEFIP_PAIR_128m, lpm_cfg,
                                                nh_ecmp_idx);
        }
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        int ipmc_route = 0;
        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
            ipmc_route = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                             hw_entry, MULTICAST_ROUTEf);
        } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
            if (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                    hw_entry, DATA_TYPEf) == 2) {
                ipmc_route = 1;
            } else {
                ipmc_route = 0;
            }
        }

        if (ipmc_route) {
            int val;
            lpm_cfg->defip_flags |= BCM_L3_IPMC;

            if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
            val = SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, EXPECTED_L3_IIFf);
            /*
             * RP ID and ExpectedIIF are overlaid.
             */
            if (soc_feature(unit, soc_feature_l3defip_rp_l3iif_resolve)) {
                int rpa_id_mask =
                    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, RPA_IDf)) - 1;
                /*
                 * 1) If value is more than RPA_ID field then its l3_iif.
                 *    N/A for TD3/MV2/HX5 to support warmboot.
                 * 2) When RPF FAIL DROP/TOCPU flags are set then its l3iif valid
                 */
                if ((lpm_cfg->defip_ipmc_flags &
                     (BCM_IPMC_RPF_FAIL_DROP | BCM_IPMC_RPF_FAIL_TOCPU)) ||
                    (!(SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit) ||
                       SOC_IS_HELIX5(unit)) && (val > rpa_id_mask))) {
                    lpm_cfg->defip_expected_intf = val;
                    if (0 != lpm_cfg->defip_expected_intf) {
                        lpm_cfg->defip_ipmc_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
                    }
                    lpm_cfg->defip_l3a_rp = BCM_IPMC_RP_ID_INVALID;
                } else {
                    lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RP_ID_EXPECTED_INTF_RESOLVE;
                    lpm_cfg->defip_l3a_rp = val & rpa_id_mask;
                    lpm_cfg->defip_expected_intf = val;
                }
            } else if (_BCM_DEFIP_IPMC_RP_IS_SET(val)) {
                lpm_cfg->defip_l3a_rp = _BCM_DEFIP_IPMC_RP_GET(val);
            } else {
                lpm_cfg->defip_expected_intf = val;

                if (0 != lpm_cfg->defip_expected_intf) {
                    lpm_cfg->defip_ipmc_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
                }
                lpm_cfg->defip_l3a_rp = BCM_IPMC_RP_ID_INVALID;
            }

            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
                uint32 dest_value = 0;
                dest_value = soc_mem_field32_dest_get(unit,
                        L3_DEFIP_PAIR_128m, hw_entry,
                        DESTINATIONf, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                    lpm_cfg->defip_mc_group = dest_value;
                } else {
                    lpm_cfg->defip_mc_group = 0;
                }
            } else {
                lpm_cfg->defip_mc_group = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                hw_entry, L3MC_INDEXf);
            }
        }
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        uint32 _key_data[2] = {0, 0};
        uint8 ecmp = 0;
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_LEVEL1m, (void *)hw_entry, LWR_ASSOC_DATA0f, _key_data);
        ecmp = soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, DEST_TYPEf);
        if (ecmp) {
            lpm_cfg->defip_ecmp = TRUE;
            lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;
        } else {
            lpm_cfg->defip_ecmp = 0;
            lpm_cfg->defip_ecmp_count = 0;
        }
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx = soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, DESTINATIONf);
        }
        /* Mark entry as IPv6 */
        lpm_cfg->defip_flags |= BCM_L3_IP6;

        /* Get entry priority. */
        lpm_cfg->defip_prio = soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, PRIf);

        /* Get classification group id. */
        lpm_cfg->defip_lookup_class =
            soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, CLASS_IDf);

        /* Get priority override bit. */
        if (soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, RPEf))  {
            lpm_cfg->defip_flags |= BCM_L3_RPE;
        }

        /* Get destination discard field. */
        if (soc_format_field32_get(unit, ASSOC_DATA_FULLfmt, _key_data, DST_DISCARDf)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }

        /* Get hit bit. */
        if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT0f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }

        if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT1f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }

        return (BCM_E_NONE);
    }
#endif
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dstt = 0, dstv;
        dstv = soc_mem_field32_dest_get(unit, L3_DEFIP_PAIR_128m, hw_entry,
                                        DESTINATIONf, &dstt);
        if (dstt == SOC_MEM_FIF_DEST_ECMP) {
            lpm_cfg->defip_ecmp = TRUE;
            lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;
        } else {
            lpm_cfg->defip_ecmp = 0;
            lpm_cfg->defip_ecmp_count = 0;
        }
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx = dstv;
        }
    } else {
        /* Check if entry points to ecmp group. */
        if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, ECMPf)) {
            /* Mark entry as ecmp */
            lpm_cfg->defip_ecmp = TRUE;
            lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

            /* Get ecmp group id. */
            if (nh_ecmp_idx != NULL) {
                *nh_ecmp_idx = SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m,
                                                     hw_entry, ECMP_PTRf);
            }
        } else {
            /* Mark entry as non-ecmp. */
            lpm_cfg->defip_ecmp = 0;

            /* Reset ecmp group next hop count. */
            lpm_cfg->defip_ecmp_count = 0;

            /* Get next hop index. */
            if (nh_ecmp_idx != NULL) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_ipmc_defip) &&
                        (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
                    *nh_ecmp_idx = BCM_XGS3_L3_INVALID_INDEX;
                } else
#endif
                {
                    *nh_ecmp_idx = SOC_MEM_OPT_FLD32_GET(unit,
                        L3_DEFIP_PAIR_128m, hw_entry, NEXT_HOP_INDEXf);
                }
            }
        }
    }

    /* Mark entry as IPv6 */
    lpm_cfg->defip_flags |= BCM_L3_IP6;

    /* Get entry priority. */
    lpm_cfg->defip_prio = SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, PRIf);

    /* Get classification group id. */
    lpm_cfg->defip_lookup_class = 
        SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, CLASS_IDf);

    /* Get hit bit. */
    if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HITf)) {
        if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }
    }

    if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HIT0f)) {
        if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT0f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }
    }

    /* Get priority override bit. */
    if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, RPEf))  {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard field. */
    if (SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, DST_DISCARDf)) {
        lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_defip_pair128_shift
 * Purpose:
 *      Shift entries in the tcam. 
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      idx_src     - (IN)Entry source. 
 *      idx_dest    - (IN)Entry destination.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_shift(int unit, int idx_src, int idx_dest)  
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */
    soc_mem_t mem = L3_DEFIP_PAIR_128m;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        mem = L3_DEFIP_PAIR_LEVEL1m;
    }

    /* Don't copy uninstalled entries. */
    if (0 != BCM_DEFIP_PAIR128_ARR(unit)[idx_src].prefix_len) {
        /* Read entry from source index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_READ(unit, mem, idx_src, hw_entry));

        /* Write entry to the destination index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, mem, idx_dest, hw_entry));

        /* Shift corresponding  URPF/source lookup entries. */
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
         */
        if (SOC_URPF_STATUS_GET(unit) &&
            !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
            /* Read entry from source index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_READ(unit, mem,
                              idx_src + BCM_DEFIP_PAIR128_URPF_OFFSET(unit),
                              hw_entry));

            /* Write entry to the destination index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_WRITE(unit, mem,
                            idx_dest +  BCM_DEFIP_PAIR128_URPF_OFFSET(unit),
                            hw_entry));
        }
    }

    /* Copy software portion of the entry. */ 
    sal_memcpy (&BCM_DEFIP_PAIR128_ARR(unit)[idx_dest],
                &BCM_DEFIP_PAIR128_ARR(unit)[idx_src], 
                sizeof(_bcm_defip_pair128_entry_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_defip_pair128_shift_range
 * Purpose:
 *      Shift entries in the tcam. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      idx_start   - (IN)Start index
 *      idx_end     - (IN)End index
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_shift_range(int unit, int idx_start, int idx_end)  
{
    int idx;     /* Iteration index. */
    int rv = BCM_E_NONE;   /* Operaton status. */ 

    /* Shift entries down (1 index up) */
    if (idx_end > idx_start) {
        for (idx = idx_end - 1; idx >= idx_start; idx--) {
            rv = _bcm_defip_pair128_shift(unit, idx, idx + 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _bcm_defip_pair128_entry_clear(unit, idx_start);
    }

    /* Shift entries up (1 index down) */
    if (idx_end < idx_start) {
        for (idx = idx_end + 1; idx <= idx_start; idx++) {
            rv = _bcm_defip_pair128_shift(unit, idx, idx - 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _bcm_defip_pair128_entry_clear(unit, idx_start);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_defip_pair128_idx_alloc
 * Purpose:
 *      Shuffle the tcam in order to insert a new entry 
 *      based on vrf weighted prefix length.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Route info 
 *      hw_index    - (OUT)Allocated hw index. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_defip_pair128_idx_alloc(int unit, _bcm_defip_cfg_t *lpm_cfg, int *hw_index,
                             int nh_ecmp_idx)
{
    int  lkup_plen;                   /* Vrf weighted lookup prefix.    */
    uint16 hash = 0;                  /* Entry hash value.              */
    int  idx;                         /* Table iteration index.         */
    int  rv;                          /* Operation return status.       */
    int  shift_min = 0;               /* Minimum entry shift required.  */
    int  range_start = 0;             /* First valid location for pfx.  */
    int  range_end = BCM_DEFIP_PAIR128_IDX_MAX(unit);
                                      /* Last valid location for pfx.   */
    int  free_idx_before = -1;        /* Free index before range_start. */
    int  free_idx_after  = -1;        /* Free index after range_end.    */

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_index == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Calculate vrf weighted prefix length. */
    lkup_plen = _bcm_defip_lkup_plen_get(unit, lpm_cfg);

    /* Calculate entry hash. */
    BCM_IF_ERROR_RETURN(_bcm_defip_pair128_hash(unit, lpm_cfg, &hash));

    /* Make sure at least one slot is available. */
    if (BCM_DEFIP_PAIR128_USED_COUNT(unit) ==
                      BCM_DEFIP_PAIR128_TOTAL(unit)) {
        return (BCM_E_FULL);
    }

    /* 
     *  Find first valid index for the prefix;
     */ 
    for (idx = 0; idx <= BCM_DEFIP_PAIR128_IDX_MAX(unit); idx++) {
        /* Remember free entry before installation location. */
        if (0 == BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            free_idx_before = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen < BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            range_start = idx;
            continue;
        }
        break;
    }

    /* 
     *  Find last valid index for the prefix;
     */ 
    for (idx = BCM_DEFIP_PAIR128_IDX_MAX(unit); idx >= 0; idx--) {

        /* Remeber free entry after installation location. */
        if (0 == BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            free_idx_after = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen >  BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            range_end = idx;
            continue;
        }
        break;
    }

    /* Check if entry should be inserted at the end. */
    if (range_start == BCM_DEFIP_PAIR128_IDX_MAX(unit)) {
        /* Shift all entries from free_idx_before till the end 1 up. */   
        rv = _bcm_defip_pair128_shift_range(unit, BCM_DEFIP_PAIR128_IDX_MAX(unit), free_idx_before);

        if (BCM_SUCCESS(rv)) { 
            /* Allocated index is last TCAM entry. */
            *hw_index = BCM_DEFIP_PAIR128_IDX_MAX(unit);
            BCM_DEFIP_PAIR128_ENTRY_SET(unit, *hw_index, lkup_plen, hash);
        }
        return (rv);
    }

    /* Check if entry should be inserted at the beginning. */
    if (range_end < 0) {
        /* Shift all entries from 0 till free_idx_after 1 down. */
        rv = _bcm_defip_pair128_shift_range(unit, 0, free_idx_after);

        if (BCM_SUCCESS(rv)) {
            /* Allocated index is first TCAM entry. */
            *hw_index = 0;
            BCM_DEFIP_PAIR128_ENTRY_SET(unit, 0, lkup_plen, hash);
        }
        return (rv);
    }

    /* Find if there is any free index in range. */
    for (idx = range_start; idx <= range_end; idx++) {
        if (0 == BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            *hw_index = idx;
            BCM_DEFIP_PAIR128_ENTRY_SET(unit, idx, lkup_plen, hash);
            return (BCM_E_NONE);
        }
    }

    /* Check if entry should be inserted at the end of range. */
    if ((free_idx_after > range_end) && (free_idx_after != -1))  {
        shift_min = free_idx_after - range_end;
    } 

    /* Check if entry should be inserted in the beginning. */
    if ((free_idx_before < range_start) && (free_idx_before != -1))  {
        if((shift_min > (range_start - free_idx_before)) || (free_idx_after == -1)) {
            shift_min = -1;
        }
    }

    if (shift_min <= 0) {
        rv = _bcm_defip_pair128_shift_range(unit, range_start, free_idx_before);
        if (BCM_SUCCESS(rv)) {
            *hw_index= range_start;
            BCM_DEFIP_PAIR128_ENTRY_SET(unit, range_start, lkup_plen, hash);
        }
    } else {
        rv = _bcm_defip_pair128_shift_range(unit, range_end, free_idx_after);
        if (BCM_SUCCESS(rv)) {
            *hw_index = range_end;
            BCM_DEFIP_PAIR128_ENTRY_SET(unit, range_end, lkup_plen, hash);
        }
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_l3_defip_pair128_get
 * Purpose:
 *      Get an entry from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_l3_defip_pair128_get(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                              int *nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int clear_hit;                           /* Clear entry hit bit flag. */
    int hw_index;                            /* Entry offset in the TCAM. */ 
    int rv;                                  /* Internal operation status.*/
    soc_mem_t mem = L3_DEFIP_PAIR_128m;

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    /* Check if clear hit bit required. */
    clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Search for matching entry in the cam. */
    rv =  _bcm_defip_pair128_match(unit, lpm_cfg, hw_entry, &hw_index);
    BCM_IF_ERROR_RETURN(rv);

    lpm_cfg->defip_index = hw_index;

    /* Parse matched entry. */
    rv = _bcm_defip_pair128_parse(unit, hw_entry, lpm_cfg, nh_ecmp_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the HIT bit */
    if (clear_hit) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
            mem = L3_DEFIP_PAIR_LEVEL1m;
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT0f, 0);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT1f, 0);
        } else
#endif
        {
            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HITf)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf, 0);
            }

            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HIT0f)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT0f, 0);
            }

            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HIT1f)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT1f, 0);
            }
        }
        rv = BCM_XGS3_MEM_WRITE(unit, mem, hw_index, hw_entry);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_td3_lpm_pair128_ipv6mc_ent_init
 * Purpose:
 *      Setup key type, key mode, vrf  128b IPMCV6 entry in L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information.
 * Returns:
 *      BCM_E_XXX
 */
#if defined(BCM_TRIDENT3_SUPPORT)
void
_bcm_td3_lpm_pair128_ipv6mc_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg, uint32 *lpm_entry)
{
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE0_LWRf, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE1_LWRf, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE0_UPRf, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE1_UPRf, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE_MASK0_LWRf, 0xf);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE_MASK1_LWRf, 0xf);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE_MASK0_UPRf, 0xf);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_TYPE_MASK1_UPRf, 0xf);

    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE1_UPRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE0_UPRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE1_LWRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE0_LWRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE_MASK1_UPRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE_MASK0_UPRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE_MASK1_LWRf, 0x3);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, KEY_MODE_MASK0_LWRf, 0x3);
    if ((BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) && (BCM_L3_VRF_GLOBAL != lpm_cfg->defip_vrf)) {
        if (SOC_IS_FIREBOLT6(unit)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_12_10f, (lpm_cfg->defip_vrf >> 10));
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_9_0f, (lpm_cfg->defip_vrf & 0x3ff));
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_12_10_MASKf, 0x7);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_9_0_MASKf, 0x3ff);
        } else {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_11_4f, (lpm_cfg->defip_vrf >> 4));
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_3_0f, (lpm_cfg->defip_vrf & 0xf));
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_11_4_MASKf, 0xff);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_3_0_MASKf, 0xf);
        }
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_ROUTEf, 0);
    }
    else { /* Override or GLO */
        if (SOC_IS_FIREBOLT6(unit)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_12_10f, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_9_0f, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_12_10_MASKf, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_9_0_MASKf, 0);
        } else {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_11_4f, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_3_0f, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_11_4_MASKf, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_V6_VRF_ID_3_0_MASKf, 0);
        }
    }
}
#endif

/*
 * Function:
 *      _bcm_l3_defip_pair128_add
 * Purpose:
 *      Add an entry to internal L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_defip_pair128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                              int nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];   /* New entry hw buffer.      */
    bcm_ip6_t mask;                             /* Subnet mask.              */
    int hw_index = 0;                           /* Entry offset in the TCAM. */ 
    int rv = BCM_E_NONE;                        /* Operation return status.  */
    soc_mem_t mem = L3_DEFIP_PAIR_128m;
    int ipmc;
    int vrf_done=0;

#ifdef BCM_HELIX5_SUPPORT
    uint32  entry_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32  entry_hit;
#endif

    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);
    uint32 used_cnt = BCM_DEFIP_PAIR128_USED_COUNT(unit);
    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    if ((used_cnt >= max_v6_entries) && 
        !(lpm_cfg->defip_flags & BCM_L3_REPLACE)) {
        return BCM_E_FULL;     
    }

    /* No support for routes matchin AFTER vrf specific. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        return (BCM_E_UNAVAIL);
    }

    /* Zero buffers. */
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        lpm_cfg->defip_flow_handle != 0) {

        BCM_IF_ERROR_RETURN(
            _bcm_l3_defip_pair128_flex_add(unit, lpm_cfg, nh_ecmp_idx, hw_entry));
        return BCM_E_NONE;
    }
#endif

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Allocate a new index for inserted entry. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        rv = _bcm_defip_pair128_idx_alloc(unit, lpm_cfg, &hw_index, nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        hw_index = lpm_cfg->defip_index;
    }

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
        /* Set hit bit. */
        if (lpm_cfg->defip_flags & BCM_L3_HIT) {
            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HITf)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf, 1);
            }
            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HIT0f)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT0f, 1);
            }
            if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, HIT1f)) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT1f, 1);
            }
        }
    }

    /* Set ip mask. */
    ipmc = (soc_feature(unit, soc_feature_ipmc_defip) &&
           (lpm_cfg->defip_flags & BCM_L3_IPMC));

    _bcm_defip_pair128_ip6_mask_set(unit, mem, hw_entry, mask, ipmc);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        uint32              ip6_field;
        uint32 _key_data[2] = {0,0};
        uint32          fmte[4] = {0,0,0,0};

        mem = L3_DEFIP_PAIR_LEVEL1m;

        if (lpm_cfg->defip_flags & BCM_L3_HIT) {
            if (SOC_TH3_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_LEVEL1m, LWR_HIT0f)) {
                SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT0f, 1);
            }
            if (SOC_TH3_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_LEVEL1m, LWR_HIT1f)) {
                SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_HIT1f, 1);
            }
        }

        /* Set valid bits. */
        SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, UPR_VALID1f, 1);
        SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, UPR_VALID0f, 1);
        SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_VALID1f, 1);
        SOC_TH3_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_VALID0f, 1);

        ip6_field = ((lpm_cfg->defip_ip6_addr[12] << 24)| (lpm_cfg->defip_ip6_addr[13] << 16) |
                        (lpm_cfg->defip_ip6_addr[14] << 8) | (lpm_cfg->defip_ip6_addr[15] << 0));
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf,
                               (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) ? lpm_cfg->defip_vrf:0);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, LWR_KEY0f, _key_data);

        _key_data[0] = _key_data[1] = 0;
        ip6_field = ((lpm_cfg->defip_ip6_addr[8] << 24)| (lpm_cfg->defip_ip6_addr[9] << 16) |
                        (lpm_cfg->defip_ip6_addr[10] << 8) | (lpm_cfg->defip_ip6_addr[11] << 0));
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf,
                               (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) ? lpm_cfg->defip_vrf:0);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, LWR_KEY1f, _key_data);

        _key_data[0] = _key_data[1] = 0;
        ip6_field = ((lpm_cfg->defip_ip6_addr[4] << 24)| (lpm_cfg->defip_ip6_addr[5] << 16) |
                        (lpm_cfg->defip_ip6_addr[6] << 8) | (lpm_cfg->defip_ip6_addr[7] << 0));
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf,
                               (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) ? lpm_cfg->defip_vrf:0);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, UPR_KEY0f, _key_data);

        _key_data[0] = _key_data[1] = 0;
        ip6_field = ((lpm_cfg->defip_ip6_addr[0] << 24)| (lpm_cfg->defip_ip6_addr[1] << 16) |
                        (lpm_cfg->defip_ip6_addr[2] << 8) | (lpm_cfg->defip_ip6_addr[3] << 0));
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, IP_ADDRf, ip6_field);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_MODEf, 0x3);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, KEY_TYPEf, 0x1);
        soc_format_field32_set(unit, L3_DEFIP_TCAM_KEYfmt, _key_data, VRF_IDf,
                               (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) ? lpm_cfg->defip_vrf:0);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, UPR_KEY1f, _key_data);

        sal_memset(fmte, 0, sizeof(fmte));
        if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
            soc_format_field32_set(unit, FIXED_DATAfmt, fmte,
                                   SUB_DB_PRIORITYf, 2);
        } else if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
            soc_format_field32_set(unit, FIXED_DATAfmt, fmte,
                                   SUB_DB_PRIORITYf, 0);
        } else {
            soc_format_field32_set(unit, FIXED_DATAfmt, fmte,
                                   SUB_DB_PRIORITYf, 1);
        }
        soc_mem_field_set(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry,
                          LWR_FIXED_DATA0f, fmte);

        fmte[0] = fmte[1] = fmte[2] = fmte[3] = 0;
        if (lpm_cfg->defip_flags & BCM_L3_RPE) {
            soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, RPEf, 1);
        }
        soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, PRIf, lpm_cfg->defip_prio);
        soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, CLASS_IDf,
                                      lpm_cfg->defip_lookup_class);
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, DEST_TYPEf, 1);
        } else if (nh_ecmp_idx != BCM_XGS3_L3_INVALID_INDEX) {
            soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, DEST_TYPEf, 0);
        }
        soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, DESTINATIONf, nh_ecmp_idx);
        if (lpm_cfg->defip_flags & BCM_L3_RPE) {
            soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, RPEf, 1);
        }
        if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
            soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, DST_DISCARDf, 1);
        }
        soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, CLASS_IDf, lpm_cfg->defip_lookup_class);
        soc_format_field32_set(unit, ASSOC_DATA_FULLfmt, fmte, PRIf, lpm_cfg->defip_prio);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, LWR_ASSOC_DATA0f, fmte);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, LWR_ASSOC_DATA1f, fmte);
        fmte[0] = fmte[1] = fmte[2] = fmte[3] = 0;
        soc_format_field32_set(unit, ALPM1_DATAfmt, fmte, DIRECT_ROUTEf, 1);
        SOC_TH3_MEM_OPT_FLD_SET(unit, (uint32 *)hw_entry, LWR_ALPM1_DATA0f, fmte);
    } else
#endif
    {
        /* Set priority override bit. */
        if (lpm_cfg->defip_flags & BCM_L3_RPE) {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, RPEf, 1);
        }

        /* Set classification group id. */
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, CLASS_IDf,
                lpm_cfg->defip_lookup_class);

        /* Write priority field. */
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, PRIf, lpm_cfg->defip_prio);


        /* Fill next hop information. */
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, hw_entry,
                    DESTINATIONf, SOC_MEM_FIF_DEST_ECMP, nh_ecmp_idx);
            } else {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry,
                                      ECMP_PTRf, nh_ecmp_idx);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, ECMPf, 1);
            }
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, hw_entry,
                    DESTINATIONf, SOC_MEM_FIF_DEST_NEXTHOP, nh_ecmp_idx);
            } else {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry,
                                      NEXT_HOP_INDEXf, nh_ecmp_idx);
            }
        }

        /* Set destination discard flag. */
        if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, DST_DISCARDf, 1);
        }

        /* Set mode bits to indicate IP v6 128b pair mode */
        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, KEY_MODE0_UPRf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE1_UPRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE0_UPRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE1_LWRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE0_LWRf, 0x3);

            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE_MASK1_UPRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE_MASK0_UPRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE_MASK1_LWRf, 0x3);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, KEY_MODE_MASK0_LWRf, 0x3);
        } else {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE1_UPRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE0_UPRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE1_LWRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE0_LWRf, 0x3);

            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE_MASK1_UPRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE_MASK0_UPRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE_MASK1_LWRf, 0x3);
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MODE_MASK0_LWRf, 0x3);
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_ipmc_defip) &&
                (lpm_cfg->defip_flags & BCM_L3_IPMC)) {

        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, MULTICAST_ROUTEf, 1);
        } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, DATA_TYPEf, 2);
        }

        if (soc_feature(unit, soc_feature_generic_dest)) {
            if (lpm_cfg->defip_mc_group > 0) {
                soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, hw_entry,
                                         DESTINATIONf, SOC_MEM_FIF_DEST_IPMC,
                                         lpm_cfg->defip_mc_group);
            }
        } else {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, L3MC_INDEXf, lpm_cfg->defip_mc_group);
        }

            /*
             * RP ID has be encoded to 14 bit value, Now it will fit in EXPECTED_L3_IIFf
             */
            if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, EXPECTED_L3_IIFf,
                                        _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));

            } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) &&
                    (lpm_cfg->defip_expected_intf != 0))  {

                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, EXPECTED_L3_IIFf, lpm_cfg->defip_expected_intf);

                if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                    SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
                }
                if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                    SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, 1);
                }
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route)) {
                _bcm_td3_lpm_pair128_ipv6mc_ent_init(unit, lpm_cfg, hw_entry);
                vrf_done = 1;
            }
#endif
        }
#endif

        /* Set valid bits. */
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_UPRf, 1);
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_UPRf, 1);
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_LWRf, 1);
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_LWRf, 1);

        /* Set destination/source ip address. */
        _bcm_defip_pair128_ip6_addr_set(unit, L3_DEFIP_PAIR_128m, hw_entry, lpm_cfg->defip_ip6_addr, ipmc);

        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, VRF_ID_0_LWRf) && !vrf_done) {
            /* Set vrf id & vrf mask. */
            if (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_0_LWRf, lpm_cfg->defip_vrf);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_1_LWRf, lpm_cfg->defip_vrf);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_0_UPRf, lpm_cfg->defip_vrf);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_1_UPRf, lpm_cfg->defip_vrf);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK0_LWRf,
                                    (1 << SOC_MEM_OPT_FLD_LENGTH(unit, L3_DEFIP_PAIR_128m,
                                                               VRF_ID_MASK0_LWRf)) - 1);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK1_LWRf,
                                    (1 << SOC_MEM_OPT_FLD_LENGTH(unit, L3_DEFIP_PAIR_128m,
                                                               VRF_ID_MASK1_LWRf)) - 1);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK0_UPRf,
                                    (1 << SOC_MEM_OPT_FLD_LENGTH(unit, L3_DEFIP_PAIR_128m,
                                                               VRF_ID_MASK0_UPRf)) - 1);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK1_LWRf,
                                    (1 << SOC_MEM_OPT_FLD_LENGTH(unit, L3_DEFIP_PAIR_128m,
                                                               VRF_ID_MASK1_UPRf)) - 1);
                if (SOC_MEM_OPT_FLD_VALID(unit, L3_DEFIP_PAIR_128m, GLOBAL_ROUTEf)) {
                    SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, GLOBAL_ROUTEf, 0);
                }
            } else {
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_0_LWRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_1_LWRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_0_UPRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_1_UPRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK0_LWRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK1_LWRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK0_UPRf, 0);
                SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, VRF_ID_MASK1_UPRf, 0);
            }
        }

        /* Configure entry for SIP lookup if URPF is enabled. This => SIP is default route.*/
        /* If soc_feature_l3_defip_advanced_lookup is TRUE then
         * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
         * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
         */
        if (soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
            SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, DEFAULTROUTEf,
                (SOC_URPF_STATUS_GET(unit) ? 1 : 0));

        }
    }

    /* Write buffer to hw. */
#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit,soc_feature_update_defip_pair_data_only) &&
        (mem == L3_DEFIP_PAIR_128m) &&
        (lpm_cfg->defip_flags & BCM_L3_REPLACE) &&
        (BCM_XGS3_L3_INVALID_INDEX != lpm_cfg->defip_index)) {
        /* Zero buffers. */
        sal_memset(entry_data, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
        SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, DATA_RANGEf, entry_data);
        rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_DATA_ONLYm, hw_index, entry_data);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf, &entry_hit);
            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_HIT_ONLYm, hw_index, &entry_hit);
        }
    } else
#endif
    {
        rv = BCM_XGS3_MEM_WRITE(unit, mem, hw_index, hw_entry);
    }
    if (BCM_FAILURE(rv)) {
        BCM_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);
        return (rv);
    }

    /* Write source lookup portion of the entry. */
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup) &&
        SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_128m)) {
        SOC_MEM_OPT_FLD32_SET(unit, L3_DEFIP_PAIR_128m, hw_entry, SRC_DISCARDf, 0);
        if (SOC_IS_TRIDENT3(unit)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry,
                                FLEX_CTR_BASE_COUNTER_IDXf, 0);
        }

#if defined(BCM_HELIX5_SUPPORT)
        if (soc_feature(unit,soc_feature_update_defip_pair_data_only) &&
                (mem == L3_DEFIP_PAIR_128m) &&
                (lpm_cfg->defip_flags & BCM_L3_REPLACE) &&
                (BCM_XGS3_L3_INVALID_INDEX != lpm_cfg->defip_index)) {
            /* Zero buffers. */
            sal_memset(entry_data, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
            SOC_MEM_OPT_FLD_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, DATA_RANGEf, entry_data);
            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_DATA_ONLYm,
                    (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), entry_data);
            if (BCM_SUCCESS(rv)) {
                rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_HIT_ONLYm,
                        (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)),0);
            }
        } else
#endif
        {
            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m,
                    (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), hw_entry);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_defip_pair128_entry_clear(unit, hw_index);
            return (rv);
        }
    }

    /* If new route added increment total number of routes.  */
    /* Lack of index indicates a new route. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, TRUE);
        BCM_DEFIP_PAIR128_USED_COUNT(unit)++;
        soc_lpm_stat_128b_count_update(unit, 1);
    }

    return rv;

}

/*
 * Function:
 *      _bcm_l3_defip_pair128_del
 * Purpose:
 *      Delete an entry from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_l3_defip_pair128_del(int unit, _bcm_defip_cfg_t *lpm_cfg) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    uint32 temp_hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int hw_index;                            /* Entry offset in the TCAM. */ 
    int rv;    
    _bcm_defip_cfg_t temp_lpm;
    int temp_nh_ecmp_idx, idx;

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);
    /* Search for matching entry in theL3_DEFIP_PAIR_128  cam.  */
    rv =  _bcm_defip_pair128_match(unit, lpm_cfg, hw_entry, &hw_index);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Delete entry from hardware & software (L3_DEFIP_PAIR_128m). */
    rv = _bcm_defip_pair128_entry_clear(unit, hw_index);
    if (BCM_SUCCESS(rv)) {
        BCM_XGS3_L3_DEFIP_CNT_DEC(unit, TRUE);
        BCM_DEFIP_PAIR128_USED_COUNT(unit)--;
        soc_lpm_stat_128b_count_update(unit, 0);
        /* Delete valid entry at highest index (or lowest LPM) in 
         * L3_DEFIP_128m table if available
         * and add it to L3_DEFIP_PAIR_128m */
        hw_index = BCM_DEFIP_PAIR128_IDX_MAX(unit);
        for (idx = BCM_DEFIP_PAIR128_IDX_MAX(unit); idx >=0; idx--) {
            if (0 == BCM_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
                hw_index =  idx - 1;
                continue;
            }
            break;
        }

        rv = BCM_XGS3_MEM_READ(unit, L3_DEFIP_128m, hw_index, &temp_hw_entry);
        if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL) {  
            return rv;
        }
        if (rv != BCM_E_UNAVAIL) {
            sal_memset(&temp_lpm, 0, sizeof(_bcm_defip_cfg_t));
            /* Parse entry. */
            rv = _bcm_defip_pair128_parse(unit, temp_hw_entry,
                                                &temp_lpm, &temp_nh_ecmp_idx);
            BCM_IF_ERROR_RETURN(rv);
        
            /* Extract entry address vrf */
            rv =  _bcm_defip_pair128_get_key(unit, temp_hw_entry, &temp_lpm);
            BCM_IF_ERROR_RETURN(rv);

            /* Add the copied entry to L3_DEFIP_PAIR_128 table */
            temp_lpm.defip_index = BCM_XGS3_L3_INVALID_INDEX;
            rv = _bcm_l3_defip_pair128_add(unit, &temp_lpm, temp_nh_ecmp_idx);
            BCM_IF_ERROR_RETURN(rv);

            
            /* Delete entry from L3_DEFIP_128 table. */
            temp_lpm.defip_index = hw_index;
            rv = BCM_E_UNAVAIL;
            rv = _bcm_l3_defip_pair128_del(unit, &temp_lpm);
            return rv;
        }
        return BCM_E_NONE;  
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_l3_defip_pair128_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_defip_pair128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int idx;                                 /* Iteration index.           */
    int rv;                                  /* Operation return status.   */
    int cmp_result;                          /* Test routine result.       */
    int nh_ecmp_idx;                         /* Next hop/Ecmp group index. */
    uint32 *hw_entry;                        /* Hw entry buffer.           */
    char *lpm_tbl_ptr;                       /* Dma table pointer.         */
    int defip_pair128_tbl_size;                    /* Defip table size.          */
    _bcm_defip_cfg_t lpm_cfg;                /* Buffer to fill route info. */
    _bcm_trvs_range_t *range_info;
    int max_index,start_index = 0;               /* START/MAX index to traverse on Entries. */
    soc_mem_t  mem = L3_DEFIP_PAIR_128m;
    int validf = 0;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
        mem = L3_DEFIP_PAIR_LEVEL1m;
    }
    /* Table DMA the LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, mem, BCM_L3_MEM_ENT_SIZE(unit, mem),
                             "lpm_tbl", &lpm_tbl_ptr, &defip_pair128_tbl_size));

    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        defip_pair128_tbl_size >>= 1;
        BCM_DEFIP_PAIR128_URPF_OFFSET(unit) = defip_pair128_tbl_size;
    } 

    max_index = defip_pair128_tbl_size - 1;
    if (!(trv_data->flags & BCM_XGS3_L3_OP_DEL)) {
        range_info = trv_data->pattern;
        if ((range_info != NULL) && (defip_pair128_tbl_size > range_info->end)) {
            max_index = range_info->end;
            start_index = range_info->start;
            range_info->current_count =  start_index;
        }
    }

    for (idx = start_index ; idx <= max_index; idx++) {
        /* Calculate entry ofset. */
        hw_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                lpm_tbl_ptr, idx);

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, L3_DEFIP_PAIR_LEVEL1m)) {
            validf = SOC_TH3_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_LEVEL1m, hw_entry, LWR_VALID0f);
        } else
#endif
        {
            validf = SOC_MEM_OPT_FLD32_GET(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_LWRf);
        }

        /* Make sure entry is valid. */
        if (!validf) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        /* Parse  the entry. */
        _bcm_defip_pair128_parse(unit, hw_entry, &lpm_cfg, &nh_ecmp_idx);
       lpm_cfg.defip_index = idx;

        /* Fill entry ip address &  subnet mask. */
        _bcm_defip_pair128_get_key(unit, hw_entry, &lpm_cfg);

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                     (void *)&lpm_cfg,
                                     (void *)&nh_ecmp_idx, &cmp_result);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, lpm_tbl_ptr);
                return rv;
            }
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            rv = _bcm_defip_pair128_reinit(unit, idx, &lpm_cfg);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, lpm_tbl_ptr);
                return rv;
            }
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
    
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fb_lpm_reinit_done(unit, 1));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    soc_cm_sfree(unit, lpm_tbl_ptr);
    return (BCM_E_NONE);
}

/* Configure L3_DEFIP_KEY_SELr register */
int
_bcm_l3_defip_key_sel_set(int unit, int urpf, int tcam_pair_count)
{
    int i;
    soc_reg_t key_selr = L3_DEFIP_KEY_SELr;
    uint32 defip_key_sel_val = 0;
    soc_field_t key_sel_f[4] = {
        V6_KEY_SEL_CAM0_1f,
        V6_KEY_SEL_CAM2_3f,
        V6_KEY_SEL_CAM4_5f,
        V6_KEY_SEL_CAM6_7f
    };
    /* Trident3 Style */
    soc_field_t key_sel_f2[4] = {
        KEY_SEL_CAM0_1f,
        KEY_SEL_CAM2_3f,
        KEY_SEL_CAM4_5f,
        KEY_SEL_CAM6_7f
    };

#ifdef BCM_UTT_SUPPORT
    if (soc_feature(unit, soc_feature_utt)) {
        BCM_IF_ERROR_RETURN
            (soc_utt_lpm_bank_config(unit, urpf,
                                         tcam_pair_count));
        if (urpf) {
            BCM_IF_ERROR_RETURN
                (soc_utt_lpm_urpf_paired_tcams(unit, &tcam_pair_count));
        }
        /* Convert tcam bank pair count to multiple UTT lookups
         * Each lookup can have more than 1 TCAM bank pairs */
        if (SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) > 2) {
            tcam_pair_count =
                ((tcam_pair_count /
                  (SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2)) +
                 ((tcam_pair_count %
                   (SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2)) ?
                  1 : 0));

            /*
             * Mul by 2: Switch case is operated based on non-urpf pair count
             */
            if (urpf) {
                tcam_pair_count *= 2;
            }
        }
    }
#endif

    /* Key sel value
     *   2,   0x00b non-URPF + lpm-64b
     *   0,   0x01b non-URPF + lpm-128b
     *   3,   0x10b uRPF + lpm-64b
     *   1    0x11b uRPF + lpm-128b
     */
    if (tcam_pair_count < 0 || tcam_pair_count > 4) {
        return BCM_E_NONE;
    }

    if (soc_reg_field_valid(unit, key_selr, KEY_SEL_CAM0_1f)) {
        /* Trident3 Style */
        if (urpf) {
            switch (tcam_pair_count) {
                case 0:
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[0], 2);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[1], 2);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[2], 3);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[3], 3);
                    break;
                case 1:
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[0], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[1], 2);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[2], 1);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[3], 3);
                    break;
                case 2:
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[0], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[1], 2);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[2], 1);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[3], 3);
                    break;
                case 3:
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[0], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[1], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[2], 1);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[3], 1);
                    break;
                case 4:
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[0], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[1], 0);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[2], 1);
                    soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                      key_sel_f2[3], 1);
                    break;
                /* coverity[dead_error_begin : FALSE] */
                default:
                    break;
            }
        } else {
            for (i = 0; i < (4 - tcam_pair_count); i++) {
                soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                  key_sel_f2[3 - i], 2);
            }
        }
    } else {
        for (i = 0; i < tcam_pair_count; i++) {
            soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                              key_sel_f[i], 0x1);
        }

        if (urpf) {
            soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                              URPF_LOOKUP_CAM4f, 0x1);
            soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                              URPF_LOOKUP_CAM5f, 0x1);
            soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                              URPF_LOOKUP_CAM6f, 0x1);
            soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                              URPF_LOOKUP_CAM7f, 0x1);

            if (tcam_pair_count == 1) {
                soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                  key_sel_f[2], 0x1);
            } else if (tcam_pair_count == 2) {
                soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                  key_sel_f[1], 0);
                soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                  key_sel_f[2], 0x1);
            } else if (tcam_pair_count == 3) {
                soc_reg_field_set(unit, key_selr, &defip_key_sel_val,
                                  key_sel_f[3], 0x1);
            }
        }
    }

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(unit, defip_key_sel_val));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l3_defip_init
 * Purpose:
 *      Configure TD2 internal route table as per soc properties.
 *      (tcam pairing for IPv4, 64/128 bit IPV6 prefixes)
 * Parameters:
 *      unit     - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_defip_init(int unit)
{
    int num_ipv6_128b_entries;
    int mem_v4, mem_v6, mem_v6_128; /* Route table memories */
    int ipv6_64_depth = 0;
    int start_index = 0;
    int tcam_pair_count = 0;
#if defined (BCM_WARM_BOOT_SUPPORT) && defined (BCM_UTT_SUPPORT)
    int urpf_tcam_pair_count = 0;
    int max_paired_count = (SOC_L3_DEFIP_MAX_TCAMS_GET(unit)) / 2;
#endif
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = num_ipv6_128b_entries;
#ifdef ALPM_ENABLE
    /* ALPM Mode */
    if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        int rv = BCM_E_NONE;
        if (soc_feature(unit, soc_feature_alpm)) {
            if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
                rv = _bcm_td2_alpm_trace_init(unit);
                if (BCM_SUCCESS(rv)) {
                rv = soc_alpm_init(unit);
                }
            }
#if defined(BCM_TOMAHAWK_SUPPORT)
            else if (soc_feature(unit, soc_feature_alpm2)) {
                rv = bcm_esw_alpm_init(unit);
            }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
            else {
                rv = _bcm_td2_alpm_trace_init(unit);
                if (BCM_SUCCESS(rv)) {
                rv = soc_th_alpm_init(unit);
                }
            }
#endif
            if (BCM_SUCCESS(rv)) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "ALPM mode initialized\n")));
            } else {
                LOG_ERROR(BSL_LS_BCM_ALPM,
                          (BSL_META_U(unit,
                                      "ALPM mode initialization failed, "
                                      "retVal = %d\n"), rv));
            }
            return rv;
        } else {
            LOG_WARN(BSL_LS_BCM_ALPM,
                     (BSL_META_U(unit,
                      "ALPM feature is not available in this device.\n")));
        }
    }
#else /* ALPM_ENABLE */
    if (soc_feature(unit, soc_feature_alpm) &&
        soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        LOG_WARN(BSL_LS_BCM_ALPM,
                 (BSL_META_U(unit,
                             "ALPM mode support is not compiled. Please, "
                             "recompile the SDK with ALPM_ENABLE proprocessor variable "
                             "defined\n")));
    }
#endif /* ALPM_ENABLE */

    SOC_IF_ERROR_RETURN(soc_fb_lpm_tcam_pair_count_get(unit, &tcam_pair_count));

    if (!SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_l3_defip_key_sel_set(unit,
            SOC_URPF_STATUS_GET(unit), tcam_pair_count));
    }
    if(!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
            ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIP_LEVEL1m);
        }
    } else {
        ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIPm);
    }

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));

    /* Overriding defaults set during lpm init */

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm128_init(unit));
        ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
        start_index = (tcam_pair_count * tcam_depth * 2);
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            if(SOC_URPF_STATUS_GET(unit)) {
#ifdef BCM_UTT_SUPPORT
                if (soc_feature(unit, soc_feature_utt)) {
                    urpf_tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
                    ipv6_64_depth = (max_paired_count - (urpf_tcam_pair_count * 2)) * tcam_depth;
                    start_index = urpf_tcam_pair_count * 2 * tcam_depth;
                } else
#endif
                {
                    switch (tcam_pair_count) {
                        case 0:
                            /* No pairing of tcams */
                            ipv6_64_depth = 4 * tcam_depth;
                            break;
                        case 1:
                        case 2:
                            start_index = 2 * tcam_depth;
                            ipv6_64_depth = 2 * tcam_depth;
                            break;
                        case 3:
                        case 4:
                            start_index = 4 * tcam_depth;
                            break;
                        default:
                            start_index = 0;
                            ipv6_64_depth = 0;
                            break;
                    }
                }
            }
        }
#endif
    } else {
        start_index = 0;
    }

    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit)))   = start_index - 1;

    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit))) = ipv6_64_depth;
    BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                                                start_index));    

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            BCM_IF_ERROR_RETURN(_bcm_defip_pair128_init(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l3_defip_deinit
 * Purpose:
 *      De-initialize route table for trident2 devices.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_l3_defip_deinit(int unit)
{
    soc_mem_t mem_v4;      /* IPv4 Route table memory.             */
    soc_mem_t mem_v6;      /* IPv6 Route table memory.             */
    soc_mem_t mem_v6_128;  /* IPv6 full prefix route table memory. */

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v4 != mem_v6) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
    }

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
         if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {   
             BCM_IF_ERROR_RETURN(soc_fb_lpm128_deinit(unit));   
         } else {
             BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
         } 
    }

#ifdef ALPM_ENABLE
    /* Delete WAR entries before uRPF status change, or we may not  
     * be able to delete the old WAR entries after status change, 
     * because alpm_lpm_deinit is currently inside alpm_lpm_init which
     * will try to delete WAR uRPF entry which is not exist before uPRF 
     * status change.
     */
    (void) soc_alpm_ipmc_war(unit, FALSE);
    (void) soc_alpm_128_ipmc_war(unit, FALSE);
#endif

    return (BCM_E_NONE);
}

STATIC int
_bcm_l3_lpm_table_sizes_get(int unit, int *paired_table_size, 
                            int *defip_table_size)
{
    return soc_fb_lpm_table_sizes_get(unit, paired_table_size, 
                                      defip_table_size);
}

int 
_bcm_l3_is_v4_64b_allowed_in_paired_tcam(int unit)
{
    return soc_fb_lpm128_is_v4_64b_allowed_in_paired_tcam(unit);
}

int
_bcm_l3_scaled_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    int rv = BCM_E_NOT_FOUND;
    int defip_table_size = 0;
    int paired_table_size = 0;
 
    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
       return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_l3_lpm_table_sizes_get(unit, &paired_table_size,
                                                    &defip_table_size));

    if (paired_table_size == 0 && lpm_cfg->defip_sub_len > 64) {
        return BCM_E_NOT_FOUND;
    }

    if ((paired_table_size != 0) && 
        (lpm_cfg->defip_sub_len > 64 || defip_table_size == 0)) {
        return  _bcm_fb_lpm128_get(unit, lpm_cfg, nh_ecmp_idx);
    }
  
    rv = _bcm_fb_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
    if ((rv == BCM_E_NOT_FOUND) && 
         _bcm_l3_is_v4_64b_allowed_in_paired_tcam(unit)) {
        rv =  _bcm_fb_lpm128_get(unit, lpm_cfg, nh_ecmp_idx);
        if (BCM_FAILURE(rv)) {
            lpm_cfg->defip_flags_high &= ~BCM_XGS3_L3_ENTRY_IN_DEFIP_PAIR;
        }
    }
 
    return rv;
}

/*
 * Function:
 *      _bcm_l3_lpm_get
 * Purpose:
 *      Get an entry from the route table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);
  
    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_flex_flow)
            && (lpm_cfg->defip_flow_handle != 0)) {
        soc_mem_t view_id;
        soc_flow_db_mem_view_info_t vinfo;

        BCM_IF_ERROR_RETURN( soc_flow_db_ffo_to_mem_view_id_get(unit,
                    lpm_cfg->defip_flow_handle,
                    lpm_cfg->defip_flow_option_handle,
                    SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                    (uint32 *)&view_id));
        BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, view_id, &vinfo));
        mem = vinfo.mem;
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(_bcm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                    lpm_cfg->defip_sub_len, &mem));
    }

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return _bcm_l3_scaled_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
    }

    switch (mem) {
        case L3_DEFIP_PAIR_128m:
        case L3_DEFIP_PAIR_LEVEL1m:
            if (max_v6_entries > 0) {
                return _bcm_l3_defip_pair128_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;  
        default:
            if (soc_mem_index_count(unit, mem) > 0) {
                return _bcm_fb_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
    }

    return (BCM_E_NOT_FOUND);
}

void
_bcm_defip_cfg_t_dump(_bcm_defip_cfg_t *lpm_cfg)
{
    int i = 0;

    if (lpm_cfg) {
        LOG_CLI((BSL_META("flags: 0x%x  vrf: %d\n"), lpm_cfg->defip_flags, lpm_cfg->defip_vrf));
        if (lpm_cfg->defip_flags & BCM_L3_IP6) {
            LOG_CLI((BSL_META("defip_ip6_addr - ")));
            for (i = 0; i < sizeof(bcm_ip6_t); i++) {
                LOG_CLI((BSL_META("0x%x:"), lpm_cfg->defip_ip6_addr[i]));
            }
            LOG_CLI((BSL_META("\n")));
        } else {
            LOG_CLI((BSL_META("defip_ip_addr: 0x%x\n"), lpm_cfg->defip_ip_addr));
        }
        LOG_CLI((BSL_META("defip_sub_len: %d defip_index: %d\n"), 
                 lpm_cfg->defip_sub_len, lpm_cfg->defip_index));
        for(i = 0; i < sizeof(bcm_mac_t); i++) {
            LOG_CLI((BSL_META("defip_mac_addr - ")));
            LOG_CLI((BSL_META("0x%x:"), lpm_cfg->defip_mac_addr[i]));
        }

        if (lpm_cfg->defip_flags & BCM_L3_IP6) {
            LOG_CLI((BSL_META("\ndefip_nexthop_ip6 - ")));
            for (i = 0; i < sizeof(bcm_ip6_t); i++) {
                LOG_CLI((BSL_META("0x%x:"), lpm_cfg->defip_nexthop_ip6[i])); 
            }
            LOG_CLI((BSL_META("\n")));
        } else {
            LOG_CLI((BSL_META("defip_nexthop_ip: 0x%x\n"), lpm_cfg->defip_nexthop_ip));
        }

        LOG_CLI((BSL_META("defip_tunnel: %d defip_prio: %d\n"), 
                 lpm_cfg->defip_tunnel, lpm_cfg->defip_prio));

        LOG_CLI((BSL_META("defip_intf: %d defip_port_tgid: %d\n"), 
                 lpm_cfg->defip_intf, lpm_cfg->defip_port_tgid));

        LOG_CLI((BSL_META("defip_stack_port: %d defip_modid: %d\n"), 
                 lpm_cfg->defip_stack_port, lpm_cfg->defip_modid));

        LOG_CLI((BSL_META("defip_vid: %d defip_ecmp: %d\n"), 
                 lpm_cfg->defip_vid, lpm_cfg->defip_ecmp));

        LOG_CLI((BSL_META("defip_ecmp_count: %d defip_ecmp_index: %d\n"), 
                 lpm_cfg->defip_ecmp_count, lpm_cfg->defip_ecmp_index));

        LOG_CLI((BSL_META("defip_l3hw_index: %d defip_tunnel_option: %d\n"), 
                 lpm_cfg->defip_l3hw_index, lpm_cfg->defip_tunnel_option));

        LOG_CLI((BSL_META("defip_mpls_label: %d defip_lookup_class: %d\n"),
                 lpm_cfg->defip_mpls_label, lpm_cfg->defip_lookup_class));
    }
}

/* When a entry is delete in unpaired TCAM, then check if you can move
 * 64B V6 or V4 entries back to unpaired TCAM. This is required to keep
 * the entries in order
 */
int _bcm_l3_lpm128_ripple_entries(int unit)
{
    _bcm_defip_cfg_t lpm_cfg_array[2];
    int nh_ecmp_idx[2];
    uint32 e[SOC_MAX_MEM_FIELD_WORDS];
    uint32 eupr[SOC_MAX_MEM_FIELD_WORDS];
    int index   = 0;
    int pfx_len = 0;
    int count   = 0;
    int ipv6    = 0;
    int rv = BCM_E_NONE;
    int idx = 0;
    soc_mem_t mem = L3_DEFIPm;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
        mem = L3_DEFIP_LEVEL1m;
    }
    /* Do the following 
     * 1. Find the smallest 64BV6 prefix
     * 2. if 64BV6 entry not found, find the smallest V4 prefix
     * 3. Try to add it to the unpaired tcam
     * 4. If adding to the unpaired tcam is successful, 
     *    then delete the entry from paired TCAM
     */
    sal_memcpy(e, soc_mem_entry_null(unit, mem),
               soc_mem_entry_words(unit,mem) * 4);
    sal_memcpy(eupr, soc_mem_entry_null(unit, mem),
               soc_mem_entry_words(unit,mem) * 4);
    ipv6 = 1; 
    rv = _bcm_fb_lpm128_get_smallest_movable_prefix(unit, ipv6, e, eupr, &index,
                                                    &pfx_len, &count); 
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_NOT_FOUND) {
            ipv6 = 0; 
            rv = _bcm_fb_lpm128_get_smallest_movable_prefix(unit, ipv6, e,eupr,
                                                            &index, &pfx_len,
                                                            &count);
            if (BCM_FAILURE(rv)) {
                return BCM_E_NONE;
            }
        } else {
            return rv;
        }
    }
    
    sal_memset(lpm_cfg_array, 0, sizeof(lpm_cfg_array));
    if (ipv6) {
        BCM_IF_ERROR_RETURN(_bcm_fb_lpm128_defip_cfg_get(unit, e, eupr,
                                                      lpm_cfg_array,
                                                      nh_ecmp_idx));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_fb_lpm_defip_cfg_get(unit, ipv6, e,
                                                      lpm_cfg_array,
                                                      nh_ecmp_idx));
    }
   for (idx = 0; idx < count; idx++) {
#ifdef BCM_TOMAHAWK2_SUPPORT
       if (soc_feature(unit, soc_feature_half_of_l3_defip_ipv4_capacity) &&
           !(idx & 1)) {
           continue;
       }
#endif
       lpm_cfg_array[idx].defip_index = BCM_XGS3_L3_INVALID_INDEX;
       lpm_cfg_array[idx].defip_flags_high = 0;
       rv = _bcm_fb_lpm_add(unit, &lpm_cfg_array[idx], 
                                      nh_ecmp_idx[idx]);
       if (BCM_FAILURE(rv)) {
           return BCM_E_NONE;
       }
       BCM_IF_ERROR_RETURN(_bcm_fb_lpm128_del(unit, &lpm_cfg_array[idx]));
   }

   return BCM_E_NONE;
}

int
_bcm_l3_lpm_ripple_entries(int unit, _bcm_defip_cfg_t *lpm_cfg,
                           int new_nh_ecmp_idx)
{

    _bcm_defip_cfg_t lpm_cfg_array[2];
    int nh_ecmp_idx[2];
    uint32 e[SOC_MAX_MEM_FIELD_WORDS];
    int index   = 0;
    int pfx_len = 0;
    int count   = 0;
    int ipv6    = 0;
    int do_ripple = 0;
    int rv = BCM_E_NONE;
    int new_is_ipv6 = 0, move_is_ipv6 = 0;
    int new_is_ipmc = 0, move_is_ipmc = 0;
    int idx = 0;
    soc_mem_t mem = L3_DEFIPm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
        mem = L3_DEFIP_LEVEL1m;
    }
#endif

    /* Do the following 
     * 1. Find the largest V4 prefix
     * 2. if V4 entry not found, find the largest 64B V6 prefix
     * 3. Try to add it to the paired tcam
     * 4. If adding to the paired tcam is successful, 
     *    then delete the entry
     * 5. Add this entry again
     */
    sal_memcpy(e, soc_mem_entry_null(unit, mem),
               soc_mem_entry_words(unit, mem) * 4);
    sal_memset(&lpm_cfg_array[0], 0x0, sizeof(_bcm_defip_cfg_t));
    sal_memset(&lpm_cfg_array[1], 0x0, sizeof(_bcm_defip_cfg_t));

    ipv6 = 0; 
    rv = _bcm_fb_get_largest_prefix(unit, ipv6, e, &index, 
                                   &pfx_len, &count); 
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_NOT_FOUND) {
            ipv6 = 1; 
            rv = _bcm_fb_get_largest_prefix(unit, ipv6, e,
                                           &index,
                                           &pfx_len, &count);
            if (BCM_FAILURE(rv)) {
                /* Failure here indicates something fatal */
                return BCM_E_INTERNAL; 
            }
        } else {
            return BCM_E_FULL;
        }
    }

    /*
     * If atomic write is enabled, one free entry is reserved to move
     * the entries during entry update.
     * count == 0 means HW doesn't hold any entry however SW v4 or v6
     * prefix entries still holds a free entry after deleting all
     * the entries in unpaired tcam.
     */
    if (soc_feature(unit, soc_feature_lpm_atomic_write) && (count == 0)) {
       return _bcm_fb_lpm128_add(unit, lpm_cfg, new_nh_ecmp_idx);
    }

    BCM_IF_ERROR_RETURN(_bcm_fb_lpm_defip_cfg_get(unit, ipv6, e, lpm_cfg_array,
                                                 nh_ecmp_idx));
    new_is_ipv6 = !!(lpm_cfg->defip_flags & BCM_L3_IP6);
    move_is_ipv6 = !!(lpm_cfg_array[0].defip_flags & BCM_L3_IP6);
    /*
     * move | new
     *------------
     *  v6  | v6    length(move) > length(new)      ripple
     *  v4  | v4    length(move) > length(new)      ripple
     *  v4  | v6    ripple
     *  v6  | v4    add to pair128
 */
    if (soc_feature(unit, soc_feature_td3_lpm_ipmc_war)) {
        new_is_ipmc = !!(lpm_cfg->defip_flags & BCM_L3_IPMC);
        move_is_ipmc = !!(lpm_cfg_array[0].defip_flags & BCM_L3_IPMC);
    }

    if (!new_is_ipv6 && move_is_ipv6) {
        do_ripple = 0;
    } else if ((new_is_ipv6 && !move_is_ipv6) ||
        ((move_is_ipmc << 8 | lpm_cfg_array[0].defip_sub_len) >
         (new_is_ipmc << 8 | lpm_cfg->defip_sub_len))) {
        /* Ripple move to pair128 */
        do_ripple = 1;
    }

    if (do_ripple) {
        for (idx = 0; idx < count; idx++) {
            lpm_cfg_array[idx].defip_index = BCM_XGS3_L3_INVALID_INDEX;
#ifdef BCM_TOMAHAWK2_SUPPORT            
            if (soc_feature(unit, soc_feature_half_of_l3_defip_ipv4_capacity) && 
                !(idx & 1)) {
                continue;
            }
#endif            
            rv = _bcm_fb_lpm128_add(unit, &lpm_cfg_array[idx], 
                                           nh_ecmp_idx[idx]);
            lpm_cfg_array[idx].defip_flags_high = 0x0;
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            BCM_IF_ERROR_RETURN(_bcm_fb_lpm_del(unit, &lpm_cfg_array[idx]));
        }
        return _bcm_fb_lpm_add(unit, lpm_cfg, new_nh_ecmp_idx);
    } else {
        /* Add new to pair128 */
        return _bcm_fb_lpm128_add(unit, lpm_cfg, new_nh_ecmp_idx);
    }

   return BCM_E_NONE;
}

/*
 * Update the LPM entry for L3_DEFIP_DATA_ONLY view during Entry Modify.
 */
STATIC int
_bcm_l3_scaled_lpm_data_modify(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
   uint32 lpm_hit[SOC_MAX_MEM_WORDS];
   uint32 lpm_hit_copy[SOC_MAX_MEM_WORDS];
   uint32 lpm_data[SOC_MAX_MEM_WORDS];
   uint32 lpm_data_copy[SOC_MAX_MEM_WORDS];
   uint32 defip_entry_lwr[SOC_MAX_MEM_WORDS];
   uint32 defip_entry_upr[SOC_MAX_MEM_WORDS];
   uint32 defip_entry[SOC_MAX_MEM_WORDS];
   int defip_change = FALSE;
   int defip_index = lpm_cfg->defip_index;
   int defip_pair_index = -1;
   int ipv6 = (lpm_cfg->defip_flags & BCM_L3_IP6);
   int pfx_len = lpm_cfg->defip_sub_len;
   int rv = BCM_E_NONE;
   int mode;


   /* Set mode  */
   if (ipv6 == 0) {
      mode = 0;
      defip_index >>= 1;
   } else if (pfx_len > 64 ||
              lpm_cfg->defip_flags_high & BCM_XGS3_L3_ENTRY_IN_DEFIP_PAIR) {
      mode = 3;
   } else {
      mode = 1;
   }

   rv = READ_L3_DEFIP_DATA_ONLYm(unit, MEM_BLOCK_ANY,
                                 defip_index, (uint32 *)lpm_data_copy);
   if (BCM_FAILURE(rv)) {
      return rv;
   }
   rv = READ_L3_DEFIP_HIT_ONLYm(unit, MEM_BLOCK_ANY,
                                defip_index, lpm_hit_copy);
   if (BCM_FAILURE(rv)) {
      return rv;
   }

   sal_memcpy(lpm_data, lpm_data_copy, sizeof(lpm_data));
   sal_memcpy(lpm_hit, lpm_hit_copy, sizeof(lpm_hit));

   if ((ipv6) || (lpm_cfg->defip_index & 1)) {
      rv = _bcm_fb_lpm_prepare_defip_data(unit, lpm_cfg,
                                          nh_ecmp_idx,
                                          (uint32 *)lpm_data, lpm_hit, 1);
      if (BCM_FAILURE(rv)) {
         return rv;
      }
   }

   if ((ipv6) || !(lpm_cfg->defip_index & 1)) {
      rv = _bcm_fb_lpm_prepare_defip_data(unit, lpm_cfg,
                                          nh_ecmp_idx,
                                          (uint32 *)lpm_data, lpm_hit, 0);
   }

   if (BCM_FAILURE(rv)) {
      return rv;
   }
   

   if (sal_memcmp(lpm_data_copy, lpm_data, sizeof(lpm_data))) {
      rv = WRITE_L3_DEFIP_DATA_ONLYm(unit, MEM_BLOCK_ANY,
                                     defip_index, lpm_data);
      if (BCM_FAILURE(rv)) {
         return rv;
      }
      /* In case of 128 bit entries */
      if (mode == 3) {
         defip_pair_index = defip_index + SOC_CONTROL(unit)->l3_defip_tcam_size;
         rv = WRITE_L3_DEFIP_DATA_ONLYm(unit, MEM_BLOCK_ANY,
                                        defip_pair_index, lpm_data);
         if (BCM_FAILURE(rv)) {
            return rv;
         }
      }
      defip_change = TRUE;
   }
   
   if (sal_memcmp(&lpm_hit_copy, lpm_hit, sizeof(lpm_hit))) {
      rv = WRITE_L3_DEFIP_HIT_ONLYm(unit, MEM_BLOCK_ANY,
                                    defip_index, &lpm_hit);
      if (BCM_FAILURE(rv)) {
         return rv;
      }
      /* In case of 128 bit entries */
      if (mode == 3) {
         defip_pair_index = defip_index + SOC_CONTROL(unit)->l3_defip_tcam_size;
         rv = WRITE_L3_DEFIP_HIT_ONLYm(unit, MEM_BLOCK_ANY,
                                       defip_pair_index, &lpm_hit);
         if (BCM_FAILURE(rv)) {
            return rv;
         }
      }
      defip_change = TRUE;
   }

   /* The cache of L3_DEFIP table should be updated if necessary */
   if (defip_change && soc_mem_cache_get(unit, L3_DEFIPm, MEM_BLOCK_ALL)) {
       sal_memset(&defip_entry, 0x0, BCM_XGS3_L3_ENT_SZ(unit, defip));
       rv = _soc_mem_alpm_read_cache(unit, L3_DEFIPm, MEM_BLOCK_ANY,
                                     defip_index, &defip_entry);
       if (SOC_SUCCESS(rv)) {
           sal_memset(&defip_entry_lwr, 0x0, BCM_XGS3_L3_ENT_SZ(unit, defip));
           if (3 == mode) {
               sal_memset(&defip_entry_upr, 0x0,
                          BCM_XGS3_L3_ENT_SZ(unit, defip));
               (void) _bcm_fb_lpm_prepare_defip_entry(
                          unit, lpm_cfg, nh_ecmp_idx,
                          defip_entry_lwr, defip_entry_upr);

               _soc_mem_alpm_write_cache(unit, L3_DEFIPm, MEM_BLOCK_ALL,
                                         defip_index, defip_entry_lwr);
               _soc_mem_alpm_write_cache(unit, L3_DEFIPm, MEM_BLOCK_ALL,
                                         defip_index + SOC_L3_DEFIP_TCAM_DEPTH_GET(unit),
                                         defip_entry_upr);
           } else {
               (void) _bcm_fb_lpm_prepare_defip_entry(
                          unit, lpm_cfg, nh_ecmp_idx,
                          defip_entry_lwr, NULL);
               if (!ipv6) {
                   if (lpm_cfg->defip_index & 0x1) {
                       (void) soc_fb_lpm_ip4entry0_to_1(
                                  unit, &defip_entry_lwr,
                                  defip_entry, PRESERVE_HIT);
                   } else {
                       (void) soc_fb_lpm_ip4entry0_to_0(
                                  unit, defip_entry_lwr,
                                  defip_entry, PRESERVE_HIT);
                   }
               }
               _soc_mem_alpm_write_cache(
                   unit, L3_DEFIPm, MEM_BLOCK_ALL, defip_index,
                   ipv6 ? defip_entry_lwr : defip_entry);
           }
       }
   }

   return BCM_E_NONE;
}

/*
 * Update the LPM entry for L3_DEFIP_DATA_ONLY view during Entry Modify.
 */
STATIC int
_bcm_l3_scaled_lpm_data_modify_urpf_entry_replicate(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    int rv;               /* Return type.                                  */
    int offset = 0;
    int defip_index = lpm_cfg->defip_index;

    if(!SOC_URPF_STATUS_GET(unit)) {
        return (SOC_E_NONE);
    }

    BCM_IF_ERROR_RETURN(soc_lpm_fb_urpf_entry_replicate_index_offset(unit, &offset));

    /* For non-ipv6 multiple by 2, since will be >> by 1 in
     * _bcm_l3_scaled_lpm_data_modify function */
    if (!(lpm_cfg->defip_flags & BCM_L3_IP6)) {
        offset = offset << 1;
    }

    /* Update with urpf entry defip index */
    lpm_cfg->defip_index += offset;

    /* Update urpf entry with new data */
    rv =  _bcm_l3_scaled_lpm_data_modify(unit, lpm_cfg, nh_ecmp_idx);

    /* Restore original defip index */
    lpm_cfg->defip_index = defip_index;

    return rv;
}

int
_bcm_l3_scaled_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    int rv = BCM_E_NONE;
    int defip_table_size = 0;
    int paired_table_size = 0;

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check */
    if (lpm_cfg == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Check if Entry is for modification */
    /* Modify only the Data part for the given LPM entry index */
    if ((lpm_cfg->defip_index >= 0) && !SOC_IS_TOMAHAWK3(unit)) {
       BCM_IF_ERROR_RETURN(_bcm_l3_scaled_lpm_data_modify(unit, lpm_cfg, nh_ecmp_idx));
       /* Update Urpf entry when URPF is ON */
       return _bcm_l3_scaled_lpm_data_modify_urpf_entry_replicate(unit, lpm_cfg, nh_ecmp_idx);
    }

    BCM_IF_ERROR_RETURN(_bcm_l3_lpm_table_sizes_get(unit, &paired_table_size, 
                                                    &defip_table_size));

    if (paired_table_size == 0 && lpm_cfg->defip_sub_len > 64) {
        return BCM_E_FULL;
    }

    if ((paired_table_size != 0 && lpm_cfg->defip_sub_len > 64) || 
        (defip_table_size == 0)) {
        return _bcm_fb_lpm128_add(unit, lpm_cfg, nh_ecmp_idx);
    }

    rv = _bcm_fb_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
    if ((rv == BCM_E_FULL) && _bcm_l3_is_v4_64b_allowed_in_paired_tcam(unit)) {
        return _bcm_l3_lpm_ripple_entries(unit, lpm_cfg,
                                          nh_ecmp_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_l3_lpm_add
 * Purpose:
 *      Add an entry to TD2 route table table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 frtu[]*      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    int rv;
    soc_mem_t mem = L3_DEFIPm;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_flex_flow)
            && (lpm_cfg->defip_flow_handle != 0)) {
        soc_mem_t view_id;
        soc_flow_db_mem_view_info_t vinfo;

        BCM_IF_ERROR_RETURN( soc_flow_db_ffo_to_mem_view_id_get(unit,
                    lpm_cfg->defip_flow_handle,
                    lpm_cfg->defip_flow_option_handle,
                    SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                    (uint32 *)&view_id));
        BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, view_id, &vinfo));
        mem = vinfo.mem;
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(_bcm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                    lpm_cfg->defip_sub_len, &mem));
    }

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        rv = _bcm_l3_scaled_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
#ifdef FB_LPM_DEBUG
        BCM_IF_ERROR_RETURN(lpm128_state_verify(unit));
#endif
        return rv;
    }
    
    switch (mem) {
        case L3_DEFIP_PAIR_128m:
        case L3_DEFIP_PAIR_LEVEL1m:
            if (soc_mem_index_count(unit, mem) > 0) {
                return _bcm_l3_defip_pair128_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break; 
        default:
            if (soc_mem_index_count(unit, mem) > 0) {
                return _bcm_fb_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break; 
    }
    return (BCM_E_FULL);
}

int
_bcm_l3_scaled_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int defip_table_size = 0;
    int paired_table_size = 0;

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return BCM_E_UNAVAIL;
    }

   BCM_IF_ERROR_RETURN(_bcm_l3_lpm_table_sizes_get(unit, &paired_table_size,
                                                   &defip_table_size));

    if ((paired_table_size != 0) && (lpm_cfg->defip_sub_len > 64 ||
        (defip_table_size == 0) ||
        lpm_cfg->defip_flags_high & BCM_XGS3_L3_ENTRY_IN_DEFIP_PAIR)) {
        return _bcm_fb_lpm128_del(unit, lpm_cfg);
    }

    if (paired_table_size == 0 && lpm_cfg->defip_sub_len > 64) {
        return BCM_E_NOT_FOUND;
    }

    lpm_cfg->defip_flags_high = 0;

    rv = _bcm_fb_lpm_del(unit, lpm_cfg);
    if (BCM_SUCCESS(rv) && _bcm_l3_is_v4_64b_allowed_in_paired_tcam(unit)) {
            return _bcm_l3_lpm128_ripple_entries(unit);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_l3_lpm_del
 * Purpose:
 *      Delete an entry from the route table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit); 
    int rv = BCM_E_NONE;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }
    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
         mem = L3_DEFIP_LEVEL1m;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_flex_flow)
            && (lpm_cfg->defip_flow_handle != 0)) {
        soc_mem_t view_id;
        soc_flow_db_mem_view_info_t vinfo;

        BCM_IF_ERROR_RETURN( soc_flow_db_ffo_to_mem_view_id_get(unit,
                    lpm_cfg->defip_flow_handle,
                    lpm_cfg->defip_flow_option_handle,
                    SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                    (uint32 *)&view_id));
        BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, view_id, &vinfo));
        mem = vinfo.mem;
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(_bcm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                    lpm_cfg->defip_sub_len, &mem));
    }
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        rv = _bcm_l3_scaled_lpm_del(unit, lpm_cfg);
#ifdef FB_LPM_DEBUG
        BCM_IF_ERROR_RETURN(lpm128_state_verify(unit));
#endif
        return rv;
    }

    switch (mem) {
        case L3_DEFIP_PAIR_128m:
#ifdef BCM_TOMAHAWK3_SUPPORT
        case L3_DEFIP_PAIR_LEVEL1m:
#endif
            if (max_v6_entries > 0) {    
                return _bcm_l3_defip_pair128_del(unit, lpm_cfg);
            }   
            break;  
        default: 
            if (soc_mem_index_count(unit, mem) > 0) {
                return _bcm_fb_lpm_del(unit, lpm_cfg);
            } 
            break;  
    }

    return (rv);
}

int _bcm_l3_scaled_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int rv1 = BCM_E_NONE;
    int rv2 = BCM_E_NONE;
    int defip_table_size = 0;
    int paired_table_size = 0;

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_l3_lpm_table_sizes_get(unit, &paired_table_size,
                                                    &defip_table_size));
    
    if (paired_table_size != 0) {
        rv1 = _bcm_fb_lpm128_update_match(unit, trv_data);
    }
   
    if (defip_table_size != 0) {
        rv2 = _bcm_fb_lpm_update_match(unit, trv_data);
    }
   
    BCM_IF_ERROR_RETURN(rv1);
    BCM_IF_ERROR_RETURN(rv2);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_l3_lpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int rv1 = BCM_E_NONE; 
    int rv2 = BCM_E_NONE; 
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);
    soc_mem_t mem = L3_DEFIPm;

    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return _bcm_l3_scaled_lpm_update_match(unit, trv_data);
    }

    if ((trv_data->flags & BCM_L3_IP6) && (max_v6_entries > 0)) {
        rv2 = _bcm_l3_defip_pair128_update_match(unit, trv_data);
    }

    if (soc_mem_index_count(unit, mem) > 0) {
        rv1 = _bcm_fb_lpm_update_match(unit, trv_data);
    }
    BCM_IF_ERROR_RETURN(rv1);
    BCM_IF_ERROR_RETURN(rv2);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_l3_defip_urpf_enable
 * Purpose:
 *      Configure TD2 internal route table for URPF mode
 * Parameters:
 *      unit     - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_defip_urpf_enable(int unit, int enable)
{
    int ipv6_64_depth = 0;
    int ipv6_128_dip_offset = 0, ipv6_128_depth = 0;
    int num_ipv6_128b_entries;
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int start_index = 0;
#ifdef BCM_UTT_SUPPORT
    int urpf_tcam_pair_count = 0;
    int max_paired_count = (SOC_L3_DEFIP_MAX_TCAMS_GET(unit)) / 2;
#endif
#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm) &&
        soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        return SOC_E_NONE;
    }
#endif
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    SOC_IF_ERROR_RETURN(soc_fb_lpm_tcam_pair_count_get(unit, &tcam_pair_count));

    if (enable) {
#ifdef BCM_UTT_SUPPORT
        if (soc_feature(unit, soc_feature_utt)) {
            urpf_tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
            ipv6_64_depth = (max_paired_count - (urpf_tcam_pair_count * 2)) * tcam_depth;
        } else
#endif
        {
            /* All CAMs in URPF mode */
            switch (tcam_pair_count) {
                case 0:
                    /* No pairing of tcams */
                    ipv6_64_depth = 4 * tcam_depth;
                    break;
                case 1:
                case 2:
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 2 * tcam_depth;
                    }
                    break;
                default:
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 0;
                    }
                    break;
            }
        }

        if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            ipv6_128_dip_offset = num_ipv6_128b_entries / 2;
            ipv6_128_depth      = num_ipv6_128b_entries / 2;
            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
        }
    } else { /* Disable case */
        ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
        ipv6_128_dip_offset = 0;
        ipv6_128_depth      = num_ipv6_128b_entries;
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
        }
    }

    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit)))  = ipv6_64_depth;
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (!enable) {
            start_index = (tcam_pair_count * tcam_depth * 2);
        } else {
#ifdef BCM_UTT_SUPPORT
            if (soc_feature(unit, soc_feature_utt)) {
                urpf_tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
                start_index = urpf_tcam_pair_count * 2 * tcam_depth;
            } else
#endif
            {
                switch (tcam_pair_count) {
                    case 1:
                    case 2:
                        start_index = 2 * tcam_depth;
                        break;
                    case 3:
                    case 4:
                        start_index = 4 * tcam_depth;
                        break;
                    default:
                        start_index = 0;
                        break;
                }
            }
        }
        /* reinit the lpm128 logic to get the correct size for table */ 
        SOC_IF_ERROR_RETURN(soc_fb_lpm_stat_init(unit));
        SOC_IF_ERROR_RETURN(soc_fb_lpm128_deinit(unit));
        SOC_IF_ERROR_RETURN(soc_fb_lpm128_init(unit));
    } else {
        BCM_DEFIP_PAIR128_URPF_OFFSET(unit)    = ipv6_128_dip_offset;
        BCM_DEFIP_PAIR128_IDX_MAX(unit)        = ipv6_128_depth - 1;
        BCM_DEFIP_PAIR128_TOTAL(unit)          = ipv6_128_depth;
        SOC_IF_ERROR_RETURN(soc_fb_lpm_stat_init(unit));
    }
    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit)))   = start_index - 1;

    BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                                                start_index));

    BCM_IF_ERROR_RETURN(
        _bcm_l3_defip_key_sel_set(unit, enable, tcam_pair_count));
    return BCM_E_NONE;
}

#if defined(BCM_KATANA2_SUPPORT) 

#define URPF_OFFSET 0
/*
 * Function:
 *      _bcm_kt2_l3_defip_mem_get
 * Purpose:
 *      Resolve route table memory for a given route entry.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      flags      - (IN)IPv6/IPv4 route.
 *      plen       - (IN)Prefix length.
 *      mem        - (OUT)Route table memory.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
    *mem = ((flags & BCM_L3_IP6) && (plen > 64)) ?
               L3_DEFIP_PAIR_128m : L3_DEFIPm ;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_defip_pair128_init
 * Purpose:
 *      Initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_defip_pair128_init(int unit)
{
    int rv;
    int mem_sz;
    int defip_config;
    int ipv6_128_depth;
    int ipv6_default_depth = 1024;

#ifdef  BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        ipv6_default_depth = 256;
    }
#endif

    /* De-allocate any existing structures. */
    if (BCM_DEFIP_PAIR128(unit) != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
    }

    /* Allocate CAM control structure. */
    mem_sz = sizeof(_bcm_defip_pair128_table_t);
    BCM_DEFIP_PAIR128(unit) = sal_alloc(mem_sz, "kt2_l3_defip_pair128");
    if (BCM_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Reset CAM control structure. */
    sal_memset(BCM_DEFIP_PAIR128(unit), 0, mem_sz);

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE,1);
    ipv6_128_depth = soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                                      (defip_config ? ipv6_default_depth : 0));

    BCM_DEFIP_PAIR128_TOTAL(unit) = ipv6_128_depth;
    BCM_DEFIP_PAIR128_URPF_OFFSET(unit) = URPF_OFFSET;
    BCM_DEFIP_PAIR128_IDX_MAX(unit) = ipv6_128_depth - 1;
    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = ipv6_128_depth;
    if (ipv6_128_depth) {
        mem_sz = ipv6_128_depth * sizeof(_bcm_defip_pair128_entry_t);
        BCM_DEFIP_PAIR128_ARR(unit) = 
            sal_alloc(mem_sz, "kt2_l3_defip_pair128_entry_array");
        if (BCM_DEFIP_PAIR128_ARR(unit) == NULL) {
            BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
            return (BCM_E_MEMORY);
        }
        sal_memset(BCM_DEFIP_PAIR128_ARR(unit), 0, mem_sz);
    }
    rv = _bcm_defip_pair128_field_cache_init(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
        return rv;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt2_l3_defip_init
 * Purpose:
 *      Configure KT2 internal route table as per soc properties.
 *      (TCAM pairing for IPv4, 64/128 bit IPV6 prefixes)
 * Parameters:
 *      unit     - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_l3_defip_init(int unit)
{
    int defip_config;
    uint32 defip_key_sel_val = 0;
    int mem_v4, mem_v6, mem_v6_128; /* Route table memories */
    int ipv6_64_depth = 0;
    int num_ipv6_128b_entries;
    int start_index = 0;
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);
    num_ipv6_128b_entries =
        soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
            (defip_config ?(SOC_IS_METROLITE(unit) ? 256:1024) : 0));

    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    }

    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = 
                                       SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    switch (tcam_pair_count) {
        case 8:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM14_15f, 0x1);
        case 7:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM12_13f, 0x1);
        case 6:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM10_11f, 0x1);
        case 5:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM8_9f, 0x1);
        case 4:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM6_7f, 0x1);
        case 3:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM4_5f, 0x1);
        case 2:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM2_3f, 0x1);
        case 1:
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                              V6_KEY_SEL_CAM0_1f, 0x1);
            break;
        default:
            break;
    }
    ipv6_64_depth = soc_mem_index_count(unit, L3_DEFIPm); 

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(unit, defip_key_sel_val));

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v4 != mem_v6) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
    }
    /* Overriding defaults set during lpm init */
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm128_init(unit));
        ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
        start_index = (tcam_pair_count * tcam_depth * 2);
    } else {
        start_index = 0;
    }

    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit))) = start_index - 1;
    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit))) = ipv6_64_depth;
    BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                                                start_index));

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            return soc_fb_lpm128_init(unit);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_kt2_defip_pair128_init(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_l3_defip_deinit
 * Purpose:
 *      De-initialize route table for katana2 devices.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt2_l3_defip_deinit(int unit)
{
    soc_mem_t mem_v4;      /* IPv4 Route table memory.             */
    soc_mem_t mem_v6;      /* IPv6 Route table memory.             */
    soc_mem_t mem_v6_128;  /* IPv6 full prefix route table memory. */

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Deinitialize IPv4 entries lookup engine. */
    BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Deinitialize IPv6 entries lookup engine. */
    if (mem_v4 != mem_v6) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
    }

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Deinitialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
         if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
             BCM_IF_ERROR_RETURN(soc_fb_lpm128_deinit(unit));
         } else {
             BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
         }
    }
    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_kt2_l3_lpm_get
 * Purpose:
 *      Get an entry from the route table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_l3_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
        }
    }

    switch (mem) {
        case L3_DEFIP_PAIR_128m:
            if (max_v6_entries > 0) {
                return _bcm_l3_defip_pair128_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
        default:
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_kt2_l3_lpm_add
 * Purpose:
 *      Add an entry to KT2 route table table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_l3_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIPm;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
        }
    }

    switch (mem) {
        case L3_DEFIP_PAIR_128m:
            if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) > 0) {
                return _bcm_l3_defip_pair128_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
        default:
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
    }

    return (BCM_E_FULL);
}

/*
 * Function:
 *      _bcm_kt2_l3_lpm_del
 * Purpose:
 *      Delete an entry from the route table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_l3_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_del(unit, lpm_cfg);
        }
    }

    switch (mem) {
        case L3_DEFIP_PAIR_128m:
            if (max_v6_entries > 0) {
                return _bcm_l3_defip_pair128_del(unit, lpm_cfg);
            }
            break;
        default: 
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_del(unit, lpm_cfg);
            }
            break;
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_kt2_l3_lpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_l3_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int rv1 = BCM_E_NONE;
    int rv2 = BCM_E_NONE;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    if (NULL == trv_data) {
        return (BCM_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        return _bcm_l3_scaled_lpm_update_match(unit, trv_data);
    }

    if ((trv_data->flags & BCM_L3_IP6) && (max_v6_entries > 0)) {
        rv2 = _bcm_l3_defip_pair128_update_match(unit, trv_data);
    }

    if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
        rv1 = _bcm_fb_lpm_update_match(unit, trv_data);
    }
    BCM_IF_ERROR_RETURN(rv1);
    BCM_IF_ERROR_RETURN(rv2);

    return (BCM_E_NONE);
}
#endif /* BCM_KATANA2_SUPPORT  */
#endif /* INCLUDE_L3  */
