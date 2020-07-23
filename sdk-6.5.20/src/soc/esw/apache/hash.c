/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Apache hash table calculation routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_APACHE_SUPPORT)

#include <soc/apache.h>
#include <soc/trident2.h>
#include <soc/bondoptions.h>

/* Get number of banks for the hash table according to the config */
int
soc_apache_hash_bank_count_get(int unit, soc_mem_t mem, int *num_banks)
{
    int count;

    int shared_bank_size = 64;
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) { 
        shared_bank_size = 0;
    }
    else 
#endif
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureUft128k)) {
        shared_bank_size = 32;
        }
    }
    switch (mem) {
    case L2Xm:
        /*
         * 8k entries in each of 2 dedicated L2 bank
         * 64k entries in each shared bank
         */
        count = soc_mem_index_count(unit, L2Xm);
        if (SOC_IS_MONTEREY(unit)) { 
            *num_banks = 2;
        }  else {  
             *num_banks = 2 + (count - 2 * 8 * 1024) / (shared_bank_size * 1024);
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        /*
         * 1k entries in each of 4 dedicated L3 bank
         * 64k entries in each shared bank
         */
        count = soc_mem_index_count(unit, L3_ENTRY_ONLYm);
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) { 
            *num_banks = 4;
        } else 
#endif
        { 
            *num_banks = 4 + (count - 4 * 1 * 1024) / (shared_bank_size * 1024);
        }
        break;
    case MPLS_ENTRYm:
    case VLAN_XLATEm:
    case VLAN_MACm:
    case EGR_VLAN_XLATEm:
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
    case L2_ENDPOINT_IDm:
    case ENDPOINT_QUEUE_MAPm:
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
soc_apache_log_to_phy_bank_map(int unit, soc_mem_t mem, int log_bank,
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

    switch (mem) {
    case L2Xm:
        if (log_bank < 0 || log_bank > 5) {
            return SOC_E_PARAM;
        }
        if (log_bank < 2) {
            *phy_bank_id = log_bank;
        } else {
            SOC_IF_ERROR_RETURN(READ_L2_ISS_LOG_TO_PHY_BANK_MAPr(unit, &val));
            /* Starts with 0 in the register which maps to Physical bank 2 */
            *phy_bank_id = 2 + soc_reg_field_get(unit, 
                                L2_ISS_LOG_TO_PHY_BANK_MAPr, val,
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
            SOC_IF_ERROR_RETURN(READ_L3_ISS_LOG_TO_PHY_BANK_MAPr(unit, &val));
            /* Starts with 0 in the register which maps to Physical bank 2 */
            *phy_bank_id = 2 + soc_reg_field_get(unit, 
                                L3_ISS_LOG_TO_PHY_BANK_MAPr, val,
                                l3_fields[log_bank - 4]);
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
soc_apache_phy_to_log_bank_map(int unit, soc_mem_t mem, int phy_bank, int *log_bank)
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

    switch (mem) {
    case L2Xm:
        if (phy_bank < 0 || phy_bank > 5) {
            return SOC_E_PARAM;
        }
        if (phy_bank < 2) {
            *log_bank = phy_bank;
        } else {
            SOC_IF_ERROR_RETURN(READ_L2_ISS_LOG_TO_PHY_BANK_MAPr(unit, &val));
            for (i = 0; i < 4; i++) {
                /* Starts with 0 in the register which maps to Physical bank 2 */
                if ((soc_reg_field_get(unit, L2_ISS_LOG_TO_PHY_BANK_MAPr, val,
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
            SOC_IF_ERROR_RETURN(READ_L3_ISS_LOG_TO_PHY_BANK_MAPr(unit, &val));
            for (i = 0; i < 4; i++) {
               /* Starts with 0 in the register which maps to Physical bank 2 */
                if ((soc_reg_field_get(unit, L3_ISS_LOG_TO_PHY_BANK_MAPr, val,
                                      l3_fields[i]) + 2) == phy_bank) {
                    *log_bank = 4 + i;
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
soc_apache_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem, uint32 *bitmap)
{
    uint32 physical_bitmap = 0;
    int count = 0;
    int i, phy_bank_id = 0;
    SOC_IF_ERROR_RETURN(soc_apache_hash_bank_count_get(unit, mem, &count));
    for (i = 0; i < count; i++) {
        SOC_IF_ERROR_RETURN(
            soc_apache_log_to_phy_bank_map(unit, mem, i, &phy_bank_id));
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
soc_apache_hash_bank_number_get(int unit, soc_mem_t mem, int seq_num,
                                  int *bank_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_apache_hash_bank_count_get(unit, mem, &count));
    if (seq_num < 0 || seq_num >= count) {
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_apache_log_to_phy_bank_map(unit, mem, seq_num, bank_num));

    return SOC_E_NONE;
}

/*
 * Map bank number to bank sequence number.
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
soc_apache_hash_seq_number_get(int unit, soc_mem_t mem, int bank_num,
                                 int *seq_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_apache_phy_to_log_bank_map(unit, mem, bank_num, seq_num));

    SOC_IF_ERROR_RETURN(soc_apache_hash_bank_count_get(unit, mem, &count));
    if (*seq_num < 0 || *seq_num >= count) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;

}

int
soc_apache_hash_bank_info_get(int unit, soc_mem_t mem, int bank,
                                int *entries_per_bank, int *entries_per_row,
                                int *entries_per_bucket, int *bank_base,
                                int *bucket_offset)
{
    int bank_size = 0, row_size = 0, bucket_size = 0, base = 0, offset;
    int seq_num = 0;

    int shared_bank_size = 65536;

#if defined(BCM_MONTEREY_SUPPORT)
    if(SOC_IS_MONTEREY(unit)) {
        shared_bank_size = 0;
    }  
    else 
#endif
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureUft128k)) {
            shared_bank_size = 32768;
        }
    }

    /* Get logical bank number */
    SOC_IF_ERROR_RETURN(soc_apache_phy_to_log_bank_map(unit, mem, bank, &seq_num));
    bank = seq_num;

    /*
     * entry index for bucket b, entry e =
     *     bank_base + b * entries_per_row + bucket_offset + e;
     */
    switch (mem) {
    case L2Xm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0: index      0-  8191 (8k entries in dedicated bank 0)
         * bank 1: index   8192- 16383 (8k entries in dedicated bank 1)
         * bank 2: index  16384- 81919 (64k entries in shared bank)
         * bank 3: index  81920-147455 (64k entries in shared bank)
         * bank 4: index 147456-212991 (64k entries in shared bank)
         * bank 5: index 212992-278527 (64k entries in shared bank)
         * each bucket has 4 entries
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 5) {
            return SOC_E_INTERNAL;
        } else if (bank < 2) { /* 4k entries for each dedicated bank */
            bank_size = 8192;
#if defined(BCM_MONTEREY_SUPPORT)
            if(SOC_IS_MONTEREY(unit)) {
                bank_size = 16384;
            }  
#endif
            base = bank * bank_size;
        } else { /* 64k entries for each shared bank */
            bank_size = shared_bank_size;
            base = 16384 + (bank - 2) * bank_size;
        }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0: index      0-  1023 (1k entries in dedicated bank 6)
         * bank 1: index   1024-  2047 (1k entries in dedicated bank 7)
         * bank 2: index   2048-  3071 (1k entries in dedicated bank 8)
         * bank 3: index   3072-  4095 (1k entries in dedicated bank 9)
         * bank 4: index   4096- 69631 (64k entries in shared bank)
         * bank 5: index  69632-135167 (64k entries in shared bank)
         * bank 6: index 135168-200703 (64k entries in shared bank)
         * bank 7: index 200704-266239 (64k entries in shared bank)
         * each bucket has 4 entries
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 7) {
            return SOC_E_INTERNAL;
        } else if (bank < 4) { /* 1k entriess for each dedicated bank */
#if defined(BCM_MONTEREY_SUPPORT)
            if (SOC_IS_MONTEREY(unit)) {
                bank_size = 2048;
            } else 
#endif
            {
                bank_size = 1024;
            }
            base = bank * bank_size;
        } else { /* 64k entries for each shared bank */
            bank_size = shared_bank_size;
            base = 4096 + (bank - 4) * bank_size;
        }
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            row_size = 2;
            bucket_size = 2;
            offset = 0;
            if (bank < 0 || bank > 7) {
                return SOC_E_INTERNAL;
            } else if (bank < 4) { /* 1k entriess for each dedicated bank */
                bank_size = 1024;
                base = bank * bank_size;
            } else { /* 64k entries for each shared bank */
                bank_size = shared_bank_size;
                base = 4096 + (bank - 4) * bank_size;
            }
        } else 
#endif
       { 
            row_size = 4;
            bucket_size = 4;
            offset = 0;
            if (bank < 0 || bank > 7) {
                return SOC_E_INTERNAL;
            } else if (bank < 4) { /* 1k entriess for each dedicated bank */
                bank_size = 1024;
                base = bank * bank_size;
            } else { /* 64k entries for each shared bank */
                bank_size = shared_bank_size;
                base = 4096 + (bank - 4) * bank_size;
            }

        } 
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            row_size = 1;
            bucket_size = 1;
            offset = 0;
            if (bank < 0 || bank > 7) {
                return SOC_E_INTERNAL;
            } else if (bank < 4) { /* 1k entriess for each dedicated bank */
                bank_size = 512;
                base = bank * bank_size;
            }
        } else 

#endif
        { 
            row_size = 4;
            bucket_size = 4;
            offset = 0;
            if (bank < 0 || bank > 7) {
                return SOC_E_INTERNAL;
            } else if (bank < 4) { /* 1k entriess for each dedicated bank */
                bank_size = 1024;
                base = bank * bank_size;
            } else { /* 64k entries for each shared bank */
                bank_size = shared_bank_size;
                base = 4096 + (bank - 4) * bank_size;
            }
        }
        break;
    case MPLS_ENTRYm:
    case VLAN_XLATEm:
    case VLAN_MACm:
    case EGR_VLAN_XLATEm:
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
    case L2_ENDPOINT_IDm:
    case ENDPOINT_QUEUE_MAPm:
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
_soc_ap_hash_entry_to_key(int unit, void *entry, uint8 *key, soc_mem_t mem,
                          soc_field_t *field_list)
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
    case L2_ENDPOINT_IDm:
        val_bits = soc_mem_field_length(unit, L2_ENDPOINT_IDm, L2__KEYf);
        break;
    default:
        break;
    }

    bits = (val_bits + 7) & ~0x7;
    val_bits = bits - val_bits;

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

    return bits;
}


/* For UFT style hash (L2 and L3 table) */
int
soc_ap_hash_offset_get(int unit, soc_mem_t mem, int bank, int *hash_offset,
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
soc_ap_hash_offset_set(int unit, soc_mem_t mem, int bank, int hash_offset,
                       int use_lsb)
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
    } else {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, HASH_ZERO_OR_LSBf, use_lsb);
    if (!is_shared) {
        soc_reg_field_set(unit, reg, &rval, field, hash_offset);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    if (is_shared) {
        reg = SHARED_TABLE_HASH_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, field, hash_offset);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_ap_shared_hash(int unit, int hash_offset, uint32 key_nbits, uint8 *key,
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
soc_ap_l2x_hash(int unit, int bank, int hash_offset, int use_lsb,
                int key_nbits, void *base_entry, uint8 *key)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank < 2) {

#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) { 
            hash_mask = 0x0fff; /* 4k buckets per dedicated L2 bank */
        } else 
#endif
        {
            hash_mask = 0x07ff; /* 2k buckets per dedicated L2 bank */
        }
        hash_bits = 11;
    } else {
        hash_mask = 0x3fff; /*16k buckets per shared bank */
        hash_bits = 14;
    }

    if (use_lsb && (hash_offset + hash_bits > 48)) {
        switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
        case TD2_L2_HASH_KEY_TYPE_BRIDGE:
        case TD2_L2_HASH_KEY_TYPE_VFI:
        case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          L2__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        case TD2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VLAN__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_VIF:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VIF__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          TRILL_NONUC_NETWORK_LONG__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            lsb_val = soc_mem_field32_get
                (unit, L2Xm, base_entry, TRILL_NONUC_NETWORK_SHORT__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_BFD:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          BFD__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_PE_VID:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          PE_VID__HASH_LSBf);
            break;
        case TD2_L2_HASH_KEY_TYPE_FCOE_ZONE:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          FCOE_ZONE__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_ap_shared_hash(unit, hash_offset, key_nbits, key, hash_mask,
                               lsb_val);
}

int
soc_ap_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case TD2_L2_HASH_KEY_TYPE_BRIDGE:
    case TD2_L2_HASH_KEY_TYPE_VFI:
    case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
        field_list[0] = L2__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
    case TD2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        field_list[0] = VLAN__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_VIF:
        field_list[0] = VIF__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
        field_list[0] = TRILL_NONUC_NETWORK_LONG__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
        field_list[0] = TRILL_NONUC_NETWORK_SHORT__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_BFD:
        field_list[0] = BFD__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_PE_VID:
        field_list[0] = PE_VID__KEYf;
        break;
    case TD2_L2_HASH_KEY_TYPE_FCOE_ZONE:
        field_list[0] = FCOE_ZONE__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_ap_hash_entry_to_key(unit, entry, key, L2Xm, field_list);
}

uint32
soc_ap_l2x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                       uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];

    key_nbits = soc_ap_l2x_base_entry_to_key(unit, entry, key);
    return soc_ap_l2x_hash(unit, bank, hash_offset, use_lsb, key_nbits, entry,
                           key);
}

uint32
soc_ap_l2x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_ap_hash_offset_get(unit, L2Xm, bank, &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_l2x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_ap_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                int key_nbits, void *base_entry, uint8 *key)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank > 5) {
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {  
            hash_mask = 0x1ff;  /* 256 buckets per dedicated L3 bank */
            hash_bits = 9;
        } else 
#endif
        { 
            hash_mask = 0xff;  /* 256 buckets per dedicated L3 bank */
            hash_bits = 8;
        } 
 
    } else {
        hash_mask = 0x3fff; /*16k buckets per shared bank */
        hash_bits = 14;
    }

    if (use_lsb && (hash_offset + hash_bits > 48)) {
        switch (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, base_entry,
                                    KEY_TYPEf)) {
        case TD2_L3_HASH_KEY_TYPE_V4UC:
        case TD2_L3_HASH_KEY_TYPE_V4UC_EXT:
        case TD2_L3_HASH_KEY_TYPE_V4MC:
        case TD2_L3_HASH_KEY_TYPE_V6UC:
        case TD2_L3_HASH_KEY_TYPE_V6UC_EXT:
        case TD2_L3_HASH_KEY_TYPE_V6MC:
        case TD2_L3_HASH_KEY_TYPE_V4L2MC: 
        case TD2_L3_HASH_KEY_TYPE_V4L2VPMC:
        case TD2_L3_HASH_KEY_TYPE_V6L2MC:
        case TD2_L3_HASH_KEY_TYPE_V6L2VPMC:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                          base_entry, IPV4UC__HASH_LSBf);
            break;
        case TD2_L3_HASH_KEY_TYPE_TRILL:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                          base_entry, TRILL__HASH_LSBf);
            break;
        case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
        case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT:
        case TD2_L3_HASH_KEY_TYPE_FCOE_HOST:
        case TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT:
        case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP:
        case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                          base_entry, FCOE__HASH_LSBf);
            break;
        case TD2_L3_HASH_KEY_TYPE_DST_NAT:
        case TD2_L3_HASH_KEY_TYPE_DST_NAPT:
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

    return _soc_ap_shared_hash(unit, hash_offset, key_nbits, key, hash_mask,
                               lsb_val);
}


int
soc_ap_l3x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];
    uint32 entry_words = 0;
    uint32 sanitized_entry[SOC_MAX_MEM_WORDS] = {0};
    void *ptr = NULL;

    ptr = (void *) entry;

    switch (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, entry, KEY_TYPEf)) {
    case TD2_L3_HASH_KEY_TYPE_V4UC_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV4_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(entry_words));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ptr, KEY_TYPEf,
                            TD2_L3_HASH_KEY_TYPE_V4UC);
        /* fall through */
    case TD2_L3_HASH_KEY_TYPE_V4UC:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = IPV4UC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_V6UC_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV6_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(entry_words));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV6_UNICASTm, ptr, KEY_TYPE_0f,
                            TD2_L3_HASH_KEY_TYPE_V6UC);
        /* fall through */
    case TD2_L3_HASH_KEY_TYPE_V6UC:
        mem = L3_ENTRY_IPV6_UNICASTm;
        field_list[0] = IPV6UC__KEY_0f;
        field_list[1] = IPV6UC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_V4MC:
    case TD2_L3_HASH_KEY_TYPE_V4L2MC:
    case TD2_L3_HASH_KEY_TYPE_V4L2VPMC:
        mem = L3_ENTRY_IPV4_MULTICASTm;
        field_list[0] = IPV4MC__KEY_0f;
        field_list[1] = IPV4MC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_V6MC:
    case TD2_L3_HASH_KEY_TYPE_V6L2MC:
    case TD2_L3_HASH_KEY_TYPE_V6L2VPMC:
        mem = L3_ENTRY_IPV6_MULTICASTm;
        field_list[0] = IPV6MC__KEY_0f;
        field_list[1] = IPV6MC__KEY_1f;
        field_list[2] = IPV6MC__KEY_2f;
        field_list[3] = IPV6MC__KEY_3f;
        field_list[4] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_TRILL:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = TRILL__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV4_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(entry_words));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ptr, KEY_TYPEf,
                            TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN);
        /* fall through */
    case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = FCOE__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV4_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(entry_words));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ptr, KEY_TYPEf,
                            TD2_L3_HASH_KEY_TYPE_FCOE_HOST);
        /* fall through */
    case TD2_L3_HASH_KEY_TYPE_FCOE_HOST:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = FCOE__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT:
        entry_words = SOC_MEM_WORDS(unit, L3_ENTRY_IPV4_UNICASTm);
        sal_memcpy(&sanitized_entry, entry, entry_words * sizeof(entry_words));
        ptr = &sanitized_entry;
        soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ptr, KEY_TYPEf,
                            TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP);
        /* fall through */
    case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP:
        mem = L3_ENTRY_IPV4_UNICASTm;
        field_list[0] = FCOE__KEYf;
        field_list[1] = INVALIDf;
        break;


    case TD2_L3_HASH_KEY_TYPE_DST_NAT:
    case TD2_L3_HASH_KEY_TYPE_DST_NAPT:
        mem = L3_ENTRY_IPV4_MULTICASTm;
        field_list[0] = NAT__KEY_0f;
        field_list[1] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_ap_hash_entry_to_key(unit, ptr, key, mem, field_list);
}

uint32
soc_ap_l3x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];

    key_nbits = soc_ap_l3x_base_entry_to_key(unit, entry, key);
    return soc_ap_l3x_hash(unit, bank, hash_offset, use_lsb, key_nbits,                           entry, key);
}

uint32
soc_ap_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_ap_hash_offset_get(unit, L3_ENTRY_ONLYm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_l3x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_ap_mpls_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));

    key_nbits = soc_td2_mpls_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_mpls_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_ap_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, MPLS_ENTRYm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_mpls_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_vlan_xlate_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));
    key_nbits = soc_td2_vlan_xlate_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_ap_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, VLAN_XLATEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_vlan_xlate_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_egr_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                  uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    sal_memset(key, 0, sizeof(key));

    key_nbits = soc_td2_egr_vlan_xlate_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_ap_egr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, EGR_VLAN_XLATEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_egr_vlan_xlate_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_ing_vp_vlan_member_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_td2_ing_vp_vlan_member_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_ing_vp_vlan_member_hash(unit, hash_sel, key_nbits, entry,
                                           key);

    return index;
}

uint32
soc_ap_ing_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, ING_VP_VLAN_MEMBERSHIPm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_ing_vp_vlan_member_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_egr_vp_vlan_member_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_td2_egr_vp_vlan_member_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_egr_vp_vlan_member_hash(unit, hash_sel, key_nbits, entry,
                                           key);

    return index;
}

uint32
soc_ap_egr_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, EGR_VP_VLAN_MEMBERSHIPm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_egr_vp_vlan_member_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_ing_dnat_address_type_entry_hash(int unit, int hash_sel,
                                        int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_td2_ing_dnat_address_type_base_entry_to_key(unit, bank,
                                                                entry, key);
    index = soc_td2_ing_dnat_address_type_hash(unit, hash_sel, key_nbits,
                                              entry, key);

    return index;
}

uint32
soc_ap_ing_dnat_address_type_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, ING_DNAT_ADDRESS_TYPEm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_ing_dnat_address_type_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_l2_endpoint_id_entry_hash(int unit, int hash_sel, int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_td2_l2_endpoint_id_base_entry_to_key(unit, bank, entry, key);
    index = soc_td2_l2_endpoint_id_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_ap_l2_endpoint_id_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, L2_ENDPOINT_IDm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_l2_endpoint_id_entry_hash(unit, hash_sel, bank, entry);
}

uint32
soc_ap_endpoint_queue_map_entry_hash(int unit, int hash_sel,
                                     int bank, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_td2_endpoint_queue_map_base_entry_to_key(unit, bank,
                                                             entry, key);
    index = soc_td2_endpoint_queue_map_hash(unit, hash_sel, key_nbits, entry,
                                            key);

    return index;
}

uint32
soc_ap_endpoint_queue_map_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_sel;

    rv = soc_td2_hash_sel_get(unit, ENDPOINT_QUEUE_MAPm, bank, &hash_sel);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_ap_endpoint_queue_map_entry_hash(unit, hash_sel, bank, entry);
}

int
soc_ap_hash_bucket_get(int unit, int mem, int bank, uint32 *entry, uint32 *bucket)
{
    switch(mem) {
    case L2Xm:
        *bucket = soc_ap_l2x_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        *bucket = soc_ap_l3x_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case ING_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_ap_ing_vp_vlan_member_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case EGR_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_ap_egr_vp_vlan_member_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case ING_DNAT_ADDRESS_TYPEm:
        *bucket = soc_ap_ing_dnat_address_type_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case L2_ENDPOINT_IDm:
        *bucket = soc_ap_l2_endpoint_id_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case ENDPOINT_QUEUE_MAPm:
        *bucket = soc_ap_endpoint_queue_map_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case VLAN_XLATEm:
        *bucket = soc_ap_vlan_xlate_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case EGR_VLAN_XLATEm:
        *bucket = soc_ap_egr_vlan_xlate_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    case MPLS_ENTRYm:
        *bucket = soc_ap_mpls_bank_entry_hash(unit, bank, entry);
        return SOC_E_NONE;
    default:
        return SOC_E_PARAM;
    }
}

int
soc_ap_hash_index_get(int unit, int mem, int bank, int bucket)
{
    int rv;
    int index;
    int entries_per_row, bank_base, bucket_offset;

    switch(mem) {
    case L2Xm:
        rv = soc_apache_hash_bank_info_get(unit, mem, bank, NULL,
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
        rv = soc_apache_hash_bank_info_get(unit, L3_ENTRY_ONLYm, bank, NULL,
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
    default:
        break;
    }

    return 0;
}

#endif /* BCM_APACHE_SUPPORT */
