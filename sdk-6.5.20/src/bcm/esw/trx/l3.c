/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Triumph L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)

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
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>


_bcm_l3_defip_128_table_t *l3_trx_defip_128[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_tr_tnl_type_to_hw_code
 * Purpose:
 *      Translate tunnel type into value used in hardware
 *      (56624(Triumph) only.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      tnl_type    - (IN) Tunnel type.
 *      hw_code     - (OUT) HW code for tunnel type. 
 *      entry_type  - (OUT) Entry type.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_tnl_type_to_hw_code(int unit, bcm_tunnel_type_t tnl_type, 
                                 int *hw_code, int *entry_type)
{
    /* Input parameters check. */
    if ((NULL == hw_code) || (NULL == entry_type)) {
        return (BCM_E_PARAM);
    }

    switch (tnl_type) {
      case bcmTunnelTypeNone:
           *hw_code = 0;
           *entry_type = BCM_XGS3_TUNNEL_INIT_NONE;
           break;

      case bcmTunnelTypeIp4In4:
      case bcmTunnelTypeIp6In4:
      case bcmTunnelTypeIpAnyIn4:
           *hw_code = 0;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeIp4In6:
      case bcmTunnelTypeIp6In6:
      case bcmTunnelTypeIpAnyIn6:
           *hw_code = 0;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelType6In4Uncheck:
           *hw_code = 0x1;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeIsatap:
           *hw_code = 0x2;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelType6In4:
           *hw_code = 0x3;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeGre4In4:
      case bcmTunnelTypeGre6In4:
      case bcmTunnelTypeGreAnyIn4:
           *hw_code = 0x4;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeGre4In6:
      case bcmTunnelTypeGre6In6:
      case bcmTunnelTypeGreAnyIn6:
           *hw_code = 0x4;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelTypePimSmDr1:
           *hw_code = 0x5;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypePimSmDr2:
           *hw_code = 0x6;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypePim6SmDr1:
           *hw_code = 0x5;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelTypePim6SmDr2:
           *hw_code = 0x6;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelTypeL2Gre:
           *hw_code = 0x7;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeWlanAcToAc:
           *hw_code = 0x8;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeWlanAcToAc6:
           *hw_code = 0x8;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelTypeWlanWtpToAc:
           *hw_code = 0x9;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeWlanWtpToAc6:
           *hw_code = 0x9;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;

      case bcmTunnelTypeAutoMulticast:
           *hw_code = 0xA;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeAutoMulticast6:
           *hw_code = 0xA;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               *entry_type = BCM_XGS3_TUNNEL_INIT_V6_TD3;
           }
#endif
           break;
#ifdef BCM_MONTEREY_SUPPORT
      case bcmTunnelTypeRoeIn4:
           *hw_code = 0xC;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V4;
           break;

      case bcmTunnelTypeRoeIn6:
           *hw_code = 0xC;
           *entry_type = BCM_XGS3_TUNNEL_INIT_V6;
           break;
#endif
      default:
           break;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_tnl_hw_code_to_type
 * Purpose:
 *      Translate hw code for tunnel type into api value 
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      hw_tnl_type - (IN) HW tunnel code.
 *      entry_type  - (IN) IPv4/IPv6/MPLS entry. 
 *      tunnel_type - (OUT) Tunnel type. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_tnl_hw_code_to_type(int unit, int hw_tnl_type, 
                             int entry_type, bcm_tunnel_type_t *tunnel_type)
{
    /* Input parameters check. */
    if (NULL == tunnel_type) {
       return (BCM_E_PARAM);
    }

    if (BCM_XGS3_TUNNEL_INIT_NONE == entry_type) {
          *tunnel_type = bcmTunnelTypeNone;
          return (BCM_E_NONE);
    }

    switch (hw_tnl_type) {
      case 0:
          if (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) {
              *tunnel_type = bcmTunnelTypeIpAnyIn4;
          } else {
              *tunnel_type = bcmTunnelTypeIpAnyIn6;
          }
          break;
      case 0x1:
          *tunnel_type = bcmTunnelType6In4Uncheck;
          break;
      case 0x2:
          *tunnel_type = bcmTunnelTypeIsatap;
          break;
      case 0x3:
          *tunnel_type = bcmTunnelType6In4;
          break;
      case 0x4:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                   bcmTunnelTypeGreAnyIn4 : bcmTunnelTypeGreAnyIn6;
          break;
      case 0x5:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypePimSmDr1 : bcmTunnelTypePim6SmDr1;
          break;
      case 0x6:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypePimSmDr2 : bcmTunnelTypePim6SmDr2;
          break;
      case 0x7:
          *tunnel_type = bcmTunnelTypeL2Gre;
          break;
      case 0x8:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypeWlanAcToAc : bcmTunnelTypeWlanAcToAc6;
          break;
      case 0x9:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypeWlanWtpToAc : bcmTunnelTypeWlanWtpToAc6;
          break;
      case 0xA:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypeAutoMulticast : bcmTunnelTypeAutoMulticast6;
          break;
      case 0xB:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypeVxlan : bcmTunnelTypeVxlan6;
          break;
#ifdef BCM_MONTEREY_SUPPORT
      case 0xC:
          *tunnel_type = (BCM_XGS3_TUNNEL_INIT_V4 == entry_type) ? \
                         bcmTunnelTypeRoeIn4 : bcmTunnelTypeRoeIn6;
          break;
#endif
      default: 
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_defip_128_hash
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
_trx_defip_128_hash(int unit, _bcm_defip_cfg_t *lpm_cfg, uint8 *hash) 
{
    uint32 key[BCM_TRX_DEFIP128_HASH_SZ];

    if (NULL == hash) {
        return (BCM_E_PARAM);
    }

    SAL_IP6_ADDR_TO_UINT32(lpm_cfg->defip_ip6_addr, key);
    key[4] = lpm_cfg->defip_sub_len;
    key[5] = lpm_cfg->defip_vrf;

    *hash = (_shr_crc16b(0, (void *)key,
                         WORDS2BITS((BCM_TRX_DEFIP128_HASH_SZ))) & 0xff);
    return (BCM_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _trx_defip_128_reinit
 * Purpose:
 *      Re-Initialize sw image for L3_DEFIP_128 internal tcam.
 *      during warm boot. 
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      idx     - (IN)Entry index   
 *      lpm_cfg - (IN)Inserted entry. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_defip_128_reinit(int unit, int idx, _bcm_defip_cfg_t *lpm_cfg)
{
    int   lkup_plen;         /* Vrf weighted lookup prefix. */
    uint8 hash;              /* Entry hash value.           */

    /* Calculate vrf weighted prefix length. */
    lkup_plen = lpm_cfg->defip_sub_len * 
        ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);

    /* Calculate entry lookup hash. */
    BCM_IF_ERROR_RETURN(_trx_defip_128_hash(unit, lpm_cfg, &hash));

    /* Set software structure info. */
    BCM_TRX_DEFIP128_ENTRY_SET(unit, idx, lkup_plen, hash);

    /* Update software usage counter. */
    BCM_TRX_DEFIP128_USED_COUNT(unit)++;

    return (BCM_E_NONE);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_trx_defip_128_init
 * Purpose:
 *      Initialize sw image for L3_DEFIP_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_defip_128_init(int unit)
{   
    int mem_sz;    /* Memory allocation size     */
    int rv;        /* Internal operation status. */

    /* De-allocate any existing structures. */
    if (NULL != BCM_TRX_DEFIP128(unit)) {
        rv = _bcm_trx_defip_128_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Allocate cam control structure. */
    mem_sz = sizeof(_bcm_l3_defip_128_table_t);
    BCM_TRX_DEFIP128(unit) = sal_alloc(mem_sz, "l3_trx_defip_128");
    if (NULL == BCM_TRX_DEFIP128(unit)) {
        return (BCM_E_MEMORY);
    }

    /* Reset cam control structure. */
    sal_memset(BCM_TRX_DEFIP128(unit), 0, mem_sz);

    BCM_TRX_DEFIP128_TOTAL(unit) = soc_mem_index_count(unit, L3_DEFIP_128m);

                                                                
    if (SOC_URPF_STATUS_GET(unit)) {
        BCM_TRX_DEFIP128_TOTAL(unit) >>= 1;
        BCM_TRX_DEFIP128_URPF_OFFSET(unit) = BCM_TRX_DEFIP128_TOTAL(unit);
    } else {
        BCM_TRX_DEFIP128_URPF_OFFSET(unit) = 0;
    }

    BCM_TRX_DEFIP128_IDX_MAX(unit) = BCM_TRX_DEFIP128_TOTAL(unit) - 1;

    mem_sz = BCM_TRX_DEFIP128_TOTAL(unit) * sizeof(_bcm_l3_defip_128_entry_t);

    BCM_TRX_DEFIP128_ARR(unit) = sal_alloc(mem_sz, "l3_defip_128_entry_array");
    if (NULL == BCM_TRX_DEFIP128_ARR(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_trx_defip_128_deinit(unit));
        return (BCM_E_MEMORY);
    }
    sal_memset(BCM_TRX_DEFIP128_ARR(unit), 0, mem_sz);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_defip_128_deinit
 * Purpose:
 *      De-initialize sw image for L3_DEFIP_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_defip_128_deinit(int unit)
{   
    if (NULL == BCM_TRX_DEFIP128(unit)) {
        return (BCM_E_NONE);
    }


    /* De-allocate any existing structures. */
    if (NULL != BCM_TRX_DEFIP128_ARR(unit)) {
        sal_free(BCM_TRX_DEFIP128_ARR(unit));
        BCM_TRX_DEFIP128_ARR(unit) = NULL;
    }
    sal_free(BCM_TRX_DEFIP128(unit));
    BCM_TRX_DEFIP128(unit) = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _trx_defip_128_get_key
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
_trx_defip_128_get_key(int unit, uint32 *hw_entry, 
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
 *      _trx_defip_128_parse
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
_trx_defip_128_parse(int unit, uint32 *hw_entry,
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
 *      _trx_defip_128_match
 * Purpose:
 *      Lookup route entry from L3_DEFIP_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      hw_entry    - (OUT)Matching hw entry. 
 *      hw_index    - (OUT)HW table index. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_defip_128_match(int unit, _bcm_defip_cfg_t *lpm_cfg, 
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
    BCM_IF_ERROR_RETURN(_trx_defip_128_hash(unit, lpm_cfg, &hash));

    /*    coverity[assignment : FALSE]    */
    for (index = 0; index <= BCM_TRX_DEFIP128_IDX_MAX(unit); index++) {

        /* Route prefix length comparison. */
        if (lkup_plen != BCM_TRX_DEFIP128_ARR(unit)[index].prefix_len) {
            continue;
        }

        /* Route lookup key hash comparison. */
        if (hash != BCM_TRX_DEFIP128_ARR(unit)[index].entry_hash) {
            continue;
        }

        /* Hash & prefix length match -> read and compare hw entry itself. */
        rv = BCM_XGS3_MEM_READ(unit, L3_DEFIP_128m, index, hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Make sure entry is valid.  */
        if ((!soc_mem_field32_get(unit, L3_DEFIP_128m, hw_entry, VALID_0f)) ||
            (!soc_mem_field32_get(unit, L3_DEFIP_128m, hw_entry, VALID_1f))) {
            /* Too bad sw image doesn't match hardware. */
            continue;
        }

        /* Extract entry address vrf */
        rv =  _trx_defip_128_get_key(unit, hw_entry, &candidate);
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

    if (index > BCM_TRX_DEFIP128_IDX_MAX(unit))  {
        return (BCM_E_NOT_FOUND);
    }

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        uint32 hit_x, hit_y;
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_READ(unit, L3_DEFIP_128_HIT_ONLY_Ym, index, &hit_y));
        hit_x = soc_mem_field32_get(unit, L3_DEFIP_128m, hw_entry, HITf);
        hit_x |= hit_y;
        soc_mem_field32_set(unit, L3_DEFIP_128m, hw_entry, HITf, hit_x);
    }
#endif /* BCM_TRIDENT_SUPPORT */

    *hw_index = index;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _trx_defip_128_entry_clear
 * Purpose:
 *      Clear/Reset a single route entry. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      hw_index  - (IN)Entry index in the tcam. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_defip_128_entry_clear(int unit, int hw_index)  
{

    /* Reset hw entry. */
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, hw_index,
                            soc_mem_entry_null(unit, L3_DEFIP_128m)));

    /* Reset sw entry. */
    BCM_TRX_DEFIP128_ENTRY_SET(unit, hw_index, 0, 0);

    /* Reset urpf source lookup entry. */
    if (SOC_URPF_STATUS_GET(unit)) {
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, 
                                (hw_index + BCM_TRX_DEFIP128_URPF_OFFSET(unit)),
                                soc_mem_entry_null(unit, L3_DEFIP_128m)));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _trx_defip_128_shift
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
_trx_defip_128_shift(int unit, int idx_src, int idx_dest)  
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */

    /* Don't copy uninstalled entries. */
    if (0 != BCM_TRX_DEFIP128_ARR(unit)[idx_src].prefix_len) {
        /* Read entry from source index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_READ(unit, L3_DEFIP_128m, idx_src, hw_entry));

        /* Write entry to the destination index. */
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, idx_dest, hw_entry));

        /* Shift corresponding  URPF/source lookup entries. */
        if (SOC_URPF_STATUS_GET(unit)) {
            /* Read entry from source index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_READ(unit, L3_DEFIP_128m, 
                                   idx_src + BCM_TRX_DEFIP128_URPF_OFFSET(unit),
                                   hw_entry));

            /* Write entry to the destination index + half tcam. */
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, 
                                    idx_dest +  BCM_TRX_DEFIP128_URPF_OFFSET(unit),
                                    hw_entry));
        }
    }
    /* Copy software portion of the entry. */ 
    sal_memcpy (&BCM_TRX_DEFIP128_ARR(unit)[idx_dest],
                &BCM_TRX_DEFIP128_ARR(unit)[idx_src], 
                sizeof(_bcm_l3_defip_128_entry_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _trx_defip_128_shift_range
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
_trx_defip_128_shift_range(int unit, int idx_start, int idx_end)  
{
    int idx;     /* Iteration index. */
    int rv = BCM_E_NONE;   /* Operaton status. */ 

    /* Shift entries down (1 index up) */
    if (idx_end > idx_start) {
        for (idx = idx_end - 1; idx >= idx_start; idx--) {
            rv = _trx_defip_128_shift(unit, idx, idx + 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _trx_defip_128_entry_clear(unit, idx_start);
    }

    /* Shift entries up (1 index down) */
    if (idx_end < idx_start) {
        for (idx = idx_end + 1; idx <= idx_start; idx++) {
            rv = _trx_defip_128_shift(unit, idx, idx - 1);  
            if (BCM_FAILURE(rv)) { 
                break;
            }
        }
        /* Delete last entry from hardware & software. */
        rv = _trx_defip_128_entry_clear(unit, idx_start);
    }

    return (rv);
}
/*
 * Function:
 *      _trx_defip_128_idx_alloc
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
_trx_defip_128_idx_alloc(int unit, _bcm_defip_cfg_t *lpm_cfg, int *hw_index) 
{
    int  lkup_plen;                   /* Vrf weighted lookup prefix.    */
    uint8 hash;                       /* Entry hash value.              */
    int  idx;                         /* Table iteration index.         */
    int  rv;                          /* Operation return status.       */
    int  shift_min = 0;               /* Minimum entry shift required.  */
    int  range_start = 0;             /* First valid location for pfx.  */
    int  range_end = BCM_TRX_DEFIP128_IDX_MAX(unit);  
                                      /* Last valid location for pfx.   */
    int  free_idx_before = -1;        /* Free index before range_start. */
    int  free_idx_after  = -1;        /* Free index after range_end.    */

    /* Input parameters check. */
    if ((NULL == lpm_cfg) || (NULL == hw_index)) {
        return (BCM_E_PARAM);
    }

    /* Calculate vrf weighted prefix length. */
    lkup_plen = lpm_cfg->defip_sub_len * 
        ((BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) ? 2 : 1);

    /* Calculate entry hash. */
    BCM_IF_ERROR_RETURN(_trx_defip_128_hash(unit, lpm_cfg, &hash));

    /* Make sure at least one slot is available. */
    if (BCM_TRX_DEFIP128_USED_COUNT(unit) == BCM_TRX_DEFIP128_TOTAL(unit)) {
        return (BCM_E_FULL);
    }

    /* 
     *  Find first valid index for the prefix;
     */ 
    for (idx = 0; idx <= BCM_TRX_DEFIP128_IDX_MAX(unit); idx++) {

        /* Remeber free entry before installation location. */
        if (0 == BCM_TRX_DEFIP128_ARR(unit)[idx].prefix_len) {
            free_idx_before = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen < BCM_TRX_DEFIP128_ARR(unit)[idx].prefix_len) {
            range_start = idx + 1;
            continue;
        }
        break;
    }

    /* 
     *  Find last valid index for the prefix;
     */ 
    for (idx = BCM_TRX_DEFIP128_IDX_MAX(unit); idx >= 0; idx--) {

        /* Remeber free entry after installation location. */
        if (0 == BCM_TRX_DEFIP128_ARR(unit)[idx].prefix_len) {
            free_idx_after = idx;
            continue;
        }
        /* Route prefix length comparison. */
        if (lkup_plen >  BCM_TRX_DEFIP128_ARR(unit)[idx].prefix_len) {
            range_end = idx;
            continue;
        }
        break;
    }

    /* Check if entry should be inserted at the end. */
    if (range_start > BCM_TRX_DEFIP128_IDX_MAX(unit)) {
    /*    coverity[negative_returns : FALSE]    */
        /* Shift all entries from free_idx_before till the end 1 up. */
    /* coverity[negative_returns] */
        rv = _trx_defip_128_shift_range(unit, BCM_TRX_DEFIP128_IDX_MAX(unit),
                                        free_idx_before);

        if (BCM_SUCCESS(rv)) { 
            /* Allocated index is last TCAM entry. */
            *hw_index = BCM_TRX_DEFIP128_IDX_MAX(unit);
            BCM_TRX_DEFIP128_ENTRY_SET(unit, *hw_index, lkup_plen, hash);
        }
        return (rv);
    }

    /* Check if entry should be inserted at the beginning. */
    if (range_end < 0) {
        /* Shift all entries from 0 till free_idx_after 1 down. */
        rv = _trx_defip_128_shift_range(unit, 0, free_idx_after);

        if (BCM_SUCCESS(rv)) {
            /* Allocated index is first TCAM entry. */
            *hw_index = 0;
            BCM_TRX_DEFIP128_ENTRY_SET(unit, 0, lkup_plen, hash);
        }
        return (rv);
    }

    /* Find if there is any free index in range. */
    for (idx = range_start; idx <= range_end; idx++) {
        if (0 == BCM_TRX_DEFIP128_ARR(unit)[idx].prefix_len) {
            *hw_index = idx;
            BCM_TRX_DEFIP128_ENTRY_SET(unit, idx, lkup_plen, hash);
            return (BCM_E_NONE);
        }
    }

    /* Check if entry should be inserted at the end of range. */
    if ((free_idx_after > range_end) && (free_idx_after != -1))  {
        shift_min = free_idx_after - range_end;
    } 

    /* Check if entry should be inserted in the beginning. */
    if ((free_idx_before < range_start) && (free_idx_before != -1))  {
        if(shift_min > (range_start - free_idx_before)) {
            shift_min = -1;
        }
    /*    coverity[negative_returns : FALSE]    */
    }

    /* coverity[negative_returns] */
    if (shift_min <= 0) {
        rv = _trx_defip_128_shift_range(unit, range_start, free_idx_before);
    /*    coverity[negative_returns : FALSE]    */
        if (BCM_SUCCESS(rv)) {
            *hw_index= range_start;
    /*    coverity[negative_returns : FALSE]    */
            BCM_TRX_DEFIP128_ENTRY_SET(unit, range_start, lkup_plen, hash);
    /* coverity[negative_returns] */
    /* coverity[negative_returns] */
        }
    } else {
    /* coverity[negative_returns] */
    /* coverity[negative_returns] */
        rv = _trx_defip_128_shift_range(unit, range_end, free_idx_after);
        if (BCM_SUCCESS(rv)) {
            *hw_index = range_end;
            BCM_TRX_DEFIP128_ENTRY_SET(unit, range_end, lkup_plen, hash);
        }
    }
    return (rv);
}
/*
 * Function:
 *      _bcm_trx_defip_128_get
 * Purpose:
 *      Get an entry from L3_DEFIP_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_defip_128_get(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                       int *nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int clear_hit;                           /* Clear entry hit bit flag. */
    int hw_index = 0;                            /* Entry offset in the TCAM. */ 
    int rv;                                  /* Internal operation status.*/

    /* Input parameters check. */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    /* Check if clear hit bit required. */
    clear_hit = lpm_cfg->defip_flags & BCM_L3_HIT_CLEAR;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);


    /* Search for matching entry in the cam. */
    rv =  _trx_defip_128_match(unit, lpm_cfg, hw_entry, &hw_index);
    BCM_IF_ERROR_RETURN(rv);

    lpm_cfg->defip_index = hw_index;

    /* Parse matched entry. */
    rv = _trx_defip_128_parse(unit, hw_entry, lpm_cfg, nh_ecmp_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the HIT bit */
    if (clear_hit) {
        soc_mem_field32_set(unit, L3_DEFIP_128m, hw_entry, HITf, 0);

        rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, hw_index, hw_entry);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_trx_defip_128_add
 * Purpose:
 *      Add an entry to internal L3_DEFIP_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_defip_128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                       int nh_ecmp_idx) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];   /* New entry hw buffer.      */
    bcm_ip6_t mask;                             /* Subnet mask.              */
    int hw_index = 0;                           /* Entry offset in the TCAM. */ 
    int rv;                                     /* Operation return status.  */
    soc_mem_t mem = L3_DEFIP_128m;              /* Route table memory.       */ 

    /* Input parameters check. */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    /* No support for routes matchin AFTER vrf specific. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        return (BCM_E_UNAVAIL);
    }

    /* Zero buffers. */
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Allocate a new index for inserted entry. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        rv = _trx_defip_128_idx_alloc(unit, lpm_cfg, &hw_index);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        hw_index = lpm_cfg->defip_index;
    }

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, mem, hw_entry, HITf, 1);
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

    /* Set valid bits. */
    soc_mem_field32_set(unit, mem, hw_entry, VALID_0f, 1);
    soc_mem_field32_set(unit, mem, hw_entry, VALID_1f, 1);


    /* Set destination/source ip address. */
    soc_mem_ip6_addr_set(unit, mem, hw_entry, IP_ADDRf, lpm_cfg->defip_ip6_addr,
                         SOC_MEM_IP6_FULL_ADDR);

    /* Set ip mask. */
    soc_mem_ip6_addr_set(unit, mem, hw_entry, IP_ADDR_MASKf, mask,
                         SOC_MEM_IP6_FULL_ADDR);

    /* Set vrf id & vrf mask. */
    if (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, mem, hw_entry, VRF_IDf, lpm_cfg->defip_vrf);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASKf,
                            (1 << soc_mem_field_length(unit, mem,
                                                       VRF_ID_MASKf)) - 1);
        if (SOC_MEM_FIELD_VALID(unit, mem, GLOBAL_ROUTEf)) {
            soc_mem_field32_set(unit, mem, hw_entry, GLOBAL_ROUTEf, 0);
        }
    } else {
        soc_mem_field32_set(unit, mem, hw_entry, VRF_IDf, 0);
        soc_mem_field32_set(unit, mem, hw_entry, VRF_ID_MASKf, 0);
    }

    /* Write buffer to hw. */
    rv = BCM_XGS3_MEM_WRITE(unit, mem, hw_index, hw_entry);
    if (BCM_FAILURE(rv)) {
        BCM_TRX_DEFIP128_ENTRY_SET(unit, hw_index, 0, 0);
        return (rv);
    }

    /* Write source lookup portion of the entry. */
    if (SOC_URPF_STATUS_GET(unit)) {
        soc_mem_field32_set(unit, mem, hw_entry, SRC_DISCARDf, 0);
        rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_128m, 
                                (hw_index + BCM_TRX_DEFIP128_URPF_OFFSET(unit)),
                                hw_entry);
        if (BCM_FAILURE(rv)) {
            _trx_defip_128_entry_clear(unit, hw_index);
            return (rv);
        }
    }

    /* If new route added increment total number of routes.  */
    /* Lack of index indicates a new route. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, TRUE);
        BCM_TRX_DEFIP128_USED_COUNT(unit)++;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_trx_defip_128_delete
 * Purpose:
 *      Delete an entry from L3_DEFIP_128 table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_defip_128_delete(int unit, _bcm_defip_cfg_t *lpm_cfg) 
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];/* Hw entry buffer.          */
    bcm_ip6_t mask;                          /* Subnet mask.              */
    int hw_index;                            /* Entry offset in the TCAM. */ 
    int rv;                                  /* Internal operation status.*/
    /* Input parameters check. */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Search for matching entry in the cam. */
    rv =  _trx_defip_128_match(unit, lpm_cfg, hw_entry, &hw_index);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    /* Delete entry from hardware & software. */
    rv = _trx_defip_128_entry_clear(unit, hw_index);
    if (BCM_SUCCESS(rv)) {
        BCM_XGS3_L3_DEFIP_CNT_DEC(unit, TRUE);
        BCM_TRX_DEFIP128_USED_COUNT(unit)--;
    }

    return (rv);
}


/*
 * Function:
 *      _bcm_trx_defip_128_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_defip_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int idx;                                 /* Iteration index.           */
    int rv;                                  /* Operation return status.   */
    int cmp_result;                          /* Test routine result.       */
    int nh_ecmp_idx;                         /* Next hop/Ecmp group index. */
    uint32 *hw_entry;                        /* Hw entry buffer.           */
    char *lpm_tbl_ptr;                       /* Dma table pointer.         */
    int defip_table_size;                    /* Defip table size.          */
    _bcm_defip_cfg_t lpm_cfg;                /* Buffer to fill route info. */
    soc_mem_t mem = L3_DEFIP_128m;           /* Route table memory.        */ 

    /* Table DMA the LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, mem, BCM_L3_MEM_ENT_SIZE(unit, mem),
                             "lpm_tbl", &lpm_tbl_ptr, &defip_table_size));

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    } 

    for (idx = 0; idx < defip_table_size; idx++) {
        /* Calculate entry ofset. */
        hw_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                lpm_tbl_ptr, idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get (unit, mem, hw_entry, VALID_0f)) {
            continue;
        }

#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TRIDENT(unit)) {
            uint32 hit_x, hit_y;
            BCM_IF_ERROR_RETURN
                (BCM_XGS3_MEM_READ(unit, L3_DEFIP_128_HIT_ONLY_Ym, idx,
                                   &hit_y));
            hit_x = soc_mem_field32_get(unit, L3_DEFIP_128m, hw_entry, HITf);
            hit_x |= hit_y;
            soc_mem_field32_set(unit, L3_DEFIP_128m, hw_entry, HITf, hit_x);
        }
#endif /* BCM_TRIDENT_SUPPORT */

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        /* Parse  the entry. */
        _trx_defip_128_parse(unit, hw_entry, &lpm_cfg, &nh_ecmp_idx);
        lpm_cfg.defip_index = idx;

        /* Fill entry ip address &  subnet mask. */
        _trx_defip_128_get_key(unit, hw_entry, &lpm_cfg);

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
            rv = _trx_defip_128_reinit(unit, idx, &lpm_cfg);
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

/*
 * Function:
 *      _bcm_trx_l3_defip_mem_get
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
int
_bcm_trx_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
    /* Input parameters check. */
    if (NULL == mem) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_l3_defip_mem_get(unit, flags, plen, mem));
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SCORPION(unit)) {
        if ((flags & BCM_L3_IP6) && (plen > 64)) {
            *mem = L3_DEFIP_128m;
        } else {
            *mem = L3_DEFIPm;
        }
    }
#endif /* BCM_SCORPION_SUPPORT */
    return (BCM_E_NONE);
}

#if defined(BCM_TRIUMPH_SUPPORT)
/*
 * Function:
 *      _bcm_trx_l3_defip_verify_internal_mem_usage
 * Purpose:
 *      Verify if entries are present within INT-DEFIP table
 * Parameters:
 *      unit  - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_trx_l3_defip_verify_internal_mem_usage(int unit)
{
    if (soc_feature(unit, soc_feature_esm_support)) {
        if (SOC_MEM_IS_VALID(unit, L3_DEFIP_128m)) {
            if (soc_mem_index_count(unit, L3_DEFIP_128m)) {
                if (BCM_TRX_DEFIP128(unit) != NULL) {
                    if (BCM_TRX_DEFIP128_USED_COUNT(unit)) {
                        return BCM_E_DISABLED;
                    }
                }
            }
        }
    }
    return BCM_E_NONE;
}
#endif


int
_bcm_trx_defip128_used_count_get(int unit)
{
    return BCM_TRX_DEFIP128_USED_COUNT(unit);
}

int
_bcm_trx_defip128_idx_max_get(int unit)
{
    return BCM_TRX_DEFIP128_IDX_MAX(unit);
}

int
_bcm_trx_defip128_array_idx_prefix_len_get(int unit, int arr_idx)
{
    return BCM_TRX_DEFIP128_ARR(unit)[arr_idx].prefix_len;
}

#else /* BCM_TRX_SUPPORT && INCLUDE_L3 */
typedef int bcm_trx_l3_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
