/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Greyhound2 hash table calculation routines
 * Requires:
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <shared/bsl.h>

#if defined(BCM_GREYHOUND2_SUPPORT)

#include <soc/greyhound2.h>

STATIC int
_soc_gh2_hash_generic_entry_to_key(
    int unit,
    void *entry,
    uint8 *key,
    soc_mem_t mem,
    soc_field_t *field_list)
{
    soc_field_t field;
    int index, key_index, val_index, fval_index;
    int right_shift_count, left_shift_count;
    uint32 val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int bits, val_bits, fval_bits;
    int8 field_length[16];

    val_bits = 0;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        field_length[index] = soc_mem_field_length(unit, mem, field);
        val_bits += field_length[index];
    }

    switch (mem) {
        case L2Xm:
            val_bits = 63;
            break;
        case VLAN_XLATEm:
            val_bits = soc_mem_field_length(unit, mem, KEY_TYPEf) +
                       soc_mem_field_length(unit, mem, KEYf); /* 44 */
            break;
        case VLAN_MACm:
            val_bits = soc_mem_field_length(unit, mem, KEY_TYPEf) +
                       soc_mem_field_length(unit, mem, KEYf); /* 52 */
            break;
        case EGR_VLAN_XLATEm:
            val_bits = soc_mem_field_length(unit, mem, ENTRY_TYPEf) +
                       soc_mem_field_length(unit, mem, KEYf); /* 42 */
            break;
        default:
            break;
    }

    bits = (val_bits + 7) & ~0x7;
    sal_memset(val, 0, sizeof(val));
    val_bits = bits - val_bits;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        soc_mem_field_get(unit, mem, entry, field, fval);
        fval_bits = field_length[index];
        /* Firelight VFIf has only 10 bits (less then GH2 for 2 bits) */
        if (SOC_IS_FIRELIGHT(unit) && (mem == L2Xm) &&
            (field_list[index] == VFIf)) {
            if (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf) ==
                GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST) {
                fval_bits += 2;
            }
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

int
soc_gh2_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int i;
    soc_field_t field_list[4];

    for (i=0; i < 4 ; i++) {
        field_list[i] = INVALIDf;
    }

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
        case GH2_L2_HASH_KEY_TYPE_BRIDGE:
            field_list[0] = KEY_TYPEf;
            field_list[1] = VLAN_IDf;
            field_list[2] = MAC_ADDRf;
            break;
        case GH2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
            field_list[0] = KEY_TYPEf;
            field_list[1] = OVIDf;
            break;
        case GH2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            field_list[0] = KEY_TYPEf;
            field_list[1] = OVIDf;
            field_list[2] = IVIDf;
            break;
        case GH2_L2_HASH_KEY_TYPE_VFI:
        case GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST:
            field_list[0] = KEY_TYPEf;
            field_list[1] = VFIf;
            field_list[2] = MAC_ADDRf;
            break;
        case GH2_L2_HASH_KEY_TYPE_VIF:
            field_list[0] = VIF__KEYf;
            break;
        case GH2_L2_HASH_KEY_TYPE_PE_VID:
            field_list[0] = PE_VID__KEYf;
            break;
        default:
            return 0;
    }

    return _soc_gh2_hash_generic_entry_to_key(unit, entry, key,
                                              L2Xm, field_list);
}

int
soc_gh2_vlan_xlate_base_entry_to_key(
    int unit,
    uint32 *entry,
    uint8 *key)
{
    int i;
    soc_field_t field_list[5];
    soc_mem_t mem;

    for (i=0; i < 5; i++) {
        field_list[i] = INVALIDf;
    }

    field_list[0] = KEY_TYPEf;
    field_list[1] = GLPf;
    mem = VLAN_XLATEm;

    switch (soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf)) {
        case GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
        case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
            field_list[2] = OVIDf;
            field_list[3] = IVIDf;
            field_list[4] = INVALIDf;
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_OTAG:
        case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
            field_list[2] = OVIDf;
            field_list[3] = INVALIDf;
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_ITAG:
        case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
            field_list[2] = IVIDf;
            field_list[3] = INVALIDf;
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_OVID:
        case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
            field_list[2] = OVIDf;
            field_list[3] = IVIDf;
            field_list[4] = INVALIDf;
            soc_mem_field32_set(unit, mem, entry, IVIDf, 0);
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_IVID:
        case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
            field_list[2] = OVIDf;
            field_list[3] = IVIDf;
            field_list[4] = INVALIDf;
            soc_mem_field32_set(unit, mem, entry, OVIDf, 0);
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            field_list[2] = OVIDf;
            field_list[3] = INVALIDf;
            soc_mem_field32_set(unit, mem, entry, OVIDf, 0);
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_VNID:
            field_list[2] = VNIDf;
            field_list[3] = INVALIDf;
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            mem = VLAN_MACm;
            field_list[1] = MAC_ADDRf;
            field_list[2] = INVALIDf;
            break;
        case GH2_VLXLT_HASH_KEY_TYPE_HPAE:
            mem = VLAN_MACm;
            field_list[1] = MAC_IP_BIND__SIPf;
            field_list[2] = INVALIDf;
            break;
        default:
            return 0;
    }

    return _soc_gh2_hash_generic_entry_to_key(unit,
                                                           entry, key, mem, field_list);
}

uint32
soc_gh2_vlan_xlate_hash(
    int unit,
    int hash_sel,
    int key_nbits,
    void *base_entry,
    uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32 mask;
        int bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 3;
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
            switch (soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, KEY_TYPEf)) {
                case GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_OTAG:
                case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_ITAG:
                case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, ITAGf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_OVID:
                case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_IVID:
                case GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, IVIDf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                    /* Use only the upper 4 bit of OTAG */
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf) >> 12;
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_VNID:
                    rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, VNIDf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                    rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                             MAC__MAC_ADDRf);
                    break;
                case GH2_VLXLT_HASH_KEY_TYPE_HPAE:
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
                                  "soc_gh2_vlan_xlate_hash: invalid hash_sel %d\n"),
                       hash_sel));
            rv = 0;
            break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}
#endif /* BCM_GREYHOUND2_SUPPORT */

