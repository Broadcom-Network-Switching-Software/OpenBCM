/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Tomahawk L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>

#if defined(INCLUDE_L3) && defined(ALPM_ENABLE)
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)

#include <soc/drv.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>

#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>

#include <soc/tomahawk.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>

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
#include <bcm_int/esw/qos.h>

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

extern int l3_alpm_ipv4_double_wide[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      _bcm_th_alpm_lpm_vrf_init
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
_bcm_th_alpm_lpm_vrf_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
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
 *     _bcm_th_alpm_lpm64_key_init
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Parameters:
 *    unit    - (IN) SOC unit number;
 *    lpm_key - (OUT) Buffer to fill.
 *    lpm_cfg - (IN) Route information.
 *    alpm_entry - (OUT) ALPM entry to fill
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_alpm_lpm64_key_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
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
 *      _bcm_th_alpm_lpm_init
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
_bcm_th_alpm_lpm_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                      defip_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    bcm_ip_t ip4_mask;
    int vrf_id;
    int vrf_mask;
    int ipv6;

    ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (_bcm_th_alpm_lpm_vrf_init(unit, lpm_cfg, &vrf_id, &vrf_mask));


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
            soc_mem_field32_dest_set(unit, L3_DEFIPm, lpm_entry,
                                     DESTINATION0f, dest_type, dest_val);
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
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_VIEW0f,
            _soc_th_alpm_lpm_entry_view_get(unit, ipv6, 0x1));
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
         * RP ID needs uplift by _BCM_L3_DEFIP_RP_ID_BASE value,
         * Now it wil fit in EXPECTED_L3_IIFf
         */
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_L3_DEFIPm_field32_set(unit, lpm_entry, EXPECTED_L3_IIF0f,
                    _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));
        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK)
                && (lpm_cfg->defip_expected_intf != 0))  {

            soc_L3_DEFIPm_field32_set(unit, lpm_entry, EXPECTED_L3_IIF0f,
                    lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_L3_DEFIPm_field32_set(unit, lpm_entry,
                        IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_L3_DEFIPm_field32_set(unit, lpm_entry,
                        IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f, 1);
            }
        }
    }

    if (ipv6) {
        /* Set prefix ip address & mask. */
        _bcm_th_alpm_lpm64_key_init(unit, lpm_cfg, lpm_entry);

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
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, DWf)) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, DWf, 0);
    }

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
 *     _bcm_th_alpm_lpm128_addr_init
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
_bcm_th_alpm_lpm128_addr_init(int unit, soc_mem_t mem, uint32 *entry,
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
 *     _bcm_th_alpm_lpm128_mask_init
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
_bcm_th_alpm_lpm128_mask_init(int unit, soc_mem_t mem, uint32 *entry,
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
_bcm_th_alpm_lpm128_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
             defip_pair_128_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    int vrf_id;
    int vrf_mask;
    bcm_ip6_t mask;                          /* Subnet mask.              */

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (_bcm_th_alpm_lpm_vrf_init(unit, lpm_cfg, &vrf_id, &vrf_mask));

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
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_VIEWf,
            _soc_th_alpm_lpm_entry_view_get(unit, L3_DEFIP_MODE_128, 0x1));
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

    if (soc_feature(unit, soc_feature_ipmc_defip) &&
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, MULTICAST_ROUTEf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                    MULTICAST_ROUTEf, 1);
        } else if (soc_mem_field_valid(unit, L3_DEFIP_PAIR_128m, DATA_TYPEf)) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                    DATA_TYPEf, 2);
        }

        if (soc_feature(unit, soc_feature_generic_dest)) {
            if (lpm_cfg->defip_mc_group > 0) {
                soc_mem_field32_dest_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                         DESTINATIONf, SOC_MEM_FIF_DEST_IPMC,
                                         lpm_cfg->defip_mc_group);
            }
        } else {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, L3MC_INDEXf,
                    lpm_cfg->defip_mc_group);
        }

        /*
         * RP ID needs uplift by _BCM_L3_DEFIP_RP_ID_BASE value,
         * Now it wil fit in EXPECTED_L3_IIFf
         */
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                    EXPECTED_L3_IIFf, _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));
        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) &&
                (lpm_cfg->defip_expected_intf != 0))  {

            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                    EXPECTED_L3_IIFf, lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                        IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                        IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, 1);
            }
        }
    }

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Set prefix ip address & mask. */
    _bcm_th_alpm_lpm128_addr_init(unit, L3_DEFIP_PAIR_128m,
                                  (uint32 *)lpm_entry,
                                  lpm_cfg->defip_ip6_addr);

    _bcm_th_alpm_lpm128_mask_init(unit, L3_DEFIP_PAIR_128m,
                                  (uint32 *)lpm_entry, mask);

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
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIP_PAIR_128m, DWf)) {
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, DWf, 1);
    }

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
 *     _bcm_th_alpm_lpm64_key_parse
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_alpm_lpm64_key_parse(int unit, const void *lpm_key,
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
 *      _bcm_th_alpm_lpm_key_parse
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
_bcm_th_alpm_lpm_key_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                           defip_entry_t *lpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6, tmp;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_th_alpm_lpm64_key_parse(unit, lpm_entry, lpm_cfg);
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
    (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &tmp);

    return;
}


/*
 * Function:
 *      _bcm_th_alpm_lpm_data_parse
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
_bcm_th_alpm_lpm_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            int *p_nh_ecmp_idx, defip_entry_t *lpm_entry)
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

    /* Check if entry points to ecmp group. */
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
            if (soc_L3_DEFIPm_field32_get(unit,
                        lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (soc_L3_DEFIPm_field32_get(unit,
                        lpm_entry, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
            /*
             * RP ID and ExpectedIIF are overlaid.
             */
            val = soc_L3_DEFIPm_field32_get(unit,
                                                   lpm_entry, EXPECTED_L3_IIF0f);
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
 *     _bcm_th_alpm_lpm128_mask_parse
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
_bcm_th_alpm_lpm128_mask_parse(int unit, soc_mem_t mem, const void *entry,
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
 *     _bcm_th_alpm_lpm128_addr_parse
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
_bcm_th_alpm_lpm128_addr_parse(int unit, soc_mem_t mem, const void *entry,
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
 *      _bcm_th_alpm_lpm128_key_parse
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
_bcm_th_alpm_lpm128_key_parse(int unit, uint32 *hw_entry,
                              _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;  /* Route table memory. */
    bcm_ip6_t mask;                      /* Subnet mask.        */

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Extract ip6 address. */
    _bcm_th_alpm_lpm128_addr_parse(unit, mem, hw_entry,
                                   lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _bcm_th_alpm_lpm128_mask_parse(unit, mem, hw_entry, mask);
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
_bcm_th_alpm_lpm128_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                               int *p_nh_ecmp_idx, defip_pair_128_entry_t *lpm_entry)
{
    int is_ecmp = 0;
    uint32 nh_ecmp_idx = 0;
    soc_mem_t mem = L3_DEFIP_PAIR_128m;
    int ipv6 = soc_mem_field32_get(unit, mem, lpm_entry, MODE0_LWR_f(unit));

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

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

    /* Check if entry points to ecmp group. */
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

    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        int ipmc_route = 0;
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

        if (ipmc_route) {
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
                        L3_DEFIP_PAIR_128m, lpm_entry,
                        DESTINATIONf, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_IPMC) {
                    lpm_cfg->defip_mc_group = dest_value;
                } else {
                    lpm_cfg->defip_mc_group = 0;
                }
            } else {
                lpm_cfg->defip_mc_group =
                    soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                        lpm_entry, L3MC_INDEXf);
            }
        }
    }

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
 *     _bcm_th_alpm_ent64_key_parse
 * Purpose:
 *    Set an IP6 address field in an ALPM memory
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_alpm_ent64_key_parse(int unit, const void *alpm_key, soc_mem_t mem,
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
 *      _bcm_th_alpm_ent_key_parse
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
_bcm_th_alpm_ent_key_parse(int unit, _bcm_defip_cfg_t *lpm_cfg,
                           defip_entry_t *lpm_entry, soc_mem_t alpm_mem,
                           void *alpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_th_alpm_ent64_key_parse(unit, alpm_entry, alpm_mem, lpm_cfg);
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
    (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &ipv6);

    return;
}


/*
 * Function:
 *      _bcm_th_alpm_ent_data_parse
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
STATIC void
_bcm_th_alpm_ent_data_parse(int unit, _bcm_defip_cfg_t *lpm_cfg, int *p_nh_ecmp_idx,
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
    } else {
        is_ecmp = soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMPf);
        if (is_ecmp) {
            nh_ecmp_idx =
                soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMP_PTRf);
        } else {
            nh_ecmp_idx =
                soc_mem_field32_get(unit, alpm_mem, alpm_entry,
                                    NEXT_HOP_INDEXf);
        }
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
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
_bcm_th_alpm_ent128_key_parse(int unit, soc_mem_t mem, uint32 *alpm_entry,
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
_bcm_th_alpm_ent128_data_parse(int unit, soc_mem_t mem, uint32 *hw_entry,
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

/* As part of ALPM VRF WAR */
STATIC int
_bcm_th_alpm_warmboot_db_type_update(int unit)
{
    int rv = BCM_E_NONE;

#if 0
    int i, defip_table_size = 0;
    char *aux_tbl_ptr = NULL;
    defip_aux_table_entry_t *aux_entry;
    int is_urpf_entry, urpf = 0;
    uint32 vrf, valid, db_type, ndb_type, nent_type;
    soc_mem_t mem = L3_DEFIP_AUX_TABLEm;
    int updated = 0;

    if (!SOC_IS_TOMAHAWKX(unit)) {
        return rv;
    }
#endif

    return rv;
#if 0
    if (SOC_URPF_STATUS_GET(unit)) {
        urpf = 1;
    }

    rv = bcm_xgs3_l3_tbl_dma(unit, mem,
                             sizeof(defip_aux_table_entry_t), "aux_tbl",
                             &aux_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = 0; i < defip_table_size; i++) {
        aux_entry = soc_mem_table_idx_to_pointer(unit, mem,
                        defip_aux_table_entry_t *, aux_tbl_ptr, i);
        if (urpf && i >= defip_table_size / 2) {
            is_urpf_entry = 1;
        } else {
            is_urpf_entry = 0;
        }

        /* Entry 0 */
        valid = soc_mem_field32_get(unit, mem, aux_entry, VALID0f);
        if (valid) {
            vrf = soc_mem_field32_get(unit, mem, aux_entry, VRF0f);
            db_type = soc_mem_field32_get(unit, mem, aux_entry, DB_TYPE0f);
            if (db_type > 0) { /* > 0 means Private routes */
                soc_alpm_db_ent_type_encoding(unit, vrf, &ndb_type, &nent_type);
                if (is_urpf_entry) {
                    ndb_type ++;
                }
                /* Old value is only 2(3), new value could be 4(5), 6(7) */
                if (db_type != ndb_type) {
                    soc_mem_field32_set(unit, mem, aux_entry, DB_TYPE0f, ndb_type);
                    soc_mem_field32_set(unit, mem, aux_entry, ENTRY_TYPE0_f(unit), nent_type);
                    updated = 1;
                }
            }
        }

        /* Entry 1 */
        valid = soc_mem_field32_get(unit, mem, aux_entry, VALID1f);
        if (valid) {
            vrf = soc_mem_field32_get(unit, mem, aux_entry, VRF1f);
            db_type = soc_mem_field32_get(unit, mem, aux_entry, DB_TYPE1f);
            if (db_type > 0) { /* > 0 means Private routes */
                soc_alpm_db_ent_type_encoding(unit, vrf, &ndb_type, &nent_type);
                if (is_urpf_entry) {
                    ndb_type ++;
                }
                /* Old value is only 2(3), new value could be 4(5), 6(7) */
                if (db_type != ndb_type) {
                    soc_mem_field32_set(unit, mem, aux_entry, DB_TYPE1f, ndb_type);
                    soc_mem_field32_set(unit, mem, aux_entry, ENTRY_TYPE1_f(unit), nent_type);
                    updated = 1;
                }
            }
        }
    }

    if (updated) {
        rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                soc_mem_index_min(unit, mem),
                soc_mem_index_max(unit, mem), aux_tbl_ptr);
    }

cleanup:
    if (aux_tbl_ptr) {
        soc_cm_sfree(unit, aux_tbl_ptr);
    }
    return rv;
#endif
}

/*
 * Function:
 *      _bcm_th_alpm_warmboot_walk
 * Purpose:
 *      Recover LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_alpm_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6;                   /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf, vrf_id;            /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkti = 0;               /*                              */
    int bkt_ptr2, bkt_ptr = 0;  /* Bucket pointer               */
    int sub_bkt_idx = 0;        /* Sub bucket pointer           */
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
    int bank_bits;              /* 2 bank mode, or 4 bank mode  */
    uint32 bank_disable;        /* Bank disable mask            */
    int ipmc_route = 0;         /* Route is ip multicast type   */
    int flex = 0;               /* Route can be with stat flex  */
    int alpm_bkt_bits = ALPM_CTRL(unit).bkt_bits;

    defip_alpm_ipv4_1_entry_t    alpm_entry_v4;
    defip_alpm_ipv6_64_1_entry_t alpm_entry_v6_64;
    defip_entry_t                local_lpm_entry;
    int bkt_ptr_tmp = 0;
    uint8 bkt_occupied = 0;

    _bcm_th_alpm_warmboot_db_type_update(unit);

    /* DMA LPM table to software copy */
    rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto free_lpm_table;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end    = defip_table_size;
    bank_count = ALPM_CTRL(unit).num_banks;
    bank_bits  = ALPM_CTRL(unit).bank_bits;

    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                         defip_entry_t *, lpm_tbl_ptr, idx);

        /* Avoid to modify the Hw entry buffer*/
        sal_memcpy(&local_lpm_entry, lpm_entry, sizeof(defip_entry_t));

        lpm_entry = &local_lpm_entry;

        ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0_f(unit));

        /* Calculate LPM table traverse step count */
        step_count = ipv6 ? 1 : 2;
      
        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (soc_th_alpm_warmboot_lpm_reinit(unit, ipv6, idx, lpm_entry) < 0) {
            goto free_lpm_table;
        }

        bkt_occupied = 0;
 
        /*
         * This loop is used for two purposes
         *  1. IPv4, DEFIP entry has two IPv4 entries.
         *  2. IPv6 double wide mode, walk next bucket.
         */
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {  /* If index == 1 */
                /* Copy upper half of lpm entry to lower half */
                soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
               
                /* Invalidate upper half */
                soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 0);
            }
    
            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            /* Extract bucket pointer from LPM entry */
            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                          ALG_BKT_PTR0f);
            bkt_ptr_tmp = bkt_ptr;

            if (ALPM_CTRL(unit).bkt_sharing) {
                sub_bkt_idx = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry,
                                                  ALG_SUB_BKT_PTR0f);
            }

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

                if (ipmc_route && (ipmc_index == 0)) {
                    continue;
                }
            }

            /* Extract VRF from LPM entry*/
            if (soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf) < 0) {
                goto free_lpm_table;
            }
            bank_disable =
                ALPM_CTRL(unit).bank_disable_bmp_8b[vrf != (SOC_VRF_MAX(unit) + 1)];

            /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
             * resides in TCAM */
            if (bkt_ptr == 0) {
                VRF_TRIE_ROUTES_INC(unit, vrf_id, vrf, ipv6);
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_th_alpm_lpm_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                            lpm_entry);
                _bcm_th_alpm_lpm_key_parse(unit, &lpm_cfg, lpm_entry);
                lpm_cfg.defip_index = idx;

                if (!!ipv6 == !!(lpm_cfg.defip_flags & BCM_L3_IP6)) {
                    if (trv_data->op_cb) {
                        (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                                  (void *)&lpm_cfg,
                                                  (void *)&nh_ecmp_idx,
                                                  &cmp_result);
                    }
                }
                continue;
            }

            pivot_idx = (idx << 1) + tmp_idx;

            /* TCAM pivot recovery */
            if (soc_th_alpm_warmboot_pivot_add(unit, ipv6, lpm_entry, 
                    pivot_idx, ALPM_LOG_BKT(bkt_ptr, sub_bkt_idx)) < 0) {
                goto free_lpm_table;
            }

            /* Set bucket bitmap */ 
            if (soc_th_alpm_warmboot_bucket_bitmap_set(unit, vrf, ipv6, bkt_ptr) < 0) {
                goto free_lpm_table;
            }

            flex = _soc_th_alpm_lpm_flex_get(unit, ipv6,
                        soc_mem_field32_get(unit, L3_DEFIPm,
                        lpm_entry, ENTRY_VIEW0f));
            if (ipv6) {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    entry_count = 3;
                    alpm_entry = &alpm_entry_v6_64;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v6_64;
                }

            } else {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v4;
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV4m;
                    entry_count = 6;
                    alpm_entry = &alpm_entry_v4;
                }
            }
            bkt_count = _soc_th_alpm_bkt_entry_cnt(unit, alpm_mem);

            ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* Get the bucket pointer from lpm entry */
            bkt_ptr2 = bkt_ptr;
            entry_num = 0;
            for (bkti = 0; bkti < bkt_count; bkti++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + alpm_bkt_bits)) | 
                           (bkt_ptr2 << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));

                entry_num++;
                if (entry_num == entry_count) {
                    entry_num = 0;
                    ALPM_NEXT_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr2 ++;
                    }
                }

                /* Read entry from bucket memory */
                if (soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                          bkt_addr, alpm_entry) < 0) {
                    goto free_lpm_table;
                }

                bkt_occupied = 1;

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                if (ALPM_CTRL(unit).bkt_sharing && sub_bkt_idx !=
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
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

                if (soc_th_alpm_warmboot_prefix_insert(unit, ipv6, flex, lpm_entry,
                                                       alpm_entry, pivot_idx, 
                                                       ALPM_LOG_BKT(bkt_ptr, sub_bkt_idx),
                                                       bkt_addr) < 0) {
                    continue;
                }

            } /* End of bucket walk loop*/
            if (bkt_occupied) {
                /* If double wide,  two alpm bkt views should be set */
                if (SOC_TH_ALPM_SCALE_CHECK(unit, ipv6)) {
                    soc_alpm_cmn_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                    soc_alpm_cmn_bkt_view_set(unit, (bkt_ptr_tmp + 1) << 2, alpm_mem);
                } else {
                    soc_alpm_cmn_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                }
            }
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */


    /* Update bpm_len of pivot after perfix trie is restored */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                         defip_entry_t *, lpm_tbl_ptr, idx);

        ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0_f(unit));

        /* Calculate LPM table traverse step count */
        step_count = ipv6 ? 1 : 2;

        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {
                /* Copy upper half of lpm entry to lower half */
                (void) soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry,
                                                      lpm_entry, TRUE);
                /* Invalidate upper half */
                soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 0);
            }

            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

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

                if (ipmc_route && (ipmc_index == 0)) {
                    continue;
                }
            }

            /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
             * resides in TCAM */
            if(soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry,
                                   ALG_BKT_PTR0f) == 0) {
               continue;
            }

            pivot_idx = (idx << 1) + tmp_idx;

            /* TCAM pivot recovery */
            if (soc_th_alpm_warmboot_pivot_bmp_len_update(
                    unit, ipv6, lpm_entry, pivot_idx) < 0) {
                goto free_lpm_table;
            }
        }
    }

    if (soc_th_alpm_warmboot_lpm_reinit_done(unit) < 0) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;

free_lpm_table:
    if (lpm_tbl_ptr) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_th_alpm_128_warmboot_walk
 * Purpose:
 *      Recover IPv6-128Bit LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_alpm_128_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6 = 2;               /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf, vrf_id;            /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkti = 0;               /*                              */
    int bkt_ptr2, bkt_ptr = 0;  /* Bucket pointer               */
    int sub_bkt_idx = 0;        /* Sub bucket pointer           */
    int bank_num = 0;           /* Number of active SRAM banks  */
    int entry_num = 0;          /* ALPM entry number in bucket  */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int entry_count;            /* ALPM entry count in bucket   */
    int bank_count;             /* SRAM bank count              */
    int step_count = 1;         /*                              */
    int cmp_result;             /* Test routine result.         */
    int rv = BCM_E_FAIL;        /* Operation return status.     */
    int defip_table_size = 0;   /* Defip table size.            */
    char *lpm_tbl_ptr = NULL;   /* DMA table pointer.           */
    void *alpm_entry = NULL;    /*                              */
    uint32 rval;                /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int bank_bits;              /* 2 bank mode, or 4 bank mode  */
    uint32 bank_disable;        /* Bank disable mask            */
    int alpm_bkt_bits = ALPM_CTRL(unit).bkt_bits;

    defip_pair_128_entry_t *lpm_entry; /* Hw entry buffer.      */
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128; /* ALPM 128  */
    int ipmc_route = 0;         /* Route is ip multicast type   */
    int bkt_ptr_tmp = 0;
    uint8 bkt_occupied = 0;

    /* DMA LPM table to software copy */
    rv = bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m,
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                    "lpm_128_warmboot_tbl",
                    &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto free_lpm_table;
    }

    if(BCM_FAILURE(READ_L3_DEFIP_RPF_CONTROLr(unit, &rval))) {
        goto free_lpm_table;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    alpm_entry  = &alpm_entry_v6_128;
    entry_count = 2;
    bank_count  = ALPM_CTRL(unit).num_banks;
    bank_bits   = ALPM_CTRL(unit).bank_bits;
    bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, alpm_mem);
    
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
        if (soc_th_alpm_128_warmboot_lpm_reinit(unit, ipv6, idx, lpm_entry) < 0) {
            goto free_lpm_table;
        }

        bkt_occupied = 0;

        /*  If IPv6 double wide mode, walk next bucket.*/
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            /* Extract bucket pointer from LPM entry */
            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                          ALG_BKT_PTRf);
            bkt_ptr_tmp = bkt_ptr;
            if (ALPM_CTRL(unit).bkt_sharing) {
                sub_bkt_idx = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                                  lpm_entry, ALG_SUB_BKT_PTRf);
            } else {
                sub_bkt_idx = 0;
            }

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

                if (ipmc_route && ipmc_index == 0) {
                    continue;
                }
            }

            /* Extract VRF from LPM entry*/
            if (soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf) < 0) {
                goto free_lpm_table;
            }

            /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
             * resides in TCAM */
            if (bkt_ptr == 0) {
                VRF_TRIE_ROUTES_INC(unit, vrf_id, vrf, ipv6);

                /* TCAM Routes Processing */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                (void) _bcm_th_alpm_lpm128_key_parse(unit, (uint32 *)lpm_entry,
                                                     &lpm_cfg);
                (void) soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry,
                                            &lpm_cfg.defip_vrf, &nh_ecmp_idx);
                _bcm_th_alpm_lpm128_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                               lpm_entry);
                lpm_cfg.defip_index = idx;
                if (trv_data->op_cb) {
                    (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                              (void *)&lpm_cfg,
                                              (void *)&nh_ecmp_idx, &cmp_result);
                }
                continue;
            }

            bank_disable =
                ALPM_CTRL(unit).bank_disable_bmp_8b[vrf != (SOC_VRF_MAX(unit) + 1)];

            /* TCAM pivot recovery */
            if (soc_th_alpm_128_warmboot_pivot_add(unit, ipv6, lpm_entry, 
                    idx, ALPM_LOG_BKT(bkt_ptr, sub_bkt_idx)) < 0) {
                goto free_lpm_table;
            }

            /* Set bucket bitmap */ 
            if (soc_th_alpm_128_warmboot_bucket_bitmap_set(unit, vrf, ipv6, 
                                                           bkt_ptr) < 0) {
                goto free_lpm_table;
            }


            ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);

            entry_num = 0;
            bkt_ptr2 = bkt_ptr;
            /* Get the bucket pointer from lpm entry */
            for (bkti = 0; bkti < bkt_count; bkti++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + alpm_bkt_bits)) | 
                           (bkt_ptr2 << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    ALPM_NEXT_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr2 ++;
                    }                    
                }

                /* Read entry from bucket memory */
                if (soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                          bkt_addr, alpm_entry) < 0) {
                    goto free_lpm_table;
                }

                /*
                 * In Tomahawk URPF mode, valid bits will make sure
                 * SIP entries in buckets are included in this walk
                 */

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }
                if (ALPM_CTRL(unit).bkt_sharing && sub_bkt_idx !=
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                bkt_occupied = 1;

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));


                /* Fill entry IP address and subnet mask. */
                _bcm_th_alpm_ent128_key_parse(unit, alpm_mem, alpm_entry,
                                              &lpm_cfg);

                (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf, &vrf);
                /* get associated data */
                _bcm_th_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry,
                                               &lpm_cfg, &nh_ecmp_idx);

                /* Execute operation routine if any. */
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb)(unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        continue;
                    }
#endif
                }

                if (soc_th_alpm_128_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                       alpm_entry, idx, ALPM_LOG_BKT(bkt_ptr, sub_bkt_idx),
                       bkt_addr) < 0) {
                    continue;
                }

            } /* End of bucket walk loop*/
            if (bkt_occupied) {
                /* If double wide,  two alpm bkt views should be set */
                if (SOC_TH_ALPM_SCALE_CHECK(unit, ipv6)) {
                    soc_alpm_cmn_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                    soc_alpm_cmn_bkt_view_set(unit, (bkt_ptr_tmp + 1) << 2, alpm_mem);
                } else {
                    soc_alpm_cmn_bkt_view_set(unit, bkt_ptr_tmp << 2, alpm_mem);
                }
            }
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    /* Update bpm_len of pivot after perfix trie is restored */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                                 defip_pair_128_entry_t *,
                                                 lpm_tbl_ptr, idx);

        /* Verify if read LPM entry is IPv6-128 entry */
        if (0x03 != soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                        lpm_entry, MODE1_UPR_f(unit))) {
            continue;
        }

        /* Verify if LPM entry is valid */
        if (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                 lpm_entry, VALID1_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                 lpm_entry, VALID0_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                 lpm_entry, VALID1_UPRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m,
                                 lpm_entry, VALID0_UPRf)) {
            continue;
        }

        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
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

                if ((ipmc_route) && ipmc_index == 0) {
                    continue;
                }
            }

            /* VRF_OVERRIDE (Global High) entries, and IP multicast prefix
             * resides in TCAM */
            if (soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    ALG_BKT_PTRf) == 0) {
                continue;
            }

            /* TCAM pivot recovery */
            if (soc_th_alpm_128_warmboot_pivot_bmp_len_update(
                    unit, ipv6, lpm_entry, idx) < 0) {
                goto free_lpm_table;
            }
        }
    }

    if (soc_th_alpm_128_warmboot_lpm_reinit_done(unit) < 0) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;
free_lpm_table:
    if (lpm_tbl_ptr) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }

    return (rv);
}

STATIC int
_bcm_th_alpm_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6;                /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* ipv4 entries iterator.       */
    char *lpm_tbl_ptr;          /* Dma table pointer.           */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_pair_128_entry_t *lpm_entry = NULL;   /* Hw entry buffer.    */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count, bank_bits;
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128;
    void *alpm_entry;
    soc_mem_t alpm_mem, pivot_mem = L3_DEFIP_PAIR_128m;
    int step_count = 1;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    typedef struct _alpm_def_route_info_s {
        int idx;
        int bkt_addr;
    } _alpm_def_route_info_t;
    _alpm_def_route_info_t *def_rte_arr = NULL;
    int sub_bkt = 0;
    uint32 bank_disable;
    int alpm_bkt_bits = ALPM_CTRL(unit).bkt_bits;

    ipv6 = (trv_data->flags & BCM_L3_IP6);
    if (!ipv6) {
        return SOC_E_NONE;
    }

    /* LPM table DMA to software copy if callback is not for 'delete all' */
    rv = bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m,
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                             "lpm_128_tbl", &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = SOC_VRF_MAX(unit) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    idx_start   = 0;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = ALPM_CTRL(unit).num_banks;
    entry_count = 2;
    alpm_entry  = &alpm_entry_v6_128;
    bank_bits   = ALPM_CTRL(unit).bank_bits;
    bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, alpm_mem);

    /* if v6 loop twice once with with defip and once with defip_pair */
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
            /* Skip reserved ipmc entry */
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

            if (ipmc_route && ipmc_index == 0) {
                continue;
            }
        }
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            bkt_ptr = soc_mem_field32_get(unit, pivot_mem, lpm_entry, ALG_BKT_PTRf);
            if (ALPM_CTRL(unit).bkt_sharing) {
                sub_bkt = soc_mem_field32_get(unit, pivot_mem, lpm_entry, ALG_SUB_BKT_PTRf);
            }
            (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                              &cmp_result);
            bank_disable =
                ALPM_CTRL(unit).bank_disable_bmp_8b[cmp_result != (SOC_VRF_MAX(unit) + 1)];

            if (bkt_ptr == 0) {

                /* OVERRIDE VRF Processing */ 

                (void) _bcm_th_alpm_lpm128_key_parse(unit, (uint32 *) lpm_entry,
                                                     &lpm_cfg);
                /* defip_vrf adjust */
                (void) soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry,
                                          &lpm_cfg.defip_vrf, &nh_ecmp_idx);
                _bcm_th_alpm_lpm128_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                               lpm_entry);

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

                continue;
            }

            entry_num = 0;
            ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* get the bucket pointer from lpm mem  entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + alpm_bkt_bits)) | 
                           (bkt_ptr << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    ALPM_NEXT_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr ++;
                    }                    
                }

                /* read entry from bucket memory */
                rv = soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                           bkt_addr, alpm_entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }

                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                if (ALPM_CTRL(unit).bkt_sharing && sub_bkt !=
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse  the entry. */
                /* Fill entry ip address &  subnet mask. */
                _bcm_th_alpm_ent128_key_parse(unit, alpm_mem, alpm_entry,
                                              &lpm_cfg);

                (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                                  &cmp_result);
                
                /* get associated data */
                _bcm_th_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry,
                                               &lpm_cfg, &nh_ecmp_idx);
                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
                    continue;
                }

                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx < SOC_VRF_MAX(unit)) {
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

        /* Calculate entry offset. */
        lpm_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                    defip_pair_128_entry_t *, lpm_tbl_ptr,
                                    def_rte_arr[idx].idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }

        bkt_addr = def_rte_arr[idx].bkt_addr;
        
        /* read entry from bucket memory */
        rv = soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                   bkt_addr, alpm_entry);
        if (SOC_FAILURE(rv)) {
            break;
        }

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        /* Fill entry ip address & subnet mask. */
        _bcm_th_alpm_ent128_key_parse(unit, alpm_mem, alpm_entry, &lpm_cfg);
        /* Parse  the entry. */
        (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                          &cmp_result);
        _bcm_th_alpm_ent128_data_parse(unit, alpm_mem, alpm_entry, &lpm_cfg,
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

cleanup:
    if (def_rte_arr) {
        sal_free(def_rte_arr);
    }
    if (lpm_tbl_ptr) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }
    
    return (rv);
}

/*
 * Function:
 *      _bcm_th_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6 = 0;            /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* ipv4 entries iterator.       */
    int tmp_start;              /* Iteration start.             */
    int tmp_end;                /* Iteration end.               */
    int tmp_delta;              /* Iteration delta.             */
    char *lpm_tbl_ptr;          /* Dma table pointer.           */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_entry_t *lpm_entry;   /* Hw entry buffer.             */
    defip_entry_t lpm_entry_data; /* Hw entry buffer.           */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count;
    defip_alpm_ipv4_1_entry_t alpm_entry_v4;
    defip_alpm_ipv6_64_1_entry_t alpm_entry_v6_64;
    void *alpm_entry;
    soc_mem_t alpm_mem;
    int step_count;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    int bank_bits;
    int sub_bkt = 0;
    uint32 bank_disable;
    int alpm_bkt_bits = ALPM_CTRL(unit).bkt_bits;
    typedef struct _alpm_def_route_info_s {
        int idx;
        int bkt_addr;
        defip_entry_t lpm_entry;
    } _alpm_def_route_info_t;

    _alpm_def_route_info_t *def_rte_arr = NULL;
    int flex;
    int size = 0;


#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        if (soc_mem_index_count(unit, BCM_XGS3_L3_MEM(unit, defip)) != 0) {
            rv = _bcm_th_alpm_warmboot_walk(unit, trv_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META_U(unit,
                          "ERROR!  ALPM Warmboot recovery failed")));
                return (rv);
            }
        }

        if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
            /* v6-128 entries */
            rv = _bcm_th_alpm_128_warmboot_walk(unit, trv_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                         (BSL_META_U(unit,
                         "ERROR!  ALPM Warmboot V6-128 recovery failed")));
            }
        }
        return (rv);
    }
#endif

    ipv6 = trv_data->flags & BCM_L3_IP6;

    if (ipv6 && soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
        /* first get v6-128 entries */
        rv = _bcm_th_alpm_128_update_match(unit, trv_data);
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
    rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = SOC_VRF_MAX(unit) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = ALPM_CTRL(unit).num_banks;
    bank_bits   = ALPM_CTRL(unit).bank_bits;

    step_count = ipv6 ? 1 : 2;

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
                    /* Check second part of the entry. */
                    soc_th_alpm_lpm_ip4entry1_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);
                } else {
                    soc_th_alpm_lpm_ip4entry0_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);            
                }
            }

            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            if (soc_feature(unit, soc_feature_ipmc_defip)) {
                /* Skip reserved ipmc entry */
                int ipmc_index = 0;
                int ipmc_route = 0;
                if (soc_mem_field_valid(unit, L3_DEFIPm, MULTICAST_ROUTE0f)) {
                    ipmc_route = soc_mem_field32_get(unit, L3_DEFIPm,
                                                     lpm_entry, MULTICAST_ROUTE0f);
                } else if (soc_mem_field_valid(unit, L3_DEFIPm, DATA_TYPE0f)) {
                    if (soc_mem_field32_get(unit, L3_DEFIPm,
                                            lpm_entry, DATA_TYPE0f) == 2) {
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

            (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                          &cmp_result);
            bank_disable =
                ALPM_CTRL(unit).bank_disable_bmp_8b[cmp_result != (SOC_VRF_MAX(unit) + 1)];

            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry,
                                      ALG_BKT_PTR0f);
            if (ALPM_CTRL(unit).bkt_sharing) {
                sub_bkt = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry,
                                              ALG_SUB_BKT_PTR0f);
            }

            if (bkt_ptr == 0) {
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_th_alpm_lpm_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                            lpm_entry);
                _bcm_th_alpm_lpm_key_parse(unit, &lpm_cfg, lpm_entry);

                lpm_cfg.defip_index = idx;

                if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb)(unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
                continue;
            }

            flex = _soc_th_alpm_lpm_flex_get(unit, ipv6 ? L3_DEFIP_MODE_64 : 0,
                        soc_mem_field32_get(unit, L3_DEFIPm,
                        lpm_entry, ENTRY_VIEW0f));
            if (ipv6) {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                    entry_count = 3;
                    alpm_entry = &alpm_entry_v6_64;
                    size = sizeof(alpm_entry_v6_64);
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v6_64;
                    size = sizeof(alpm_entry_v6_64);
                }

            } else {
                if (flex) {
                    alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                    entry_count = 4;
                    alpm_entry = &alpm_entry_v4;
                    size = sizeof(alpm_entry_v4);
                } else {
                    alpm_mem = L3_DEFIP_ALPM_IPV4m;
                    entry_count = 6;
                    alpm_entry = &alpm_entry_v4;
                    size = sizeof(alpm_entry_v4);
                }
            }

            entry_num = 0;
            bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, alpm_mem);
            ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* Get the bucket pointer from lpm mem entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + alpm_bkt_bits)) |
                           (bkt_ptr << bank_bits) |
                           (bank_num & ((1U << bank_bits) - 1));
                entry_num++;
                if (entry_num == entry_count) {
                    entry_num = 0;
                    ALPM_NEXT_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        ALPM_FIRST_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr ++;
                    }
                }

                sal_memset(alpm_entry, 0x00, size);
                /* Read entry from bucket memory */
                rv = soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                           bkt_addr, alpm_entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }

                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                if (ALPM_CTRL(unit).bkt_sharing && sub_bkt !=
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                            alpm_mem, alpm_entry);

                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
                    continue;
                }
                 
                /* Fill entry IP address and subnet mask. */
                _bcm_th_alpm_ent_key_parse(unit, &lpm_cfg, lpm_entry,
                                           alpm_mem, alpm_entry);

                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx < SOC_VRF_MAX(unit)) {
                        def_rte_arr[def_arr_idx].bkt_addr = bkt_addr;
                        def_rte_arr[def_arr_idx].idx = idx;
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
        bkt_addr = def_rte_arr[idx].bkt_addr;

        flex = _soc_th_alpm_lpm_flex_get(unit, ipv6 ? L3_DEFIP_MODE_64 : 0,
                    soc_mem_field32_get(unit, L3_DEFIPm,
                    lpm_entry, ENTRY_VIEW0f));
        if (ipv6) {
            if (flex) {
                alpm_mem = L3_DEFIP_ALPM_IPV6_64_1m;
                alpm_entry = &alpm_entry_v6_64;
                size = sizeof(alpm_entry_v6_64);
            } else {
                alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
                alpm_entry = &alpm_entry_v6_64;
                size = sizeof(alpm_entry_v6_64);
            }

        } else {
            if (flex) {
                alpm_mem = L3_DEFIP_ALPM_IPV4_1m;
                alpm_entry = &alpm_entry_v4;
                size = sizeof(alpm_entry_v4);
            } else {
                alpm_mem = L3_DEFIP_ALPM_IPV4m;
                alpm_entry = &alpm_entry_v4;
                size = sizeof(alpm_entry_v4);
            }
        }
        sal_memset(alpm_entry, 0x00, size);
        /* read entry from bucket memory */
        rv = soc_mem_read_no_cache(unit, 0, alpm_mem, 0, MEM_BLOCK_ANY,
                                   bkt_addr, alpm_entry);
        if (SOC_FAILURE(rv)) {
            break;
        }

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                      &cmp_result);
        /* Parse  the entry. */
        _bcm_th_alpm_ent_data_parse(unit, &lpm_cfg, &nh_ecmp_idx,
                                    alpm_mem, alpm_entry);

        /* If protocol doesn't match skip the entry. */
        if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
            continue;
        }

        /* Fill entry ip address & subnet mask. */
        _bcm_th_alpm_ent_key_parse(unit, &lpm_cfg, lpm_entry,
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

cleanup:
    if (def_rte_arr != NULL) {
        sal_free(def_rte_arr);
    }
    if (lpm_tbl_ptr != NULL) {
        soc_cm_sfree(unit, lpm_tbl_ptr);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_th_alpm_find
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
_bcm_th_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
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
        BCM_IF_ERROR_RETURN(_bcm_th_alpm_lpm128_init(unit, lpm_cfg,
                                            &lpm_128_key, 0, &flags));
        /* Perform hw lookup. */
        rv = soc_th_alpm_128_find_best_match(unit, &lpm_128_key, &lpm_128_entry, 
                                             &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);

        /* Parse hw buffer to defip entry. */
        _bcm_th_alpm_lpm128_data_parse(unit, lpm_cfg, nh_ecmp_idx,
                                       &lpm_128_entry);
        /* OVERRIDE VRF Processing */
        _bcm_th_alpm_lpm128_key_parse(unit, (uint32 *)&lpm_128_entry, lpm_cfg);
        soc_th_alpm_128_lpm_vrf_get(unit, &lpm_128_entry, &lpm_cfg->defip_vrf, &tmp);

        break;
    default:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_th_alpm_lpm_init(unit, lpm_cfg, &lpm_key, 0,
                                                  &flags));
        /* Perform hw lookup. */
        rv = soc_th_alpm_find_best_match(unit, &lpm_key, &lpm_entry, 
                                      &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);
    
        /* Parse hw buffer to defip entry. */
        _bcm_th_alpm_lpm_data_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_entry);
        _bcm_th_alpm_lpm_key_parse(unit, lpm_cfg, &lpm_entry);

        break;
    }
    
    return (BCM_E_NONE);
}

#endif /* defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) */
#endif /* INCLUDE_L3 && ALPM_ENABLE */

