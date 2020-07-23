/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Tomahawk3 hash table calculation routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/tomahawk3.h>

/* Map physical to logical bank mapping for a memory
 *  mem         - memory
 *  phy_bank    - physical bank number
 *  log_bank    - logical bank id
 */
STATIC int
soc_tomahawk3_phy_to_log_bank_map(int unit, soc_mem_t mem, int phy_bank, int *log_bank)
{
    uint32 hash_control_entry[4];
    int i, done = 0;
    int bank_min=0, bank_max=3;
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf
    };

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
        case EXACT_MATCH_2m:
        case EXACT_MATCH_4m:
            bank_min = 0;
            bank_max = 3;
            break;
        default:
            *log_bank = phy_bank;
            done = 1;
    }

    /* Shared Banks */
    if (done == 0) {
        /* Sanity */
        if (phy_bank < bank_min || phy_bank > bank_max) {
            return SOC_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EXACT_MATCH_HASH_CONTROLm, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        for (i = bank_min; i <= bank_max; i++) {
            if(soc_mem_field32_get(unit, EXACT_MATCH_HASH_CONTROLm, hash_control_entry,
                                   logical_to_physical_fields[i]) == phy_bank) {
                *log_bank = i;
                break;
            }
        }
    }

    return SOC_E_NONE;
}



/* Map logical to physical bank mapping for a memory
 *  mem         - memory 
 *  log_bank    - logical bank number (or sequence bank number)
 *  phy_bank_id - Physical bank id 
 */
int
soc_tomahawk3_log_to_phy_bank_map(int unit, soc_mem_t mem, int log_bank,
                                 int *phy_bank_id)
{
    int done = 0, bank_min = 0, bank_max = 3;
    uint32 hash_control_entry[4];

    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
    };


    switch(mem) {
    case L2Xm:
    case MPLS_ENTRY_SINGLEm:
        bank_min = 0;
        bank_max = 1;
        done = 1;
        break;

    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        bank_min = 0;
        bank_max = 3;
        done = 1;
        break;


    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        bank_min = 0;
        bank_max = 3;
        break;

    default:
        break;
    }

    if (log_bank < bank_min || log_bank > bank_max) {
        return SOC_E_PARAM;
    }

    if (!done) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EXACT_MATCH_HASH_CONTROLm, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
        *phy_bank_id = soc_mem_field32_get(unit, EXACT_MATCH_HASH_CONTROLm, hash_control_entry,
                       logical_to_physical_fields[log_bank]);
    } else {
        *phy_bank_id = log_bank;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_hash_entry_to_key(int unit, int bank, void *entry, uint8 *key, soc_mem_t mem,
                          soc_field_t *field_list)
{
    soc_field_t field;
    int         index, key_index, val_index, fval_index;
    int         right_shift_count, left_shift_count;
    uint32      val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int         bits, val_bits, fval_bits, nbits;
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
        val_bits = SOC_TH3_L2_ENTRY_MAX_KEY_WIDTH;
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        val_bits = SOC_TH3_L3_MAX_KEY_WIDTH;
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        val_bits = SOC_TH3_L3_TUNNEL_MAX_KEY_WIDTH;
        break;
    case MPLS_ENTRY_SINGLEm:
        val_bits = SOC_TH3_L3_MPLS_MAX_KEY_WIDTH;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        val_bits = SOC_TH3_EXACT_MATCH_MAX_KEY_WIDTH;
        break;
    default:
        break;
    }

    nbits = val_bits;

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        switch (mem) {
        case L2Xm:
            rbh_cfg = &(ROBUSTHASH(unit)->l2);
            break;
        case L3_ENTRY_ONLY_SINGLEm:
        case L3_ENTRY_SINGLEm:
        case L3_ENTRY_DOUBLEm:
        case L3_ENTRY_QUADm:
            rbh_cfg = &(ROBUSTHASH(unit)->l3);
            break;
        case EXACT_MATCH_4m:
        case EXACT_MATCH_2m:
            rbh_cfg = &(ROBUSTHASH(unit)->exact_match);
            break;
        case MPLS_ENTRY_SINGLEm:
            rbh_cfg = &(ROBUSTHASH(unit)->mpls_entry);
            break;
        case L3_TUNNEL_SINGLEm:
        case L3_TUNNEL_DOUBLEm:
        case L3_TUNNEL_QUADm:
            rbh_cfg = &(ROBUSTHASH(unit)->tunnel);
            break;
        default:
            break;
        }
    }

#endif /* SOC_ROBUST_HASH */
    bits = val_bits;
    val_bits = 0;

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

    bits = (((nbits - 1) / 32) + 1) * 32;

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        if (rbh_cfg) {
            if (rbh_cfg->enable) {
                (void) soc_robust_hash_get2(unit, rbh_cfg, bank, key, nbits); /* nbits = unpadded */
            }

            bits += ROBUST_HASH_KEY_SPACE_WIDTH;
        }
    }
#endif /* SOC_ROBUST_HASH */

    return bits;
}

STATIC int
_soc_tomahawk3_shared_hash(int unit, int hash_offset, uint32 key_nbits, uint8 *keyA,
                     uint8 *keyB, uint32 result_mask, uint16 lsb, int use_lsb)
{
   uint32 crc_hi, crc_lo;
   uint64 hash_vector, mask;

   if (use_lsb) {
       /* Debug Mode
        * Hash Vector [63:48] ->  Zero
        * Hash Vector [47:32] ->  Hash LSB
        */
       crc_hi = lsb;
   } else {
       crc_hi = soc_crc32b(keyB, key_nbits);
   }

   crc_lo = soc_crc32b(keyA, key_nbits);

   COMPILER_64_SET(hash_vector, crc_hi, crc_lo);
   COMPILER_64_SET(mask, 0, result_mask);
   COMPILER_64_SHR(hash_vector, hash_offset);
   COMPILER_64_AND(hash_vector, mask);
   return (int)COMPILER_64_LO(hash_vector);
}

/* Get number of banks for the hash table according to the config
 */
int
soc_tomahawk3_hash_bank_count_get(int unit, soc_mem_t mem, int *num_banks)
{
    int count;

    if (NULL == num_banks) {
        return SOC_E_PARAM;
    }

    switch (mem) {
    case L2Xm:
        /*
         * 4k single wide entries in each of 2 dedicated L2 bank
         */
        *num_banks = 2;
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        /*
         * 4k single wide entries in each of 4 dedicated L3 bank
         */
        *num_banks = 4;
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        /*
         * 16k double wide entries in each shared bank
         */
        count = soc_mem_index_count(unit, EXACT_MATCH_2m);
        *num_banks = count / (16 * 1024);
        break;

    case MPLS_ENTRY_SINGLEm:
        /*
         * 8k single wide entries in each shared bank
         */
        *num_banks = 2;
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        /*
         * 4k single wide entries in each dedicated bank
         */
        *num_banks = 4;
        break;

    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/* Map bank sequence number to physical bank number */
int
soc_tomahawk3_hash_bank_number_get(int unit, soc_mem_t mem, int seq_num,
                             int *bank_num) {
    int count;

    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_bank_count_get(unit, mem, &count));
    if (seq_num < 0 || seq_num >= count) {
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_tomahawk3_log_to_phy_bank_map(unit, mem, seq_num,
                                                        bank_num));

    return SOC_E_NONE;
}

int
soc_tomahawk3_hash_seq_number_get(int unit, soc_mem_t mem, int bank_num, int *seq_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_tomahawk3_phy_to_log_bank_map(unit, mem, bank_num,
                                                    seq_num));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_bank_count_get(unit, mem, &count));
    if (*seq_num < 0 || *seq_num >= count) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

uint32
soc_tomahawk3_tunnel_hash(int unit, int bank, int hash_offset, int use_lsb,
                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0x3FF;
    hash_bits = 10;

    if (use_lsb && (hash_offset + hash_bits > 32)) {

        switch (soc_mem_field32_get(unit, L3_TUNNEL_SINGLEm,base_entry,
                                    KEY_TYPEf)) {
        case TH3_MPLS_HASH_KEY_TYPE_MPLS:
            lsb_val = soc_mem_field32_get(unit, L3_TUNNEL_SINGLEm, base_entry,
                                          MPLS__HASH_LSBf);
            break;

        case TH3_TUNNEL_HASH_KEY_TYPE_V4:
            lsb_val = soc_mem_field32_get(unit, L3_TUNNEL_DOUBLEm, base_entry,
                                          IPV4UC__HASH_LSBf);
            break;

        case TH3_TUNNEL_HASH_KEY_TYPE_V6:
            lsb_val = soc_mem_field32_get(unit, L3_TUNNEL_QUADm, base_entry,
                                          IPV6UC__HASH_LSBf);
            break;

        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_tomahawk3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}


int
soc_tomahawk3_tunnel_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];

    switch (soc_mem_field32_get(unit, L3_TUNNEL_SINGLEm, entry, KEY_TYPEf)) {
    case TH3_TUNNEL_HASH_KEY_TYPE_MPLS:
        mem = L3_TUNNEL_SINGLEm;
        field_list[0] = MPLS__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TH3_TUNNEL_HASH_KEY_TYPE_V4:
        mem = L3_TUNNEL_DOUBLEm;
        field_list[0] = IPV4__KEY_0f;
        field_list[1] = IPV4__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH3_TUNNEL_HASH_KEY_TYPE_V6:
        mem = L3_TUNNEL_QUADm;
        field_list[0] = IPV6__KEY_0f;
        field_list[1] = IPV6__KEY_1f;
        field_list[2] = IPV6__KEY_2f;
        field_list[3] = IPV6__KEY_3f;
        field_list[4] = INVALIDf;
        break;
    default:
        return 0;
    }
    return _soc_tomahawk3_hash_entry_to_key(unit, bank, entry, key, mem, field_list);
}

uint32
soc_tomahawk3_tunnel_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_tomahawk3_tunnel_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_tomahawk3_tunnel_base_entry_to_key(unit, 1, entry, key2);
    return soc_tomahawk3_tunnel_hash(unit, bank, hash_offset, use_lsb,
                             key_nbits, entry, key1, key2);
}

uint32
soc_tomahawk3_tunnel_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_tomahawk3_hash_offset_get(unit, L3_TUNNEL_SINGLEm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tomahawk3_tunnel_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_tomahawk3_exact_match_hash(int unit, int bank, int hash_offset, int use_lsb,
                        int key_nbits, void *base_entry, uint8 *key1,
                        uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0x1fff; /* 4k buckets per shared bank for EXACT_MATCH_2m -> 8k base buckets for BM0 */
    hash_bits = 13;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry,
                                    KEY_TYPEf)) {
            case TH3_FPEM_HASH_KEY_TYPE_128B:
                lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m,
                                              base_entry, MODE128__HASH_LSBf);
                break;
            case TH3_FPEM_HASH_KEY_TYPE_160B:
                lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m,
                                              base_entry, MODE160__HASH_LSBf);
                break;
            case TH3_FPEM_HASH_KEY_TYPE_320B:
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

    return _soc_tomahawk3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                    hash_mask, lsb_val, use_lsb);
}


int
soc_tomahawk3_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];

    switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, entry, KEY_TYPEf)) {
        case TH3_FPEM_HASH_KEY_TYPE_128B:
            mem = EXACT_MATCH_2m;
            field_list[0] = MODE128__KEY_0f;
            field_list[1] = MODE128__KEY_1f;
            field_list[2] = INVALIDf;
            break;
        case TH3_FPEM_HASH_KEY_TYPE_160B:
            mem = EXACT_MATCH_2m;
            field_list[0] = MODE160__KEY_0f;
            field_list[1] = MODE160__KEY_1f;
            field_list[2] = INVALIDf;
            break;
        case TH3_FPEM_HASH_KEY_TYPE_320B:
            mem = EXACT_MATCH_4m;
            field_list[0] = MODE320__KEY_0f;
            field_list[1] = MODE320__KEY_1f;
            field_list[2] = MODE320__KEY_2f;
            field_list[3] = INVALIDf;
            break;
        default:
            return 0;
    }
    return _soc_tomahawk3_hash_entry_to_key(unit, bank, entry, key, mem, field_list);
}

uint32
soc_tomahawk3_exact_match_entry_hash(int unit, int bank, int hash_offset,
                            int use_lsb, uint32 *entry)
{
    int         key_nbits;
    uint8       key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8       key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_tomahawk3_exact_match_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_tomahawk3_exact_match_base_entry_to_key(unit, 1, entry, key2);

    return soc_tomahawk3_exact_match_hash(unit, bank, hash_offset, use_lsb,
                                    key_nbits, entry, key1, key2);
}

int
soc_tomahawk3_mpls_base_entry_to_key(int unit, int bank, void *entry, uint8 *key) {

    soc_field_t field_list[2];
    switch (soc_mem_field32_get(unit, MPLS_ENTRY_SINGLEm, entry, KEY_TYPEf)) {
        case TH3_MPLS_HASH_KEY_TYPE_MPLS:
            
            field_list[0] = MPLS__KEYf;
            break;
        default:
            return 0;
    }

    field_list[1] = INVALIDf;
    return _soc_tomahawk3_hash_entry_to_key(unit, bank, entry, key,
                                      MPLS_ENTRY_SINGLEm, field_list);
}

uint32
soc_tomahawk3_mpls_hash(int unit, int bank, int hash_offset, int use_lsb,
                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0x7FF; /* 2k buckets per shared bank of size 8k */
    hash_bits = 11;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, MPLS_ENTRY_SINGLEm, base_entry,
                                    KEY_TYPEf)) {
        case TH3_MPLS_HASH_KEY_TYPE_MPLS:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRY_SINGLEm, base_entry,
                                          MPLS__HASH_LSBf);
            break;

        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_tomahawk3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

uint32
soc_tomahawk3_mpls_entry_hash(int unit, int bank, int hash_offset,
                        int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_tomahawk3_mpls_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_tomahawk3_mpls_base_entry_to_key(unit, 1, entry, key2);

    return soc_tomahawk3_mpls_hash(unit, bank, hash_offset, use_lsb,
                             key_nbits, entry, key1, key2);
}

uint32
soc_tomahawk3_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_tomahawk3_hash_offset_get(unit, MPLS_ENTRY_SINGLEm, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tomahawk3_mpls_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

int
soc_tomahawk3_hash_bank_info_get(int unit, soc_mem_t mem, int bank,
                                int *entries_per_bank, int *entries_per_row,
                                int *entries_per_bucket, int *bank_base,
                                int *bucket_offset)
{
    int bank_size, row_size, bucket_size, base, offset;
    int seq_num = 0;

    /* Get logical bank number */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_phy_to_log_bank_map(unit, mem, bank, &seq_num));
    bank = seq_num;

    /* bank_size = logical entries per bank 
     * row_size = logical entries per base bucket
     * bucket_size = logical entries per logical bucket = 4
     * offset = 0 since this is bank_info
     */

    switch (mem) {
    case L2Xm:
        bank_size = 4096;
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        base = bank * bank_size;
        break;

    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        bucket_size = 4;
        offset = 0;
        if (mem == L3_ENTRY_QUADm) {
            row_size = 1;
            bank_size = 4096/4;
        } else if (mem == L3_ENTRY_DOUBLEm) {
            row_size = 2;
            bank_size = 4096/2;
        } else {
            row_size = 4;
            bank_size = 4096;
        }
        base = bank * bank_size;
        break;

    case EXACT_MATCH_2m:
        row_size = 2;
        bucket_size = 4;
        offset = 0;
        bank_size = 16384;
        base = bank * bank_size;
        break;

    case EXACT_MATCH_4m:
        row_size = 1;
        bucket_size = 4;
        offset = 0;
        bank_size = 8192;
        base = bank * bank_size;
        break;

    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        bucket_size = 4;
        offset = 0;
        if (mem == L3_TUNNEL_QUADm) {
            row_size = 1;
            bank_size = 4096/4;
        } else if (mem == L3_TUNNEL_DOUBLEm) {
            row_size = 2;
            bank_size = 4096/2;
        } else {
            row_size = 4;
            bank_size = 4096;
        }
        base = bank * bank_size;
        break;

    case MPLS_ENTRY_SINGLEm:
        bank_size = 8192;
        row_size = 1;
        bucket_size = 4;
        offset = 0;
        base = bank_size * bank;
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

int
soc_tomahawk3_hash_offset_get(int unit, soc_mem_t mem, int bank, int *hash_offset,
                        int *use_lsb)
{
    uint32 hash_control_entry[4];
    int is_shared = 1;
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;
    static const soc_field_t uft_fields_d[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        HASH_OFFSET_DEDICATED_BANK_2f, HASH_OFFSET_DEDICATED_BANK_3f

    };
    static const soc_field_t uft_fields_s[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf
    };


    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        if (bank < 0 || bank > 1) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EXACT_MATCH_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields_s[bank];
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = L3_TUNNEL_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_ONLY_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    case MPLS_ENTRY_SINGLEm:
        if (bank < 0 || bank > 1) {
            return SOC_E_PARAM;
        }
        mem = MPLS_ENTRY_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
    if (use_lsb) {
        *use_lsb = soc_mem_field32_get(
            unit, mem, hash_control_entry, HASH_TABLE_TEST_MODEf);
    }

    /* Shared Banks */
    if (is_shared > 0) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, shared_mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        *hash_offset = soc_mem_field32_get(unit, shared_mem,
                                           hash_control_entry, field);
    } else {
        *hash_offset = soc_mem_field32_get(unit, mem, hash_control_entry, field);
    }

    return SOC_E_NONE;
}

int
soc_tomahawk3_hash_offset_set(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb)
{
    uint32 hash_control_entry[4];
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;
    int is_shared = 1;

    static const soc_field_t uft_fields_d[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        HASH_OFFSET_DEDICATED_BANK_2f, HASH_OFFSET_DEDICATED_BANK_3f

    };
    static const soc_field_t uft_fields_s[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf
    };


    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        if (bank < 0 || bank > 1) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EXACT_MATCH_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields_s[bank];
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = L3_TUNNEL_HASH_CONTROLm;
        field = uft_fields_d[bank];
        is_shared = 0;
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_ONLY_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        is_shared = 0;
        field = uft_fields_d[bank];
        break;
    case MPLS_ENTRY_SINGLEm:
        if (bank < 0 || bank > 1) {
            return SOC_E_PARAM;
        }
        mem = MPLS_ENTRY_HASH_CONTROLm;
        is_shared = 0;
        field = uft_fields_d[bank];
        break;
    default:
        return SOC_E_INTERNAL;
    }

    /* Shared Banks */
    if (is_shared > 0) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, shared_mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        soc_mem_field32_set(unit, shared_mem, hash_control_entry, field, hash_offset);
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, shared_mem, MEM_BLOCK_ALL, 0, hash_control_entry));
    } else {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        soc_mem_field32_set(unit, mem, hash_control_entry, HASH_TABLE_TEST_MODEf,
                        use_lsb);
        soc_mem_field32_set(unit, mem, hash_control_entry, field, hash_offset);
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, hash_control_entry));
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_hash_index_get(int unit, int mem, int bank, int bucket)
{
    int rv;
    int index = 0;
    int entries_per_row, bank_base, bucket_offset, entries_per_bucket;

    switch(mem) {
    case MPLS_ENTRY_SINGLEm:
        rv = soc_tomahawk3_hash_bank_info_get(unit, mem, bank, NULL,
                                        NULL, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_bucket + bucket_offset;
        }
        break;

    default:
        rv = soc_tomahawk3_hash_bank_info_get(unit, mem, bank, NULL,
                                        &entries_per_row, NULL, &bank_base,
                                        &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_row + bucket_offset;
            return index;
        }
        break;
    }

    return index;
}


uint32
soc_tomahawk3_l2x_hash(int unit, int bank, int hash_offset, int use_lsb,
                int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint16 lsb_val;
    uint32 hash_mask = 0x03FF; /* 1k buckets per dedicated L2 bank */
    uint32 hash_bits = 10;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
        case TH3_L2_HASH_KEY_TYPE_BRIDGE:
            lsb_val =
                soc_mem_field32_get(unit, L2Xm, base_entry, L2__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_tomahawk3_shared_hash(unit,
            hash_offset, key_nbits, key1, key2, hash_mask, lsb_val, use_lsb);
}

int
soc_tomahawk3_l2x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[2];

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case TH3_L2_HASH_KEY_TYPE_BRIDGE:
        field_list[0] = L2__KEYf;
        break;
    case TH3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        field_list[0] = VLAN__KEYf;
        break;
    default:
        return 0;
    }
    field_list[1] = INVALIDf;
    return _soc_tomahawk3_hash_entry_to_key(unit, bank, entry, key, L2Xm, field_list);
}

uint32
soc_tomahawk3_l2x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                       uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    key_nbits = soc_tomahawk3_l2x_base_entry_to_key(unit, 0, entry, key1);
    key_nbits = soc_tomahawk3_l2x_base_entry_to_key(unit, 1, entry, key2);
    return soc_tomahawk3_l2x_hash(
        unit, bank, hash_offset, use_lsb, key_nbits, entry, key1, key2);
}

uint32
soc_tomahawk3_l2x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_tomahawk3_hash_offset_get(unit, L2Xm, bank, &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tomahawk3_l2x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

int
soc_tomahawk3_hash_init(int unit)
{
#define KEY_ATTR_MEM_FIELD_CNT  5
    int index;
    int mem_idx, field_idx;
    int shared_bank_offset = 0;
    int num_fpem_banks = 0;
    int num_dedicated_banks = 2;
    int offset_base, offset_multiplier;
    uint32 fields_val[KEY_ATTR_MEM_FIELD_CNT] = {0};
    uint32 hash_control_entry[SOC_MAX_MEM_WORDS];
    uft_shared_banks_control_entry_t shared_hash_control_entry;
    static const soc_mem_t dedicated_bank_mem[][2] = {
        { L2Xm,                 L2_ENTRY_HASH_CONTROLm },
        { L3_ENTRY_SINGLEm,     L3_ENTRY_HASH_CONTROLm },
        { MPLS_ENTRY_SINGLEm,   MPLS_ENTRY_HASH_CONTROLm },
        { L3_TUNNEL_SINGLEm,    L3_TUNNEL_HASH_CONTROLm }
    };
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf
    };
    static const soc_field_t dedicated_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        HASH_OFFSET_DEDICATED_BANK_2f, HASH_OFFSET_DEDICATED_BANK_3f
    };
    static const soc_field_t shared_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf
    };

    /*            PARAMETERS FOR <xyz>_KEY_ATTRIBUTE MEMORY
     *
     * NOTE: 1) Each supported KEY_TYPE for the mem should have one entry.
     *       2) All entries to the array should be sequential in terms of
     *          the KEY_TYPE value, i.e. if a memory supports 2 keys with
     *          values 0 and 2 then add three entries to the table with 2nd
     *          entry as all 0's and comment as 'Not Supported'. See
     *          l3_key_type_attr below as an example.
     *       3) If a new KEY TYPE is defined later, please add an entry below
     *
     * The below defined <XYZ>_key_type_attr array declares the values
     * for these five fields from the respective KEY_ATTRIBUTE memory,
     * { BUCKET |KEY   |KEY       |DATA      |HASH      }
     * { MODE   |WIDTH |BASEWIDTH |BASEWIDTH |LSBOFFSET }
     */
    static const uint8 l2x_key_type_attr[][KEY_ATTR_MEM_FIELD_CNT] = {
       { 0      ,15    ,0         ,0         ,0 }, /* BRIDGE */
       { 0      ,6     ,0         ,0         ,0 }  /* SINGLE X CONNECT VLAN */
    };
    static const uint8 l3_key_type_attr[][KEY_ATTR_MEM_FIELD_CNT] = {
       { 0      ,11    ,0         ,1         ,0 }, /* IPV4UC */
       { 0      ,0     ,0         ,0         ,0 }, /* Not Supported */
       { 0      ,15    ,1         ,2         ,0 }, /* IPV6UC */
       { 0      ,0     ,0         ,0         ,0 }, /* Not Supported */
       { 0      ,22    ,0         ,1         ,0 }, /* IPV4MC */
       { 0      ,23    ,2         ,1         ,0 }  /* IPV6MC */
    };
    static const uint8 mpls_key_type_attr[][KEY_ATTR_MEM_FIELD_CNT] = {
       { 0      ,7     ,0         ,0         ,0 }  /* MPLS */
    };
    static const uint8 tunnel_key_type_attr[][KEY_ATTR_MEM_FIELD_CNT] = {
       { 0      ,7     ,0         ,0         ,0 }, /* MPLS */
       { 0      ,3     ,1         ,0         ,0 }, /* IPV4 */
       { 0      ,7     ,3         ,0         ,0 }  /* IPv6 */
    };
    static const uint8 fpem_key_type_attr[][KEY_ATTR_MEM_FIELD_CNT] = {
       { 0      ,3     ,1         ,0         ,0 }, /* MODE128 */
       { 0      ,11    ,1         ,0         ,0 }, /* MODE160 */
       { 0      ,21    ,2         ,1         ,0 }  /* MODE320 */
    };
    static const struct key_type_attr_mem_mappings_s {
        soc_mem_t   mem;          /* Key Attribut Memory */
        int         key_cnt;      /* KeyType Max count from the enums */
        const uint8 (*map)[KEY_ATTR_MEM_FIELD_CNT]; /* key attr fields vals */
    } key_attr_map[] = {
        {   L2_ENTRY_KEY_ATTRIBUTESm,
                TH3_L2_HASH_KEY_TYPE_COUNT, l2x_key_type_attr          },
        {   L3_ENTRY_KEY_ATTRIBUTESm,
                TH3_L3_HASH_KEY_TYPE_COUNT, l3_key_type_attr           },
        {   MPLS_ENTRY_KEY_ATTRIBUTESm,
                TH3_MPLS_HASH_KEY_TYPE_COUNT, mpls_key_type_attr       },
        {   L3_TUNNEL_KEY_ATTRIBUTESm,
                TH3_TUNNEL_HASH_KEY_TYPE_COUNT, tunnel_key_type_attr   },
        {   EXACT_MATCH_KEY_ATTRIBUTESm,
                TH3_FPEM_HASH_KEY_TYPE_COUNT, fpem_key_type_attr       }
    };
    soc_field_t key_attr_mem_fields[KEY_ATTR_MEM_FIELD_CNT] = {
        BUCKET_MODEf,
        KEY_WIDTHf,
        KEY_BASE_WIDTHf,
        DATA_BASE_WIDTHf,
        HASH_LSB_OFFSETf
    };

#if 0
    
    if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        int non_alpm = num_fpem_banks + num_l3_banks +
                       num_l2_banks - 2 * (num_dedicated_banks);
        sal_memset(hash_control_entry, 0,
                    sizeof(uft_shared_banks_control_entry_t));
        SOC_IF_ERROR_RETURN
        (READ_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
        if ((non_alpm) && (non_alpm <= 4)) {
            /* If Shared banks are used between ALPM and non-ALPM memories,
             * then ALPM uses Shared Bank B2, B3, B4, B5 and non-ALPM uses
             * B6, B7, B8, B9 banks
             */
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, ALPM_MODEf, 1);
            num_shared_alpm_banks[unit] = 4;
            shared_bank_offset += 4;
        } else {
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, ALPM_MODEf, 0);
            num_shared_alpm_banks[unit] = 8;
        }
        SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             hash_control_entry));
    }
#endif

    /* Program the hash memories which has dedicated banks */
    for (mem_idx = 0 ; mem_idx < COUNTOF(dedicated_bank_mem) ; mem_idx++) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_hash_bank_count_get(
                unit, dedicated_bank_mem[mem_idx][0], &num_dedicated_banks));

        offset_multiplier = 64 / num_dedicated_banks;
        offset_base = 64 % num_dedicated_banks;

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit,
                         dedicated_bank_mem[mem_idx][1],
                         MEM_BLOCK_ANY, 0, hash_control_entry));
        soc_mem_field32_set(
            unit, dedicated_bank_mem[mem_idx][1], hash_control_entry,
            HASH_TABLE_BANK_CONFIGf, ((1 << num_dedicated_banks) - 1));

        for (index = 0; index < num_dedicated_banks; index++) {
            soc_mem_field32_set(
                unit, dedicated_bank_mem[mem_idx][1], hash_control_entry,
                dedicated_fields[index], offset_base+(index*offset_multiplier));
        }
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit,
                            dedicated_bank_mem[mem_idx][1],
                            MEM_BLOCK_ALL, 0, hash_control_entry));
    }

    /* Program the hash memories which has shared banks */
    sal_memset(hash_control_entry, 0, sizeof(exact_match_hash_control_entry_t));
    sal_memset(&shared_hash_control_entry, 0,
                sizeof(uft_shared_banks_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EXACT_MATCH_HASH_CONTROLm(
            unit, MEM_BLOCK_ANY, 0, hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_UFT_SHARED_BANKS_CONTROLm(
            unit, MEM_BLOCK_ANY, 0, &shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_hash_bank_count_get(
            unit, EXACT_MATCH_2m, &num_fpem_banks));

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, hash_control_entry,
                      HASH_TABLE_BANK_CONFIGf,
                      ((1 << num_fpem_banks) - 1) << shared_bank_offset);
    /* Handle case where fpem banks are set to 0 */
    if (num_fpem_banks) {
        offset_multiplier = 64 / num_fpem_banks;
        offset_base = 64 % num_fpem_banks;
    }
    for (index = 0 ; index < num_fpem_banks; index++) {
        soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm , hash_control_entry,
                          logical_to_physical_fields[index], index +
                          shared_bank_offset);
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            &shared_hash_control_entry,
                            shared_fields[shared_bank_offset + index],
                            offset_base + (index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                            hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        &shared_hash_control_entry));

    /* Program the KEY_ATTRIBUTE memories */
    for (mem_idx = 0 ; mem_idx < COUNTOF(key_attr_map) ; mem_idx++) {
        /* Clear the memory to begin-with */
        

        /* Write the relevant entries in it */
        for (index = 0 ; index < key_attr_map[mem_idx].key_cnt ; index++) {
            for (field_idx=0 ; field_idx<KEY_ATTR_MEM_FIELD_CNT ; field_idx++) {
                fields_val[field_idx] =
                    (key_attr_map[mem_idx].map)[index][field_idx];
            }
            SOC_IF_ERROR_RETURN(
                soc_mem_fields32_modify(unit, key_attr_map[mem_idx].mem, index,
                      KEY_ATTR_MEM_FIELD_CNT, key_attr_mem_fields, fields_val));
        }
    }
#undef KEY_ATTR_MEM_FIELD_CNT

/* Robust Hash initialization */
#ifdef SOC_ROBUST_HASH

    /* Initialize Robust Hash Global Datastructure */
    if (soc_feature(unit, soc_feature_robust_hash)) {
        uint32 seed = 0;
        uint32 hash_control_entry[4];

        if (NULL == ROBUSTHASH(unit)) {
            ROBUSTHASH(unit) = sal_alloc(sizeof(soc_robust_hash_db_t),
                                         "soc_robust_hash");
            if (ROBUSTHASH(unit) == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memset(ROBUSTHASH(unit), 0, sizeof(soc_robust_hash_db_t));
        }

        /* L2 Robust Hash Init */

        sal_memset(hash_control_entry, 0, sizeof(l2_entry_hash_control_entry_t));

        /* Disable robust hash for L2 - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get(unit, "robust_hash_disable_l2", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_L2_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l2.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
            soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l2.enable = 1;

            /* Configure remap and action tables for L2 table */
            ROBUSTHASH(unit)->l2.remap_tab[0] = L2_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->l2.remap_tab[1] = L2_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->l2.action_tab[0] = L2_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->l2.action_tab[1] = L2_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_L2, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->l2),
                                           seed));
        }

        /* L3 Robust Hash Init */

        sal_memset(hash_control_entry, 0, sizeof(l3_entry_hash_control_entry_t));

        /* Disable robust hash for L3 - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get(unit, "robust_hash_disable_l3", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_L3_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l3.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
            soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l3.enable = 1;

            /* Configure remap and action tables for L3 table */
            ROBUSTHASH(unit)->l3.remap_tab[0] = L3_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->l3.remap_tab[1] = L3_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->l3.action_tab[0] = L3_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->l3.action_tab[1] = L3_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_L3, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->l3),
                                           seed));
        }

        /* Exact Match Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(exact_match_hash_control_entry_t));

        /* Disable robust hash for EM - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_exact_match", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_EXACT_MATCH_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->exact_match.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
            soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                 hash_control_entry));
            ROBUSTHASH(unit)->exact_match.enable = 1;

            /* Configure remap and action tables for EM table */
            ROBUSTHASH(unit)->exact_match.remap_tab[0] = EXACT_MATCH_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->exact_match.remap_tab[1] = EXACT_MATCH_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->exact_match.action_tab[0] = EXACT_MATCH_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->exact_match.action_tab[1] = EXACT_MATCH_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_EXACT_MATCH, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->exact_match),
                                           seed));
        }

        /* Mpls Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(mpls_entry_hash_control_entry_t));

        /* Disable robust hash for mpls - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_mpls", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                        hash_control_entry));
            ROBUSTHASH(unit)->mpls_entry.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                     hash_control_entry));
            soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                      hash_control_entry));
            ROBUSTHASH(unit)->mpls_entry.enable = 1;

            /* Configure remap and action tables for MPLS ENTRY table */
            ROBUSTHASH(unit)->mpls_entry.remap_tab[0] =
                                                MPLS_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.remap_tab[1] =
                                                MPLS_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->mpls_entry.action_tab[0] =
                                                MPLS_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.action_tab[1] =
                                                MPLS_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_MPLS, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->mpls_entry),
                                           seed));
        }


        /* Tunnel Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(tunnel_hash_control_entry_t));

        /* Disable robust hash for mpls - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_mpls", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_L3_TUNNEL_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, L3_TUNNEL_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_TUNNEL_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                        hash_control_entry));
            ROBUSTHASH(unit)->tunnel.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_L3_TUNNEL_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                     hash_control_entry));
            soc_mem_field32_set(unit, L3_TUNNEL_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_TUNNEL_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                      hash_control_entry));
            ROBUSTHASH(unit)->tunnel.enable = 1;

            /* Configure remap and action tables for L3_TUNNEL ENTRY table */
            ROBUSTHASH(unit)->tunnel.remap_tab[0] =
                                                L3_TUNNEL_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->tunnel.remap_tab[1] =
                                                L3_TUNNEL_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->tunnel.action_tab[0] =
                                                L3_TUNNEL_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->tunnel.action_tab[1] =
                                                L3_TUNNEL_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_L3_TUNNEL, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->tunnel),
                                           seed));
        }



    }
#endif /* SOC_ROBUST_HASH */

    return SOC_E_NONE;
}

int
soc_tomahawk3_shared_hash_mem_bucket_read(int unit, int ent, int entry_width, int *width,
                                    soc_mem_t base_mem, soc_mem_t *mem,
                                    void *bkt_entry, void *entry)
{

    soc_mem_info_t meminfo;
    soc_field_info_t fieldinfo;
    /* Exclude HIT bit */
    int entry_bits = 0, hit_bits=0;

    if (entry_width <= 0) {
        return (SOC_E_PARAM);
    }

    switch (base_mem) {
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        switch (entry_width) {
        case 1:  /* 1 base_entry wide */
            *width = 1;
            *mem = L3_ENTRY_SINGLEm;
            break;
        case 2:  /* 2 base entries wide */
            *width = 2;
            *mem = L3_ENTRY_DOUBLEm;
            break;
        default:
            *width = 4; /* 4 base_entries wide */
            *mem = L3_ENTRY_QUADm;
        }
        hit_bits = *width;
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        switch (entry_width) {
            case 2:
                *width = 1;
                *mem = EXACT_MATCH_2m;
                break;
            case 4:
            default:
                *width = 2;
                *mem = EXACT_MATCH_4m;
        }
        break;

    case MPLS_ENTRY_SINGLEm:
        *width = 1;
        *mem = MPLS_ENTRY_SINGLEm;
        break;

    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        switch (entry_width) {
        case 1:  /* 1 base_entry wide */
            *width = 1;
            *mem = L3_TUNNEL_SINGLEm;
            break;
        case 2:  /* 2 base entries wide */
            *width = 2;
            *mem = L3_TUNNEL_DOUBLEm;
            break;
        default:
            *width = 4; /* 4 base_entries wide */
            *mem = L3_TUNNEL_QUADm;
        }
        break;

    default:
        return SOC_E_PARAM;
    }

    entry_bits = soc_mem_entry_bits(unit, *mem) - hit_bits;

    meminfo.flags = 0;
    meminfo.bytes = SOC_MEM_WORDS(unit, base_mem) * 4;
    fieldinfo.flags = SOCF_LE;
    fieldinfo.len = entry_bits;

    fieldinfo.bp =  entry_bits * (ent/entry_width);
    (void)soc_meminfo_fieldinfo_field_get(bkt_entry, &meminfo,
                                          &fieldinfo, entry);

    return SOC_E_NONE;

}


int
soc_tomahawk3_hash_mem_status_get(int unit, soc_mem_t mem, void* entry, uint32 *v) {
    int i;
    v[0] = v[1] = v[2] = v[3] = 0;

    switch (mem) {
    case L3_ENTRY_QUADm:
    case L3_TUNNEL_QUADm:
    case EXACT_MATCH_4m:
        soc_mem_field_get(unit, mem, entry, BASE_VALID_3f, &v[3]);
        soc_mem_field_get(unit, mem, entry, BASE_VALID_2f, &v[2]);
        /* pass through */
    case L3_ENTRY_DOUBLEm:
    case L3_TUNNEL_DOUBLEm:
    case EXACT_MATCH_2m:
        soc_mem_field_get(unit, mem, entry, BASE_VALID_1f, &v[1]);
        soc_mem_field_get(unit, mem, entry, BASE_VALID_0f, &v[0]);
        break;
    case L3_ENTRY_SINGLEm:
    case L3_TUNNEL_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case MPLS_ENTRY_SINGLEm:
        soc_mem_field_get(unit, mem, entry, BASE_VALIDf, &v[0]);
        break;
    default:
        v[0] = v[1] = v[2] = v[3] = 1;
        break;

    }
    /* Use v[i] for constructing a valid bitmap */
    for (i = 0; i < 4; i++) {
        v[i] = (v[i] > 0);
    }
    return SOC_E_NONE;
}


/* Get bank bitmap for the hash table
 * Required only for shared hash tables
 */
int
soc_tomahawk3_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem, uint32 *bitmap)
{
    uint32 hash_control_entry[4];
    switch(mem) {
    case L2Xm:
        mem = L2_ENTRY_HASH_CONTROLm;
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLY_QUADm:
        mem = L3_ENTRY_HASH_CONTROLm;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        mem = EXACT_MATCH_HASH_CONTROLm;
        break;
    case MPLS_ENTRY_SINGLEm:
        mem = MPLS_ENTRY_HASH_CONTROLm;
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        mem = L3_TUNNEL_HASH_CONTROLm;
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
    *bitmap = soc_mem_field32_get(unit, mem, hash_control_entry,
                                  HASH_TABLE_BANK_CONFIGf);

    return SOC_E_NONE;
}

uint32
soc_tomahawk3_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

     hash_mask = 0x3FF; /* 1k buckets per dedicated banks */
     hash_bits = 10;


    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, L3_ENTRY_SINGLEm, base_entry,
                                    KEY_TYPEf)) {
        case TH3_L3_HASH_KEY_TYPE_V4UC:
        case TH3_L3_HASH_KEY_TYPE_V4MC:
        case TH3_L3_HASH_KEY_TYPE_V6UC:
        case TH3_L3_HASH_KEY_TYPE_V6MC:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_SINGLEm,
                                          base_entry, IPV4UC__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_tomahawk3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_tomahawk3_l3x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];

    switch (soc_mem_field32_get(unit, L3_ENTRY_SINGLEm, entry, KEY_TYPEf)) {
    case TH3_L3_HASH_KEY_TYPE_V4UC:
        mem = L3_ENTRY_SINGLEm;
        field_list[0] = IPV4UC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TH3_L3_HASH_KEY_TYPE_V6UC:
        mem = L3_ENTRY_DOUBLEm;
        field_list[0] = IPV6UC__KEY_0f;
        field_list[1] = IPV6UC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TH3_L3_HASH_KEY_TYPE_V4MC:
        mem = L3_ENTRY_DOUBLEm;
        field_list[0] = IPV4MC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TH3_L3_HASH_KEY_TYPE_V6MC:
        mem = L3_ENTRY_QUADm;
        field_list[0] = IPV6MC__KEY_0f;
        field_list[1] = IPV6MC__KEY_1f;
        field_list[2] = IPV6MC__KEY_2f;
        field_list[3] = INVALIDf;
        break;

    default:
        return 0;
    }
    return _soc_tomahawk3_hash_entry_to_key(unit, bank, entry, key, mem, field_list);
}

uint32
soc_tomahawk3_l3x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_tomahawk3_l3x_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_tomahawk3_l3x_base_entry_to_key(unit, 1, entry, key2);

    return soc_tomahawk3_l3x_hash(unit, bank, hash_offset, use_lsb, key_nbits,
                            entry, key1, key2);
}

uint32
soc_tomahawk3_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_tomahawk3_hash_offset_get(unit, L3_ENTRY_SINGLEm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tomahawk3_l3x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_tomahawk3_exact_match_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_tomahawk3_hash_offset_get(unit, EXACT_MATCH_2m, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_tomahawk3_exact_match_entry_hash(unit, bank, hash_offset, use_lsb,
                                          entry);
}

int
soc_tomahawk3_hash_bucket_get(int unit, int mem, int bank, uint32 *entry,
                        uint32 *bucket)
{
    /* The bits in the indexes cannot be limited as TH3 operates in BM0 */
    switch (mem) {
    case L2Xm:
        *bucket = soc_tomahawk3_l2x_bank_entry_hash(unit, bank, entry);
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        *bucket = soc_tomahawk3_l3x_bank_entry_hash(unit, bank, entry);
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        *bucket = soc_tomahawk3_exact_match_bank_entry_hash(unit, bank, entry);
        break;
    case MPLS_ENTRY_SINGLEm:
        *bucket = soc_tomahawk3_mpls_bank_entry_hash(unit, bank, entry);
        break;
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        *bucket = soc_tomahawk3_tunnel_bank_entry_hash(unit, bank, entry);
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}



int
soc_tomahawk3_tbl_entries_per_sram_entry_get(int unit, soc_mem_t mem,
                       int *table_entries_per_sram_entry)
{
    switch (mem) {
    case L2Xm:
    case L2_ENTRY_SINGLEm:
    case L2_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_TUNNEL_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
         *table_entries_per_sram_entry = 4;
         break;

    case L3_ENTRY_DOUBLEm:
    case L3_TUNNEL_DOUBLEm:
    case EXACT_MATCH_2m:
         *table_entries_per_sram_entry = 2;
         break;

    case L3_ENTRY_QUADm:
    case L3_TUNNEL_QUADm:
    case EXACT_MATCH_4m:
         *table_entries_per_sram_entry = 1;
         break;

    case MPLS_ENTRY_SINGLEm:
         *table_entries_per_sram_entry = 1;
         break;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

