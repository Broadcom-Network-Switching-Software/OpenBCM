/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Katana2 hash table calculation routines
 * Requires:
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <shared/bsl.h>
#if defined(BCM_KATANA2_SUPPORT)

#include <soc/katana2.h>


STATIC int
_soc_kt2_hash_generic_entry_to_key(int unit, void *entry, uint8 *key,
                                   soc_mem_t mem, soc_field_t *field_list)
{
    soc_field_t field;
    int         index, key_index, val_index, fval_index;
    int         right_shift_count, left_shift_count;
    uint32      val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int         bits, val_bits, fval_bits;
    int8        field_length[16];

    val_bits = 0;
    for (index = 0; field_list[index] != INVALIDf; index++) {
      field = field_list[index];
      field_length[index] = soc_mem_field_length(unit, mem, field);
#ifdef BCM_METROLITE_SUPPORT
      if ((field == L3_IIFf) && SOC_IS_METROLITE(unit)) {
        /* L3_IIF field is 9 bits but hash key considers it as 13 bits */
        field_length[index] += 4; /*13 - 9*/
      }
#endif
      val_bits += field_length[index];
    }

    switch (mem) {
    case L2Xm:
        val_bits = soc_mem_field_length(unit, L2Xm, L2__KEYf);
            break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        val_bits = 274;
#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            val_bits = 271;
        }
#endif
        break;
    case VLAN_XLATEm:
    case VLAN_MACm:
        val_bits = 52;
        break;
    case EGR_VLAN_XLATEm:
        val_bits = 42;
        break;
    case MPLS_ENTRYm:
        val_bits = 63;
        break;
    default:
        break;
    }

    bits = (val_bits + 7) & ~0x7;
    sal_memset(val, 0, sizeof(val));
    val_bits = bits - val_bits;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
#ifdef BCM_METROLITE_SUPPORT
        sal_memset(fval, 0, sizeof(fval));
#endif
        soc_mem_field_get(unit, mem, entry, field, fval);
        fval_bits = field_length[index];

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

int
soc_kt2_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case KT2_L2_HASH_KEY_TYPE_BRIDGE:
    case KT2_L2_HASH_KEY_TYPE_VFI:
        field_list[0] = L2__KEYf;
        break;
    case KT2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
    case KT2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        field_list[0] = VLAN__KEYf;
        break;
    case KT2_L2_HASH_KEY_TYPE_VIF:
        field_list[0] = VIF__KEYf;
        break;
    case KT2_L2_HASH_KEY_TYPE_BFD:
        field_list[0] = BFD__KEYf;
        break;
    case KT2_L2_HASH_KEY_TYPE_PE_VID:
        field_list[0] = PE_VID__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_kt2_hash_generic_entry_to_key(unit, entry, key, L2Xm,
                                              field_list);
}


int
soc_kt2_l3x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[8];
    void *ptr;

    ptr = entry;

    switch (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, entry, KEY_TYPEf)) {
    case KT2_L3_HASH_KEY_TYPE_V4UC:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = IP_ADDRf;
        field_list[1] = VRF_IDf;
        field_list[2] = KEY_TYPEf;
        field_list[3] = INVALIDf;
        break;
    case KT2_L3_HASH_KEY_TYPE_V4MC:
        mem = L3_ENTRY_IPV4_MULTICASTm;
        field_list[0] = GROUP_IP_ADDRf;
        field_list[1] = SOURCE_IP_ADDRf;
        field_list[2] = L3_IIFf;
        field_list[3] = VRF_IDf;
        field_list[4] = KEY_TYPE_0f;
        field_list[5] = INVALIDf;
        break;
    case KT2_L3_HASH_KEY_TYPE_V6UC:
        mem = L3_ENTRY_IPV6_UNICASTm;
        field_list[0] = IP_ADDR_LWR_64f;
        field_list[1] = IP_ADDR_UPR_64f;
        field_list[2] = VRF_IDf;
        field_list[3] = KEY_TYPE_0f;
        field_list[4] = INVALIDf;
        break;
    case KT2_L3_HASH_KEY_TYPE_V6MC:
        mem = L3_ENTRY_IPV6_MULTICASTm;
        field_list[0] = GROUP_IP_ADDR_LWR_64f;
        field_list[1] = GROUP_IP_ADDR_UPR_56f;
        field_list[2] = SOURCE_IP_ADDR_LWR_64f;
        field_list[3] = SOURCE_IP_ADDR_UPR_64f;
        field_list[4] = L3_IIFf;
        field_list[5] = VRF_IDf;
        field_list[6] = KEY_TYPE_0f;
        field_list[7] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_kt2_hash_generic_entry_to_key(unit, ptr, key, mem, field_list);
}


int
soc_kt2_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[8];

    switch (soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf)) {
    case KT2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
    case KT2_VLXLT_HASH_KEY_TYPE_OTAG:
    case KT2_VLXLT_HASH_KEY_TYPE_ITAG:
    case KT2_VLXLT_HASH_KEY_TYPE_OVID:
    case KT2_VLXLT_HASH_KEY_TYPE_IVID:
    case KT2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
        mem = VLAN_XLATEm;
        field_list[0] = XLATE__KEYf;
        field_list[1] = INVALIDf;
        break;
    case KT2_VLXLT_HASH_KEY_TYPE_VIF:
    case KT2_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
    case KT2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
    case KT2_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
    case KT2_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
        mem = VLAN_XLATEm;
        field_list[0] = VIF__KEYf;
        field_list[1] = INVALIDf;
        break;

    case KT2_VLXLT_HASH_KEY_TYPE_LLTAG_VID:
    case KT2_VLXLT_HASH_KEY_TYPE_LLVID_IVID:
    case KT2_VLXLT_HASH_KEY_TYPE_LLVID_OVID:
        mem = VLAN_XLATEm;
        field_list[0] = LLTAG__KEYf;
        field_list[1] = INVALIDf;
        break;
    case KT2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
        mem = VLAN_MACm;
        field_list[0] = MAC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case KT2_VLXLT_HASH_KEY_TYPE_HPAE:
        mem = VLAN_MACm;
        field_list[0] = MAC_IP_BIND__KEYf;
        field_list[1] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_kt2_hash_generic_entry_to_key(unit, entry, key, mem,
                                              field_list);
}

int
soc_kt2_egr_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf)) {
    case KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
    case KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
        field_list[0] = XLATE__KEYf;
        break;
    case KT2_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
    case KT2_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
        field_list[0] = MIM_ISID__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_kt2_hash_generic_entry_to_key(unit, entry, key,
                                              EGR_VLAN_XLATEm, field_list);
}

int
soc_kt2_mpls_base_entry_to_key(int unit, void *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, MPLS_ENTRYm, entry, KEY_TYPEf)) {
    case KT2_MPLS_HASH_KEY_TYPE_MPLS:
        field_list[0] = MPLS__KEYf;
        break;
    case KT2_MPLS_HASH_KEY_TYPE_MIM_NVP:
        field_list[0] = MIM_NVP__KEYf;
        break;
    case KT2_MPLS_HASH_KEY_TYPE_MIM_ISID:
    case KT2_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
        field_list[0] = MIM_ISID__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_kt2_hash_generic_entry_to_key(unit, entry, key, MPLS_ENTRYm,
                                              field_list);
}

uint32
soc_kt2_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                        void *base_entry, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 16 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 4;
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
#ifdef BCM_SABER2_SUPPORT
        /* Saber2 variants has VLAN_XLATE limited by bond option,
            use the original 16K = 10 bits
        */
        if (SOC_IS_SABER2(unit)) {
            bits = 10;
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

        case KT2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_OTAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_ITAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, ITAGf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_OVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_IVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, IVIDf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            /* Use only the upper 4 bit of OTAG */
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf) >>
                12;
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VIF:
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
        case KT2_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     VIF__SRC_VIFf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_LLTAG_VID:
        case KT2_VLXLT_HASH_KEY_TYPE_LLVID_IVID:
        case KT2_VLXLT_HASH_KEY_TYPE_LLVID_OVID:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     LLTAG__LLVIDf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC__MAC_ADDRf);
            break;
        case KT2_VLXLT_HASH_KEY_TYPE_HPAE:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC_IP_BIND__SIPf);
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
                              "soc_kt2_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

uint32
soc_kt2_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                        void *base_entry, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate == 0) {
        uint32  mask;
        int     bits;

        /* 16 Entries per bucket */
        mask = soc_mem_index_max(unit, EGR_VLAN_XLATEm) >> 4;
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
        switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                        ENTRY_TYPEf)) {
            case KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
            case KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         OVIDf);
                break;
            case KT2_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
            case KT2_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
                rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         MIM_ISID__VFIf) |
                    (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                         MIM_ISID__DVPf) <<
                     soc_mem_field_length(unit, EGR_VLAN_XLATEm,
                                          MIM_ISID__VFIf));
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
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_kt2_egr_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate;
}

#endif /* BCM_KATANA2_SUPPORT */
