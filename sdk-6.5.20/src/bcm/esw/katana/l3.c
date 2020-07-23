/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Katana L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/l3.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>


_bcm_l3_defip_pair128_table_t *l3_kt_defip_pair128[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _kt_defip_pair128_hash
 * Purpose:
 *      Calculate an entry 8 bit hash.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      entry_hash  - (OUT)Entry hash
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_kt_defip_pair128_hash(int unit, _bcm_defip_cfg_t *lpm_cfg, uint8 *hash) 
{
    uint32 key[BCM_KT_DEFIP_PAIR128_HASH_SZ];

    if (hash == NULL) {
        return (BCM_E_PARAM);
    }

    SAL_IP6_ADDR_TO_UINT32(lpm_cfg->defip_ip6_addr, key);
    key[4] = lpm_cfg->defip_sub_len;
    key[5] = lpm_cfg->defip_vrf;

    *hash = (_shr_crc16b(0, (void *)key,
                         WORDS2BITS((BCM_KT_DEFIP_PAIR128_HASH_SZ))) & 0xff);
    return (BCM_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _kt_defip_pair128_reinit
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
_kt_defip_pair128_reinit(int unit, int idx, _bcm_defip_cfg_t *lpm_cfg)
{
    int   lkup_plen;         /* Vrf weighted lookup prefix. */
    uint8 hash;              /* Entry hash value.           */

    /* Calculate vrf weighted prefix length. */
    lkup_plen = lpm_cfg->defip_sub_len * 
        ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);

    /* Calculate entry lookup hash. */
    BCM_IF_ERROR_RETURN(_kt_defip_pair128_hash(unit, lpm_cfg, &hash));

    /* Set software structure info. */
    BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, idx, lkup_plen, hash);

    /* Update software usage counter. */
    BCM_KT_DEFIP_PAIR128_USED_COUNT(unit)++;

    return (BCM_E_NONE);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_kt_defip_pair128_init
 * Purpose:
 *      Initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt_defip_pair128_init(int unit)
{   
    int mem_sz;    /* Memory allocation size     */
    int rv;        /* Internal operation status. */
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);

    if (lpm_ipv6_info->ipv6_128b.depth == 0) {
       /* We cannot use L3_DEFIP for storing IPv6 LPM 128b entries */
       return BCM_E_NONE;
    }

    /* De-allocate any existing structures. */
    if (BCM_KT_DEFIP_PAIR128(unit) != NULL) {
        rv = _bcm_kt_defip_pair128_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Allocate cam control structure. */
    mem_sz = sizeof(_bcm_l3_defip_pair128_table_t);
    BCM_KT_DEFIP_PAIR128(unit) = sal_alloc(mem_sz, "l3_kt_defip_pair128");
    if (BCM_KT_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Reset cam control structure. */
    sal_memset(BCM_KT_DEFIP_PAIR128(unit), 0, mem_sz);

    BCM_KT_DEFIP_PAIR128_TOTAL(unit) =
        soc_mem_index_count(unit, L3_DEFIP_PAIR_128m);
    BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit) =
        lpm_ipv6_info->ipv6_128b.sip_start_offset;
    BCM_KT_DEFIP_PAIR128_IDX_MAX(unit) = lpm_ipv6_info->ipv6_128b.depth - 1;

    mem_sz = lpm_ipv6_info->ipv6_128b.depth *
             sizeof(_bcm_l3_defip_pair128_entry_t);
    BCM_KT_DEFIP_PAIR128_ARR(unit) = 
                            sal_alloc(mem_sz, "l3_defip_pair128_entry_array");
    if (BCM_KT_DEFIP_PAIR128_ARR(unit) == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_kt_defip_pair128_deinit(unit));
        return (BCM_E_MEMORY);
    }
    sal_memset(BCM_KT_DEFIP_PAIR128_ARR(unit), 0, mem_sz);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt_defip_pair128_deinit
 * Purpose:
 *      De-initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt_defip_pair128_deinit(int unit)
{
    uint32 ipv6_lpm_128b_enable;
    
    ipv6_lpm_128b_enable = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 0);
    if (!ipv6_lpm_128b_enable) {
       /* We cannot use L3_DEFIP for storing IPv6 LPM 128b entries */
       return BCM_E_NONE;
    }

    if (BCM_KT_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_NONE);
    }

    /* De-allocate any existing structures. */
    if (BCM_KT_DEFIP_PAIR128_ARR(unit) != NULL) {
        sal_free(BCM_KT_DEFIP_PAIR128_ARR(unit));
        BCM_KT_DEFIP_PAIR128_ARR(unit) = NULL;
    }
    sal_free(BCM_KT_DEFIP_PAIR128(unit));
    BCM_KT_DEFIP_PAIR128(unit) = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:    
 *     _kt_defip_pair128_ip6_mask_set
 * Purpose:  
 *     Set IP6 mask field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
 
void

_kt_defip_pair128_ip6_mask_set(int unit, soc_mem_t mem, uint32 *entry, 
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

/*
 * Function:    
 *     _kt_defip_pair128_ip6_mask_get
 * Purpose:  
 *     Read IP6 mask field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
void

_kt_defip_pair128_ip6_mask_get(int unit, soc_mem_t mem, const void *entry, 
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
 *     _kt_defip_pair128_ip6_addr_set
 * Purpose:  
 *     Set IP6 address field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */

void
_kt_defip_pair128_ip6_addr_set(int unit, soc_mem_t mem, uint32 *entry, 
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
 *     _kt_defip_pair128_ip6_addr_get
 * Purpose:  
 *     Read IP6 address field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
void

_kt_defip_pair128_ip6_addr_get(int unit, soc_mem_t mem, const void *entry, 
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
 *      _kt_defip_pair128_get_key
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
_kt_defip_pair128_get_key(int unit, uint32 *hw_entry, 
                       _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;  /* Route table memory. */ 
    bcm_ip6_t mask;                      /* Subnet mask.        */

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Extract ip6 address. */
    _kt_defip_pair128_ip6_addr_get(unit,mem, hw_entry, lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _kt_defip_pair128_ip6_mask_get(unit, mem, hw_entry, mask);
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if(!soc_mem_field32_get(unit, mem, hw_entry, VRF_ID_MASK0_LWRf)) {
        lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        lpm_cfg->defip_vrf = soc_mem_field32_get(unit, mem, hw_entry, VRF_ID_0_LWRf);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _kt_defip_pair128_parse
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
STATIC INLINE int 
_kt_defip_pair128_parse(int unit, uint32 *hw_entry,
                     _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;  /* Route table memory. */ 

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (soc_mem_field32_get(unit, mem, hw_entry, ECMPf)) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = TRUE;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx = soc_mem_field32_get(unit, mem, hw_entry, ECMP_PTRf);
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx =
                soc_mem_field32_get(unit, mem, hw_entry, NEXT_HOP_INDEXf);
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
    if ((soc_mem_field32_get(unit, mem, hw_entry, HIT0f) &&
         soc_mem_field32_get(unit, mem, hw_entry, HIT1f))) {
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
 *      _kt_trx_defip_128_get_key
 * Purpose:
 *      Parse route entry key from L3_DEFIP_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      entry       - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_kt_trx_defip_128_get_key(int unit, uint32 *hw_entry, 
                       _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIP_128m;  /* Route table memory. */ 
    bcm_ip6_t mask;                 /* Subnet mask.        */

    /* Input parameters check. */
    if ((NULL == lpm_cfg) || (NULL == hw_entry)) {
        return (BCM_E_PARAM);
    }

    /* Extract ip6 address. */
    soc_mem_ip6_addr_get(unit, mem, hw_entry, IP_ADDRf, lpm_cfg->defip_ip6_addr,
                         SOC_MEM_IP6_FULL_ADDR);

    /* Extract subnet mask. */
    soc_mem_ip6_addr_get(unit, mem, hw_entry, IP_ADDR_MASKf, mask,
                         SOC_MEM_IP6_FULL_ADDR);
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if(!soc_mem_field32_get(unit, mem, hw_entry, VRF_ID_MASKf)) {
        lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        lpm_cfg->defip_vrf = soc_mem_field32_get(unit, mem, hw_entry, VRF_IDf);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _kt_trx_defip_128_parse
 * Purpose:
 *      Parse route entry from L3_DEFIP_128 table to sw structure.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      hw_entry    - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 *      nh_ecmp_idx - (OUT)Next hop ecmp table index. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC INLINE int 
_kt_trx_defip_128_parse(int unit, uint32 *hw_entry,
                     _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIP_128m;  /* Route table memory. */ 


    /* Input parameters check. */
    if ((NULL == hw_entry) || (NULL == lpm_cfg)) {
        return (BCM_E_PARAM);
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (soc_mem_field32_get(unit, mem, hw_entry, ECMPf)) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = TRUE;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (NULL != nh_ecmp_idx) {
            *nh_ecmp_idx = soc_mem_field32_get(unit, mem, hw_entry, ECMP_PTRf);
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (NULL != nh_ecmp_idx) {
            *nh_ecmp_idx =
                soc_mem_field32_get(unit, mem, hw_entry, NEXT_HOP_INDEXf);
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
    if (soc_mem_field32_get(unit, mem, hw_entry, HITf)) {
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
 *      _kt_defip_pair128_match
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
_kt_defip_pair128_match(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                     uint32 *hw_entry, int *hw_index)
{
    _bcm_defip_cfg_t candidate;        /* Match condidate.            */
    int   lkup_plen;                   /* Vrf weighted lookup prefix. */
    int   index;                       /* Table iteration index.      */
    uint8 hash;                        /* Entry hash value.           */
    int   rv = BCM_E_NONE;             /* Internal operation status.  */


    /* Initialization */
    sal_memset(&candidate, 0, sizeof(_bcm_defip_cfg_t));
    
    /* Calculate vrf weighted prefix length. */
    lkup_plen = lpm_cfg->defip_sub_len * 
        ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);

    /* Calculate entry lookup hash. */
    BCM_IF_ERROR_RETURN(_kt_defip_pair128_hash(unit, lpm_cfg, &hash));

    /*    coverity[assignment : FALSE]    */
    for (index = 0; index <= BCM_KT_DEFIP_PAIR128_IDX_MAX(unit); index++) {

        /* Route prefix length comparison. */
        if (lkup_plen != BCM_KT_DEFIP_PAIR128_ARR(unit)[index].prefix_len) {
            continue;
        }

        /* Route lookup key hash comparison. */
        if (hash != BCM_KT_DEFIP_PAIR128_ARR(unit)[index].entry_hash) {
            continue;
        }

        /* Hash & prefix length match -> read and compare hw entry itself. */
        rv = BCM_XGS3_MEM_READ(unit, L3_DEFIP_PAIR_128m, index, hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Make sure entry is valid.  */
        if ((!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_LWRf)) ||
            (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_LWRf)) ||
            (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID0_UPRf)) ||
            (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, hw_entry, VALID1_UPRf))
            ) {
            /* Too bad sw image doesn't match hardware. */
            continue;
        }

        /* Extract entry address vrf */
        rv =  _kt_defip_pair128_get_key(unit, hw_entry, &candidate);
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

    if (index > BCM_KT_DEFIP_PAIR128_IDX_MAX(unit))  {
        return (BCM_E_NOT_FOUND);
    }

    *hw_index = index;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _kt_defip_pair128_entry_clear
 * Purpose:
 *      Clear/Reset a single route entry. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      hw_index  - (IN)Entry index in the tcam. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_kt_defip_pair128_entry_clear(int unit, int hw_index)  
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
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
    COMPILER_64_ZERO(val64);
    for (i = 0; i < field_count; i++) {
        soc_mem_field64_set(unit, mem, hw_entry, key_field[i], val64);
        soc_mem_field64_set(unit, mem, hw_entry, mask_field[i], val64);
    }

    /* Reset hw entry. */
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, hw_index,
                            &soc_mem_entry_null(unit, L3_DEFIP_PAIR_128m)));
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, hw_index, hw_entry));

    /* Reset sw entry. */
    BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);

    /* Reset urpf source lookup entry. */
    if (SOC_URPF_STATUS_GET(unit)) {
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, 
                            (hw_index + BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit)),
                            &soc_mem_entry_null(unit, L3_DEFIP_PAIR_128m)));
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, 
                 (hw_index+ BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit)), hw_entry));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _kt_defip_pair128_shift
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
_kt_defip_pair128_shift(int unit, int idx_src, int idx_dest)  
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */

    /* Don't copy uninstalled entries. */
    if (0 != BCM_KT_DEFIP_PAIR128_ARR(unit)[idx_src].prefix_len) {
        /* Read entry from source index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_READ(unit, L3_DEFIP_PAIR_128m, idx_src, hw_entry));

        /* Write entry to the destination index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, idx_dest, hw_entry));

        /* Shift corresponding  URPF/source lookup entries. */
        if (SOC_URPF_STATUS_GET(unit)) {
            /* Read entry from source index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_READ(unit, L3_DEFIP_PAIR_128m, 
                                   idx_src + BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit),
                                   hw_entry));

            /* Write entry to the destination index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, 
                                    idx_dest +  BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit),
                                    hw_entry));
        }
    }
    /* Copy software portion of the entry. */ 
    sal_memcpy (&BCM_KT_DEFIP_PAIR128_ARR(unit)[idx_dest],
                &BCM_KT_DEFIP_PAIR128_ARR(unit)[idx_src], 
                sizeof(_bcm_l3_defip_pair128_entry_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _kt_defip_pair128_shift_range
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
_kt_defip_pair128_shift_range(int unit, int idx_start, int idx_end)  
{
    int idx;     /* Iteration index. */
    int rv = BCM_E_NONE;   /* Operaton status. */ 

    /* Shift entries down (1 index up) */
    if (idx_end > idx_start) {
        for (idx = idx_end - 1; idx >= idx_start; idx--) {
            rv = _kt_defip_pair128_shift(unit, idx, idx + 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _kt_defip_pair128_entry_clear(unit, idx_start);
    }

    /* Shift entries up (1 index down) */
    if (idx_end < idx_start) {
        for (idx = idx_end + 1; idx <= idx_start; idx++) {
            rv = _kt_defip_pair128_shift(unit, idx, idx - 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _kt_defip_pair128_entry_clear(unit, idx_start);
    }

    return (rv);
}
/*
 * Function:
 *      _kt_defip_pair128_idx_alloc
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
STATIC int 
_kt_defip_pair128_idx_alloc(int unit, _bcm_defip_cfg_t *lpm_cfg, int *hw_index,
                            int nh_ecmp_idx)
{
    int  lkup_plen;                   /* Vrf weighted lookup prefix.    */
    uint8 hash;                       /* Entry hash value.              */
    int  idx;                         /* Table iteration index.         */
    int  rv;                          /* Operation return status.       */
    int  shift_min = 0;               /* Minimum entry shift required.  */
    int  range_start = 0;             /* First valid location for pfx.  */
    int  range_end = BCM_KT_DEFIP_PAIR128_IDX_MAX(unit);
                                      /* Last valid location for pfx.   */
    int  free_idx_before = -1;        /* Free index before range_start. */
    int  free_idx_after  = -1;        /* Free index after range_end.    */
    uint32 temp_hw_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */
    _bcm_defip_cfg_t temp_lpm;
    int temp_nh_ecmp_idx;

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_index == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Calculate vrf weighted prefix length. */
    lkup_plen = lpm_cfg->defip_sub_len * 
        ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);

    /* Calculate entry hash. */
    BCM_IF_ERROR_RETURN(_kt_defip_pair128_hash(unit, lpm_cfg, &hash));

    
    /* Make sure at least one slot is available. */
    if (BCM_KT_DEFIP_PAIR128_USED_COUNT(unit) == ( range_end + 1)) {
        /* L3_DEFIP_PAIR_128 table is full */
        /* a. Copy entry at index 0 of L3_DEFIP_PAIR_128
           b. Add the copied entry to L3_DEFIP_128 table
           c. Delete entry at index 0 from the L3_DEFIP_PAIR_128 table
           d. Add the new entry to L3_DEFIP_PAIR_128 table
        */
        rv = BCM_XGS3_MEM_READ(unit, L3_DEFIP_PAIR_128m, 0, &temp_hw_entry);
        BCM_IF_ERROR_RETURN(rv);

        sal_memset(&temp_lpm, 0, sizeof(_bcm_defip_cfg_t));

        /* Parse entry. */
        rv = _kt_defip_pair128_parse(unit, temp_hw_entry, &temp_lpm, &temp_nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);
        
        /* Extract entry address vrf */
        rv =  _kt_defip_pair128_get_key(unit, temp_hw_entry, &temp_lpm);
        BCM_IF_ERROR_RETURN(rv);
        

        /* Add the copied entry to L3_DEFIP_128 table */
        temp_lpm.defip_index = BCM_XGS3_L3_INVALID_INDEX;
        rv = _bcm_trx_defip_128_add(unit, &temp_lpm, temp_nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);

        /* Delete entry from hardware & software. */
        rv = _kt_defip_pair128_entry_clear(unit, 0);
        if (BCM_SUCCESS(rv)) {
            BCM_XGS3_L3_DEFIP_CNT_DEC(unit, TRUE);
            BCM_KT_DEFIP_PAIR128_USED_COUNT(unit)--;
        }
    }

    /* 
     *  Find first valid index for the prefix;
     */ 
    for (idx = 0; idx <= BCM_KT_DEFIP_PAIR128_IDX_MAX(unit); idx++) {
        /* Remember free entry before installation location. */
        if (0 == BCM_KT_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            free_idx_before = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen < BCM_KT_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            range_start = idx;
            continue;
        }
        break;
    }

    /* 
     *  Find last valid index for the prefix;
     */ 
    for (idx = BCM_KT_DEFIP_PAIR128_IDX_MAX(unit); idx >= 0; idx--) {

        /* Remeber free entry after installation location. */
        if (0 == BCM_KT_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            free_idx_after = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen >  BCM_KT_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            range_end = idx;
            continue;
        }
        break;
    }

    /* Check if entry should be inserted at the end. */
    if (range_start == BCM_KT_DEFIP_PAIR128_IDX_MAX(unit)) {
        /* Shift all entries from free_idx_before till the end 1 up. */   
        rv = _kt_defip_pair128_shift_range(unit, BCM_KT_DEFIP_PAIR128_IDX_MAX(unit), free_idx_before);

        if (BCM_SUCCESS(rv)) { 
            /* Allocated index is last TCAM entry. */
            *hw_index = BCM_KT_DEFIP_PAIR128_IDX_MAX(unit);
            BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, *hw_index, lkup_plen, hash);
        }
        return (rv);
    }

    /* Check if entry should be inserted at the beginning. */
    if (range_end < 0) {
        /* Shift all entries from 0 till free_idx_after 1 down. */
        rv = _kt_defip_pair128_shift_range(unit, 0, free_idx_after);

        if (BCM_SUCCESS(rv)) {
            /* Allocated index is first TCAM entry. */
            *hw_index = 0;
            BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, 0, lkup_plen, hash);
        }
        return (rv);
    }

    /* Find if there is any free index in range. */
    for (idx = range_start; idx <= range_end; idx++) {
        if (0 == BCM_KT_DEFIP_PAIR128_ARR(unit)[idx].prefix_len) {
            *hw_index = idx;
            BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, idx, lkup_plen, hash);
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
        rv = _kt_defip_pair128_shift_range(unit, range_start, free_idx_before);
        if (BCM_SUCCESS(rv)) {
            *hw_index= range_start;
            BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, range_start, lkup_plen, hash);
        }
    } else {
        rv = _kt_defip_pair128_shift_range(unit, range_end, free_idx_after);
        if (BCM_SUCCESS(rv)) {
            *hw_index = range_end;
            BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, range_end, lkup_plen, hash);
        }
    }
    return (rv);
}
/*
 * Function:
 *      _bcm_kt_defip_pair128_get
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
_bcm_kt_defip_pair128_get(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                       int *nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int clear_hit;                           /* Clear entry hit bit flag. */
    int hw_index = 0;                        /* Entry offset in the TCAM. */ 
    int rv;                                  /* Internal operation status.*/
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    if (lpm_ipv6_info->ipv6_128b.depth == 0) {
       /* We cannot use L3_DEFIP for storing IPv6 128b lookup */
       return BCM_E_NOT_FOUND;
    }

    rv = _bcm_trx_defip_128_get(unit, lpm_cfg, nh_ecmp_idx);

    if (rv == BCM_E_NOT_FOUND) {

        /* Check if clear hit bit required. */
        clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

        /* Create mask from prefix length. */
        bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

        /* Apply mask on address. */
        bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

        /* Search for matching entry in the cam. */
        rv =  _kt_defip_pair128_match(unit, lpm_cfg, hw_entry, &hw_index);
        BCM_IF_ERROR_RETURN(rv);

        lpm_cfg->defip_index = hw_index;

        /* Parse matched entry. */
        rv = _kt_defip_pair128_parse(unit, hw_entry, lpm_cfg, nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);

        /* Clear the HIT bit */
        if (clear_hit) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT0f, 0);
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, hw_entry, HIT1f, 0);

            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, hw_index, hw_entry);
        }
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_kt_defip_pair128_add
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
_bcm_kt_defip_pair128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                       int nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];   /* New entry hw buffer.      */
    bcm_ip6_t mask;                             /* Subnet mask.              */
    int hw_index = 0;                           /* Entry offset in the TCAM. */ 
    int rv;                                     /* Operation return status.  */
    soc_mem_t mem = L3_DEFIP_PAIR_128m;              /* Route table memory.       */
    int  lkup_plen;                   /* Vrf weighted lookup prefix.    */
    int  range_end;                   /* Last valid location for pfx.   */
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    /* No support for routes matchin AFTER vrf specific. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        return (BCM_E_UNAVAIL);
    }

    if (lpm_ipv6_info->ipv6_128b.depth == 0) {
       /* We cannot use L3_DEFIP for storing IPv6 128b lookup */
       return BCM_E_FULL;
    }   

    /* Zero buffers. */
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Allocate a new index for inserted entry. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        range_end = BCM_KT_DEFIP_PAIR128_IDX_MAX(unit) + 1;
        if (BCM_KT_DEFIP_PAIR128_USED_COUNT(unit) == range_end) {
            /* If L3_DEFIP_PAIR_128 table is full and weighted prefix length
             * of new entry is >= that of 0th entry in L3_DEFIP_PAIR_128 table
             * then add the new entry in L3_DEFIP_128 table */
            lkup_plen = lpm_cfg->defip_sub_len * 
                ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);
            if (lkup_plen >= BCM_KT_DEFIP_PAIR128_ARR(unit)[0].prefix_len) {
                return _bcm_trx_defip_128_add(unit, lpm_cfg, nh_ecmp_idx);
            }
        }
        rv = _kt_defip_pair128_idx_alloc(unit, lpm_cfg, &hw_index, nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        hw_index = lpm_cfg->defip_index;
    }

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, mem, hw_entry, HIT0f, 1);
        soc_mem_field32_set(unit, mem, hw_entry, HIT1f, 1);
    }

    /* Set priority override bit. */
    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, hw_entry, RPEf, 1);
    }

    /* Set classification group id. */
    soc_mem_field32_set(unit, mem, hw_entry, CLASS_IDf, 
                        lpm_cfg->defip_lookup_class);


    /* Write priority field. */
    soc_mem_field32_set(unit, mem, hw_entry, PRIf, lpm_cfg->defip_prio);


    /* Fill next hop information. */
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        soc_mem_field32_set(unit, mem, hw_entry, ECMP_PTRf, nh_ecmp_idx);
        soc_mem_field32_set(unit, mem, hw_entry, ECMPf, 1);
    } else {
        soc_mem_field32_set(unit, mem, hw_entry, NEXT_HOP_INDEXf, nh_ecmp_idx);
    }

    /* Set destination discard flag. */
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, hw_entry, DST_DISCARDf, 1);
    }

    /* Set mode bits to indicate IP v6 128b pair mode */
    soc_mem_field32_set(unit, mem, hw_entry, MODE0_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE1_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE0_UPRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE1_UPRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE_MASK0_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE_MASK1_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE_MASK0_UPRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, MODE_MASK1_UPRf, 1);

    /* Set valid bits. */
    soc_mem_field32_set(unit, mem, hw_entry, VALID0_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, VALID1_LWRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, VALID0_UPRf, 1);
    soc_mem_field32_set(unit, mem, hw_entry, VALID1_UPRf, 1);

    /* Set destination/source ip address. */
    _kt_defip_pair128_ip6_addr_set(unit, mem, hw_entry, lpm_cfg->defip_ip6_addr);

    /* Set ip mask. */
    _kt_defip_pair128_ip6_mask_set(unit, mem, hw_entry, mask);

    /* Set vrf id & vrf mask. */
    if (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_0_LWRf, lpm_cfg->defip_vrf);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_1_LWRf, lpm_cfg->defip_vrf);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_0_UPRf, lpm_cfg->defip_vrf);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_1_UPRf, lpm_cfg->defip_vrf);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK0_LWRf,
                            (1 << soc_mem_field_length(unit, mem,
                                                       VRF_ID_MASK0_LWRf)) - 1);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK1_LWRf,
                            (1 << soc_mem_field_length(unit, mem,
                                                       VRF_ID_MASK1_LWRf)) - 1);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK0_UPRf,
                            (1 << soc_mem_field_length(unit, mem,
                                                       VRF_ID_MASK0_UPRf)) - 1);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK1_LWRf,
                            (1 << soc_mem_field_length(unit, mem,
                                                       VRF_ID_MASK1_UPRf)) - 1);
        if (SOC_MEM_FIELD_VALID(unit, mem, GLOBAL_ROUTEf)) {
            soc_mem_field32_set(unit, mem, hw_entry, GLOBAL_ROUTEf, 0);
        }
    } else {
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_0_LWRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_1_LWRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_0_UPRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_1_UPRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK0_LWRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK1_LWRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK0_UPRf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASK1_UPRf, 0);
    }

    /* Write buffer to hw. */
    rv = BCM_XGS3_MEM_WRITE(unit, mem, hw_index, hw_entry);
    if (BCM_FAILURE(rv)) {
        BCM_KT_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);
        return (rv);
    }

    /* Write source lookup portion of the entry. */
    if (SOC_URPF_STATUS_GET(unit)) {
        soc_mem_field32_set(unit, mem, hw_entry, SRC_DISCARDf, 0);
        rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m, 
                                (hw_index + BCM_KT_DEFIP_PAIR128_URPF_OFFSET(unit)),
                                hw_entry);
        if (BCM_FAILURE(rv)) {
            _kt_defip_pair128_entry_clear(unit, hw_index);
            return (rv);
        }
    }

    /* If new route added increment total number of routes.  */
    /* Lack of index indicates a new route. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, TRUE);
        BCM_KT_DEFIP_PAIR128_USED_COUNT(unit)++;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_kt_defip_pair128_delete
 * Purpose:
 *      Delete an entry from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt_defip_pair128_delete(int unit, _bcm_defip_cfg_t *lpm_cfg) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    uint32 temp_hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int hw_index = 0;                        /* Entry offset in the TCAM. */ 
    int rv, rv1;                             /* Internal operation status.*/
    _bcm_defip_cfg_t temp_lpm;
    int temp_nh_ecmp_idx, idx;
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info;

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);

    if (lpm_ipv6_info->ipv6_128b.depth == 0) {
       /* We cannot use L3_DEFIP for storing IPv6 128b lookup */
       return BCM_E_NOT_FOUND;
    }    


    /* First search for matching entry in L3_DEFIP_128 table.
     * If found then delete it. If it is not found in L3_DEFIP_128 table
     * then search in L3_DEFIP_PAIR_128 table and delete matching entry. */

    /* First try to delete from L3_DEFIP_128 table. If successful then exit.
     * If not found in L3_DEFIP_128m then search the L3_DEFIP_PAIR_128 table.
     * If it is found in L3_DEFIP_PAIR_128m then delete it. */  
    rv =  _bcm_trx_defip_128_delete(unit, lpm_cfg);
    if (rv == BCM_E_NOT_FOUND) {
        /* Create mask from prefix length. */
        bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

        /* Apply mask on address. */
        bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);
        /* Search for matching entry in theL3_DEFIP_PAIR_128  cam.  */
        rv1 =  _kt_defip_pair128_match(unit, lpm_cfg, hw_entry, &hw_index);
        if (BCM_FAILURE(rv1)) {
            return (rv1);
        }
        /* Delete entry from hardware & software (L3_DEFIP_PAIR_128m). */
        rv1 = _kt_defip_pair128_entry_clear(unit, hw_index);
        if (BCM_SUCCESS(rv1)) {
            BCM_XGS3_L3_DEFIP_CNT_DEC(unit, TRUE);
            BCM_KT_DEFIP_PAIR128_USED_COUNT(unit)--;
            /* Delete valid entry at highest index (or lowest LPM) in 
             * L3_DEFIP_128m table if available
             * and add it to L3_DEFIP_PAIR_128m */
            if (_bcm_trx_defip128_used_count_get(unit) > 0) {
                hw_index = _bcm_trx_defip128_idx_max_get(unit);
                for (idx = _bcm_trx_defip128_idx_max_get(unit);
                    idx >=0; idx--) {
                    if (_bcm_trx_defip128_array_idx_prefix_len_get(unit, idx)
                            == 0) {
                        hw_index =  idx - 1;
                        continue;
                    }
                    break;
                }
                rv = BCM_XGS3_MEM_READ(unit, L3_DEFIP_128m, hw_index, &temp_hw_entry);
                BCM_IF_ERROR_RETURN(rv);
                sal_memset(&temp_lpm, 0, sizeof(_bcm_defip_cfg_t));
                /* Parse entry. */
                rv = _kt_trx_defip_128_parse(unit, temp_hw_entry, &temp_lpm, &temp_nh_ecmp_idx);
                BCM_IF_ERROR_RETURN(rv);
                
                /* Extract entry address vrf */
                rv =  _kt_trx_defip_128_get_key(unit, temp_hw_entry, &temp_lpm);
                BCM_IF_ERROR_RETURN(rv);

                /* Add the copied entry to L3_DEFIP_PAIR_128 table */
                temp_lpm.defip_index = BCM_XGS3_L3_INVALID_INDEX;
                rv = _bcm_kt_defip_pair128_add(unit, &temp_lpm, temp_nh_ecmp_idx);
                BCM_IF_ERROR_RETURN(rv);

                /* Delete entry from L3_DEFIP_128 table. */
                temp_lpm.defip_index = hw_index;
                rv = _bcm_trx_defip_128_delete(unit, &temp_lpm);
                return rv;
            }
        }
        return rv1;
    } 
    return (rv);
}


/*
 * Function:
 *      _bcm_kt_defip_pair128_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_defip_pair128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int idx;                                 /* Iteration index.           */
    int rv;                                  /* Operation return status.   */
    int cmp_result;                          /* Test routine result.       */
    int nh_ecmp_idx;                         /* Next hop/Ecmp group index. */
    uint32 *hw_entry;                        /* Hw entry buffer.           */
    char *lpm_tbl_ptr;                       /* Dma table pointer.         */
    int defip_pair128_tbl_size;                    /* Defip table size.          */
    _bcm_defip_cfg_t lpm_cfg;                /* Buffer to fill route info. */
    soc_mem_t mem = L3_DEFIP_PAIR_128m;           /* Route table memory.        */ 
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);

    if (lpm_ipv6_info->ipv6_128b.depth == 0) {
       /* L3_DEFIP table is not used for storing IPv6 128b lookup */
       return BCM_E_NONE;
    }

    /* Table DMA the LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, mem, BCM_L3_MEM_ENT_SIZE(unit, mem),
                             "lpm_tbl", &lpm_tbl_ptr, &defip_pair128_tbl_size));

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_pair128_tbl_size >>= 1;
    } 

    for (idx = 0; idx < defip_pair128_tbl_size; idx++) {
        /* Calculate entry ofset. */
        hw_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                lpm_tbl_ptr, idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get (unit, mem, hw_entry, VALID0_LWRf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        /* Parse  the entry. */
        _kt_defip_pair128_parse(unit, hw_entry, &lpm_cfg, &nh_ecmp_idx);
        lpm_cfg.defip_index = idx;

        /* Fill entry ip address &  subnet mask. */
        _kt_defip_pair128_get_key(unit, hw_entry, &lpm_cfg);

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
            rv = _kt_defip_pair128_reinit(unit, idx, &lpm_cfg);
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

int
_bcm_kt_defip_pair128_used_count_get(int unit)
{
    return BCM_KT_DEFIP_PAIR128_USED_COUNT(unit);
}

/*
 * Function:
 *      _bcm_kt_l3_info_dump
 * Purpose:
 *      Dump the status of hardware.
 * Parameters:
 *      unit   - (IN)SOC unit number
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_kt_l3_info_dump(int unit)
{
    uint32 ipv6_lpm_128b_enable;
    uint32 ipv6_64b_free = 0;
    uint32 ipv6_64b_used = 0;
    uint32 ipv6_128b_free = 0;
    uint32 ipv6_128b_used = 0;
    uint32 ipv6_128b_total = 0;
    uint32 ipv4_free = 0;
    uint32 ipv4_used = 0;
    uint32 trx_defip128_idx_max = 0;
    uint32 trx_defip128_used_count = 0;
    soc_kt_lpm_ipv6_info_t *lpm_ipv6_info;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    trx_defip128_idx_max = SOC_IS_KATANA(unit) ?
        _bcm_trx_defip128_idx_max_get(unit) : 0;

    trx_defip128_used_count = SOC_IS_KATANA(unit) ?
        _bcm_trx_defip128_used_count_get(unit) : 0;

    if (SOC_IS_KATANA(unit)) {
        ipv6_lpm_128b_enable =
            soc_property_get(unit,spn_IPV6_LPM_128B_ENABLE, 0);
        LOG_CLI((BSL_META_U(unit,
                            "\n*************************************************")));
        LOG_CLI((BSL_META_U(unit,
                            "\nURPF check \t\t\t\t= %s"),
                 SOC_URPF_STATUS_GET(unit) ? "ENABLED" : "DISABLED"));
        LOG_CLI((BSL_META_U(unit,
                            "\nconfig ipv6_lpm_128b_enable \t\t= %d"),
                 ipv6_lpm_128b_enable));
        LOG_CLI((BSL_META_U(unit,
                            "\n*************************************************")));
        LOG_CLI((BSL_META_U(unit,
                            "\nMaximum LPM entries types")));
        LOG_CLI((BSL_META_U(unit,
                            "\n     IPv4 * IPv6 64 bit *IPv6 128 bit")));
        if (SOC_URPF_STATUS_GET(unit)) {
            if (ipv6_lpm_128b_enable == 0 ) {
                LOG_CLI((BSL_META_U(unit,
                                    "\nupto 6144   3072         128")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\nupto 2048   1024         1152")));
            }
        } else {
            if (ipv6_lpm_128b_enable == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\nupto 12288  6144         256")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\nupto 4096   2048         2304")));
            }
        }

        lpm_ipv6_info = soc_kt_lpm_ipv6_info_get(unit);
        /* IPv4, IPv6 64b and IPv6 pair128b entries share L3_DEFIPm 
         * Ipv6 64b and IPv6 pair128b are stored in mutually exclusive entries*/
        if (lpm_ipv6_info->ipv6_128b.depth > 0) {
            ipv6_128b_total = lpm_ipv6_info->ipv6_128b.depth +
                trx_defip128_idx_max + 1;
            ipv6_128b_used = BCM_KT_DEFIP_PAIR128_USED_COUNT(unit) + 
                            trx_defip128_used_count;
            ipv6_128b_free = ipv6_128b_total - ipv6_128b_used;
        } else {
            ipv6_128b_total = trx_defip128_idx_max + 1;
            ipv6_128b_used = trx_defip128_used_count;
            ipv6_128b_free = ipv6_128b_total - ipv6_128b_used;
        }
        ipv6_64b_used = BCM_XGS3_L3_DEFIP_IP6_CNT(unit) - ipv6_128b_used;
        ipv4_used = BCM_XGS3_L3_DEFIP_IP4_CNT(unit);        
        ipv6_64b_free = lpm_ipv6_info->ipv6_64b.depth -
                        ((ipv4_used+1)/2 + ipv6_64b_used);
        ipv4_free = ipv6_64b_free * 2;

        LOG_CLI((BSL_META_U(unit,
                            "\n*************************************************")));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv4 LPM entries in use \t\t= %5d"), ipv4_used));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv4 LPM entries free \t\t\t= %5d"), ipv4_free));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv4 LPM entries total \t\t\t= %5d"),
                 ipv4_free + ipv4_used));
        LOG_CLI((BSL_META_U(unit,
                            "\n**********")));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv6 64 bit LPM entries in use \t\t= %5d"),
                 ipv6_64b_used));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv6 64 bit LPM entries free \t\t= %5d"),
                 ipv6_64b_free));
        LOG_CLI((BSL_META_U(unit,
                            "\n64 bit LPM DEFIP entries total \t\t= %5d"),
                 ipv6_64b_used + ipv6_64b_free));
        LOG_CLI((BSL_META_U(unit,
                            "\n**********")));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv6 128 bit LPM entries in use \t= %5d"),
                 ipv6_128b_used));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv6 128 bit LPM entries free \t\t= %5d"),
                 ipv6_128b_free));
        LOG_CLI((BSL_META_U(unit,
                            "\nIPv6 128 bit LPM entries total \t\t= %5d"),
                 (ipv6_128b_used + ipv6_128b_free)));
        LOG_CLI((BSL_META_U(unit,
                            "\n**********")));
        LOG_CLI((BSL_META_U(unit,
                            "\nTotal IPv6 LPM entries in use \t\t= %5d"),
                 BCM_XGS3_L3_DEFIP_IP6_CNT(unit)));
        LOG_CLI((BSL_META_U(unit,
                            "\nTotal IPv6 LPM entries free \t\t= %5d"), 
                 (ipv6_64b_free + ipv6_128b_free)));
        LOG_CLI((BSL_META_U(unit,
                            "\nTotal IPv6 LPM entries \t\t\t= %5d\n"),
                 BCM_XGS3_L3_DEFIP_IP6_CNT(unit) +
                 (ipv6_64b_free + ipv6_128b_free)));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcmi_kt_extended_queue_config_destroy
 * Purpose:
 *      Destroy cosq settings done as part of l3 egress.
 * Parameters:
 *      unit   - (IN)SOC unit number
 *      idx    - (IN)Egress obj id.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcmi_kt_extended_queue_config_destroy(int unit, bcm_if_t idx)
{
    ing_queue_map_entry_t ing_queue_entry;
    ing_l3_next_hop_entry_t in_entry;
    int old_queue_id = 0;
    int mem = ING_L3_NEXT_HOPm;

    sal_memset(&in_entry, 0, BCM_XGS3_L3_ENT_SZ(unit, nh));
    /* Read ingress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, &in_entry));

    old_queue_id = soc_mem_field32_get(unit, mem, &in_entry,
                                  EH_QUEUE_TAGf);
    if (old_queue_id > 0) {
        /*  ING_QUEUE_MAP needs to be reset */
        sal_memset(&ing_queue_entry, 0, sizeof(ing_queue_map_entry_t));
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, ING_QUEUE_MAPm,
                                  old_queue_id, &ing_queue_entry));
    }
    return BCM_E_NONE;
}

#else /* BCM_KT_SUPPORT && INCLUDE_L3 */
typedef int bcm_kt_l3_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_KT_SUPPORT && INCLUDE_L3 */

