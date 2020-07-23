/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hash table calculation routines
 */

#include <assert.h>

#include <sal/types.h>
#include <sal/core/thread.h>
#include <shared/bsl.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#endif
#ifdef BCM_GREYHOUND_SUPPORT
#include <soc/greyhound.h>
#endif
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
#include <soc/hurricane3.h>
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif
#ifdef BCM_APACHE_SUPPORT
#include <soc/apache.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_XGS_SWITCH_SUPPORT
uint32
soc_crc32b(uint8 *data, int data_nbits)
{
    uint32 rv;
    rv = _shr_crc32b(0, data, data_nbits);
    rv = _shr_bit_rev_by_byte_word32(rv);
    return rv;
}

uint16
soc_crc16b(uint8 *data, int data_nbits)
{
    uint16 rv;
    rv = _shr_crc16b(0, data, data_nbits);
    rv = _shr_bit_rev16(rv);
    return rv;
}

int 
soc_dual_hash_recurse_depth_get(int unit, soc_mem_t mem)
{
    switch(mem) {
    case L2Xm: return SOC_DUAL_HASH_MOVE_MAX_L2X(unit) ? 
        SOC_DUAL_HASH_MOVE_MAX_L2X(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
#if defined(BCM_TRX_SUPPORT)
    case MPLS_ENTRYm: return SOC_DUAL_HASH_MOVE_MAX_MPLS(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_MPLS(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case EGR_VLAN_XLATEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
         return SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit) ?
                SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit) :
                SOC_DUAL_HASH_MOVE_MAX(unit);
    case VLAN_XLATEm:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
         return SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) ?
                SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) :
                SOC_DUAL_HASH_MOVE_MAX(unit);
#endif
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRX_SUPPORT)
    case VLAN_MACm: return SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
#endif
#if defined(INCLUDE_L3)
    case L3_DEFIPm:
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm: return SOC_DUAL_HASH_MOVE_MAX_L3X(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_L3X(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    case AXP_WRX_WCDm: return SOC_DUAL_HASH_MOVE_MAX_WLAN_CLIENT(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_WLAN_CLIENT(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case AXP_WRX_SVP_ASSIGNMENTm: return SOC_DUAL_HASH_MOVE_MAX_WLAN_PORT(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_WLAN_PORT(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case FT_SESSIONm: return SOC_DUAL_HASH_MAX_FT_SESSION_IPV4(unit) ?
         SOC_DUAL_HASH_MAX_FT_SESSION_IPV4(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case FT_SESSION_IPV6m: return SOC_DUAL_HASH_MAX_FT_SESSION_IPV6(unit) ?
        SOC_DUAL_HASH_MAX_FT_SESSION_IPV6(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    case ING_VP_VLAN_MEMBERSHIPm: return SOC_DUAL_HASH_MOVE_MAX_ING_VP_VLAN_MEMBER(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_ING_VP_VLAN_MEMBER(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case EGR_VP_VLAN_MEMBERSHIPm: return SOC_DUAL_HASH_MOVE_MAX_EGR_VP_VLAN_MEMBER(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_EGR_VP_VLAN_MEMBER(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case ING_DNAT_ADDRESS_TYPEm: return SOC_DUAL_HASH_MOVE_MAX_ING_DNAT_ADDRESS_TYPE(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_ING_DNAT_ADDRESS_TYPE(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case L2_ENDPOINT_IDm: return SOC_DUAL_HASH_MOVE_MAX_L2_ENDPOINT_ID(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_L2_ENDPOINT_ID(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
    case ENDPOINT_QUEUE_MAPm: return SOC_DUAL_HASH_MOVE_MAX_ENDPOINT_QUEUE_MAP(unit) ?
         SOC_DUAL_HASH_MOVE_MAX_ENDPOINT_QUEUE_MAP(unit) : SOC_DUAL_HASH_MOVE_MAX(unit);
#endif /* BCM_TRIDENT2_SUPPORT */
    default: return SOC_DUAL_HASH_MOVE_MAX(unit);
    }
}

int 
soc_multi_hash_recurse_depth_get(int unit, soc_mem_t mem)
{
    switch(mem) {
    case L2Xm:
    case L2_ENTRY_1m:
    case L2_ENTRY_2m:
        return SOC_MULTI_HASH_MOVE_MAX_L2(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_L2(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#if defined(INCLUDE_L3)
    case L3_ENTRY_1m:
    case L3_ENTRY_2m:
    case L3_ENTRY_4m: 
    case L3_DEFIPm:
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif
        return SOC_MULTI_HASH_MOVE_MAX_L3(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_L3(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#if defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        return SOC_MULTI_HASH_MOVE_MAX_L3_TUNNEL(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_L3_TUNNEL(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* INCLUDE_L3 */
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m: return SOC_MULTI_HASH_MOVE_MAX_FPEM(unit) ?
         SOC_MULTI_HASH_MOVE_MAX_FPEM(unit) : SOC_MULTI_HASH_MOVE_MAX(unit);
    case MPLS_ENTRYm:
    case MPLS_ENTRY_1m:
    case MPLS_ENTRY_EXTDm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case MPLS_ENTRY_SINGLEm:
#endif
        return SOC_MULTI_HASH_MOVE_MAX_MPLS(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_MPLS(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
    case VLAN_XLATEm:
    case VLAN_XLATE_1m:
    case VLAN_XLATE_EXTDm:
        return SOC_MULTI_HASH_MOVE_MAX_VLAN(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_VLAN(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#ifdef BCM_TRIDENT3_SUPPORT
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        return SOC_MULTI_HASH_MOVE_MAX_VLAN_1(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_VLAN_1(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        return SOC_MULTI_HASH_MOVE_MAX_VLAN_2(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_VLAN_2(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#endif
    case EP_VLAN_XLATE_1m:
    case EGR_VLAN_XLATEm:
        return SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#ifdef BCM_TRIDENT3_SUPPORT
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        return SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_1(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_1(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        return SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_2(unit) ?
                SOC_MULTI_HASH_MOVE_MAX_EGRESS_VLAN_2(unit) :
                SOC_MULTI_HASH_MOVE_MAX(unit);
#endif
    default:
        return SOC_MULTI_HASH_MOVE_MAX(unit);
    }
}

/*
 * Implement the crc32 routines so that the bit ordering matches Draco
 */

uint32
soc_draco_crc32(uint8 *data, int data_size)
{
    uint32 rv;
    rv = _shr_crc32(0, data, data_size);
    rv = _shr_bit_rev_by_byte_word32(rv);
    return rv;
}

uint16
soc_draco_crc16(uint8 *data, int data_size)
{
    uint16 rv;
    rv = _shr_crc16(0, data, data_size);
    rv = _shr_bit_rev16(rv);
    return rv;
}

/*
 * And now some routines to deal with details
 */

void
soc_draco_l2x_base_entry_to_key(int unit, l2x_entry_t *entry, uint8 *key)
{
    sal_mac_addr_t mac;
    int vid;

    soc_L2Xm_mac_addr_get(unit, entry, MAC_ADDRf, mac);

    vid = soc_L2Xm_field32_get(unit, entry, VLAN_IDf);

    soc_draco_l2x_param_to_key(mac, vid, key);
}

void
soc_draco_l2x_param_to_key(sal_mac_addr_t mac, int vid, uint8 *key)
{
    int ix;

    key[0] = 0;
    for (ix = 0; ix < 6; ix++) {
        key[ix + 1] = (mac[5 - ix] >> 4) & 0x0f;
        key[ix + 0] |= (mac[5 - ix] << 4) & 0xf0;
    }

    key[6] |= (vid << 4) & 0xf0;
    key[7] = (vid >> 4) & 0xff;
}

void
soc_draco_l3x_param_to_key(ip_addr_t ip, ip_addr_t src_ip,
                           uint16 vid, uint8 *key)
{
    int ix;

    for (ix = 0; ix < 4; ix++) {
        key[ix] = (ip >> (8*ix)) & 0xff;
        key[ix + 4] = (src_ip >> (8*ix)) & 0xff;
    }

    key[8] = vid & 0xff;
    key[9] = (vid >> 8) & 0x0f;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * Get the XGS3 ECMP hash result based on HW
 */
uint32 
soc_xgs3_l3_ecmp_hash(int unit, soc_xgs3_ecmp_hash_t *data)
{
    uint8    key[SOC_MAX_MEM_BYTES];
    uint8    use_l4_port = 0;
    uint8    use_dip;
    uint8    hash_sel;
    uint8    crc_val_shift;
    uint32   crc_val;
    uint32   regval;
    int      index;
    int      idx; 
    uint8    udf;
    uint16   mask;

    if (NULL == data) {
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &regval));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                 regval, ECMP_HASH_SELf);

    /* If hash is disabled return 0. */
    if (FB_HASH_ZERO == hash_sel) {
        return 0;
    }

    /* If hash is based on LSB of sip. */
    if (FB_HASH_LSB == hash_sel) {
        if(data->v6) {
            crc_val = data->sip6[0] & 0x1f;
        }  else {
            crc_val = data->sip & 0x1f;
        }
        return ((crc_val & 0x1F) % (data->ecmp_count + 1));
    }

    use_dip = soc_reg_field_get(unit, HASH_CONTROLr,
                                 regval, ECMP_HASH_USE_DIPf);

    udf = soc_reg_field_get(unit, HASH_CONTROLr,
                            regval, ECMP_HASH_UDFf);

    if (SOC_REG_FIELD_VALID(unit, HASH_CONTROLr, USE_TCP_UDP_PORTSf)) {
        use_l4_port = soc_reg_field_get(unit, HASH_CONTROLr,
                                        regval, USE_TCP_UDP_PORTSf);
    } 

    /* Initialize key structure. */
    sal_memset(key, 0, SOC_MAX_MEM_BYTES * sizeof (uint8));

    /*KEY FORMAT IS UDF[12] DST_PORT[10-11] SRC_PORT[8-9] DIP[4-7] SIP[0-3]*/
    if (data->v6) {
        /* IP[32] = IP[0-31] ^ IP[32-63] ^ IP[64-95] ^ IP[96-127] */
        for (idx = 0; idx < 4; idx++) {
            for (index = 3 - idx; index < 16; index += 4) {
                key[idx] ^= data->sip6[index]  & 0xff;
                if (use_dip) {
                    key[idx + 4] ^= data->dip6[index] & 0xff;
                }
            }
        }
    } else {
        for (idx = 0; idx < 4; idx++) {
            key[idx] = (data->sip >> (8*idx)) & 0xff;
            if (use_dip) {
                key[idx + 4] = (data->dip >> (8*idx)) & 0xff;
            }
        }
    }

    if (use_l4_port) {
        for (idx = 0; idx < 2; idx++) {
            key[idx + 8] = (data->l4_src_port >> (8*idx)) & 0xff;
            key[idx + 10] = (data->l4_dst_port >> (8*idx)) & 0xff;
        }
    }

    key[12] = udf & 0xff;
   
    /* XGS3 always uses CRC32 for ECMP hash calculation */
    crc_val = soc_draco_crc32(key, 13);

    crc_val_shift = 27;
    mask = 0x1F;
    if (SOC_IS_APOLLO(unit) || SOC_IS_TRIUMPH2(unit)) {
        crc_val_shift = 24;
        mask = 0xFF;
    } else if (SOC_IS_TRIUMPH3(unit)) {
       crc_val_shift = 22;
       mask = 0x3FF;
    } else if (SOC_IS_TD_TT(unit)) {
        crc_val_shift = 16;
        mask = 0xFFFF;
    }

    if (FB_HASH_CRC32_UPPER == hash_sel) {
        return ((crc_val >> crc_val_shift) % (data->ecmp_count + 1));
    } else if (FB_HASH_CRC32_LOWER == hash_sel) {
        return ((crc_val & mask) % (data->ecmp_count + 1));
    }
    /* Hopefully never reached. */
    return (SOC_E_INTERNAL);
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

uint32
soc_draco_trunk_hash(sal_mac_addr_t da, sal_mac_addr_t sa, int tgsize)
{
    uint8 key[12];
    int ix;
    uint32 crc32;

    if (tgsize <= 0 || tgsize > 8) {
        return 0;
    }

    for (ix = 0; ix < 6; ix++) {
        key[ix + 0] = da[5 - ix];
    }

    for (ix = 0; ix < 6; ix++) {
        key[ix + 6] = sa[5 - ix];
    }

    crc32 = soc_draco_crc32(key, 12);

    return (crc32 & 0xffff) % tgsize;
}

int
soc_draco_hash_set(int unit, int hash_sel)
{
    uint32              hash_control, ohash;

    assert(hash_sel >= 0 && hash_sel <= 5);

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    ohash = hash_control;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      HASH_SELECTf, hash_sel);
    if (hash_control != ohash) {
        SOC_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));
    }

    return SOC_E_NONE;
}

int
soc_draco_hash_get(int unit, int *hash_sel)
{
    uint32              hash_control;

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

    *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                  HASH_SELECTf);

    if (*hash_sel > 5) {
        *hash_sel = 5;
    }

    return SOC_E_NONE;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
#define GEN_KEY2(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff

#define GEN_KEY3(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff;                        \
    k[ks + 2] = (k1 >> 12) & 0xff;                       \
    k[ks + 3] = ((k1 >> 20) & 0x0f)

#define GEN_KEY4(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff;                        \
    k[ks + 2] = (k1 >> 12) & 0xff;                       \
    k[ks + 3] = (k1 >> 20) & 0xff;                       \
    k[ks + 4] = ((k1 >> 28) & 0x0f)
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

uint32
soc_fb_l2_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask, mask_bits;
        int     bits;

        /* Get the effective table max for the hash mask */
        mask = soc_mem_index_max(unit, L2_HITDA_ONLYm);
        /* Need the maximum table size for the shift bits */
        mask_bits = SOC_MEM_INFO(unit, L2_HITDA_ONLYm).index_max;
        bits = 0;
        rv = 1;
        while (rv && (mask_bits & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l2x = mask;
        SOC_CONTROL(unit)->hash_bits_l2x = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        if (SOC_IS_HAWKEYE(unit)) {
            rv >>= 16 - (SOC_CONTROL(unit)->hash_bits_l2x + 1);
        } else {
            rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        }
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        if (SOC_IS_HAWKEYE(unit)) {
            rv >>= 32 - (SOC_CONTROL(unit)->hash_bits_l2x + 1);
        } else {
            rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        }
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_fb_l2_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

uint32
soc_fb_l3_hash(int unit, int hash_sel, int key_nbits, uint8 *key)
{
    uint32 rv;
#ifdef BCM_SABER2_SUPPORT
    uint32 pad_bits = 0, key_size = 0;
#endif
    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l3x == 0) {
        uint32  mask, mask_bits;
        int     bits;

        /* Get the effective table max for the hash mask */
        mask = soc_mem_index_max(unit, L3_ENTRY_HIT_ONLYm);
        /* Need the maximum table size for the shift bits */
        mask_bits = SOC_MEM_INFO(unit, L3_ENTRY_HIT_ONLYm).index_max;
        bits = 0;
        rv = 1;
        while (rv && (mask_bits & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l3x = mask;
        SOC_CONTROL(unit)->hash_bits_l3x = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = ((uint32)key[0]) | ((uint32)key[1] << 8) | ((uint32)key[2] << 16);
        /* See _soc_fb_l3x_realign_key() to understand the shift values below */

        if ((key_nbits == 80)  || (key_nbits == 264)) {
                rv >>= 4;
        } else if ((key_nbits == 40)  || (key_nbits == 136)) {
                rv >>= 2;
        } else if ((key_nbits == 88)  || (key_nbits == 272)) {
#if defined (BCM_SABER2_SUPPORT)
               if (SOC_IS_SABER2(unit)) {
                   key_size = 271;
                   pad_bits = (key_size + 7) & ~0x7;
                   pad_bits = pad_bits - key_size;
                   rv >>= pad_bits; /*take care of padding bits*/
               } else
#endif 
#if defined (BCM_GREYHOUND2_SUPPORT)
               if (SOC_IS_GREYHOUND2(unit)) {
                   rv >>= 4;
               } else
#endif
               {
                   rv >>= 6;
               }
        } else if ((key_nbits == 96)  || (key_nbits == 280)) {
#if defined (BCM_TRIUMPH_SUPPORT)
            if(SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, L3_IIFf)) {
                rv >>= (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) || 
                        SOC_IS_HURRICANEX(unit) ||
                        SOC_IS_GREYHOUND(unit) ||
                        SOC_IS_GREYHOUND2(unit)) ? 5 : 6;
            } else 
#endif /* BCM_TRX_SUPPORT */
            {
                rv >>= 7;
            }
        } else if ((key_nbits == 48)  || (key_nbits == 144)) {
#if defined (BCM_ENDURO_SUPPORT)
           if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANEX(unit) ||
             SOC_IS_GREYHOUND(unit)) {
               rv >>= 2;
           } else
#endif /* BCM_ENDURO_SUPPORT */
           {
                rv >>= 3;
            }
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_fb_l3_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l3x;
}

uint32
soc_fb_vlan_mac_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 4 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 2;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
        SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 6);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 6);
        break;

    case FB_HASH_LSB:
        /* Extract more than required 8 bits. Masked below anyway */
        rv = (((uint32)key[0]) | ((uint32)key[1] << 8));
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 6);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 6);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_fb_vlan_mac_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

/* In Hurricane, VRF_ID field is invalid, but the hashing algorithm retains the space
*   in the key, and expects it to be 0s. So we have special handling for Hu
*/
STATIC int
_soc_hash_generic_entry_to_key(int unit, void *entry, uint8 *key,
                               soc_mem_t mem, soc_field_t *field_list)
{
    soc_field_t field;
    int         index, key_index, val_index, fval_index;
    int         right_shift_count, left_shift_count;
    uint32      val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int         bits, val_bits, fval_bits;
    int8        field_length[16];

    val_bits = 0;
    for (index = 0; index < 16; index++) {
        field_length[index] = 0;
    }
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        if (field == NUM_SOC_FIELD || !SOC_MEM_FIELD_VALID(unit, mem, field)) {
#ifdef BCM_HURRICANE_SUPPORT 
            if((SOC_IS_HURRICANEX(unit)|| SOC_IS_GREYHOUND(unit) ||
                SOC_IS_GREYHOUND2(unit))
                 && (field == VRF_IDf)) {
                field_length[index] = 11;
                val_bits += field_length[index];
            }
#endif
            continue;
        }
        field_length[index] = soc_mem_field_length(unit, mem, field);
#ifdef BCM_HURRICANE_SUPPORT
        if((SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) && 
            (field == PORT_GROUP_IDf)) {
            field_length[index] = 13;
        }
#endif

#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            if ((field == VRF_IDf) || (field == L3_IIFf)) {
                field_length[index] = 12;
            }
        }
#endif /* BCM_FIRELIGHT_SUPPORT */

        val_bits += field_length[index];
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_TD_TT(unit)||SOC_IS_KATANA(unit)||
        SOC_IS_GREYHOUND(unit)||SOC_IS_GREYHOUND2(unit)) {
        if (mem == L2Xm || mem == VLAN_MACm) {
            if (soc_feature(unit, soc_feature_trill)) {
                val_bits = 60 + /* VLAN_ID + MAC_ADDRESS */
                    soc_mem_field_length(unit, L2Xm, KEY_TYPEf) +
                    soc_mem_field_length(unit, L2Xm,
                                         TRILL_NONUC_NETWORK_LONG__TREE_IDf);
            } else {
                val_bits = 60 + /* VLAN_ID + MAC_ADDRESS */
                    soc_mem_field_length(unit, L2Xm, KEY_TYPEf);
            }
        } else if (mem == L3_ENTRY_ONLYm ||
                 mem == L3_ENTRY_IPV4_UNICASTm ||
                 mem == L3_ENTRY_IPV4_MULTICASTm ||
                 mem == L3_ENTRY_IPV6_UNICASTm) {
            /* Pad to the longest hashing key (L3_ENTRY_IPV6_MULTICAST) */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                val_bits = 120 + /* GROUP_IP_ADDR_* */
                    128 + /* SOURCE_IP_ADDR_* */
                    12 + /* L3_IIF */
                    12 + /* VRF_ID */
                    soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                         KEY_TYPE_0f);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                val_bits = 120 + /* GROUP_IP_ADDR_* */
                    128 + /* SOURCE_IP_ADDR_* */
                    soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm, L3_IIFf) +
                    ((SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) ? 11 :
                     soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                          VRF_IDf)) +
                    soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                         KEY_TYPE_0f);
            }
        } else if (mem == VLAN_XLATEm) {
            /* Pad to the longest hashing key (VLAN_MAC) */
            val_bits = 48 + /* MAC_ADDR */
                soc_mem_field_length(unit, VLAN_MACm, KEY_TYPEf);
#ifdef BCM_HURRICANE_SUPPORT
        if(SOC_IS_HURRICANEX(unit)|| SOC_IS_GREYHOUND(unit) ||
           SOC_IS_GREYHOUND2(unit)) {
            val_bits = 41;
        }
#endif
        } else if (mem == EGR_VLAN_XLATEm) {
            /* Pad to the longest hashing key (WLAN SVP) */
            if (SOC_IS_TD_TT(unit)) {
                val_bits =
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, OVIDf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, DST_MODIDf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, DST_PORTf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, IVIDf);
            } else if (SOC_IS_KATANA(unit)|| SOC_IS_GREYHOUND(unit) ||
                        SOC_IS_HURRICANE3(unit)||SOC_IS_GREYHOUND2(unit)) { 
                val_bits =
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, OVIDf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, PORT_GROUP_IDf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, DUMMY_BITSf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, IVIDf);
            } else if (!(SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit))) {
                val_bits =
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf) +
                    soc_mem_field_length(unit, EGR_VLAN_XLATEm,
                                         WLAN_SVP__KEYf);
            }
        } else if (mem == MPLS_ENTRYm) {
            /* Pad to the longest hashing key (MIM_NVP) */
            val_bits = soc_mem_field_length(unit, MPLS_ENTRYm, KEY_TYPEf) +
                soc_mem_field_length(unit, MPLS_ENTRYm, MIM_NVP__BVIDf) +
                soc_mem_field_length(unit, MPLS_ENTRYm, MIM_NVP__BMACSAf);
        }
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        if (mem == MPLS_ENTRYm) {
            /* Pad to the longest hashing key (MIM_NVP) */
            val_bits = soc_mem_field_length(unit, MPLS_ENTRYm, KEY_TYPEf) +
                soc_mem_field_length(unit, MPLS_ENTRYm, MIM_NVP__BVIDf) +
                soc_mem_field_length(unit, MPLS_ENTRYm, MIM_NVP__BMACSAf);
        }        
    } else
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        if (mem == AXP_WRX_WCDm || mem == AXP_WRX_SVP_ASSIGNMENTm) {
            val_bits = 55;
        }
        if (mem == FT_SESSIONm) {
            val_bits =
                soc_mem_field_length(unit, FT_SESSIONm, KEYf);
        }
        if (mem == FT_SESSION_IPV6m) {
            val_bits =
                soc_mem_field_length(unit, FT_SESSION_IPV6m, KEYf) +
                soc_mem_field_length(unit, FT_SESSION_IPV6m, IPV6_LOWER_KEYf);
        }
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        if (mem == VLAN_XLATEm) {
            /* Pad to the longest hashing key (KEY_TYPE 0) */
            val_bits =
                soc_mem_field_length(unit, VLAN_XLATEm, KEY_TYPEf) +
                soc_mem_field_length(unit, VLAN_XLATEm, GLPf) +
                soc_mem_field_length(unit, VLAN_XLATEm, OVIDf) +
                soc_mem_field_length(unit, VLAN_XLATEm, IVIDf);
        }
    }

    bits = (val_bits + 7) & ~0x7;
    sal_memset(val, 0, sizeof(val));
    val_bits = bits - val_bits;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
#ifdef BCM_HURRICANE_SUPPORT
        if ((SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit))
            && (field == VRF_IDf) ) {
            fval[0] = fval[1] = 0;
            fval_bits = 11;
        } else
#endif
        {
            if (field == NUM_SOC_FIELD || !SOC_MEM_FIELD_VALID(unit, mem, field)) {
                continue;
            }

            soc_mem_field_get(unit, mem, entry, field, fval);
            fval_bits = field_length[index];
        }

        val_index = val_bits >> 5;
        fval_index = 0;
        left_shift_count = val_bits & 0x1f;
        right_shift_count = 32 - left_shift_count;
        val_bits += fval_bits;

        if (left_shift_count) {
            for (; fval_bits > 0; fval_bits -= 32) {
                val[val_index++] |= fval[fval_index] << left_shift_count;
                val[val_index] |= fval[fval_index++] >> right_shift_count;
            }
        } else {
            for (; fval_bits > 0; fval_bits -= 32) {
                val[val_index++] = fval[fval_index++];
            }
        }
    }

    key_index = 0;
    for (val_index = 0; val_bits > 0; val_index++) {
        for (right_shift_count = 0; right_shift_count < 32;
             right_shift_count += 8) {
            if (val_bits <= 0) {
                break;
            }
            key[key_index++] = (val[val_index] >> right_shift_count) & 0xff;
            val_bits -= 8;
        }
    }

    if ((bits + 7) / 8 > key_index) {
        sal_memset(&key[key_index], 0, (bits + 7) / 8 - key_index);
    }

    return bits;
}

STATIC int
_soc_fb_l3x_ip4ucast_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        IP_ADDRf,
        VRF_IDf,
        KEY_TYPEf,
        INVALIDf
    };

    soc_mem_t mem;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_mem_is_valid(unit, L3_ENTRY_ONLY_SINGLEm)) {
        mem = L3_ENTRY_ONLY_SINGLEm;
    } else
#endif
    {
        mem = L3_ENTRY_IPV4_UNICASTm;
    }
    return _soc_hash_generic_entry_to_key
        (unit, entry, key, mem, field_list);
}

STATIC int
_soc_fb_l3x_ip4mcast_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        GROUP_IP_ADDRf,
        SOURCE_IP_ADDRf,
        VLAN_IDf,
        VRF_IDf,
        KEY_TYPEf,
        KEY_TYPE_0f,
        INVALIDf
    };

    soc_mem_t mem;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_mem_is_valid(unit, L3_ENTRY_ONLY_DOUBLEm)) {
        mem = L3_ENTRY_ONLY_DOUBLEm;
    } else
#endif
    {
        mem = L3_ENTRY_IPV4_MULTICASTm;
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIFf) ||
        SOC_MEM_FIELD_VALID(unit, mem, L3_IIFf)) {
        field_list[2] = L3_IIFf;
    }

    return _soc_hash_generic_entry_to_key
        (unit, entry, key, mem, field_list);
}

STATIC int
_soc_fb_l3x_ip6ucast_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        IP_ADDR_LWR_64f,
        IP_ADDR_UPR_64f,
        VRF_IDf,
        VRF_ID_0f,
        KEY_TYPE_0f,
        INVALIDf
    };

    soc_mem_t mem;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_mem_is_valid(unit, L3_ENTRY_ONLY_DOUBLEm)) {
        mem = L3_ENTRY_ONLY_DOUBLEm;
    } else
#endif
    {
        mem = L3_ENTRY_IPV6_UNICASTm;
    }
    return _soc_hash_generic_entry_to_key
        (unit, entry, key, mem, field_list);
}

STATIC int
_soc_fb_l3x_ip6mcast_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        GROUP_IP_ADDR_LWR_64f,
        GROUP_IP_ADDR_UPR_56f,
        SOURCE_IP_ADDR_LWR_64f,
        SOURCE_IP_ADDR_UPR_64f,
        L3_IIFf,
        VLAN_ID_0f,
        VRF_IDf,
        VRF_ID_0f,
        KEY_TYPE_0f,
        INVALIDf
    };

    soc_mem_t mem;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_mem_is_valid(unit, L3_ENTRY_ONLY_DOUBLEm)) {
        mem = L3_ENTRY_ONLY_QUADm;
    } else
#endif
    {
        mem = L3_ENTRY_IPV6_MULTICASTm;
    }
    return _soc_hash_generic_entry_to_key
        (unit, entry, key, mem, field_list);
}

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_soc_tr2_l3x_lmep_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        LMEP__SGLPf,
        LMEP__VIDf,
        KEY_TYPEf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key,
                                          L3_ENTRY_ONLYm, field_list);
}

STATIC int
_soc_tr2_l3x_rmep_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        RMEP__SGLPf,
        RMEP__VIDf,
        RMEP__MDLf,
        RMEP__MEPIDf,
        KEY_TYPEf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key,
                                          L3_ENTRY_ONLYm, field_list);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
_soc_td_l3x_trill_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        TRILL__INGRESS_RBRIDGE_NICKNAMEf,
        TRILL__TREE_IDf,
        KEY_TYPEf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key,
                                          L3_ENTRY_ONLYm, field_list);
}
#endif /* BCM_TRIDENT_SUPPORT */

int
soc_fb_l3x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint8 key_type = 0;
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_l3x_base_entry_to_key(unit, entry, key);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         return soc_td2_l3x_base_entry_to_key(unit, 0, entry, key);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_ONLYm, KEY_TYPEf)) {
        key_type = soc_mem_field32_get(unit, L3_ENTRY_ONLYm, entry, KEY_TYPEf);
    } else if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_SINGLEm, KEY_TYPEf)) {
        key_type = soc_mem_field32_get(unit, L3_ENTRY_SINGLEm, entry, KEY_TYPEf);
    }

    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_ONLYm, KEY_TYPEf) 
        || SOC_MEM_FIELD_VALID(unit, L3_ENTRY_SINGLEm, KEY_TYPEf)
       ) {
        switch (key_type) {
        case TR_L3_HASH_KEY_TYPE_V4UC:
            return _soc_fb_l3x_ip4ucast_entry_to_key(unit, entry, key);
        case TR_L3_HASH_KEY_TYPE_V4MC:
            return _soc_fb_l3x_ip4mcast_entry_to_key(unit, entry, key);
        case TR_L3_HASH_KEY_TYPE_V6UC:
            return _soc_fb_l3x_ip6ucast_entry_to_key(unit, entry, key);
        case TR_L3_HASH_KEY_TYPE_V6MC:
            return _soc_fb_l3x_ip6mcast_entry_to_key(unit, entry, key);
#ifdef BCM_TRIUMPH2_SUPPORT
        case TR_L3_HASH_KEY_TYPE_LMEP:
            if (!soc_feature(unit, soc_feature_oam)) {
                return 0;
            }
            return _soc_tr2_l3x_lmep_entry_to_key(unit, entry, key);
        case TR_L3_HASH_KEY_TYPE_RMEP:
            if (!soc_feature(unit, soc_feature_oam)) {
                return 0;
            }
            return _soc_tr2_l3x_rmep_entry_to_key(unit, entry, key);
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        case TR_L3_HASH_KEY_TYPE_TRILL:
            if (!soc_feature(unit, soc_feature_trill)) {
                return 0;
            }
            return _soc_td_l3x_trill_entry_to_key(unit, entry, key);
#endif /* BCM_TRIDENT_SUPPORT */
        default:
            return 0;
        }
    }

    if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, V6f)) {
        if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
            return _soc_fb_l3x_ip6mcast_entry_to_key(unit, entry, key);
        } else {
            return _soc_fb_l3x_ip6ucast_entry_to_key(unit, entry, key);
        }
    } else {
        if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
            return _soc_fb_l3x_ip4mcast_entry_to_key(unit, entry, key);
        } else {
            return _soc_fb_l3x_ip4ucast_entry_to_key(unit, entry, key);
        }
    }
}

uint32
soc_fb_l3x2_entry_hash(int unit, uint32 *entry)
{
    int             hash_sel;
    uint32          index;
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          tmp_hs;

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                 tmp_hs, L3_HASH_SELECTf);
    key_nbits = soc_fb_l3x_base_entry_to_key(unit, entry, key);
    index = soc_fb_l3_hash(unit, hash_sel, key_nbits, key);

    return index;
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
int
soc_fb_l2x_entry_bank_hash_sel_get(int unit, int bank, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_L2_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                      tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

uint32
soc_fb_l2x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    soc_draco_l2x_base_entry_to_key(unit, (l2x_entry_t *)entry, key);
    index = soc_fb_l2_hash(unit, hash_sel, key);

    return index;
}

int
soc_fb_l3x_entry_bank_hash_sel_get(int unit, int bank, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_L3_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                     tmp_hs, L3_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

uint32
soc_fb_l3x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_fb_l3x_base_entry_to_key(unit, entry, key);
    index = soc_fb_l3_hash(unit, hash_sel, key_nbits, key);

    return index;
}

int
soc_fb_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int        hash_sel;

    SOC_IF_ERROR_RETURN
        (soc_fb_l3x_entry_bank_hash_sel_get(unit, bank, &hash_sel));

    return soc_fb_l3x_entry_hash(unit, hash_sel, entry);
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRIUMPH_SUPPORT || BCM_RAVEN_SUPPORT */

int
soc_fb_rv_vlanmac_hash_sel_get(int unit, int dual, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* From Triumph onwards VLAN_MAC uses VLAN_XLATE hash select reg */
        return soc_tr_hash_sel_get(unit, VLAN_XLATEm, dual, hash_sel);    
    }
#endif
#if defined(BCM_RAVEN_SUPPORT)
    if (dual > 0 && SOC_REG_IS_VALID(unit, VLAN_MAC_AUX_HASH_CONTROLr)) {
        SOC_IF_ERROR_RETURN(READ_VLAN_MAC_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, VLAN_MAC_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, VLAN_MAC_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }
#endif /* BCM_RAVEN_SUPPORT */

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                      tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
uint32
soc_tr_l2x_hash(int unit, int hash_sel, int key_nbits, void *base_entry,
                uint8 *key)
{
    uint32 rv;
    uint32 fval[SOC_MAX_MEM_WORDS];
   
    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask;
        int     bits;

        mask = soc_mem_index_max(unit, L2_HITDA_ONLYm);
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l2x = mask;
        SOC_CONTROL(unit)->hash_bits_l2x = bits;
    }
     
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
            case GH_L2_HASH_KEY_TYPE_BRIDGE:
                soc_mem_field_get(unit, L2Xm, base_entry, MAC_ADDRf, fval);
                rv = fval[0];
                break;
            case GH_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
                rv = soc_mem_field32_get(unit, L2Xm, base_entry, OVIDf);
                break;
            case GH_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
                rv = soc_mem_field32_get(unit, L2Xm, base_entry, OVIDf) |
                    (soc_mem_field32_get(unit, L2Xm, base_entry, IVIDf) <<
                     soc_mem_field_length(unit, L2Xm, OVIDf));
                break;
            case GH_L2_HASH_KEY_TYPE_VIF:
                /* use only 12 bit of the 14 bit DST_VIF */
                rv = (soc_mem_field32_get(unit, L2Xm, base_entry,
                                      VIF__DST_VIFf) & 0xfff) |
                (soc_mem_field32_get(unit, L2Xm, base_entry,
                                     VIF__NAMESPACEf) << 12);                
                break;
            case GH_L2_HASH_KEY_TYPE_PE_VID:
                rv = (soc_mem_field32_get(unit, L2Xm, base_entry,
                                      PE_VID__ETAG_VIDf) & 0xfff) |
                (soc_mem_field32_get(unit, L2Xm, base_entry,
                                     PE_VID__NAMESPACEf) << 12);                
                break;
            default:
                rv = 0;
                break;
            }
            break;
        }
#endif  /* BCM_GREYHOUND_SUPPORT */
        switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
        case TR_L2_HASH_KEY_TYPE_VFI:
            soc_mem_field_get(unit, L2Xm, base_entry, MAC_ADDRf, fval);
            rv = fval[0];
            break;
        case TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
            rv = soc_mem_field32_get(unit, L2Xm, base_entry, OVIDf);
            break;
        case TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            rv = soc_mem_field32_get(unit, L2Xm, base_entry, OVIDf) |
                (soc_mem_field32_get(unit, L2Xm, base_entry, IVIDf) <<
                 soc_mem_field_length(unit, L2Xm, OVIDf));
            break;
#ifdef BCM_TRIDENT_SUPPORT
        case TR_L2_HASH_KEY_TYPE_VIF:
            /* use only 12 bit of the 14 bit DST_VIF */
            rv = (soc_mem_field32_get(unit, L2Xm, base_entry,
                                      VIF__DST_VIFf) & 0xfff) |
                (soc_mem_field32_get(unit, L2Xm, base_entry,
                                     VIF__NAMESPACEf) << 12);
            break;
        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            soc_mem_field_get(unit, L2Xm, base_entry,
                              TRILL_NONUC_ACCESS__MAC_ADDRf, fval);
            rv = fval[0];
            break;
        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            soc_mem_field_get(unit, L2Xm, base_entry,
                              TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, fval);
            rv = fval[0];
            break;
        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            rv = soc_mem_field32_get(unit, L2Xm, base_entry,
                                     TRILL_NONUC_NETWORK_SHORT__TREE_IDf) |
                (soc_mem_field32_get(unit, L2Xm, base_entry,
                                     TRILL_NONUC_NETWORK_SHORT__VLAN_IDf) <<
                 soc_mem_field_length(unit, L2Xm,
                                      TRILL_NONUC_NETWORK_SHORT__TREE_IDf));
            rv = 0;
            break;
#endif /* BCM_TRIDENT_SUPPORT */
        default:
            rv = 0;
            break;
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr_l2_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

STATIC int
_soc_tr_l2x_bridge_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        VLAN_IDf,
        MAC_ADDRf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

#ifdef BCM_KATANA_SUPPORT
STATIC int
_soc_tr_l2x_bfd_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        BFD__KEYf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}
#endif

STATIC int
_soc_tr_l2x_scc_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        OVIDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

STATIC int
_soc_tr_l2x_dcc_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        OVIDf,
        IVIDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

STATIC int
_soc_tr_l2x_vfi_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        VLAN_IDf,  /* padded from VFIf */
        MAC_ADDRf,
        INVALIDf
    };
    l2x_entry_t new_entry;
    uint32 vfi;

    sal_memcpy(&new_entry, entry, sizeof(new_entry));
    vfi = soc_mem_field32_get(unit, L2Xm, &new_entry, VFIf);
    soc_mem_field32_set(unit, L2Xm, &new_entry, VLAN_IDf, vfi);

    return _soc_hash_generic_entry_to_key(unit, &new_entry, key, L2Xm,
                                          field_list);
}

#ifdef BCM_KATANA_SUPPORT
STATIC int
soc_kt_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case KT_L2_HASH_KEY_TYPE_BRIDGE:
        return _soc_tr_l2x_bridge_entry_to_key(unit, entry, key);
        break;
    case KT_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        return _soc_tr_l2x_scc_entry_to_key(unit, entry, key);
    case KT_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        return _soc_tr_l2x_dcc_entry_to_key(unit, entry, key);
        break;
    case KT_L2_HASH_KEY_TYPE_VFI:
        return _soc_tr_l2x_vfi_entry_to_key(unit, entry, key);
        break;
    case KT_L2_HASH_KEY_TYPE_BFD:
        return _soc_tr_l2x_bfd_entry_to_key(unit, entry, key);
        break;
    default:
        return 0;
    }
}
#endif

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
_soc_td_l2x_vif_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        VIF__NAMESPACEf,
        VIF__DST_VIFf,
        VIF__Pf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

STATIC int
_soc_td_l2x_trill_access_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        TRILL_NONUC_ACCESS__VLAN_IDf,
        TRILL_NONUC_ACCESS__MAC_ADDRf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

STATIC int
_soc_td_l2x_trill_network_long_entry_to_key(int unit, uint32 *entry,
                                            uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        TRILL_NONUC_NETWORK_LONG__VLAN_IDf,
        TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf,
        TRILL_NONUC_NETWORK_LONG__TREE_IDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, L2Xm, field_list);
}

STATIC int
_soc_td_l2x_trill_network_short_entry_to_key(int unit, uint32 *entry,
                                             uint8 *key)
{
    /* The format of the stream fed into hash calculation is the same as
     * TRILL_NONUC_NETWORK_LONG except MAC_ADDRESS portion is all 0's */
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        TRILL_NONUC_NETWORK_LONG__VLAN_IDf,
        TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf,
        TRILL_NONUC_NETWORK_LONG__TREE_IDf,
        INVALIDf
    };
    l2x_entry_t new_entry;
    sal_mac_addr_t mac;
    uint32 tree_id;

    sal_memcpy(&new_entry, entry, sizeof(new_entry));
    /* VLAN_ID is at the same location for both SHORT and LONG view */
    sal_memset(&mac, 0, sizeof(mac));
    soc_mem_mac_addr_set(unit, L2Xm, &new_entry,
                         TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
    tree_id = soc_mem_field32_get(unit, L2Xm, entry,
                                  TRILL_NONUC_NETWORK_SHORT__TREE_IDf);
    soc_mem_field32_set(unit, L2Xm, &new_entry,
                        TRILL_NONUC_NETWORK_LONG__TREE_IDf, tree_id);

    return _soc_hash_generic_entry_to_key(unit, &new_entry, key, L2Xm,
                                          field_list);
}
#endif /* BCM_TRIDENT_SUPPORT */

int
soc_tr_l2x_base_entry_to_key(int unit, void *entry, uint8 *key)
{
#ifdef BCM_KATANA2_SUPPORT
if (SOC_IS_KATANA2(unit)) {
    return soc_kt2_l2x_base_entry_to_key(unit, entry, key);
} else
#endif

#ifdef BCM_KATANA_SUPPORT
if (SOC_IS_KATANA(unit)) {
    return soc_kt_l2x_base_entry_to_key(unit, entry, key);
} else
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
    return soc_greyhound_l2x_base_entry_to_key(unit, entry, key);
#ifdef BCM_GREYHOUND2_SUPPORT
    } else if (SOC_IS_GREYHOUND2(unit)) {
        return soc_gh2_l2x_base_entry_to_key(unit, entry, key);
#endif /* BCM_GREYHOUND2_SUPPORT */
} else
#endif /* BCM_GREYHOUND_SUPPORT */
  {
    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case TR_L2_HASH_KEY_TYPE_BRIDGE:
        return _soc_tr_l2x_bridge_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        return _soc_tr_l2x_scc_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        return _soc_tr_l2x_dcc_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_VFI:
        return _soc_tr_l2x_vfi_entry_to_key(unit, entry, key);
#ifdef BCM_TRIDENT_SUPPORT
    case TR_L2_HASH_KEY_TYPE_VIF:
        if (!soc_feature(unit, soc_feature_niv)) {
            return 0;
        }
        return _soc_td_l2x_vif_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
        if (!soc_feature(unit, soc_feature_trill)) {
            return 0;
        }
        return _soc_td_l2x_trill_access_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
        if (!soc_feature(unit, soc_feature_trill)) {
            return 0;
        }
        return _soc_td_l2x_trill_network_long_entry_to_key(unit, entry, key);
    case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
        if (!soc_feature(unit, soc_feature_trill)) {
            return 0;
        }
        return _soc_td_l2x_trill_network_short_entry_to_key(unit, entry, key);
#endif /* BCM_TRIDENT_SUPPORT */
    default:
        return 0;
    }
  }
}

uint32
soc_tr_l2x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_l2x_base_entry_to_key(unit, entry, key);
    index = soc_tr_l2x_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_l2x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int             rv;
    int             hash_sel = 0;

    rv = soc_fb_l2x_entry_bank_hash_sel_get(unit, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tr_l2x_entry_hash(unit, hash_sel, entry);
}

int
soc_tr_hash_sel_get(int unit, soc_mem_t mem, int bank, int *hash_sel)
{
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval;

    field = bank > 0 ? HASH_SELECT_Bf : HASH_SELECT_Af;
    switch (mem) {
    case VLAN_XLATEm:
    case VLAN_MACm:
        reg = VLAN_XLATE_HASH_CONTROLr;
        break;
    case EGR_VLAN_XLATEm:
        reg = EGR_VLAN_XLATE_HASH_CONTROLr;
        break;
    case MPLS_ENTRYm: 
        reg = MPLS_ENTRY_HASH_CONTROLr;
        break;
    case AXP_WRX_WCDm:
        reg = AXP_WRX_WCD_HASH_CTRLr;
        field = bank > 0 ? SELECT_Bf : SELECT_Af;
        break;
    case AXP_WRX_SVP_ASSIGNMENTm:
        reg = AXP_WRX_SVP_HASH_CTRLr;
        field = bank > 0 ? SELECT_Bf : SELECT_Af;
        break;
#ifdef BCM_TRIUMPH3_SUPPORT
    case FT_SESSIONm:
    case FT_SESSION_IPV6m:
        reg = FT_HASH_CONTROLr;
        field = bank > 0 ? HASH_SELECT_Bf : HASH_SELECT_Af;
        break;
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        return (soc_tomahawk3_hash_offset_get(unit, mem, bank, hash_sel, NULL));
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* INCLUDE_L3 */
    default:
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    *hash_sel = soc_reg_field_get(unit, reg, rval, field);

    return SOC_E_NONE;
}

uint32
soc_tr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, void *base_entry,
                       uint8 *key)
{
    uint32 rv = 0;
    uint32 fval[SOC_MAX_MEM_WORDS];

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_vlan_xlate_hash(unit, hash_sel, key_nbits, base_entry, key);
    }
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        return soc_hr3_vlan_xlate_hash(unit, hash_sel, key_nbits, base_entry, key);
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return soc_gh2_vlan_xlate_hash(unit, hash_sel, key_nbits, base_entry, key);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }

        /* For the Variants whose VLAN_XLATE (and VLAN_MAC) memory size are
            limited by Bond Option, The shift amount should be based on the "Original"
            memory size
        */
#ifdef BCM_KATANA_SUPPORT
        /* Sabre variants has VLAN_XLATE limited by bond option,
            use the original 16K = 11 bits
        */
        if (SOC_IS_KATANA(unit)) {
            bits = 11;
        }
#endif
        SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
        SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        switch (soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                    KEY_TYPEf)) {
        case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OTAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_ITAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, ITAGf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            soc_mem_field_get(unit, VLAN_MACm, base_entry, MAC_ADDRf, fval);
            rv = fval[0];
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_IVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, IVIDf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            /* Use only the upper 4 bit of OTAG */
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf) >>
                12;
            break;
        case TR_VLXLT_HASH_KEY_TYPE_HPAE:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC_IP_BIND__SIPf);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_VIF:
        case TR_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     VIF__SRC_VIFf);
            break;
        default:
            rv = 0;
            break;
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

STATIC int
_soc_tr_vlan_xlate_dtag_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        OVIDf,
        IVIDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_xlate_otag_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        OTAGf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_xlate_itag_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        ITAGf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_mac_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        MAC_ADDRf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_MACm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_xlate_ovid_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        OVIDf,
        IVIDf, /* zero */
        INVALIDf
    };
    vlan_xlate_entry_t new_entry;

    sal_memcpy(&new_entry, entry, sizeof(new_entry));
    soc_mem_field32_set(unit, VLAN_XLATEm, &new_entry, IVIDf, 0);

    return _soc_hash_generic_entry_to_key(unit, &new_entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_xlate_ivid_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        OVIDf, /* zero */
        IVIDf,
        INVALIDf
    };
    vlan_xlate_entry_t new_entry;

    sal_memcpy(&new_entry, entry, sizeof(new_entry));
    soc_mem_field32_set(unit, VLAN_XLATEm, &new_entry, OVIDf, 0);

    return _soc_hash_generic_entry_to_key(unit, &new_entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_vlan_xlate_pri_cfi_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        GLPf,
        OTAGf, /* OVID portion of this field is zero */
        INVALIDf
    };

    vlan_xlate_entry_t new_entry;

    sal_memcpy(&new_entry, entry, sizeof(new_entry));
    soc_mem_field32_set(unit, VLAN_XLATEm, &new_entry, OVIDf, 0);

    return _soc_hash_generic_entry_to_key(unit, &new_entry, key, VLAN_XLATEm,
                                          field_list);
}

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_soc_tr2_vlan_xlate_hpae_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        MAC_IP_BIND__SIPf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_MACm,
                                          field_list);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
_soc_td_vlan_xlate_vif_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        VIF__GLPf,
        VIF__SRC_VIFf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_td_vlan_xlate_vif_vlan_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        VIF__GLPf,
        VIF__SRC_VIFf,
        VIF__VLANf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, VLAN_XLATEm,
                                          field_list);
}
#endif /* BCM_TRIDENT_SUPPORT */

int
soc_tr_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_vlan_xlate_base_entry_to_key(unit, entry, key);
    }
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        return soc_hr3_vlan_xlate_base_entry_to_key(unit, entry, key);
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return soc_gh2_vlan_xlate_base_entry_to_key(unit, entry, key);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    switch (soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf)) {
    case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
        return _soc_tr_vlan_xlate_dtag_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_OTAG:
        return _soc_tr_vlan_xlate_otag_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_ITAG:
        return _soc_tr_vlan_xlate_itag_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
        return _soc_tr_vlan_mac_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_OVID:
        return _soc_tr_vlan_xlate_ovid_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_IVID:
        return _soc_tr_vlan_xlate_ivid_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
        return _soc_tr_vlan_xlate_pri_cfi_entry_to_key(unit, entry, key);
#ifdef BCM_TRIUMPH2_SUPPORT
    case TR_VLXLT_HASH_KEY_TYPE_HPAE:
        if (!soc_feature(unit, soc_feature_ip_source_bind)) {
            return 0;
        }
        return _soc_tr2_vlan_xlate_hpae_entry_to_key(unit, entry, key);
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    case TR_VLXLT_HASH_KEY_TYPE_VIF:
        if (!soc_feature(unit, soc_feature_niv)) {
            return 0;
        }
        return _soc_td_vlan_xlate_vif_entry_to_key(unit, entry, key);
    case TR_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
        if (!soc_feature(unit, soc_feature_niv)) {
            return 0;
        }
        return _soc_td_vlan_xlate_vif_vlan_entry_to_key(unit, entry, key);
#endif /* BCM_TRIDENT_SUPPORT */
    default:
        return 0;
    }
}

uint32
soc_tr_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_vlan_xlate_base_entry_to_key(unit, entry, key);

    index = soc_tr_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int     hash_sel;

    SOC_IF_ERROR_RETURN
        (soc_tr_hash_sel_get(unit, VLAN_XLATEm, bank, &hash_sel));
    return soc_tr_vlan_xlate_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                           void *base_entry, uint8 *key)
{
    uint32 rv;
    uint32 fval[SOC_MAX_MEM_WORDS];

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, base_entry, key);
    }
#endif

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, EGR_VLAN_XLATEm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate = mask;
        SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf)) {
            switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                        ENTRY_TYPEf)) {
            case 0: /* VLAN_XLATE */
            case 1: /* VLAN_XLATE_DVP */
            case 2: /* VLAN_XLATE_WLAN */
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         OVIDf);
                break;
            case 3: /* ISID_XLATE */
            case 4: /* ISID_DVP_XLATE */
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         MIM_ISID__VFIf) |
                    (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         MIM_ISID__DVPf) <<
                     soc_mem_field_length(unit, EGR_VLAN_XLATEm,
                                          MIM_ISID__VFIf));
                break;
            case 5: /* WLAN_SVP_TUNNEL */
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         WLAN_SVP__TUNNEL_IDf);
                break;
            case 6: /* WLAN_SVP_BSSID */
                soc_mem_field_get(unit, MPLS_ENTRYm, base_entry,
                                  WLAN_SVP__BSSIDf, fval);
                rv = fval[0];
                break;
            case 7: /* WLAN_SVP_BSSID_RID */
                soc_mem_field_get(unit, MPLS_ENTRYm, base_entry,
                                  WLAN_SVP__BSSIDf, fval);
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         WLAN_SVP__RIDf) |
                    (fval[0] << soc_mem_field_length(unit, EGR_VLAN_XLATEm,
                                                     WLAN_SVP__RIDf));
                break;
            default:
                rv = 0;
                break;
            }
        } else {
            rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry, OVIDf);
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate;
}

STATIC int
_soc_tr_egr_vlan_xlate_xlate_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        ENTRY_TYPEf,
        OVIDf,
        PORT_GROUP_IDf,
        NUM_SOC_FIELD, /* skip mark */
        IVIDf,
        INVALIDf
    };

    if (SOC_IS_TD_TT(unit)) {
        field_list[2] = DST_MODIDf;
        field_list[3] = DST_PORTf;
    } else if (SOC_IS_KATANA(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
        field_list[3] = DUMMY_BITSf; 
    } else if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_XLATEm, DVPf)) {
        field_list[2] = DVPf;
    }

    return _soc_hash_generic_entry_to_key(unit, entry, key, EGR_VLAN_XLATEm,
                                          field_list);
}

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_soc_tr_egr_vlan_xlate_mim_isid_entry_to_key(int unit, uint32 *entry,
                                             uint8 *key)
{
    static soc_field_t field_list[] = {
        ENTRY_TYPEf,
        MIM_ISID__VFIf,
        MIM_ISID__DVPf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, EGR_VLAN_XLATEm,
                                          field_list);
}

STATIC int
_soc_tr_egr_vlan_xlate_wlan_svp_entry_to_key(int unit, uint32 *entry,
                                             uint8 *key)
{
    static soc_field_t field_list[] = {
        ENTRY_TYPEf,
        WLAN_SVP__RIDf,
        WLAN_SVP__BSSIDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, EGR_VLAN_XLATEm,
                                          field_list);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

int
soc_tr_egr_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_egr_vlan_xlate_base_entry_to_key(unit, entry, key);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf)) {
        switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry,
                                    ENTRY_TYPEf)) {
        case 0: /* VLAN_XLATE */
        case 1: /* VLAN_XLATE_DVP */
            return _soc_tr_egr_vlan_xlate_xlate_entry_to_key(unit, entry, key);
        case 2: /* VLAN_XLATE_WLAN */
            if (!soc_feature(unit, soc_feature_wlan)) {
                return 0;
            }
            return _soc_tr_egr_vlan_xlate_xlate_entry_to_key(unit, entry, key);
        case 3: /* ISID_XLATE */
        case 4: /* ISID_DVP_XLATE */
            if (!soc_feature(unit, soc_feature_mim)) {
                return 0;
            }
            return _soc_tr_egr_vlan_xlate_mim_isid_entry_to_key(unit, entry,
                                                                key);
        case 5: /* WLAN_SVP_TUNNEL */
        case 6: /* WLAN_SVP_BSSID */
        case 7: /* WLAN_SVP_BSSID_RID */
            if (!soc_feature(unit, soc_feature_wlan)) {
                return 0;
            }
            return _soc_tr_egr_vlan_xlate_wlan_svp_entry_to_key(unit, entry,
                                                                key);
        default:
            return 0;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return _soc_tr_egr_vlan_xlate_xlate_entry_to_key(unit, entry, key);
}

uint32
soc_tr_egr_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_egr_vlan_xlate_base_entry_to_key(unit, entry, key);
    index = soc_tr_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_egr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int     hash_sel;

    SOC_IF_ERROR_RETURN
        (soc_tr_hash_sel_get(unit, EGR_VLAN_XLATEm, bank, &hash_sel));
    return soc_tr_egr_vlan_xlate_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr_mpls_hash(int unit, int hash_sel, int key_nbits, void *base_entry,
                 uint8 *key)
{
    uint32 rv;
    uint32 fval[SOC_MAX_MEM_WORDS];
#ifdef BCM_SABER2_SUPPORT
        uint16 dev_id;
        uint8 rev_id;
#endif

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_mpls == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, MPLS_ENTRYm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }

        /* For the Variants whose MPLS_ENTRY memory size is limited by Bond
            Option, The shift amount should be based on the "Original" size
        */
#ifdef BCM_SABER2_SUPPORT
        /* Dagger2 variants has MPLS_ENTRY limited by bond option,
         * use the original 4K = 9 bits
         */
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (SOC_IS_SABER2(unit) && (dev_id == BCM56233_DEVICE_ID)) {
            bits = 9;
        }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_ENDURO_SUPPORT)
        /* Saber and Dagger variants has MPLS_ENTRY limited by bond option,
         * use the original 4K = 9 bits
         */
        if (SOC_IS_KATANA(unit) || SOC_IS_ENDURO(unit)) {
            bits = 9;
        }
#endif /* BCM_KATANA_SUPPORT || BCM_ENDURO_SUPPORT */
        SOC_CONTROL(unit)->hash_mask_mpls = mask;
        SOC_CONTROL(unit)->hash_bits_mpls = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, KEY_TYPEf)) {
            switch (soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                        KEY_TYPEf)) {
            case 0: /* MPLS */
                rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                         MPLS_LABELf);
                break;
            case 1: /* MIM_NVP */
                soc_mem_field_get(unit, MPLS_ENTRYm, base_entry,
                                  MIM_NVP__BMACSAf, fval);
                rv = fval[0];
                break;
            case 2: /* MIM_ISID */
            case 3: /* MIM_ISID_SVP */
                rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                         MIM_ISID__ISIDf);
                break;
            case 4: /* WLAN_MAC */
                soc_mem_field_get(unit, MPLS_ENTRYm, base_entry,
                                  WLAN_MAC__MAC_ADDRf, fval);
                rv = fval[0];
                break;
            case 5: /* TRILL */
                rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                         TRILL__RBRIDGE_NICKNAMEf);
                break;
            default:
                rv = 0;
                break;
            }
        } else {
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     MPLS_LABELf);
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr_mpls_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_mpls;
}

STATIC int
_soc_tr_mpls_legacy_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        PORT_NUMf,
        MODULE_IDf,
        Tf,
        MPLS_LABELf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_soc_tr_mpls_mim_nvp_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        MIM_NVP__BVIDf,
        MIM_NVP__BMACSAf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}

STATIC int
_soc_tr_mpls_mim_isid_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        MIM_ISID__ISIDf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}

STATIC int
_soc_tr_mpls_mim_isid_svp_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        MIM_ISID__ISIDf,
        MIM_ISID__SVPf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}

STATIC int
_soc_tr_mpls_wlan_mac_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        WLAN_MAC__MAC_ADDRf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
_soc_tr_mpls_trill_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEY_TYPEf,
        TRILL__RBRIDGE_NICKNAMEf,
        INVALIDf
    };

    return _soc_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                          field_list);
}
#endif /* BCM_TRIDENT_SUPPORT */

int
soc_tr_mpls_base_entry_to_key(int unit, void *entry, uint8 *key)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return soc_kt2_mpls_base_entry_to_key(unit, entry, key);
    }
#endif
    
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, KEY_TYPEf)) {
        switch (soc_mem_field32_get(unit, MPLS_ENTRYm, entry, KEY_TYPEf)) {
        case 0: /* MPLS */
            return _soc_tr_mpls_legacy_entry_to_key(unit, entry, key);
        case 1: /* MIM_NVP */
            if (!soc_feature(unit, soc_feature_mim)) {
                return 0;
            }
            return _soc_tr_mpls_mim_nvp_entry_to_key(unit, entry, key);
        case 2: /* MIM_ISID */
            if (!soc_feature(unit, soc_feature_mim)) {
                return 0;
            }
            return _soc_tr_mpls_mim_isid_entry_to_key(unit, entry, key);
        case 3: /* MIM_ISID_SVP */
            if (!soc_feature(unit, soc_feature_mim)) {
                return 0;
            }
            return _soc_tr_mpls_mim_isid_svp_entry_to_key(unit, entry, key);
        case 4: /* WLAN_MAC */
            if (!soc_feature(unit, soc_feature_wlan)) {
                return 0;
            }
            return _soc_tr_mpls_wlan_mac_entry_to_key(unit, entry, key);
#ifdef BCM_TRIDENT_SUPPORT
        case 5: /* TRILL */
            if (!soc_feature(unit, soc_feature_trill)) {
                return 0;
            }
            return _soc_tr_mpls_trill_entry_to_key(unit, entry, key);
#endif /* BCM_TRIDENT_SUPPORT */
        default:
            return 0;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return _soc_tr_mpls_legacy_entry_to_key(unit, entry, key);

}

uint32
soc_tr_mpls_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_mpls_base_entry_to_key(unit, entry, key);
    index = soc_tr_mpls_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int     hash_sel;
    
    SOC_IF_ERROR_RETURN
        (soc_tr_hash_sel_get(unit, MPLS_ENTRYm, bank, &hash_sel));
    return soc_tr_mpls_entry_hash(unit, hash_sel, entry);
}
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
int
soc_tr3_ft_session_entry_to_key(int unit, soc_mem_t mem, void *entry, uint8 *key)
{
    static soc_field_t field_list_ipv4[] = {
        KEYf,
        INVALIDf
    };
    static soc_field_t field_list_ipv6[] = {
        KEYf,
        IPV6_LOWER_KEYf,
        INVALIDf
    };

    if (mem == FT_SESSIONm || mem == FT_SESSION_IPV6m) {
        switch (soc_mem_field32_get(unit, mem, entry,
                                    KEY_TYPEf)) {
        case 0x01: /* IPv4 entry */
            return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_ipv4);
            break;
        case 0x02: /* IPv6 upper - not sure if these can be handled from same entry */
        case 0x03: /* IPv6 lower */
            return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_ipv6);
            break;
        default:
            return 0;
            break;
        }
    }

    return 0;
}

int
soc_tr3_wlan_base_entry_to_key(int unit, soc_mem_t mem, void *entry, uint8 *key)
{
    static soc_field_t field_list_port1[] = {
        TUNNEL_IDf,
        INVALIDf
    };
    static soc_field_t field_list_port2[] = {
        BSSIDf,
        INVALIDf
    };
    static soc_field_t field_list_port3[] = {
        RIDf,
        BSSIDf,
        INVALIDf
    };
    static soc_field_t field_list_client[] = {
        MAC_ADDRf,
        INVALIDf
    };

    if (mem == AXP_WRX_SVP_ASSIGNMENTm) {
        switch (soc_mem_field32_get(unit, AXP_WRX_SVP_ASSIGNMENTm, entry,
                                    ENTRY_TYPEf)) {
        case 1: /* WLAN_SVP_TUNNEL */
            return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_port1);
        case 2: /* WLAN_SVP_BSSID */
            return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_port2);
        case 3: /* WLAN_SVP_BSSID_RID */
            return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_port3);
        default:
            return 0;
        }
    } else { /* AXP_WRX_WCD:MAC_ADDR */
        return _soc_hash_generic_entry_to_key(unit, entry, key, mem, field_list_client);
    }
}

uint32
soc_tr3_ft_session_hash(int unit, soc_mem_t mem, int hash_sel, int key_nbits,
                  void *base_entry, uint8 *key)
{
    uint32 rv;
    uint32 fval[SOC_MAX_MEM_WORDS];

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (mem == FT_SESSIONm && SOC_CONTROL(unit)->hash_mask_ft_session_ipv4 == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket, 1K buckets, = 8192 IPv4, 4096 IPv6 entries */
        mask = soc_mem_index_max(unit, FT_SESSIONm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_ft_session_ipv4 = mask;
        SOC_CONTROL(unit)->hash_bits_ft_session_ipv4 = bits;
    }
    if (mem == FT_SESSION_IPV6m && SOC_CONTROL(unit)->hash_mask_ft_session_ipv6 == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, FT_SESSION_IPV6m) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_ft_session_ipv6 = mask;
        SOC_CONTROL(unit)->hash_bits_ft_session_ipv6 = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - (mem == FT_SESSIONm ? SOC_CONTROL(unit)->hash_bits_ft_session_ipv4 :
                                           SOC_CONTROL(unit)->hash_bits_ft_session_ipv6);
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        if (mem == FT_SESSIONm) {
            soc_mem_field_get(unit, FT_SESSIONm, base_entry,
                              KEYf, fval);
            rv = fval[0];
        } else { /* mem == FT_SESSION_IPV6 */
            soc_mem_field_get(unit, FT_SESSION_IPV6m, base_entry,
                              IPV6_LOWER_KEYf, fval);
            rv = fval[0];
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - (mem == FT_SESSIONm ? SOC_CONTROL(unit)->hash_bits_ft_session_ipv4 :
                                           SOC_CONTROL(unit)->hash_bits_ft_session_ipv6);
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr3_ft_session_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & (mem == FT_SESSIONm ? SOC_CONTROL(unit)->hash_mask_ft_session_ipv4 :
                                       SOC_CONTROL(unit)->hash_mask_ft_session_ipv6);
}

uint32
soc_tr3_wlan_hash(int unit, soc_mem_t mem, int hash_sel, int key_nbits,
                  void *base_entry, uint8 *key)
{
    uint32 rv;
    uint32 fval[SOC_MAX_MEM_WORDS];

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (mem == AXP_WRX_WCDm && SOC_CONTROL(unit)->hash_mask_wlan_client == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, AXP_WRX_WCDm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_wlan_client = mask;
        SOC_CONTROL(unit)->hash_bits_wlan_client = bits;
    }
    if (mem == AXP_WRX_SVP_ASSIGNMENTm && SOC_CONTROL(unit)->hash_mask_wlan_port == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, AXP_WRX_SVP_ASSIGNMENTm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_wlan_port = mask;
        SOC_CONTROL(unit)->hash_bits_wlan_port = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - (mem == AXP_WRX_WCDm ? SOC_CONTROL(unit)->hash_bits_wlan_client : 
                                           SOC_CONTROL(unit)->hash_bits_wlan_port);
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        if (mem == AXP_WRX_SVP_ASSIGNMENTm) {
            switch (soc_mem_field32_get(unit, AXP_WRX_SVP_ASSIGNMENTm, base_entry,
                                        ENTRY_TYPEf)) {
            case 1: /* WLAN_SVP_TUNNEL */
                rv = soc_mem_field32_get(unit, AXP_WRX_SVP_ASSIGNMENTm, base_entry,
                                         TUNNEL_IDf);
                break;
            case 2: /* WLAN_SVP_BSSID */
                soc_mem_field_get(unit, AXP_WRX_SVP_ASSIGNMENTm, base_entry,
                                  BSSIDf, fval);
                rv = fval[0];
                break;
            case 3: /* WLAN_SVP_BSSID_RID */
                soc_mem_field_get(unit, AXP_WRX_SVP_ASSIGNMENTm, base_entry,
                                  BSSIDf, fval);
                rv = soc_mem_field32_get(unit, AXP_WRX_SVP_ASSIGNMENTm, base_entry,
                                         RIDf) |
                    (fval[0] << soc_mem_field_length(unit, AXP_WRX_SVP_ASSIGNMENTm,
                                                     RIDf));
                break;
            default:
                rv = 0;
                break;
            }
        } else { /* AXP_WRX_WCD:MAC_ADDR */
            rv = soc_mem_field32_get(unit, AXP_WRX_WCDm, base_entry, MAC_ADDRf);
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - (mem == AXP_WRX_WCDm ? SOC_CONTROL(unit)->hash_bits_wlan_client : 
                                           SOC_CONTROL(unit)->hash_bits_wlan_port);
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_tr3_wlan_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & (mem == AXP_WRX_WCDm ? SOC_CONTROL(unit)->hash_mask_wlan_client : 
                                       SOC_CONTROL(unit)->hash_mask_wlan_port);
}

uint32
soc_tr3_ft_session_entry_hash(int unit, soc_mem_t mem, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr3_ft_session_entry_to_key(unit, mem, entry, key);
    index = soc_tr3_ft_session_hash(unit, mem, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr3_wlan_entry_hash(int unit, soc_mem_t mem, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr3_wlan_base_entry_to_key(unit, mem, entry, key);
    index = soc_tr3_wlan_hash(unit, mem, hash_sel, key_nbits, entry, key);

    return index;
}
#endif /* BCM_TRIUMPH3_SUPPORT */


#ifdef SOC_ROBUST_HASH
/*
 * Function:
 * soc_robust_hash_get
 * Purpose:
 *      Internal function to compute Robust Hash key transformation
 * Parameters:
 *      unit    - (IN) SOC unit #
 *      rbh_cfg - (IN) Pointer to Robust hash table structure
 *      bank    - (IN) Dual hash bank # (0/1)
 *      key     - (IN/OUT) Hashing key
 *      n_bits  - (IN) Number of bits in the Input hash key
 * Returns:
 *      BCM_E_XXX
*/
int
soc_robust_hash_get(int unit, soc_robust_hash_config_t *rbh_cfg, int bank,
                    uint8 *key, int n_bits)
{
    int ix, n_bytes, byte_offset = 0;
    int append = 0;
    int nibble_swapped_key = 0, key_size = 0;
    int remap_table_index = 0, action_table_index = 0;
    uint32 remap_val;
    uint64 action;
    int sr_bits = 0;
    int sl_bits = 0;

    COMPILER_64_ZERO(action);
    n_bytes = (n_bits + 7) / 8;

    /* Generate Remap table index */
    for(ix = 0; ix < n_bytes; ix++) {
        remap_table_index = remap_table_index ^ key[ix];
    }

    /* Generate Action table index */
    for(ix = 0; ix < n_bytes; ix++) {
        nibble_swapped_key = ((key[ix] & 0x0F) << 4) |
                                    ((key[ix] & 0xF0) >> 4 );
        action_table_index = action_table_index ^ nibble_swapped_key;
    }

    /* Read Remap and Action table values from SW cache*/
    if (bank == 1) {
        remap_val = rbh_cfg->remap_data_B[remap_table_index];
        action = rbh_cfg->action_data_B[action_table_index /
                          ROBUST_HASH_ACTION_TABLE_WIDTH];
    } else {
        remap_val = rbh_cfg->remap_data_A[remap_table_index];
        action = rbh_cfg->action_data_A[action_table_index /
                          ROBUST_HASH_ACTION_TABLE_WIDTH];
    }

    /* Robust hash key transformation */
    if (COMPILER_64_BITTEST(action,
                (action_table_index % ROBUST_HASH_ACTION_TABLE_WIDTH)) == 0) {
        /* PREPEND */
        for (ix = n_bytes; ix > 0; ix--) {
            key[ix + 1] = key[ix - 1];
        }
        byte_offset = n_bytes + (ROBUST_HASH_KEY_SPACE_WIDTH / 8);
    } else if (COMPILER_64_BITTEST(action,
                (action_table_index % ROBUST_HASH_ACTION_TABLE_WIDTH)) == 1) {
        /* APPEND */
        key_size = n_bytes;
        byte_offset = n_bytes;
        append = 1;
    }

    /* Trident3 does not have byte boundary aligned robust hash values */
    if ((soc_feature(unit, soc_feature_robust_hash_byte_boundary_alignment)) &&
        (append == 1)){
        key[n_bits / 8] = (remap_val & 0xFF) << (n_bits % 8);
        key[(n_bits / 8) + 1] = ((remap_val & 0xFF) >> (8 - (n_bits % 8))) | ((((remap_val >> 8) & 0xF) |
            ((action_table_index & 0xF) << 4)) << (n_bits % 8));
        if (n_bits % 8) {
            key[(n_bits / 8) + 2] = (((remap_val >> 8) & 0xF) |
            ((action_table_index & 0xF) << 4)) >> (8 - n_bits % 8); 
        }
    } else {
    key[key_size] = remap_val & 0xFF;
    key[key_size + 1] = ((remap_val >> 8) & 0xF) |
        ((action_table_index & 0xF) << 4);
    }

    if (!(soc_feature(unit, soc_feature_robust_hash_byte_boundary_alignment))) {
    /* Adjust the byte boundary */
    if (n_bits % 8 != 0) {
        sr_bits = n_bits % 8;
        sl_bits = 8 - sr_bits;
        for (ix = byte_offset - 1; ix > 0; ix--) {
            key[ix] = key[ix] << sl_bits;
            key[ix] |= key[ix - 1] >> sr_bits;
        }
        key[ix] = key[ix] << sl_bits;
    }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * soc_robust_hash_get2
 * Purpose:
 *      Internal function to compute Robust Hash key transformation
 * Parameters:
 *      unit    - (IN) SOC unit #
 *      rbh_cfg - (IN) Pointer to Robust hash table structure
 *      bank    - (IN) Dual hash bank # (0/1)
 *      key     - (IN/OUT) Hashing key
 *      n_bits  - (IN) Number of bits in the Input hash key
 * Returns:
 *      BCM_E_XXX
*/
int
soc_robust_hash_get2(int unit, soc_robust_hash_config_t *rbh_cfg, int bank,
                    uint8 *key, int n_bits)
{
    int ix, n_bytes, x;
    int append = 0;
    int nibble_swapped_key = 0, key_size = 0;
    int remap_table_index = 0, action_table_index = 0;
    uint32 remap_val;
    uint64 action;
    uint16 temp16;

    COMPILER_64_ZERO(action);
    n_bytes = (n_bits + 7) / 8;

    /* Generate Remap table index */
    for(ix = 0; ix < n_bytes; ix++) {
        remap_table_index = remap_table_index ^ key[ix];
    }

    /* Generate Action table index */
    for(ix = 0; ix < n_bytes; ix++) {
        nibble_swapped_key = ((key[ix] & 0x0F) << 4) |
                                    ((key[ix] & 0xF0) >> 4 );
        action_table_index = action_table_index ^ nibble_swapped_key;
    }

    /* Read Remap and Action table values from SW cache*/
    if (bank == 1) {
        remap_val = rbh_cfg->remap_data_B[remap_table_index];
        action = rbh_cfg->action_data_B[action_table_index /
                          ROBUST_HASH_ACTION_TABLE_WIDTH];
    } else {
        remap_val = rbh_cfg->remap_data_A[remap_table_index];
        action = rbh_cfg->action_data_A[action_table_index /
                          ROBUST_HASH_ACTION_TABLE_WIDTH];
    }

    /* Robust hash key transformation */
    if (COMPILER_64_BITTEST(action,
                (action_table_index % ROBUST_HASH_ACTION_TABLE_WIDTH)) == 0) {
        /* PREPEND */
        for (ix = n_bytes; ix > 0; ix--) {
            key[ix + 1] = key[ix - 1];
        }
    } else if (COMPILER_64_BITTEST(action,
                (action_table_index % ROBUST_HASH_ACTION_TABLE_WIDTH)) == 1) {
        /* APPEND */
        key_size = n_bytes;
        append = 1;
    }

    temp16 = (((remap_val >> 8) & 0xF) << 8) | (remap_val & 0xFF);
    temp16 |= (action_table_index & 0xF) << 12;

    x = n_bits & 0x7; /* ie n_bits % 8 */

    if ((x) && (append == 1)) {
           key[key_size - 1] &= ~((((1 << (8 - x)) - 1)) << x);
           key[key_size - 1] |= (temp16 & ((1 << (8 - x)) - 1)) << x;
           key[key_size] = (temp16 >> (8 - x)) & 0xFF;
           key[key_size + 1] = temp16 >> (16 - x);
    } else {
        key[key_size] = temp16 & 0xFF;
        key[key_size + 1] =  (temp16 >> 8) & 0xFF;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 * soc_robust_hash_table_set
 * Purpose:
 *      Internal function to configure Robust hash remapping structures
 * Parameters:
 *      unit    - (IN) SOC unit #
 *      rbh_cfg - (IN) Pointer to Robust hash table structure
 *      seed    - (IN) Random seed
 * Returns:
 *      BCM_E_XXX
*/
int
soc_robust_hash_table_set(int unit, soc_robust_hash_config_t *rbh_cfg, int seed)
{
    int index, num_tables, action = 0, action_lo;
    unsigned int random_data, swap, remap_rand;
    uint8 action_arr[ROBUST_HASH_ACTION_TABLE_SIZE];
    uint64 val64, action_tab_entry;
    uint32 *remapdata[ROBUST_HASH_NUM_MODULES];
    uint64 *actiondata[ROBUST_HASH_NUM_MODULES];
    COMPILER_64_ZERO(val64);
    COMPILER_64_ZERO(action_tab_entry);

    remapdata[0] = rbh_cfg->remap_data_A;
    remapdata[1] = rbh_cfg->remap_data_B;
    actiondata[0] = rbh_cfg->action_data_A;
    actiondata[1] = rbh_cfg->action_data_B;

    /* Set random seed */
    sal_srand(seed);

    /* Fill remap and action tables with random data */
    for (num_tables = 0; num_tables < ROBUST_HASH_NUM_MODULES; num_tables++) {
        /* Initialize local variable to fill action table with equal number
           of 1s and 0s */
        for (index = 0; index < ROBUST_HASH_ACTION_TABLE_SIZE; index++) {
            action_arr[index] = index % 2;
        }

        for (index = 0; index < ROBUST_HASH_REMAP_TABLE_SIZE; index++) {
            random_data = sal_rand();
            remap_rand = random_data & 0xFFF;

            /* Configure remap table with random data */
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, rbh_cfg->remap_tab[num_tables], MEM_BLOCK_ALL,
                               index, &remap_rand));

            /* Copy remap data to SW cache */
            *(remapdata[num_tables] + index) = remap_rand;

            /* Randomize action variable values */
            swap = action_arr[index];
            action_arr[index] = action_arr[random_data %
                                           ROBUST_HASH_ACTION_TABLE_SIZE];
            action_arr[random_data % ROBUST_HASH_ACTION_TABLE_SIZE] = swap;
        }

        /* Write randomized action table values to memory */
        action_lo = 0;
        for (index = 0; index < ROBUST_HASH_ACTION_TABLE_SIZE; index++) {
            action |= action_arr[index] <<
                                (index % (ROBUST_HASH_ACTION_TABLE_WIDTH / 2));
            if ((index + 1) % (ROBUST_HASH_ACTION_TABLE_WIDTH / 2) == 0) {
                if ((index + 1) % ROBUST_HASH_ACTION_TABLE_WIDTH == 0) {
                    COMPILER_64_SET(val64, action, action_lo);
                    /* Action table protected with ECC/Parity are       *
                     * more than 64 bit wide., Code to accommodate ECC. *
                     * Action vector is still 64bit wide                */
                    if (SOC_MEM_FIELD_VALID(unit, rbh_cfg->action_tab[num_tables], ECCPf)) {
                        uint64 act_tab_entry[2];
                        uint32 eccp = 0;
                        sal_memset(act_tab_entry, 0, sizeof(act_tab_entry));
                        soc_mem_field64_set(unit, rbh_cfg->action_tab[num_tables],
                                            (uint64 *)act_tab_entry,
                                            ACTIONf, val64);
                        soc_mem_field32_set(unit, rbh_cfg->action_tab[num_tables],
                                            (uint64 *)act_tab_entry,
                                            ECCPf, eccp);
                        SOC_IF_ERROR_RETURN
                            (soc_mem_write(unit, rbh_cfg->action_tab[num_tables],
                                           MEM_BLOCK_ALL,
                                           index / ROBUST_HASH_ACTION_TABLE_WIDTH,
                                           act_tab_entry));
                    } else {
                        soc_mem_field64_set(unit, rbh_cfg->action_tab[num_tables],
                                            (uint64 *)&action_tab_entry,
                                            ACTIONf, val64);
                        SOC_IF_ERROR_RETURN
                            (soc_mem_write(unit, rbh_cfg->action_tab[num_tables],
                                           MEM_BLOCK_ALL,
                                           index / ROBUST_HASH_ACTION_TABLE_WIDTH,
                                           &action_tab_entry));
                    }
                    /* Copy action data to SW cache */
                    COMPILER_64_SET(*(actiondata[num_tables] +
                                    (index / ROBUST_HASH_ACTION_TABLE_WIDTH)),
                                    action, action_lo);

                    action = 0;
                    action_lo = 0;
                } else {
                    action_lo = action;
                    action = 0;
                }
            }
        }
    }

    return SOC_E_NONE;
}
#endif /* SOC_ROBUST_HASH */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
int
soc_td2x_th_l2x_hash(int unit, uint32 *base_entry, int *num_banks,
                     int *bucket_array, int *index_array, uint8 *hash_key)
{
    int bank            = 0;
    int bucket          = 0;
    int entries_per_row = 0;
    int bank_base       = 0;
    int bucket_offset   = 0;

    if (NULL == base_entry || NULL == num_banks ||
        NULL == bucket_array || NULL == index_array || NULL == hash_key) {
        return SOC_E_PARAM;
    }

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_apache_hash_bank_count_get(unit, L2Xm, num_banks));
        (void) soc_ap_l2x_base_entry_to_key(unit, base_entry, hash_key);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_helix5_hash_bank_count_get(unit, L2Xm, num_banks));
        (void) soc_hx5_l2x_base_entry_to_key(unit, 0, base_entry, hash_key);
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_trident3_hash_bank_count_get(unit, L2Xm, num_banks));
       (void) soc_td3_l2x_base_entry_to_key(unit, 0, base_entry, hash_key);
    } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_hash_bank_count_get(unit, L2Xm, num_banks));
       (void) soc_tomahawk3_l2x_base_entry_to_key(unit, 0, base_entry, hash_key);
    } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_trident2_hash_bank_count_get(unit, L2Xm, num_banks));
       (void) soc_td2_l2x_base_entry_to_key(unit, 0, base_entry, hash_key);
    } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_hash_bank_count_get(unit, L2Xm, num_banks));
        (void) soc_th_l2x_base_entry_to_key(unit, 0, base_entry, hash_key);
    } else
#endif
    {
       return SOC_E_INTERNAL;
    }

    for (bank = 0; bank < *num_banks; bank++) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_apache_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_apache_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                              &entries_per_row, NULL,
                                              &bank_base, &bucket_offset));
            bucket = soc_ap_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)|| SOC_IS_FIREBOLT6(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_hx5_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_hx5_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_hx5_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_td3_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_td3_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_td3_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_tomahawk3_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_tomahawk3_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_trident2_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_trident2_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_td2_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            int phy_bank;
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_tomahawk_hash_bank_info_get(unit, L2Xm, phy_bank, NULL,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_th_l2x_bank_entry_hash(unit, phy_bank, base_entry);
        }
#endif
       bucket_array[bank] = bucket;
       index_array[bank]  = bank_base + bucket * entries_per_row + bucket_offset;
    }

    return SOC_E_NONE;
}

int
soc_td2x_th_l3x_hash(int unit, soc_mem_t mem, uint32 *base_entry, int *num_banks,
                     int *bucket_array, int *index_array, int *phy_bank_array,
                     int *entry_num_array)
{
    int bank             = 0;
    int phy_bank         = 0;
    int bucket           = 0;
    int entries_per_bank = 0;
    int entries_per_row  = 0;
    int bank_base        = 0;
    int bucket_offset    = 0;

    if (NULL == base_entry || NULL == num_banks ||
        NULL == bucket_array || NULL == index_array) {
        return SOC_E_PARAM;
    }

    switch (mem) {
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
        break;
    default:
        return SOC_E_PARAM;
    }
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_helix5_hash_bank_count_get(unit, mem, num_banks));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_trident3_hash_bank_count_get(unit, mem, num_banks));
    } else
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_apache_hash_bank_count_get(unit, mem, num_banks));
    } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_hash_bank_count_get(unit, mem, num_banks));
    } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_trident2_hash_bank_count_get(unit, mem, num_banks));
    } else
#endif
    {
       return SOC_E_INTERNAL;
    }

    for (bank = 0; bank < *num_banks; bank++) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_hx5_hash_bank_number_get(unit, mem, bank, &phy_bank));
                SOC_IF_ERROR_RETURN(
                        soc_hx5_hash_bank_info_get(unit, mem, phy_bank, &entries_per_bank,
                            &entries_per_row, NULL,
                            &bank_base, &bucket_offset));
                bucket = soc_hx5_l3x_bank_entry_hash(unit, phy_bank, base_entry);
            } else
#endif
            {
                SOC_IF_ERROR_RETURN
                    (soc_td3_hash_bank_number_get(unit, mem, bank, &phy_bank));
                SOC_IF_ERROR_RETURN(
                        soc_td3_hash_bank_info_get(unit, mem, phy_bank, &entries_per_bank,
                            &entries_per_row, NULL,
                            &bank_base, &bucket_offset));
                bucket = soc_td3_l3x_bank_entry_hash(unit, phy_bank, base_entry);
            }
 
            /* Bucket is calculated based on SINGLE memory. In case of
             * Double and Quad, the hash bucket to be adjusted correctly.
             * If there are 4096 entires in the bank and 4 entries per row
             * (or bucket), there are 1024 buckets and the mask is 0x3FF.
             */
            bucket &= ((entries_per_bank/entries_per_row)-1);
        } else
#endif
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_apache_hash_bank_number_get(unit, mem, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_apache_hash_bank_info_get(unit, mem, phy_bank, &entries_per_bank,
                                              &entries_per_row, NULL,
                                              &bank_base, &bucket_offset));
            bucket = soc_ap_l3x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_hash_bank_number_get(unit, mem, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_tomahawk_hash_bank_info_get(unit, mem, phy_bank, &entries_per_bank,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_th_l3x_bank_entry_hash(unit, phy_bank, base_entry);
        } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_trident2_hash_bank_number_get(unit, mem, bank, &phy_bank));
            SOC_IF_ERROR_RETURN(
                soc_trident2_hash_bank_info_get(unit, mem, phy_bank, &entries_per_bank,
                                                &entries_per_row, NULL,
                                                &bank_base, &bucket_offset));
            bucket = soc_td2_l3x_bank_entry_hash(unit, phy_bank, base_entry);
        }
#endif
        bucket_array[bank] = bucket;
        index_array[bank]  = bank_base + bucket * entries_per_row + bucket_offset;
        if (phy_bank_array) {
            phy_bank_array[bank]  = phy_bank;
        }
        if (entry_num_array) {
            entry_num_array[bank] = entries_per_bank;
        }
    }

    return SOC_E_NONE;
}
#endif

#endif /* BCM_XGS_SWITCH_SUPPORT */
