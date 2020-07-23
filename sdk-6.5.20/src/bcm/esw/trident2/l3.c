/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Triumph3 L3 function implementations
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(INCLUDE_L3) && defined(BCM_TRIDENT2_SUPPORT) 

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>
#if defined(BCM_TOMAHAWK_SUPPORT) 
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT) 
#include <soc/monterey.h>
#include <bcm_int/esw/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/trident3.h>
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_APACHE_SUPPORT) 
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT) 
#include <soc/helix5.h>
#include <bcm_int/esw/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#endif /* BCM_TRIDENT3_SUPPORT*/
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>

#ifdef ALPM_ENABLE
#include <bcm_int/esw/alpm.h>
#endif

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

#define TH_IPMC_NO_SRC_CHECK_PORT      0xFF

/* NH nat id update key type*/
#define _BCM_NH_UPDATE_KEY_L3_INTF      0
#define _BCM_NH_UPDATE_KEY_L3_IIF       1
#define _BCM_NH_UPDATE_KEY_VLAN         2

/* IP Options Handling */
#define L3_INFO(_unit_) (&_bcm_l3_bk_info[_unit_])
#define _BCM_IP_OPTION_PROFILE_CHUNK 256
#define _BCM_IP4_OPTIONS_LEN    \
            (soc_mem_index_count(unit, IP_OPTION_CONTROL_PROFILE_TABLEm)/ \
             _BCM_IP_OPTION_PROFILE_CHUNK)

/* Add mutex semaphore to protect "L3_INFO(unit)->ip4_profiles_hw_idx" */
STATIC sal_mutex_t ip4_profiles_hw_idx_lock[BCM_MAX_NUM_UNITS];

#define HW_IDX_LOCK(unit) \
    do { \
        if (ip4_profiles_hw_idx_lock[unit] != NULL) { \
            sal_mutex_take(ip4_profiles_hw_idx_lock[unit], sal_mutex_FOREVER); \
        } \
    } while (0)

#define HW_IDX_UNLOCK(unit) \
    do { \
        if (ip4_profiles_hw_idx_lock[unit] != NULL) { \
            sal_mutex_give(ip4_profiles_hw_idx_lock[unit]); \
        } \
    } while (0)

#define GET_HW_IDX(unit, idx, id) \
    do { \
        HW_IDX_LOCK(unit); \
        idx = L3_INFO(unit)->ip4_profiles_hw_idx[id]; \
        HW_IDX_UNLOCK(unit); \
    } while (0)                        

#define SET_HW_IDX(unit, id, idx) \
    do { \
        HW_IDX_LOCK(unit); \
        L3_INFO(unit)->ip4_profiles_hw_idx[id] = idx; \
        HW_IDX_UNLOCK(unit); \
    } while (0)

/*
 * IP_OPTIONS_PROFILE usage bitmap operations
 */
#define _BCM_IP4_OPTIONS_USED_GET(_u_, _identifier_) \
        SHR_BITGET(L3_INFO(_u_)->ip4_options_bitmap, (_identifier_))
#define _BCM_IP4_OPTIONS_USED_SET(_u_, _identifier_) \
        SHR_BITSET(L3_INFO((_u_))->ip4_options_bitmap, (_identifier_))
#define _BCM_IP4_OPTIONS_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(L3_INFO((_u_))->ip4_options_bitmap, (_identifier_))

#define _BCM_IP_OPTION_REINIT_INVALID_HW_IDX 0xffff

/* Bookkeeping info for ECMP resilient hashing */
typedef struct _td2_ecmp_rh_info_s {
    int num_ecmp_rh_flowset_blocks;
    SHR_BITDCL *ecmp_rh_flowset_block_bitmap; /* Each block corresponds to
                                                 64 entries, 8 for monterey */
    uint32 ecmp_rh_rand_seed; /* The seed for pseudo-random number generator */
    uint8  ecmp_block_size;    /* Block size in bits
                                * 6 for TD2/+/Apache
                                * 3 for Monterey */
    uint8  ecmp_rand_seed_shift;
} _td2_ecmp_rh_info_t;

STATIC _td2_ecmp_rh_info_t *_td2_ecmp_rh_info[BCM_MAX_NUM_UNITS];

/* per ecmp group enhanced hashing enabled info  */
#ifdef BCM_TRIDENT3_SUPPORT
extern uint8  ecmp_grp_enhanced_hashing[BCM_MAX_NUM_UNITS][4096];
#endif

#define _BCM_ECMP_RH_FLOWSET_BLOCK_USED_GET(_u_, _idx_) \
    SHR_BITGET(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_RH_FLOWSET_BLOCK_USED_SET(_u_, _idx_) \
    SHR_BITSET(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_RH_FLOWSET_BLOCK_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_RH_FLOWSET_BLOCK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, \
            _idx_, _count_)
#define _BCM_ECMP_RH_FLOWSET_BLOCK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, \
            _idx_, _count_)
#define _BCM_ECMP_RH_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(_td2_ecmp_rh_info[_u_]->ecmp_rh_flowset_block_bitmap, \
            _idx_, _count_, _result_)

typedef struct _td2_ecmp_rh_member_s {
    int nh_index; /* Next hop index of the member */
    int member_id; /* Member ID */
    int num_replica; /* Number of members with the same next hop index as
                        this member. Valid only for the first replica. */
    int replica_id; /* Index among members with the same next hop index as
                       this member. Valid for every replica. */
    int next_replica_id; /* Index of the next replica to be assigned to
                            a flow set entry containing the next hop index
                            shared by replicas. Valid only for the first
                            replica. */
} _td2_ecmp_rh_member_t;

/*----- STATIC FUNCS ----- */
int
_bcm_td2_l3_ipmc_ent_init(int unit, uint32 *buf_p, _bcm_l3_cfg_t *l3cfg);


/*
 * Function:
 *      _bcm_td2_l3_ent_init
 * Purpose:
 *      TD2 helper routine used to init l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
int
_bcm_td2_l3_ent_init(int unit, soc_mem_t mem, 
                      _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 base_valid0, base_valid1;
#endif


    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem)); 

    if (ipv6) { /* ipv6 entry */
        if (BCM_XGS3_L3_MEM(unit, v6) == mem) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
                soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_LWR_64f,
                                 l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
                soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_UPR_64f,
                                 l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
                soc_mem_field32_set(unit, mem, buf_p, IPV6UC__VRF_IDf,
                                                      l3cfg->l3c_vrf);

            } else
#endif
            {
            soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_UPR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                                  l3cfg->l3c_vrf);
            }
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit))
            {
                if (SOC_IS_TRIDENT3X(unit))
                {
                soc_mem_field32_set(unit, mem, buf_p, DATA_TYPEf, TD3_L3_HASH_DATA_TYPE_V6UC);
                    base_valid0 = 3;
                    base_valid1 = 4;
                }
                else
                {
                    base_valid0 = 1;
                    base_valid1 = 2;
                }
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, TD3_L3_HASH_KEY_TYPE_V6UC);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, base_valid0);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, base_valid1);
            }
            else
#endif
            {
            soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 2);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 2);
            }
        } else if (BCM_XGS3_L3_MEM(unit, v6_4) == mem) { 
            soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC_EXT__IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC_EXT__IP_ADDR_UPR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem, buf_p, IPV6UC_EXT__VRF_IDf,
                                                  l3cfg->l3c_vrf);
            /* ipv6 extended host entry */
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit))
            {
                if (SOC_IS_TRIDENT3X(unit))
                {
                soc_mem_field32_set(unit, mem, buf_p, DATA_TYPEf, TD3_L3_HASH_DATA_TYPE_V6UC_EXT);
                }
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, TD3_L3_HASH_KEY_TYPE_V6UC_EXT);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, 4);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_2f, 7);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_3f, 7);
            } else
#endif
            {
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 3);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 3);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_2f, 3);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_3f, 3);

            soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_2f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_3f, 1); 
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else { /* ipv4 entry */                /* if uC is still not ready, return timeout */
        if (BCM_XGS3_L3_MEM(unit, v4) == mem) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit))
            {
                soc_mem_field32_set(unit, mem, buf_p, IPV4UC__IP_ADDRf, 
												l3cfg->l3c_ip_addr);
                soc_mem_field32_set(unit, mem, buf_p, IPV4UC__VRF_IDf, 
												l3cfg->l3c_vrf);
            } else
#endif
            {
            soc_mem_field32_set(unit, mem, buf_p, IP_ADDRf,
                                               l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                                  l3cfg->l3c_vrf);
            }
            /* ipv4 unicast */
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, 0); 
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALIDf, 1);
            } else 
#endif
            {
            soc_mem_field32_set(unit, mem, buf_p, VALIDf, 1); 
            }
        } else if (BCM_XGS3_L3_MEM(unit, v4_2) == mem) { 
            soc_mem_field32_set(unit, mem, buf_p, IPV4UC_EXT__IP_ADDRf,
                                               l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem, buf_p, IPV4UC_EXT__VRF_IDf,
                                                  l3cfg->l3c_vrf);
            /* ipv4 extended host entry */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                /* TD3 is different from previous chips. 0 in KEY_TYPE was used to distinguish regular from extended
                 * entries in previous chips. In TD3 DATA_TYPE is 1 for extended entries 
                 */
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, TD3_L3_HASH_KEY_TYPE_V4UC_EXT);
                if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, buf_p, DATA_TYPEf, TD3_L3_HASH_DATA_TYPE_V4UC_EXT);
                }
                base_valid0 = 1;
                if (SOC_IS_TOMAHAWK3(unit)) {
                    base_valid1 = 2;
                } else {
                    base_valid1 = 7;
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, 7); 
                }
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, base_valid1);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, base_valid0);
            } else
#endif
            {
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 1);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 1);
            soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
}

/*----- STATIC FUNCS ----- */
/*
 * Function:
 *      _bcm_td2_l3_ent_parse
 * Purpose:
 *      TD2 helper routine used to parse hw l3 entry to api format.
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry key & parse result.
 *      nh_idx    - (IN/OUT) Next hop index. 
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_td2_l3_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                        int *nh_idx, void *l3x_entry, int key_type)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 hit = 0;               /* composite hit = hit_x|hit_y */
    uint32 *buf_p;                /* HW buffer address.          */
    uint32 glp;                   /* Global port. */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    int embedded_nh = FALSE;
    l3_entry_hit_only_x_entry_t hit_x;
    l3_entry_hit_only_y_entry_t hit_y;
    l3_entry_hit_only_entry_t hit_default;
    int idx, idx_max, idx_offset, hit_idx_shift;
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f};

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    buf_p = (uint32 *)l3x_entry;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        l3cfg->l3c_flow_handle != 0) {

        return _bcm_td3_l3_flex_ent_parse(unit, mem, l3cfg, nh_idx, l3x_entry);
    }
#endif

    /*
     * Set table fields 
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit))
    {
        BCM_TH3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        /*
         * Check if embedded NH table is being used 
         */
        embedded_nh = 0;

    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_TD3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);  
        /* Check if embedded NH table is being used */
        embedded_nh = (mem == L3_ENTRY_DOUBLEm && !ipv6) || 
                      (mem == L3_ENTRY_QUADm && ipv6);

    } else 
#endif
    {
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);  
    /* Check if embedded NH table is being used */
    embedded_nh = (mem == L3_ENTRY_IPV4_MULTICASTm && !ipv6) || 
                  (mem == L3_ENTRY_IPV6_MULTICASTm && ipv6);

    }

    /* Reset entry flags */
    l3cfg->l3c_flags = (ipv6) ? BCM_L3_IP6 : 0;

    idx_max = 1;
    idx_offset = (l3cfg->l3c_hw_index & 0x3);
    hit_idx_shift = 2;
    if (mem == L3_ENTRY_IPV4_MULTICASTm || 
        mem == L3_ENTRY_IPV6_UNICASTm ||
        (mem == L3_ENTRY_DOUBLEm)) {
        idx_max = 2;
        hit_idx_shift = 1;
        idx_offset = (l3cfg->l3c_hw_index & 0x1) << 1;
    } else if ((mem == L3_ENTRY_IPV6_MULTICASTm) ||
               (mem == L3_ENTRY_QUADm)) {
        idx_max = 4;
        hit_idx_shift = 0;
        idx_offset = 0;
    }

    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        /* SOC_IS_TOMAHAWKX(unit) */
        /* Get info from L3 and next hop table */
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLYm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_x));

        hit = 0;
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLYm,
                                       &hit_x, hitf[idx]);
        }
    } else if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
        /* Get info from L3 and next hop table */
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLY_Xm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_x));

        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLY_Ym,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_y));

        hit = 0;
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLY_Xm,
                                       &hit_x, hitf[idx]);
        }

        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLY_Ym,
                                       &hit_y, hitf[idx]);
        }
    } else {
        /* Get info from L3 and next hop table */
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLYm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_default));

        hit = 0;
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLYm,
                                       &hit_default, hitf[idx]);
        }
    }

    soc_mem_field32_set(unit, mem, buf_p, fld->hit, hit);

    if (hit) {
        l3cfg->l3c_flags |= BCM_L3_HIT;
    }
    /* Get priority override flag. */
    if (soc_mem_field32_get(unit, mem, buf_p, fld->rpe)) {
        l3cfg->l3c_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (soc_mem_field32_get(unit, mem, buf_p, fld->dst_discard)) {
        l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
    }

    /* Get local addr bit. */
    if (soc_mem_field32_get(unit, mem, buf_p, fld->local_addr)) {
        l3cfg->l3c_flags |= BCM_L3_HOST_LOCAL;
    }

#ifdef BCM_MONTEREY_SUPPORT
    /* Get Roe Header Decap Control */
    if (soc_feature(unit, soc_feature_roe) ||
        soc_feature(unit, soc_feature_roe_custom)) {
        if (soc_mem_field32_get(unit, mem, buf_p, fld->roe_decap)) {
            l3cfg->l3c_flags2 |= BCM_L3_FLAGS2_DECAP_ROE_HEADER;
        }
    }
#endif

    /* Get classification group id. */
    l3cfg->l3c_lookup_class = 
        soc_mem_field32_get(unit, mem, buf_p, fld->class_id);

    /* Get priority. */
    l3cfg->l3c_prio = soc_mem_field32_get(unit, mem, buf_p, fld->priority);

    /* Get virtual router id. */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, mem, buf_p, fld->vrf);

    /* Get bfd_enable bit. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->bfd_enable)) {
        l3cfg->l3c_bfd_enable
            = soc_mem_field32_get(unit, mem, buf_p, fld->bfd_enable);
    }

    if (embedded_nh) {
        uint32 port_addr_max = 0, modid_max  = 0;
#ifdef BCM_RIOT_SUPPORT
        int dest_type = 0, dvp = -1;
#endif

        l3cfg->l3c_flags |= BCM_L3_DEREFERENCED_NEXTHOP;
        if (nh_idx) {
            *nh_idx = BCM_XGS3_L3_INVALID_INDEX; /* Embedded NH */
        }
        l3cfg->l3c_intf = soc_mem_field32_get(unit, mem, buf_p,
                                           fld->l3_intf);
#ifdef BCM_TOMAHAWK_SUPPORT
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th_l3_vp_ent_parse(unit, mem, l3cfg, buf_p));
        } else
#endif
        {
#if defined(BCM_RIOT_SUPPORT)
            if ((soc_feature(unit, soc_feature_riot)) &&
                (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
                /* Derive MAC address using profile table */
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_l3_macda_oui_profile_entry_parse(unit, mem, l3cfg, buf_p));
            } else
#endif
            {
                soc_mem_mac_addr_get(unit, mem, buf_p,
                                    fld->mac_addr, l3cfg->l3c_mac_addr);
            }
            
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                uint32 dtype;
                glp = soc_mem_field32_dest_get(unit, mem, buf_p, fld->dest, &dtype);
                dest_type = dtype;
            } else
#endif
            {
            glp = soc_mem_field32_get(unit, mem, buf_p, fld->glp);
            }

            port_addr_max = SOC_PORT_ADDR_MAX(unit) + 1;
            modid_max = SOC_MODID_MAX(unit) + 1;

#ifdef BCM_RIOT_SUPPORT
            if (BCMI_RIOT_IS_ENABLED(unit)) {
                if (SOC_IS_TRIDENT3X(unit)) {
                    if (dest_type == SOC_MEM_FIF_DEST_DVP) {
                        dvp = glp; /*glp already holds the DEST value from earlier call */
                        _bcm_vp_encode_gport(unit, dvp, &(l3cfg->l3c_port_tgid));
                        l3cfg->l3c_modid = 0;
                    }
                } else {

                dest_type = soc_mem_field32_get(unit, mem, buf_p, fld->dest_type);
                if (dest_type == 1) {
                    dvp = soc_mem_field32_get(unit, mem, buf_p, fld->destination);
                    _bcm_vp_encode_gport(unit, dvp, &(l3cfg->l3c_port_tgid));
                    l3cfg->l3c_modid = 0;
                }
            }
            }
            if (dvp == -1)
#endif
            {
                if ((glp & (port_addr_max * modid_max))
#if defined(BCM_TRIDENT3_SUPPORT)
                    || (SOC_IS_TRIDENT3X(unit) && (dest_type == SOC_MEM_FIF_DEST_LAG))
#endif
                    ) {
                    l3cfg->l3c_flags |= BCM_L3_TGID;
                    l3cfg->l3c_port_tgid = glp & BCM_TGID_PORT_TRUNK_MASK(unit);
                    l3cfg->l3c_modid = 0;
                } else {
                    l3cfg->l3c_port_tgid = glp & SOC_PORT_ADDR_MAX(unit);
                    l3cfg->l3c_modid = (glp / port_addr_max) & SOC_MODID_MAX(unit);
                }
            }

        }
        l3cfg->l3c_eh_q_tag_type = soc_mem_field32_get(unit, mem, buf_p, 
                                                       fld->eh_tag_type);
        l3cfg->l3c_eh_q_tag = soc_mem_field32_get(unit, mem, buf_p,
                                                  fld->eh_queue_tag);
        /* Get copy_to_cpu bit. */
        if (SOC_MEM_FIELD_VALID(unit, mem, fld->copy_to_cpu)) {
            if (soc_mem_field32_get(unit, mem, buf_p, fld->copy_to_cpu)) {
                l3cfg->l3c_flags |= BCM_L3_COPY_TO_CPU;
            }
        }
    } else {
        /* Get next hop info. */
        if (nh_idx) {
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                    uint32 dest_type;
                    *nh_idx = soc_mem_field32_dest_get(unit, mem, buf_p, fld->dest, &dest_type);
                    if (dest_type != SOC_MEM_FIF_DEST_NEXTHOP) {
                        /* Mark entry as ecmp */
                        l3cfg->l3c_ecmp = TRUE;
                        l3cfg->l3c_flags |= BCM_L3_MULTIPATH;
                    }
            } else
#endif
            {
            *nh_idx = soc_mem_field32_get(unit, mem, buf_p, fld->nh_idx);
                if (soc_mem_field32_get(unit, mem, buf_p, ECMPf)) {
                /* Mark entry as ecmp */
                l3cfg->l3c_ecmp = TRUE;
                l3cfg->l3c_flags |= BCM_L3_MULTIPATH;
        }
            }
        }
#ifdef BCM_TRIDENT3_SUPPORT
        if (!SOC_IS_TRIDENT3X(unit)) {
#endif
        if (soc_mem_field32_get(unit, mem, buf_p, ECMPf)) {
            /* Mark entry as ecmp */
            l3cfg->l3c_ecmp  = TRUE;
            l3cfg->l3c_flags |= BCM_L3_MULTIPATH;
        } else {
            l3cfg->l3c_ecmp  = FALSE;
            l3cfg->l3c_flags &= ~BCM_L3_MULTIPATH;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        }
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_clear_hit
 * Purpose:
 *      Clear hit bit on l3 entry
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      mem       - (IN)L3 table memory.
 *      l3cfg     - (IN)l3 entry info. 
 *      l3x_entry - (IN)l3 entry filled hw buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_clear_hit(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                   void *l3x_entry, int l3_entry_idx)
{
    int ipv6;                     /* Entry is IPv6 flag.      */
    int mcast;                    /* Entry is multicast flag. */ 
    uint32 *buf_p;                /* HW buffer address.       */ 
    _bcm_l3_fields_t *fld;        /* L3 table common fields.  */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f };
    int idx;

    /* Input parameters check */
    if ((NULL == l3cfg) || (NULL == l3x_entry)) {
        return (BCM_E_PARAM);
    }

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    mcast = (l3cfg->l3c_flags & BCM_L3_IPMC);

    /* Set table fields */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_TH3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    } else
#endif
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_TD3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    } else {
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    }

    /* Init memory pointers. */ 
    buf_p = (uint32 *)l3x_entry;

    /* If entry was not hit  there is nothing to clear */
    if (!(l3cfg->l3c_flags & BCM_L3_HIT)) {
        return (BCM_E_NONE);
    }

    /* Reset hit bit. */
    soc_mem_field32_set(unit, mem, buf_p, fld->hit, 0);

    if (ipv6 && mcast) {
        /* IPV6 multicast entry hit reset. */
        for (idx = 0; idx < 4; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    } else if (ipv6 || mcast) {
        /* Reset IPV6 unicast and IPV4 multicast hit bits. */
        for (idx = 0; idx < 2; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    }

    /* Write entry back to hw. */
    return BCM_XGS3_MEM_WRITE(unit, mem, l3_entry_idx, buf_p);
}

/*
 * Function:
 *      _bcm_td2_l3_entry_add
 * Purpose:
 *      Add and entry to TD2 L3 host table.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_entry_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{

    int ipv6;
    uint32 *bufp;             /* Hardware buffer ptr     */
    soc_mem_t mem;            /* L3 table memory.        */  
    int rv = BCM_E_NONE;      /* Operation status.       */
    _bcm_l3_fields_t *fld;    /* L3 table common fields. */
    int embedded_nh = FALSE;
    uint32 glp = 0, port_id = 0, modid = 0;
    _bcm_l3_intf_cfg_t l3_intf;
    l3_entry_ipv4_unicast_entry_t l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;   /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;   /* IPv6-Embedded */
    uint32 port_addr_max = 0, modid_max  = 0;
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_RIOT_SUPPORT)
    int macda_oui_profile_index = -1;
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    int macda_oui_profile_index1 = -1;
    int vntag_etag_profile_index = 0;
    int vntag_etag_profile_index1 = 0;
    int ref_count = 0;
#endif
#ifdef BCM_RIOT_SUPPORT
    int dvp = -1;
#endif

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Check if embedded NH table is being used */
    embedded_nh = BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit))
    {
        int ipmc = 0;
        ipmc = (l3cfg->l3c_flags & BCM_L3_IPMC);
        /*
         * Get table memory. 
         */
        BCM_TH3_L3_HOST_TABLE_MEM(unit, l3cfg->l3c_intf, ipv6, ipmc, mem, nh_idx);

        /*
         * Set table fields 
         */
        BCM_TH3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

        /*
         * Assign entry buf based on table being used 
         */
        BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem, bufp, l3v4_entry, l3v4_ext_entry, l3v6_entry, l3v6_ext_entry);
    }
    else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit))
    {
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {

        return _bcm_td3_l3_flex_entry_add(unit, l3cfg, nh_idx);
    }

        /* Get table memory. */
        BCM_TD3_L3_HOST_TABLE_MEM(unit, l3cfg->l3c_intf, ipv6, mem, nh_idx);
    
        /* Set table fields */
        BCM_TD3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

        /* Assign entry buf based on table being used */
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                      l3v4_entry,
                                      l3v4_ext_entry,
                                      l3v6_entry,
                                      l3v6_ext_entry);
    } else 
#endif
    {
    /* Get table memory. */
    BCM_TD2_L3_HOST_TABLE_MEM(unit, l3cfg->l3c_intf, ipv6, mem, nh_idx);

    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
    }
    /* Prepare host entry for addition. */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, l3cfg, bufp));

    /* Set hit bit. */
    if (l3cfg->l3c_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, mem, bufp, fld->hit, 1);
    }

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, bufp, fld->rpe, 1);
    }

    /* Set destination discard bit. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, bufp, fld->dst_discard, 1);
    }

    /* Set local addr bit. */
    if (l3cfg->l3c_flags & BCM_L3_HOST_LOCAL) {
        soc_mem_field32_set(unit, mem, bufp, fld->local_addr, 1);
    }

    /* Get bfd_enable bit. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->bfd_enable)) {
        if (l3cfg->l3c_bfd_enable) {
            soc_mem_field32_set(unit, mem, bufp, fld->bfd_enable, 1);
        }
    }
#ifdef BCM_MONTEREY_SUPPORT
    /* Set Roe Header Decap control */
    if (soc_feature(unit, soc_feature_roe) ||
            soc_feature(unit, soc_feature_roe_custom)) {
        if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_DECAP_ROE_HEADER) {
            soc_mem_field32_set(unit, mem, bufp, fld->roe_decap, 1);
        }
    }
#endif

    /* Set classification group id. */
    soc_mem_field32_set(unit, mem, bufp, fld->class_id, 
                        l3cfg->l3c_lookup_class);
    /*  Set priority. */
    soc_mem_field32_set(unit, mem, bufp, fld->priority, l3cfg->l3c_prio);

    if (embedded_nh) {
        sal_memset(&l3_intf, 0, sizeof(_bcm_l3_intf_cfg_t));
        BCM_XGS3_L3_INTF_IDX_SET(l3_intf, l3cfg->l3c_intf);

        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = BCM_XGS3_L3_HWCALL_EXEC(unit, if_get) (unit, &l3_intf);
        BCM_XGS3_L3_MODULE_UNLOCK(unit);

        port_addr_max = SOC_PORT_ADDR_MAX(unit) + 1;
        modid_max = SOC_MODID_MAX(unit) + 1;
#ifdef BCM_RIOT_SUPPORT
        if ((BCMI_RIOT_IS_ENABLED(unit))
            && (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
            _bcm_vp_vfi_type_vp_get(unit, l3cfg->l3c_port_tgid, &dvp);
        } else
#endif
        {
            if (l3cfg->l3c_flags & BCM_L3_TGID) {
                glp = (BCM_TGID_PORT_TRUNK_MASK(unit) & l3cfg->l3c_port_tgid);
#if defined(BCM_TRIDENT3_SUPPORT)
                if (!SOC_IS_TRIDENT3X(unit)) {
                    glp |= (port_addr_max * modid_max);
                }
#else
                    glp |= (port_addr_max * modid_max);
#endif
            } else {
                port_id = (SOC_PORT_ADDR_MAX(unit) & l3cfg->l3c_port_tgid);
                modid = (SOC_MODID_MAX(unit) & l3cfg->l3c_modid) * port_addr_max;
                glp = modid | port_id;
            }
        }
        soc_mem_field32_set(unit, mem, bufp, fld->l3_intf, l3cfg->l3c_intf);
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (dvp != -1) {
            soc_mem_field32_dest_set(unit, mem, bufp, fld->dest, 
                                     SOC_MEM_FIF_DEST_DVP, dvp);
        } else {
            uint32 dest_type;
            if (l3cfg->l3c_flags & BCM_L3_TGID) {
                dest_type = SOC_MEM_FIF_DEST_LAG;
            } else {
                dest_type = SOC_MEM_FIF_DEST_DGPP;
            }
            soc_mem_field32_dest_set(unit, mem, bufp, fld->dest, 
                                     dest_type, glp);
        }
    } else
#endif
    {
#ifdef BCM_RIOT_SUPPORT
        if (dvp != -1) {
            soc_mem_field32_set(unit, mem, bufp, fld->dest_type, 1);
            soc_mem_field32_set(unit, mem, bufp, fld->destination, dvp);
        } else
#endif
        {
            soc_mem_field32_set(unit, mem, bufp, fld->glp, glp);
        }
    }
#ifdef BCM_TOMAHAWK_SUPPORT
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            if (l3cfg->l3c_flags & BCM_L3_REPLACE) {
                /* For BCM_L3_REPLACE, get profile index for EGR_MACDA_OUI_PROFILE
                 * and EGR_VNTAG_ETAG_PROFILE table. Delete the profile entry if
                 * it's not used by any other host.
                 */
                rv = _bcm_th_get_extended_profile_index(unit, mem, l3cfg,
                                                    &macda_oui_profile_index,
                                                    &vntag_etag_profile_index,
                                                    &ref_count);
                if (BCM_FAILURE(rv) && (rv!=BCM_E_NOT_FOUND)) {
                    return rv;
                }
                if (macda_oui_profile_index == -1) {
                    /* Host entry was done in regular L3_ENTRY table view */
                    BCM_IF_ERROR_RETURN
                    (_bcm_th_l3_vp_entry_add(unit, mem, l3cfg, bufp,
                                             &macda_oui_profile_index1,
                                             &vntag_etag_profile_index1));
                } else {                                
                    if (ref_count > 1) {
                        rv = _bcm_th_l3_vp_entry_add(unit, mem, l3cfg, bufp,
                                                     &macda_oui_profile_index1,
                                                     &vntag_etag_profile_index1);
                        if (BCM_SUCCESS(rv)) {
                            BCM_IF_ERROR_RETURN
                            (_bcm_th_l3_vp_entry_del(unit, l3cfg, 
                                                    macda_oui_profile_index,
                                                    vntag_etag_profile_index));
                        } else {
                            return rv;
                        }
                    } else {
                        BCM_IF_ERROR_RETURN
                        (_bcm_th_l3_vp_entry_del(unit, l3cfg,
                                                macda_oui_profile_index,
                                                vntag_etag_profile_index));
                        BCM_IF_ERROR_RETURN
                        (_bcm_th_l3_vp_entry_add(unit, mem, l3cfg, bufp,
                                                 &macda_oui_profile_index1,
                                                 &vntag_etag_profile_index1));

                    }
                }

            } else {
                BCM_IF_ERROR_RETURN
                (_bcm_th_l3_vp_entry_add(unit, mem, l3cfg, bufp,
                                         &macda_oui_profile_index1,
                                         &vntag_etag_profile_index1));

            }

        } else
#endif
#if defined(BCM_RIOT_SUPPORT)
        if ((soc_feature(unit, soc_feature_riot)) &&
            (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
            if (l3cfg->l3c_flags & BCM_L3_REPLACE) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_l3_macda_oui_profile_entry_replace(unit, mem, l3cfg, bufp));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_l3_macda_oui_profile_entry_add(unit, mem, l3cfg, bufp,
                                                              &macda_oui_profile_index));
            }
        } else
#endif
        {
            soc_mem_mac_addr_set(unit, mem, bufp,
                                fld->mac_addr, l3cfg->l3c_mac_addr);
        }
        soc_mem_field32_set(unit, mem, bufp, 
                            fld->eh_tag_type, l3cfg->l3c_eh_q_tag_type);
        soc_mem_field32_set(unit, mem, bufp, 
                            fld->eh_queue_tag, l3cfg->l3c_eh_q_tag);

        /* Set copy_to_cpu bit. */
        if (l3cfg->l3c_flags & BCM_L3_COPY_TO_CPU) {
            if (SOC_MEM_FIELD_VALID(unit, mem, fld->copy_to_cpu)) {
                soc_mem_field32_set(unit, mem, bufp, fld->copy_to_cpu, 1);
            }
        }
    } else {
        /* Set next hop index. */
        if (l3cfg->l3c_flags & BCM_L3_MULTIPATH) {
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_dest_set(unit, mem, bufp, fld->dest,
                                        SOC_MEM_FIF_DEST_ECMP, nh_idx);
            } else
#endif
            {

            soc_mem_field32_set(unit, mem, bufp, ECMP_PTRf, nh_idx);
            soc_mem_field32_set(unit, mem, bufp, ECMPf, 1);
            }
        } else {
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
               soc_mem_field32_dest_set(unit, mem, bufp, fld->dest, 
                                        SOC_MEM_FIF_DEST_NEXTHOP, nh_idx);
            } else
#endif
            {
            soc_mem_field32_set(unit, mem, bufp, fld->nh_idx, nh_idx);
        }
    }
    }

    /* Insert in table */
    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, bufp);

    /* Handle host entry 'replace' actions */
    if ((BCM_E_EXISTS == rv) && (l3cfg->l3c_flags & BCM_L3_REPLACE)) {
        rv = BCM_E_NONE;
    }


#ifdef BCM_TOMAHAWK_SUPPORT
    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        if (BCM_FAILURE(rv)) {
            /* if host add is not successful, then corresponding entries in the
             * profile table need to be removed for extended view hosts.
             */
            BCM_IF_ERROR_RETURN
            (_bcm_th_l3_vp_entry_del(unit, l3cfg,
                                     macda_oui_profile_index1,
                                     vntag_etag_profile_index1));
        }
    }
#endif
#if defined(BCM_RIOT_SUPPORT)
    if ((soc_feature(unit, soc_feature_riot)) &&
        (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
        if (BCM_FAILURE(rv)) {
            /* if host add is not successful, then corresponding entries in the
             * profile table need to be removed for extended view hosts.
             */
            BCM_IF_ERROR_RETURN
                (_bcm_td2p_l3_macda_oui_profile_entry_del(unit, l3cfg, macda_oui_profile_index));
        }
    }
#endif /* BCM_RIOT_SUPPORT */

    /* Write status check. */
    if ((rv >= 0) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)++ : BCM_XGS3_L3_IP4_CNT(unit)++;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_td2_l3_entry_get
 * Purpose:
 *      Get an entry from TD2 3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 *      embd     - (OUT) If TRUE, entry was found as embedded NH.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_entry_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx, int *embd)
{
    int ipv6;                                   /* IPv6 entry indicator.    */
    int clear_hit;                              /* Clear hit bit.           */
    int rv = BCM_E_NONE;                        /* Operation return status. */
    soc_mem_t mem, mem_ext;                     /* L3 table memories        */
    uint32 *buf_key, *buf_entry;                /* Key and entry buffer ptrs*/
    l3_entry_ipv4_unicast_entry_t l3v4_key, l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_key, l3v4_ext_entry;   /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_key, l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_key, l3v6_ext_entry;   /* IPv6-Embedded */

    /* Indexed NH entry */
    *embd = _BCM_TD2_HOST_ENTRY_NOT_FOUND;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Preserve clear_hit value. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Table memories */
    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6); mem_ext = BCM_XGS3_L3_MEM(unit, v6_4);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4); mem_ext = BCM_XGS3_L3_MEM(unit, v4_2);
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /*
         * Assign entry-key buf based on table being used 
         */
        BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key, l3v4_key, l3v4_ext_key, l3v6_key, l3v6_ext_key);

        /*
         * Assign entry buf based on table being used 
         */
        BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry, l3v4_entry, l3v4_ext_entry, l3v6_entry, l3v6_ext_entry);
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {

        return _bcm_td3_l3_flex_entry_get(unit, l3cfg, nh_idx, embd);
    }

         /* Assign entry-key buf based on table being used */
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                            l3v4_key,
                                            l3v4_ext_key,
                                            l3v6_key,
                                            l3v6_ext_key);
    
        /* Assign entry buf based on table being used */
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                            l3v4_entry,
                                            l3v4_ext_entry,
                                            l3v6_entry,
                                            l3v6_ext_entry);
    } else
#endif
    {
    /* Assign entry-key buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                        l3v4_key,
                                        l3v4_ext_key,
                                        l3v6_key,
                                        l3v6_ext_key);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                        l3v4_entry,
                                        l3v4_ext_entry,
                                        l3v6_entry,
                                        l3v6_ext_entry);
    }
    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, l3cfg, buf_key));

    /* Perform lookup */
    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                _BCM_TD2_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {
        /* Indexed NH entry */
        *embd = FALSE;
        /* Extract entry info. */
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_parse(unit, mem, l3cfg,
                                                  nh_idx, buf_entry, 0));
        /* Clear the HIT bit */
        if (clear_hit) {
            BCM_IF_ERROR_RETURN(_bcm_td2_l3_clear_hit(unit, mem,
                                       l3cfg, buf_entry, l3cfg->l3c_hw_index));
        }
    } else if ((BCM_E_NOT_FOUND == rv) &&
               (soc_feature(unit, soc_feature_l3_extended_host_entry))) {
        /* Search in the extended tables only if the extended host entry
         * feature is supported and if the entry was not found in the 
         * regular host entry */

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit))
        {
            /*
             * Assign entry-key buf based on table being used 
             */
            BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key, l3v4_key, l3v4_ext_key, l3v6_key, l3v6_ext_key);

            /*
             * Assign entry buf based on table being used 
             */
            BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                      l3v4_entry, l3v4_ext_entry, l3v6_entry, l3v6_ext_entry);
        }
        else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit))
        {
        /* Assign entry-key buf based on table being used */
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                                        l3v4_key,
                                        l3v4_ext_key,
                                        l3v6_key,
                                        l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                        l3v4_entry,
                                        l3v4_ext_entry,
                                        l3v6_entry,
                                        l3v6_ext_entry);
        } else
#endif
        {
        /* Assign entry-key buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                                        l3v4_key,
                                        l3v4_ext_key,
                                        l3v6_key,
                                        l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                        l3v4_entry,
                                        l3v4_ext_entry,
                                        l3v6_entry,
                                        l3v6_ext_entry);
		}

        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem_ext, l3cfg, buf_key));

        /* Perform lookup hw. */
        rv = soc_mem_generic_lookup(unit, mem_ext, MEM_BLOCK_ANY,
                                    _BCM_TD2_L3_MEM_BANKS_ALL,
                                    buf_key, buf_entry, &l3cfg->l3c_hw_index);

        if (BCM_SUCCESS(rv)) {

            /* Embedded NH entry */
            *embd = TRUE;
            /* Extract entry info. */
            BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_parse(unit, mem_ext, l3cfg,
                                                          nh_idx, buf_entry, 0));
            /* Clear the HIT bit */
            if (clear_hit) {
                BCM_IF_ERROR_RETURN(_bcm_td2_l3_clear_hit(unit, mem_ext,
                                              l3cfg, buf_entry, l3cfg->l3c_hw_index));
            }
        }
    } 
     
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_entry_del
 * Purpose:
 *      Delete an entry from TD2 L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_entry_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int ipv6;                          /* IPv6 entry indicator.*/
    soc_mem_t mem, mem_ext;            /* L3 table memory.     */  
    int rv = BCM_E_NONE;               /* Operation status.    */
    uint32 *buf_entry;                 /* Entry buffer ptrs    */
    l3_entry_ipv4_unicast_entry_t l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;   /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;   /* IPv6-Embedded */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_RIOT_SUPPORT)
        int macda_oui_profile_index = -1;
        int ref_count = 0;
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
        int vntag_etag_profile_index = 0;
#endif

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6); mem_ext = BCM_XGS3_L3_MEM(unit, v6_4);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4); mem_ext = BCM_XGS3_L3_MEM(unit, v4_2);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {
        return _bcm_td3_l3_flex_entry_del(unit, l3cfg);
    }
    /* Assign entry-key buf based on table being used */
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                            l3v4_entry,
                                            l3v4_ext_entry,
                                            l3v6_entry,
                                            l3v6_ext_entry);
    } else
#endif
    {
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                        l3v4_entry,
                                        l3v4_ext_entry,
                                        l3v6_entry,
                                        l3v6_ext_entry);
    }
    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, l3cfg, buf_entry));

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void*) buf_entry);

    if (rv >= 0) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)-- : BCM_XGS3_L3_IP4_CNT(unit)--;
    }    

    if ((BCM_E_NOT_FOUND == rv) &&
        (soc_feature(unit, soc_feature_l3_extended_host_entry))) { 
        /* Delete from  the extended tables only if the extended host entry
         * feature is supported and if the entry was not found in the 
         * regular host entry */

#ifdef BCM_TOMAHAWK_SUPPORT
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            BCM_IF_ERROR_RETURN
            (_bcm_th_get_extended_profile_index(unit, mem_ext, l3cfg,
                                                &macda_oui_profile_index,
                                                &vntag_etag_profile_index,
                                                &ref_count));
       }
#endif

#if defined(BCM_RIOT_SUPPORT)
        if ((soc_feature(unit, soc_feature_riot)) &&
            (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2p_l3_get_macda_oui_profile_index(unit, mem_ext, l3cfg,
                                                          &macda_oui_profile_index,
                                                          &ref_count));
        }
#endif
        /* Assign entry-key buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem_ext, l3cfg, buf_entry));

        rv = soc_mem_delete(unit, mem_ext, MEM_BLOCK_ANY, (void*) buf_entry);
        
        if (rv >= 0) {
            (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)-- : BCM_XGS3_L3_IP4_CNT(unit)--;
            
#ifdef BCM_TOMAHAWK_SUPPORT            
        BCM_IF_ERROR_RETURN
            (_bcm_th_l3_vp_entry_del(unit, l3cfg, 
                                     macda_oui_profile_index,
                                     vntag_etag_profile_index));
#endif
#if defined(BCM_RIOT_SUPPORT)
        if ((soc_feature(unit, soc_feature_riot)) &&
            (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_l3_macda_oui_profile_entry_del(unit, l3cfg,
                                                              macda_oui_profile_index));
            }
#endif
        }
    }

    return rv;
}

int
_bcm_td2_ip_key_to_l3cfg(int unit, bcm_l3_key_t *ipkey, _bcm_l3_cfg_t *l3cfg)
{
    int ipv6;           /* IPV6 key.                     */

    /* Input parameters check */
    if ((NULL == ipkey) || (NULL == l3cfg)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination buffer first. */
    sal_memset(l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg->l3c_rp_id = BCM_IPMC_RP_ID_INVALID;
    l3cfg->l3c_ing_intf = BCM_IF_INVALID;

    ipv6 = (ipkey->l3k_flags & BCM_L3_IP6);

    /* Set vrf id. */
    l3cfg->l3c_vrf = ipkey->l3k_vrf;
    l3cfg->l3c_vid = ipkey->l3k_vid;

    if (ipv6) {
        if (BCM_IP6_MULTICAST(ipkey->l3k_ip6_addr)) {
            /* Copy ipv6 group, source & vid. */
            sal_memcpy(l3cfg->l3c_ip6, ipkey->l3k_ip6_addr, sizeof(bcm_ip6_t));
            sal_memcpy(l3cfg->l3c_sip6, ipkey->l3k_sip6_addr,
                       sizeof(bcm_ip6_t));
            l3cfg->l3c_vid = ipkey->l3k_vid;
            l3cfg->l3c_flags = (BCM_L3_IP6 | BCM_L3_IPMC);
        } else {
            /* Copy ipv6 address. */
            sal_memcpy(l3cfg->l3c_ip6, ipkey->l3k_ip6_addr, sizeof(bcm_ip6_t));
            l3cfg->l3c_flags = BCM_L3_IP6;
        }
    } else {
        if (BCM_IP4_MULTICAST(ipkey->l3k_ip_addr)) {
            /* Copy ipv4 mcast group, source & vid. */
            l3cfg->l3c_ip_addr = ipkey->l3k_ip_addr;
            l3cfg->l3c_src_ip_addr = ipkey->l3k_sip_addr;
            l3cfg->l3c_vid = ipkey->l3k_vid;
            l3cfg->l3c_flags = BCM_L3_IPMC;
        } else {
            /* Copy ipv4 address . */
            l3cfg->l3c_ip_addr = ipkey->l3k_ip_addr;
        }
    }
    return (BCM_E_NONE);
}

int
_bcm_td2_l3cfg_to_ipkey(int unit, bcm_l3_key_t *ipkey, _bcm_l3_cfg_t *l3cfg)
{
    /* Input parameters check */
    if ((NULL == ipkey) || (NULL == l3cfg)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination buffer first. */
    sal_memset(ipkey, 0, sizeof(bcm_l3_key_t));

    ipkey->l3k_vrf = l3cfg->l3c_vrf;
    ipkey->l3k_vid = l3cfg->l3c_vid;

    if (l3cfg->l3c_flags & BCM_L3_IP6) {
        if (l3cfg->l3c_flags & BCM_L3_IPMC) {
            /* Copy ipv6 group, source & vid. */
            sal_memcpy(ipkey->l3k_ip6_addr, l3cfg->l3c_ip6, sizeof(bcm_ip6_t));
            sal_memcpy(ipkey->l3k_sip6_addr, l3cfg->l3c_sip6,
                       sizeof(bcm_ip6_t));
            ipkey->l3k_vid = l3cfg->l3c_vid;
        } else {
            /* Copy ipv6 address. */
            sal_memcpy(ipkey->l3k_ip6_addr, l3cfg->l3c_ip6, sizeof(bcm_ip6_t));
        }
    } else {
        if (l3cfg->l3c_flags & BCM_L3_IPMC) {
            /* Copy ipv4 mcast group, source & vid. */
            ipkey->l3k_ip_addr = l3cfg->l3c_ip_addr;
            ipkey->l3k_sip_addr = l3cfg->l3c_src_ip_addr;
            ipkey->l3k_vid = l3cfg->l3c_vid;
        } else {
            /* Copy ipv4 address . */
            ipkey->l3k_ip_addr = l3cfg->l3c_ip_addr;
        }
    }
    /* Store entry flags. */
    ipkey->l3k_flags = l3cfg->l3c_flags;
    return (BCM_E_NONE);
}

int
bcm_td2_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                        int cf_max, int *cf_count)
{
    _bcm_l3_cfg_t l3cfg;
    uint32 *bufp;      /* Hardware buffer ptr     */
    soc_mem_t mem;            /* L3 table memory type.   */  
    int nh_idx, index, rv = BCM_E_NONE;      /* Operation status.       */
    _bcm_l3_fields_t *fld;    /* L3 table common fields. */
    l3_entry_ipv4_unicast_entry_t l3v4_entry;    
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;    
    l3_entry_ipv6_unicast_entry_t l3v6_entry;
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;
    uint8 i,num_ent;
    uint32 l3_entry[SOC_MAX_MEM_WORDS];
    uint32 ipmc, ipv6, embedded_nh;
    uint32 bucket;   
    int num_banks, max_bank = 9;
    int hash_offset = 0, use_lsb = 0, bidx = 0;
    
    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((NULL == ipkey) || (NULL == cf_count) || 
        (NULL == cf_array) || (cf_max <= 0)) {
        return (BCM_E_PARAM);
    }

    embedded_nh = ipkey->l3k_flags & BCM_L3_DEREFERENCED_NEXTHOP;
    nh_idx = (embedded_nh) ? BCM_XGS3_L3_INVALID_INDEX : 0;

    /* Translate lookup key to l3cfg format. */
    BCM_IF_ERROR_RETURN(_bcm_td2_ip_key_to_l3cfg(unit, ipkey, &l3cfg));
    
    /* Get entry type. */
    ipv6 = (l3cfg.l3c_flags & BCM_L3_IP6);
    ipmc = (l3cfg.l3c_flags & BCM_L3_IPMC);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit))
    {
        /*
         * Get table memory. 
         */
        BCM_TH3_L3_HOST_TABLE_MEM(unit, l3cfg.l3c_intf, ipv6, ipmc, mem, nh_idx);
        BCM_TH3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem, bufp, l3v4_entry, l3v4_ext_entry, l3v6_entry, l3v6_ext_entry);
        max_bank = 3;
    } else
#endif
    {
    /* Get table memory. */
    BCM_TD2_L3_HOST_TABLE_MEM(unit, l3cfg.l3c_intf, ipv6, mem, nh_idx);

    /* Bump up the size conditionally */
    mem = (embedded_nh || ipmc) ? (ipv6 ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm) : mem;
    
    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                    l3v4_entry,
                                    l3v4_ext_entry,
                                    l3v6_entry,
                                    l3v6_ext_entry);
    }
    /* Prepare host entry for addition. */
    if (ipmc) {
        /*  Zero entry buffer. */
        sal_memcpy(bufp, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ipmc_ent_init(unit, bufp, &l3cfg));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, &l3cfg, bufp));
    }   

    *cf_count = 0;

    /* Get entries of each banks */
    switch (mem)
    {
    case L3_ENTRY_IPV4_UNICASTm:
        case L3_ENTRY_SINGLEm:
        num_ent =  4;
        break;
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
        case L3_ENTRY_DOUBLEm:
        num_ent =  2;
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
        case L3_ENTRY_QUADm:
        num_ent =  1;
        break;

    /* coverity[dead_error_begin] */
    default:
        return BCM_E_PARAM;        
    }

    

    /* Get number of L3 banks */
#ifdef BCM_TOMAHAWK_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_bank_count_get(unit, mem, &num_banks));
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
        if (SOC_IS_TOMAHAWKX(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_hash_bank_count_get(unit, mem, &num_banks));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_apache_hash_bank_count_get(unit, mem, &num_banks));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5X(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_helix5_hash_bank_count_get(unit, mem, &num_banks));
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                /* Bank 1 to 0 are dedicated L3 banks with no shared banks */
                max_bank = 1;
            }
#endif /* BCM_HURRICANE4_SUPPORT */
        } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_trident3_hash_bank_count_get(unit, mem, &num_banks));
        } else
#endif /* BCM_TRIDENT3_SUPPORT */

        {
            SOC_IF_ERROR_RETURN
                (soc_trident2_hash_bank_count_get(unit, mem, &num_banks));
        }

    /* bank 9 to 6 are dedicated L3 banks, 5 to 3 are dedicated, depends on
     * mode how many shared banks are assigned to l3 
     */
    for (bidx = max_bank; bidx >= (max_bank - num_banks + 1); bidx--) {
        /* Calculate hash index */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            rv = soc_ap_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset,
                                        &use_lsb);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = soc_tomahawk3_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset, &use_lsb);
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */

        if (SOC_IS_TOMAHAWKX(unit)) {
            rv = soc_th_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset,
                                        &use_lsb);
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5X(unit)) {
            rv = soc_hx5_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset,
                                         &use_lsb);
        } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = soc_td3_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset,
                                        &use_lsb);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            rv = soc_td2_hash_offset_get(unit, L3_ENTRY_ONLYm, bidx, &hash_offset,
                                         &use_lsb);
        }

        if (BCM_FAILURE(rv)) {
            return BCM_E_INTERNAL;
        }
        
        /* coverity[overrun-buffer-val: FALSE] */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            bucket = soc_ap_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_ap_hash_index_get(unit, mem, bidx, bucket);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            bucket = soc_tomahawk3_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_tomahawk3_hash_index_get(unit, mem, bidx, bucket);
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            bucket = soc_th_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_th_hash_index_get(unit, mem, bidx, bucket);
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5X(unit)) {
            bucket = soc_hx5_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_hx5_hash_index_get(unit, mem, bidx, bucket);
        } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            bucket = soc_td3_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_td3_hash_index_get(unit, mem, bidx, bucket);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            bucket = soc_td2_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
            index = soc_hash_index_get(unit, mem, bidx, bucket);
        }

        /* Get conflicting IPs from all HW entries in current bank */
        for (i = 0; (i < num_ent) && (*cf_count < cf_max);) {
            rv = soc_mem_read(unit, mem, COPYNO_ALL, index + i, l3_entry);
            if (SOC_FAILURE(rv)) {
                return BCM_E_MEMORY;
            }
            if (soc_mem_field32_get(unit, mem, &l3_entry, fld->valid)) {
                switch(soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type)) {
                case TD2_L3_HASH_KEY_TYPE_V4UC:
                      l3cfg.l3c_flags = 0;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V4UC_EXT:
                      l3cfg.l3c_flags = BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V4MC:
                      l3cfg.l3c_flags = BCM_L3_IPMC;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V6UC:
                      l3cfg.l3c_flags = BCM_L3_IP6;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V6UC_EXT:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V6MC:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V4L2MC:
                case TD2_L3_HASH_KEY_TYPE_V4L2VPMC:
                      l3cfg.l3c_flags = BCM_L3_IPMC | BCM_L3_L2ONLY;
                      break;
                case TD2_L3_HASH_KEY_TYPE_V6L2MC:
                case TD2_L3_HASH_KEY_TYPE_V6L2VPMC:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC | BCM_L3_L2ONLY;
                default:
                      break;
                }
                /* Get host ip address. */                   		
                rv = _bcm_td2_l3_get_by_idx(unit, NULL, index + i, &l3cfg, &nh_idx);
                if (BCM_FAILURE(rv)) {
                    continue;          
                }
                                    
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_l3cfg_to_ipkey(unit, cf_array + (*cf_count), &l3cfg));
                if ((++(*cf_count)) >= cf_max) {
                    break;
                }
            }
            /* increment based upon existing entry */
            i += (mem == ((SOC_MEM_IS_VALID(unit, L3_ENTRY_IPV6_MULTICASTm))?L3_ENTRY_IPV6_MULTICASTm:L3_ENTRY_DOUBLEm)) ? 2 : 1;
        }

        /* Stop loop if conflicting IPs got from HW is no less than cf_max. */
        if ((*cf_count) >= cf_max) {
            break;
        }
    } 
    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_td2_l3_get_host_ent_by_idx
 * Purpose:
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      soc_mem_ - (IN)Table memory.
 *      idx      - (IN)Index to read.
 *      l3cfg    - (OUT)l3 entry search result.
 *      nh_index - (OUT)Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_get_host_ent_by_idx(int unit, void *dma_ptr, soc_mem_t mem,
                                int idx, _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    uint32 ipv6;                       /* IPv6 entry indicator.   */
    int key_type;
    int clear_hit;                     /* Clear hit bit flag.     */
    _bcm_l3_fields_t *fld;             /* L3 table common fields. */
    uint32 valid, base_valid_bits = 1;
    uint32 *buf_entry;                 /* Key and entry buffer ptrs*/
    l3_entry_ipv4_unicast_entry_t l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry; /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry; /* IPv6-Embedded */
    soc_field_t uc_ipv6_lwr = INVALIDf, uc_ipv6_upr = INVALIDf;
    soc_field_t data_type = 44;
#ifdef BCM_TRIDENT3_SUPPORT
    int rv = BCM_E_NOT_FOUND;
    uint32 view_id = 0;
#endif
    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    /* Get clear hit flag. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit))
        {
            BCM_TH3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
            BCM_TH3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry, l3v4_entry, l3v4_ext_entry, l3v6_entry, l3v6_ext_entry);
        }
        else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_TD3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                        l3v4_entry,
                                        l3v4_ext_entry,
                                        l3v6_entry,
                                        l3v6_ext_entry);
    } else

#endif
    {

    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                    l3v4_entry,
                                    l3v4_ext_entry,
                                    l3v6_entry,
                                    l3v6_ext_entry);
    }
    

    if (NULL == dma_ptr) { /* Read from hardware. */
        /* Zero buffers. */
        sal_memset(buf_entry, 0, BCM_L3_MEM_ENT_SIZE(unit, mem));

        /* Read entry from hw. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, buf_entry)); 
    } else { /* Read from dma. */
        buf_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                   uint32 *, dma_ptr, idx);
    }

    valid = soc_mem_field32_get(unit, mem, buf_entry, fld->valid);
    key_type = soc_mem_field32_get(unit, mem, buf_entry, fld->key_type);
    base_valid_bits = 1;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
        if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            data_type = soc_mem_field32_get(unit, mem, buf_entry, DATA_TYPEf);
            switch(mem) {
                case L3_ENTRY_SINGLEm:
                    base_valid_bits = 1;
                    break;
                case L3_ENTRY_DOUBLEm:
                    if ((key_type == 0) ||
                        ((key_type == (int)TD2_L3_HASH_KEY_TYPE_V6UC) &&
                                        (SOC_IS_TOMAHAWK3(unit)))) {
                        base_valid_bits = 1;
                    } else {
                        base_valid_bits = 3;
                    }
                    break;
                case L3_ENTRY_QUADm:
                    if (SOC_IS_TRIDENT3X(unit)) {
                        if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_V6UC_EXT) &&
                                (data_type == TD3_L3_HASH_DATA_TYPE_V6UC_EXT)) {
                            base_valid_bits = 3;
                        } else {
                            base_valid_bits = 5;
                        }
                    } else if ((key_type == (int)TD2_L3_HASH_KEY_TYPE_V6MC) ||
                              (SOC_IS_TOMAHAWK3(unit))) {
                        base_valid_bits = 1;
                    }
                    break;
                default:
                    break;
            }
        }
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
        soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
        uint32 num_ffo = 0;

        rv = soc_flow_db_mem_to_view_id_get(unit,
                mem,
                SOC_FLOW_DB_KEY_TYPE_INVALID,
                data_type,
                0,
                NULL,
                &view_id);
        if (rv == BCM_E_NONE) {
            /* get the flow handle, function id, option id */
            BCM_IF_ERROR_RETURN(
                soc_flow_db_mem_view_to_ffo_get(unit,
                    view_id,
                    SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                    ffo,
                    &num_ffo));
            l3cfg->l3c_flow_handle = ffo[0].flow_handle;
            l3cfg->l3c_flow_option_handle = ffo[0].option_id;
        }
    }
#endif
    if(valid != base_valid_bits) {
        if (valid) {
           base_valid_bits = 1;
        }
        return (BCM_E_NOT_FOUND);
    }

    l3cfg->l3c_flags = 0;
    switch (mem) {
    case L3_ENTRY_IPV4_UNICASTm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_ENTRY_SINGLEm:
#endif
        if  (!(soc_feature(unit, soc_feature_flex_flow)) &&
             ((key_type != TD2_L3_HASH_KEY_TYPE_V4UC) ||
             ((SOC_IS_TRIDENT3X(unit)) && (mem == L3_ENTRY_SINGLEm) && (data_type != 0)))) {
            /* All other keys are invalid */
            return BCM_E_NOT_FOUND;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        /* In TD3, can't distinguish regular from extended entries as
         * in previous chips. Hence check for next entry BASE_VALID field
         * to check for extended entries */
        if (SOC_IS_TRIDENT3X(unit) && (idx != soc_mem_index_max(unit,
            L3_ENTRY_SINGLEm))) {
            l3_entry_ipv4_unicast_entry_t td3_l3v4_entry;
            uint32 *td3_buf_entry = (uint32 *) &td3_l3v4_entry;

            if (NULL == dma_ptr) { /* Read from hardware. */
                /* Zero buffers. */
                sal_memset(td3_buf_entry, 0, BCM_L3_MEM_ENT_SIZE(unit, mem));

                /* Read entry from hw. */
                BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx + 1,
                                                      td3_buf_entry));
            } else { /* Read from dma. */
                td3_buf_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                    uint32 *, dma_ptr, idx + 1);
            }

            valid = soc_mem_field32_get(unit, mem, td3_buf_entry, fld->valid);
            if (valid == 7) {
                return BCM_E_NOT_FOUND;
            }
        }
#endif
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_ENTRY_DOUBLEm:
#endif
        if (key_type == TD2_L3_HASH_KEY_TYPE_V4MC) {
            l3cfg->l3c_flags = BCM_L3_IPMC;
        } else if (((key_type == TD2_L3_HASH_KEY_TYPE_V4UC) &&
                (mem == L3_ENTRY_DOUBLEm) && (data_type == 1)) ||
                ((key_type == TD2_L3_HASH_KEY_TYPE_V4UC_EXT) && (mem != L3_ENTRY_DOUBLEm))){
            l3cfg->l3c_flags = 0;
        } else if ((key_type == TD2_L3_HASH_KEY_TYPE_V4L2MC) || 
            (key_type == TD2_L3_HASH_KEY_TYPE_V4L2VPMC)) {
            l3cfg->l3c_flags = BCM_L3_IPMC | BCM_L3_L2ONLY;
        } else if ((key_type == TD2_L3_HASH_KEY_TYPE_V6UC) &&
              (((mem == L3_ENTRY_DOUBLEm) &&
                (SOC_IS_TOMAHAWK3(unit) || (data_type == 2))) ||
               (mem != L3_ENTRY_DOUBLEm))){

            l3cfg->l3c_flags = BCM_L3_IP6;
            if (mem == L3_ENTRY_DOUBLEm) {
            uc_ipv6_lwr = IPV6UC__IP_ADDR_LWR_64f;
            uc_ipv6_upr = IPV6UC__IP_ADDR_UPR_64f;
        } else {
                uc_ipv6_lwr = IP_ADDR_LWR_64f;
                uc_ipv6_upr = IP_ADDR_UPR_64f;
            }
        } else {
#if defined(BCM_TRIDENT3_SUPPORT)
            if ((SOC_IS_TRIDENT3X(unit)) && (rv == BCM_E_NONE)) {
                /* If it is a flex entry in TD3X, do not return error */
            } else
#endif
            {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    case L3_ENTRY_IPV6_UNICASTm:
        if (key_type == TD2_L3_HASH_KEY_TYPE_V6UC) {
            l3cfg->l3c_flags = BCM_L3_IP6;
            uc_ipv6_lwr = IP_ADDR_LWR_64f;
            uc_ipv6_upr = IP_ADDR_UPR_64f;
        } else {
            return BCM_E_NOT_FOUND;
        }
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_ENTRY_QUADm:
#endif
        uc_ipv6_lwr =  IPV6UC_EXT__IP_ADDR_LWR_64f;
        uc_ipv6_upr =  IPV6UC_EXT__IP_ADDR_UPR_64f;
        if (key_type == TD2_L3_HASH_KEY_TYPE_V6MC) {
            l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
        } if ((key_type == TD2_L3_HASH_KEY_TYPE_V6UC_EXT)) {
            l3cfg->l3c_flags = BCM_L3_IP6;
        } else if ((key_type == TD2_L3_HASH_KEY_TYPE_V6L2MC) ||
                (key_type == TD2_L3_HASH_KEY_TYPE_V6L2VPMC)) {
            l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC | BCM_L3_L2ONLY;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        else if (SOC_IS_TRIDENT3X(unit) && (key_type ==
                TD3_L3_HASH_KEY_TYPE_V6UC_EXT) && (data_type ==
                TD3_L3_HASH_DATA_TYPE_V6UC_EXT)) {
            l3cfg->l3c_flags = BCM_L3_IP6;
        }
#endif
        else {
#if defined(BCM_TRIDENT3_SUPPORT)
            if ((soc_feature(unit, soc_feature_flex_flow)) && (rv == BCM_E_NONE)) {
                /* TD3X flex entry */
                if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__DIPf)) {
                    l3cfg->l3c_flags = BCM_L3_IP6;
                    soc_mem_ip6_addr_get(unit, view_id, buf_entry,IPV6__DIPf, l3cfg->l3c_ip6, 0);
                } else if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__SIPf)) {
                    l3cfg->l3c_flags = BCM_L3_IP6;
                    soc_mem_ip6_addr_get(unit, view_id, buf_entry,IPV6__SIPf, l3cfg->l3c_ip6, 0);
                }
            } else
#endif
            {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    default :
        return BCM_E_NOT_FOUND;
    }

    /* Ignore protocol mismatch & multicast entries. */
    if ((ipv6  != (l3cfg->l3c_flags & BCM_L3_IP6)) ||
        (l3cfg->l3c_flags & BCM_L3_IPMC)) {
        return (BCM_E_NONE);
    }

    if (ipv6) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (!(soc_feature(unit, soc_feature_flex_flow) && (rv == BCM_E_NONE)))
#endif
        {/* Extract host info from the entry. */
            soc_mem_ip6_addr_get(unit, mem, buf_entry,
                    uc_ipv6_lwr, l3cfg->l3c_ip6,
                    SOC_MEM_IP6_LOWER_ONLY);

            soc_mem_ip6_addr_get(unit, mem, buf_entry,
                    uc_ipv6_upr, l3cfg->l3c_ip6,
                    SOC_MEM_IP6_UPPER_ONLY);
        }
    }
    /* Set index to l3cfg. */
    l3cfg->l3c_hw_index = idx;

    if (!ipv6) {
        l3cfg->l3c_ip_addr = 
            soc_mem_field32_get(unit, mem, buf_entry, fld->ip4);
    }

    /* Parse entry data. */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_l3_ent_parse(unit, mem, l3cfg, nh_idx, buf_entry, key_type));

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_clear_hit(unit, mem, l3cfg,
                                           (void *)buf_entry, idx));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_ent_init
 * Purpose:
 *      Set GROUP/SOURCE/VID/IMPC flag in the entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3x_entry - (IN/OUT) IPMC entry to fill. 
 *      l3cfg     - (IN) Api IPMC data. 
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_l3_ipmc_ent_init(int unit, uint32 *buf_p,
                             _bcm_l3_cfg_t *l3cfg)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    int ipv6;                          /* IPv6 entry indicator. */
    int idx;                           /* Iteration index.      */
    soc_field_t v4typef[] = { KEY_TYPE_0f, KEY_TYPE_1f };
    soc_field_t v6typef[] = { KEY_TYPE_0f, KEY_TYPE_1f, 
                              KEY_TYPE_2f, KEY_TYPE_3f };
    soc_field_t validf[] = { VALID_0f, VALID_1f, VALID_2f, VALID_3f };
    int rv = BCM_E_NONE;
    int vfi, key_type = 0;
    uint32 base_valid0, base_valid1, base_valid2, base_valid3;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
        mem = (ipv6) ? L3_ENTRY_QUADm : L3_ENTRY_DOUBLEm;
    } else {
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;
    }

    if (ipv6) {
        /* Set group address. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
        l3cfg->l3c_ip6[0] = 0x0;    /* Don't write ff entry already mcast. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_UPR_56f, 
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
        l3cfg->l3c_ip6[0] = 0xff;    /* Restore The entry  */

        /* Set source  address. */
        if (soc_mem_field_valid(unit, mem, IPV6MC__SOURCE_IP_ADDR_BITS_63_32f)) {
            soc_mem_ip6_addr_set(unit, mem, buf_p,
                IPV6MC__SOURCE_IP_ADDR_BITS_63_32f, l3cfg->l3c_sip6,
                SOC_MEM_IP6_BITS_63_32);
            soc_mem_ip6_addr_set(unit, mem, buf_p,
                IPV6MC__SOURCE_IP_ADDR_BITS_31_0f, l3cfg->l3c_sip6,
                SOC_MEM_IP6_BITS_31_0);
        } else {
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        }
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        /* Set vlan id, VPN or L3_IIF. 
         * if flag BCM_L3_L2ONLY is set or l3c_ing_intf is invalid, then program
         * VLAN or VPN. 
         * Else program L3_IIF. 
         */
        if ((l3cfg->l3c_flags & BCM_L3_L2ONLY) ||
                (l3cfg->l3c_ing_intf == BCM_IF_INVALID)) {
            if (_BCM_VPN_IS_SET(l3cfg->l3c_vid)) {

                if (soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn) && 
                        (l3cfg->l3c_flags & BCM_L3_L2ONLY)) {
                    if (_BCM_VPN_IS_L3(l3cfg->l3c_vid)) {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_L3, l3cfg->l3c_vid);
                    } else if (_BCM_VPN_IS_VPWS(l3cfg->l3c_vid)) {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VPWS, l3cfg->l3c_vid);
                    } else {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l3cfg->l3c_vid);
                    }
                    soc_mem_field32_set(unit, mem, buf_p, IPV6MC__VFIf, vfi); 
                    key_type = TD2_L3_HASH_KEY_TYPE_V6L2VPMC;
                } else {
                    soc_mem_field32_set(unit, mem, buf_p, IPV6MC__L3_IIFf, l3cfg->l3c_vid);
                    key_type = TD2_L3_HASH_KEY_TYPE_V6MC;
                }
            } else {
                soc_mem_field32_set(unit, mem, buf_p, IPV6MC__VLAN_IDf, l3cfg->l3c_vid);
                key_type = TD2_L3_HASH_KEY_TYPE_V6L2MC;
            }
            if (!soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn) ||
                    !soc_property_get(unit, spn_IPMC_L2_USE_VLAN_VPN, 0)) {
                /* Override the key in case feature not supported */
                key_type = TD2_L3_HASH_KEY_TYPE_V6MC;
            }
        } else {
            soc_mem_field32_set(unit, mem, buf_p, IPV6MC__L3_IIFf,
                                l3cfg->l3c_ing_intf);
            key_type = TD2_L3_HASH_KEY_TYPE_V6MC;
        }
        if (soc_feature(unit, soc_feature_base_valid)) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, key_type);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, buf_p, DATA_TYPEf, key_type);
            }
            /* Set entry valid bit. */
            if (SOC_IS_TOMAHAWK3(unit)) {
                base_valid0 = 1;
                base_valid1 = 2;
                base_valid2 = 2;
                base_valid3 = 7;
            } else {
                base_valid0 = 5;
                base_valid1 = 6;
                base_valid2 = 6;
                base_valid3 = 7;
            }
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, base_valid0);
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, base_valid1);
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_2f, base_valid2);
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_3f, base_valid3);
        } else {
        for (idx = 0; idx < 4; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, v6typef[idx], 
                                key_type);
            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);
            }
        }
        /* Set virtual router id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV6MC__VRF_IDf, l3cfg->l3c_vrf);

    } else {
        /* Set group id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__GROUP_IP_ADDRf,
                            l3cfg->l3c_ip_addr);

        /* Set source address. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__SOURCE_IP_ADDRf,
                            l3cfg->l3c_src_ip_addr);

        if ((l3cfg->l3c_flags & BCM_L3_L2ONLY) ||
                (l3cfg->l3c_ing_intf == BCM_IF_INVALID)) {
            if (_BCM_VPN_IS_SET(l3cfg->l3c_vid)) {

                if (soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn) && 
                   (l3cfg->l3c_flags & BCM_L3_L2ONLY)) {
                    if (_BCM_VPN_IS_L3(l3cfg->l3c_vid)) {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_L3, l3cfg->l3c_vid);
                    } else if (_BCM_VPN_IS_VPWS(l3cfg->l3c_vid)) {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VPWS, l3cfg->l3c_vid);
                    } else {
                        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l3cfg->l3c_vid);
                    }
                    soc_mem_field32_set(unit, mem, buf_p, IPV4MC__VFIf, vfi); 
                    key_type = TD2_L3_HASH_KEY_TYPE_V4L2VPMC;
                } else {
                    soc_mem_field32_set(unit, mem, buf_p, IPV4MC__L3_IIFf, l3cfg->l3c_vid);
                    key_type = TD2_L3_HASH_KEY_TYPE_V4MC;
                }
            } else {
                soc_mem_field32_set(unit, mem, buf_p, IPV4MC__VLAN_IDf, l3cfg->l3c_vid);
                key_type = TD2_L3_HASH_KEY_TYPE_V4L2MC;
            }
            if (!soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn) ||
                 !soc_property_get(unit, spn_IPMC_L2_USE_VLAN_VPN, 0)) {
                /* Override the key in case feature not supported */
                key_type = TD2_L3_HASH_KEY_TYPE_V4MC;
            }
        } else {
            /* L3_IIF. */
            soc_mem_field32_set(unit, mem, buf_p, IPV4MC__L3_IIFf, l3cfg->l3c_ing_intf);
            key_type = TD2_L3_HASH_KEY_TYPE_V4MC;
        }

        if (soc_feature(unit, soc_feature_base_valid)) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, key_type);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, buf_p, DATA_TYPEf, key_type);
            }
            /* Set entry valid bit. */
            if (!SOC_IS_TOMAHAWK3(unit)) {
                base_valid0 = 3;
                base_valid1 = 4;
            } else {
                base_valid0 = 1;
                base_valid1 = 7;
            }
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, base_valid0);
            soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, base_valid1);
        } else {
        for (idx = 0; idx < 2; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, v4typef[idx], 
                                key_type);
            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);
        }
        }
        /* Set virtual router id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__VRF_IDf, l3cfg->l3c_vrf);
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        /* 
         * This Tomahawk specific code handles migration of 
         * T, MODULE_Id, PORT_NUM, TGID fields from L3_IPMC_1 table to
         * L3_ENTRY_IPV4_MULTICAST table 
         */
        int is_trunk, mod, port_tgid, no_src_check = FALSE;
        soc_field_t trunk_port, module_id,port_num, trunk_id;
        
        if (ipv6) {
            trunk_port = IPV6MC__Tf;
            module_id  = IPV6MC__MODULE_IDf;
            port_num   = IPV6MC__PORT_NUMf;
            trunk_id   = IPV6MC__TGIDf;
        } else {
            trunk_port = IPV4MC__Tf;
            module_id  = IPV4MC__MODULE_IDf;
            port_num   = IPV4MC__PORT_NUMf;
            trunk_id   = IPV4MC__TGIDf;
        }
        if ((l3cfg->l3c_flags & BCM_IPMC_SOURCE_PORT_NOCHECK) ||
            (l3cfg->l3c_port_tgid < 0)) {                       /* no source port */
            no_src_check = TRUE;
            is_trunk = 0;
            mod = SOC_MODID_MAX(unit);
            port_tgid = TH_IPMC_NO_SRC_CHECK_PORT;
        } else if (l3cfg->l3c_tunnel) {                         /* trunk source port */
            is_trunk = 1;
            mod = 0;
            port_tgid = l3cfg->l3c_port_tgid;
        } else {                                               /* source port */
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;
    
            mod_in = l3cfg->l3c_modid;
            port_in = l3cfg->l3c_port_tgid;
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                        mod_in, port_in,
                                        &mod_out, &port_out));
            /* Check parameters, since above is an application callback */
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return BCM_E_PORT;
            }
            is_trunk = 0;
            mod = mod_out;
            port_tgid = port_out;
        }
    
        if (is_trunk) {
            soc_mem_field32_set(unit, mem, buf_p, trunk_port, 1);
            soc_mem_field32_set(unit, mem, buf_p, trunk_id,  port_tgid);
        } else {
            if (!SOC_IS_TOMAHAWK3(unit)) {
            soc_mem_field32_set(unit, mem, buf_p, module_id, mod);
            }
            soc_mem_field32_set(unit, mem, buf_p, port_num,  port_tgid);
            if (no_src_check) {
                /* For no src check program all 1's in Tf, MOD_ID and PORT_NUM */
                if (SOC_IS_TRIDENT3X(unit))
                    soc_mem_field32_set(unit, mem, buf_p, trunk_port, 0);
                else
                soc_mem_field32_set(unit, mem, buf_p, trunk_port, 1);
            } else {
                soc_mem_field32_set(unit, mem, buf_p, trunk_port, 0);
            }
        }
    
    }
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_ent_parse
 * Purpose:
 *      Service routine used to parse hw l3 ipmc entry to api format.
 * Parameters:
 *      unit      - (IN)SOC unit number. 
 *      l3cfg     - (IN/OUT)l3 entry parsing destination buf.
 *      l3x_entry - (IN/OUT)hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_td2_l3_ipmc_ent_parse(int unit, _bcm_l3_cfg_t *l3cfg,
                          l3_entry_ipv6_multicast_entry_t *l3x_entry)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    uint32 *buf_p;                     /* HW buffer address.    */
    int ipv6;                          /* IPv6 entry indicator. */
    uint32 hit = 0;                    /* composite hit = hit_x|hit_y */
    l3_entry_hit_only_x_entry_t hit_x;
    l3_entry_hit_only_y_entry_t hit_y;
    l3_entry_hit_only_entry_t hit_default;
    int idx, idx_max, idx_offset, hit_idx_shift;   /* Iteration index.      */
    soc_field_t hitf[] = {HIT_0f, HIT_1f, HIT_2f, HIT_3f};
    soc_field_t rpe, dst_discard, vrf_id, l3mc_idx, l3mc_idx_l2, class_id, pri,
                rpa_id, expected_l3_iif, l3iif;
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
    soc_field_t vlan_id;
#endif
#ifdef BCM_MONTEREY_SUPPORT
    soc_field_t roe_decap;
#endif
    soc_field_t rpf_fail_drop, rpf_fail_tocpu;
    soc_field_t trunk_port, module_id, source_vp, port_num, trunk_id, svp_valid;
    soc_field_t flex_ctr_base_counter_idx;
    soc_field_t flex_ctr_offset_mode;
    soc_field_t flex_ctr_pool_number;
    uint8 l2only = 0;                   /* Entry is L2 only domain */
    buf_p = (uint32 *)l3x_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    l2only = (l3cfg->l3c_flags & BCM_L3_L2ONLY);

    /* Get table memory and table fields */
    if (ipv6) {
        if (soc_mem_is_valid(unit, L3_ENTRY_QUADm)) {
            mem         = L3_ENTRY_QUADm;
            if (soc_mem_field_valid(unit, L3_ENTRY_QUADm,
                                    IPV6MC__DESTINATIONf)) {
            l3mc_idx    = IPV6MC__DESTINATIONf;
        } else {
                l3mc_idx = IPV6MC__L3MC_INDEXf;
            }
            l3mc_idx_l2 = IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
        } else {
        mem         = L3_ENTRY_IPV6_MULTICASTm;
            l3mc_idx    = IPV6MC__L3MC_INDEXf;
            if (soc_mem_field_valid(unit, mem,
                                    IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf)) {
                l3mc_idx_l2 = IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
            } else {
                l3mc_idx_l2 = INVALIDf;
            }
        }
        pri         = IPV6MC__PRIf;
        rpe         = IPV6MC__RPEf;
        vrf_id      = IPV6MC__VRF_IDf;
        l3iif       = IPV6MC__L3_IIFf;
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        vlan_id     = IPV6MC__VLAN_IDf;
#endif
#ifdef BCM_MONTEREY_SUPPORT
        roe_decap   = IPV6MC__ROE_DECAPf;
#endif
        class_id    = IPV6MC__CLASS_IDf;
        dst_discard = IPV6MC__DST_DISCARDf;
        rpa_id      = IPV6MC__RPA_IDf;
        expected_l3_iif = IPV6MC__EXPECTED_L3_IIFf;
        rpf_fail_drop   = IPV6MC__IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf;
        rpf_fail_tocpu  = IPV6MC__IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf;
        idx_max     = 4;
        hit_idx_shift = 0;
        idx_offset = 0;
        trunk_port = IPV6MC__Tf;
        module_id  = IPV6MC__MODULE_IDf;
        source_vp  = IPV6MC__SOURCE_VPf;
        port_num   = IPV6MC__PORT_NUMf;
        trunk_id   = IPV6MC__TGIDf;
        svp_valid  = IPV6MC__SVP_VALIDf;
        flex_ctr_base_counter_idx = IPV6MC__FLEX_CTR_BASE_COUNTER_IDXf;
        flex_ctr_offset_mode      = IPV6MC__FLEX_CTR_OFFSET_MODEf;
        flex_ctr_pool_number      = IPV6MC__FLEX_CTR_POOL_NUMBERf;
    } else {
        if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
            mem         = L3_ENTRY_DOUBLEm;
            if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_DOUBLEm, 
									IPV4MC__DESTINATIONf)) {
            l3mc_idx    = IPV4MC__DESTINATIONf;
        } else {
                l3mc_idx = IPV4MC__L3MC_INDEXf;
            }
            l3mc_idx_l2 = IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
        } else {
        mem         = L3_ENTRY_IPV4_MULTICASTm;
            l3mc_idx    = IPV4MC__L3MC_INDEXf;
            if (soc_mem_field_valid(unit, mem,
                                    IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf)) {
                l3mc_idx_l2 = IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
            } else {
                l3mc_idx_l2 = INVALIDf;
            }
        }
        pri         = IPV4MC__PRIf;
        rpe         = IPV4MC__RPEf;
        vrf_id      = IPV4MC__VRF_IDf;
        l3iif       = IPV4MC__L3_IIFf;
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        vlan_id     = IPV4MC__VLAN_IDf;
#endif
#ifdef BCM_MONTEREY_SUPPORT
        roe_decap   = IPV4MC__ROE_DECAPf;
#endif
        class_id    = IPV4MC__CLASS_IDf;
        dst_discard = IPV4MC__DST_DISCARDf;
        rpa_id      = IPV4MC__RPA_IDf;
        expected_l3_iif = IPV4MC__EXPECTED_L3_IIFf;
        rpf_fail_drop   = IPV4MC__IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf;
        rpf_fail_tocpu  = IPV4MC__IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf;
        idx_max     = 2;
        hit_idx_shift = 1;
        idx_offset = (l3cfg->l3c_hw_index & 0x1) << 1;
        trunk_port = IPV4MC__Tf;
        module_id  = IPV4MC__MODULE_IDf;
        source_vp  = IPV4MC__SOURCE_VPf;
        port_num   = IPV4MC__PORT_NUMf;
        trunk_id   = IPV4MC__TGIDf;
        svp_valid  = IPV4MC__SVP_VALIDf;
        flex_ctr_base_counter_idx = IPV4MC__FLEX_CTR_BASE_COUNTER_IDXf;
        flex_ctr_offset_mode      = IPV4MC__FLEX_CTR_OFFSET_MODEf;
        flex_ctr_pool_number      = IPV4MC__FLEX_CTR_POOL_NUMBERf;
    }

    /* Mark entry as multicast & clear rest of the flags. */
    l3cfg->l3c_flags = BCM_L3_IPMC;
    if (ipv6) {
       l3cfg->l3c_flags |= BCM_L3_IP6;
    }
    if (l2only) {
        l3cfg->l3c_flags |= BCM_L3_L2ONLY;
    }

    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLYm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_x));

        /* Read hit value. */
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLYm,
                    &hit_x, hitf[idx]);
        }

        /* Check Source Virtual Port */
        if (soc_mem_field_valid(unit, mem, svp_valid) && 
		    soc_mem_field32_get(unit, mem, buf_p, svp_valid)) {
            l3cfg->l3c_port_tgid = soc_mem_field32_get(unit, mem, buf_p,
															 source_vp);
        } else {  /* Check Trunk Port */
            if (soc_mem_field32_get(unit, mem, buf_p, trunk_port)) {
                l3cfg->l3c_tunnel = 1;
                l3cfg->l3c_port_tgid = soc_mem_field32_get(unit, mem, buf_p, trunk_id);
                if ((SOC_MEM_FIELD_VALID(unit, mem, module_id)) &&
                   (((l3cfg->l3c_port_tgid & TH_IPMC_NO_SRC_CHECK_PORT)
                                == TH_IPMC_NO_SRC_CHECK_PORT)) &&
                        (soc_mem_field32_get(unit, mem, buf_p, module_id) == SOC_MODID_MAX(unit))) {
                    l3cfg->l3c_flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
                    l3cfg->l3c_modid = SOC_MODID_MAX(unit);
                } else {
                    l3cfg->l3c_flags |= BCM_L3_TGID;
                }
            } else {
#ifdef BCM_TRIDENT3_SUPPORT
                /* The 'if' below needs to have initialized port/trunk id
                 * for comparison
                 */
                l3cfg->l3c_port_tgid = soc_mem_field32_get(unit, mem, buf_p,
                                                           port_num);
                if (SOC_IS_TRIDENT3X(unit) &&
                    ((((l3cfg->l3c_port_tgid & TH_IPMC_NO_SRC_CHECK_PORT)
                            == TH_IPMC_NO_SRC_CHECK_PORT)) &&
                    (soc_mem_field32_get(unit, mem, buf_p, module_id)
                            == SOC_MODID_MAX(unit)))) {
                    l3cfg->l3c_flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
                    l3cfg->l3c_modid = SOC_MODID_MAX(unit);
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
                {
                    if (soc_mem_field_valid(unit, mem, module_id)) {
                    l3cfg->l3c_modid = soc_mem_field32_get(unit, mem, buf_p,
                                                           module_id);
                    }
                    l3cfg->l3c_port_tgid = soc_mem_field32_get(unit, mem, buf_p,
                                                               port_num);
                }
                l3cfg->l3c_tunnel = 0;
            }
        }
    } else if (soc_feature(unit, soc_feature_two_ingress_pipes) &&
               !SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLY_Xm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_x));

        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLY_Ym,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_y));

        /* Read hit value. */
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLY_Xm,
                                       &hit_x, hitf[idx]);
        }

        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLY_Ym,
                                       &hit_y, hitf[idx]);
        }
    } else {
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLYm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_default));

        /* Read hit value. */
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLYm,
                                       &hit_default, hitf[idx]);
        }
    }

    if (hit) {
        l3cfg->l3c_flags |= BCM_L3_HIT;
    }

    /* Set ipv6 group address to multicast. */
    if (ipv6) {
        l3cfg->l3c_ip6[0] = 0xff;   /* Set entry ip to mcast address. */
    }
    if (l2only) {
        /* Vlan/VFI is key, read L3iif */
        l3cfg->l3c_ing_intf = soc_mem_field32_get(unit, mem, buf_p, l3iif);
    } else {
        /* L3IIF is key, read vid */
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, vlan_id) &&
            SOC_MEM_FIELD_VALID(unit, mem, l3iif) &&
            (soc_mem_field_length(unit, mem, vlan_id) >
             soc_mem_field_length(unit, mem, l3iif))) {
            l3cfg->l3c_vid = soc_mem_field32_get(unit, mem, buf_p, vlan_id);
        } else
#endif
        {
            l3cfg->l3c_vid = soc_mem_field32_get(unit, mem, buf_p, l3iif);
        }
    }
    /* Read priority override */
    if(soc_mem_field32_get(unit, mem, buf_p, rpe)) { 
        l3cfg->l3c_flags |= BCM_L3_RPE;
    }

    /* Read destination discard bit. */
    if(soc_mem_field32_get(unit, mem, buf_p, dst_discard)) { 
        l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
    }

    /* Read Virtual Router Id. */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, mem, buf_p, vrf_id);

    /* Pointer to ipmc replication table. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
        l3cfg->l3c_ipmc_ptr = soc_mem_field32_dest_get(unit, mem, buf_p,
                                l3mc_idx, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_IPMC) {
            return BCM_E_FAIL;
        }
    } else {
    l3cfg->l3c_ipmc_ptr = soc_mem_field32_get(unit, mem, buf_p, l3mc_idx);
    }

    if (l3mc_idx_l2 != INVALIDf && soc_mem_field_valid(unit, mem, l3mc_idx_l2)) {
        l3cfg->l3c_ipmc_ptr_l2 = soc_mem_field32_get(unit, mem, buf_p, l3mc_idx_l2);
    }

    /* Classification lookup class id. */
    l3cfg->l3c_lookup_class = soc_mem_field32_get(unit, mem, buf_p, class_id);

    /* Read priority value. */
    l3cfg->l3c_prio = soc_mem_field32_get(unit, mem, buf_p, pri);

    /* Read RPA_ID value. */
    if (soc_mem_field_valid(unit, mem, rpa_id)) {
    l3cfg->l3c_rp_id = soc_mem_field32_get(unit, mem, buf_p, rpa_id);
    } else {
        l3cfg->l3c_rp_id = 0;
    }

    if (l3cfg->l3c_rp_id == 0 &&
        (l3cfg->l3c_vid != 0 ||
         soc_mem_field32_get(unit, mem, buf_p, expected_l3_iif) != 0)) {
        /* If RPA_ID field value is 0, it could mean (1) this IPMC entry is
         * not a PIM-BIDIR entry, or (2) this entry is a PIM-BIDIR entry and
         * is associated with rendezvous point 0. If either the L3 IIF in the
         * key or the expected L3 IIF in the data is not zero, this
         * entry is not a PIM-BIDIR entry.
         */ 
        l3cfg->l3c_rp_id = BCM_IPMC_RP_ID_INVALID;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && l3cfg->l3c_rp_id == 0) {
        l3cfg->l3c_rp_id = BCM_IPMC_RP_ID_INVALID;
    }
#endif

    /* Read expected iif */
    l3cfg->l3c_intf = soc_mem_field32_get(unit, mem, buf_p, expected_l3_iif);
    if (0 != l3cfg->l3c_intf) {
        l3cfg->l3c_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
    }
    if (soc_mem_field32_get(unit, mem, buf_p, rpf_fail_drop)) {
        l3cfg->l3c_flags |= BCM_IPMC_RPF_FAIL_DROP;
    }
    if (soc_mem_field32_get(unit, mem, buf_p, rpf_fail_tocpu)) {
        l3cfg->l3c_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
    }

    
    if (!SOC_IS_MONTEREY(unit)) {
    /* Read base counter index. */
    l3cfg->l3c_flex_ctr_base_id =
        soc_mem_field32_get(unit, mem, buf_p, flex_ctr_base_counter_idx);
    /* Read counter offset mode. */
    l3cfg->l3c_flex_ctr_mode =
        soc_mem_field32_get(unit, mem, buf_p, flex_ctr_offset_mode);
    /* Read counter pool number. */
    l3cfg->l3c_flex_ctr_pool =
        soc_mem_field32_get(unit, mem, buf_p, flex_ctr_pool_number);
    }

#ifdef BCM_MONTEREY_SUPPORT
    if (soc_feature(unit, soc_feature_roe) ||
        soc_feature(unit, soc_feature_roe_custom)) {
        if (SOC_MEM_FIELD_VALID(unit, mem, roe_decap)) {
            if (soc_mem_field32_get(unit, mem, buf_p, roe_decap)) {
                l3cfg->l3c_flags2 |= BCM_L3_FLAGS2_DECAP_ROE_HEADER;
            }
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given L3 Host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) GPORT ID
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t
_bcm_td2_l3_host_stat_get_table_info(
                   int                        unit,
                   bcm_l3_host_t              *info,
                   uint32                     *num_of_tables,
                   bcm_stat_flex_table_info_t *table_info)
{
    int rv, embd;
    soc_mem_t mem;          
    _bcm_l3_cfg_t l3cfg;     

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    if (!(soc_feature(unit, soc_feature_l3_extended_host_entry))) {
        return BCM_E_UNAVAIL;
    }

    mem = INVALIDm;
    /* Vrf is in device supported range check. */
    if ((info->l3a_vrf > SOC_VRF_MAX(unit)) || 
        (info->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_flags = info->l3a_flags;
    l3cfg.l3c_vrf = info->l3a_vrf;

    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(l3cfg.l3c_ip6, info->l3a_ip6_addr, BCM_IP6_ADDRLEN);
    } else {
        l3cfg.l3c_ip_addr = info->l3a_ip_addr;
    }
   
    rv = _bcm_td2_l3_entry_get(unit, &l3cfg, NULL, &embd);
    if (BCM_SUCCESS(rv)) { /* Entry found */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            mem = (l3cfg.l3c_flags & BCM_L3_IP6) ? L3_ENTRY_QUADm :
                                               L3_ENTRY_DOUBLEm;
        } else 
#endif
        {
            if (embd != TRUE) {
                return (BCM_E_PARAM);
            }
        mem = (l3cfg.l3c_flags & BCM_L3_IP6) ? L3_ENTRY_IPV6_MULTICASTm :
                                               L3_ENTRY_IPV4_MULTICASTm;       
        }

        table_info[*num_of_tables].table = mem;
        table_info[*num_of_tables].index = l3cfg.l3c_hw_index;
        table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }
    
    return rv;
}

/*----- END STATIC FUNCS ----- */

/*
 * Function:
 *      _bcm_td2_l3_add
 * Purpose:
 *      Add and entry to TD2 L3 host table.
 *      The decision to add to is based on 
 *      switch control(s) and table full conditions.
 *      conditions.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{
    int rv;

    rv = _bcm_td2_l3_entry_add(unit, l3cfg, nh_idx);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_get
 * Purpose:
 *      Get an entry from TD2 L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    int rv;
    int embd = _BCM_TD2_HOST_ENTRY_NOT_FOUND;

    rv = _bcm_td2_l3_entry_get(unit, l3cfg, nh_idx, &embd);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_del
 * Purpose:
 *      Del an entry from TD2 L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int rv;

    /* Attempt to delete entry */
    rv = _bcm_td2_l3_entry_del(unit, l3cfg);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_traverse
 * Purpose:
 *      Walk over range of entries in l3 host table.
 * Parameters:
 *      unit - SOC unit number.
 *      flags - L3 entry flags to match during traverse.  
 *      start - First index to read. 
 *      end - Last index to read.
 *      cb  - Caller notification callback.
 *      user_data - User cookie, which should be returned in cb. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_traverse(int unit, int flags, uint32 start, uint32 end,
                      bcm_l3_host_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    int ipv6;                   /* Protocol IPv6/IPv4          */
    int nh_idx;                 /* Next hop index.                 */
    int total = 0;              /* Total number of entries walked. */
    int table_size;             /* Number of entries in the table. */
    int table_ent_size;         /* Table entry size. */
    bcm_l3_host_t info;         /* Callback info buffer.           */
    _bcm_l3_cfg_t l3cfg;        /* HW entry info.                  */
    char *l3_tbl_ptr = NULL;    /* Table dma pointer.              */
    int tbl, idx, idx_min, idx_max;  /* Min and Max iteration index */
    int num_tables = 2;
    soc_mem_t mem[2] = {INVALIDm, INVALIDm};
    int show_idx_flag;

    /* If no callback provided, we are done.  */
    if (NULL == cb) {
        return (BCM_E_NONE);
    }

    ipv6 = (flags & BCM_L3_IP6) ? TRUE : FALSE;
    show_idx_flag = (flags & _BCM_L3_SHOW_INDEX) ? TRUE : FALSE;
    /* If table is empty -> there is nothing to read. */
    if (ipv6 && (!BCM_XGS3_L3_IP6_CNT(unit))) {
        return (BCM_E_NONE);
    }
    if (!ipv6 && (!BCM_XGS3_L3_IP4_CNT(unit))) {
        return (BCM_E_NONE);
    }

    if (ipv6) {
        mem[0] = BCM_XGS3_L3_MEM(unit, v6); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v6_4); 
        }
    } else {
        mem[0] = BCM_XGS3_L3_MEM(unit, v4); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v4_2); 
        }
    }

    for(tbl = 0; tbl < num_tables; tbl++) {
  
        if (INVALIDm == mem[tbl]) {
            continue;
        }
        idx_max = soc_mem_index_max(unit, mem[tbl]);
        idx_min =  soc_mem_index_min(unit, mem[tbl]);
        table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem[tbl]);

        BCM_IF_ERROR_RETURN (bcm_xgs3_l3_tbl_dma(unit, mem[tbl], table_ent_size, 
                                      "l3_tbl", &l3_tbl_ptr, &table_size));

        /* Input index`s sanity. */
        if (start > (uint32)table_size || start > end) {
            if(l3_tbl_ptr) {
                soc_cm_sfree(unit, l3_tbl_ptr);
            }
            return (BCM_E_NOT_FOUND);
        }

        /* Iterate over all the entries - show matching ones. */
        for (idx = idx_min; idx <= idx_max; idx++) {
            /* Reset buffer before read. */
            sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
            l3cfg.l3c_flags = flags;

            /* Get entry from  hw. */
            rv = _bcm_td2_l3_get_host_ent_by_idx(unit, l3_tbl_ptr, mem[tbl],
                                                  idx, &l3cfg, &nh_idx);

            /* Check for read errors & invalid entries. */
            if (rv < 0) {
                if (rv != BCM_E_NOT_FOUND) {
                    break;
                }
                continue;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flow)
                && (l3cfg.l3c_flow_handle)
                && (l3cfg.l3c_flags & BCM_L3_IPMC))  {
                if (!IPMC_USED_ISSET(unit, l3cfg.l3c_ipmc_ptr)) {
                   LOG_ERROR(BSL_LS_BCM_L3,
                       (BSL_META_U(unit,
                   "L3 host: Invalid IPMC group %d " \
                   "in L3 host entry index %d\n"),
                           l3cfg.l3c_ipmc_ptr, idx));
                    continue;
                }

                if (BCM_L3_CMP_EQUAL !=
                    _bcm_td3_l3_flex_entry_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                        continue;
                }

                /* Valid entry found -> increment total table_sizeer */
                total++;
                if ((uint32)total < start) {
                    continue;
                }

                /* Don't read beyond last required index. */
                if ((uint32)total > end) {
                    break;
                }

            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                /* Check read entry flags & update index accordingly. */
                if (BCM_L3_CMP_EQUAL !=
                    _bcm_xgs3_trvrs_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                    continue;
                }

                /* Valid entry found -> increment total table_sizeer */
                total++;
                if ((uint32)total < start) {
                    continue;
                }

                /* Don't read beyond last required index. */
                if ((uint32)total > end) {
                    break;
                }

                /* Get next hop info. */
                rv = _bcm_xgs3_l3_get_nh_info(unit, &l3cfg, nh_idx);
                if (rv < 0) {
                    continue;
                }
            }

            /* Fill host info. */
            _bcm_xgs3_host_ent_init(unit, &l3cfg, TRUE, &info);
            if (show_idx_flag && user_data) {
                *(int *)user_data = idx;
            }

            if (cb) {
                rv = (*cb) (unit, total, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    break;
                }
#endif
            }
        }

        if (l3_tbl_ptr) {
            soc_cm_sfree(unit, l3_tbl_ptr);
        }
    }

    /* Reset last read status. */
    if (BCM_E_NOT_FOUND == rv) {
        rv = BCM_E_NONE;
    }

    return (rv);

}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_attach
 * Description:
 *      Attach counters entries to the given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description  
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      On BCM56640, the L3 host tables (tables L3_ENTRY_2 and L3_ENTRY_4, 
 *      table EXT_L3_UCAST_DATA_WIDE) may contain a flex stat pointer when 
 *      entry has a dereference next hop (no NEXT_HOP pointer, but the NH info 
 *      instead).  This counter is analogous to what would be in the egress 
 *      object, but is implemented in the ingress and so requires an ingress 
 *      counter pool allocated.
 */
bcm_error_t
_bcm_td2_l3_host_stat_attach(
            int unit,
            bcm_l3_host_t *info,
            uint32 stat_counter_id)
{
    soc_mem_t                 table[4]={0};
    uint32                    flex_table_index=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,4,&actual_num_tables,&table[0],&direction));

    BCM_IF_ERROR_RETURN(_bcm_td2_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));
    /* TD3 doesn't support Flex Counter for L3_ENTRY_DOUBLE view */
    if (SOC_IS_TRIDENT3(unit) &&
        SOC_URPF_STATUS_GET(unit) &&
        table_info[0].table == L3_ENTRY_DOUBLEm) {
        return BCM_E_UNAVAIL;
    }

    for (count=0; count < num_of_tables ; count++) {
         for (flex_table_index=0; 
              flex_table_index < actual_num_tables ; 
              flex_table_index++) {
              if ((table_info[count].direction == direction) &&
                  (table_info[count].table == table[flex_table_index]) ) {
                  if (direction == bcmStatFlexDirectionIngress) {
                      return _bcm_esw_stat_flex_attach_ingress_table_counters(
                             unit,
                             table_info[count].table,
                             table_info[count].index,
                             offset_mode,
                             base_index,
                             pool_number);
                  } else {
                      return _bcm_esw_stat_flex_attach_egress_table_counters(
                             unit,
                             table_info[count].table,
                             table_info[count].index,
                             0,
                             offset_mode,
                             base_index,
                             pool_number);
                  }
              }
         }
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_detach
 * Description:
 *      Detach counters entries to the given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info		 - (IN) L3 host description
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
_bcm_td2_l3_host_stat_detach(
            int unit,
            bcm_l3_host_t *info)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_td2_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index);
           if (BCM_E_NONE != rv && 
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                          unit,
                          0,
                          table_info[count].table,
                          table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_counter_get
 * Description:
 *      Get counter statistic values for specific L3 host entry
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_td2_l3_host_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }
    if (stat == bcmL3StatInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
               /*ctr_offset_info.offset_index = counter_indexes[index_count];*/
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_counter_set
 * Description:
 *      Set counter statistic values for specific L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_td2_l3_host_stat_counter_set(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }
    if (stat == bcmL3StatInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                unit,
                                table_info[table_count].index,
                                table_info[table_count].table,
                                0,
                                byte_flag,
                                counter_indexes[index_count],
                                &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_host_stat_id_get
 * Description:
 *      Get stat counter id associated with given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_td2_l3_host_stat_id_get(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_l3_host_stat_get_table_info(
                        unit,info,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcm_td2_l3_replace
 * Purpose:
 *      Replace an entry in TD2 L3 host table.
 * Parameters:
 *      unit -  (IN)SOC unit number.
 *      l3cfg - (IN)L3 entry information.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_l3_replace(int unit, _bcm_l3_cfg_t *l3cfg)
{
    
    _bcm_l3_cfg_t entry;        /* Original l3 entry.             */
    int nh_idx_old;             /* Original entry next hop index. */
    int nh_idx_new;             /* New allocated next hop index   */
    int rv = BCM_E_UNAVAIL;     /* Operation return status.       */
    int embd_old = 0, embd_new = 0;
    int mpath = 0;              /* Multipath */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if (NULL == l3cfg) {
        return (BCM_E_PARAM); 
    }

    /* Set lookup key. */
    entry = *l3cfg;

    if (BCM_XGS3_L3_MCAST_ENTRY(l3cfg)) {
        /* Check if identical entry exits. */
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, ipmc_get)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, ipmc_get) (unit, &entry);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        }
        if (rv < 0) {
            return rv;
        }

        /* Write mcast entry to hw. */
        l3cfg->l3c_hw_index = entry.l3c_hw_index;       /*Replacement index & flag. */
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, ipmc_add)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, ipmc_add) (unit, l3cfg);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else {
        if (!(BCM_XGS3_L3_EGRESS_MODE_ISSET(unit))) {
            /* Trunk id validation. */
            BCM_XGS3_L3_IF_INVALID_TGID_RETURN(unit, l3cfg->l3c_flags,
                                               l3cfg->l3c_port_tgid);
        }
        mpath = l3cfg->l3c_flags & BCM_L3_MULTIPATH; 
        if (!mpath &&
            BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, l3cfg->l3c_intf)) {
            return BCM_E_PARAM;
        }

        if (mpath &&
            !BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, l3cfg->l3c_intf)) {
            return BCM_E_PARAM;
        }

        /* Check if identical entry exits. */
        rv = _bcm_td2_l3_entry_get(unit, &entry, &nh_idx_old, &embd_old);
        if (((BCM_E_NOT_FOUND == rv) || (BCM_E_DISABLED == rv)) &&
            !soc_property_get(unit, spn_HOST_AS_ROUTE_DISABLE, 0)) {
            return  bcm_xgs3_host_as_route(unit, l3cfg, BCM_XGS3_L3_OP_ADD, rv);
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
        l3cfg->l3c_bfd_enable = entry.l3c_bfd_enable;

        if (BCM_GPORT_IS_BLACK_HOLE(l3cfg->l3c_port_tgid)) {
            nh_idx_new = 0;
        } else { /* If new entry is in indexed NH table */
            /* Get next hop index. */
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_nh_init_add(unit, l3cfg, NULL, &nh_idx_new));
        }

#ifdef BCM_TOMAHAWK_SUPPORT
        if (soc_feature(unit, soc_feature_extended_view_no_trunk_support) &&
            BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx_new) &&
            (l3cfg->l3c_flags & BCM_L3_TGID)) {
            return BCM_E_UNAVAIL;
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) &&
            soc_feature(unit, soc_feature_ecmp_resilient_hash) &&
            mpath && (entry.l3c_flags & BCM_L3_MULTIPATH)) {
            /* If an old ECMP group is replaced by a new one, and if
             * they have members in common, the resilient hash flow set
             * entries containing the shared members are copied from the old
             * ECMP group to the new ECMP group, in order to minimize
             * flow-to-member reassignments.
             */
            BCM_IF_ERROR_RETURN(bcm_td2_l3_egress_ecmp_rh_shared_copy(unit,
                                nh_idx_old, nh_idx_new));
        }
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) &&
            soc_feature(unit, soc_feature_ecmp_resilient_hash_optimized) &&
            mpath && (entry.l3c_flags & BCM_L3_MULTIPATH)) {
                BCM_IF_ERROR_RETURN(bcm_opt_l3_egress_ecmp_rh_shared_copy(unit,
                                    nh_idx_old, nh_idx_new));
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_TRIDENT2_SUPPORT */

        embd_new = BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, 
                                                    nh_idx_new);
        /* Write entry to hw. */
        if (embd_old != embd_new) {
            rv = bcm_xgs3_l3_del(unit, &entry);
            l3cfg->l3c_hw_index = BCM_XGS3_L3_INVALID_INDEX;
        } else {
            /*Replacement index & flag. */
            l3cfg->l3c_hw_index = entry.l3c_hw_index; 
        }
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, l3_add)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, l3_add)(unit, l3cfg, nh_idx_new);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        } else {
            rv = BCM_E_UNAVAIL;
        }

        /* Do best effort clean_up if needed. */
        if (BCM_FAILURE(rv)) {
            bcm_xgs3_nh_del(unit, 0, nh_idx_new);
        }

        if (embd_old == embd_new) {
            /* Free original next hop index. */
            if (entry.l3c_flags & BCM_L3_MULTIPATH) {
                /* Destroy ecmp group only if previous route is multipath */
                BCM_IF_ERROR_RETURN(
                    bcm_xgs3_ecmp_group_del(unit, nh_idx_old, 0));
            } else {
                BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx_old));
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_l3_del_intf
 * Purpose:
 *      Delete all the entries in TD2 L3 host table with
 *      NH matching a certain interface.
 * Parameters:
 *      unit   - (IN)SOC unit number.
 *      l3cfg  - (IN)Pointer to memory for l3 table related information.
 *      negate - (IN)0 means interface match; 1 means not match
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_l3_del_intf(int unit, _bcm_l3_cfg_t *l3cfg, int negate)
{
    int rv;                             /* Operation return status.      */
    int tmp_rv;                         /* First error occured.          */
    bcm_if_t intf;                      /* Deleted interface id.         */
    int nh_index;                       /* Next hop index.               */
    bcm_l3_egress_t egr;                /* Egress object.                */
    _bcm_if_del_pattern_t pattern;      /* Interface & negate combination 
                                           to be used for every l3 entry
                                           comparision.                  */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check */
    if (NULL == l3cfg) {
        return (BCM_E_PARAM);
    }

    intf = l3cfg->l3c_intf;

    /* Check l3 switching mode. */ 
    if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) || 
            BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
                nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit); 
            } else {
                nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
            /* Get egress object information. */
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_get(unit, nh_index, &egr));
            /* Use egress object interface for iteration */
            intf = egr.intf;
        }
    }

    pattern.l3_intf = intf;
    pattern.negate = negate;

    /* Delete all ipv4 entries matching the interface. */
    tmp_rv = _bcm_td2_l3_del_match(unit, 0, (void *)&pattern,
                                    _bcm_xgs3_l3_intf_cmp, NULL, NULL);

    /* Delete all ipv6 entries matching the interface. */
    rv = _bcm_td2_l3_del_match(unit, BCM_L3_IP6, (void *)&pattern,
                                _bcm_xgs3_l3_intf_cmp, NULL, NULL);

    return (tmp_rv < 0) ? tmp_rv : rv;
}

/*
 * Function:
 *      _bcm_td2_l3_get_by_idx
 * Purpose:
 *      Get an entry from L3 table by index.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      idx      - (IN)Index to read.
 *      l3cfg    - (OUT)l3 entry search result.
 *      nh_index - (OUT)Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_get_by_idx(int unit, void *dma_ptr, int idx,
                        _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    int v6;
    int embeded_nh;
    soc_mem_t mem;
    int rv = BCM_E_UNAVAIL;     /* Operation return code. */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    if (l3cfg->l3c_flags & BCM_L3_IPMC) {
        /* Get multicast entry. */
        rv = _bcm_td2_l3_ipmc_get_by_idx(unit, dma_ptr, idx, l3cfg);
    } else {
        /* Get unicast entry. */
        v6 = (l3cfg->l3c_flags & BCM_L3_IP6); 
        embeded_nh = (l3cfg->l3c_flags & BCM_L3_DEREFERENCED_NEXTHOP); 
        mem = (v6) ? 
              ((embeded_nh) ? 
              BCM_XGS3_L3_MEM(unit, v6_4) : BCM_XGS3_L3_MEM(unit, v6)) :
              ((embeded_nh) ?  
              BCM_XGS3_L3_MEM(unit, v4_2) : BCM_XGS3_L3_MEM(unit, v4)); 
        rv = _bcm_td2_l3_get_host_ent_by_idx(unit, dma_ptr, mem,
                                             idx, l3cfg, nh_idx);
    }
    return rv;
}


/*
 * Function:
 *      _bcm_td2_l3_del_match
 * Purpose:
 *      Delete an entry in L3 table matches a certain rule.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      flags.    - (IN)Generic l3 flags. 
 *      pattern   - (IN)Comparison match argurment.
 *      cmp_func  - (IN)Comparison function.
 *      notify_cb - (IN)Delete notification callback. 
 *      user_data - (IN)User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_del_match(int unit, int flags, void *pattern,
                       bcm_xgs3_ent_op_cb cmp_func,
                       bcm_l3_host_traverse_cb notify_cb, void *user_data)
{
    int rv = BCM_E_NONE;
    int ipv6;             /* IPv6/IPv4 lookup flag. */
    int nh_idx;           /* Next hop index.        */
    int tbl, idx;         /* Iteration indices.     */
    int cmp_result;       /* Compare against pattern result. */
    bcm_l3_host_t info;   /* Host cb buffer.        */
    _bcm_l3_cfg_t l3cfg;  /* Hw entry info.         */
    int idx_max;
    int num_tables = 2;  
    soc_mem_t mem[2] = {INVALIDm, INVALIDm};
    int table_ent_size;         /* Table entry size. */
    int table_size;             /* Number of entries in the table. */
    char *l3_tbl_ptr = NULL;    /* Table dma pointer.              */

    /* Check Protocol. */
    ipv6 = (flags & BCM_L3_IP6) ? TRUE : FALSE;

    if (ipv6) {
        mem[0] = BCM_XGS3_L3_MEM(unit, v6); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v6_4); 
        }
    } else {
        mem[0] = BCM_XGS3_L3_MEM(unit, v4); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v4_2); 
        }
    }

    for (tbl = 0; tbl < num_tables; tbl++) {
        if (INVALIDm == mem[tbl]) {
            continue;
        }
        idx_max = soc_mem_index_max(unit, mem[tbl]);
        table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem[tbl]);
        BCM_IF_ERROR_RETURN(
            bcm_xgs3_l3_tbl_dma(unit, mem[tbl], table_ent_size,
                                "l3_tbl", &l3_tbl_ptr, &table_size));

        for (idx = 0; idx <= idx_max; idx++) {

            /* Set protocol. */
            sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
            l3cfg.l3c_flags = flags;
            rv = _bcm_td2_l3_get_host_ent_by_idx(unit, l3_tbl_ptr, mem[tbl], idx,
                                                 &l3cfg, &nh_idx);
            /* Check for read errors & invalid entries. */
            if (rv < 0) {
                if (rv != BCM_E_NOT_FOUND) {
                    break;
                }
                /* If not found, reset the return value */
                rv = BCM_E_NONE;
                continue;
            }

 #if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flow)
                && (l3cfg.l3c_flow_handle)
                && (l3cfg.l3c_flags & BCM_L3_IPMC))  {
                   LOG_VERBOSE(BSL_LS_BCM_L3,
                       (BSL_META_U(unit,
                   "L3 host: IPMC do not compare the l3 entry flags.")));
                    nh_idx = BCM_XGS3_L3_INVALID_INDEX;
                    if (BCM_L3_CMP_EQUAL !=
                    _bcm_td3_l3_flex_entry_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                        continue;
                    }
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {

                /* Check read entry flags & update index accordingly. */
                if (BCM_L3_CMP_EQUAL !=
                    _bcm_xgs3_trvrs_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                    continue;
                }
            }

            /* If compare function provided match the entry against pattern */
            if (cmp_func) {
                rv = (*cmp_func) (unit, pattern, (void *)&l3cfg,
                                  (void *)&nh_idx, &cmp_result);
                if (BCM_FAILURE(rv)) {
                    break;
                }
                /* If entry doesn't match the pattern don't delete it. */
                if (BCM_L3_CMP_EQUAL != cmp_result) {
                    continue;
                }
            }

            /* Entry matches the rule -> delete it. */
            rv = _bcm_td2_l3_entry_del(unit, &l3cfg);
            if (BCM_FAILURE(rv)) {
                break;
            }
    
            if (l3cfg.l3c_flags & BCM_L3_MULTIPATH) {
                /* Update ECMP reference count */
                if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
                    rv = bcm_xgs3_ecmp_group_del(unit, nh_idx, 0);
                }
            } else {
                /* Release next hop index. */
                rv = bcm_xgs3_nh_del(unit, 0, nh_idx);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
   
            /* Check if notification required. */
            if (notify_cb) {
                /* Fill host info. */
                _bcm_xgs3_host_ent_init(unit, &l3cfg, FALSE, &info);
                /* Call notification callback. */
                (*notify_cb) (unit, idx, &info, user_data);
            }
        }

        if (l3_tbl_ptr) {
            soc_cm_sfree(unit, l3_tbl_ptr);
            l3_tbl_ptr = NULL;
        }
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    /* Reset last read status. */
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_get
 * Purpose:
 *      Get TD2 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN/OUT)Group/Source key & Get result buffer.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg)
{
    uint32 l3x_key[SOC_MAX_MEM_FIELD_WORDS];    /* Lookup key buffer.    */
    uint32 l3x_entry[SOC_MAX_MEM_FIELD_WORDS];  /* Search result buffer. */
    int clear_hit;                              /* Clear hit flag.       */
    soc_mem_t mem;                              /* IPMC table memory.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int rv;                                     /* Return value.         */
    int l3_entry_idx;                           /* Entry index           */

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {
        int nh_idx = 0;
        int embd = 0;

        return _bcm_td3_l3_flex_entry_get(unit, l3cfg, &nh_idx, &embd);
    }
#endif

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
        mem = (ipv6) ? L3_ENTRY_QUADm : L3_ENTRY_DOUBLEm;
    } else {
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;
    }

    /*  Zero buffers. */
    sal_memcpy(l3x_key, soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit,mem) * 4);
    sal_memcpy(l3x_entry, soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit,mem) * 4);

    /* Check if clear hit bit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Lookup Key preparation. */
    rv = _bcm_td2_l3_ipmc_ent_init(unit, (uint32 *)l3x_key, l3cfg);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Perform hw lookup. */
    MEM_LOCK(unit, mem);
    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                _BCM_TD2_L3_MEM_BANKS_ALL,
                                l3x_key, l3x_entry, &l3_entry_idx);
    l3cfg->l3c_hw_index = l3_entry_idx;

    MEM_UNLOCK(unit, mem);
    BCM_XGS3_LKUP_IF_ERROR_RETURN(rv, BCM_E_NOT_FOUND);

    /* Extract buffer information. */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_ipmc_ent_parse(unit, l3cfg, (void *)l3x_entry));

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_clear_hit(unit, mem, l3cfg, l3x_entry,
                                                  l3_entry_idx));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_get_by_idx
 * Purpose:
 *      Get l3 multicast entry by entry index.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      idx       - (IN)Index to read. 
 *      l3cfg     - (IN/OUT)Entry data.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_l3_ipmc_get_by_idx(int unit, void *dma_ptr,
                           int idx, _bcm_l3_cfg_t *l3cfg)
{
    uint32 l3_entry_v6[SOC_MAX_MEM_FIELD_WORDS];    /* Read buffer.            */
    uint32 l3_entry_v4[SOC_MAX_MEM_FIELD_WORDS];    /* Read buffer.            */
    void *buf_p;                      /* Hardware buffer ptr     */
    soc_mem_t mem;                     /* IPMC table memory.      */
    uint32 ipv6;                       /* IPv6 entry indicator.   */
    int clear_hit;                     /* Clear hit bit indicator.*/
    int key_type;
    soc_field_t l3iif;
    uint32 base_valid0, base_valid1, base_valid2, base_valid3;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    if (ipv6) {
        mem   = L3_ENTRY_IPV6_MULTICASTm;
        buf_p = (l3_entry_ipv6_multicast_entry_t *)l3_entry_v6;
        l3iif = IPV6MC__L3_IIFf;
    } else {
        mem   = L3_ENTRY_IPV4_MULTICASTm;
        buf_p = (l3_entry_ipv4_multicast_entry_t *)l3_entry_v4;
        l3iif = IPV4MC__L3_IIFf;
    }
    if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
        mem =  (ipv6) ? L3_ENTRY_QUADm : L3_ENTRY_DOUBLEm;
        if (ipv6) {
            buf_p = (l3_entry_quad_entry_t *)l3_entry_v6;
        } else {
            buf_p = (l3_entry_double_entry_t *)l3_entry_v4;
        }
    }

    /* Check if clear hit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    if (NULL == dma_ptr) {             /* Read from hardware. */
        /* Zero buffers. */
        sal_memcpy(buf_p, soc_mem_entry_null(unit, mem), 
                     soc_mem_entry_words(unit,mem) * 4);

        /* Read entry from hw. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, buf_p));
    } else {                    /* Read from dma. */
        if (ipv6) {
            if (soc_mem_is_valid(unit, L3_ENTRY_QUADm)) {
                buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                            l3_entry_quad_entry_t *, dma_ptr, idx);
            } else {
            buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                     l3_entry_ipv6_multicast_entry_t *, dma_ptr, idx);
            }
        } else {
            if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
                buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                            l3_entry_double_entry_t *, dma_ptr, idx);
        } else {
            buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                     l3_entry_ipv4_multicast_entry_t *, dma_ptr, idx);
        }
    }
    }

    /* Ignore invalid entries. */
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (ipv6) {
           if (SOC_IS_TOMAHAWK3(unit))
           {
                base_valid0 = 1;
                base_valid1 = 2;
                base_valid2 = 2;
                base_valid3 = 7;
            }
            else
            {
                base_valid0 = 5;
                base_valid1 = 6;
                base_valid2 = 6;
                base_valid3 = 7;
            }

            if (soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_0f) != base_valid0 ||
                soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_1f) != base_valid1 ||
                soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_2f) != base_valid2 ||
                soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_3f) != base_valid3) {
                return (BCM_E_NOT_FOUND);
            }
        } else {
            if (SOC_IS_TOMAHAWK3(unit))
            {
                base_valid0 = 1;
                base_valid1 = 7;
            }
            else
            {
                base_valid0 = 3;
                base_valid1 = 4;
            }
            if (soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_0f) != base_valid0 ||
                soc_mem_field32_get(unit, mem, buf_p, BASE_VALID_1f) != base_valid1) {
                return (BCM_E_NOT_FOUND);
            }
        }
        key_type = soc_mem_field32_get(unit, mem, buf_p, KEY_TYPEf);
    } else {
    if (!soc_mem_field32_get(unit, mem, buf_p, VALID_0f)) {
        return (BCM_E_NOT_FOUND);
    }

    key_type = soc_mem_field32_get(unit, mem, buf_p, KEY_TYPE_0f);
    }

    switch (key_type) {
      case TD2_L3_HASH_KEY_TYPE_V4L2MC:
      case TD2_L3_HASH_KEY_TYPE_V4L2VPMC:
          l3cfg->l3c_flags = BCM_L3_IPMC | BCM_L3_L2ONLY;
          break;
      case TD2_L3_HASH_KEY_TYPE_V4MC:
          l3cfg->l3c_flags = BCM_L3_IPMC;
          break;
      case TD2_L3_HASH_KEY_TYPE_V6L2MC:
      case TD2_L3_HASH_KEY_TYPE_V6L2VPMC:
          l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC | BCM_L3_L2ONLY;
          break;
      case TD2_L3_HASH_KEY_TYPE_V6MC:
          l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
          break;
      default:
          /* Catch all other key types, set flags to zero so that the 
           * following flag check is effective for all key types */
          l3cfg->l3c_flags = 0;  
          break;
    }

    /* Ignore protocol mismatch & multicast entries. */
    if ((ipv6  != (l3cfg->l3c_flags & BCM_L3_IP6)) ||
        (!(l3cfg->l3c_flags & BCM_L3_IPMC))) {
        return (BCM_E_NOT_FOUND); 
    }

    /* Set index to l3cfg. */
    l3cfg->l3c_hw_index = idx;

    if (ipv6) {
        /* Get group address. */
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);

        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_UPR_56f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);

        /* Get source  address. */
        if (soc_mem_field_valid(unit, mem, IPV6MC__SOURCE_IP_ADDR_BITS_63_32f)) {
            soc_mem_ip6_addr_get(unit, mem, buf_p,
                IPV6MC__SOURCE_IP_ADDR_BITS_63_32f, l3cfg->l3c_sip6,
                SOC_MEM_IP6_BITS_63_32);
            soc_mem_ip6_addr_get(unit, mem, buf_p,
                IPV6MC__SOURCE_IP_ADDR_BITS_31_0f, l3cfg->l3c_sip6,
                SOC_MEM_IP6_BITS_31_0);
        } else {
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        }
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        l3cfg->l3c_ip6[0] = 0xff;    /* Set entry to multicast*/
    } else {
        /* Get group id. */
        l3cfg->l3c_ip_addr =
            soc_mem_field32_get(unit, mem, buf_p, IPV4MC__GROUP_IP_ADDRf);

        /* Get source address. */
        l3cfg->l3c_src_ip_addr =
            soc_mem_field32_get(unit, mem,  buf_p, IPV4MC__SOURCE_IP_ADDRf);
    }

    /* Get L3_IIF or vlan id. */
    l3cfg->l3c_vid = soc_mem_field32_get(unit, mem, buf_p, l3iif);

    /* Parse entry data. */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_ipmc_ent_parse(unit, l3cfg, buf_p));

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_clear_hit(unit, mem, l3cfg, buf_p,
                                                     l3cfg->l3c_hw_index));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_add
 * Purpose:
 *      Add TD2 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN/OUT)Group/Source key.
 * Returns:
 *    BCM_E_XXX
 */
int
 _bcm_td2_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg)
{
    uint32 l3x_entry[SOC_MAX_MEM_FIELD_WORDS];  /* Write entry buffer.   */
    soc_mem_t mem;                              /* IPMC table memory.    */
    uint32 *buf_p;                              /* HW buffer address.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int idx;                                    /* Iteration index.      */
    int idx_max;                                /* Iteration index max.  */
    int rv;                                     /* Return value.         */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f};
    soc_field_t rpe, dst_discard, vrf_id, l3mc_idx, l3mc_idx_l2, class_id, pri, rpa_id;
    soc_field_t exp_iif, exp_iif_drop, exp_iif_copytocpu;
    soc_field_t flex_ctr_base_counter_idx;
    soc_field_t flex_ctr_offset_mode;
    soc_field_t flex_ctr_pool_number;
#ifdef BCM_MONTEREY_SUPPORT
    soc_field_t roe_decap;
#endif

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {
        return _bcm_td3_l3_flex_entry_add(unit, l3cfg, 0);
    }
#endif

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory and table fields */
    if (ipv6) {
        if (soc_mem_is_valid(unit, L3_ENTRY_QUADm)) {
            mem     = L3_ENTRY_QUADm;
            l3mc_idx    = IPV6MC__DESTINATIONf;
            if (soc_feature(unit, soc_feature_generic_dest)) {
	            l3mc_idx = IPV6MC__DESTINATIONf;
            } else {
                l3mc_idx = IPV6MC__L3MC_INDEXf;
            }
            l3mc_idx_l2 = IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
        } else {
        mem         = L3_ENTRY_IPV6_MULTICASTm;
            l3mc_idx    = IPV6MC__L3MC_INDEXf;
            if (soc_mem_field_valid(unit, mem,
                                    IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf)) {
                l3mc_idx_l2 = IPV6MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
            } else {
                l3mc_idx_l2 = INVALIDf;
            }
        }
        pri         = IPV6MC__PRIf;
        rpe         = IPV6MC__RPEf;
        vrf_id      = IPV6MC__VRF_IDf;
        class_id    = IPV6MC__CLASS_IDf;
        dst_discard = IPV6MC__DST_DISCARDf;
        rpa_id      = IPV6MC__RPA_IDf;
        exp_iif     = IPV6MC__EXPECTED_L3_IIFf;
        exp_iif_drop      = IPV6MC__IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf;
        exp_iif_copytocpu = IPV6MC__IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf;       
        flex_ctr_base_counter_idx = IPV6MC__FLEX_CTR_BASE_COUNTER_IDXf;
        flex_ctr_offset_mode      = IPV6MC__FLEX_CTR_OFFSET_MODEf;
        flex_ctr_pool_number      = IPV6MC__FLEX_CTR_POOL_NUMBERf;
#ifdef BCM_MONTEREY_SUPPORT
        roe_decap = IPV6MC__ROE_DECAPf;
#endif
    } else {
        if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
            mem     = L3_ENTRY_DOUBLEm;
            if (soc_feature(unit, soc_feature_generic_dest)) {
            l3mc_idx    = IPV4MC__DESTINATIONf;
        } else {
	            l3mc_idx    = IPV4MC__L3MC_INDEXf;
            }
            l3mc_idx_l2 = IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
        } else {
        mem         = L3_ENTRY_IPV4_MULTICASTm;
            l3mc_idx    = IPV4MC__L3MC_INDEXf;
            if (soc_mem_field_valid(unit, mem,
                                    IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf)) {
                l3mc_idx_l2 = IPV4MC__L3MC_INDEX_FOR_L2_DISTRIBUTIONf;
            } else {
                l3mc_idx_l2 = INVALIDf;
            }
        }

        pri         = IPV4MC__PRIf;
        rpe         = IPV4MC__RPEf;
        vrf_id      = IPV4MC__VRF_IDf;
        class_id    = IPV4MC__CLASS_IDf;
        dst_discard = IPV4MC__DST_DISCARDf;
        rpa_id      = IPV4MC__RPA_IDf;
        exp_iif     = IPV4MC__EXPECTED_L3_IIFf;
        exp_iif_drop      = IPV4MC__IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf;
        exp_iif_copytocpu = IPV4MC__IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf;
        flex_ctr_base_counter_idx = IPV4MC__FLEX_CTR_BASE_COUNTER_IDXf;
        flex_ctr_offset_mode      = IPV4MC__FLEX_CTR_OFFSET_MODEf;
        flex_ctr_pool_number      = IPV4MC__FLEX_CTR_POOL_NUMBERf;
#ifdef BCM_MONTEREY_SUPPORT
        roe_decap = IPV4MC__ROE_DECAPf;
#endif
    }

    /*  Zero buffers. */
    buf_p = l3x_entry;
    sal_memcpy(buf_p, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

   /* Prepare entry to write. */
   rv = _bcm_td2_l3_ipmc_ent_init(unit, l3x_entry, l3cfg);
   if (BCM_FAILURE(rv)) {
       return rv;
   }

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, buf_p, rpe, 1);
    }

    /* Set destination discard. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, buf_p, dst_discard, 1);
    }
    
    /* Virtual router id. */
    soc_mem_field32_set(unit, mem, buf_p, vrf_id, l3cfg->l3c_vrf);

    /* Set priority. */
    soc_mem_field32_set(unit, mem, buf_p, pri, l3cfg->l3c_prio);

    /* Pointer to ipmc table. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_mem_field32_dest_set(unit, mem, buf_p, l3mc_idx,
            SOC_MEM_FIF_DEST_IPMC, l3cfg->l3c_ipmc_ptr);
    } else {
    soc_mem_field32_set(unit, mem, buf_p, l3mc_idx, l3cfg->l3c_ipmc_ptr);
    }

    /* Set L3MC_INDEX_FOR_L2_DISTRIBUTION */
    if (l3mc_idx_l2 != INVALIDf && soc_mem_field_valid(unit, mem, l3mc_idx_l2)) {
        soc_mem_field32_set(unit, mem, buf_p, l3mc_idx_l2, l3cfg->l3c_ipmc_ptr_l2);
    }

    /* Classification lookup class id. */
    soc_mem_field32_set(unit, mem, buf_p, class_id, l3cfg->l3c_lookup_class);
   
    /* Rendezvous point ID. */
    if (SOC_MEM_FIELD_VALID(unit, mem, rpa_id)) {
    if (l3cfg->l3c_rp_id != BCM_IPMC_RP_ID_INVALID) {
        soc_mem_field32_set(unit, mem, buf_p, rpa_id, l3cfg->l3c_rp_id);
    }
    }

    /* RPF */
    if ((BCM_IPMC_POST_LOOKUP_RPF_CHECK & l3cfg->l3c_flags) &&
        (0 != l3cfg->l3c_intf)) {
        /* Set Expected_IIF to L3_IIF*/
        soc_mem_field32_set(unit, mem, buf_p, exp_iif, l3cfg->l3c_intf);
        if (BCM_IPMC_RPF_FAIL_DROP & l3cfg->l3c_flags) {
            soc_mem_field32_set(unit, mem, buf_p, exp_iif_drop, 1);
        }
        if (BCM_IPMC_RPF_FAIL_TOCPU & l3cfg->l3c_flags) {
            soc_mem_field32_set(unit, mem, buf_p, exp_iif_copytocpu, 1);
        }
    }

    idx_max = (ipv6) ? 4 : 2;
    for (idx = 0; idx < idx_max; idx++) {
        /* Set hit bit. */
        if (l3cfg->l3c_flags & BCM_L3_HIT) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 1);
        }
    }

    
    if (!SOC_IS_MONTEREY(unit)) {
    /* Base counter index. */
    soc_mem_field32_set(unit, mem, buf_p, flex_ctr_base_counter_idx,
                        l3cfg->l3c_flex_ctr_base_id);
    /* Counter offset mode. */
    soc_mem_field32_set(unit, mem, buf_p, flex_ctr_offset_mode,
                        l3cfg->l3c_flex_ctr_mode);
    /* Counter pool number. */
    soc_mem_field32_set(unit, mem, buf_p, flex_ctr_pool_number,
                        l3cfg->l3c_flex_ctr_pool);
    }

#ifdef BCM_MONTEREY_SUPPORT
    if (soc_feature(unit, soc_feature_roe) ||
        soc_feature(unit, soc_feature_roe_custom)) {
        if (SOC_MEM_FIELD_VALID(unit, mem, roe_decap)) {
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_DECAP_ROE_HEADER) {
                soc_mem_field32_set(unit, mem, buf_p, roe_decap, 1);
            }
        }
    }
#endif

    /* Write entry to the hw. */
    MEM_LOCK(unit, mem);
    /* Handle replacement. */
    if (BCM_XGS3_L3_INVALID_INDEX != l3cfg->l3c_hw_index) {
        rv = BCM_XGS3_MEM_WRITE(unit, mem, l3cfg->l3c_hw_index, buf_p);
    } else {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)buf_p);
    }

    /* Increment number of ipmc routes. */
    if (BCM_SUCCESS(rv) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)++ : \
                 BCM_XGS3_L3_IP4_IPMC_CNT(unit)++;
    }
    /*    coverity[overrun-local : FALSE]    */
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_ipmc_del
 * Purpose:
 *      Delete TD2 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN)Group/Source deletion key.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    uint32 l3x_entry[SOC_MAX_MEM_FIELD_WORDS];  /* Delete buffer.          */
    soc_mem_t mem;                              /* IPMC table memory.      */
    int ipv6;                                   /* IPv6 entry indicator.   */
    int rv;                                     /* Operation return value. */

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3cfg->l3c_flow_handle != 0)) {
        return _bcm_td3_l3_flex_entry_del(unit, l3cfg);
    }
#endif

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
        mem = (ipv6) ? L3_ENTRY_QUADm : L3_ENTRY_DOUBLEm;
    } else {
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;
    }

    /*  Zero entry buffer. */
    sal_memcpy(l3x_entry, soc_mem_entry_null(unit, mem),
               soc_mem_entry_words(unit,mem) * 4);


    /* Key preparation. */
    rv = _bcm_td2_l3_ipmc_ent_init(unit, l3x_entry, l3cfg);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Delete the entry from hw. */
    MEM_LOCK(unit, mem);

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)l3x_entry);

    /* Decrement number of ipmc routes. */
    if (BCM_SUCCESS(rv)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)-- : \
            BCM_XGS3_L3_IP4_IPMC_CNT(unit)--;
    }
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*
 * FLEX STAT FUNCTIONS for L3 ROUTE and L3 NAT
 *
 * This following functions implements flex stat routines to attach, detach and get/set
 * flex counter functions.
 * 
 * FLEX ATTACH 
 *   The function allocates flex counter from the counter pool and configures
 *   attributes to set the base counter index and base offset. Flex counter pool,
 *   base index and offset is associated to the table index request for the
 *   module identifiers like, vpn, port, vp Additionally the reference counter
 *   for the flex counter is incremented for multple references of the counter.
 *
 * FLEX DETACH 
 *   The function disassociates flex counter details assigned during flex attach
 *   and decrements the reference counter.
 *
 * FLEX COUNTER GET/SET 
 *   The function read/set the associate counter values. Set is required for the
 *   purpose of clearing the counter.
 *
 *
 * FLEX COUNTER MULTI GET/SET
 *   The function reads/sets multiple flex stats and returns the counter arrays. 
 *
 * FLEX STAT ID
 *   The funciton retrieves the stat id for the given module identifiers. The
 *   flex counter stat id has the following format.
 *
 *          (8 modes)  (32 groups) (16 pools)  (32 objIds)  (64K index)
 *         +---------+------------+----------+-----------+-----------------+ 
 *         |         |            |          |           |                 | 
 *         | ModeId  |  GroupMode |   PoolId | Account   |  Base Index     |
 *         |         |            |          | objectId  |                 |
 *         +---------+------------+----------+-----------+-----------------+
 *        (31)    (29)(28)     (24)(23)   (20)(19)    (15)(14)            (0)
 *
 *
 *   PoolId      : Counter pool no. 
 *                 ( TD2: Ingress(8 pool x 4k counters)
 *                        Egress (4 pool x 4k counters)
 *   GroupMode   : Attribute group mode determines base offset and no. of counters
 *   ModeId      : Ingress/Egress Modes
 *   Base Idx    : Index into counter block of 256 counters
 *   Account Obj : Flexible accounting objects
 */

/*
 * Function:
 *      _bcm_td2_l3_route_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given ingress interface.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
#ifdef BCM_TOMAHAWK_SUPPORT
#ifdef ALPM_ENABLE
#define ALPM_BKT_NODE_PTR_CNT   32768
/* No need per unit setting */
uint32 *alpm_bkt_node_ptr[ALPM_BKT_NODE_PTR_CNT];

uint32 *alpm_bkt_node_ptr_arr_get(uint32 idx)
{
    if (idx >= 0 && idx < ALPM_BKT_NODE_PTR_CNT) {
        return alpm_bkt_node_ptr[idx];
    }

    return NULL;
}

int alpm_bkt_node_ptr_arr_idx_get(uint32 *bkt_node_ptr, uint32 *idx)
{
    int i;

    for (i = 0; i < ALPM_BKT_NODE_PTR_CNT; i++) {
        if (alpm_bkt_node_ptr[i] == bkt_node_ptr) {
            *idx = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

int alpm_bkt_node_ptr_arr_add(uint32 *bkt_node_ptr, uint32 *idx)
{
    int i, first_null_idx = -1;

    for (i = 0; i < ALPM_BKT_NODE_PTR_CNT; i++) {
        if (alpm_bkt_node_ptr[i] == bkt_node_ptr) {
            *idx = i;
            return BCM_E_EXISTS;
        }

        if (first_null_idx == -1 && alpm_bkt_node_ptr[i] == NULL) {
            first_null_idx = i;
        }
    }

    if (first_null_idx == -1) {
        return BCM_E_FULL;
    } else {
        alpm_bkt_node_ptr[first_null_idx] = bkt_node_ptr;
        *idx = first_null_idx;
        return BCM_E_NONE;
    }
}

int alpm_bkt_node_ptr_arr_del(uint32 *bkt_node_ptr)
{
    int i;

    for (i = 0; i < ALPM_BKT_NODE_PTR_CNT; i++) {
        if (alpm_bkt_node_ptr[i] == bkt_node_ptr) {
            alpm_bkt_node_ptr[i] = NULL;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

#endif /* ALPM_ENABLE */
#endif
static
bcm_error_t  _bcm_td2_l3_route_stat_get_table_info(
            int                        unit,
            bcm_l3_route_t	           *info,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info,
            _bcm_defip_cfg_t           *defip_copy)
{
    int rv;
    int index, scale;
    soc_mem_t mem;
    int max_prefix_length;
    _bcm_defip_cfg_t defip;
    soc_mem_t defip_mem = L3_DEFIPm;
    soc_mem_t defip_pair_mem = L3_DEFIP_PAIR_128m;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
	    defip_mem = L3_DEFIP_LEVEL1m;
        defip_pair_mem = L3_DEFIP_PAIR_LEVEL1m;
    }

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Vrf is in device supported range check. */
    if ((info->l3a_vrf > SOC_VRF_MAX(unit)) || 
        (info->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
        return BCM_E_PARAM;
    }

    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, info->l3a_flags)) {
        return BCM_E_UNAVAIL;
    }

    mem = defip_mem;
    scale = 1;

    sal_memset(&defip, 0, sizeof(_bcm_defip_cfg_t));
    defip.defip_flags = info->l3a_flags;
    defip.defip_vrf = info->l3a_vrf;

    L3_LOCK(unit);
    if (info->l3a_flags & BCM_L3_IP6) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit) && soc_feature(unit, soc_feature_flex_flow)
            && (info->flow_handle != 0)) {
            soc_mem_t view_id;
            soc_flow_db_mem_view_info_t vinfo;

            BCM_IF_ERROR_RETURN(
                    soc_flow_db_ffo_to_mem_view_id_get(unit,
                        info->flow_handle,
                        info->flow_option_handle,
                        SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                        (uint32 *)&view_id));
            BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, view_id, &vinfo));
            mem = vinfo.mem;
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            max_prefix_length =
                soc_feature(unit, soc_feature_lpm_prefix_length_max_128) ? 128 : 64;
            sal_memcpy(defip.defip_ip6_addr, info->l3a_ip6_net, BCM_IP6_ADDRLEN);
            defip.defip_sub_len = bcm_ip6_mask_length(info->l3a_ip6_mask);
            if (defip.defip_sub_len > max_prefix_length) {
                L3_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if (defip.defip_sub_len > 64) {
                if (SOC_PERSIST(unit)->memState[defip_pair_mem].index_max >= 0) {
                    mem = defip_pair_mem;
                }
            } else {
                scale = 2;
            }
        }
        rv = mbcm_driver[unit]->mbcm_ip6_defip_cfg_get(unit, &defip);
    } else {
        defip.defip_ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        defip.defip_sub_len = bcm_ip_mask_length(info->l3a_ip_mask);
        rv = mbcm_driver[unit]->mbcm_ip4_defip_cfg_get(unit, &defip);
    }

    L3_UNLOCK(unit);
    if (rv < 0) {
        return rv;
    }

    index = defip.defip_index * scale;

#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm) &&
        soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {

        if (defip.defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT) {
            /* override table and index to ALPM format */
            if (soc_feature(unit, soc_feature_alpm2)) {
                if (SOC_MEM_IS_VALID(unit, L3_DEFIP_ALPM_RAWm)) {
                mem = L3_DEFIP_ALPM_RAWm;
            } else {
                    if (soc_property_get(unit, "l3_alpm2_level", 3) == 2) {
                        mem = L3_DEFIP_ALPM_LEVEL2m;
                    } else {
                        mem = L3_DEFIP_ALPM_LEVEL3m;
                    }
               }
            } else {
                if (info->l3a_flags & BCM_L3_IP6) {
                    if (soc_mem_index_count(unit, defip_pair_mem)) {
                        mem = L3_DEFIP_ALPM_IPV6_128m;
                    } else {
                        mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    }
                } else {
                    mem = L3_DEFIP_ALPM_IPV4_1m;
                }
                if (mem == L3_DEFIP_ALPM_IPV6_64_1m || mem == L3_DEFIP_ALPM_IPV4_1m) {
                    if (!(defip.defip_alpm_cookie & SOC_ALPM_COOKIE_FLEX)) {
                        return BCM_E_PARAM;
                    }
                }
            }
            index = ALPM_ENT_INDEX(defip.defip_index);
            if (defip_copy) {
                sal_memcpy(defip_copy, &defip, sizeof(defip));
            }
        } else { /* Entries in TCAM, like Global High, IPMC */
            index = defip.defip_index;
            if (info->l3a_flags & BCM_L3_IP6) {
                if (soc_mem_index_count(unit, defip_pair_mem)) {
                    mem = defip_pair_mem;
                } else {
                    mem = defip_mem;
                    index *= 2;
                }
            } else {
                mem = defip_mem;
            }
            if (defip_copy) {
                sal_memcpy(defip_copy, &defip, sizeof(defip));
            }
        }
    }
#endif

    table_info[*num_of_tables].table = mem;
    table_info[*num_of_tables].index = index;
    table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
    (*num_of_tables)++;

    LOG_INFO(BSL_LS_BCM_L3,
             (BSL_META_U(unit,
                         "L3 Route Stat: table = %s, index = %d\n"),
              SOC_MEM_NAME(unit, mem), index));
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_l3_route_stat_attach
 * Description:
 *      Attach counters entries to the given  l3 route defip index . Allocate
 *      flex counter and assign counter index and offset for the index pointed
 *      by l3 defip index
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
    
bcm_error_t  _bcm_td2_l3_route_stat_attach (
                            int                 unit,
                            bcm_l3_route_t	    *info,
                            uint32              stat_counter_id)
{
    int                        rv = BCM_E_NONE;
    int                        counter_flag = 0;
    soc_mem_t                  table[5] = {INVALIDm};
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     flex_table_index = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    _bcm_defip_cfg_t           defip;


    /* Get pool, base index group mode and offset modes from stat counter id */
    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, 5, &actual_num_tables, &table[0], &direction));
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_get_table_info(
                        unit, info, &num_of_tables, &table_info[0], &defip));

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
       (table_info[0].table == L3_DEFIP_ALPM_RAWm ||
        table_info[0].table == L3_DEFIP_ALPM_LEVEL2m ||
        table_info[0].table == L3_DEFIP_ALPM_LEVEL3m)) {
        /* flex counter only available in Full route data mode,
           ALPM_ROUTE_FULL_MD is same as SOC_ALPM_COOKIE_FLEX */
        if ((defip.defip_alpm_cookie & ALPM_ROUTE_FULL_MD) == 0) {
            return BCM_E_UNAVAIL;
        }

        rv = alpm_bkt_node_ptr_arr_add(defip.bkt_node, &table_info[0].index);
        if (rv == BCM_E_FULL) {
            return rv;
        }
    }
#endif
#endif

    for (count = 0; count < num_of_tables; count++) {
        for (flex_table_index=0; 
             flex_table_index < actual_num_tables ; 
             flex_table_index++) {
            if ((table_info[count].direction == direction) &&
                (table_info[count].table == table[flex_table_index]) ) {
                if (direction == bcmStatFlexDirectionIngress) {
                    counter_flag = 1;
                    rv = _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         offset_mode,
                         base_index,
                         pool_number);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                } else {
                    counter_flag = 1;
                    rv = _bcm_esw_stat_flex_attach_egress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         0,
                         offset_mode,
                         base_index,
                         pool_number);

                    if(BCM_FAILURE(rv)) {
                        break;
                    }
                } 
            }
        }
    }

    if (counter_flag == 0) {
        rv = BCM_E_NOT_FOUND;
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm) &&
        soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {

        if (defip.defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT) {
            /* for ALPM, need call add to trigger propation */
            L3_LOCK(unit);
            defip.defip_flags |= BCM_L3_REPLACE;
            if (info->l3a_flags & BCM_L3_IP6) {
                rv = mbcm_driver[unit]->mbcm_ip6_defip_add(unit, &defip);
            } else {
                rv = mbcm_driver[unit]->mbcm_ip4_defip_add(unit, &defip);
            }
            L3_UNLOCK(unit);
        }
    }
#endif

    return rv;
}


/*
 * Function:
 *      _bcm_td2_l3_route_stat_detach
 * Description:
 *      Detach counter entries to the given  l3 route index. 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info		     - (IN) L3 Route Info
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  _bcm_td2_l3_route_stat_detach (
                            int                 unit,
                            bcm_l3_route_t	    *info)
{
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
    _bcm_defip_cfg_t           defip;

    /* Get the table index to be detached */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_get_table_info(
                        unit, info, &num_of_tables, &table_info[0], &defip));

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
        (table_info[0].table == L3_DEFIP_ALPM_RAWm ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL2m ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL3m)) {
        /* flex counter only available in Full route data mode,
           ALPM_ROUTE_FULL_MD is same as SOC_ALPM_COOKIE_FLEX */
        if ((defip.defip_alpm_cookie & ALPM_ROUTE_FULL_MD) == 0) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(
            alpm_bkt_node_ptr_arr_idx_get(defip.bkt_node, &table_info[0].index));
    }
#endif
#endif

    for (count = 0; count < num_of_tables; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
             }
         } else {
             rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
             if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
             }
         }
    }

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
        table_info[0].table == L3_DEFIP_ALPM_RAWm) {
        (void)alpm_bkt_node_ptr_arr_del(defip.bkt_node);
    }
#endif
#endif

    BCM_STAT_FLEX_DETACH_RV_SET(rv, err_code)

    if (BCM_FAILURE(rv)) {
        return rv;
    }

#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm) &&
        soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {

        if (defip.defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT) {
            /* for ALPM, need call add to trigger propation */
            L3_LOCK(unit);
            defip.defip_flags |= BCM_L3_REPLACE;
            if (info->l3a_flags & BCM_L3_IP6) {
                rv = mbcm_driver[unit]->mbcm_ip6_defip_add(unit, &defip);
            } else {
                rv = mbcm_driver[unit]->mbcm_ip4_defip_add(unit, &defip);
            }
            L3_UNLOCK(unit);
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_route_stat_counter_get
 *
 * Description:
 *  Get l3 route counter value for specified l3 route index
 *  if sync_mode is set, sync the sw accumulated count
 *  with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      info			 - (IN)  L3 Route Info
 *      stat             - (IN) l3 route counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  _bcm_td2_l3_route_stat_counter_get(
                            int                 unit, 
                            int                 sync_mode, 
                            bcm_l3_route_t	    *info,
                            bcm_l3_route_stat_t stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    _bcm_defip_cfg_t           defip;

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmL3RouteInPackets) ||
        (stat == bcmL3RouteInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
        /* direction = bcmStatFlexDirectionEgress;*/
        return BCM_E_UNAVAIL;
    }
    if (stat == bcmL3RouteInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_get_table_info(
                        unit, info, &num_of_tables, &table_info[0], &defip));

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
        (table_info[0].table == L3_DEFIP_ALPM_RAWm ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL2m ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL3m)) {
        BCM_IF_ERROR_RETURN(
            alpm_bkt_node_ptr_arr_idx_get(defip.bkt_node, &table_info[0].index));
    }
#endif
#endif

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_route_stat_counter_set
 *
 * Description:
 *  Set l3 route counter value for specified l3 route index
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      stat             - (IN) l3 route counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  _bcm_td2_l3_route_stat_counter_set(
                            int                 unit, 
                            bcm_l3_route_t	    *info,
                            bcm_l3_route_stat_t stat, 
                            uint32              num_entries, 
                            uint32              *counter_indexes, 
                            bcm_stat_value_t    *counter_values)
{
    uint32                     table_count=0;
    uint32                     index_count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    _bcm_defip_cfg_t           defip;

    /* Get the flex counter direction - ingress/egress */
    if ((stat == bcmL3RouteInPackets) ||
        (stat == bcmL3RouteInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
        /* direction = bcmStatFlexDirectionEgress;*/
        return BCM_E_UNAVAIL;
    }
    if (stat == bcmL3RouteInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_get_table_info(
                        unit, info, &num_of_tables, &table_info[0], &defip));

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
        (table_info[0].table == L3_DEFIP_ALPM_RAWm ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL2m ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL3m)) {
        BCM_IF_ERROR_RETURN(
            alpm_bkt_node_ptr_arr_idx_get(defip.bkt_node, &table_info[0].index));
    }
#endif
#endif

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l3_route_stat_multi_get
 *
 * Description:
 *  Get Multiple l3 route counter value for specified l3 route index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

    
 bcm_error_t  _bcm_td2_l3_route_stat_multi_get(
                            int                 unit, 
                            bcm_l3_route_t	    *info,
                            int                 nstat, 
                            bcm_l3_route_stat_t *stat_arr,
                            uint64              *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    int              sync_mode = 0; 

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_counter_get( 
                             unit, sync_mode, info, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmL3RouteInPackets)) {
             COMPILER_64_SET(value_arr[idx],
                             COMPILER_64_HI(counter_values.packets64),
                             COMPILER_64_LO(counter_values.packets64));
         } else {
             COMPILER_64_SET(value_arr[idx],
                             COMPILER_64_HI(counter_values.bytes),
                             COMPILER_64_LO(counter_values.bytes));
         }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_route_stat_multi_get32
 *
 * Description:
 *  Get 32bit l3 route counter value for specified l3 route index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info		 	 - (IN) L3 Route Info
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  _bcm_td2_l3_route_stat_multi_get32(
                            int                 unit, 
                            bcm_l3_route_t	    *info,
                            int                 nstat, 
                            bcm_l3_route_stat_t *stat_arr,
                            uint32              *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};
    int              sync_mode = 0;

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_counter_get( 
                             unit, sync_mode, info, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
         if ((stat_arr[idx] == bcmL3RouteInPackets)) {
                value_arr[idx] = counter_values.packets;
         } else {
             value_arr[idx] = COMPILER_64_LO(counter_values.bytes);
         }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_td2_l3_route_stat_multi_set
 *
 * Description:
 *  Set l3 route counter value for specified l3 route index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      stat             - (IN) l3 route counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  _bcm_td2_l3_route_stat_multi_set(
                            int                 unit, 
                            bcm_l3_route_t	    *info,
                            int                 nstat, 
                            bcm_l3_route_stat_t *stat_arr,
                            uint64              *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    /* Iterate f all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmL3RouteInPackets)) {
              counter_values.packets = COMPILER_64_LO(value_arr[idx]);
         } else {
              COMPILER_64_SET(counter_values.bytes,
                              COMPILER_64_HI(value_arr[idx]),
                              COMPILER_64_LO(value_arr[idx]));
         }
          BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_counter_set( 
                             unit, info, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;

}

/*
 * Function:
 *      _bcm_td2_l3_route_stat_multi_set32
 *
 * Description:
 *  Set 32bit l3 route counter value for specified l3 route index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) L3 Route Info
 *      stat             - (IN) l3 route counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t  _bcm_td2_l3_route_stat_multi_set32(
                            int                 unit, 
                            bcm_l3_route_t	    *info,
                            int                 nstat, 
                            bcm_l3_route_stat_t *stat_arr,
                            uint32              *value_arr)
{
    int              rv = BCM_E_NONE;
    int              idx;
    uint32           counter_indexes = 0;
    bcm_stat_value_t counter_values = {0};

    /* Iterate of all stats array to retrieve flex counter values */
    for (idx = 0; idx < nstat ;idx++) {
         if ((stat_arr[idx] == bcmL3RouteInPackets)) {
             counter_values.packets = value_arr[idx];
         } else {
             COMPILER_64_SET(counter_values.bytes,0,value_arr[idx]);
         }

         BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_counter_set( 
                             unit, info, stat_arr[idx], 
                             1, &counter_indexes, &counter_values));
    }
    return rv;
}

/*
 * Function: 
 *      _bcm_td2_l3_route_stat_id_get
 *
 * Description: 
 *      Get Stat Counter if associated with given l3 route index
 *
 * Parameters: 
 *      unit             - (IN) bcm device
 *      info			 - (IN) L3 Route Info
 *      stat             - (IN) l3 route counter stat types
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t  _bcm_td2_l3_route_stat_id_get(
                            int                 unit,
                            bcm_l3_route_t	    *info,
                            bcm_l3_route_stat_t stat, 
                            uint32              *stat_counter_id) 
{
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;
    _bcm_defip_cfg_t           defip;

    if ((stat == bcmL3RouteInPackets) ||
        (stat == bcmL3RouteInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
        /* direction = bcmStatFlexDirectionEgress;*/
        return BCM_E_UNAVAIL;
    }

    /*  Get Tables, for which flex counter are attached  */
    BCM_IF_ERROR_RETURN(_bcm_td2_l3_route_stat_get_table_info(
                        unit, info, &num_of_tables, &table_info[0], &defip));

#ifdef ALPM_ENABLE
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2) &&
        (table_info[0].table == L3_DEFIP_ALPM_RAWm ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL2m ||
         table_info[0].table == L3_DEFIP_ALPM_LEVEL3m)) {
        BCM_IF_ERROR_RETURN(
            alpm_bkt_node_ptr_arr_idx_get(defip.bkt_node, &table_info[0].index));
    }
#endif
#endif

    /* Retrieve stat counter id */
    for (index = 0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids, stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}
/***************************************************************
 *                        IP4 Options
 */

/* Free IP4 options resources */
int _bcm_td2_l3_ip4_options_free_resources(int unit)
{
    _bcm_l3_bookkeeping_t *l3_bk_info = L3_INFO(unit);

    /* free ip4 options profile usage bitmap */
    if (NULL != l3_bk_info->ip4_options_bitmap) {
        sal_free(l3_bk_info->ip4_options_bitmap);
        l3_bk_info->ip4_options_bitmap = NULL;
    }

    /* free ip4 options hw index usage bitmap */
    if (NULL != l3_bk_info->ip4_profiles_hw_idx) {
        sal_free(l3_bk_info->ip4_profiles_hw_idx);
        l3_bk_info->ip4_profiles_hw_idx = NULL;
    }
    if (NULL != ip4_profiles_hw_idx_lock[unit]) {
        sal_mutex_destroy(ip4_profiles_hw_idx_lock[unit]);
        ip4_profiles_hw_idx_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}

/* Initialize the IP4 options book keeping */
int 
_bcm_td2_l3_ip4_options_profile_init(int unit)
{
    _bcm_l3_bookkeeping_t *l3_bk_info = L3_INFO(unit);
    int ip4_options_profiles;
    int default_index = BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX;
    int rv = BCM_E_NONE;

    ip4_options_profiles = _BCM_IP4_OPTIONS_LEN;

    /* Allocate ip4 options profile usage bitmap */
    if (NULL == l3_bk_info->ip4_options_bitmap) {
        l3_bk_info->ip4_options_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ip4_options_profiles), "ip4_options_bitmap");
        if (l3_bk_info->ip4_options_bitmap == NULL) {
            _bcm_td2_l3_ip4_options_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(l3_bk_info->ip4_options_bitmap, 0, SHR_BITALLOCSIZE(ip4_options_profiles));
    if (NULL == l3_bk_info->ip4_profiles_hw_idx) {
        l3_bk_info->ip4_profiles_hw_idx = 
            sal_alloc(sizeof(uint32) * ip4_options_profiles, "ip4_profiles_hw_idx");
        if (l3_bk_info->ip4_profiles_hw_idx == NULL) {
            _bcm_td2_l3_ip4_options_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(l3_bk_info->ip4_profiles_hw_idx, 0, (sizeof(uint32) * ip4_options_profiles));

    if (NULL == ip4_profiles_hw_idx_lock[unit]) {
        ip4_profiles_hw_idx_lock[unit] = sal_mutex_create("Ip4 profiles hw idx mutex");
        
        if (NULL == ip4_profiles_hw_idx_lock[unit]) {
            _bcm_td2_l3_ip4_options_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    rv = _bcm_td2_l3_ip4_options_profile_create (unit, BCM_L3_IP4_OPTIONS_WITH_ID, 
                                            bcmIntfIPOptionActionDrop, &default_index);
    return rv;
}



STATIC int
_bcm_td2_l3_ip4_profile_id_alloc(int unit)
{
    int i, option_profile_size = -1;
    SHR_BITDCL *bitmap;

    bitmap = L3_INFO(unit)->ip4_options_bitmap;
    option_profile_size = _BCM_IP4_OPTIONS_LEN;
    for (i = 0; i < option_profile_size; i++) {
        if (!SHR_BITGET(bitmap, i)) {
            return i;
        }
    }
    return -1;
}

int
_bcm_td2_l3_ip4_options_profile_create(int unit, uint32 flags, 
                                   bcm_l3_ip4_options_action_t default_action, 
                                   int *ip4_options_profile_id)
{
    ip_option_control_profile_table_entry_t ip_option_profile[_BCM_IP_OPTION_PROFILE_CHUNK];
    int id, index = -1, rv = BCM_E_NONE;
    int i, copy_cpu, drop;
    void *entries[1], *entry;

    /* Check for pre-specified ID */
    if (flags & BCM_L3_IP4_OPTIONS_WITH_ID) {
        id = *ip4_options_profile_id;
        if (id >= _BCM_IP4_OPTIONS_LEN ) {
            return BCM_E_PARAM;
        }  
        if (_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
            if (!(flags & BCM_L3_IP4_OPTIONS_REPLACE)) {
                return BCM_E_EXISTS;
            } else {
                if (id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
                    return BCM_E_BADID;
                }
            }
        } 
    }
    /* Reserve a chunk in the IP4_OPTION Profile table */
    sal_memset(ip_option_profile, 0, sizeof(ip_option_profile));
    entries[0] = &ip_option_profile;

    /* Set the default action for the profile */
    switch (default_action) {
    case bcmIntfIPOptionActionCopyToCPU:
        copy_cpu = 1;
        drop = 0;
        break;
    case bcmIntfIPOptionActionDrop:
        copy_cpu = 0;
        drop = 1;
        break;
    case bcmIntfIPOptionActionCopyCPUAndDrop:
        copy_cpu = 1;
        drop = 1;
        break;
    case bcmIntfIPOptionActionNone:
    default :
        copy_cpu = 0;
        drop = 0;
        break;
    }
    for (i = 0; i < _BCM_IP_OPTION_PROFILE_CHUNK; i++) {
        entry = &ip_option_profile[i];
        soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                            COPYTO_CPUf, copy_cpu);
        soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                            DROPf, drop);
    }

    BCM_IF_ERROR_RETURN(_bcm_l3_ip4_options_profile_entry_add(unit, entries, 
                                        _BCM_IP_OPTION_PROFILE_CHUNK,
                                         (uint32 *)&index));
       /* To avoid deadlock, There should be not any semaphore 
        * between HW_IDX_LOCK and HW_IDX_UNLOCK 
        */
    HW_IDX_LOCK(unit);

    if (flags & BCM_L3_IP4_OPTIONS_WITH_ID) {
        id = *ip4_options_profile_id;
        if (0 == (_BCM_IP4_OPTIONS_USED_GET(unit, id))) {
            _BCM_IP4_OPTIONS_USED_SET(unit, id);
        }
    } else {
        id = _bcm_td2_l3_ip4_profile_id_alloc(unit);
        if (id == -1) {
            HW_IDX_UNLOCK(unit);
            _bcm_l3_ip4_options_profile_entry_delete(unit, index);
            return BCM_E_RESOURCE;
        }
        _BCM_IP4_OPTIONS_USED_SET(unit, id);
        *ip4_options_profile_id = id;
    }

    L3_INFO(unit)->ip4_profiles_hw_idx[id] = (index / 
                                          _BCM_IP_OPTION_PROFILE_CHUNK);
    HW_IDX_UNLOCK(unit);

    return rv;
}



int 
_bcm_td2_l3_ip4_options_profile_destroy(int unit, int ip4_options_profile_id)
{
    int id, rv = BCM_E_UNAVAIL;
    int index;
    
    if (ip4_options_profile_id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
        return BCM_E_BADID;
    }
    L3_LOCK(unit);
    id = ip4_options_profile_id;
    if (!_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
        L3_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
    
        HW_IDX_LOCK(unit);
        index = L3_INFO(unit)->ip4_profiles_hw_idx[id];
        L3_INFO(unit)->ip4_profiles_hw_idx[id] = 0;
        HW_IDX_UNLOCK(unit);
        
        rv = _bcm_l3_ip4_options_profile_entry_delete
                 (unit, index * _BCM_IP_OPTION_PROFILE_CHUNK);
        
        _BCM_IP4_OPTIONS_USED_CLR(unit, id);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    L3_UNLOCK(unit);
    return rv;
}

/* Add an entry to a  ip_option_profile */
/* Read the existing profile chunk, modify what's needed and add the 
 * new profile. This can result in the HW profile index changing for a 
 * given ID */
int 
_bcm_td2_l3_ip4_options_profile_action_set(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t action)
{
    ip_option_control_profile_table_entry_t ip_option_profile[_BCM_IP_OPTION_PROFILE_CHUNK];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[1], *entry;
    int offset;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    id = ip4_options_profile_id;
    if (id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
        return BCM_E_BADID;
    }
    L3_LOCK(unit);

    if (!_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
        L3_UNLOCK(unit);
        return BCM_E_PARAM;
    } else {
        entries[0] = ip_option_profile;

        /* Base index of table in hardware */
        GET_HW_IDX(unit, index, id);
        index = index * _BCM_IP_OPTION_PROFILE_CHUNK;

        rv = _bcm_l3_ip4_options_profile_entry_get(unit, index, 
                                            _BCM_IP_OPTION_PROFILE_CHUNK, 
                                            entries);
        if (BCM_FAILURE(rv)) {
            L3_UNLOCK(unit);
            return (rv);
        }

        /* Offset within table */
        offset = ip4_option;

        /* Modify what's needed */
        entry = &ip_option_profile[offset];
        switch (action) {
        case bcmIntfIPOptionActionCopyToCPU:
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                COPYTO_CPUf, 1);
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                DROPf, 0);
            break;

        case bcmIntfIPOptionActionDrop:
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                COPYTO_CPUf, 0);
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                DROPf, 1);
            break;

        case bcmIntfIPOptionActionCopyCPUAndDrop:
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                COPYTO_CPUf, 1);
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                DROPf, 1);
            break;

        case bcmIntfIPOptionActionNone:
        default :
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                COPYTO_CPUf, 0);
            soc_mem_field32_set(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                DROPf, 0);

        }
        /* Delete the old profile chunk */
        rv = _bcm_l3_ip4_options_profile_entry_delete(unit, index);
        if (BCM_FAILURE(rv)) {
            L3_UNLOCK(unit);
            return (rv);
        }

        /* Add new chunk and store new HW index */
        rv = _bcm_l3_ip4_options_profile_entry_add(unit, entries,
                                            _BCM_IP_OPTION_PROFILE_CHUNK, 
                                            (uint32 *)&index);
        
        SET_HW_IDX(unit, id, index / _BCM_IP_OPTION_PROFILE_CHUNK);

    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    L3_UNLOCK(unit);
    return rv;
}


int 
_bcm_td2_l3_ip4_options_profile_action_get(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t *action)
{
    ip_option_control_profile_table_entry_t ip_option_profile[_BCM_IP_OPTION_PROFILE_CHUNK];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[1], *entry;
    int offset;
    int copy_to_cpu, drop;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    id = ip4_options_profile_id;
    L3_LOCK(unit);

    if (!_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
        L3_UNLOCK(unit);
        return BCM_E_PARAM;
    } else {
        entries[0] = ip_option_profile;

        /* Base index of table in hardware */
        GET_HW_IDX(unit, index, id);

        index = index * _BCM_IP_OPTION_PROFILE_CHUNK;

        rv = _bcm_l3_ip4_options_profile_entry_get(unit, index, 
                                            _BCM_IP_OPTION_PROFILE_CHUNK, 
                                            entries);
        if (BCM_FAILURE(rv)) {
            L3_UNLOCK(unit);
            return (rv);
        }

        /* Offset within table */
        offset = ip4_option;

        /* Modify what's needed */
        entry = &ip_option_profile[offset];

        copy_to_cpu = soc_mem_field32_get(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                COPYTO_CPUf);
        drop = soc_mem_field32_get(unit, IP_OPTION_CONTROL_PROFILE_TABLEm, entry,
                                DROPf);

        *action = bcmIntfIPOptionActionNone;
        if ((copy_to_cpu == 0) && (drop == 0)) {
            *action = bcmIntfIPOptionActionNone;
        }
        if ((copy_to_cpu == 1) && (drop == 0)) {
            *action = bcmIntfIPOptionActionCopyToCPU;
        }
        if ((copy_to_cpu == 0) && (drop == 1)) {
            *action = bcmIntfIPOptionActionDrop;
        }
        if ((copy_to_cpu == 1) && (drop == 1)) {
            *action = bcmIntfIPOptionActionCopyCPUAndDrop;
        }
    }
    L3_UNLOCK(unit);
    return rv;
}

/* Get the list of all created IP options profile IDs */
int
_bcm_td2_l3_ip4_options_profile_multi_get(int unit, int array_size, 
                                      int *ip_options_ids_array, 
                                      int *array_count)
{
    int rv = BCM_E_NONE;
    int idx, count;

    L3_LOCK(unit);
    if (array_size == 0) {
        /* querying the number of map-ids for storage allocation */
        if (array_count == NULL) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            *array_count = 0;
            SHR_BITCOUNT_RANGE(L3_INFO(unit)->ip4_options_bitmap, count, 
                               0, _BCM_IP4_OPTIONS_LEN);
            *array_count += count;
        }
    } else {
        if ((ip_options_ids_array == NULL) || (array_count == NULL)) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            for (idx = 0; ((idx < _BCM_IP4_OPTIONS_LEN) && 
                         (count < array_size)); idx++) {
                if (_BCM_IP4_OPTIONS_USED_GET(unit, idx)) {
                    *(ip_options_ids_array + count) = idx;
                    count++;
                }
            }
            *array_count = count;
        }
    }
    L3_UNLOCK(unit);
    return rv;
}

/* Get the IP options actions for a given profile ID */
int
_bcm_td2_l3_ip4_options_profile_action_multi_get(int unit, uint32 flags,
                          int ip_option_profile_id, int array_size, 
                          bcm_l3_ip4_options_action_t *array, int *array_count)
{
    int             rv = BCM_E_NONE;
    int             num_entries, idx, id, hw_id, alloc_size, entry_size, count;
    uint8           *dma_buf;
    void            *entry;
    soc_mem_t       mem;
    bcm_l3_ip4_options_action_t   *action;
    int copy_to_cpu, drop;

    /* ignore with_id & replace flags */
    id = ip_option_profile_id;
    hw_id = 0;
    num_entries = 0;
    entry_size = 0;
    L3_LOCK(unit);

    if (!_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
        rv = BCM_E_PARAM;
    }
    num_entries = _BCM_IP_OPTION_PROFILE_CHUNK;
    entry_size = sizeof(ip_option_control_profile_table_entry_t);
    
    GET_HW_IDX(unit, hw_id, id);
    hw_id = hw_id * _BCM_IP_OPTION_PROFILE_CHUNK;
    mem = IP_OPTION_CONTROL_PROFILE_TABLEm;


    L3_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (array_size == 0) { /* querying the size of profile-chunk */
        *array_count = num_entries;
        return BCM_E_NONE;
    }
    if (!array || !array_count) {
        return BCM_E_PARAM;
    }

    /* Allocate memory for DMA & regular */
    alloc_size = num_entries * entry_size;
    dma_buf = soc_cm_salloc(unit, alloc_size, "IP option multi get DMA buf");
    if (!dma_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(dma_buf, 0, alloc_size);

    /* Read the profile chunk */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, hw_id, 
                            (hw_id + num_entries - 1), dma_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, dma_buf);
        return rv;
    }

    count = 0;
    L3_LOCK(unit);
    for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
        action = &array[idx];
        entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                             dma_buf, idx);
        copy_to_cpu = soc_mem_field32_get(unit, mem, entry, COPY_TO_CPUf);
        drop = soc_mem_field32_get(unit, mem, entry, DROPf);

        *action = bcmIntfIPOptionActionNone;
        if ((copy_to_cpu == 0) && (drop == 0)) {
            *action = bcmIntfIPOptionActionNone;
        }
        if ((copy_to_cpu == 1) && (drop == 0)) {
            *action = bcmIntfIPOptionActionCopyToCPU;
        }
        if ((copy_to_cpu == 0) && (drop == 1)) {
            *action = bcmIntfIPOptionActionDrop;
        }
        if ((copy_to_cpu == 1) && (drop == 1)) {
            *action = bcmIntfIPOptionActionCopyCPUAndDrop;
        }
        count++;
    }
    L3_UNLOCK(unit);
    soc_cm_sfree(unit, dma_buf);

    BCM_IF_ERROR_RETURN(rv);

    *array_count = count;
    return BCM_E_NONE;
}


/* Function:
*	   _bcm_td2_l3_ip4_options_profile_id2idx
* Purpose:
*	   Translate profile ID into hardware table index.
* Parameters:
* Returns:
*	   BCM_E_XXX
*/	   
int
_bcm_td2_l3_ip4_options_profile_id2idx(int unit, int profile_id, int *hw_idx)
{
    if (hw_idx == NULL) {
        return BCM_E_PARAM;
    }
    /*  Make sure module was initialized. */
    if (L3_INFO(unit)->ip4_options_bitmap == NULL) {
        return (BCM_E_INIT);
    }

    if (!_BCM_IP4_OPTIONS_USED_GET(unit, profile_id)) {
        return BCM_E_NOT_FOUND;
    } else {
        GET_HW_IDX(unit, *hw_idx, profile_id);
        *hw_idx = *hw_idx * _BCM_IP_OPTION_PROFILE_CHUNK;
    }
    return BCM_E_NONE;
}

/* Function:
*	   _bcm_td2_l3_ip4_options_profile_idx2id
* Purpose:
*	   Translate hardware table index into Profile ID used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/	   
int
_bcm_td2_l3_ip4_options_profile_idx2id(int unit, int hw_idx, int *profile_id)
{
    int id;
    int profiles_hw_idx;
    
    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }
       /* To avoid deadlock, There should be not any semaphore 
        * between HW_IDX_LOCK and HW_IDX_UNLOCK 
        */
    HW_IDX_LOCK(unit); 

    for (id = 0; id < _BCM_IP4_OPTIONS_LEN; id++) {
        if (_BCM_IP4_OPTIONS_USED_GET(unit, id)) {
            profiles_hw_idx = L3_INFO(unit)->ip4_profiles_hw_idx[id];
            if (profiles_hw_idx * _BCM_IP_OPTION_PROFILE_CHUNK == hw_idx) {
                *profile_id = id;
                HW_IDX_UNLOCK(unit); 
                return BCM_E_NONE;
            }
        }
    }
    HW_IDX_UNLOCK(unit); 
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_td2_nh_nat_id_update
 * Purpose:
 *      Routine updates the dst realm nat id of 
 *      next hop interfaces associating with the key.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      key_type - (IN) Type of the key which the nh entries associate with.
 *      val      - (IN) Value of the key. 
 *      nat_id   - (IN) Nat realm id.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_td2_nh_nat_id_update(int unit, int key_type, void *val, int nat_id)
{
    uint32 *ing_entry_ptr;        /* Ingress next hop entry pointer.*/
    uint32 *initial_ing_entry_ptr;/* Initial Ingress next hop entry pointer.*/
    char *ing_tbl_ptr = NULL;     /* Dma table pointer.             */
    char *initial_ing_tbl_ptr = NULL; /* Dma initial ing nh table pointer. */
    int ifindex;                  /* Interface index.               */
    soc_mem_t mem;                /* Next hop memory.               */
    soc_mem_t initial_mem;        /* Inital Ingress Next hop memory. */
    int idx;                      /* Iteration index.               */
    char *egr_tbl_ptr = NULL;     /* Dma egress nh table pointer.   */
    uint32 *egr_entry_ptr = NULL; /* Egress next hop entry pointer. */
    int rv = BCM_E_NONE;          /* Operation return status.       */
    uint32 *egr_intf_entry_ptr;        /* EGR_L3_INTF entry pointer. */
    char   *egr_intf_tbl_ptr = NULL;   /* EGR_L3_INTF dma table pointer.  */
    uint32 *vlan_mpls_entry_ptr;        /* VLAN_MPLS entry pointer. */
    char   *vlan_mpls_tbl_ptr = NULL;   /* VLAN_MPLS dma table pointer.  */
    bcm_vlan_t     vlan;
    bcm_if_t       l3_iif = BCM_IF_INVALID;
    _bcm_l3_intf_fields_t *fld;    /* Interface table common fields. */
    soc_field_t intf_field = INTF_NUMf;
    soc_mem_t vlan_mem = VLAN_MPLSm;
    int table_ent_size;
    soc_mem_t vfi_mem = VFIm;
    vfi_entry_t vfi_entry;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        vlan_mem = VLAN_TABm;
    }
#endif
    
    /* Get next table memory. */
    mem         = BCM_XGS3_L3_MEM(unit, nh);
    initial_mem = INITIAL_ING_L3_NEXT_HOPm;
    fld = (_bcm_l3_intf_fields_t *)BCM_XGS3_L3_MEM_FIELDS(unit, intf);

    /* Table DMA the nhtable to software copy */
    rv = bcm_xgs3_l3_tbl_dma(unit, mem, BCM_XGS3_L3_ENT_SZ(unit, nh), "nh_tbl",
                             &ing_tbl_ptr, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Get initial next table memory. */
    rv = bcm_xgs3_l3_tbl_dma(unit, initial_mem,
                             sizeof(initial_ing_l3_next_hop_entry_t),
                             "initial_nh_tbl", &initial_ing_tbl_ptr, NULL);
    if (rv < 0) {
        goto cleanup;
    }


    /*  Read egress next hop entry. */
    rv = bcm_xgs3_l3_tbl_dma(unit, EGR_L3_NEXT_HOPm,
                             sizeof(egr_l3_next_hop_entry_t), "egr_nh_tbl",
                             &egr_tbl_ptr, NULL);
    if (rv < 0) {
        goto cleanup;
    }

    if ((key_type == _BCM_NH_UPDATE_KEY_VLAN) ||
         (key_type == _BCM_NH_UPDATE_KEY_L3_IIF)) {
        /*  Read EGR_L3_INTF table. */
        rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, intf),
                                 sizeof(egr_l3_intf_entry_t), "egr_l3_intf_tbl",
                                 &egr_intf_tbl_ptr, NULL);
        if (rv < 0) {
            goto cleanup;
        }
    }

    if ((BCM_XGS3_L3_INGRESS_INTF_MAP_MODE_ISSET(unit)) &&
        (key_type == _BCM_NH_UPDATE_KEY_L3_IIF)) {
        /*  Read VLAN_MPLS table. */
        table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, vlan_mem);
        rv = bcm_xgs3_l3_tbl_dma(unit, vlan_mem,
                                 table_ent_size, "vlan_mpls_tbl",
                                 &vlan_mpls_tbl_ptr, NULL);
        if (rv < 0) {
            goto cleanup;
        }
    }
    

    for (idx = 0; idx < BCM_XGS3_L3_NH_TBL_SIZE(unit); idx++) {
        /* Skip unused entries. */
        if (!BCM_XGS3_L3_ENT_REF_CNT
                (BCM_XGS3_L3_TBL_PTR(unit, next_hop), idx)) {
            continue;
        }

        /* Skip trap to CPU entry internally installed entry. */
        if (BCM_XGS3_L3_L2CPU_NH_IDX(unit) == idx) {
            continue;
        }

        /*  Read egress next hop entry. */
        egr_entry_ptr =
            soc_mem_table_idx_to_pointer(unit, EGR_L3_NEXT_HOPm,
                                         uint32 *, egr_tbl_ptr, idx);

        /* Get interface index. */
        if (SOC_IS_TRIDENT3X(unit)) {
            intf_field = L3__INTF_NUMf;
        }
        ifindex =  soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                       egr_entry_ptr, intf_field);
        if (key_type == _BCM_NH_UPDATE_KEY_L3_INTF) {
            if (ifindex != *(bcm_if_t *)val) {
                continue;
            }
        } else if ((key_type == _BCM_NH_UPDATE_KEY_VLAN) ||
                   (key_type == _BCM_NH_UPDATE_KEY_L3_IIF)) {
            egr_intf_entry_ptr =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, intf),
                                             uint32 *, egr_intf_tbl_ptr,
                                             ifindex);
            /* coverity[forward_null : FALSE] */
            vlan = soc_mem_field32_get(unit, BCM_XGS3_L3_MEM(unit, intf),
                                       egr_intf_entry_ptr, fld->vlan_id);
            
            if (key_type == _BCM_NH_UPDATE_KEY_VLAN) {
                if (vlan != *(bcm_vlan_t *)val) {
                    continue;
                }
            } else if (key_type == _BCM_NH_UPDATE_KEY_L3_IIF) {
                if (BCM_XGS3_L3_INGRESS_INTF_MAP_MODE_ISSET(unit)) {
                    /* TD3 has both VLAN and VFI fields - check both for matching l3_iif */
                    int vfi_present = 1;
                    int vlan_present = 1;
                    if (soc_mem_field_valid(unit, BCM_XGS3_L3_MEM(unit, intf),
                         VLAN_VFI_OVERLAY_TYPEf)) {
                        int overlay_type = soc_mem_field32_get(unit,
                                             BCM_XGS3_L3_MEM(unit, intf),
                                             egr_intf_entry_ptr,
                                             VLAN_VFI_OVERLAY_TYPEf);
                        vfi_present = overlay_type;
                        vlan_present = !overlay_type;
                    }

                    if (vlan_present && BCM_VLAN_VALID(vlan)) {
                        vlan_mpls_entry_ptr =
                            soc_mem_table_idx_to_pointer(unit, vlan_mem,
                                                         uint32 *,
                                                         vlan_mpls_tbl_ptr,
                                                         vlan);
                        /* coverity[forward_null : FALSE] */
                        l3_iif = soc_mem_field32_get(unit, vlan_mem,
                                                    vlan_mpls_entry_ptr,
                                                    L3_IIFf);

                        if (l3_iif == *(bcm_if_t *)val) vfi_present = 0;
                    }
                    if (vfi_present) {
                        if (SOC_MEM_IS_VALID(unit, vfi_mem)) {
                           /* If vlan is not valid, check for VFI */
                           vlan = soc_mem_field32_get(unit, BCM_XGS3_L3_MEM(unit, intf),
                                                     egr_intf_entry_ptr, VFIf);

                           rv = soc_mem_read(unit, vfi_mem, MEM_BLOCK_ANY, vlan, &vfi_entry);
                           if (rv < 0) {
                               goto cleanup;
                           }
                           l3_iif = soc_mem_field32_get(unit, vfi_mem, &vfi_entry, L3_IIFf);
                        }
                    }
                } else {
                    l3_iif = vlan;
                }

                if (l3_iif != *(bcm_if_t *)val) {
                    continue;
                }
            }
        } else {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        /* Calculate entry ofset. */
        ing_entry_ptr =
            soc_mem_table_idx_to_pointer(unit, mem, uint32 *, ing_tbl_ptr, idx);
        soc_mem_field32_set(unit, mem, ing_entry_ptr, DST_REALM_IDf, nat_id);

        /* Initial ingress nh table */
        initial_ing_entry_ptr = soc_mem_table_idx_to_pointer(
                                    unit, initial_mem, uint32 *,
                                    initial_ing_tbl_ptr, idx);
        soc_mem_field32_set(unit, initial_mem, initial_ing_entry_ptr,
                            DST_REALM_IDf, nat_id);
    }

    /* Write the buffer back. */
    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                             soc_mem_index_min(unit, mem),
                             soc_mem_index_max(unit, mem), ing_tbl_ptr);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    
    rv = soc_mem_write_range(unit, initial_mem, MEM_BLOCK_ALL,
                             soc_mem_index_min(unit, initial_mem),
                             soc_mem_index_max(unit, initial_mem),
                             initial_ing_tbl_ptr);

cleanup:
    if (ing_tbl_ptr) {
        soc_cm_sfree(unit, ing_tbl_ptr);
    }
    
    if (initial_ing_tbl_ptr) {
        soc_cm_sfree(unit, initial_ing_tbl_ptr);
    }
    
    if (egr_tbl_ptr) {
        soc_cm_sfree(unit, egr_tbl_ptr);
    }
    
    if (egr_intf_tbl_ptr) {
        soc_cm_sfree(unit, egr_intf_tbl_ptr);
    }
    
    if (vlan_mpls_tbl_ptr) {
        soc_cm_sfree(unit, vlan_mpls_tbl_ptr);
    }    

    return rv;
}

/*
 * Function:
 *      _bcm_td2_nh_nat_id_update_by_l3_intf
 * Purpose:
 *      Routine updates the dst realm nat id of 
 *      next hop interfaces associating with the l3 egress intf.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      intf    - (IN) Interface index.
 *      nat_id  - (IN) Nat realm id.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_nh_nat_id_update_by_l3_intf(int unit, bcm_if_t intf, int nat_id)
{
    return _bcm_td2_nh_nat_id_update(unit, _BCM_NH_UPDATE_KEY_L3_INTF,
                                     (void *)&intf, nat_id);
}

/*
 * Function:
 *      _bcm_td2_nh_nat_id_update_by_l3_iif
 * Purpose:
 *      Routine updates the dst realm nat id of 
 *      next hop interfaces associating with the l3 ingress intf.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      l3_iif  - (IN) L3 ingress interface.
 *      nat_id  - (IN) Nat realm id.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_nh_nat_id_update_by_l3_iif(int unit, bcm_if_t l3_iif, int nat_id)
{
    return _bcm_td2_nh_nat_id_update(unit, _BCM_NH_UPDATE_KEY_L3_IIF,
                                     (void *)&l3_iif, nat_id);
}

/*
 * Function:
 *      _bcm_td2_nh_nat_id_update_by_vlan
 * Purpose:
 *      Routine updates the dst realm nat id of 
 *      next hop interfaces associating with the vlan.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      vlan    - (IN) Vlan id.
 *      nat_id  - (IN) Nat realm id.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_nh_nat_id_update_by_vlan(int unit, bcm_vlan_t vlan, int nat_id)
{
    return _bcm_td2_nh_nat_id_update(unit, _BCM_NH_UPDATE_KEY_VLAN,
                                     (void *)&vlan, nat_id);
}

/* 
 * Function:
 *      _bcm_td2_vp_urpf_mode_set
 * Purpose:
 *      Set VP routing urpf mode
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_vp_urpf_mode_set(int unit, bcm_port_t port, int arg) {

    int vp;
    source_vp_entry_t svp;

    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    soc_SOURCE_VPm_field32_set(unit, &svp, URPF_MODEf, (arg ? 1 : 0));
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));
    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_td2_vp_urpf_mode_get
 * Purpose:
 *      Get VP routing urpf mode
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_vp_urpf_mode_get(int unit, bcm_port_t port, int *arg) {

    int vp;
    source_vp_entry_t svp;

    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    *arg = soc_SOURCE_VPm_field32_get(unit, &svp, URPF_MODEf);
    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_td2_l3_iif_profile_get
 * Purpose:
 *      Read TD2 iif profile entry and convert to corresponding
 *      legacy vlan profile flags.
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      NONE
 */
int
_bcm_td2_l3_intf_iif_profile_get(int unit, bcm_vlan_t vid,
                                 bcm_vlan_control_vlan_t *control) {

    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/

    if (NULL == control) {
        return BCM_E_INTERNAL;
    }

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;
    BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

    control->flags |= (iif.flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) ?
                 BCM_VLAN_IP4_DISABLE: 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST ) ?
                 BCM_VLAN_IP6_DISABLE: 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST ) ?
                 BCM_VLAN_IP4_MCAST_DISABLE: 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST ) ?
                 BCM_VLAN_IP6_MCAST_DISABLE: 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU ) ?
                BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU : 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU ) ?
                BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU : 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU ) ?
                 BCM_VLAN_ICMP_REDIRECT_TOCPU : 0;
    control->flags |= (iif.flags & BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE) ?
                      BCM_VLAN_IPMC_DO_VLAN_DISABLE : 0;
    if (iif.flags & BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK) {
        control->flags &= ~BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE;
    }
    if (iif.flags & BCM_L3_INGRESS_GLOBAL_ROUTE) {
        control->flags &= ~BCM_VLAN_L3_VRF_GLOBAL_DISABLE;
    }

    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _bcm_td2_l3_iif_profile_update
 * Purpose:
 *      Update TD2 iif profile based on vlan intf flags
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      NONE
 */
int
_bcm_td2_l3_intf_iif_profile_update(int unit, bcm_vlan_t vid, 
                                    bcm_vlan_control_vlan_t *control) {

    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    /* Set default values */
    iif.flags |= BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    iif.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    iif.flags &= ~BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;
    iif.flags &= ~BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
    iif.flags &= ~BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST;
    iif.flags &= ~BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    iif.flags &= ~BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU;
    iif.flags &= ~BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU;
    iif.flags &= ~BCM_L3_INGRESS_UNKNOWN_IPMC_AS_L2MC;
    iif.flags &= ~BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU;
    iif.flags &= ~BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE;

    iif.flags |= (control->flags & BCM_VLAN_IP4_DISABLE) ?
                 BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST : 0;
    iif.flags |= (control->flags & BCM_VLAN_IP6_DISABLE) ?
                 BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST : 0;
    iif.flags |= (control->flags & BCM_VLAN_IP4_MCAST_DISABLE) ?
                 BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST : 0;
    iif.flags |= (control->flags & BCM_VLAN_IP6_MCAST_DISABLE) ?
                 BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST : 0;
    iif.flags |= (control->flags & BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU) ?
                 BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU : 0;
    iif.flags |= (control->flags & BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU) ?
                 BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU : 0;
    iif.flags |= (control->flags & BCM_VLAN_ICMP_REDIRECT_TOCPU) ?
                 BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU : 0;
    iif.flags |= (control->flags & BCM_VLAN_IPMC_DO_VLAN_DISABLE) ?
                 BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE : 0;

    if (control->flags & BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE) {
        iif.flags &= ~BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    }
    if (control->flags & BCM_VLAN_L3_VRF_GLOBAL_DISABLE) {
        iif.flags &= ~BCM_L3_INGRESS_GLOBAL_ROUTE;
    }

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_td2_l3_iif_ip4_options_profile_index_set
 * Purpose:
 *      Set the IP_OPTION_PROFILE_INDEX of the table L3_IIF.
 * Parameters:
 *      unit       - (IN) unit number
 *      intf_info  - (IN/OUT) information of the l3 ingress interface
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_iif_ip4_options_profile_index_set(int unit,
                                              _bcm_l3_intf_cfg_t *intf_info)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config. */
    int                    rv = BCM_E_NONE;
    uint32                 buf[SOC_MAX_MEM_FIELD_WORDS];
    bcm_vlan_t             vid;
    soc_mem_t              vlan_attrs_mem = VLAN_MPLSm;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        vlan_attrs_mem = VLAN_TABm;
    }
#endif

    if (!soc_feature(unit, soc_feature_l3_ip4_options_profile)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
    iif.intf_id = intf_info->l3i_vid;

    if (BCM_XGS3_L3_INGRESS_INTF_MAP_MODE_ISSET(unit)) {
        if (SOC_MEM_FIELD_VALID(unit, vlan_attrs_mem, L3_IIFf)) {
            sal_memset (buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            vid = intf_info->l3i_vid;
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vid, buf));

            if (!soc_mem_field32_get(unit, VLAN_TABm, buf, VALIDf)) {
                return BCM_E_NONE;
            }

            sal_memset (buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, vlan_attrs_mem, MEM_BLOCK_ANY, vid, buf));
            iif.intf_id = soc_mem_field32_get(unit, vlan_attrs_mem,
                                              buf, L3_IIFf);
        }
    }

    soc_mem_lock(unit, L3_IIFm);

    rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, L3_IIFm);
        return rv;
    }

    iif.profile_flags |= _BCM_L3_IIF_PROFILE_DO_NOT_UPDATE;
    iif.ip4_options_profile_id = intf_info->l3i_ip4_options_profile_id;
    rv = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);

    soc_mem_unlock(unit, L3_IIFm);
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l3_iif_ip4_options_profile_index_get
 * Purpose:
 *      Get the IP_OPTION_PROFILE_INDEX of the table L3_IIF.
 * Parameters:
 *      unit      - (IN) unit number
 *      intf_info - (IN/OUT) information of the l3 ingress interface
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_iif_ip4_options_profile_index_get(int unit,
                                              _bcm_l3_intf_cfg_t *intf_info)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config. */
    int                    rv = BCM_E_NONE;
    vlan_tab_entry_t       vt;
    uint32                 buf[SOC_MAX_MEM_FIELD_WORDS];
    bcm_vlan_t             vid;
    soc_mem_t              vlan_attrs_mem = VLAN_MPLSm;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        vlan_attrs_mem = VLAN_TABm;
    }
#endif


    if (!soc_feature(unit, soc_feature_l3_ip4_options_profile)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
    iif.intf_id = intf_info->l3i_vid;

    if (BCM_XGS3_L3_INGRESS_INTF_MAP_MODE_ISSET(unit)) {
        if (SOC_MEM_FIELD_VALID(unit, vlan_attrs_mem, L3_IIFf)) {
            vid = intf_info->l3i_vid;
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vid, &vt));

            if (!soc_mem_field32_get(unit, VLAN_TABm, &vt, VALIDf)) {
                return BCM_E_NONE;
            }

            sal_memset (buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, vlan_attrs_mem, MEM_BLOCK_ANY, vid, buf));
            iif.intf_id = soc_mem_field32_get(unit, vlan_attrs_mem,
                                              buf, L3_IIFf);
        }
    }

    rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
    intf_info->l3i_ip4_options_profile_id = iif.ip4_options_profile_id;

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/* 
 * Function:
 *      _bcm_td2_l3_ip4_options_profile_sw_dump
 * Purpose:
 *      Displays IP oprion profile software state info
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      NONE
 */
void
_bcm_td2_l3_ip4_options_profile_sw_dump(int unit)
{
    int i;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_CLI((BSL_META_U(unit,
                            "ERROR: L3  module not initialized on Unit:%d \n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "L3 IP Option: IP_OPTION_CONTROL_PROFILE_TABLEm info \n")));
    for (i = 0; i < _BCM_IP4_OPTIONS_LEN; i++) {
        if (_BCM_IP4_OPTIONS_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    Profile id:%4d    HW index:%4d\n"), i, 
                     L3_INFO(unit)->ip4_profiles_hw_idx[i]));
        }
    }

}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT


/* SCACHE size for WB */
int 
_bcm_td2_l3_ip4_options_profile_scache_len_get(int unit, int *wb_alloc_size)
{
    if (wb_alloc_size == NULL) {
        return BCM_E_PARAM;
    }
    *wb_alloc_size = sizeof(uint32) * _BCM_IP4_OPTIONS_LEN;

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_td2_l3_ip4_options_profile_reinit_hw_profiles_update
 * Purpose:
 *      Updates the shared memory profile tables reference counts
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_l3_ip4_options_profile_reinit_hw_profiles_update (int unit)
{
    int     i;
    for (i=0; i < _BCM_IP4_OPTIONS_LEN; i++) {
        if (_BCM_IP4_OPTIONS_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_l3_ip4_options_profile_entry_reference(unit, 
                              ((L3_INFO(unit)->ip4_profiles_hw_idx[i]) * 
                               _BCM_IP_OPTION_PROFILE_CHUNK), 
                               _BCM_IP_OPTION_PROFILE_CHUNK));
        }
    }
    return BCM_E_NONE;
}



/* 
 * Function:
 *      _bcm_td2_l3_ip4_options_recover
 * Purpose:
 *      Recover IP Option Profile usage
 * Parameters:
 *      unit    : (IN) Device Unit Number
 *      scache_ptr - (IN) Scache pointer
 *                   (OUT) Updated Scache pointer
 *      offset - (IN) Scache pointer offset
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_ip4_options_recover(int unit, uint8 **scache_ptr, int offset)
{
    uint32 hw_idx;
    int    idx;
    int    rv = BCM_E_NONE;
    int    stable_size = 0;
    SOC_SCACHE_DUMP_DECL;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

        if (stable_size == 0) { /* level 1 */
            /* Nothing to recover */
        } else {  /* Level 2 */
            SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                                  *scache_ptr - offset);

            SOC_SCACHE_DUMP_DATA_BEGIN("idx;l3_ip4_option_profile_hw_idx");
            /* recover from scache into book-keeping structs */
            for (idx = 0; idx < _BCM_IP4_OPTIONS_LEN; idx++) {
                sal_memcpy(&hw_idx, (*scache_ptr), sizeof(uint32));
                SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), idx, uint32);
                (*scache_ptr) += sizeof(hw_idx);
                if (hw_idx != _BCM_IP_OPTION_REINIT_INVALID_HW_IDX) {
                    _BCM_IP4_OPTIONS_USED_SET(unit, idx);
                    L3_INFO(unit)->ip4_profiles_hw_idx[idx] = hw_idx;
                }
            }
            SOC_SCACHE_DUMP_DATA_END();

            SOC_SCACHE_DUMP_STOP();

            rv = _bcm_td2_l3_ip4_options_profile_reinit_hw_profiles_update(unit);
        }
    }


    return rv;
}

/* 
 * Function:
 *      _bcm_td2_l3_ip4_options_sync
 * Purpose:
 *      Store IP Option Profile usage table into scache.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 *      scache_ptr - (IN) Scache pointer
 *                   (OUT) Updated Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_l3_ip4_options_sync(int unit, uint8 **scache_ptr, int offset)
{
    uint32 hw_idx;
    int    idx;
    SOC_SCACHE_DUMP_DECL;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);

    SOC_SCACHE_DUMP_DATA_BEGIN("idx;l3_ip4_option_profile_hw_idx");
    /* now store the state into the compressed format */
    for (idx = 0; idx < _BCM_IP4_OPTIONS_LEN; idx++) {
        if (_BCM_IP4_OPTIONS_USED_GET(unit, idx)) {
            hw_idx = L3_INFO(unit)->ip4_profiles_hw_idx[idx];
        } else {
            hw_idx = _BCM_IP_OPTION_REINIT_INVALID_HW_IDX;
        }
        sal_memcpy((*scache_ptr), &hw_idx, sizeof(uint32));
        SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), idx, uint32);
        (*scache_ptr) += sizeof(hw_idx);
    }
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_STOP();

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef ALPM_ENABLE
/* ALPM Functions */


/*
 * Function:
 *      _bcm_td2_alpm_lpm_vrf_init
 * Purpose:
 *      Service routine used to translate API vrf id to hw specific.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      vrf_id    - (OUT)Internal vrf id.
 *      vrf_mask  - (OUT)Internal vrf mask.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_alpm_lpm_vrf_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                           int *vrf_id, int *vrf_mask)
{
    /* Handle special vrf id cases. */
    switch (lpm_cfg->defip_vrf) {
      case BCM_L3_VRF_OVERRIDE:
      case BCM_L3_VRF_GLOBAL: /* only for td2 */
          *vrf_id = 0;
          *vrf_mask = 0;
          break;
      default:
          *vrf_id = lpm_cfg->defip_vrf;
          *vrf_mask = SOC_VRF_MAX(unit);
          break;
    }

    /* In any case vrf id shouldn't exceed max field mask. */
    if ((*vrf_id < 0) || (*vrf_id > SOC_VRF_MAX(unit))) {
        return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}


/*
 * Function:	
 *     _bcm_td2_alpm_mem_ip6_defip_set
 * Purpose:
 *    Set IP6 address mask field in L3_DEFIPm from defip_cfg
 * Parameters: 
 *    unit    - (IN) SOC unit number; 
 *    lpm_key - (OUT) Buffer to fill. 
 *    lpm_cfg - (IN) Route information. 
 *    alpm_entry - (OUT) ALPM entry to fill
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td2_alpm_lpm64_key_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             defip_entry_t *lpm_key)
{
    bcm_ip6_t mask;                      /* Subnet mask.        */
    uint8 *ip6;                 /* Ip6 address.       */
    uint32 ip6_word[2];         /* Temp storage.      */
    int idx;                    /* Iteration index .  */

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply subnet mask */
    idx = lpm_cfg->defip_sub_len / 8;   /* Unchanged byte count.    */
    ip6[idx] &= mask[idx];      /* Apply mask on next byte. */
    for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
        ip6[idx] = 0;           /* Reset rest of bytes.     */
    }

    ip6_word[1] = ((ip6[0] << 24) | (ip6[1] << 16) | (ip6[2] << 8) | (ip6[3]));
    ip6_word[0]= ((ip6[4] << 24) | (ip6[5] << 16) | (ip6[6] << 8) | (ip6[7]));

    /* Set IP Addr */
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR1f, (void *)&ip6_word[1]);
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR0f, (void *)&ip6_word[0]);

    /* Set IP MASK */
    ip6_word[0] = ((mask[0] << 24) | (mask[1] << 16) | (mask[2] << 8) | (mask[3]));
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR_MASK1f, (void *)&ip6_word[0]);

    ip6_word[0] = ((mask[4] << 24) | (mask[5] << 16) | (mask[6] << 8) | (mask[7]));
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR_MASK0f, (void *)&ip6_word[0]);
}


/*
 * Function:
 *      _bcm_td2_alpm_lpm_ent_init
 * Purpose:
 *      Service routine used to initialize lkup key for lpm entry.
 *      Also initializes the ALPM SRAM entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      lpm_entry - (OUT)Hw buffer to fill.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_alpm_lpm_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                           defip_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    bcm_ip_t ip4_mask;
    int vrf_id;
    int vrf_mask;
    int ipv6;

    ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_alpm_lpm_vrf_init(unit, lpm_cfg, &vrf_id, &vrf_mask));


    /* Zero buffers. */
    sal_memset(lpm_entry, 0, BCM_XGS3_L3_ENT_SZ(unit, defip));

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, HIT0f, 1);
    }

    /* Set priority override bit. */
    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, RPE0f, 1);
    }

    /* Write priority field. */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, PRI0f, lpm_cfg->defip_prio);

    /* Fill next hop information. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_NEXTHOP;
        uint32 dest_val  = 0;
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            dest_type = SOC_MEM_FIF_DEST_ECMP;
            dest_val  = nh_ecmp_idx;
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            dest_type = SOC_MEM_FIF_DEST_NEXTHOP;
            dest_val  = nh_ecmp_idx;
        } else {
            dest_type = SOC_MEM_FIF_DEST_INVALID;
        }

        if (dest_type != SOC_MEM_FIF_DEST_INVALID) {
            soc_mem_field32_dest_set(unit, L3_DEFIPm, lpm_entry, DESTINATION0f,
                                     dest_type, dest_val);
        }
    } else {
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, ECMP0f, 1);
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, NEXT_HOP_INDEX0f,
                                      nh_ecmp_idx);
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, NEXT_HOP_INDEX0f,
                                        nh_ecmp_idx);
        }
    }

    /* Set destination discard flag. */
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, DST_DISCARD0f, 1);
    }

    /* remember src discard flag */
    if (lpm_cfg->defip_flags & BCM_L3_SRC_DISCARD) {
        *flags |= SOC_ALPM_RPF_SRC_DISCARD;
    }

    if (soc_feature(unit, soc_feature_alpm_flex_stat)) {
        *flags |= SOC_ALPM_STAT_FLEX;
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f, 0x1);
    }

    /* Set classification group id. */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, CLASS_ID0f, 
                              lpm_cfg->defip_lookup_class);

    /* Set Global route flag. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 0x1);
    }

    /* Indicate this is an override entry */
    if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_HIGH0f, 0x1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 0x1);
    }

    /* Set VRF */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_0f, vrf_id);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_MASK0f, vrf_mask);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_defip) &&
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        if (soc_mem_field_valid(unit, L3_DEFIPm, MULTICAST_ROUTE0f)) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, MULTICAST_ROUTE0f, 1);
        } else if (soc_mem_field_valid(unit, L3_DEFIPm, DATA_TYPE0f)) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, DATA_TYPE0f, 2);
        }

        if (soc_feature(unit, soc_feature_generic_dest)) {
            if (lpm_cfg->defip_mc_group > 0) {
                soc_mem_field32_dest_set(unit, L3_DEFIPm, lpm_entry, DESTINATION0f,
                    SOC_MEM_FIF_DEST_IPMC, lpm_cfg->defip_mc_group);
            }
        } else {
            soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, L3MC_INDEX0f, lpm_cfg->defip_mc_group);
        }

        /*
         * RP ID needs uplift by _BCM_L3_DEFIP_RP_ID_BASE value, Now it wil fit in EXPECTED_L3_IIFf
         */
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, EXPECTED_L3_IIF0f, 
                                    _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));
        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) && 
                (lpm_cfg->defip_expected_intf != 0))  {

            soc_L3_DEFIPm_field32_set(unit, lpm_entry, EXPECTED_L3_IIF0f, lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_L3_DEFIPm_field32_set(unit, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_L3_DEFIPm_field32_set(unit, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, 1);
            }
        }
    }
#endif

    if (ipv6) {
        /* Set prefix ip address & mask. */
        _bcm_td2_alpm_lpm64_key_init(unit, lpm_cfg,lpm_entry);

        /* Set second part valid bit. */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 1);

        /* Set mode to ipv6 */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, MODE0_f(unit), 1);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, MODE1_f(unit), 1);

        /* Set Virtual Router id */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_1f, vrf_id);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_MASK1f, vrf_mask);
    } else {
        ip4_mask = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        /* Apply subnet mask. */
        lpm_cfg->defip_ip_addr &= ip4_mask;

        /* Set address to the buffer. */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, IP_ADDR0f,
                                  lpm_cfg->defip_ip_addr);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, IP_ADDR_MASK0f, ip4_mask);
    }
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID0f, 1);

    /* Set Mode Masks */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE_MASK0_f(unit),
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK0_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE_MASK1_f(unit),
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK1_f(unit))) - 1);

    /* Set Entry Type Masks */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE0_f(unit),
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, ENTRY_TYPE0_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE1_f(unit),
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, ENTRY_TYPE1_f(unit))) - 1);
    /*
     * Note ipv4 entries are expected to reside in part 0 of the entry.
     */

    /* apply for ALPM and Global high */
    /* Base counter index. */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_BASE_COUNTER_IDX0f,
                        lpm_cfg->defip_flex_ctr_base_id);
    /* Counter offset mode. */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_OFFSET_MODE0f,
                        lpm_cfg->defip_flex_ctr_mode);
    /* Counter pool number. */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, FLEX_CTR_POOL_NUMBER0f,
                        lpm_cfg->defip_flex_ctr_pool);
    return (BCM_E_NONE);
}

/*
 * Function:    
 *     _bcm_td2_alpm_lpm128_addr_init
 * Purpose:  
 *     Set IP6 address field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_td2_alpm_lpm128_addr_init(int unit, soc_mem_t mem, uint32 *entry,
                               const ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR0_LWRf, &ip6_field[3]);
    ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                    (ip6[10] << 8) | (ip6[11] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR1_LWRf, &ip6_field[2]);
    ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                    (ip6[6] << 8)  | (ip6[7] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR0_UPRf, &ip6_field[1]);
    ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                    (ip6[2] << 8)  | (ip6[3] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR1_UPRf, ip6_field);
}

/*
 * Function:    
 *     _bcm_td2_alpm_lpm128_mask_init
 * Purpose:  
 *     Set IP6 mask field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_td2_alpm_lpm128_mask_init(int unit, soc_mem_t mem, uint32 *entry,
                               const ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK0_LWRf, &ip6_field[3]);
    ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                    (ip6[10] << 8) | (ip6[11] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK1_LWRf, &ip6_field[2]);
    ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                    (ip6[6] << 8)  | (ip6[7] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK0_UPRf, &ip6_field[1]);
    ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                    (ip6[2] << 8)  | (ip6[3] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK1_UPRf, ip6_field);
}

STATIC int
_bcm_td2_alpm_lpm128_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
             defip_pair_128_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    int vrf_id;
    int vrf_mask;
    bcm_ip6_t mask;                          /* Subnet mask.              */

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_alpm_lpm_vrf_init(unit, lpm_cfg, &vrf_id, &vrf_mask));

    /* Zero buffers. */
    sal_memset(lpm_entry, 0, sizeof(*lpm_entry));

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, HITf, 1);
    }

    /* Set priority override bit. */
    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, RPEf, 1);
    }

    /* Write priority field. */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, PRIf, lpm_cfg->defip_prio);

    /* Fill next hop information. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_NEXTHOP;
        uint32 dest_val  = 0;
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            dest_type = SOC_MEM_FIF_DEST_ECMP;
            dest_val  = nh_ecmp_idx;
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            dest_type = SOC_MEM_FIF_DEST_NEXTHOP;
            dest_val  = nh_ecmp_idx;
        } else {
            dest_type = SOC_MEM_FIF_DEST_INVALID;
        }

        if (dest_type != SOC_MEM_FIF_DEST_INVALID) {
            soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                     DESTINATIONf, dest_type, dest_val);
        }
    } else {
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, ECMPf, 1);
            soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, NEXT_HOP_INDEXf,
                                      nh_ecmp_idx);
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, NEXT_HOP_INDEXf,
                                                nh_ecmp_idx);
        }
    }

    /* Set destination discard flag. */
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, DST_DISCARDf, 1);
    }

    /* remember src discard flag */
    if (lpm_cfg->defip_flags & BCM_L3_SRC_DISCARD) {
        *flags |= SOC_ALPM_RPF_SRC_DISCARD;
    }

    if (soc_feature(unit, soc_feature_alpm_flex_stat)) {
        *flags |= SOC_ALPM_STAT_FLEX;
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_VIEWf, 0x1);
    }

    /* Set classification group id. */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, CLASS_IDf, 
                              lpm_cfg->defip_lookup_class);

    /* Set Global route flag. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_ROUTEf, 0x1);
    }

    /* Indicate this is an override entry */
    if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_HIGHf, 0x1);
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_ROUTEf, 0x1);
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_defip) &&
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MULTICAST_ROUTEf, 1);
        } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, DATA_TYPEf, 2);
        }

        if (soc_feature(unit, soc_feature_generic_dest)) {
            if (lpm_cfg->defip_mc_group > 0) {
                soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                         DESTINATIONf, SOC_MEM_FIF_DEST_IPMC,
                                         lpm_cfg->defip_mc_group);
            }
        } else {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, L3MC_INDEXf, lpm_cfg->defip_mc_group);
        }

        /*
         * RP ID needs uplift by _BCM_L3_DEFIP_RP_ID_BASE value, Now it wil fit in EXPECTED_L3_IIFf
         */
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                  EXPECTED_L3_IIFf, _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));
        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) &&
                (lpm_cfg->defip_expected_intf != 0))  {

            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, EXPECTED_L3_IIFf, lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, 1);
            }
        }
    }
#endif

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Set prefix ip address & mask. */
    _bcm_td2_alpm_lpm128_addr_init(unit, L3_DEFIP_PAIR_128m,
                                (uint32 *) lpm_entry, lpm_cfg->defip_ip6_addr);

    _bcm_td2_alpm_lpm128_mask_init(unit, L3_DEFIP_PAIR_128m,
                                   (uint32 *) lpm_entry, mask);

    /* Set Virtual Router id */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_0_LWRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_1_LWRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_0_UPRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_1_UPRf, vrf_id);

    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK0_LWRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK1_LWRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK0_UPRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK1_UPRf, 
                                       vrf_mask);

    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID0_LWRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID1_LWRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID0_UPRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID1_UPRf, 1);

    /* Set mode to ipv6-128 */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE0_LWR_f(unit), 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE1_LWR_f(unit), 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE0_UPR_f(unit), 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE1_UPR_f(unit), 3);

    /* Set Mode Masks */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK0_LWR_f(unit),
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK0_LWR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK1_LWR_f(unit),
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK1_LWR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK0_UPR_f(unit),
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK0_UPR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK1_UPR_f(unit),
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK1_UPR_f(unit))) - 1);

    /* Set Entry Type Masks */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE0_LWR_f(unit),
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE0_LWR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE1_LWR_f(unit),
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE1_LWR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE0_UPR_f(unit),
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE0_UPR_f(unit))) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE1_UPR_f(unit),
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE1_UPR_f(unit))) - 1);

    /* Base counter index. */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, FLEX_CTR_BASE_COUNTER_IDXf,
                        lpm_cfg->defip_flex_ctr_base_id);
    /* Counter offset mode. */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, FLEX_CTR_OFFSET_MODEf,
                        lpm_cfg->defip_flex_ctr_mode);
    /* Counter pool number. */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, FLEX_CTR_POOL_NUMBERf,
                        lpm_cfg->defip_flex_ctr_pool);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_td2_alpm_lpm64_key_parse
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td2_alpm_lpm64_key_parse(int unit, const void *lpm_key,
                              _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word;            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR1f, &ip6_word);
    ip6[0] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR0f, &ip6_word);
    ip6[4] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR_MASK1f, &ip6_word);
    mask[0] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[1] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[2] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[3] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR_MASK0f, &ip6_word);
    mask[4] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[5] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[6] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[7] = (uint8) (ip6_word & 0xff);

    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);
}

/*
 * Function:
 *      _bcm_td2_alpm_lpm_key_parse
 * Purpose:
 *      Parse entry key from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information. 
 *      lpm_entry   - (IN) Buffer read from hw. 
 * Returns:
 *      void
 */
STATIC void
_bcm_td2_alpm_lpm_key_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            defip_entry_t *lpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_td2_alpm_lpm64_key_parse(unit, lpm_entry, lpm_cfg);
    } else {
        /* Get ipv4 address. */
        lpm_cfg->defip_ip_addr =
            soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR0f);

        /* Get subnet mask. */
        v4_mask = soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR_MASK0f);

        /* Fill mask length. */
        lpm_cfg->defip_sub_len = bcm_ip_mask_length(v4_mask);
    }

    /* Get Virtual Router id */
    soc_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, NULL);
    return;
}


/*
 * Function:
 *      _bcm_td2_alpm_lpm_data_parse
 * Purpose:
 *      Parse an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      nh_ecmp_idx - (OUT)Next hop index or ecmp group id.
 *      lpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
STATIC INLINE void
_bcm_td2_alpm_lpm_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg, int *p_nh_ecmp_idx,
                             defip_entry_t *lpm_entry)
{
    int is_ecmp = 0;
    uint32 nh_ecmp_idx = 0;
    int ipv6 = soc_L3_DEFIPm_field32_get(unit, lpm_entry, MODE0_f(unit));

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
        nh_ecmp_idx = soc_mem_field32_dest_get(unit, L3_DEFIPm, lpm_entry,
                                               DESTINATION0f, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
            is_ecmp = 1;
        } else if (dest_type != SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_ecmp_idx = 0;
        }
    } else {
        is_ecmp = soc_L3_DEFIPm_field32_get(unit, lpm_entry, ECMP0f);
        if (is_ecmp) {
            nh_ecmp_idx = soc_L3_DEFIPm_field32_get(unit, lpm_entry, ECMP_PTR0f);
        } else {
            nh_ecmp_idx = soc_L3_DEFIPm_field32_get(unit, lpm_entry, NEXT_HOP_INDEX0f);
        }
    }

    if (is_ecmp) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    }
    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_L3_DEFIPm_field32_get(unit, lpm_entry, PRI0f);

    /* Get hit bit. */
    if (!ALPM_CTRL(unit).hit_bits_skip &&
        soc_L3_DEFIPm_field32_get(unit, lpm_entry, HIT0f)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, RPE0f)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, DST_DISCARD0f)) {
        if(soc_L3_DEFIPm_field32_get(unit, lpm_entry, DST_DISCARD0f)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

#if defined(BCM_TRX_SUPPORT)
    /* Set classification group id. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, CLASS_ID0f)) {
        lpm_cfg->defip_lookup_class = 
            soc_L3_DEFIPm_field32_get(unit, lpm_entry, CLASS_ID0f);
    }
#endif /* BCM_TRX_SUPPORT */


    if (ipv6) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
        /* Get hit bit from the second part of the entry. */
        if (!ALPM_CTRL(unit).hit_bits_skip &&
            soc_L3_DEFIPm_field32_get(unit, lpm_entry, HIT1f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }

        /* Get priority override bit from the second part of the entry. */
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, RPE1f)) {
            lpm_cfg->defip_flags |= BCM_L3_RPE;
        }
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        int ipmc_route = 0;
        if (soc_mem_field_valid(unit, L3_DEFIPm, MULTICAST_ROUTE0f)) {
            ipmc_route = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MULTICAST_ROUTE0f);
        } else if (soc_mem_field_valid(unit, L3_DEFIPm, DATA_TYPE0f)) {
            if (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, DATA_TYPE0f) == 2) {
                ipmc_route = 1;
            } else {
                ipmc_route = 0;
            }
        }

        if (ipmc_route) {
            int val;

            lpm_cfg->defip_flags |= BCM_L3_IPMC;
            if (p_nh_ecmp_idx) {
                *p_nh_ecmp_idx = BCM_XGS3_L3_INVALID_INDEX;
            }
            if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
            /*
             * RP ID and ExpectedIIF are overlaid.
             */
            val = soc_L3_DEFIPm_field32_get(unit, lpm_entry, EXPECTED_L3_IIF0f);

            if (soc_feature(unit, soc_feature_l3defip_rp_l3iif_resolve)) {
                int rpa_id_mask = (1 << soc_mem_field_length(unit, L3_DEFIPm, RPA_ID0f)) - 1;
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
                        L3_DEFIPm, lpm_entry,
                        DESTINATION0f, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                    lpm_cfg->defip_mc_group = dest_value;
                } else {
                    lpm_cfg->defip_mc_group = 0;
                }
            } else {
                lpm_cfg->defip_mc_group = soc_mem_field32_get(unit, L3_DEFIPm,
                                                lpm_entry, L3MC_INDEX0f);
            }
        }
    }
#endif


    /* Read base counter index. */
    lpm_cfg->defip_flex_ctr_base_id =
        soc_L3_DEFIPm_field32_get(unit, lpm_entry, FLEX_CTR_BASE_COUNTER_IDX0f);
    /* Read counter offset mode. */
    lpm_cfg->defip_flex_ctr_mode =
        soc_L3_DEFIPm_field32_get(unit, lpm_entry, FLEX_CTR_OFFSET_MODE0f);
    /* Read counter pool number. */
    lpm_cfg->defip_flex_ctr_pool =
        soc_L3_DEFIPm_field32_get(unit, lpm_entry, FLEX_CTR_POOL_NUMBER0f);

    return;
}


/*
 * Function:
 *     _bcm_td2_alpm_lpm128_mask_parse
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
_bcm_td2_alpm_lpm128_mask_parse(int unit, soc_mem_t mem, const void *entry,
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK0_LWRf, (uint32 *)&ip6_field[3]);
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK1_LWRf, (uint32 *)&ip6_field[2]);
    ip6[8] = (uint8) (ip6_field[2] >> 24);
    ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK0_UPRf, (uint32 *)&ip6_field[1]);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] =(uint8) (ip6_field[1] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK1_UPRf, ip6_field);
    ip6[0] =(uint8) (ip6_field[0] >> 24);
    ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] =(uint8) (ip6_field[0] & 0xff);
}

/*
 * Function:
 *     _bcm_td2_alpm_lpm128_addr_parse
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
_bcm_td2_alpm_lpm128_addr_parse(int unit, soc_mem_t mem, const void *entry,
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    soc_mem_field_get(unit, mem, entry, IP_ADDR0_LWRf, (uint32 *)&ip6_field[3]);
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR1_LWRf, (uint32 *)&ip6_field[2]);
    ip6[8] = (uint8) (ip6_field[2] >> 24);
    ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR0_UPRf, (uint32 *)&ip6_field[1]);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] =(uint8) (ip6_field[1] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR1_UPRf, ip6_field);
    ip6[0] =(uint8) (ip6_field[0] >> 24);
    ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] =(uint8) (ip6_field[0] & 0xff);
}

/*
 * Function:
 *      _bcm_td2_alpm_lpm128_key_parse
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
_bcm_td2_alpm_lpm128_key_parse(int unit, uint32 *hw_entry,
                               _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;  /* Route table memory. */
    bcm_ip6_t mask;                      /* Subnet mask.        */

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Extract ip6 address. */
    _bcm_td2_alpm_lpm128_addr_parse(unit,mem, hw_entry,
                                    lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _bcm_td2_alpm_lpm128_mask_parse(unit, mem, hw_entry, mask);
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if(!soc_mem_field32_get(unit, mem, hw_entry, VRF_ID_MASK0_LWRf)) {
        lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        lpm_cfg->defip_vrf = soc_mem_field32_get(unit, mem, hw_entry,
                                                 VRF_ID_0_LWRf);
    }
    return (BCM_E_NONE);
}


STATIC INLINE void
_bcm_td2_alpm_lpm128_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                int *p_nh_ecmp_idx, defip_pair_128_entry_t *lpm_entry)
{
    int is_ecmp = 0;
    uint32 nh_ecmp_idx = 0;
    soc_mem_t mem = L3_DEFIP_PAIR_128m;
    int ipv6 = soc_mem_field32_get(unit, mem, lpm_entry, MODE0_LWR_f(unit));

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
        nh_ecmp_idx = soc_mem_field32_dest_get(unit, mem, lpm_entry,
                                               DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
            is_ecmp = 1;
        } else if (dest_type != SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_ecmp_idx = 0;
        }
    } else {
        is_ecmp = soc_mem_field32_get(unit, mem, lpm_entry, ECMPf);
        if (is_ecmp) {
            nh_ecmp_idx = soc_mem_field32_get(unit, mem, lpm_entry, ECMP_PTRf);
        } else {
            nh_ecmp_idx = soc_mem_field32_get(unit, mem, lpm_entry, NEXT_HOP_INDEXf);
        }
    }

    if (is_ecmp) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    }
    /* Get entry priority. */
    lpm_cfg->defip_prio = 
                    soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, PRIf);

    /* Get hit bit. */
    if (!ALPM_CTRL(unit).hit_bits_skip &&
        soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, HITf)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, RPEf)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIP_PAIR_128m, DST_DISCARDf)) {
        if(soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, DST_DISCARDf)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

#if defined(BCM_TRX_SUPPORT)
    /* Set classification group id. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIP_PAIR_128m, CLASS_IDf)) {
        lpm_cfg->defip_lookup_class = 
            soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, CLASS_IDf);
    }
#endif /* BCM_TRX_SUPPORT */

    if (ipv6) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
        /* Get hit bit from the second part of the entry. */
        if (!ALPM_CTRL(unit).hit_bits_skip &&
            soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, HITf)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }

        /* Get priority override bit from the second part of the entry. */
        if (soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, RPEf)) {
            lpm_cfg->defip_flags |= BCM_L3_RPE;
        }
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        int is_ipmc = 0;

        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
            is_ipmc = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                          lpm_entry, MULTICAST_ROUTEf);
        } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
            if (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                    lpm_entry, DATA_TYPEf) == 2) {
                is_ipmc = 1;
            } else {
                is_ipmc = 0;
            }
        }

        if (is_ipmc) {
            int val;
            lpm_cfg->defip_flags |= BCM_L3_IPMC;

            if (p_nh_ecmp_idx) {
                *p_nh_ecmp_idx = BCM_XGS3_L3_INVALID_INDEX;
            }
            if (soc_L3_DEFIP_PAIR_128m_field32_get(unit,
                        lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (soc_L3_DEFIP_PAIR_128m_field32_get(unit,
                        lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
            /*
             * RP ID and ExpectedIIF are overlaid.
             */
            val = soc_L3_DEFIP_PAIR_128m_field32_get(unit,
                                                   lpm_entry, EXPECTED_L3_IIFf);
            if (soc_feature(unit, soc_feature_l3defip_rp_l3iif_resolve)) {
                int rpa_id_mask = (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, RPA_IDf)) - 1;
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
                dest_value = soc_mem_field32_dest_get(unit, mem, lpm_entry,
                                                       DESTINATIONf, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                    lpm_cfg->defip_mc_group = dest_value;
                } else {
                    lpm_cfg->defip_mc_group = 0;
                }
            } else {
                lpm_cfg->defip_mc_group = soc_L3_DEFIP_PAIR_128m_field32_get(unit,
                                                lpm_entry, L3MC_INDEXf);
            }
        }
    }
#endif


    /* Read base counter index. */
    lpm_cfg->defip_flex_ctr_base_id =
        soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, FLEX_CTR_BASE_COUNTER_IDXf);
    /* Read counter offset mode. */
    lpm_cfg->defip_flex_ctr_mode =
        soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, FLEX_CTR_OFFSET_MODEf);
    /* Read counter pool number. */
    lpm_cfg->defip_flex_ctr_pool =
        soc_L3_DEFIP_PAIR_128m_field32_get(unit, lpm_entry, FLEX_CTR_POOL_NUMBERf);

    return;
}

/*
 * Function:
 *      _bcm_td2_alpm_l3_defip_mem_get
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
_bcm_td2_alpm_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
    soc_mem_t defip_mem = L3_DEFIPm;
    soc_mem_t defip_pair_mem = L3_DEFIP_PAIR_128m;

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
        defip_mem = L3_DEFIP_LEVEL1m;
        defip_pair_mem = L3_DEFIP_PAIR_LEVEL1m;
    }

    /* Default value */
    if (mem) {
        *mem = defip_mem;
    }
    if ((flags & BCM_L3_IP6)) {
        if (soc_mem_index_count(unit, defip_pair_mem) > 0) {
            if (mem) {
                *mem = defip_pair_mem;
            }
            return BCM_E_NONE;
        }
        /* no v6-128 config. allow only v6-64 */
        if (plen > 64) {
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "Cannot allow prefix length > 64 in "
                                  "%s\n"), SOC_MEM_NAME(unit, defip_mem)));
            return BCM_E_PARAM;
        }
    } else if (BCM_XGS3_L3_DEFIP_TBL_SIZE(unit) == 0) {
        /* no l3_defip. cannot insert ipv4 */
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "Cannot allow ipv4 routes in "
                              "%s\n"), SOC_MEM_NAME(unit, defip_pair_mem)));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_alpm_128_clear_hit
 * Purpose:
 *      Clear prefix entry hit bit.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      key_data  - (IN) LPM key_data.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_alpm_128_clear_hit(int u, void *key_data, int index, int flex)
{
    soc_mem_t   mem;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    mem = L3_DEFIP_ALPM_IPV6_128m;

    BCM_IF_ERROR_RETURN(
        soc_mem_read(u, mem, MEM_BLOCK_ANY, index, e));
    if (SOC_MEM_FIELD_VALID(u, mem, HITf)) {
        /*
         * Clear the hit bit anyway, instead of reading at most twice (from X
         * pipe and Y pipe) from H/W and check the hit status.
         */
        soc_mem_field32_set(u, mem, e, HITf, 0);
        BCM_IF_ERROR_RETURN(soc_mem_write(u, mem, MEM_BLOCK_ALL, index, e));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_alpm_clear_hit
 * Purpose:
 *      Clear prefix entry hit bit.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      key_data  - (IN) LPM key_data.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_alpm_clear_hit(int u, void *key_data, int index, int flex)
{
    int         ipv6;
    soc_mem_t   mem;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    ipv6 = soc_mem_field32_get(u, L3_DEFIPm, key_data, MODE0_f(u));

    mem = (ipv6) ? (flex ? L3_DEFIP_ALPM_IPV6_64_1m : L3_DEFIP_ALPM_IPV6_64m) :
                   (flex ? L3_DEFIP_ALPM_IPV4_1m : L3_DEFIP_ALPM_IPV4m);

    BCM_IF_ERROR_RETURN(soc_mem_read(u, mem, MEM_BLOCK_ANY, index, e));
    if (SOC_MEM_FIELD_VALID(u, mem, HITf)) {
        /*
         * Clear the hit bit anyway, instead of reading at most twice (from X
         * pipe and Y pipe) from H/W and check the hit status.
         */
        soc_mem_field32_set(u, mem, e, HITf, 0);
        BCM_IF_ERROR_RETURN(soc_mem_write(u, mem, MEM_BLOCK_ALL, index, e));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_alpm_get
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    defip_entry_t lpm_key;      /* Route lookup key.        */
    defip_entry_t lpm_entry;    /* Search result buffer.    */
    defip_pair_128_entry_t lpm_128_key;
    defip_pair_128_entry_t lpm_128_entry;
    int clear_hit;              /* Clear hit indicator.     */
    int rv = 0;                 /* Operation return status. */
    uint32 flags = 0;
    soc_mem_t   mem;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }
    
    /* Zero buffers. */
    sal_memset(&lpm_entry, 0, BCM_XGS3_L3_ENT_SZ(unit, defip));
    sal_memset(&lpm_key, 0, BCM_XGS3_L3_ENT_SZ(unit, defip));
    sal_memset(&lpm_128_entry, 0, sizeof(lpm_128_entry));
    sal_memset(&lpm_128_key, 0, sizeof(lpm_128_key));

    /* Check if clear hit bit required. */
    clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

    BCM_IF_ERROR_RETURN(_bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));
    switch (mem) {
      case L3_DEFIP_PAIR_128m:
       /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm128_init(unit, lpm_cfg,
                             &lpm_128_key, 0, &flags));
        /* Perform hw lookup. */
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_128_lookup(unit, &lpm_128_key, &lpm_128_entry, 
                                     &lpm_cfg->defip_index, &lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_128_lookup(unit, &lpm_128_key, &lpm_128_entry, 
                                     &lpm_cfg->defip_index, &lpm_cfg->defip_alpm_cookie);
        }
#endif

        BCM_IF_ERROR_RETURN(rv);

        /* Parse hw buffer to defip entry. */
        _bcm_td2_alpm_lpm128_data_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_128_entry);

        break;

      default:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm_ent_init(unit, lpm_cfg, &lpm_key, 0,
                                                    &flags));
        /* Perform hw lookup. */
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_lookup(unit, &lpm_key, &lpm_entry, &lpm_cfg->defip_index,
                            &lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_lookup(unit, &lpm_key, &lpm_entry, &lpm_cfg->defip_index,
                                &lpm_cfg->defip_alpm_cookie);
        }
#endif

        BCM_IF_ERROR_RETURN(rv);

        /* Parse hw buffer to defip entry. */
        _bcm_td2_alpm_lpm_data_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_entry);
        break;
    }

    /* Clear the HIT bit */
    if (clear_hit && BCM_SUCCESS(rv)) {
        if (mem == L3_DEFIP_PAIR_128m) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_alpm_128_clear_hit(unit, &lpm_128_key,
                    ALPM_ENT_INDEX(lpm_cfg->defip_index),
                    flags & SOC_ALPM_STAT_FLEX));
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_alpm_clear_hit(unit, &lpm_key,
                    ALPM_ENT_INDEX(lpm_cfg->defip_index),
                    flags & SOC_ALPM_STAT_FLEX));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_alpm_add
 * Purpose:
 *      Add an entry to DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    defip_entry_t lpm_entry;
    defip_pair_128_entry_t lpm_128_entry;
    int rv = 0;                     /* Operation return status. */
    uint32  flags= 0;
    soc_mem_t   mem;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));
    switch (mem) {
      case L3_DEFIP_PAIR_128m:
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm128_init(unit, lpm_cfg,
                            &lpm_128_entry, nh_ecmp_idx, &flags));
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_128_insert(unit, &lpm_128_entry, flags, 
                                    lpm_cfg->defip_index, lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_128_insert(unit, &lpm_128_entry, flags, 
                                    lpm_cfg->defip_index, lpm_cfg->defip_alpm_cookie);        
        }
#endif
        break;
      default:
        /* 
         * Initialize hw buffer from lpm configuration. 
         * NOTE: DON'T MOVE _bcm_fb_defip_ent_init CALL UP, 
         * We want to copy flags & nh info, avoid  ipv6 mask & address 
         * corruption
         */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm_ent_init(unit, lpm_cfg, &lpm_entry,
                            nh_ecmp_idx, &flags));

        /* Write buffer to hw. */
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_insert(unit, &lpm_entry, flags, lpm_cfg->defip_index, 
                                lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_insert(unit, &lpm_entry, flags, lpm_cfg->defip_index, 
                                lpm_cfg->defip_alpm_cookie);
        }
#endif
        break;
    }
    _bcm_td2_alpm_trace_log(unit, 0, lpm_cfg, nh_ecmp_idx, rv);

    /* If new route added increment total number of routes.  */
    /* Lack of index indicates a new route. */
    if ((rv >= 0) && !(lpm_cfg->defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT ||
                       lpm_cfg->defip_alpm_cookie & SOC_ALPM_LPM_LOOKUP_HIT)) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, (lpm_cfg->defip_flags & BCM_L3_IP6));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_fb_lpm_del
 * Purpose:
 *      Delete an entry from DEFIP table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = 0;                 /* Operation return status. */
    defip_entry_t lpm_entry;    /* Search result buffer.    */
    defip_pair_128_entry_t lpm_128_entry;    /* Search result buffer.    */
    uint32 flags;
    soc_mem_t   mem;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    /* Zero buffers. */
    sal_memset(&lpm_entry, 0, BCM_XGS3_L3_ENT_SZ(unit, defip));
    sal_memset(&lpm_128_entry, 0, sizeof(lpm_128_entry));

    BCM_IF_ERROR_RETURN(_bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));
    
    switch (mem) {
      case L3_DEFIP_PAIR_128m:
        /* Initialize hw buffer deletion key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm128_init(unit, lpm_cfg,
                                                   &lpm_128_entry, 0, &flags));
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_128_delete(unit, &lpm_128_entry, lpm_cfg->defip_index,
                                    lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_128_delete(unit, &lpm_128_entry, lpm_cfg->defip_index,
                                    lpm_cfg->defip_alpm_cookie);
        }
#endif
        break;
      default:
        /* Initialize hw buffer deletion key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm_ent_init(unit, lpm_cfg, &lpm_entry, 0,
                                               &flags));
        /* Write buffer to hw. */
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
            rv = soc_alpm_delete(unit, &lpm_entry, lpm_cfg->defip_index,
                                    lpm_cfg->defip_alpm_cookie);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        else {
            rv = soc_th_alpm_delete(unit, &lpm_entry, lpm_cfg->defip_index,
                                    lpm_cfg->defip_alpm_cookie);
        }
#endif
        break;
    }
    _bcm_td2_alpm_trace_log(unit, 1, lpm_cfg, -1, rv);

    /* If route deleted, decrement total number of routes.  */
    if (rv >= 0) {
        BCM_XGS3_L3_DEFIP_CNT_DEC(unit, lpm_cfg->defip_flags & BCM_L3_IP6);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_fb_mem_ip6_defip_get
 * Purpose:
 *    Get IP6 address mask field in ALPM mem to defip_cfg
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td2_alpm_ent64_key_parse(int unit, const void *alpm_key, soc_mem_t mem,
                              _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word[2];            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    soc_mem_field_get(unit, mem, alpm_key, KEYf, &ip6_word[0]);
    ip6[0] = (uint8) ((ip6_word[1] >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word[1] >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word[1] >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word[1] & 0xff);

    ip6[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word[0] & 0xff);

    lpm_cfg->defip_sub_len = soc_mem_field32_get(unit, mem, alpm_key, LENGTHf);
}

/*
 * Function:
 *      _bcm_td2_alpm_ent_key_parse
 * Purpose:
 *      Parse entry key from ALPM table.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information. 
 *      *lpm_entry  - (IN) Pointer to lpm buffer read from hw. 
 *      alpm_mem    - (IN) ALPM memory type.
 *      *alpm_entry - (IN) Pointer to alpm entry read from SRAM bank.
 * Returns:
 *      void
 */
STATIC void
_bcm_td2_alpm_ent_key_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            defip_entry_t *lpm_entry, soc_mem_t alpm_mem,
                            void *alpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_td2_alpm_ent64_key_parse(unit, alpm_entry, alpm_mem, lpm_cfg);
    } else {
        /* Get ipv4 address. */
        lpm_cfg->defip_ip_addr = soc_mem_field32_get(unit, alpm_mem, 
                                                     alpm_entry, KEYf);

        /* Get subnet mask. */
        v4_mask = soc_mem_field32_get(unit, alpm_mem, alpm_entry, LENGTHf);
        if (v4_mask) {
            v4_mask = ~((1 << (32 - v4_mask)) - 1);
        }

        /* Fill mask length. */
        lpm_cfg->defip_sub_len = bcm_ip_mask_length(v4_mask);
    }

    /* Get Virtual Router id */
    soc_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, NULL);
    return;
}

/*
 * Function:
 *      _bcm_td2_alpm_ent_data_parse
 * Purpose:
 *      Parse an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information. 
 *      nh_ecmp_idx - (OUT)Next hop index or ecmp group id.  
 *      alpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
STATIC void
_bcm_td2_alpm_ent_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg, int *p_nh_ecmp_idx,
                             soc_mem_t alpm_mem, void *alpm_entry)
{

    int is_ecmp = 0;
    uint32 nh_ecmp_idx = 0;

    /* Check if entry points to ecmp group. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
        nh_ecmp_idx = soc_mem_field32_dest_get(unit, alpm_mem, alpm_entry,
                                               DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
            is_ecmp = 1;
        } else if (dest_type != SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_ecmp_idx = 0;
        }
    } else if (!(SOC_IS_HURRICANEX(unit))) {
        is_ecmp = soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMPf);
        if (is_ecmp) {
            nh_ecmp_idx = soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMP_PTRf);
        } else {
            nh_ecmp_idx = soc_mem_field32_get(unit, alpm_mem, alpm_entry, NEXT_HOP_INDEXf);
        }
    } else {
        nh_ecmp_idx = soc_mem_field32_get(unit, alpm_mem, alpm_entry, NEXT_HOP_INDEXf);
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    /* hurricane does not have ecmp flag*/
    if ((!(SOC_IS_HURRICANEX(unit))) && is_ecmp) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (p_nh_ecmp_idx) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    }
    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_mem_field32_get(unit, alpm_mem, alpm_entry, 
                                             PRIf);

    /* Get hit bit. */
    if (!ALPM_CTRL(unit).hit_bits_skip &&
        soc_mem_field32_get(unit, alpm_mem, alpm_entry, HITf)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_mem_field32_get(unit, alpm_mem, alpm_entry, RPEf)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, alpm_mem, DST_DISCARDf)) {
        if(soc_mem_field32_get(unit, alpm_mem, alpm_entry, DST_DISCARDf)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

    /* Set classification group id. */
    lpm_cfg->defip_lookup_class = soc_mem_field32_get(unit, alpm_mem, 
                                                       alpm_entry, CLASS_IDf);

    if (alpm_mem == L3_DEFIP_ALPM_IPV6_64m ||
        alpm_mem == L3_DEFIP_ALPM_IPV6_64_1m ||
        alpm_mem == L3_DEFIP_ALPM_IPV6_128m) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
    }
    return;
}

STATIC int
_bcm_td2_alpm_ent128_key_parse(int unit, soc_mem_t mem, uint32 *alpm_entry,
                               _bcm_defip_cfg_t *lpm_cfg)
{
    uint32 ip6_word[4];

    uint8 *ip6;

    ip6 = lpm_cfg->defip_ip6_addr;

    soc_mem_field_get(unit, mem, alpm_entry, KEYf, ip6_word);
    ip6[0] = (ip6_word[3] >> 24);
    ip6[1] = (ip6_word[3] >> 16) & 0xff;
    ip6[2] = (ip6_word[3] >> 8) & 0xff;
    ip6[3] = ip6_word[3];

    ip6[4] = ip6_word[2] >> 24;
    ip6[5] = (ip6_word[2] >> 16) & 0xff;
    ip6[6] = (ip6_word[2] >> 8) & 0xff;
    ip6[7] = ip6_word[2];

    ip6[8] = ip6_word[1] >> 24;
    ip6[9] = (ip6_word[1] >> 16) & 0xff;
    ip6[10] = (ip6_word[1] >> 8) & 0xff;
    ip6[11] = ip6_word[1];

    ip6[12] = ip6_word[0] >> 24;
    ip6[13] = (ip6_word[0] >> 16) & 0xff;
    ip6[14] = (ip6_word[0] >> 8) & 0xff;
    ip6[15] = ip6_word[0];

    lpm_cfg->defip_sub_len = 
                    soc_mem_field32_get(unit, mem, alpm_entry, LENGTHf);

    return SOC_E_NONE;
}

STATIC int
_bcm_td2_alpm_ent128_data_parse(int unit, soc_mem_t mem, uint32 *hw_entry,
                                _bcm_defip_cfg_t *lpm_cfg, int *p_nh_ecmp_idx)
{
    int is_ecmp = 0;
    uint32 nh_ecmp_idx = 0;

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Check if entry points to ecmp group. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
        nh_ecmp_idx = soc_mem_field32_dest_get(unit, mem, hw_entry,
                                               DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
            is_ecmp = 1;
        } else if (dest_type != SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_ecmp_idx = 0;
        }
    } else {
        is_ecmp = soc_mem_field32_get(unit, mem, hw_entry, ECMPf);
        if (is_ecmp) {
            nh_ecmp_idx = soc_mem_field32_get(unit, mem, hw_entry, ECMP_PTRf);
        } else {
            nh_ecmp_idx = soc_mem_field32_get(unit, mem, hw_entry, NEXT_HOP_INDEXf);
        }
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (is_ecmp) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = TRUE;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (p_nh_ecmp_idx != NULL) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (p_nh_ecmp_idx != NULL) {
            *p_nh_ecmp_idx = nh_ecmp_idx;
        }
    }

    /* Mark entry as IPv6 */
    lpm_cfg->defip_flags |= BCM_L3_IP6;

    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_mem_field32_get(unit, mem, hw_entry, PRIf);

    /* Get classification group id. */
    lpm_cfg->defip_lookup_class = 
        soc_mem_field32_get(unit, mem, hw_entry, CLASS_IDf);

    /* Get hit bit. */
    if (!ALPM_CTRL(unit).hit_bits_skip &&
        soc_mem_field32_get(unit, mem, hw_entry, HITf)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_mem_field32_get(unit, mem, hw_entry, RPEf)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard field. */
    if(soc_mem_field32_get(unit, mem, hw_entry, DST_DISCARDf)) {
        lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_alpm_warmboot_walk
 * Purpose:
 *      Recover LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6;                   /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf_id;                 /*                              */
    int vrf;                    /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkt_idx = 0;            /*                              */
    int bkt_ptr = 0;            /* Bucket pointer               */
    int bank_num = 0;           /* Number of active SRAM banks  */
    int pivot_idx = 0;          /* Pivot Index                  */
    int entry_num = 0;          /* ALPM entry number in bucket  */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int entry_count;            /* ALPM entry count in bucket   */
    int bank_count;             /* SRAM bank count              */
    int step_count;             /*                              */
    int cmp_result;             /* Test routine result.         */
    int rv = BCM_E_FAIL;        /* Operation return status.     */
    int defip_table_size = 0;   /* Defip table size.            */
    char *lpm_tbl_ptr = NULL;   /* DMA table pointer.           */
    void *alpm_entry = NULL;    /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    defip_entry_t *lpm_entry;   /* Hw entry buffer.             */
    defip_alpm_ipv4_1_entry_t    alpm_entry_v4;
    defip_alpm_ipv6_64_1_entry_t alpm_entry_v6_64;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int ipmc_route = 0;         /* Route is ip multicast type   */
#endif
    int flex;
    int bkt_ptr_tmp = 0;
    uint8 bkt_occupied = 0;
    
    /* DMA LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size));

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end    = defip_table_size;
    bank_count = 4;
    
    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                         defip_entry_t *, lpm_tbl_ptr, idx);

        ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0_f(unit));

        /* Calculate LPM table traverse step count */
        if (ipv6) {
            if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
                step_count = 2;
            } else {
                step_count = 1;
            }
        } else {
            step_count = 2;
        }
      
        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (BCM_FAILURE(soc_alpm_warmboot_lpm_reinit(unit, ipv6, 
                                                     idx, lpm_entry))) {
            goto free_lpm_table;
        }

        bkt_occupied = 0;

        /*
         * This loop is used for two purposes
         *  1. IPv4, DEFIP entry has two IPv4 entries.
         *  2. IPv6 double wide mode, walk next bucket.
         */
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {  /* If index == 1*/
                if (!ipv6) {
                    /* Copy upper half of lpm entry to lower half */
                    soc_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
               
                    /* Invalidate upper half */
                    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 0);
                }
            }
    
            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            if (ipv6 && tmp_idx) {
                /* 
                 * IPv6 double wide mode,  Walk bucket entries at next bucket.
                 */
                bkt_ptr++;
            } else {
                /* Extract bucket pointer from LPM entry */
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                              ALG_BKT_PTR0f);
                bkt_ptr_tmp = bkt_ptr;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_ipmc_defip)) {
                    /* Skip reserved ipmc entry */
                    int ipmc_index = 0;
                    ipmc_route = 0;
                    if (soc_mem_field_valid(unit, L3_DEFIPm, MULTICAST_ROUTE0f)) {
                        ipmc_route = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MULTICAST_ROUTE0f);
                    } else if (soc_mem_field_valid(unit, L3_DEFIPm, DATA_TYPE0f)) {
                        if (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, DATA_TYPE0f) == 2) {
                            ipmc_route = 1;
                        } else {
                            ipmc_route = 0;
                        }
                    }

                    if (soc_feature(unit, soc_feature_generic_dest)) {
                        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
                        uint32 dest_value = 0;
                        dest_value = soc_mem_field32_dest_get(unit,
                                L3_DEFIPm, lpm_entry,
                                DESTINATION0f, &dest_type);
                        if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                            ipmc_index = dest_value;
                        } else {
                            ipmc_index = 0;
                        }
                    } else {
                        ipmc_index = soc_mem_field32_get(unit, L3_DEFIPm,
                                        lpm_entry, L3MC_INDEX0f);
                    }

                    if (ipmc_route && ipmc_index == 0) {
                        continue;
                    }
                }
#endif
                /* Extract VRF from LPM entry*/
                if (BCM_FAILURE(soc_alpm_lpm_vrf_get
                                (unit, lpm_entry, &vrf_id, &vrf))) {
                    goto free_lpm_table;
                }

                /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix 
                 * resides in TCAM */
                if (bkt_ptr == 0) {
                    VRF_TRIE_ROUTES_INC(unit, vrf_id, vrf, ipv6);

                    sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                    _bcm_td2_alpm_lpm_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                           lpm_entry);
                    _bcm_td2_alpm_lpm_key_parse(unit, &lpm_cfg, lpm_entry);
                    lpm_cfg.defip_index = idx;

                    if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
                        if (trv_data->op_cb) {
                            (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                                      (void *)&lpm_cfg,
                                                      (void *)&nh_ecmp_idx, 
                                                      &cmp_result);
                        }
                    }
                    if (ipv6) {
                        tmp_idx++;
                    }
                    continue;
                }
                    
                pivot_idx = (idx << 1) + tmp_idx;

                /* TCAM pivot recovery */
                if(BCM_FAILURE(soc_alpm_warmboot_pivot_add(unit, ipv6, lpm_entry, pivot_idx,
                                                           bkt_ptr))) {
                    goto free_lpm_table;
                }

                /* Set bucket bitmap */
                if(BCM_FAILURE(soc_alpm_warmboot_bucket_bitmap_set(unit, ipv6, bkt_ptr))) {
                    goto free_lpm_table;
                }
            }

            flex = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f);
            if (ipv6) {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    bkt_count = ALPM_IPV6_64_BKT_COUNT_FLEX;
                    entry_count = 3;
                    alpm_entry = &alpm_entry_v6_64;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                    bkt_count = ALPM_IPV6_64_BKT_COUNT;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v6_64;
                }
            } else {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                    bkt_count = ALPM_IPV4_BKT_COUNT_FLEX;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v4;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV4m;
                    bkt_count = ALPM_IPV4_BKT_COUNT;
                    entry_count = 6;
                    alpm_entry = &alpm_entry_v4;
                }
            }

            entry_num = 0;
            bank_num = 0;

            /* Get the bucket pointer from lpm entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }

                /* Read entry from bucket memory */
                if (BCM_FAILURE(soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                  bkt_addr, alpm_entry))) {
                    goto free_lpm_table;
                }

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                bkt_occupied = 1;

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_td2_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                             alpm_mem, alpm_entry);
               
                /* Execute operation routine if any. */
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        continue;
                    }
#endif
                }

                if(BCM_FAILURE(soc_alpm_warmboot_prefix_insert(unit, ipv6, flex, lpm_entry,
                                         alpm_entry, pivot_idx, bkt_ptr, bkt_addr))) {
                    continue;
                }

            } /* End of bucket walk loop*/
            if (bkt_occupied) {
                /* If ipv6 double wide,  two alpm bkt views should be set */
                if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
                    _soc_trident2_alpm_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                    _soc_trident2_alpm_bkt_view_set(unit, (bkt_ptr_tmp + 1) << 2, alpm_mem);
                } else {
                    _soc_trident2_alpm_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                }
            }
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if(BCM_FAILURE(soc_alpm_warmboot_lpm_reinit_done(unit))) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;
free_lpm_table:
    soc_cm_sfree(unit, lpm_tbl_ptr);

    return (rv);
}

/*
 * Function:
 *      _bcm_td2_alpm_128_warmboot_walk
 * Purpose:
 *      Recover IPv6-128Bit LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_128_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6 = 2;               /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf, vrf_id;            /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkt_idx = 0;            /*                              */
    int bkt_ptr = 0;            /* Bucket pointer               */
    int bank_num = 0;           /* Number of active SRAM banks  */
    int entry_num = 0;          /* ALPM entry number in bucket  */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int entry_count;            /* ALPM entry count in bucket   */
    int bank_count;             /* SRAM bank count              */
    int step_count;             /*                              */
    int cmp_result;             /* Test routine result.         */
    int rv = BCM_E_FAIL;        /* Operation return status.     */
    int defip_table_size = 0;   /* Defip table size.            */
    char *lpm_tbl_ptr = NULL;   /* DMA table pointer.           */
    void *alpm_entry = NULL;    /*                              */
    uint32 rval;                /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    defip_pair_128_entry_t *lpm_entry; /* Hw entry buffer.      */
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128; /* ALPM 128  */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int ipmc_route = 0;         /* Route is ip multicast type   */
#endif
    int bkt_ptr_tmp = 0;
    uint8 bkt_occupied = 0;

    /* DMA LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m,
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                    "lpm_128_warmboot_tbl",
                    &lpm_tbl_ptr, &defip_table_size));

    if(BCM_FAILURE(READ_L3_DEFIP_RPF_CONTROLr(unit, &rval))) {
        goto free_lpm_table;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    alpm_entry  = &alpm_entry_v6_128;
    bkt_count   = ALPM_IPV6_128_BKT_COUNT;
    entry_count = 2;
    bank_count  = 4;
    
    if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
        step_count = 2;
    } else {
        step_count = 1;
    }
    
    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                         defip_pair_128_entry_t *, lpm_tbl_ptr, idx);

        /* Verify if read LPM entry is IPv6-128 entry */
        if (0x03 != soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                        MODE1_UPR_f(unit))) {
            continue;
        };
     
        /* Verify if LPM entry is valid */
        if (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID1_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID0_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                    VALID1_UPRf) || 
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID0_UPRf)) {
                continue;
        };

        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (BCM_FAILURE(soc_alpm_128_warmboot_lpm_reinit(unit, ipv6, 
                                                     idx, lpm_entry))) {
            goto free_lpm_table;
        }

        bkt_occupied = 0;

        /*  If IPv6 double wide mode, walk next bucket.*/
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
    
            if (tmp_idx) {
                /* IPv6 double wide mode, Walk bucket entries at next bucket.*/
                bkt_ptr++;
            } else {
                /* Extract bucket pointer from LPM entry */
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                              ALG_BKT_PTRf);
                bkt_ptr_tmp = bkt_ptr;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_ipmc_defip)) {
                    /* Skip reserved ipmc entry */
                    int ipmc_index = 0;
                    ipmc_route = 0;
                    if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
                        ipmc_route = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                                      lpm_entry, MULTICAST_ROUTEf);
                    } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
                        if (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                                lpm_entry, DATA_TYPEf) == 2) {
                            ipmc_route = 1;
                        } else {
                            ipmc_route = 0;
                        }
                    }

                    if (soc_feature(unit, soc_feature_generic_dest)) {
                        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
                        uint32 dest_value = 0;
                        dest_value = soc_mem_field32_dest_get(unit,
                                L3_DEFIP_PAIR_128m, lpm_entry,
                                DESTINATIONf, &dest_type);
                        if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                            ipmc_index = dest_value;
                        } else {
                            ipmc_index = 0;
                        }
                    } else {
                        ipmc_index = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                        lpm_entry, L3MC_INDEXf);
                    }

                    if ((ipmc_route) && (ipmc_index == 0)) {
                        continue;
                    }
                }
#endif

                /* Extract VRF from LPM entry*/
                if (BCM_FAILURE(soc_alpm_128_lpm_vrf_get
                                (unit, lpm_entry, &vrf_id, &vrf))) {
                    goto free_lpm_table;
                }

                /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix resides in TCAM */
                if (bkt_ptr == 0) {
                    VRF_TRIE_ROUTES_INC(unit, vrf_id, vrf, ipv6);

                    /* TCAM Routes Processing */ 
                    sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                    (void) _bcm_td2_alpm_lpm128_key_parse(unit, (uint32 *) lpm_entry,
                                                     &lpm_cfg);
                    (void) soc_alpm_128_lpm_vrf_get(unit, lpm_entry, 
                                                    &lpm_cfg.defip_vrf, 
                                                    &nh_ecmp_idx);
                    (void) _bcm_td2_alpm_lpm128_data_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);

                    lpm_cfg.defip_index = idx;
                    if (trv_data->op_cb) {
                        (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                                  (void *)&lpm_cfg,
                                                  (void *)&nh_ecmp_idx, 
                                                  &cmp_result);
                    }
                    tmp_idx++;
                    continue;
                }

                /* TCAM pivot recovery */
                if(BCM_FAILURE(soc_alpm_128_warmboot_pivot_add(unit, ipv6,
                               lpm_entry, idx, bkt_ptr))) {
                    goto free_lpm_table;
                }

                /* Set bucket bitmap */
                if(BCM_FAILURE(soc_alpm_128_warmboot_bucket_bitmap_set(unit, ipv6,
                               bkt_ptr))) {
                    goto free_lpm_table;
                }
            }

            /* Get the bucket pointer from lpm entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);
                entry_num++; 
                
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }

                /* Read entry from bucket memory */
                if (BCM_FAILURE(soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                  bkt_addr, alpm_entry))) {
                    goto free_lpm_table;
                }

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                bkt_occupied = 1;

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Fill entry IP address and subnet mask. */
                _bcm_td2_alpm_ent128_key_parse(unit, alpm_mem, alpm_entry, &lpm_cfg);
                soc_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf, &vrf);
                /* Parse the entry. */
                _bcm_td2_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry,
                                            &lpm_cfg, &nh_ecmp_idx);
               
                /* Execute operation routine if any. */
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        continue;
                    }
#endif
                }

                if(BCM_FAILURE(soc_alpm_128_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                                         alpm_entry, idx, bkt_ptr, bkt_addr))) {
                    continue;
                }

            } /* End of bucket walk loop*/
            if (bkt_occupied) {
                /* If IPV6 double wide,  two alpm bkt views should be set */
                if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
                    _soc_trident2_alpm_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                    _soc_trident2_alpm_bkt_view_set(unit, (bkt_ptr_tmp + 1) << 2, alpm_mem);
                } else {
                    _soc_trident2_alpm_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                }
            }
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if(BCM_FAILURE(soc_alpm_128_warmboot_lpm_reinit_done(unit))) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;
free_lpm_table:
    soc_cm_sfree(unit, lpm_tbl_ptr);

    return (rv);
}

STATIC int
_bcm_td2_alpm_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6;                /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int phy_idx;                /* Iteration index.             */
    int tmp_idx;                /* ipv4 entries iterator.       */
    char *lpm_tbl_ptr = NULL;   /* Dma table pointer.           */
    char *alpm_tbl_ptr = NULL;  /* Dma table pointer.           */
    char *alpm_hit_tbl_ptr = NULL;  /* Dma hit only table pointer.  */
    char *alpm_hit_tbl_y_ptr = NULL;  /* Dma hit only Y pipe table pointer. */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_pair_128_entry_t *lpm_entry = NULL;   /* Hw entry buffer.    */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count;
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128;
    void *alpm_entry;
    void *raw_entry ;
    soc_mem_t alpm_mem, pivot_mem = L3_DEFIP_PAIR_128m;
    void *hit_entry;
    soc_mem_t alpm_hit_mem, alpm_hit_y_mem;
    int step_count;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    typedef struct _alpm_def_route_info_s {
        int idx;
        int bkt_addr;
    } _alpm_def_route_info_t;
    _alpm_def_route_info_t *def_rte_arr = NULL;

    ipv6 = (trv_data->flags & BCM_L3_IP6);
    if (!ipv6) {
        return SOC_E_NONE;
    }

    /* Table DMA the LPM table to software copy */
    alpm_mem = L3_DEFIP_ALPM_RAWm;

    rv = bcm_xgs3_l3_tbl_dma(unit, alpm_mem,
                             WORDS2BYTES(soc_mem_entry_words(unit, alpm_mem)),
                             "alpm_tbl", &alpm_tbl_ptr, NULL);
    alpm_mem = L3_DEFIP_ALPM_IPV6_128m;
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    rv = bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m,
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                             "lpm_128_tbl", &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
        alpm_hit_mem = L3_DEFIP_ALPM_HIT_ONLYm;
        rv = bcm_xgs3_l3_tbl_dma(unit, alpm_hit_mem,
                                 WORDS2BYTES(soc_mem_entry_words(unit, alpm_hit_mem)),
                                 "alpm_hit_tbl", &alpm_hit_tbl_ptr, NULL);
        if (BCM_FAILURE(rv)) {
            goto error;
        }

        if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
            alpm_hit_y_mem = L3_DEFIP_ALPM_HIT_ONLY_Ym;
            rv = bcm_xgs3_l3_tbl_dma(unit, alpm_hit_y_mem,
                                     WORDS2BYTES(soc_mem_entry_words(unit,
                                             alpm_hit_y_mem)),
                                     "alpm_hit_y_tbl", &alpm_hit_tbl_y_ptr, NULL);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
    }

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = (SOC_VRF_MAX(unit) + 1) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    bkt_count   = ALPM_IPV6_128_BKT_COUNT;
    idx_start   = 0;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = 4;
    entry_count = 2;
    alpm_entry  = &alpm_entry_v6_128;

    if (SOC_ALPM_V6_SCALE_CHECK(unit, 1)) {
        step_count = 2;
    } else {
        step_count = 1;
    }


    /* Walk all lpm entries */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {    

        /* Calculate entry ofset. */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                    defip_pair_128_entry_t *, lpm_tbl_ptr, idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }

        /* Skip reserved ipmc entry */
        if (soc_feature(unit, soc_feature_ipmc_defip)) {
            int ipmc_route = 0;
            int ipmc_index = 0;
            if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
                ipmc_route = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                              lpm_entry, MULTICAST_ROUTEf);
            } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
                if (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                        lpm_entry, DATA_TYPEf) == 2) {
                    ipmc_route = 1;
                } else {
                    ipmc_route = 0;
                }
            }

            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
                uint32 dest_value = 0;
                dest_value = soc_mem_field32_dest_get(unit,
                        L3_DEFIP_PAIR_128m, lpm_entry,
                        DESTINATIONf, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                    ipmc_index = dest_value;
                } else {
                    ipmc_index = 0;
                }
            } else {
                ipmc_index = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                lpm_entry, L3MC_INDEXf);
            }

            if (ipmc_route && (ipmc_index == 0)) {
                continue;
            }
        }

        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {
                bkt_ptr++;
            } else {
                bkt_ptr = soc_mem_field32_get(unit, pivot_mem, lpm_entry, 
                                          ALG_BKT_PTRf);
            }

            if (bkt_ptr == 0) {
                
                /* OVERRIDE VRF Processing */ 

                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_td2_alpm_lpm128_key_parse(unit, (uint32 *) lpm_entry, &lpm_cfg);
                (void) soc_alpm_128_lpm_vrf_get(unit, lpm_entry,
                                                &lpm_cfg.defip_vrf,
                                                &nh_ecmp_idx);
                _bcm_td2_alpm_lpm128_data_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);

                lpm_cfg.defip_index = idx;
                
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        break;
                    }
#endif
                }
            
                /* Skip second iteration for VRF OVERRIDE routes */
                tmp_idx++;

                continue;
            }

            /* get the bucket pointer from lpm mem  entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);
                /* read entry from bucket memory */
                raw_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_ALPM_RAWm,
                        defip_alpm_raw_entry_t *, alpm_tbl_ptr, bkt_addr & 0xFFFF);
                _soc_alpm_raw_mem_read(unit, alpm_mem, raw_entry, entry_num, alpm_entry);

                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }
                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse  the entry. */
                /* Fill entry ip address &  subnet mask. */
                _bcm_td2_alpm_ent128_key_parse(unit, alpm_mem,
                                           (uint32 *) alpm_entry, &lpm_cfg);
                
                soc_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                        &cmp_result);

                /* get associated data */
                _bcm_td2_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry,
                                                &lpm_cfg, &nh_ecmp_idx);

                if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
                    int hit_offset;
                    soc_field_t hit_field[4] = {HIT_0f, HIT_1f, HIT_2f, HIT_3f};
                    hit_entry = soc_mem_table_idx_to_pointer(unit,
                                    L3_DEFIP_ALPM_HIT_ONLYm,
                                    defip_alpm_hit_only_entry_t *,
                                    alpm_hit_tbl_ptr, (bkt_addr / 4));
                    hit_offset = bkt_addr % 4;
                    if (!ALPM_CTRL(unit).hit_bits_skip) {
                        if (soc_mem_field32_get(unit, alpm_hit_mem, hit_entry,
                                                hit_field[hit_offset])) {
                            lpm_cfg.defip_flags |= BCM_L3_HIT;
                        } else if (soc_feature(unit,
                                       soc_feature_two_ingress_pipes)) {
                            hit_entry = soc_mem_table_idx_to_pointer(unit,
                                            L3_DEFIP_ALPM_HIT_ONLY_Ym,
                                            defip_alpm_hit_only_y_entry_t *,
                                            alpm_hit_tbl_y_ptr, (bkt_addr / 4));
                            if (soc_mem_field32_get(unit, alpm_hit_y_mem,
                                    hit_entry, hit_field[hit_offset])) {
                                lpm_cfg.defip_flags |= BCM_L3_HIT;
                            }
                        }
                    }
                }

                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
                    continue;
                }

                phy_idx = soc_alpm_physical_idx(unit, L3_DEFIP_PAIR_128m, idx, 1);
                phy_idx = SOC_ALPM_128_ADDR_LWR(unit, phy_idx ) << 1;
                lpm_cfg.defip_alpm_cookie = SOC_ALPM_DELETE_ALL | phy_idx;
                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx <= SOC_VRF_MAX(unit)) {
                        def_rte_arr[def_arr_idx].bkt_addr = bkt_addr;
                        def_rte_arr[def_arr_idx].idx = idx;
                        def_arr_idx ++;
                    }
                } else {
                    /* Execute operation routine if any. */
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
            }
        }
    }
   
    /* Process Default routes */
    for (idx = 0; idx < def_arr_idx; idx++) {

        /* Calculate entry ofset. */
        lpm_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                    defip_pair_128_entry_t *, lpm_tbl_ptr,
                                    def_rte_arr[idx].idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }

        bkt_addr = def_rte_arr[idx].bkt_addr;
        
        /* read entry from bucket memory */
        raw_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_ALPM_RAWm,
                defip_alpm_raw_entry_t *, alpm_tbl_ptr, bkt_addr & 0xFFFF);
        _soc_alpm_raw_mem_read(unit, alpm_mem, raw_entry, bkt_addr >> 16, alpm_entry);

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        phy_idx = soc_alpm_physical_idx(unit, L3_DEFIP_PAIR_128m,
                                        def_rte_arr[idx].idx, 1);
        phy_idx = SOC_ALPM_128_ADDR_LWR(unit, phy_idx) << 1;
        lpm_cfg.defip_alpm_cookie = SOC_ALPM_DELETE_ALL | phy_idx;
        lpm_cfg.defip_index = bkt_addr;

        /* Parse  the entry. */
        soc_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                 &cmp_result);
        /* Fill entry ip address & subnet mask. */
        _bcm_td2_alpm_ent128_key_parse(unit, alpm_mem, alpm_entry, &lpm_cfg);

        _bcm_td2_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry, &lpm_cfg,
                                    &nh_ecmp_idx);

        /* If protocol doesn't match skip the entry. */
        if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
            continue;
        }

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                    (void *)&lpm_cfg,
                                    (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

error:
    if (def_rte_arr != NULL) {
        sal_free(def_rte_arr);
    }
    if (lpm_tbl_ptr != NULL) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }
    if (alpm_tbl_ptr != NULL) {
        soc_cm_sfree(unit, alpm_tbl_ptr);
    }
    if (alpm_hit_tbl_ptr != NULL) {
        soc_cm_sfree(unit, alpm_hit_tbl_ptr);
    }
    if (alpm_hit_tbl_y_ptr != NULL) {
        soc_cm_sfree(unit, alpm_hit_tbl_y_ptr);
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_td2_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6 = 0;            /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int phy_idx;                /* Iteration index.             */
    int tmp_idx;                /* Iteration index.             */
    int tmp_start;              /* Iteration start.             */
    int tmp_end;                /* Iteration end.               */
    int tmp_delta;              /* Iteration delta.             */
    char *lpm_tbl_ptr = NULL;   /* Dma table pointer.           */
    char *alpm_tbl_ptr = NULL;  /* Dma table pointer.           */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_entry_t *lpm_entry;   /* Hw entry buffer.             */
    defip_entry_t lpm_entry_data; /* Hw entry buffer.           */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    char *alpm_hit_tbl_ptr = NULL;  /* Dma hit only table pointer.  */
    char *alpm_hit_tbl_y_ptr = NULL;  /* Dma Y pipe hit only table pointer.  */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count;
    defip_alpm_ipv4_1_entry_t alpm_entry_v4;
    defip_alpm_ipv6_64_1_entry_t alpm_entry_v6_64;
    void *alpm_entry;
    void *raw_entry ;
    soc_mem_t alpm_mem;
    void *hit_entry;
    soc_mem_t alpm_hit_mem, alpm_hit_y_mem;
    int step_count;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    int flex;
    typedef struct _alpm_def_route_info_s {
        int idx;
        int tmp_idx;
        int bkt_addr;
        defip_entry_t lpm_entry;
    } _alpm_def_route_info_t;
       
    _alpm_def_route_info_t *def_rte_arr = NULL;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        if (soc_mem_index_count(unit, BCM_XGS3_L3_MEM(unit, defip)) != 0) {
            rv =_bcm_td2_alpm_warmboot_walk(unit, trv_data);
            if (rv < 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERROR!  ALPM Warmboot recovery failed")));
                return (rv);
            }
        }
        if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
            /* v6-128 entries */
            rv = _bcm_td2_alpm_128_warmboot_walk(unit, trv_data);
            if (rv < 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERROR!  ALPM Warmboot V6-128 recovery failed")));
            }
        }
        return (rv);
    }
#endif

    ipv6 = trv_data->flags & BCM_L3_IP6;

    if (ipv6 && soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
        /* first get v6-128 entries */
        rv = _bcm_td2_alpm_128_update_match(unit, trv_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#endif
    }

    if (soc_mem_index_count(unit, BCM_XGS3_L3_MEM(unit, defip)) == 0) {
        return BCM_E_NONE;
    }


    /* Table DMA the LPM table to software copy */

    alpm_mem = L3_DEFIP_ALPM_RAWm;
    rv = bcm_xgs3_l3_tbl_dma(unit, alpm_mem,
                             WORDS2BYTES(soc_mem_entry_words(unit, alpm_mem)),
                             "alpm_tbl", &alpm_tbl_ptr, NULL);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
        alpm_hit_mem = L3_DEFIP_ALPM_HIT_ONLYm;
        rv = bcm_xgs3_l3_tbl_dma(unit, alpm_hit_mem,
                                 WORDS2BYTES(soc_mem_entry_words(unit, alpm_hit_mem)),
                                 "alpm_hit_tbl", &alpm_hit_tbl_ptr, NULL);
        if (BCM_FAILURE(rv)) {
            goto error;
        }

        if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
            alpm_hit_y_mem = L3_DEFIP_ALPM_HIT_ONLY_Ym;
            rv = bcm_xgs3_l3_tbl_dma(unit, alpm_hit_y_mem,
                     WORDS2BYTES(soc_mem_entry_words(unit, alpm_hit_y_mem)),
                     "alpm_hit_tbl_y", &alpm_hit_tbl_y_ptr, NULL);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
    }

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = (SOC_VRF_MAX(unit) + 1) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end    = defip_table_size;
    bank_num   = 0;
    entry_num  = 0;
    bank_count = 4;

    if (ipv6) {
        if (SOC_ALPM_V6_SCALE_CHECK(unit, 1)) {
            step_count = 2;
        } else {
            step_count = 1;
        }
    } else {
        step_count = 2;
    }

    /* Walk all lpm entries */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {
            /* Calculate entry ofset. */
        lpm_entry =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                             defip_entry_t *, lpm_tbl_ptr, idx);
        sal_memcpy (&lpm_entry_data, lpm_entry, sizeof (lpm_entry_data));

        if (ipv6 && (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0_f(unit)) == 0)) {
           /*
            * Function called for IPv6 LPM/ALPM walk and LPM entry is IPv4; Continue;
            */
           continue;
        }
        
        /* Each LPM index has two IPv4 entries*/
        if (ipv6) {
           tmp_start = 0;
           tmp_end   = step_count;
           tmp_delta = 1;
        } else {
           tmp_start = step_count - 1;
           tmp_end   = -1;
           tmp_delta = -1;
        }

        for (tmp_idx = tmp_start; tmp_idx != tmp_end; tmp_idx += tmp_delta) {
            if (!ipv6) {
               if (tmp_idx) {
                   soc_alpm_lpm_ip4entry1_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);
               } else {
                   soc_alpm_lpm_ip4entry0_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);
               }
            }

            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            /* Skip reserved ipmc entry */
            if (soc_feature(unit, soc_feature_ipmc_defip)) {
                /* Skip reserved ipmc entry */
                int ipmc_index = 0;
                int ipmc_route = 0;
                if (soc_mem_field_valid(unit, L3_DEFIPm, MULTICAST_ROUTE0f)) {
                    ipmc_route = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MULTICAST_ROUTE0f);
                } else if (soc_mem_field_valid(unit, L3_DEFIPm, DATA_TYPE0f)) {
                    if (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, DATA_TYPE0f) == 2) {
                        ipmc_route = 1;
                    } else {
                        ipmc_route = 0;
                    }
                }

                if (soc_feature(unit, soc_feature_generic_dest)) {
                    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
                    uint32 dest_value = 0;
                    dest_value = soc_mem_field32_dest_get(unit,
                            L3_DEFIPm, lpm_entry,
                            DESTINATION0f, &dest_type);
                    if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                        ipmc_index = dest_value;
                    } else {
                        ipmc_index = 0;
                    }
                } else {
                    ipmc_index = soc_mem_field32_get(unit, L3_DEFIPm,
                                    lpm_entry, L3MC_INDEX0f);
                }

                if (ipmc_route && (ipmc_index == 0)) {
                    continue;
                }
            }

            cmp_result = ipv6 ? 1 : 0;
            if (cmp_result != (soc_mem_field32_get(unit, L3_DEFIPm,
                                                lpm_entry, MODE0_f(unit)))) {
                continue;
            }

            soc_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf, NULL);


            if (ipv6 && tmp_idx) {
                bkt_ptr++;
            } else {
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry,
                                          ALG_BKT_PTR0f);
                if (bkt_ptr == 0) {
                    sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                    _bcm_td2_alpm_lpm_data_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);
                    _bcm_td2_alpm_lpm_key_parse(unit, &lpm_cfg, lpm_entry);

                    lpm_cfg.defip_index = idx;

                    if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
                        if (trv_data->op_cb) {
                            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                                break;
                            }
#endif
                        }
                    }
                    if (ipv6) {
                        /* Skip IPv6 second iteration for TCAM routes */
                        tmp_idx = tmp_end - tmp_delta;
                    }
                    continue;
                }

            }

            flex = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f);
            if (ipv6) {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    bkt_count = ALPM_IPV6_64_BKT_COUNT_FLEX;
                    entry_count = 3;
                    alpm_entry = &alpm_entry_v6_64;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                    bkt_count = ALPM_IPV6_64_BKT_COUNT;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v6_64;
                }

            } else {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                    bkt_count = ALPM_IPV4_BKT_COUNT_FLEX;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v4;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV4m;
                    bkt_count = ALPM_IPV4_BKT_COUNT;
                    entry_count = 6;
                    alpm_entry = &alpm_entry_v4;
                }
            }

            /* Get the bucket pointer from lpm mem entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);

                raw_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_ALPM_RAWm,
                        defip_alpm_raw_entry_t *, alpm_tbl_ptr, bkt_addr & 0xFFFF);
                _soc_alpm_raw_mem_read(unit, alpm_mem, raw_entry, entry_num, alpm_entry);

                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_td2_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                        alpm_mem, alpm_entry);
                if (!soc_feature(unit, soc_feature_alpm_hit_bits_not_support)) {
                    int hit_offset;
                    soc_field_t hit_field[4] = {HIT_0f, HIT_1f, HIT_2f, HIT_3f};
                    hit_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_ALPM_HIT_ONLYm,
                                defip_alpm_hit_only_entry_t *, alpm_hit_tbl_ptr, (bkt_addr / 4));
                    hit_offset = bkt_addr % 4;
                    if (!ALPM_CTRL(unit).hit_bits_skip) {
                        if (soc_mem_field32_get(unit, alpm_hit_mem,
                                hit_entry, hit_field[hit_offset])) {
                            lpm_cfg.defip_flags |= BCM_L3_HIT;
                        } else if (soc_feature(unit,
                                       soc_feature_two_ingress_pipes)) {
                            hit_entry = soc_mem_table_idx_to_pointer(unit,
                                            L3_DEFIP_ALPM_HIT_ONLY_Ym,
                                            defip_alpm_hit_only_y_entry_t *,
                                            alpm_hit_tbl_y_ptr, (bkt_addr / 4));
                            if (soc_mem_field32_get(unit, alpm_hit_y_mem,
                                    hit_entry, hit_field[hit_offset])) {
                                lpm_cfg.defip_flags |= BCM_L3_HIT;
                            }
                        }
                    }
                }

                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) !=  ipv6) {
                    continue;
                }
                 
                /* Fill entry IP address and subnet mask. */
                _bcm_td2_alpm_ent_key_parse(unit, &lpm_cfg, lpm_entry,
                                          alpm_mem, alpm_entry);

                phy_idx = ipv6 ? idx: ((idx << 1) + tmp_idx);
                phy_idx = soc_alpm_physical_idx(unit, L3_DEFIPm, phy_idx, ipv6);
                phy_idx = phy_idx << (ipv6 ? 1 : 0);
                lpm_cfg.defip_alpm_cookie = SOC_ALPM_DELETE_ALL | phy_idx;
                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx <= SOC_VRF_MAX(unit)) {
                        def_rte_arr[def_arr_idx].bkt_addr = bkt_addr;
                        def_rte_arr[def_arr_idx].idx = idx;
                        def_rte_arr[def_arr_idx].tmp_idx = tmp_idx;
                        sal_memcpy(&def_rte_arr[def_arr_idx].lpm_entry,
                                   lpm_entry, sizeof(*lpm_entry));
                        def_arr_idx ++;
                    }
                } else {
                    /* Execute operation routine if any. */
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
            }
        }
    }

    /* Process Default routes */
    for (idx = 0; idx < def_arr_idx; idx++) {

        /* Calculate entry ofset. */
        lpm_entry = &def_rte_arr[idx].lpm_entry;

        cmp_result = ipv6 ? 1 : 0;
        if (cmp_result != (soc_mem_field32_get(unit, L3_DEFIPm,
                                            lpm_entry, MODE0_f(unit)))) {
            /*
             * Function called for IPv6 LPM/ALPM walk and LPM entry is IPv4; 
             * Continue;
             */
            continue;
        }
        flex = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f);
        if (ipv6) {
            if (flex) {
                alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                alpm_entry = &alpm_entry_v6_64;
            } else {
                alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                alpm_entry = &alpm_entry_v6_64;
            }

        } else {
            if (flex) {
                alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                alpm_entry = &alpm_entry_v4;
            } else {
                alpm_mem = L3_DEFIP_ALPM_IPV4m;
                alpm_entry = &alpm_entry_v4;
            }
        }

        bkt_addr = def_rte_arr[idx].bkt_addr;
        
        /* read entry from bucket memory */
        raw_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_ALPM_RAWm,
                defip_alpm_raw_entry_t *, alpm_tbl_ptr, bkt_addr & 0xFFFF);
        _soc_alpm_raw_mem_read(unit, alpm_mem, raw_entry, bkt_addr >> 16, alpm_entry);

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        phy_idx = ipv6 ? def_rte_arr[idx].idx:
                       ((def_rte_arr[idx].idx << 1) + def_rte_arr[idx].tmp_idx);
        phy_idx = soc_alpm_physical_idx(unit, L3_DEFIPm, phy_idx, ipv6);
        phy_idx = phy_idx << (ipv6 ? 1 : 0);
        lpm_cfg.defip_alpm_cookie = SOC_ALPM_DELETE_ALL | phy_idx;
        lpm_cfg.defip_index = bkt_addr;

        soc_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf, NULL);
        /* Parse  the entry. */
        _bcm_td2_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                     alpm_mem, alpm_entry);

        /* If protocol doesn't match skip the entry. */
        if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
            continue;
        }

        /* Fill entry ip address & subnet mask. */
        _bcm_td2_alpm_ent_key_parse(unit, &lpm_cfg, lpm_entry,
                                    alpm_mem, alpm_entry);

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                    (void *)&lpm_cfg,
                                    (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

error:
    if (def_rte_arr != NULL) {
        sal_free(def_rte_arr);
    }
    if (lpm_tbl_ptr != NULL) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }
    if (alpm_tbl_ptr != NULL) {
        soc_cm_sfree(unit, alpm_tbl_ptr);
    }
    if (alpm_hit_tbl_ptr != NULL) {
        soc_cm_sfree(unit, alpm_hit_tbl_ptr);
    }
    if (alpm_hit_tbl_y_ptr != NULL) {
        soc_cm_sfree(unit, alpm_hit_tbl_y_ptr);
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_td2_alpm_find
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    defip_entry_t lpm_key;      /* Route lookup key.        */
    defip_entry_t lpm_entry;    /* Search result buffer.    */
    defip_pair_128_entry_t lpm_128_key;
    defip_pair_128_entry_t lpm_128_entry;
    int do_urpf = 0;            /* Find uRPF region */
    int rv;                     /* Operation return status. */
    uint32 flags = 0;
    soc_mem_t mem;
    int lpm_mode, tmp;
    uint32 rval;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(unit, &rval));
    lpm_mode  = soc_reg_field_get(unit, L3_DEFIP_RPF_CONTROLr, rval, LPM_MODEf);
    /* Not supported in Legacy LPM mode */
    if (lpm_mode == 0) {
        return (BCM_E_UNAVAIL);
    }
    
    /* Zero buffers. */
    sal_memset(&lpm_entry, 0, sizeof(defip_entry_t));
    sal_memset(&lpm_key, 0, sizeof(defip_entry_t));
    sal_memset(&lpm_128_entry, 0, sizeof(defip_pair_128_entry_t));
    sal_memset(&lpm_128_key, 0, sizeof(defip_pair_128_entry_t));

    /* Default value */
    mem = L3_DEFIPm;
    if ((lpm_cfg->defip_flags & BCM_L3_IP6)) {
        if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) > 0) {
            mem = L3_DEFIP_PAIR_128m;
        }
    }
    
    switch (mem) {
    case L3_DEFIP_PAIR_128m:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm128_init(unit, lpm_cfg,
                                            &lpm_128_key, 0, &flags));
        /* Perform hw lookup. */
        rv = soc_alpm_128_find_best_match(unit, &lpm_128_key, &lpm_128_entry, 
                                          &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);

        /* Parse hw buffer to defip entry. */
        /* OVERRIDE VRF Processing */ 
        _bcm_td2_alpm_lpm128_key_parse(unit, (uint32 *)&lpm_128_entry, lpm_cfg);
        soc_alpm_128_lpm_vrf_get(unit, &lpm_128_entry, &lpm_cfg->defip_vrf, &tmp);
        _bcm_td2_alpm_lpm128_data_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_128_entry);

        break;
    default:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_alpm_lpm_ent_init(unit, lpm_cfg, &lpm_key, 0,
                                                   &flags));
        /* Perform hw lookup. */
        rv = soc_alpm_find_best_match(unit, &lpm_key, &lpm_entry, 
                                      &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);
    
        /* Parse hw buffer to defip entry. */
        _bcm_td2_alpm_lpm_data_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_entry);
        _bcm_td2_alpm_lpm_key_parse(unit, lpm_cfg, &lpm_entry);
        
        break;
    }
    
    return (BCM_E_NONE);
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_th3_alpm_clear_hit
 * Purpose:
 *      Clear prefix entry hit bit.
 * Parameters:
 *      u       - (IN)SOC unit number.
 *      lpm_cfg - (IN)Found LPM Defip config info
 * Returns:
 *      BCM_E_XXX
 */
STATIC int _bcm_th3_alpm_clear_hit(int u,
						    _bcm_defip_cfg_t * lpm_cfg)
{
	return bcm_esw_alpm_clear_hit(u, lpm_cfg);
}

/*
 * Function:
 *      _bcm_th3_alpm_get
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information.
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
    int bcm_th3_alpm_get(
    int unit,
    _bcm_defip_cfg_t * lpm_cfg,
    int *nh_ecmp_idx)
    {
        int clear_hit;          /* Clear hit indicator.  */
        int alpm_global_routes = 0;

        /*
         * Input parameters check 
         */
        if (lpm_cfg == NULL)
        {
            return (BCM_E_PARAM);
        }

#if 0
        BCM_IF_ERROR_RETURN(
            _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                           lpm_cfg->defip_sub_len, NULL));
#else
        if ((lpm_cfg->defip_sub_len > 64 &&
             (soc_mem_index_count(unit, L3_DEFIP_PAIR_LEVEL1m) <= 0)) ||
            (!(lpm_cfg->defip_flags & BCM_L3_IP6) &&
             (BCM_XGS3_L3_DEFIP_TBL_SIZE(unit) == 0))
           ) {
            return BCM_E_PARAM;
        }
#endif
        /*
         * Check if clear hit bit required. 
         */
        clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

        /*
         * Perform hw lookup. 
         */
        BCM_IF_ERROR_RETURN(bcm_esw_alpm_lookup(unit, lpm_cfg));

        if (nh_ecmp_idx)
        {
            *nh_ecmp_idx = lpm_cfg->defip_ecmp_index;
        }

        if (bcm_esw_alpm_hit_enabled(unit) &&
            !(lpm_cfg->defip_flags & BCM_L3_IPMC) &&
            _soc_alpm_mode[unit] != 0 &&
            (lpm_cfg->defip_vrf == BCM_L3_VRF_OVERRIDE ||
             (lpm_cfg->defip_vrf == BCM_L3_VRF_GLOBAL &&
              _soc_alpm_mode[unit] == 3))) {
            alpm_global_routes = 1;
        }

        if (bcm_esw_alpm_hit_enabled(unit) &&
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            alpm_global_routes = 1;
        }

        /*
         * Clear the HIT bit 
         */
        if (clear_hit && !alpm_global_routes)
        {
            BCM_IF_ERROR_RETURN(_bcm_th3_alpm_clear_hit(unit, lpm_cfg));
        }

        return (BCM_E_NONE);
    }

/*
 * Function:
 *      _bcm_th3_alpm_add
 * Purpose:
 *      Add an entry to DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information.
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
    int bcm_th3_alpm_add(
    int unit,
    _bcm_defip_cfg_t * lpm_cfg,
    int nh_ecmp_idx)
    {
        int rv = 0;             /* Operation return status. */
        /*
         * Input parameters check 
         */
        if (lpm_cfg == NULL)
        {
            return (BCM_E_PARAM);
        }
#if 0
        BCM_IF_ERROR_RETURN(
            _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                           lpm_cfg->defip_sub_len, NULL));
#else
        if ((lpm_cfg->defip_sub_len > 64 &&
             (soc_mem_index_count(unit, L3_DEFIP_PAIR_LEVEL1m) <= 0)) ||
            (!(lpm_cfg->defip_flags & BCM_L3_IP6) &&
             (BCM_XGS3_L3_DEFIP_TBL_SIZE(unit) == 0))
           ) {
            return BCM_E_PARAM;
        }
#endif
        /*
         * Write buffer to hw. 
         */
        lpm_cfg->defip_ecmp_index = nh_ecmp_idx;
        rv = bcm_esw_alpm_insert(unit, lpm_cfg);

        bcm_esw_alpm_trace_log(unit, 0, lpm_cfg, nh_ecmp_idx, rv);
        /*
         * If new route added increment total number of routes.  
         */
        /*
         * Lack of index indicates a new route. 
         */
        if ((rv >= 0) && !(lpm_cfg->defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT))
        {
            BCM_XGS3_L3_DEFIP_CNT_INC(unit, (lpm_cfg->defip_flags & BCM_L3_IP6));
        }

        return rv;
    }

/*
 * Function:
 *      _bcm_th3_alpm_del
 * Purpose:
 *      Delete an entry from DEFIP table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information.
 * Returns:
 *      BCM_E_XXX
 */
    int bcm_th3_alpm_del(
    int unit,
    _bcm_defip_cfg_t * lpm_cfg)
    {
        int rv = 0;             /* Operation return status. */
        /*
         * Input parameters check 
         */
        if (lpm_cfg == NULL)
        {
            return (BCM_E_PARAM);
        }

         /* For fast delete_all, we skip delete route and only do delete ecmp_nh_idx */
        if (lpm_cfg->fast_delete_all) {
            return rv;
        }
#if 0
        BCM_IF_ERROR_RETURN(
            _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                           lpm_cfg->defip_sub_len, NULL));
#else
        if ((lpm_cfg->defip_sub_len > 64 &&
             (soc_mem_index_count(unit, L3_DEFIP_PAIR_LEVEL1m) <= 0)) ||
            (!(lpm_cfg->defip_flags & BCM_L3_IP6) &&
             (BCM_XGS3_L3_DEFIP_TBL_SIZE(unit) == 0))
           ) {
            return BCM_E_PARAM;
        }
#endif
        /*
         * Write buffer to hw. 
         */
        rv = bcm_esw_alpm_delete(unit, lpm_cfg);

        bcm_esw_alpm_trace_log(unit, 1, lpm_cfg, -1, rv);
        /*
         * If route deleted, decrement total number of routes.  
         */
        if (rv >= 0)
        {
            BCM_XGS3_L3_DEFIP_CNT_DEC(unit, lpm_cfg->defip_flags & BCM_L3_IP6);
        }
        return rv;
    }

/*
 * Function:
 *      _bcm_th3_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
    int bcm_th3_alpm_update_match(
    int unit,
    _bcm_l3_trvrs_data_t * trv_data)
    {
        int rv = BCM_E_NONE;

        rv = bcm_esw_alpm_update_match(unit, trv_data);
        return (rv);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Function:
 *      _bcm_th_alpm_clear_hit
 * Purpose:
 *      Clear prefix entry hit bit.
 * Parameters:
 *      u       - (IN)SOC unit number.
 *      lpm_cfg - (IN)Found LPM Defip config info
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_th_alpm_clear_hit(int u, _bcm_defip_cfg_t * lpm_cfg)
{
    return bcm_esw_alpm_clear_hit(u, lpm_cfg);
}

/*
 * Function:
 *      bcm_th_alpm_get
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information.
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_alpm_get(int unit, _bcm_defip_cfg_t * lpm_cfg, int *nh_ecmp_idx)
{
    int clear_hit;          /* Clear hit indicator.  */
    soc_mem_t mem;
    int alpm_global_routes = 0;
    /*
     * Input parameters check
     */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                       lpm_cfg->defip_sub_len, &mem));

    /*
     * Check if clear hit bit required. 
     */
    clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

    /*
     * Perform hw lookup.
     */
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_lookup(unit, lpm_cfg));

    if (nh_ecmp_idx) {
        *nh_ecmp_idx = lpm_cfg->defip_ecmp_index;
    }

    if (bcm_esw_alpm_hit_enabled(unit) &&
        !(lpm_cfg->defip_flags & BCM_L3_IPMC) &&
        _soc_alpm_mode[unit] != 0 &&
        (lpm_cfg->defip_vrf == BCM_L3_VRF_OVERRIDE ||
         (lpm_cfg->defip_vrf == BCM_L3_VRF_GLOBAL &&
          _soc_alpm_mode[unit] == 3))) {
        alpm_global_routes = 1;
    }

    if (bcm_esw_alpm_hit_enabled(unit) &&
        (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        alpm_global_routes = 1;
    }

    /*
     * Clear the HIT bit
     */
    if (clear_hit && !alpm_global_routes) {
        BCM_IF_ERROR_RETURN(_bcm_th_alpm_clear_hit(unit, lpm_cfg));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_th_alpm_add
 * Purpose:
 *      Add an entry to DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information.
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_th_alpm_add(int unit, _bcm_defip_cfg_t * lpm_cfg, int nh_ecmp_idx)
{
    int rv = 0;             /* Operation return status. */
    soc_mem_t mem;

    /*
     * Input parameters check 
     */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                       lpm_cfg->defip_sub_len, &mem));

    /*
     * Write buffer to hw.
     */
    lpm_cfg->defip_ecmp_index = nh_ecmp_idx;
    rv = bcm_esw_alpm_insert(unit, lpm_cfg);

    bcm_esw_alpm_trace_log(unit, 0, lpm_cfg, nh_ecmp_idx, rv);

    /*
     * If new route added increment total number of routes.  
     */
    /*
     * Lack of index indicates a new route. 
     */
    if ((rv >= 0) && !(lpm_cfg->defip_alpm_cookie & SOC_ALPM_LOOKUP_HIT ||
                       lpm_cfg->defip_alpm_cookie & SOC_ALPM_LPM_LOOKUP_HIT)) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, (lpm_cfg->defip_flags & BCM_L3_IP6));
    }

    return rv;
}

/*
 * Function:
 *      bcm_th_alpm_del
 * Purpose:
 *      Delete an entry from DEFIP table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_th_alpm_del(int unit, _bcm_defip_cfg_t * lpm_cfg)
{
    int rv = 0;             /* Operation return status. */
    soc_mem_t mem;

    /*
     * Input parameters check 
     */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_alpm_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                       lpm_cfg->defip_sub_len, &mem));

    /*
     * Write buffer to hw.
     */
    rv = bcm_esw_alpm_delete(unit, lpm_cfg);

    bcm_esw_alpm_trace_log(unit, 1, lpm_cfg, -1, rv);

    /*
     * If route deleted, decrement total number of routes.
     */
    if (rv >= 0) {
        BCM_XGS3_L3_DEFIP_CNT_DEC(unit, lpm_cfg->defip_flags & BCM_L3_IP6);
    }
    return rv;
}

/*
 * Function:
 *      bcm_th_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t * trv_data)
{
    int rv = BCM_E_NONE;

    rv = bcm_esw_alpm_update_match(unit, trv_data);
    return (rv);
}
#endif /* BCM_TOMAHAWK_SUPPORT */

#endif /* ALPM_ENABLE */

/*
 * Function:
 *      bcm_td2_ecmp_rh_deinit
 * Purpose:
 *      Deallocate ECMP resilient hashing internal data structures
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
void
bcm_td2_ecmp_rh_deinit(int unit)
{
    if (_td2_ecmp_rh_info[unit]) {
        if (_td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap) {
            sal_free(_td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap);
            _td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap = NULL;
        }
        sal_free(_td2_ecmp_rh_info[unit]);
        _td2_ecmp_rh_info[unit] = NULL;
    }
}

/*
 * Function:
 *      bcm_td2_ecmp_rh_init
 * Purpose:
 *      Initialize ECMP resilient hashing internal data structures
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_ecmp_rh_init(int unit)
{
    int num_ecmp_rh_flowset_entries;

    bcm_td2_ecmp_rh_deinit(unit);

    _td2_ecmp_rh_info[unit] = sal_alloc(sizeof(_td2_ecmp_rh_info_t),
            "_td2_ecmp_rh_info");
    if (_td2_ecmp_rh_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_td2_ecmp_rh_info[unit], 0, sizeof(_td2_ecmp_rh_info_t));

    /* The LAG and ECMP resilient hash features share the same flow set table.
     * The table can be configured in one of 3 modes:
     * - dedicated to LAG resilient hashing,
     * - dedicated to ECMP resilient hashing,
     * - split evenly between LAG and ECMP resilient hashing.
     * The mode was configured during Trident2 initialization.
     */
    num_ecmp_rh_flowset_entries = soc_mem_index_count(unit, RH_ECMP_FLOWSETm);

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        _td2_ecmp_rh_info[unit]->ecmp_block_size = 3;
        _td2_ecmp_rh_info[unit]->ecmp_rand_seed_shift = 22;
    } else
#endif
    {
        _td2_ecmp_rh_info[unit]->ecmp_block_size = 6;
        _td2_ecmp_rh_info[unit]->ecmp_rand_seed_shift = 16;
    }

    /* Each bit in ecmp_rh_flowset_block_bitmap corresponds to a block of 64
     * resilient hashing flow set table entries.
     */
    _td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks =
        num_ecmp_rh_flowset_entries >> _td2_ecmp_rh_info[unit]->ecmp_block_size;
    if (_td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks > 0) {
        _td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(_td2_ecmp_rh_info[unit]->
                        num_ecmp_rh_flowset_blocks),
                    "ecmp_rh_flowset_block_bitmap");
        if (_td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap == NULL) {
            bcm_td2_ecmp_rh_deinit(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_td2_ecmp_rh_info[unit]->ecmp_rh_flowset_block_bitmap, 0,
                SHR_BITALLOCSIZE(_td2_ecmp_rh_info[unit]->
                    num_ecmp_rh_flowset_blocks));
    }

    /* Set the seed for the pseudo-random number generator */
    _td2_ecmp_rh_info[unit]->ecmp_rh_rand_seed = sal_time_usecs();

    /* Clear flow set table entries */
    SOC_IF_ERROR_RETURN
        (soc_mem_clear(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL, TRUE));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_dynamic_size_encode
 * Purpose:
 *      Encode ECMP resilient hashing flow set size.
 * Parameters:
 *      dynamic_size - (IN) Number of flow sets.
 *      encoded_value - (OUT) Encoded value.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_dynamic_size_encode(int unit, int dynamic_size, int *encoded_value)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return _bcm_monterey_ecmp_rh_dynamic_size_encode(dynamic_size,
                                                        encoded_value);
    }
#endif
    switch (dynamic_size) {
        case 64:
            *encoded_value = 1;
            break;
        case 128:
            *encoded_value = 2;
            break;
        case 256:
            *encoded_value = 3;
            break;
        case 512:
            *encoded_value = 4;
            break;
        case 1024:
            *encoded_value = 5;
            break;
        case 2048:
            *encoded_value = 6;
            break;
        case 4096:
            *encoded_value = 7;
            break;
        case 8192:
            *encoded_value = 8;
            break;
        case 16384:
            *encoded_value = 9;
            break;
        case 32768:
            *encoded_value = 10;
            break;
        case 65536:
            *encoded_value = 11;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_dynamic_size_decode
 * Purpose:
 *      Decode ECMP resilient hashing flow set size.
 * Parameters:
 *      encoded_value - (IN) Encoded value.
 *      dynamic_size - (OUT) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_dynamic_size_decode(int unit, int encoded_value, int *dynamic_size)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return _bcm_monterey_ecmp_rh_dynamic_size_decode(encoded_value,
                                                        dynamic_size);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return _bcm_hx5_ecmp_rh_dynamic_size_decode(encoded_value,
                                                    dynamic_size);
    }
#endif /* BCM_HELIX5_SUPPORT */
    switch (encoded_value) {
        case 1:
            *dynamic_size = 64;
            break;
        case 2:
            *dynamic_size = 128;
            break;
        case 3:
            *dynamic_size = 256;
            break;
        case 4:
            *dynamic_size = 512;
            break;
        case 5:
            *dynamic_size = 1024;
            break;
        case 6:
            *dynamic_size = 2048;
            break;
        case 7:
            *dynamic_size = 4096;
            break;
        case 8:
            *dynamic_size = 8192;
            break;
        case 9:
            *dynamic_size = 16384;
            break;
        case 10:
            *dynamic_size = 32768;
            break;
        case 11:
            *dynamic_size = 65536;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_free_resource
 * Purpose:
 *      Free resources for an ECMP resilient hashing group.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      ecmp_group - (IN) ECMP group ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_free_resource(int unit, int ecmp_group)
{
    int rv = BCM_E_NONE;
    ecmp_count_entry_t ecmp_count_entry;
    int flow_set_size;
    int entry_base_ptr;
    int num_entries;
    int block_base_ptr;
    int num_blocks;
    int alloc_size;
    uint32 *buf_ptr = NULL;
    int index_min, index_max, enhanced_hashing = 1;
    soc_field_t enhanced_hashing_fld;
    uint32 rval;

    if (soc_reg_field_valid(unit, ENHANCED_HASHING_CONTROLr, RH_DLB_SELECTIONf)) {
        SOC_IF_ERROR_RETURN(READ_ENHANCED_HASHING_CONTROLr(unit, &rval));
        if (0 == soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,
                RH_DLB_SELECTIONf)) {
           /* ECMP resilient hashing is not enabled chip-wide. */
            return BCM_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                ecmp_group, &ecmp_count_entry));
    if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
        enhanced_hashing_fld = LB_MODEf;
        enhanced_hashing = 4;
    } else {
        enhanced_hashing_fld = ENHANCED_HASHING_ENABLEf;
    }
    if (soc_L3_ECMP_COUNTm_field32_get(unit,
            &ecmp_count_entry, enhanced_hashing_fld)!= enhanced_hashing) {
        /* Either (1) resilient hashing is not enabled on this ECMP
         * group, or (2) resilient hashing is enabled with no members.
         * For case (2), L3_ECMP_COUNT.FLOW_SET_SIZE needs to be cleared.
         */
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
                RH_FLOW_SET_SIZEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL,
                    ecmp_group, &ecmp_count_entry));
        return BCM_E_NONE;
    }
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit,
            &ecmp_count_entry, RH_FLOW_SET_SIZEf);
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit,
            &ecmp_count_entry, RH_FLOW_SET_BASEf);

    /* Clear resilient hashing fields */
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
           enhanced_hashing_fld, 0);
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf, 0);
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL,
                ecmp_group, &ecmp_count_entry));

    /* Clear flow set table entries */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries));
    alloc_size = num_entries * sizeof(rh_ecmp_flowset_entry_t);
    buf_ptr = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == buf_ptr) {
        return BCM_E_MEMORY;
    }
    sal_memset(buf_ptr, 0, alloc_size);
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, buf_ptr);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, buf_ptr);
        return rv;
    }
    soc_cm_sfree(unit, buf_ptr);

    /* Free flow set table resources */
    block_base_ptr = entry_base_ptr >> \
                     _td2_ecmp_rh_info[unit]->ecmp_block_size ;
    num_blocks = num_entries >> \
                 _td2_ecmp_rh_info[unit]->ecmp_block_size;
    _BCM_ECMP_RH_FLOWSET_BLOCK_USED_CLR_RANGE(unit, block_base_ptr, num_blocks);


#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        ecmp_grp_enhanced_hashing[unit][ecmp_group] = 0;
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_dynamic_size_set
 * Purpose:
 *      Set the dynamic size for an ECMP resilient hashing group with
 *      no members.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      ecmp_group - (IN) ECMP group ID.
 *      dynamic_size - (IN) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_dynamic_size_set(int unit, int ecmp_group, int dynamic_size)
{
    ecmp_count_entry_t ecmp_count_entry;
    int flow_set_size, enhanced_hashing = 1;
    soc_field_t enhanced_hashing_fld;

    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group,
                &ecmp_count_entry));
    if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
       enhanced_hashing_fld = LB_MODEf;
      enhanced_hashing = 4;
    } else {
       enhanced_hashing_fld = ENHANCED_HASHING_ENABLEf;
    }
    if (soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
               enhanced_hashing_fld) == enhanced_hashing) {
        /* Not expecting resilient hashing to be enabled */
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_ecmp_rh_dynamic_size_encode(unit, dynamic_size, &flow_set_size));
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry, RH_FLOW_SET_SIZEf,
            flow_set_size);
    SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL, ecmp_group,
                &ecmp_count_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_rand_get
 * Purpose:
 *      Get a random number between 0 and the given max value.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      rand_max - (IN) Maximum random number.
 *      rand_num - (OUT) Random number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This procedure uses the pseudo-random number generator algorithm
 *      suggested by the C standard.
 */
STATIC int
_bcm_td2_ecmp_rh_rand_get(int unit, int rand_max, int *rand_num)
{
    int modulus;
    int rand_seed_shift;

    if (rand_max < 0) {
        return BCM_E_PARAM;
    }

    if (NULL == rand_num) {
        return BCM_E_PARAM;
    }

    /* Make sure the modulus does not exceed limit. For instance,
     * if the 32-bit rand_seed is shifted to the right by 16 bits before
     * the modulo operation, the modulus should not exceed 1 << 16.
     */
    modulus = rand_max + 1;
    rand_seed_shift = _td2_ecmp_rh_info[unit]->ecmp_rand_seed_shift;
    if (modulus > (1 << (32 - rand_seed_shift))) {
        return BCM_E_PARAM;
    }

    _td2_ecmp_rh_info[unit]->ecmp_rh_rand_seed =
        _td2_ecmp_rh_info[unit]->ecmp_rh_rand_seed * 1103515245 + 12345;

    *rand_num = (_td2_ecmp_rh_info[unit]->ecmp_rh_rand_seed >> rand_seed_shift) %
                modulus;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_member_choose
 * Purpose:
 *      Choose a member of the ECMP group.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      num_members - (IN) Number of members to choose from.
 *      entry_count_arr   - (IN/OUT) An array keeping track of how many
 *                                   flow set entries have been assigned
 *                                   to each member.
 *      max_entry_count   - (IN/OUT) The maximum number of flow set entries
 *                                   that can be assigned to a member. 
 *      chosen_index      - (OUT) The index of the chosen member.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_member_choose(int unit, int num_members, int *entry_count_arr,
        int *max_entry_count, int *chosen_index)
{
    int member_index;
    int next_index;

    *chosen_index = 0;

    /* Choose a random member index */
    BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_rand_get(unit, num_members - 1,
                &member_index));

    if (entry_count_arr[member_index] < *max_entry_count) {
        entry_count_arr[member_index]++;
        *chosen_index = member_index;
    } else {
        /* The randomly chosen member has reached the maximum
         * flow set entry count. Choose the next member that
         * has not reached the maximum entry count.
         */
        next_index = (member_index + 1) % num_members;
        while (next_index != member_index) {
            if (entry_count_arr[next_index] < *max_entry_count) {
                entry_count_arr[next_index]++;
                *chosen_index = next_index;
                break;
            } else {
                next_index = (next_index + 1) % num_members;
            }
        }
        if (next_index == member_index) {
            /* All members have reached the maximum flow set entry
             * count. This scenario occurs when dividing the number of
             * flow set entries by the number of members results
             * in a non-zero remainder. The remainder flow set entries
             * will be distributed among members, at most 1 remainder
             * entry per member.
             */
            (*max_entry_count)++;
            if (entry_count_arr[member_index] < *max_entry_count) {
                entry_count_arr[member_index]++;
                *chosen_index = member_index;
            } else {
                /* It's possible that the member's entry count equals
                 * to the incremented value of max_entry_count, when
                 * this procedure is invoked by
                 * _bcm_td2_ecmp_rh_populate_empty_entries.
                 */
                next_index = (member_index + 1) % num_members;
                while (next_index != member_index) {
                    if (entry_count_arr[next_index] < *max_entry_count) {
                        entry_count_arr[next_index]++;
                        *chosen_index = next_index;
                        break;
                    } else {
                        next_index = (next_index + 1) % num_members;
                    }
                }
                if (next_index == member_index) {
                    return BCM_E_INTERNAL;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_set
 * Purpose:
 *      Configure an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_set(int unit,
        bcm_l3_egress_ecmp_t *ecmp,
        int intf_count,
        bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int num_blocks;
    int total_blocks;
    int max_block_base_ptr;
    int block_base_ptr = 0;
    int entry_base_ptr;
    SHR_BITDCL occupied;
    int alloc_size;
    uint32 *buf_ptr = NULL;
    int *entry_count_arr = NULL;
    int max_entry_count;
    int chosen_index;
    int i;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int offset;
    int next_hop_index;
    int index_min, index_max;
    ecmp_count_entry_t ecmp_count_entry;
    int flow_set_size, enhanced_hashing = 1;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    _bcm_l3_bookkeeping_t *l3_bk_info = &_bcm_l3_bk_info[unit];
#endif
    soc_field_t enhanced_hashing_fld;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (intf_count == 0) {
        /* Store the dynamic_size in hardware */
        BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_dynamic_size_set(unit,
                    ecmp_group, ecmp->dynamic_size));
        return BCM_E_NONE;
    }

    /* Find a contiguous region of flow set table that's large
     * enough to hold the requested number of flow sets.
     * The flow set table is allocated in 64-entry blocks.
     */
    num_blocks = ecmp->dynamic_size >> _td2_ecmp_rh_info[unit]->ecmp_block_size;
    total_blocks = _td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks;
    LOG_DEBUG(BSL_LS_SOC_L3, (BSL_META_U(unit,
        "Total_blocks = %d , num_blocks = %d\n "), total_blocks, num_blocks));

    if (num_blocks > total_blocks) {
        return BCM_E_RESOURCE;
    }
    max_block_base_ptr = total_blocks - num_blocks;

    /* 
     * In case of Riot or Multi-Layer ECMP, RH_ECMP_FLOWSET table is divided into two halves.
     * One half goes to overlay and other goes to underlay range.
     * When ECMP is created then user tells the software that which range
     * should be used. SDK will test the blocks only in that range.
     */ 
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    if (
#if defined(BCM_RIOT_SUPPORT)
        (l3_bk_info->riot_enable) ||
#endif
        BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
        if (ecmp->ecmp_group_flags & BCM_L3_ECMP_OVERLAY) {
            block_base_ptr = 0;
            max_block_base_ptr = (l3_bk_info->l3_ecmp_rh_overlay_entries >> \
                                  _td2_ecmp_rh_info[unit]->ecmp_block_size)
                                  - num_blocks;
            LOG_DEBUG(BSL_LS_SOC_L3, (BSL_META_U(unit,
                "Total_blocks = %d , num_blocks = %d\n "),
                total_blocks, num_blocks));

        } else {
            block_base_ptr = (l3_bk_info->l3_ecmp_rh_overlay_entries >> \
                              _td2_ecmp_rh_info[unit]->ecmp_block_size);
            max_block_base_ptr = total_blocks;
        }
    }
#endif
    for (; block_base_ptr <= max_block_base_ptr; block_base_ptr++) {
        /* Check if the contiguous region of flow set table from
         * block_base_ptr to (block_base_ptr + num_blocks - 1) is free. 
         */
        _BCM_ECMP_RH_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
                occupied); 
        if (!occupied) {
            break;
        }
    }
    if (block_base_ptr > max_block_base_ptr) {
        /* A contiguous region of the desired size could not be found in
         * flow set table.
         */
        return BCM_E_RESOURCE;
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
        if (soc_mem_index_count(unit, RH_ECMP_FLOWSETm)) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROLr,
                         REG_PORT_ANY, RH_DLB_SELECTIONf, 1));

        }
    }
#endif

    /* Configure flow set table */
    alloc_size = ecmp->dynamic_size * sizeof(rh_ecmp_flowset_entry_t);
    buf_ptr = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == buf_ptr) {
        return BCM_E_MEMORY;
    }
    sal_memset(buf_ptr, 0, alloc_size);

    entry_count_arr = sal_alloc(sizeof(int) * intf_count,
            "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        soc_cm_sfree(unit, buf_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(entry_count_arr, 0, sizeof(int) * intf_count);
    max_entry_count = ecmp->dynamic_size / intf_count;

    for (i = 0; i < ecmp->dynamic_size; i++) {
        /* Choose a member of the ECMP */
        rv = _bcm_td2_ecmp_rh_member_choose(unit, intf_count,
                entry_count_arr, &max_entry_count, &chosen_index);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buf_ptr);
            sal_free(entry_count_arr);
            return rv;
        }

        /* Set flow set entry */
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, buf_ptr, i);
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry, VALIDf, 1);
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    intf_array[chosen_index])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit,
                    intf_array[chosen_index])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            soc_cm_sfree(unit, buf_ptr);
            sal_free(entry_count_arr);
            return BCM_E_PARAM;
        }
        next_hop_index = intf_array[chosen_index] - offset;
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                NEXT_HOP_INDEXf, next_hop_index);
    }

    entry_base_ptr = block_base_ptr << _td2_ecmp_rh_info[unit]->ecmp_block_size;
    index_min = entry_base_ptr;
    index_max = index_min + ecmp->dynamic_size - 1;
    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, buf_ptr);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, buf_ptr);
        sal_free(entry_count_arr);
        return rv;
    }
    soc_cm_sfree(unit, buf_ptr);
    sal_free(entry_count_arr);

    _BCM_ECMP_RH_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);

    /* Update ECMP group table */
    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group,
                &ecmp_count_entry));
    if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
        enhanced_hashing_fld = LB_MODEf;
        enhanced_hashing = 4;
    } else {
        enhanced_hashing_fld = ENHANCED_HASHING_ENABLEf;
    }
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
           enhanced_hashing_fld, enhanced_hashing);
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf, entry_base_ptr);
    BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_dynamic_size_encode(
                unit, ecmp->dynamic_size, &flow_set_size));
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf, flow_set_size);
    SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL, ecmp_group,
                &ecmp_count_entry));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        ecmp_grp_enhanced_hashing[unit][ecmp_group] = 1;
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_member_replica_find
 * Purpose:
 *      For each member in the member_array, find others members
 *      who share the same next hop index.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_members - (IN) Number of members.
 *      member_array - (IN/OUT) Array of members.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_member_replica_find(int unit, int num_members,
        _td2_ecmp_rh_member_t *member_array)
{
    int i, k;

    if (num_members < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < num_members - 1; i++) {
        if (member_array[i].replica_id == 0) {
            /* replica_id == 0 indicates first replica.
             * Search for other replicas in subsequent members.
             */
            for (k = i + 1; k < num_members; k++) {
                if (member_array[k].nh_index == member_array[i].nh_index) {
                    /* Replica found. Assign replica id. */
                    member_array[k].replica_id = member_array[i].num_replica;

                    /* For non-first replicas, set non-applicable fields to -1. */
                    member_array[k].num_replica = -1;
                    member_array[k].next_replica_id = -1;

                    /* Increment the num_replica field of the first replica */
                    member_array[i].num_replica++;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_member_id_buf_assign
 * Purpose:
 *      For each entry in the flow set entry buffer, match its next hop
 *      index against the next hop index of a member, then assign the
 *      matching member's member ID to the entry.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_members - (IN) Number of members.
 *      member_array - (IN/OUT) Array of members.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN) Flow set entry buffer.
 *      member_id_buf - (OUT) Member ID buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_member_id_buf_assign(int unit, int num_members,
        _td2_ecmp_rh_member_t *member_array, int num_entries,
        uint32 *flowset_buf, int *member_id_buf)
{
    int i, k, m;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int next_hop_index;

    for (i = 0; i < num_entries; i++) {
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, flowset_buf, i);
        if (!soc_mem_field32_get(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    VALIDf)) {
            return BCM_E_INTERNAL;
        }
        next_hop_index = soc_mem_field32_get(unit, RH_ECMP_FLOWSETm,
                flowset_entry, NEXT_HOP_INDEXf);

        /* Search the member_array for matching next hop index */
        for (k = 0; k < num_members; k++) {
            if (member_array[k].nh_index == next_hop_index) {
                if (member_array[k].num_replica == 1) {
                    /* The next hop index is not shared by other members. */
                    member_id_buf[i] = member_array[k].member_id;
                    break;
                }

                /* Else there are more than one members sharing the next
                 * hop index. Assign the member ID of the replica
                 * indicated by next_replica_id.
                 */
                for (m = k; m < num_members; m++) {
                    if ((member_array[m].nh_index == next_hop_index) &&
                        (member_array[m].replica_id == member_array[k].next_replica_id)) {
                        member_id_buf[i] = member_array[m].member_id;
                        break;
                    }
                }
                if (m == num_members) {
                    /* Cannot find a matching replica */
                    return BCM_E_INTERNAL;
                }

                /* Increment the next_replica_id field of the first replica */
                member_array[k].next_replica_id = (member_array[k].next_replica_id + 1) %
                    member_array[k].num_replica;

                break;
            }
        }
        if (k == num_members) {
            /* Cannot find a member with matching next hop index */
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_add_rebalance
 * Purpose:
 *      Re-balance flow set entries from existing members to the new member.
 *      For example, if the number of flow set entries is 64, and the number of
 *      existing members is 6, then each existing member has between 10 and 11
 *      entries. The entries should be re-assigned from the 6 existing members
 *      to the new member such that each member will end up with between 9 and
 *      10 entries.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN/OUT) Flow set entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      num_members - (IN) Number of members in member_array.
 *      member_array - (IN) Array of existing members.
 *      entry_count_array - (IN/OUT) Array of entry counts of existing members.
 *      new_member - (IN) The member to be added.
 *      entry_count - (OUT) Entry count of the new member.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_add_rebalance(int unit,
        int num_entries, uint32 *flowset_buf, int *member_id_buf,
        int num_members, _td2_ecmp_rh_member_t *member_array,
        int *entry_count_array,
        _td2_ecmp_rh_member_t *new_member, int *entry_count)
{
    int lower_bound, upper_bound;
    int threshold;
    int entry_index, next_entry_index;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int member_id;
    int is_new_member;
    int member_index = 0;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == flowset_buf || NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (num_members < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }
    if (NULL == new_member) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count) {
        return BCM_E_PARAM;
    }

    lower_bound = num_entries / (num_members + 1);
    upper_bound = (num_entries % (num_members + 1)) ?
                  (lower_bound + 1) : lower_bound;
    threshold = upper_bound;
    *entry_count = 0;
    while (*entry_count < lower_bound) {
        /* Pick a random entry in the flow set entry buffer */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_ecmp_rh_rand_get(unit, num_entries - 1, &entry_index));
        flowset_entry = soc_mem_table_idx_to_pointer(unit, RH_ECMP_FLOWSETm,
                rh_ecmp_flowset_entry_t *, flowset_buf, entry_index);
        member_id = member_id_buf[entry_index];

        /* Determine if the randomly picked entry contains the new member.
         * If not, determine the existing member index.
         */
        if (member_id == new_member->member_id) {
            is_new_member = TRUE;
        } else {
            is_new_member = FALSE;

            /* In the member_array, it's assumed that each array element's
             * member_id matches the element's array index.
             */
            member_index = member_id;
        }

        if (!is_new_member && (entry_count_array[member_index] > threshold)) {
            soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    NEXT_HOP_INDEXf, new_member->nh_index);
            member_id_buf[entry_index] = new_member->member_id;
            entry_count_array[member_index]--;
            (*entry_count)++;
        } else {
            /* Either the member of the randomly chosen entry is
             * the same as the new member, or the member is an existing
             * member and its entry count has decreased to threshold.
             * In both cases, find the next entry that contains a
             * member that's not the new member and whose entry count
             * has not decreased to threshold.
             */
            next_entry_index = (entry_index + 1) % num_entries;
            while (next_entry_index != entry_index) {
                flowset_entry = soc_mem_table_idx_to_pointer(unit,
                        RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *,
                        flowset_buf, next_entry_index);
                member_id = member_id_buf[next_entry_index];

                /* Determine if the entry contains the new member.
                 * If not, determine the existing member index.
                 */
                if (member_id == new_member->member_id) {
                    is_new_member = TRUE;
                } else {
                    is_new_member = FALSE;

                    /* In the member_array, it's assumed that each array element's
                     * member_id matches the array index of the element.
                     */
                    member_index = member_id;
                }

                if (!is_new_member &&
                        (entry_count_array[member_index] > threshold)) {
                    soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                            NEXT_HOP_INDEXf, new_member->nh_index);
                    member_id_buf[next_entry_index] = new_member->member_id;
                    entry_count_array[member_index]--;
                    (*entry_count)++;
                    break;
                } else {
                    next_entry_index = (next_entry_index + 1) % num_entries;
                }
            }
            if (next_entry_index == entry_index) {
                /* The entry count of all existing members has decreased
                 * to threshold. The entry count of the new member has
                 * not yet increased to lower_bound. Lower the threshold.
                 */
                threshold--;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_clear_by_member_id_th
 * Purpose:
 *      In the given flow set entry buffer, clear entries whose
 *      member ID is greater than or equal to the given member ID threshold.
 *      Also, keep track of entry count for each member ID smaller
 *      than the given member ID threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN/OUT) Flow set entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      member_id_th - (IN) Member ID threshold.
 *      array_size - (IN) Number of elements in entry_count_array.
 *      entry_count_array - (OUT) Array of entry counts for member IDs not cleared.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_clear_by_member_id_th(int unit,
        int num_entries, uint32 *flowset_buf, int *member_id_buf,
        int member_id_th, int array_size, int *entry_count_array)
{
    int i;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int member_id;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == flowset_buf || NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (member_id_th != array_size) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < num_entries; i++) {
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, flowset_buf, i);
        if (!soc_mem_field32_get(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    VALIDf)) {
            return BCM_E_INTERNAL;
        }
        member_id = member_id_buf[i];

        if (member_id >= member_id_th) {
            /* Clear the valid bit */
            soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    VALIDf, 0);

            /* Do not clear the next hop index, which is needed later by
             * _bcm_td2_ecmp_rh_populate_empty_entries.
             */

            /* Clear member ID */
            member_id_buf[i] = -1;
        } else {
            entry_count_array[member_id]++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_populate_empty_entries
 * Purpose:
 *      In the given flow set entry buffer, populate entries not
 *      containing a member, such that maximal balance is achieve
 *      among members in member_array.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN/OUT) Flow set entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      num_members - (IN) Number of elements in member_array and
 *                         entry_count_array.
 *      member_array - (IN) Array of members.
 *      entry_count_array - (IN/OUT) Array of entry counts.
 *      num_shared_members - (IN) Number of members in member_array that
 *                                are shared by the old and the new ECMP
 *                                groups.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_populate_empty_entries(int unit,
        int num_entries, uint32 *flowset_buf, int *member_id_buf,
        int num_members, _td2_ecmp_rh_member_t *member_array,
        int *entry_count_array, int num_shared_members)
{
    int max_entry_count;
    int i, k;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int next_hop_index;
    int chosen_index = 0;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == flowset_buf || NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (0 == num_members) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }
    if (num_shared_members > num_members) {
        return BCM_E_PARAM;
    }

    max_entry_count = num_entries / num_members;
    for (i = 0; i < num_entries; i++) {
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, flowset_buf, i);
        if (soc_mem_field32_get(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    VALIDf)) {
            /* Skip valid entries */
            continue;
        }

        /* First, try to find a shared member whose next hop index
         * matches the current entry and whose entry count has not reached
         * max_entry_count. Doing so minimizes flow-to-member reassignments.
         */
        next_hop_index = soc_mem_field32_get(unit, RH_ECMP_FLOWSETm,
                flowset_entry, NEXT_HOP_INDEXf);
        for (k = 0; k < num_shared_members; k++) {
            if (member_array[k].nh_index == next_hop_index) {
                if (entry_count_array[k] < max_entry_count) {
                    entry_count_array[k]++;
                    chosen_index = k;
                    break;
                }
            }
        }
        if (k == num_shared_members) {
            /* Cannot find a shared member with matching next hop index and
             * with entry count < max_entry_count. Randomly choose a member.
             */
            BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_member_choose(unit, num_members,
                        entry_count_array, &max_entry_count, &chosen_index));
        }

        /* Set flow set entry */
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry, VALIDf, 1);
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                NEXT_HOP_INDEXf, member_array[chosen_index].nh_index);
        member_id_buf[i] = member_array[chosen_index].member_id;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_balance_with_min_shared_mod
 * Purpose:
 *      Modify the given flow set entry buffer to achieve balance among
 *      the shared members and members exclusive to the new ECMP group,
 *      while keeping modification of entries containing the
 *      shared members to a minimum.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN/OUT) Flow set entry buffer. Initially, this buffer
 *                             contains the old ECMP group's flow set entries.
 *      shared_intf_count - (IN) Number of members shared by old and new ECMP
 *                               groups.
 *      shared_intf_array - (IN) Array of members shared by old and new ECMP
 *                               groups.
 *      ex_old_intf_count - (IN) Number of members exclusive to old ECMP group.
 *      ex_old_intf_array - (IN) Array of members exclusive to old ECMP group.
 *      ex_new_intf_count - (IN) Number of members exclusive to new ECMP group.
 *      ex_new_intf_array - (IN) Array of members exclusive to new ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_balance_with_min_shared_mod(int unit,
        int num_entries, uint32 *flowset_buf,
        int shared_intf_count, bcm_if_t *shared_intf_array,
        int ex_old_intf_count, bcm_if_t *ex_old_intf_array,
        int ex_new_intf_count, bcm_if_t *ex_new_intf_array)
{
    int rv = BCM_E_NONE;
    int old_intf_count;
    int new_intf_count;
    int num_members;
    int alloc_size;
    _td2_ecmp_rh_member_t *member_array = NULL;
    int *entry_count_array = NULL;
    int i;
    int ex_old_index, ex_new_index;
    int *member_id_buf = NULL;
    int intf_count;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == flowset_buf) {
        return BCM_E_PARAM;
    }
    if (0 == shared_intf_count) {
        return BCM_E_PARAM;
    }
    if (NULL == shared_intf_array) {
        return BCM_E_PARAM;
    }
    if ((ex_old_intf_count > 0) && (NULL == ex_old_intf_array)) {
        return BCM_E_PARAM;
    }
    if ((ex_new_intf_count > 0) && (NULL == ex_new_intf_array)) {
        return BCM_E_PARAM;
    }

    /* Allocate an array of members and an array of entry counts */
    old_intf_count = shared_intf_count + ex_old_intf_count;
    new_intf_count = shared_intf_count + ex_new_intf_count;
    num_members = (old_intf_count > new_intf_count) ? old_intf_count : new_intf_count;
    alloc_size = num_members * sizeof(_td2_ecmp_rh_member_t);
    member_array = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == member_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_array, 0, alloc_size);

    alloc_size = num_members * sizeof(int);
    entry_count_array = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_array, 0, alloc_size);

    /* Initialize the member_array with shared members, followed by members
     * exclusive to the old ECMP group.
     */
    for (i = 0; i < shared_intf_count; i++) {
        member_array[i].nh_index = shared_intf_array[i];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }
    for (i = shared_intf_count; i < old_intf_count; i++) {
        ex_old_index = i - shared_intf_count;
        member_array[i].nh_index = ex_old_intf_array[ex_old_index];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_td2_ecmp_rh_member_replica_find(unit, old_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the flow set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_td2_ecmp_rh_member_id_buf_assign(unit, old_intf_count,
            member_array, num_entries, flowset_buf, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Traverse the flow set entry buffer to clear entries with member
     * IDs greater than the member IDs of the shared members. Also keep
     * track of each shared member's entry count.
     */
    rv = _bcm_td2_ecmp_rh_clear_by_member_id_th(unit, num_entries, flowset_buf,
            member_id_buf, shared_intf_count, shared_intf_count,
            entry_count_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Clear from the member_array members exclusive to the old ECMP group.
     * Also need to reset share members' replica info.
     */
    for (i = shared_intf_count; i < old_intf_count; i++) {
        sal_memset(&member_array[i], 0, sizeof(_td2_ecmp_rh_member_t));
    }
    for (i = 0; i < shared_intf_count; i++) {
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Add to member_array members exclusive to the new ECMP group */
    for (i = shared_intf_count; i < new_intf_count; i++) {
        ex_new_index = i - shared_intf_count;
        member_array[i].nh_index = ex_new_intf_array[ex_new_index];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_td2_ecmp_rh_member_replica_find(unit, new_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Populate the cleared entries in the flow set entry buffer.
     * There're 2 cases:
     *
     * Case 1:
     * If the number of members in the old ECMP group is greater than or
     * equal to the number in the new ECMP group, populate the cleared
     * entries with shared members and members exclusive to the new ECMP
     * group, such that maximal balance is achieved among them.
     *
     * Case 2:
     * If the old ECMP group has fewer members than the new ECMP group,
     * populate the cleared entries with N members that are exclusive
     * to the new ECMP group, such that N + number of shared members equals
     * to the number of members in old ECMP group.
     *
     * In both cases, the flow set entries containing the shared members
     * are unmodified.
     */
    if (old_intf_count >= new_intf_count) {
        intf_count = new_intf_count;
    } else {
        intf_count = old_intf_count;
    }
    rv = _bcm_td2_ecmp_rh_populate_empty_entries(unit, num_entries, flowset_buf,
            member_id_buf, intf_count, member_array, entry_count_array,
            shared_intf_count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* For any remaining unused members in member_array, move some
     * flow set entries from existing members to these remaining
     * members, such that maximal balance is achieved.
     */
    if (new_intf_count > intf_count) {
        for (i = intf_count; i < new_intf_count; i++) {
            rv = _bcm_td2_ecmp_rh_add_rebalance(unit, num_entries, flowset_buf,
                    member_id_buf, i, member_array, entry_count_array,
                    &member_array[i], &entry_count_array[i]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (member_array) {
        sal_free(member_array);
    }
    if (entry_count_array) {
        sal_free(entry_count_array);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_arrange_with_no_shared_entries
 * Purpose:
 *      Arrange the given flow set entry buffer to achieve Maximization of the shared members,
 *      while entries containing none of shared members.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of flow set entries in buffer.
 *      flowset_buf - (IN/OUT) Flow set entry buffer. Initially, this buffer
 *                             contains the old ECMP group's flow set entries.
 *      old_intf_count - (IN) Number of members exclusive to old ECMP group.
 *      old_intf_array - (IN) Array of members exclusive to old ECMP group.
 *      new_intf_count - (IN) Number of members exclusive to new ECMP group.
 *      new_intf_array - (IN) Array of members exclusive to new ECMP group.
 *      shared_intf_count - (OUT) Number of members shared by old and new ECMP
 *                               groups.
 *      shared_intf_array - (OUT) Array of members shared by old and new ECMP
 *                               groups.
 *      ex_old_intf_count - (OUT) Number of members exclusive to old ECMP group.
 *      ex_old_intf_array - (OUT) Array of members exclusive to old ECMP group.
 *      ex_new_intf_count - (OUT) Number of members exclusive to new ECMP group.
 *      ex_new_intf_array - (OUT) Array of members exclusive to new ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_ecmp_rh_arrange_with_no_shared_entries(int unit,
        int num_entries, uint32 *flowset_buf,
        int old_intf_count, bcm_if_t *old_intf_array,
        int new_intf_count, bcm_if_t *new_intf_array,
        int *shared_intf_count, bcm_if_t *shared_intf_array,
        int *ex_old_intf_count, bcm_if_t *ex_old_intf_array,
        int *ex_new_intf_count, bcm_if_t *ex_new_intf_array)
{
    int rv = BCM_E_NONE;
    int num_members;
    int alloc_size;
    _td2_ecmp_rh_member_t *member_array = NULL;
    int i;
    int *member_id_buf = NULL;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int member_id;
    int max_shared;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == flowset_buf) {
        return BCM_E_PARAM;
    }
    if (old_intf_count > 0 && old_intf_array == NULL) {
        return BCM_E_PARAM;
    }
    if (new_intf_count > 0 && new_intf_array == NULL) {
        return BCM_E_PARAM;
    }
    if (NULL == shared_intf_array) {
        return BCM_E_PARAM;
    }
    if (NULL == ex_old_intf_array) {
        return BCM_E_PARAM;
    }
    if (NULL == ex_new_intf_array) {
        return BCM_E_PARAM;
    }

    /* Allocate an array of members and an array of entry counts */
    num_members = old_intf_count;
    alloc_size = num_members * sizeof(_td2_ecmp_rh_member_t);
    member_array = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == member_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_array, 0, alloc_size);

    /* Initialize the member_array with shared members, followed by members
     * exclusive to the old ECMP group.
     */
    for (i = 0; i < num_members; i++) {
        member_array[i].nh_index = old_intf_array[i];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_td2_ecmp_rh_member_replica_find(unit, old_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the flow set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_td2_ecmp_rh_member_id_buf_assign(unit, num_members,
            member_array, num_entries, flowset_buf, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > new_intf_count) ? new_intf_count :
        old_intf_count;

    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];
        if (member_id >= max_shared) {
            continue;
        }
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, flowset_buf, i);
        if (!soc_mem_field32_get(unit, RH_ECMP_FLOWSETm, flowset_entry,
                    VALIDf)) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                NEXT_HOP_INDEXf, new_intf_array[member_id]);
    }

    *shared_intf_count = max_shared;
    sal_memcpy(shared_intf_array, new_intf_array, 
                                *shared_intf_count * sizeof(bcm_if_t));

    if (old_intf_count > new_intf_count) {
        *ex_new_intf_count = 0;
        *ex_old_intf_count = old_intf_count - *shared_intf_count;
        sal_memcpy(ex_old_intf_array, &old_intf_array[*shared_intf_count], 
                                    *ex_old_intf_count * sizeof(bcm_if_t));
    } else {
        *ex_old_intf_count = 0;
        *ex_new_intf_count = new_intf_count - *shared_intf_count;
        sal_memcpy(ex_new_intf_array, &new_intf_array[*shared_intf_count], 
                                    *ex_new_intf_count * sizeof(bcm_if_t));
    }

cleanup:
    if (member_array) {
        sal_free(member_array);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_add
 * Purpose:
 *      Add a member to an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of resilient hashing eligible members,
 *                        including the member to be added.
 *      new_intf   - (IN) New member to be added.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_td2_ecmp_rh_add(int unit,
        bcm_l3_egress_ecmp_t *ecmp,
        int intf_count,
        bcm_if_t *intf_array,
        bcm_if_t new_intf)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int offset;
    int new_next_hop_index;
    int num_existing_members;
    ecmp_count_entry_t ecmp_count_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int alloc_size;
    uint32 *flowset_buf = NULL;
    int index_min, index_max;
    _td2_ecmp_rh_member_t *existing_member_arr = NULL;
    int i;
    int *member_id_buf = NULL;
    int *entry_count_arr = NULL;
    int member_id;
    int lower_bound, upper_bound;
    _td2_ecmp_rh_member_t new_member;
    int new_member_entry_count;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    if (intf_count == 0 || intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, new_intf)) {
        offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, new_intf)) {
        offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }
    new_next_hop_index = new_intf - offset;

    /* Check that the new member is the last element of the array of
     * resilient hashing eligible members.
     */
    if (new_intf != intf_array[intf_count - 1]) {
        return BCM_E_PARAM;
    }
    num_existing_members = intf_count - 1;

    if (intf_count == 1) {
        /* Adding the first member is the same as setting one member */
        return _bcm_td2_ecmp_rh_set(unit, ecmp, intf_count, intf_array);
    }

    /* Read all the flow set entries of this ECMP resilient hash group */
    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group,
                &ecmp_count_entry));
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf);
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries));

    alloc_size = num_entries * sizeof(rh_ecmp_flowset_entry_t);
    flowset_buf = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == flowset_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(flowset_buf, 0, alloc_size);
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_read_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ANY,
            index_min, index_max, flowset_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Construct an array of existing members */
    alloc_size = num_existing_members * sizeof(_td2_ecmp_rh_member_t);
    existing_member_arr = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == existing_member_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(existing_member_arr, 0, alloc_size);
    for (i = 0; i < num_existing_members; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        existing_member_arr[i].nh_index = intf_array[i] - offset;
        existing_member_arr[i].member_id = i;
        existing_member_arr[i].num_replica = 1;
        existing_member_arr[i].replica_id = 0;
        existing_member_arr[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in existing_member_arr.
     */
    rv = _bcm_td2_ecmp_rh_member_replica_find(unit, num_existing_members,
            existing_member_arr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the flow set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_td2_ecmp_rh_member_id_buf_assign(unit, num_existing_members,
            existing_member_arr, num_entries, flowset_buf, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Compute the number of entries currently assigned to each
     * existing member.
     */
    alloc_size = num_existing_members * sizeof(int);
    entry_count_arr = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_arr, 0, alloc_size);
    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];

        /* In this procedure, the member_id happens to be the same as
         * the index into the existing_member_arr.
         */
        entry_count_arr[member_id]++;
    }

    /* Check that the distribution of flow set entries among existing members
     * is balanced. For instance, if the number of flow set entries is 64, and
     * the number of existing members is 6, then every member should have
     * between 10 and 11 entries. 
     */ 
    lower_bound = num_entries / num_existing_members;
    upper_bound = (num_entries % num_existing_members) ?
                  (lower_bound + 1) : lower_bound;
    for (i = 0; i < num_existing_members; i++) {
        if (entry_count_arr[i] < lower_bound ||
                entry_count_arr[i] > upper_bound) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
    }

    /* Re-balance flow set entries from existing members to the new member */
    sal_memset(&new_member, 0, sizeof(_td2_ecmp_rh_member_t));
    new_member.nh_index = new_next_hop_index;
    new_member.member_id = num_existing_members;
    rv = _bcm_td2_ecmp_rh_add_rebalance(unit, num_entries, flowset_buf,
            member_id_buf, num_existing_members, existing_member_arr,
            entry_count_arr, &new_member, &new_member_entry_count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Write re-balanced flow set entries to hardware */
    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, flowset_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (flowset_buf) {
        soc_cm_sfree(unit, flowset_buf);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }
    if (existing_member_arr) {
        sal_free(existing_member_arr);
    }
    if (entry_count_arr) {
        sal_free(entry_count_arr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_delete
 * Purpose:
 *      Delete a member from an ECMP resilient hashing group.
 * Parameters:
 *      unit         - (IN) SOC unit number. 
 *      ecmp         - (IN) ECMP group info.
 *      intf_count   - (IN) Number of elements in intf_array.
 *      intf_array   - (IN) Array of resilient hashing eligible members,
 *                        except the member to be deleted.
 *      leaving_intf - (IN) Member to delete.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_delete(int unit,
        bcm_l3_egress_ecmp_t *ecmp,
        int intf_count,
        bcm_if_t *intf_array,
        bcm_if_t leaving_intf)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int offset;
    int leaving_next_hop_index;
    ecmp_count_entry_t ecmp_count_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int alloc_size;
    uint32 *flowset_buf = NULL;
    int index_min, index_max;
    int num_existing_members;
    _td2_ecmp_rh_member_t *existing_member_arr = NULL;
    int i;
    int *member_id_buf = NULL;
    int *entry_count_arr = NULL;
    int lower_bound, upper_bound;
    int num_remaining_members;
    int threshold;
    int leaving_member_id;
    rh_ecmp_flowset_entry_t *flowset_entry;
    int member_id;
    int chosen_index;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, leaving_intf)) {
        offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, leaving_intf)) {
        offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }
    leaving_next_hop_index = leaving_intf - offset;

    if (intf_count == 0) {
        /* Deleting the last member is the same as freeing all resources */
        BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_free_resource(unit, ecmp_group));

        /* Store the dynamic_size in hardware */
        BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_dynamic_size_set(unit, ecmp_group,
                    ecmp->dynamic_size));

        return BCM_E_NONE;
    }

    /* Read all the flow set entries of this ECMP resilient hash group */
    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group,
                &ecmp_count_entry));
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf);
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries));

    alloc_size = num_entries * sizeof(rh_ecmp_flowset_entry_t);
    flowset_buf = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == flowset_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(flowset_buf, 0, alloc_size);
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_read_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ANY,
            index_min, index_max, flowset_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Construct an array of existing members */
    num_existing_members = intf_count + 1;
    alloc_size = num_existing_members * sizeof(_td2_ecmp_rh_member_t);
    existing_member_arr = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == existing_member_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(existing_member_arr, 0, alloc_size);
    for (i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        existing_member_arr[i].nh_index = intf_array[i] - offset;
        existing_member_arr[i].member_id = i;
        existing_member_arr[i].num_replica = 1;
        existing_member_arr[i].replica_id = 0;
        existing_member_arr[i].next_replica_id = 0;
    }
    existing_member_arr[intf_count].nh_index = leaving_next_hop_index;
    existing_member_arr[intf_count].member_id = intf_count;
    existing_member_arr[intf_count].num_replica = 1;
    existing_member_arr[intf_count].replica_id = 0;
    existing_member_arr[intf_count].next_replica_id = 0;

    /* Find members that share the same next hop index, and
     * update the replica information in existing_member_arr.
     */
    rv = _bcm_td2_ecmp_rh_member_replica_find(unit, num_existing_members,
            existing_member_arr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the flow set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_td2_ecmp_rh_member_id_buf_assign(unit, num_existing_members,
            existing_member_arr, num_entries, flowset_buf, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Compute the number of entries currently assigned to each
     * existing member.
     */
    alloc_size = num_existing_members * sizeof(int);
    entry_count_arr = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_arr, 0, alloc_size);
    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];

        /* In the existing_member_arr, each element's member_id is the
         * same as its array index.
         */
        entry_count_arr[member_id]++;
    }

    /* Check that the distribution of flow set entries among all members
     * is balanced. For instance, if the number of flow set entries is 64, and
     * the number of members is 6, then every member should have
     * between 10 and 11 entries. 
     */ 
    lower_bound = num_entries / num_existing_members;
    upper_bound = (num_entries % num_existing_members) ?
                  (lower_bound + 1) : lower_bound;
    for (i = 0; i < num_existing_members; i++) {
        if (entry_count_arr[i] < lower_bound ||
                entry_count_arr[i] > upper_bound) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
    }

    /* Re-balance flow set entries from the leaving member to the remaining
     * members. For example, if the number of flow set entries is 64, and
     * the number of members is 6, then each member has between 10 and 11
     * entries. The entries should be re-assigned from the leaving member to
     * the remaining 5 members such that each remaining member will end up
     * with between 12 and 13 entries.
     */
    num_remaining_members = num_existing_members - 1;
    lower_bound = num_entries / num_remaining_members;
    upper_bound = (num_entries % num_remaining_members) ?
                  (lower_bound + 1) : lower_bound;
    threshold = lower_bound;
    leaving_member_id = existing_member_arr[intf_count].member_id;
    for (i = 0; i < num_entries; i++) {
        flowset_entry = soc_mem_table_idx_to_pointer(unit,
                RH_ECMP_FLOWSETm, rh_ecmp_flowset_entry_t *, flowset_buf, i);
        member_id = member_id_buf[i];
        if (member_id != leaving_member_id) {
            continue;
        }

        /* Randomly choose a member among the remaining members */
        rv = _bcm_td2_ecmp_rh_member_choose(unit, num_remaining_members,
                entry_count_arr, &threshold, &chosen_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, RH_ECMP_FLOWSETm, flowset_entry,
                NEXT_HOP_INDEXf, existing_member_arr[chosen_index].nh_index);
        member_id_buf[i] = existing_member_arr[chosen_index].member_id;
    }

    /* Write re-balanced flow set entries to hardware */
    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, flowset_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (flowset_buf) {
        soc_cm_sfree(unit, flowset_buf);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }
    if (existing_member_arr) {
        sal_free(existing_member_arr);
    }
    if (entry_count_arr) {
        sal_free(entry_count_arr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_ecmp_rh_replace
 * Purpose:
 *      Replace ECMP resilient hashing group members without flowset table shuffle.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      old_intf_count - (IN) Number of elements in old_intf_array.
 *      old_intf_array - (IN) Array of Egress forwarding objects before replacing.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_ecmp_rh_replace(int unit,
        bcm_l3_egress_ecmp_t *ecmp,
        int intf_count,
        bcm_if_t *intf_array,
        int old_intf_count,
        bcm_if_t *old_intf_array)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int offset;
    ecmp_count_entry_t ecmp_count_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int alloc_size;
    uint32 *flowset_buf = NULL;
    int index_min, index_max;
    int max_shared;
    int i, j;
    int shared_intf_count;
    int ex_old_intf_count;
    int ex_new_intf_count;
    bcm_if_t *shared_intf_array = NULL;
    bcm_if_t *ex_old_intf_array = NULL;
    bcm_if_t *ex_new_intf_array = NULL;
    bcm_if_t *temp_old_intf_array = NULL;
    bcm_if_t *temp_new_intf_array = NULL;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (intf_count == 0) {
        /* Replacing the last member is the same as freeing all resources */
        BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_free_resource(unit, ecmp_group));

        /* Store the dynamic_size in hardware */
        BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_dynamic_size_set(unit, ecmp_group,
                    ecmp->dynamic_size));

        return BCM_E_NONE;
    }

    if (old_intf_count == 0) {
        /* Replacing the first member is the same as setting one member */
        return _bcm_td2_ecmp_rh_set(unit, ecmp, intf_count, intf_array);
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > intf_count) ? intf_count :
        old_intf_count;

    alloc_size = max_shared * sizeof(bcm_if_t);
    shared_intf_array = sal_alloc(alloc_size, "shared ecmp member array");
    if (NULL == shared_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(shared_intf_array, 0, alloc_size);

    alloc_size = old_intf_count * sizeof(bcm_if_t);
    ex_old_intf_array = sal_alloc(alloc_size, 
                                "array of members exclusive to old ecmp group");
    if (NULL == ex_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_old_intf_array, 0, alloc_size);

    alloc_size = intf_count * sizeof(bcm_if_t);
    ex_new_intf_array = sal_alloc(alloc_size,
                                "array of members exclusive to new ecmp group");
    if (NULL == ex_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_new_intf_array, 0, alloc_size);

    alloc_size = old_intf_count * sizeof(bcm_if_t);
    temp_old_intf_array = sal_alloc(alloc_size, "copy of old_intf_array");
    if (NULL == temp_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_old_intf_array, old_intf_array, alloc_size);
    for (i = 0; i < old_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, temp_old_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, temp_old_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        temp_old_intf_array[i] -= offset;
    }

    alloc_size = intf_count * sizeof(bcm_if_t);
    temp_new_intf_array = sal_alloc(alloc_size, "copy of intf_array");
    if (NULL == temp_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_new_intf_array, intf_array, alloc_size);
    for (i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, temp_new_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, temp_new_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        temp_new_intf_array[i] -= offset;
    }

    /* Replace arrangement */
    shared_intf_count = 0;
    ex_new_intf_count = 0;
    for (i = 0; i < intf_count; i++) {
        for (j = 0; j < old_intf_count; j++) {
            if (temp_new_intf_array[i] == temp_old_intf_array[j]) {
                shared_intf_array[shared_intf_count++] = temp_new_intf_array[i];
                /* Mark the matched element invalid */
                temp_old_intf_array[j] = BCM_XGS3_L3_INVALID_INDEX;
                break;
            }
        }
        if (j == old_intf_count) {
            ex_new_intf_array[ex_new_intf_count++] = temp_new_intf_array[i];
        }
    }

    ex_old_intf_count = 0;
    for (i = 0; i < old_intf_count; i++) {
        if (temp_old_intf_array[i] != BCM_XGS3_L3_INVALID_INDEX) {
            ex_old_intf_array[ex_old_intf_count++] = temp_old_intf_array[i];
        }
    }

    /* Read all the flow set entries of this ECMP resilient hash group */
    rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group,
                &ecmp_count_entry);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf);
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf);

    rv = _bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    alloc_size = num_entries * sizeof(rh_ecmp_flowset_entry_t);
    flowset_buf = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == flowset_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(flowset_buf, 0, alloc_size);
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_read_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ANY,
            index_min, index_max, flowset_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if (shared_intf_count == 0) {
        /* The old and the new ECMP groups don't share any members.
         * Replacing flow set entries with max_shared members so as to 
         * set shared_intf_count = max_shared.
         */
        rv = _bcm_td2_ecmp_rh_arrange_with_no_shared_entries(unit,
                num_entries, flowset_buf,
                old_intf_count, temp_old_intf_array,
                intf_count, temp_new_intf_array,
                &shared_intf_count, shared_intf_array,
                &ex_old_intf_count, ex_old_intf_array,
                &ex_new_intf_count, ex_new_intf_array);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /* Modify the flow set entry buffer to achieve balance among
     * shared members and members exclusive to the new ECMP group,
     * while keeping modifications of entries containing the
     * shared members to a minimum.
     */
    rv = _bcm_td2_ecmp_rh_balance_with_min_shared_mod(unit,
            num_entries, flowset_buf,
            shared_intf_count, shared_intf_array,
            ex_old_intf_count, ex_old_intf_array,
            ex_new_intf_count, ex_new_intf_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, flowset_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (flowset_buf) {
        soc_cm_sfree(unit, flowset_buf);
    }
    if (shared_intf_array) {
        sal_free(shared_intf_array);
    }
    if (ex_old_intf_array) {
        sal_free(ex_old_intf_array);
    }
    if (ex_new_intf_array) {
        sal_free(ex_new_intf_array);
    }
    if (temp_old_intf_array) {
        sal_free(temp_old_intf_array);
    }
    if (temp_new_intf_array) {
        sal_free(temp_new_intf_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_create 
 * Purpose:
 *      Create or modify an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      op         - (IN) Member operation: SET, ADD, DELETE, or REPLACE.
 *      count      - (IN) Number of elements in intf.
 *      intf       - (IN) Egress forwarding objects to add, delete, or replace.

 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_l3_egress_ecmp_rh_create(int unit, bcm_l3_egress_ecmp_t *ecmp,
        int intf_count, bcm_if_t *intf_array, int op, int count, bcm_if_t *intf)
{
    int dynamic_size_encode;
    int rh_enable;
    int ecmp_group;
    initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;


    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* Verify ecmp->dynamic_size */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_ecmp_rh_dynamic_size_encode(unit, ecmp->dynamic_size,
                                                  &dynamic_size_encode));
        rh_enable = 1;
    } else {
        rh_enable = 0;
    }

    if (op == BCM_L3_ECMP_MEMBER_OP_SET) {
        /* Free resilient hashing resources associated with this ecmp group */
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        BCM_IF_ERROR_RETURN
            (_bcm_td2_ecmp_rh_free_resource(unit, ecmp_group));

        if (rh_enable) {
            /* Set resilient hashing members for this ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_td2_ecmp_rh_set(unit, ecmp, intf_count, intf_array));

            if (SOC_IS_TRIDENT3X(unit)) {
                ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ANY,
                                    ecmp_group,&initial_l3_ecmp_group_entry));

                soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                    &initial_l3_ecmp_group_entry, LB_MODEf, 1);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ALL,
                                                ecmp_group, &initial_l3_ecmp_group_entry));
            }
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_ADD) {
        if (rh_enable) {
            /* Add new resilient hashing member to ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_td2_ecmp_rh_add(unit, ecmp, intf_count, intf_array, *intf));
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_DELETE) {
        if (rh_enable) {
            /* Delete resilient hashing member from ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_td2_ecmp_rh_delete(unit, ecmp, intf_count, intf_array, *intf));
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_REPLACE) {
        if (rh_enable) {
            /* Replace resilient hashing member for ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_td2_ecmp_rh_replace(unit, ecmp, intf_count, intf_array, count, intf));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_destroy
 * Purpose:
 *      Destroy an ECMP resilient hashing group.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      mpintf  - (IN) L3 interface id pointing to Egress multipath object.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf) 
{
    int ecmp_group;

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, mpintf)) {
        ecmp_group = mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_free_resource(unit, ecmp_group));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_get
 * Purpose:
 *      Get info about an ECMP resilient hashing group.
 * Parameters:
 *      unit - (IN) bcm device.
 *      ecmp - (INOUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_l3_egress_ecmp_rh_get(int unit, bcm_l3_egress_ecmp_t *ecmp)
{
    int ecmp_group;
    ecmp_count_entry_t ecmp_count_entry;
    int flow_set_size;
    int num_entries;

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    /* An ECMP group can be in one of 3 resilient hashing states:
     * (1) RH not enabled: L3_ECMP_COUNT.RH_FLOW_SET_SIZE = 0.
     * (2) RH enabled but without any members:
     *     L3_ECMP_COUNT.RH_FLOW_SET_SIZE is non-zero,
     *     L3_ECMP_COUNT.ENHANCED_HASHING_ENABLE = 0.
     * (3) RH enabled with members:
     *     L3_ECMP_COUNT.RH_FLOW_SET_SIZE is non-zero,
     *     L3_ECMP_COUNT.ENHANCED_HASHING_ENABLE = 1.
     */

    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                ecmp_group, &ecmp_count_entry));
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit,
            &ecmp_count_entry, RH_FLOW_SET_SIZEf);

    if (0 == flow_set_size) {
        /* Resilient hashing is not enabled on this ECMP group. */
        return BCM_E_NONE;
    }

    ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    BCM_IF_ERROR_RETURN(_bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size,
                        &num_entries));
    ecmp->dynamic_size = num_entries;
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      ECMP resilient hashing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_l3_egress_ecmp_rh_ethertype_set(
    int unit, 
    uint32 flags, 
    int ethertype_count, 
    int *ethertype_array)
{
    uint32 control_reg;
    int i, j;
    rh_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;

    /* Input check */
    if (ethertype_count > soc_mem_index_count(unit,
                RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)) {
        return BCM_E_RESOURCE;
    }

    /* Update ethertype eligibility control register */
    SOC_IF_ERROR_RETURN
        (READ_RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit, &control_reg));
    soc_reg_field_set(unit, RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
            &control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf,
            flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
    soc_reg_field_set(unit, RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
            &control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf,
            flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
    SOC_IF_ERROR_RETURN
        (WRITE_RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit, control_reg));

    /* Update Ethertype eligibility map table */
    for (i = 0; i < ethertype_count; i++) {
        sal_memset(&ethertype_entry, 0,
                sizeof(rh_ecmp_ethertype_eligibility_map_entry_t));
        soc_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, VALIDf, 1);
        soc_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, ETHERTYPEf, ethertype_array[i]);
        SOC_IF_ERROR_RETURN(WRITE_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, i, &ethertype_entry));
    }

    /* Zero out remaining entries of Ethertype eligibility map table */
    for (j = i; j < soc_mem_index_count(unit,
                RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); j++) {
        SOC_IF_ERROR_RETURN(WRITE_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, j, soc_mem_entry_null(unit,
                        RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      ECMP resilient hashing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (INOUT) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_l3_egress_ecmp_rh_ethertype_get(
    int unit, 
    uint32 *flags, 
    int ethertype_max, 
    int *ethertype_array, 
    int *ethertype_count)
{
    uint32 control_reg;
    int i;
    int ethertype;
    rh_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;

    *ethertype_count = 0;

    /* Get flags */
    SOC_IF_ERROR_RETURN
        (READ_RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit, &control_reg));
    if (soc_reg_field_get(unit, RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf)) {
        *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE;
    }
    if (soc_reg_field_get(unit, RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
        *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER;
    }

    /* Get Ethertypes */
    for (i = 0; i < soc_mem_index_count(unit,
                RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); i++) {
        SOC_IF_ERROR_RETURN(READ_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ANY, i, &ethertype_entry));
        if (soc_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, VALIDf)) {
            ethertype = soc_RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, ETHERTYPEf);
            if (NULL != ethertype_array) {
                ethertype_array[*ethertype_count] = ethertype;
            }
            (*ethertype_count)++;
            if ((ethertype_max > 0) && (*ethertype_count == ethertype_max)) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l3_egress_ecmp_rh_shared_copy
 * Purpose:
 *      For members shared by the old and the new ECMP groups,
 *      copy the resilient hash flow set entries containing these
 *      members from the old to the new ECMP group, in order to minimize
 *      flow-to-member reassignments when the old ECMP group in a route
 *      entry is replaced by the new one.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      old_ecmp_group - (IN) Old ECMP group.
 *      new_ecmp_group - (IN) New ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
        int new_ecmp_group)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t old_ecmp, new_ecmp;
    int old_intf_count, new_intf_count;
    bcm_if_t *old_intf_array = NULL;
    bcm_if_t *new_intf_array = NULL;
    int offset;
    int count;
    int max_shared;
    bcm_if_t *shared_intf_array = NULL;
    bcm_if_t *ex_old_intf_array = NULL;
    bcm_if_t *ex_new_intf_array = NULL;
    bcm_if_t *temp_old_intf_array = NULL;
    int shared_intf_count, ex_new_intf_count, ex_old_intf_count;
    int i, j;
    ecmp_count_entry_t ecmp_count_entry;
    int entry_base_ptr, flow_set_size, num_entries;
    int alloc_size;
    uint32 *buf_ptr = NULL;
    int index_min, index_max;

    /* Check if there are any routing entries pointing to the
     * new ECMP group. If so, the new ECMP group's flow set
     * entries should not be modified.
     */
    if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp),
                new_ecmp_group) > 2) {
        /* When the new ECMP group was created, its reference count
         * was set to 1. Then, in bcm_xgs3_defip_add, its reference count
         * was incremented before this procedure is invoked. Hence,
         * if there are any routing entry already pointing at the new
         * ECMP group, the reference count would be greater than 2.
         */
        return BCM_E_NONE;
    }

    /* Check if the old ECMP group is resilient hash enabled. */
    bcm_l3_egress_ecmp_t_init(&old_ecmp);
    old_ecmp.ecmp_intf = old_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_ecmp_get(unit, &old_ecmp,
                0, NULL, &old_intf_count));
    if (old_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* The old ECMP group was not resilient hash enabled. Copying
         * flow set entries is not applicable.
         */
        return BCM_E_NONE;
    }

    /* Check if the new ECMP group is resilient hash enabled. */
    bcm_l3_egress_ecmp_t_init(&new_ecmp);
    new_ecmp.ecmp_intf = new_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_ecmp_get(unit, &new_ecmp,
                0, NULL, &new_intf_count));
    if (new_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* The new ECMP group is not resilient hash enabled. Copying
         * flow set entries is not applicable.
         */
        return BCM_E_NONE;
    }

    if (old_ecmp.dynamic_size != new_ecmp.dynamic_size) {
        /* The number of flow set entries in old ECMP group is not the
         * same as the number in new ECMP group. Copying
         * flow set entries is not applicable.
         */ 
        return BCM_E_NONE;
    }

    /* Get old ECMP group members */
    old_intf_array = sal_alloc(old_intf_count * sizeof(bcm_if_t),
            "old ecmp member array");
    if (NULL == old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(old_intf_array, 0, old_intf_count * sizeof(bcm_if_t));
    rv = bcm_esw_l3_egress_ecmp_get(unit, &old_ecmp, old_intf_count,
            old_intf_array, &count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    for (i = 0; i < old_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, old_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, old_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        old_intf_array[i] -= offset;
    }

    /* Get new ECMP group members */
    new_intf_array = sal_alloc(new_intf_count * sizeof(bcm_if_t),
            "new ecmp member array");
    if (NULL == new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(new_intf_array, 0, new_intf_count * sizeof(bcm_if_t));
    rv = bcm_esw_l3_egress_ecmp_get(unit, &new_ecmp, new_intf_count,
            new_intf_array, &count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    for (i = 0; i < new_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, new_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, new_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        new_intf_array[i] -= offset;
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > new_intf_count) ? new_intf_count :
        old_intf_count;
    shared_intf_array = sal_alloc(max_shared * sizeof(bcm_if_t),
            "shared ecmp member array");
    if (NULL == shared_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(shared_intf_array, 0, max_shared * sizeof(bcm_if_t));

    ex_old_intf_array = sal_alloc(old_intf_count * sizeof(bcm_if_t),
            "array of members exclusive to old ecmp group");
    if (NULL == ex_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_old_intf_array, 0, old_intf_count * sizeof(bcm_if_t));

    ex_new_intf_array = sal_alloc(new_intf_count * sizeof(bcm_if_t),
            "array of members exclusive to new ecmp group");
    if (NULL == ex_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_new_intf_array, 0, new_intf_count * sizeof(bcm_if_t));

    temp_old_intf_array = sal_alloc(old_intf_count * sizeof(bcm_if_t),
            "copy of old_intf_array");
    if (NULL == temp_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_old_intf_array, old_intf_array, old_intf_count * sizeof(bcm_if_t));

    shared_intf_count = 0;
    ex_new_intf_count = 0;
    for (i = 0; i < new_intf_count; i++) {
        for (j = 0; j < old_intf_count; j++) {
            if (new_intf_array[i] == temp_old_intf_array[j]) {
                shared_intf_array[shared_intf_count++] = new_intf_array[i];
                /* Mark the matched element invalid */
                temp_old_intf_array[j] = BCM_XGS3_L3_INVALID_INDEX;
                break;
            }
        }
        if (j == old_intf_count) {
            ex_new_intf_array[ex_new_intf_count++] = new_intf_array[i];
        }
    }

    ex_old_intf_count = 0;
    for (i = 0; i < old_intf_count; i++) {
        if (temp_old_intf_array[i] != BCM_XGS3_L3_INVALID_INDEX) {
            ex_old_intf_array[ex_old_intf_count++] = temp_old_intf_array[i];
        }
    }

    if (shared_intf_count == 0) {
        /* The old and the new ECMP groups don't share any members.
         * Copying flow set entries is not applicable.
         */
        rv = BCM_E_NONE;
        goto cleanup;
    }

    /* Read old ECMP group's flow set entries into a buffer */
    rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, old_ecmp_group,
            &ecmp_count_entry);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf);
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf);
    rv = _bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    alloc_size = num_entries * sizeof(rh_ecmp_flowset_entry_t);
    buf_ptr = soc_cm_salloc(unit, alloc_size, "RH_ECMP_FLOWSET entries");
    if (NULL == buf_ptr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(buf_ptr, 0, alloc_size);
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_read_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ANY,
            index_min, index_max, buf_ptr);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Modify the flow set entry buffer to achieve balance among
     * shared members and members exclusive to the new ECMP group,
     * while keeping modifications of entries containing the
     * shared members to a minimum.
     */
    rv = _bcm_td2_ecmp_rh_balance_with_min_shared_mod(unit,
            num_entries, buf_ptr,
            shared_intf_count, shared_intf_array,
            ex_old_intf_count, ex_old_intf_array,
            ex_new_intf_count, ex_new_intf_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Replace the new ECMP group's flow set entries with the buffer */
    rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, new_ecmp_group,
            &ecmp_count_entry);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_BASEf);
    flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_count_entry,
            RH_FLOW_SET_SIZEf);
    rv = _bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size, &num_entries);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    index_min = entry_base_ptr;
    index_max = index_min + num_entries - 1;
    rv = soc_mem_write_range(unit, RH_ECMP_FLOWSETm, MEM_BLOCK_ALL,
            index_min, index_max, buf_ptr);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (old_intf_array) {
        sal_free(old_intf_array);
    }
    if (new_intf_array) {
        sal_free(new_intf_array);
    }
    if (shared_intf_array) {
        sal_free(shared_intf_array);
    }
    if (ex_old_intf_array) {
        sal_free(ex_old_intf_array);
    }
    if (ex_new_intf_array) {
        sal_free(ex_new_intf_array);
    }
    if (temp_old_intf_array) {
        sal_free(temp_old_intf_array);
    }
    if (buf_ptr) {
        soc_cm_sfree(unit, buf_ptr);
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_td2_ecmp_rh_hw_recover
 * Purpose:
 *      Recover ECMP resilient hashing internal state from hardware.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ecmp_rh_hw_recover(int unit) 
{
    int rv = BCM_E_NONE;
    int alloc_size;
    uint32 *buf_ptr = NULL;
    int i;
    ecmp_count_entry_t *ecmp_count_entry;
    int flow_set_size;
    int entry_base_ptr;
    int block_base_ptr;
    int num_entries;
    int num_blocks, enhanced_hashing = 1;
    soc_field_t enhanced_hashing_fld;
    uint32 rval;

    if (soc_reg_field_valid(unit, ENHANCED_HASHING_CONTROLr, RH_DLB_SELECTIONf)) {
        SOC_IF_ERROR_RETURN(READ_ENHANCED_HASHING_CONTROLr(unit, &rval));
        if (0 == soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,
                RH_DLB_SELECTIONf)) {
           /* ECMP resilient hashing is not enabled chip-wide. */
            return BCM_E_NONE;
        }
    }

    /* Read entire L3_ECMP_COUNT table */
    alloc_size = soc_mem_index_count(unit, L3_ECMP_COUNTm) *
        sizeof(ecmp_count_entry_t);
    buf_ptr = soc_cm_salloc(unit, alloc_size, "L3_ECMP_COUNT entries");
    if (NULL == buf_ptr) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY,
            soc_mem_index_min(unit, L3_ECMP_COUNTm),
            soc_mem_index_max(unit, L3_ECMP_COUNTm),
            buf_ptr);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, buf_ptr);
        return rv;
    }

    /* Traverse ECMP groups to recover resilient hashing flow set
     * table usage.
     */
    for (i = 0; i < soc_mem_index_count(unit, L3_ECMP_COUNTm); i++) {

        /* An ECMP group can be in one of 3 resilient hashing states:
         * (1) RH not enabled: L3_ECMP_COUNT.RH_FLOW_SET_SIZE = 0.
         * (2) RH enabled but without any members:
         *     L3_ECMP_COUNT.RH_FLOW_SET_SIZE is non-zero,
         *     L3_ECMP_COUNT.ENHANCED_HASHING_ENABLE = 0.
         * (3) RH enabled with members:
         *     L3_ECMP_COUNT.RH_FLOW_SET_SIZE is non-zero,
         *     L3_ECMP_COUNT.ENHANCED_HASHING_ENABLE = 1.
         */

        ecmp_count_entry = soc_mem_table_idx_to_pointer(unit, L3_ECMP_COUNTm,
                ecmp_count_entry_t *, buf_ptr, i);
        flow_set_size = soc_L3_ECMP_COUNTm_field32_get(unit,
                ecmp_count_entry, RH_FLOW_SET_SIZEf);
        if (0 == flow_set_size) {
            /* Resilient hashing is not enabled on this ECMP group. */
            continue;
        }
        if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
            enhanced_hashing_fld = LB_MODEf;
            enhanced_hashing = 4;
        } else {
            enhanced_hashing_fld = ENHANCED_HASHING_ENABLEf;
        }
        if (soc_L3_ECMP_COUNTm_field32_get(unit,
                   ecmp_count_entry, enhanced_hashing_fld) == enhanced_hashing) {
            /* Recover ECMP resilient hashing flow set block usage */
            entry_base_ptr = soc_L3_ECMP_COUNTm_field32_get(unit,
                    ecmp_count_entry, RH_FLOW_SET_BASEf);
            block_base_ptr = entry_base_ptr >> \
                             _td2_ecmp_rh_info[unit]->ecmp_block_size;
            rv = _bcm_td2_ecmp_rh_dynamic_size_decode(unit, flow_set_size,
                    &num_entries);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf_ptr);
                return rv;
            }
            num_blocks = num_entries >> \
                         _td2_ecmp_rh_info[unit]->ecmp_block_size ;
            _BCM_ECMP_RH_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
                    num_blocks);
#ifdef BCM_TRIDENT3_SUPPORT
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                ecmp_grp_enhanced_hashing[unit][i] = 1;
            }
#endif
        }
    }

    soc_cm_sfree(unit, buf_ptr);

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_td2_ecmp_rh_sw_dump
 * Purpose:
 *     Displays ECMP resilient hashing state maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void bcm_td2_ecmp_rh_sw_dump(int unit)
{
    int i, j;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    _bcm_l3_bookkeeping_t *l3_bk_info = &_bcm_l3_bk_info[unit];
#endif 

    LOG_CLI((BSL_META_U(unit,
                        "  ECMP Resilient Hashing Info -\n")));
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    if ((
#if defined(BCM_RIOT_SUPPORT)
         l3_bk_info->riot_enable ||
#endif
         BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) &&
        (l3_bk_info->l3_ecmp_rh_overlay_entries > 0)) {

        LOG_CLI((BSL_META_U(unit,
                        "    Overlay RH Flowset Table Blocks Total: %d\n"),
              l3_bk_info->l3_ecmp_rh_overlay_entries >> \
              _td2_ecmp_rh_info[unit]->ecmp_block_size));

        LOG_CLI((BSL_META_U(unit,
                        "    Underlay RH Flowset Table Blocks Total: %d\n"),
             _td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks -
             (l3_bk_info->l3_ecmp_rh_overlay_entries >> \
              _td2_ecmp_rh_info[unit]->ecmp_block_size)));
    } else
#endif
    {

        LOG_CLI((BSL_META_U(unit,
                        "    RH Flowset Table Blocks Total: %d\n"),
             _td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks));
    }

    /* Print ECMP RH flowset table usage */
    LOG_CLI((BSL_META_U(unit,
                        "    RH Flowset Table Blocks Used:")));
    j = 0;
    for (i = 0; i < _td2_ecmp_rh_info[unit]->num_ecmp_rh_flowset_blocks; i++) {
        if (_BCM_ECMP_RH_FLOWSET_BLOCK_USED_GET(unit, i)) {
            j++;
            if (j % 15 == 1) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n     ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %4d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 && BCM_TRIDENT2_SUPPORT  */
