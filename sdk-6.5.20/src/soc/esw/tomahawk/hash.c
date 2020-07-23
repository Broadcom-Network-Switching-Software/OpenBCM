/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Tomahawk hash table calculation routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_TOMAHAWK_SUPPORT)

#include <soc/tomahawk.h>

/* Get number of banks for the hash table according to the config */
int
soc_tomahawk_hash_bank_count_get(int unit, soc_mem_t mem, int *num_banks)
{
    int count;

    switch (mem) {
    case L2Xm:
        /*
         * 4k entries in each of 2 dedicated L2 bank
         * 32k(TH)/64k(TH2) entries in each shared bank
         */
        count = soc_mem_index_count(unit, L2Xm);
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            *num_banks = 2 + (count - 2 * 4 * 1024) / (64 * 1024);
        } else
#endif
        {
            *num_banks = 2 + (count - 2 * 4 * 1024) / (32 * 1024);
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        /*
         * 2k entries in each of 4 dedicated L3 bank
         * 32k(TH)/64k(TH2) entries in each shared bank
         */
        count = soc_mem_index_count(unit, L3_ENTRY_ONLYm);
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            *num_banks = 4 + (count - 4 * 2 * 1024) / (64 * 1024);
        } else
#endif
        {
            *num_banks = 4 + (count - 4 * 2 * 1024) / (32 * 1024);
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
        /*
         * 16k(TH)/32k(TH2) entries in each shared bank
         */
        count = soc_mem_index_count(unit, mem);
        if (SOC_IS_TOMAHAWK2(unit)) {
            *num_banks = count / (32 * 1024);
        } else {
            *num_banks = count / (16 * 1024);
        }
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:
         /*
                 * 8k(TH)/16K(TH2) em4 entries in each shared bank
                 */
        count = soc_mem_index_count(unit, mem);
        if (SOC_IS_TOMAHAWK2(unit)) {
            *num_banks = count / (16 * 1024);
        } else {
            *num_banks = count / (8 * 1024);
        }        
        break;
    case MPLS_ENTRYm:
    case VLAN_XLATEm:
    case VLAN_MACm:
    case EGR_VLAN_XLATEm:
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
        *num_banks = 2;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/* Map logical to physical bank mapping for a memory
 *  mem         - memory 
 *  log_bank    - logical bank number (or sequence bank number)
 *  phy_bank_id - Physical bank id 
 */
int
soc_tomahawk_log_to_phy_bank_map(int unit, soc_mem_t mem, int log_bank,
                                 int *phy_bank_id)
{
    uint32 val;
    static const soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };

    SOC_IF_ERROR_RETURN(soc_th_iss_log_to_phy_bank_map_shadow_get(unit, &val));
    switch (mem) {
    case L2Xm:
        if (log_bank < 0 || log_bank > 5) {
            return SOC_E_PARAM;
        }
        if (log_bank < 2) {
            *phy_bank_id = log_bank;
        } else {
            /* Starts with 0 in the register which maps to Physical bank 2 */
            *phy_bank_id = 2 + soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                          l2_fields[log_bank - 2]);
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        if (log_bank < 0 || log_bank > 7) {
            return SOC_E_PARAM;
        }
        if (log_bank < 4) {
            *phy_bank_id = 6 + log_bank; /* dedicated banks */
        } else {
            /* Starts with 0 in the register which maps to Physical bank 2 */
            *phy_bank_id = 2 + soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                          l3_fields[log_bank - 4]);
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:
        if (log_bank < 0 || log_bank > 3) {
            return SOC_E_PARAM;
        }
        else {
            /* Starts with 0 in the register which maps to Physical bank 2 */
            *phy_bank_id = 2 + soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                                                 fpem_fields[log_bank]);
        }
        break;
    default:
        *phy_bank_id = log_bank;
        break;
    }
    return SOC_E_NONE;
}

/* Map physical to logical bank mapping for a memory
 *  mem         - memory
 *  phy_bank    - physical bank number
 *  log_bank    - logical bank id
 */
int
soc_tomahawk_phy_to_log_bank_map(int unit, soc_mem_t mem, int phy_bank, int *log_bank)
{
    uint32 val, i;
    static const soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };

    SOC_IF_ERROR_RETURN(READ_ISS_LOG_TO_PHY_BANK_MAPr(unit, &val));
    switch (mem) {
    case L2Xm:
        if (phy_bank < 0 || phy_bank > 5) {
            return SOC_E_PARAM;
        }
        if (phy_bank < 2) {
            *log_bank = phy_bank;
        } else {
            for (i = 0; i < 4; i++) {
                /* Starts with 0 in the register which maps to Physical bank 2 */
                if ((soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                                       l2_fields[i]) + 2) == phy_bank) {
                    *log_bank = 2 + i;
                    break;
                }
            }
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        if (phy_bank < 2 || phy_bank > 9) {
            return SOC_E_PARAM;
        }
        if (phy_bank > 5) {
            *log_bank = phy_bank - 6;
        } else {
            for (i = 0; i < 4; i++) {
               /* Starts with 0 in the register which maps to Physical bank 2 */
                if ((soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                                      l3_fields[i]) + 2) == phy_bank) {
                    *log_bank = 4 + i;
                    break;
                }
            }
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:
        if (phy_bank < 2 || phy_bank > 5) {
            return SOC_E_PARAM;
        } else {
            for (i = 0; i < 4; i++) {
                /* Starts with 0 in the register which maps to Physical bank 2 */
                if ((soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, val,
                                       fpem_fields[i]) + 2) == phy_bank) {
                    *log_bank = i;
                    break;
                }
            }
        }
        break;
    default:
        *log_bank = phy_bank;
        break;
    }
    return SOC_E_NONE;
}

/* Get bank bitmap for the hash table according to the config */
int
soc_tomahawk_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem, uint32 *bitmap)
{
    uint32 physical_bitmap = 0;
    int count = 0;
    int i, phy_bank_id = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk_hash_bank_count_get(unit, mem, &count));
    for (i = 0; i < count; i++) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_log_to_phy_bank_map(unit, mem, i, &phy_bank_id));
        physical_bitmap |= (1 << phy_bank_id);
    }
    *bitmap = physical_bitmap;
    return SOC_E_NONE;
}

/*
 * Map bank sequence number to bank number.
 *     Sequence number: 0 .. (number of bank - 1)
 *     bank number: bank id in hardware
 * This is only useful for L3 table which has non-sequential bank number,
 * sequence number equals to bank number for all other tables.
 */
int
soc_tomahawk_hash_bank_number_get(int unit, soc_mem_t mem, int seq_num,
                                  int *bank_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_tomahawk_hash_bank_count_get(unit, mem, &count));
    if (seq_num < 0 || seq_num >= count) {
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_tomahawk_log_to_phy_bank_map(unit, mem, seq_num, bank_num));

    return SOC_E_NONE;
}

/*
 * Map bank bank number to sequence number.
 *     Sequence number: 0 .. (number of bank - 1)
 *     bank number: bank id in hardware
 * This is only useful for L3 table which has non-sequential bank number,
 *
 * L3 seq_num   bank_num
 *          0          6
 *          1          7
 *          2          8
 *          3          9
 *          4          5
 *          5          4
 */
int
soc_tomahawk_hash_seq_number_get(int unit, soc_mem_t mem, int bank_num,
                                 int *seq_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_tomahawk_phy_to_log_bank_map(unit, mem, bank_num, seq_num));

    SOC_IF_ERROR_RETURN(soc_tomahawk_hash_bank_count_get(unit, mem, &count));
    if (*seq_num < 0 || *seq_num >= count) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;

}

int
soc_tomahawk_hash_bank_info_get(int unit, soc_mem_t mem, int bank,
                                int *entries_per_bank, int *entries_per_row,
                                int *entries_per_bucket, int *bank_base,
                                int *bucket_offset)
{
    int bank_size, row_size, bucket_size, base, offset;
    int seq_num = 0;

    /* Get logical bank number */
    SOC_IF_ERROR_RETURN(soc_tomahawk_phy_to_log_bank_map(unit, mem, bank, &seq_num));
    bank = seq_num;

    /*
     * entry index for bucket b, entry e =
     *     bank_base + b * entries_per_row + bucket_offset + e;
     */
    switch (mem) {
    case L2Xm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0: index      0-  4095 (4k entries in dedicated bank 0)
         * bank 1: index   4096-  8191 (4k entries in dedicated bank 1)
         * bank 2: index   8192- 40959 (32k entries in shared bank)
         * bank 3: index  40960- 73727 (32k entries in shared bank)
         * bank 4: index  73728-106495 (32k entries in shared bank)
         * bank 5: index 106496-139263 (32k entries in shared bank)
         * each bucket has 4 entries
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 5) {
            return SOC_E_INTERNAL;
        } else if (bank < 2) { /* 4k entries for each dedicated bank */
            bank_size = 4096;
            base = bank * bank_size;
        } else { /* 32k entries for each shared bank */
            bank_size = SOC_IS_TOMAHAWK2(unit) ? 65536 : 32768;
            base = 8192 + (bank - 2) * bank_size;
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0: index      0-  2047 (2k entries in dedicated bank 6)
         * bank 1: index   2048-  4095 (2k entries in dedicated bank 7)
         * bank 2: index   4096-  6143 (2k entries in dedicated bank 8)
         * bank 3: index   6144-  8191 (2k entries in dedicated bank 9)
         * bank 4: index   8192- 40959 (32k entries in shared bank)
         * bank 5: index  40960- 73727 (32k entries in shared bank)
         * bank 6: index  73728-106495 (32k entries in shared bank)
         * bank 7: index 106496-139263 (32k entries in shared bank)
         * each bucket has 4 entries
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 7) {
            return SOC_E_INTERNAL;
        } else if (bank < 4) { /* 2k entriess for each dedicated bank */
            bank_size = 2048;
            base = bank * bank_size;
        } else { /* 32k entries for each shared bank */
            bank_size = SOC_IS_TOMAHAWK2(unit) ? 65536 : 32768;
            base = 8192 + (bank - 4) * bank_size;
        }
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
        row_size = 2;
        bucket_size = 2;
        offset = 0;
        if (bank < 0 || bank > 7) {
            return SOC_E_INTERNAL;
        } else if (bank < 4) { /* 2k entriess for each dedicated bank */
            bank_size = 1024;
            base = bank * bank_size;
        } else { /* 32k entries for each shared bank */
            bank_size = SOC_IS_TOMAHAWK2(unit) ? 32768 : 16384;
            base = 4096 + (bank - 4) * bank_size;
        }
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
        row_size = 1;
        bucket_size = 1;
        offset = 0;
        if (bank < 0 || bank > 7) {
            return SOC_E_INTERNAL;
        } else if (bank < 4) { /* 2k entriess for each dedicated bank */
            bank_size = 512;
            base = bank * bank_size;
        } else { /* 32k entries for each shared bank */
            bank_size = SOC_IS_TOMAHAWK2(unit) ? 16384 : 8192;
            base = 2048 + (bank - 4) * bank_size;
        }
        break;
    case EXACT_MATCH_2m:
        /* for TH
         * bank 0: index      0- 16383 (16k entries in shared bank)
         * bank 1: index  16384- 32767 (16k entries in shared bank)
         * bank 2: index  32768- 49151 (16k entries in shared bank)
         * bank 3: index  49152- 65535 (16k entries in shared bank)
         * each bucket has 2 sram entries
         */
        bank_size = SOC_IS_TOMAHAWK2(unit) ? 32768 : 16384;
        row_size = 2;
        bucket_size = 4;
        base = bank * bank_size;
        offset = 0;
        break;
    case EXACT_MATCH_4m:
        /* for TH
         * bank 0: index      0- 16383 (16k entries in shared bank)
         * bank 1: index  16384- 32767 (16k entries in shared bank)
         * bank 2: index  32768- 49151 (16k entries in shared bank)
         * bank 3: index  49152- 65535 (16k entries in shared bank)
         * each bucket has 1 sram entry
         */
        bank_size = SOC_IS_TOMAHAWK2(unit) ? 16384 : 8192;
        row_size = 1;
        bucket_size = 4;
        base = bank * bank_size;
        offset = 0;
        break;        
    case MPLS_ENTRYm:
    case VLAN_XLATEm:
    case VLAN_MACm:
    case EGR_VLAN_XLATEm:
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
        /*
         * first half of entries in each row are for bank 0
         * the other half of entries in each row are for bank 1
         */
        bank_size = soc_mem_index_count(unit, mem) / 2;
        row_size = 8;
        bucket_size = 4;
        base = 0;
        offset = bank * bucket_size;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    if (entries_per_bank != NULL) {
        *entries_per_bank = bank_size;
    }
    if (entries_per_row != NULL) {
        *entries_per_row = row_size;
    }
    if (entries_per_bucket != NULL) {
        *entries_per_bucket = bucket_size;
    }
    if (bank_base != NULL) {
        *bank_base = base;
    }
    if (bucket_offset != NULL) {
        *bucket_offset = offset;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th_hash_entry_to_key(int unit, int bank, void *entry, uint8 *key, soc_mem_t mem,
                          soc_field_t *field_list)
{
    soc_field_t field;
    int         index, key_index, val_index, fval_index;
    int         right_shift_count, left_shift_count;
    uint32      val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int         bits, val_bits, fval_bits;
    int8        field_length[16];
#ifdef SOC_ROBUST_HASH
    soc_robust_hash_config_t *rbh_cfg = NULL;
#endif /* SOC_ROBUST_HASH */

    val_bits = 0;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        field_length[index] = soc_mem_field_length(unit, mem, field);
        val_bits += field_length[index];
    }

    switch (mem) {
    case L2Xm:
        val_bits = soc_mem_field_length(unit, L2Xm,
                                        TRILL_NONUC_NETWORK_LONG__KEYf);
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
        val_bits = soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                        IPV6MC__KEY_0f) +
            soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                 IPV6MC__KEY_1f) +
            soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                 IPV6MC__KEY_2f) +
            soc_mem_field_length(unit, L3_ENTRY_IPV6_MULTICASTm,
                                 IPV6MC__KEY_3f);
        break;
    case EXACT_MATCH_2m:
        val_bits = soc_mem_field_length(unit, EXACT_MATCH_4m,
                                        MODE320__KEY_0f) +
            soc_mem_field_length(unit, EXACT_MATCH_4m, MODE320__KEY_1f) +
            soc_mem_field_length(unit, EXACT_MATCH_4m, MODE320__KEY_2f) +
            soc_mem_field_length(unit, EXACT_MATCH_4m, MODE320__KEY_3f);
        break;
    case VLAN_XLATEm:
    case VLAN_MACm:
        val_bits = soc_mem_field_length(unit, VLAN_MACm, MAC_PORT__KEYf);
        break;
    case EGR_VLAN_XLATEm:
        val_bits = soc_mem_field_length(unit, EGR_VLAN_XLATEm, XLATE__KEYf);
        break;
    case MPLS_ENTRYm:
        val_bits = soc_mem_field_length(unit, MPLS_ENTRYm, L2GRE_VPNID__KEYf);
        break;
    default:
        break;
    }

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        switch (mem) {
        case VLAN_XLATEm:
        case VLAN_MACm:
            rbh_cfg = &(ROBUSTHASH(unit)->ing_xlate);
            break;
        case MPLS_ENTRYm:
            rbh_cfg = &(ROBUSTHASH(unit)->mpls_entry);
            break;
        case EGR_VLAN_XLATEm:
            rbh_cfg = &(ROBUSTHASH(unit)->egr_xlate);
            break;
        default:
            break;
        }
    }
    /* If robust hash is enabled, byte boundry is adjusted after robust
       hash key computation */
    if (rbh_cfg && rbh_cfg->enable) {
        bits = val_bits;
        val_bits = 0;
    } else
#endif /* SOC_ROBUST_HASH */
    {
        bits = (val_bits + 7) & ~0x7;
        val_bits = bits - val_bits;
    }

    sal_memset(val, 0, sizeof(val));
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
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

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        if (rbh_cfg) {
            if (rbh_cfg->enable) {
                (void) soc_robust_hash_get(unit, rbh_cfg, bank, key, bits);
            }
            /* crc generator has the same key length irrespective
             * of whether robust hash is enabled
             */
            bits = (bits + 7) & ~0x7;
            bits += ROBUST_HASH_KEY_SPACE_WIDTH;
        }
    }
#endif /* SOC_ROBUST_HASH */

    return bits;
}

/* For non-UFT style hash (all tables except L2 and L3) */
STATIC int
soc_th_hash_sel_get(int unit, soc_mem_t mem, int bank, int *hash_sel)
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
    case ING_VP_VLAN_MEMBERSHIPm:
        reg = ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr;
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        reg = EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr;
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        reg = ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    *hash_sel = soc_reg_field_get(unit, reg, rval, field);

    return SOC_E_NONE;
}

/* For UFT style hash (L2 and L3 table) */
int
soc_th_hash_offset_get(int unit, soc_mem_t mem, int bank, int *hash_offset,
                       int *use_lsb)
{
    uint32 rval;
    int is_shared = 0;
    soc_reg_t reg;
    soc_field_t field;
    static const soc_field_t l2_fields[] = {
        BANK0_HASH_OFFSETf, BANK1_HASH_OFFSETf, BANK2_HASH_OFFSETf,
        BANK3_HASH_OFFSETf, BANK4_HASH_OFFSETf, BANK5_HASH_OFFSETf
    };
    static const soc_field_t l3_fields[] = {
        BANK6_HASH_OFFSETf, BANK7_HASH_OFFSETf, BANK8_HASH_OFFSETf,
        BANK9_HASH_OFFSETf, BANK2_HASH_OFFSETf, BANK3_HASH_OFFSETf,
        BANK4_HASH_OFFSETf, BANK5_HASH_OFFSETf
    };
    static const soc_field_t fpem_fields[] = {
        BANK2_HASH_OFFSETf, BANK3_HASH_OFFSETf, BANK4_HASH_OFFSETf,
        BANK5_HASH_OFFSETf
    };

    if (mem == L2Xm) {
        if (bank < 0 || bank > 5) {
            return SOC_E_PARAM;
        }
        is_shared = bank > 1;
        reg = L2_TABLE_HASH_CONTROLr;
        field = l2_fields[bank];
    } else if (mem == L3_ENTRY_ONLYm) {
        if (bank < 2 || bank > 9) {
            return SOC_E_PARAM;
        }
        if (bank >= 6 && bank <= 9) { /* Dedicated banks */
            bank = bank - 6;
        } else if (bank >= 2 && bank <= 5) { /* Shared Banks */
            bank = 2 + bank;
            is_shared = 1;
        }
        reg = L3_TABLE_HASH_CONTROLr;
        field = l3_fields[bank];
    } else if (mem == EXACT_MATCH_2m) {
        if (bank < 2 || bank > 5) {
            return SOC_E_PARAM;
        }
        bank = bank - 2;
        is_shared = 1;
        reg = FPEM_CONTROLr;
        field = fpem_fields[bank];
    } else {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    *use_lsb = soc_reg_field_get(unit, reg, rval, HASH_ZERO_OR_LSBf);

    if (is_shared) {
        reg = SHARED_TABLE_HASH_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    }

    *hash_offset = soc_reg_field_get(unit, reg, rval, field);

    return SOC_E_NONE;
}

int
soc_th_hash_offset_set(int unit, soc_mem_t mem, int bank, int hash_offset,
                       int use_lsb)
{
    uint32 rval;
    soc_reg_t reg;
    static const soc_field_t fields[] = {
        BANK0_HASH_OFFSETf, BANK1_HASH_OFFSETf, BANK2_HASH_OFFSETf,
        BANK3_HASH_OFFSETf, BANK4_HASH_OFFSETf, BANK5_HASH_OFFSETf,
        BANK6_HASH_OFFSETf, BANK7_HASH_OFFSETf, BANK8_HASH_OFFSETf,
        BANK9_HASH_OFFSETf
    };

    if (mem == L2Xm) {
        if (bank < 0 || bank > 5) {
            return SOC_E_PARAM;
        }
        reg = L2_TABLE_HASH_CONTROLr;
    } else if (mem == L3_ENTRY_ONLYm) {
        if (bank < 2 || bank > 9) {
            return SOC_E_PARAM;
        }
        reg = L3_TABLE_HASH_CONTROLr;
    } else if (mem == EXACT_MATCH_2m) {
        if (bank < 2 || bank > 5) {
            return SOC_E_PARAM;
        }
        reg = FPEM_CONTROLr;
    } else {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, HASH_ZERO_OR_LSBf, use_lsb);
    if (bank < 2 || bank > 5) {
        soc_reg_field_set(unit, reg, &rval, fields[bank], hash_offset);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    if (bank > 1 && bank < 6) {
        reg = SHARED_TABLE_HASH_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, fields[bank], hash_offset);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th_shared_hash(int unit, int hash_offset, uint32 key_nbits, uint8 *key,
                    uint32 result_mask, uint16 lsb)
{
    uint32 crc_hi, crc_lo;

    if (hash_offset >= 48) {
        if (hash_offset > 48) {
            lsb >>= hash_offset - 48;
        }
        return lsb & result_mask;
    }

    crc_hi = soc_crc16b(key, key_nbits) | ((uint32)lsb << 16);
    if (hash_offset >= 32) {
        if (hash_offset > 32) {
            crc_hi >>= hash_offset - 32;
        }
        return crc_hi & result_mask;
    }

    crc_lo = soc_crc32b(key, key_nbits);
    if (hash_offset > 0) {
        crc_lo >>= hash_offset;
        crc_lo |= crc_hi << (32 - hash_offset);
    }
    return crc_lo & result_mask;
}

uint32
soc_th_l2x_hash(int unit, int bank, int hash_offset, int use_lsb,
                int key_nbits, void *base_entry, uint8 *key)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank < 2) {
        hash_mask = 0x03ff; /* 1k buckets per dedicated L2 bank */
        hash_bits = 10;
    } else {
        if (SOC_IS_TOMAHAWK2(unit)) {
            hash_mask = 0x3fff; /* 16k buckets per shared bank */
            hash_bits = 14;
        } else {
            hash_mask = 0x1fff; /* 8k buckets per shared bank */
            hash_bits = 13;
        }
    }

    if (use_lsb && (hash_offset + hash_bits > 48)) {
        switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
        case TH_L2_HASH_KEY_TYPE_BRIDGE:
        case TH_L2_HASH_KEY_TYPE_VFI:
        case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          L2__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        case TH_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VLAN__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_VIF:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VIF__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          TRILL_NONUC_NETWORK_LONG__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            lsb_val = soc_mem_field32_get
                (unit, L2Xm, base_entry, TRILL_NONUC_NETWORK_SHORT__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_BFD:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          BFD__HASH_LSBf);
            break;
        case TH_L2_HASH_KEY_TYPE_PE_VID:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          PE_VID__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_th_shared_hash(unit, hash_offset, key_nbits, key, hash_mask,
                               lsb_val);
}

int
soc_th_l2x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case TH_L2_HASH_KEY_TYPE_BRIDGE:
    case TH_L2_HASH_KEY_TYPE_VFI:
    case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
        field_list[0] = L2__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
    case TH_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        field_list[0] = VLAN__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_VIF:
        field_list[0] = VIF__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
        field_list[0] = TRILL_NONUC_NETWORK_LONG__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
        field_list[0] = TRILL_NONUC_NETWORK_SHORT__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_BFD:
        field_list[0] = BFD__KEYf;
        break;
    case TH_L2_HASH_KEY_TYPE_PE_VID:
        field_list[0] = PE_VID__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_th_hash_entry_to_key(unit, bank, entry, key, L2Xm, field_list);
}

uint32
soc_th_l2x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                       uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];

    key_nbits = soc_th_l2x_base_entry_to_key(unit, bank, entry, key);
    return soc_th_l2x_hash(unit, bank, hash_offset, use_lsb, key_nbits, entry,
                           key);
}

uint32
soc_th_l2x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_th_hash_offset_get(unit, L2Xm, bank, &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_l2x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_th_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                int key_nbits, void *base_entry, uint8 *key)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank > 5) {
        hash_mask = 0x1ff;  /* 512 buckets per dedicated L3 bank */
        hash_bits = 9;
    } else {
        if (SOC_IS_TOMAHAWK2(unit)) {
            hash_mask = 0x3fff; /* 16k buckets per shared bank */
            hash_bits = 14;
        } else {
            hash_mask = 0x1fff; /* 8k buckets per shared bank */
            hash_bits = 13;
        }
    }

    if (use_lsb && (hash_offset + hash_bits > 48)) {
        switch (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, base_entry,
                                    KEY_TYPEf)) {
        case TH_L3_HASH_KEY_TYPE_V4UC:
        case TH_L3_HASH_KEY_TYPE_V4UC_EXT:
        case TH_L3_HASH_KEY_TYPE_V4MC:
        case TH_L3_HASH_KEY_TYPE_V6UC:
        case TH_L3_HASH_KEY_TYPE_V6UC_EXT:
        case TH_L3_HASH_KEY_TYPE_V6MC:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                          base_entry, IPV4UC__HASH_LSBf);
            break;
        case TH_L3_HASH_KEY_TYPE_TRILL:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                          base_entry, TRILL__HASH_LSBf);
            break;
        case TH_L3_HASH_KEY_TYPE_DST_NAT:
        case TH_L3_HASH_KEY_TYPE_DST_NAPT:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          base_entry, NAT__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_th_shared_hash(unit, hash_offset, key_nbits, key, hash_mask,
                               lsb_val);
}

int
soc_th_hash_bucket_get(int unit, int mem, int bank, uint32 *entry, uint32 *bucket)
{
    switch(mem) {
    case L2Xm:
        *bucket = soc_th_l2x_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        *bucket = soc_th_l3x_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:        
        *bucket = soc_th_exact_match_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case VLAN_XLATEm:
        *bucket = soc_th_vlan_xlate_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case EGR_VLAN_XLATEm:
        *bucket = soc_th_egr_vlan_xlate_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case MPLS_ENTRYm:
        *bucket = soc_th_mpls_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case ING_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_th_ing_vp_vlan_member_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case EGR_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_th_egr_vp_vlan_member_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case ING_DNAT_ADDRESS_TYPEm:
        *bucket = soc_th_ing_dnat_address_type_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    default:
        return SOC_E_PARAM;
    }
}

int
soc_th_hash_index_get(int unit, int mem, int bank, int bucket)
{
    int rv;
    int index;
    int entries_per_row, entries_per_bucket, bank_base, bucket_offset;

    switch(mem) {
    case L2Xm:
        rv = soc_tomahawk_hash_bank_info_get(unit, mem, bank, NULL,
                                             &entries_per_row, NULL,
                                             &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            return bank_base + bucket * entries_per_row + bucket_offset;
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        rv = soc_tomahawk_hash_bank_info_get(unit, L3_ENTRY_ONLYm, bank, NULL,
                                             &entries_per_row, NULL,
                                             &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_row + bucket_offset;
            if (mem == L3_ENTRY_IPV4_MULTICASTm || mem == L3_ENTRY_IPV6_UNICASTm) {
                return index / 2;
            } else if (mem == L3_ENTRY_IPV6_MULTICASTm) {
                return index / 4;
            }
            return index;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
        rv = soc_tomahawk_hash_bank_info_get(unit, EXACT_MATCH_2m, bank, NULL,
                                             &entries_per_row, &entries_per_bucket,
                                             &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_bucket + bucket_offset;
            return index;
        }
        break;        
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:  
        rv = soc_tomahawk_hash_bank_info_get(unit, EXACT_MATCH_4m, bank, NULL,
                                             &entries_per_row, &entries_per_bucket,
                                             &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_bucket + bucket_offset;
            return index;
        }
        break;
    default:
        break;
    }

    return 0;
}

int
soc_th_l3x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem = L3_ENTRY_ONLYm;
    soc_field_t field_list[5];
    uint32 entry_words = 0;
    uint32 sanitized_entry[SOC_MAX_MEM_WORDS] = {0};
    void *ptr = NULL;

    ptr = (void *)entry;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        mem = L3_ENTRY_ONLY_SINGLEm;
    }
    switch (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf)) {
    case TH_L3_HASH_KEY_TYPE_V4UC_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV4_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(uint32));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ptr, KEY_TYPEf,
                            TH_L3_HASH_KEY_TYPE_V4UC);
        /* fall through */
    case TH_L3_HASH_KEY_TYPE_V4UC:
        mem = L3_ENTRY_IPV4_UNICASTm;
        if (!SOC_MEM_IS_VALID(unit, L3_ENTRY_IPV4_UNICASTm)) {
            mem = L3_ENTRY_ONLY_SINGLEm;
        }
        field_list[0] = IPV4UC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TH_L3_HASH_KEY_TYPE_V6UC_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV6_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(uint32));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV6_UNICASTm, ptr, KEY_TYPE_0f,
                            TH_L3_HASH_KEY_TYPE_V6UC);
        /* fall through */
    case TH_L3_HASH_KEY_TYPE_V6UC:
        mem = L3_ENTRY_IPV6_UNICASTm;
        if (!SOC_MEM_IS_VALID(unit, L3_ENTRY_IPV6_UNICASTm)) {
            mem = L3_ENTRY_ONLY_DOUBLEm;
        }
        field_list[0] = IPV6UC__KEY_0f;
        field_list[1] = IPV6UC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH_L3_HASH_KEY_TYPE_V4MC:
        mem = L3_ENTRY_IPV4_MULTICASTm;
        if (!SOC_MEM_IS_VALID(unit, L3_ENTRY_IPV4_MULTICASTm)) {
            mem = L3_ENTRY_ONLY_DOUBLEm;
        }
        field_list[0] = IPV4MC__KEY_0f;
        field_list[1] = IPV4MC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH_L3_HASH_KEY_TYPE_V6MC:
        mem = L3_ENTRY_IPV6_MULTICASTm;
        if (!SOC_MEM_IS_VALID(unit, L3_ENTRY_IPV6_MULTICASTm)) {
            mem = L3_ENTRY_ONLY_QUADm;
        }
        field_list[0] = IPV6MC__KEY_0f;
        field_list[1] = IPV6MC__KEY_1f;
        field_list[2] = IPV6MC__KEY_2f;
        field_list[3] = IPV6MC__KEY_3f;
        field_list[4] = INVALIDf;
        break;
    case TH_L3_HASH_KEY_TYPE_TRILL:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = TRILL__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TH_L3_HASH_KEY_TYPE_DST_NAT:
    case TH_L3_HASH_KEY_TYPE_DST_NAPT:
        mem = L3_ENTRY_IPV4_MULTICASTm;
        field_list[0] = NAT__KEY_0f;
        field_list[1] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_th_hash_entry_to_key(unit, bank, ptr, key, mem, field_list);
}

uint32
soc_th_l3x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];

    key_nbits = soc_th_l3x_base_entry_to_key(unit, bank, entry, key);
    return soc_th_l3x_hash(unit, bank, hash_offset, use_lsb, key_nbits, entry, key);
}

uint32
soc_th_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_th_hash_offset_get(unit, L3_ENTRY_ONLYm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_l3x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_th_exact_match_hash(int unit, int bank, int hash_offset, int use_lsb,
                        int key_nbits, void *base_entry, uint8 *key)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    if (SOC_IS_TOMAHAWK2(unit)) {
        hash_mask = 0x1fff; /* 8k buckets per shared bank */
        hash_bits = 14;
    } else {
        hash_mask = 0xfff; /* 4k buckets per shared bank */
        hash_bits = 13;
    }

    if (use_lsb && (hash_offset + hash_bits > 48)) {
        switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry,
                                    KEY_TYPE_0f)) {
        case TH_FPEM_HASH_KEY_TYPE_128B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m,
                                          base_entry, MODE128__HASH_LSBf);
            break;
        case TH_FPEM_HASH_KEY_TYPE_160B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m,
                                          base_entry, MODE160__HASH_LSBf);
            break;
        case TH_FPEM_HASH_KEY_TYPE_320B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_4m,
                                          base_entry, MODE320__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }
    if (TH_FPEM_HASH_KEY_TYPE_320B == 
        soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry, KEY_TYPE_0f)) {
        hash_mask >>= 1; /* for em quad mode, one bucket holds 4 sram entries */ 
    }
    return _soc_th_shared_hash(unit, hash_offset, key_nbits, key, hash_mask,
                               lsb_val);
}

int
soc_th_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];

    switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, entry, KEY_TYPE_0f)) {
    case TH_FPEM_HASH_KEY_TYPE_128B:
        mem = EXACT_MATCH_2m;
        field_list[0] = MODE128__KEY_0f;
        field_list[1] = MODE128__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH_FPEM_HASH_KEY_TYPE_160B:
        mem = EXACT_MATCH_2m;
        field_list[0] = MODE160__KEY_0f;
        field_list[1] = MODE160__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH_FPEM_HASH_KEY_TYPE_320B:
        mem = EXACT_MATCH_4m;
        field_list[0] = MODE320__KEY_0f;
        field_list[1] = MODE320__KEY_1f;
        field_list[2] = MODE320__KEY_2f;
        field_list[3] = MODE320__KEY_3f;
        field_list[4] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_th_hash_entry_to_key(unit, bank, entry, key, mem, field_list);
}

uint32
soc_th_exact_match_entry_hash(int unit, int bank, int hash_offset,
                              int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];

    key_nbits = soc_th_exact_match_base_entry_to_key(unit, bank, entry, key);
    return soc_th_exact_match_hash(unit, bank, hash_offset, use_lsb,
                                   key_nbits, entry, key);
}

uint32
soc_th_exact_match_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_th_hash_offset_get(unit, EXACT_MATCH_2m, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_exact_match_entry_hash(unit, bank, hash_offset, use_lsb,
                                         entry);
}

uint32
soc_th_mpls_hash(int unit, int hash_sel, int key_nbits, void *base_entry,
                 uint8 *key)
{
    uint32 rv = 0;

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
        switch (soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                    KEY_TYPEf)) {
        case TH_MPLS_HASH_KEY_TYPE_MPLS:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     MPLS__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_MIM_NVP:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     MIM_NVP__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_MIM_ISID:
        case TH_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     MIM_ISID__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_TRILL:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     TRILL__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_L2GRE_SIP:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     L2GRE_SIP__HASH_LSBf);
        break;
        case TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP:
        case TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     L2GRE_VPNID__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_VXLAN_SIP:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     VXLAN_SIP__HASH_LSBf);
            break;
        case TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID:
        case TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP:
            rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                     VXLAN_VN_ID__HASH_LSBf);
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
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_th_mpls_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_mpls;
}

int
soc_th_mpls_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, MPLS_ENTRYm, entry, KEY_TYPEf)) {
    case TH_MPLS_HASH_KEY_TYPE_MPLS:
        field_list[0] = MPLS__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_MIM_NVP:
        field_list[0] = MIM_NVP__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_MIM_ISID:
    case TH_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
        field_list[0] = MIM_ISID__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_TRILL:
        field_list[0] = TRILL__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_L2GRE_SIP:
        field_list[0] = L2GRE_SIP__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP:
    case TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID:
        field_list[0] = L2GRE_VPNID__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_VXLAN_SIP:
        field_list[0] = VXLAN_SIP__KEYf;
        break;
    case TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID:
    case TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP:
        field_list[0] = VXLAN_VN_ID__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_th_hash_entry_to_key(unit, bank, entry, key, MPLS_ENTRYm,
                                     field_list);
}

uint32
soc_th_mpls_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));

    key_nbits = soc_th_mpls_base_entry_to_key(unit, bank, entry, key);
    index = soc_th_mpls_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_th_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, MPLS_ENTRYm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_mpls_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_th_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                       void *base_entry, uint8 *key)
{
    uint32 rv = 0;

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
        case TH_VLXLT_HASH_KEY_TYPE_IVID_OVID:
        case TH_VLXLT_HASH_KEY_TYPE_OTAG:
        case TH_VLXLT_HASH_KEY_TYPE_ITAG:
        case TH_VLXLT_HASH_KEY_TYPE_OVID:
        case TH_VLXLT_HASH_KEY_TYPE_IVID:
        case TH_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     XLATE__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_HPAE:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC_IP_BIND__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_VIF:
        case TH_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
        case TH_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
        case TH_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
        case TH_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     VIF__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
            rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry,
                                     MAC_PORT__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     L2GRE_DIP__HASH_LSBf);
            break;
        case TH_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
            rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry,
                                     VXLAN_DIP__HASH_LSBf);
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
                              "soc_th_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

int
soc_th_vlan_xlate_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf)) {
    case TH_VLXLT_HASH_KEY_TYPE_IVID_OVID:
    case TH_VLXLT_HASH_KEY_TYPE_OTAG:
    case TH_VLXLT_HASH_KEY_TYPE_ITAG:
    case TH_VLXLT_HASH_KEY_TYPE_OVID:
    case TH_VLXLT_HASH_KEY_TYPE_IVID:
    case TH_VLXLT_HASH_KEY_TYPE_PRI_CFI:
        mem = VLAN_XLATEm;
        field_list[0] = XLATE__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
        mem = VLAN_MACm;
        field_list[0] = MAC__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_HPAE:
        mem = VLAN_MACm;
        field_list[0] = MAC_IP_BIND__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_VIF:
    case TH_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
    case TH_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
    case TH_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
    case TH_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
        mem = VLAN_XLATEm;
        field_list[0] = VIF__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
        mem = VLAN_MACm;
        field_list[0] = MAC_PORT__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
        mem = VLAN_XLATEm;
        field_list[0] = L2GRE_DIP__KEYf;
        break;
    case TH_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
        mem = VLAN_XLATEm;
        field_list[0] = VXLAN_DIP__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_th_hash_entry_to_key(unit, bank, entry, key, mem, field_list);
}

uint32
soc_th_vlan_xlate_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));
    key_nbits = soc_th_vlan_xlate_base_entry_to_key(unit, bank, entry, key);
    index = soc_th_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_th_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, VLAN_XLATEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_vlan_xlate_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_th_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                           void *base_entry, uint8 *key)
{
    uint32 rv;

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
        switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                    ENTRY_TYPEf)) {
        case TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
        case TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
            rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                     XLATE__HASH_LSBf);
            break;
        case TH_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
        case TH_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
            rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                     MIM_ISID__HASH_LSBf);
            break;
        case TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI:
        case TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP:
            rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                     L2GRE_VFI__HASH_LSBf);
            break;
        case TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI:
        case TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP:
            rv = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, base_entry,
                                     VXLAN_VFI__HASH_LSBf);
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
                              "soc_th_egr_vlan_xlate_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate;
}

int
soc_th_egr_vlan_xlate_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf)) {
    case TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
    case TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
        field_list[0] = XLATE__KEYf;
        break;
    case TH_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
    case TH_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
        field_list[0] = MIM_ISID__KEYf;
        break;
    case TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI:
    case TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP:
        field_list[0] = L2GRE_VFI__KEYf;
        break;
    case TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI:
    case TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP:
        field_list[0] = VXLAN_VFI__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_th_hash_entry_to_key(unit, bank, entry, key, EGR_VLAN_XLATEm,
                                     field_list);
}

uint32
soc_th_egr_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                 uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));

    key_nbits = soc_th_egr_vlan_xlate_base_entry_to_key(unit, bank, entry, key);
    index = soc_th_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_th_egr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, EGR_VLAN_XLATEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_egr_vlan_xlate_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_th_ing_vp_vlan_member_hash(int unit, int hash_sel, int key_nbits,
                               void *base_entry, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_ing_vp_vlan_member == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, ING_VP_VLAN_MEMBERSHIPm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_ing_vp_vlan_member = mask;
        SOC_CONTROL(unit)->hash_bits_ing_vp_vlan_member = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_ing_vp_vlan_member;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        rv = soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm, base_entry,
                                 HASH_LSBf);
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_ing_vp_vlan_member;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_th_inv_vp_vlan_member_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_ing_vp_vlan_member;
}

int
soc_th_ing_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_th_hash_entry_to_key(unit, bank, entry, key,
                                     ING_VP_VLAN_MEMBERSHIPm, field_list);
}

uint32
soc_th_ing_vp_vlan_member_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_th_ing_vp_vlan_member_base_entry_to_key(unit, bank, entry, key);
    index = soc_th_ing_vp_vlan_member_hash(unit, hash_sel, key_nbits, entry,
                                           key);

    return index;
}

uint32
soc_th_ing_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, ING_VP_VLAN_MEMBERSHIPm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_ing_vp_vlan_member_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_th_egr_vp_vlan_member_hash(int unit, int hash_sel, int key_nbits,
                               void *base_entry, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_egr_vp_vlan_member == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, EGR_VP_VLAN_MEMBERSHIPm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_egr_vp_vlan_member = mask;
        SOC_CONTROL(unit)->hash_bits_egr_vp_vlan_member = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_egr_vp_vlan_member;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        rv = soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm, base_entry,
                                 HASH_LSBf);
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_egr_vp_vlan_member;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_th_inv_vp_vlan_member_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vp_vlan_member;
}

int
soc_th_egr_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_th_hash_entry_to_key(unit, bank, entry, key,
                                     EGR_VP_VLAN_MEMBERSHIPm, field_list);
}

uint32
soc_th_egr_vp_vlan_member_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_th_egr_vp_vlan_member_base_entry_to_key(unit, bank, entry, key);
    index = soc_th_egr_vp_vlan_member_hash(unit, hash_sel, key_nbits, entry,
                                           key);

    return index;
}

uint32
soc_th_egr_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, EGR_VP_VLAN_MEMBERSHIPm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_egr_vp_vlan_member_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_th_ing_dnat_address_type_hash(int unit, int hash_sel, int key_nbits,
                                  void *base_entry, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_ing_dnat_address_type == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, ING_DNAT_ADDRESS_TYPEm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_ing_dnat_address_type = mask;
        SOC_CONTROL(unit)->hash_bits_ing_dnat_address_type = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_ing_dnat_address_type;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 0) {
            return 0;
        }
        rv = soc_mem_field32_get(unit, ING_DNAT_ADDRESS_TYPEm, base_entry,
                                 HASH_LSBf);
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_ing_dnat_address_type;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        LOG_ERROR(BSL_LS_SOC_HASH,
                  (BSL_META_U(unit,
                              "soc_th_inv_vp_vlan_member_hash: invalid hash_sel %d\n"),
                   hash_sel));
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_ing_dnat_address_type;
}

int
soc_th_ing_dnat_address_type_base_entry_to_key(int unit, int bank, void *entry,
                                               uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_th_hash_entry_to_key(unit, bank, entry, key, ING_DNAT_ADDRESS_TYPEm,
                                     field_list);
}

uint32
soc_th_ing_dnat_address_type_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_th_ing_dnat_address_type_base_entry_to_key(unit, bank,
                                                               entry, key);
    index = soc_th_ing_dnat_address_type_hash(unit, hash_sel, key_nbits,
                                              entry, key);

    return index;
}

uint32
soc_th_ing_dnat_address_type_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_th_hash_sel_get(unit, ING_DNAT_ADDRESS_TYPEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_th_ing_dnat_address_type_entry_hash(unit, hash_sel, bank, entry);
}

#endif /* BCM_TOMAHAWK_SUPPORT */
