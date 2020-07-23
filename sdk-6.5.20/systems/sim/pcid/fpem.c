/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fpem.c
 * Purpose:     Exact match table support
 *
 * Provides:
 *      soc_internal_exact_match_read
 *      soc_internal_exact_match_write
 *      soc_internal_exact_match_ins
 *      soc_internal_exact_match_del
 *      soc_internal_exact_match_lkup
 */

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#include <shared/bsl.h>
#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"

#ifdef BCM_TOMAHAWK_SUPPORT

enum {
    _ENTRY0f,
    _ENTRY1f
} soc_EXACT_MATCH_DUMMYm_ftype;

static soc_field_info_t soc_EXACT_MATCH_DUMMYm_fields[] = {
#ifdef BCM_TOMAHAWK3_SUPPORT
    { _ENTRY0f, 240, 240, SOCF_LE },
    { _ENTRY1f, 240,   0, SOCF_LE }
#else
    { _ENTRY0f, 0, 0, SOCF_LE },
    { _ENTRY1f, 0, 0, SOCF_LE }
#endif
};
static soc_mem_info_t exact_match_minfo = {
    /* mem            EXACT_MATCH_?m */
    /* flags       */ 0,
    /* cmp_fn      */ _soc_mem_cmp_undef,
    /* *null_entry */ _soc_mem_entry_null_zeroes,
    /* index_min   */ 0,
    /* index_max   */ 0,
    /* minblock    */ 0,
    /* maxblock    */ 0,
    /* blocks      */ 0,
    /* blocks_hi   */ 0,
    /* base        */ 0,
    /* gran        */ 1,
#ifdef BCM_TOMAHAWK3_SUPPORT
    /* data_bits */ 480,
    /* bytes       */ 60,
#else
    /* data_bits */ 424,
    /* bytes       */ 53,
#endif
    /* nFields     */ 2, /* Used */
    /* *fields     */ soc_EXACT_MATCH_DUMMYm_fields, /* Used */
};

int
soc_internal_exact_match_read(pcid_info_t * pcid_info, soc_mem_t mem,
                              uint32 addr, uint32 *entry)
{
    int         unit;
    int         offset;
    uint32      tmp_entry[SOC_MAX_MEM_WORDS];
    uint32      fval;
    int         bit;
    int         bucket;
    int         index;
    int 	bucket_size;
    int         block_info_index;
    soc_block_t block;
    uint8       acc_type;
    soc_mem_t   alt_mem;
    uint32      alt_addr;
    static soc_field_t hit_fields[] = { HIT_0f, HIT_1f };

    unit = pcid_info->unit;
    offset = addr ^ SOC_MEM_INFO(unit, mem).base;
    sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));

    bucket_size = 2;

    /*
     * Fix exact_match_minfo using info from EXACT_MATCH_*m table
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
            soc_EXACT_MATCH_DUMMYm_fields[0].len =
            soc_EXACT_MATCH_DUMMYm_fields[1].len =
            soc_mem_entry_bits(unit, EXACT_MATCH_2m);
        }
        else
#endif
        {
            soc_EXACT_MATCH_DUMMYm_fields[0].len =
            soc_EXACT_MATCH_DUMMYm_fields[1].len =
            soc_mem_entry_bits(unit, EXACT_MATCH_2_ENTRY_ONLYm);
        }
    soc_EXACT_MATCH_DUMMYm_fields[0].bp = 0;
    soc_EXACT_MATCH_DUMMYm_fields[1].bp =
        soc_EXACT_MATCH_DUMMYm_fields[0].len;

    switch (mem) {
        case EXACT_MATCH_4m:
            /* Uses two slots per entry */
            index = offset * 2;
            bucket = index / bucket_size;

            /* Read data only portion of content */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                alt_mem = EXACT_MATCH_2m;
            } else
#endif
            {
                alt_mem = EXACT_MATCH_2_ENTRY_ONLYm;
            }
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            /* Read lower half of data only entry */
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           entry);
            /* Read upper half of data only entry */
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index + 1, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           tmp_entry);
            soc_meminfo_field_set(mem, &exact_match_minfo, entry, _ENTRY1f,
                                  tmp_entry);

            /* Read hit bit portion of content */
            alt_mem = EXACT_MATCH_HIT_ONLYm;
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, bucket, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           tmp_entry);
            fval = soc_mem_field32_get(unit, alt_mem, tmp_entry, HIT_0f);
            soc_mem_field32_set(unit, mem, entry, HIT_0f, fval);
            fval = soc_mem_field32_get(unit, alt_mem, tmp_entry, HIT_1f);
            soc_mem_field32_set(unit, mem, entry, HIT_1f, fval);
            break;

        case EXACT_MATCH_2m:
            /* Uses one slot per entry */
            index = offset;
            bucket = index / bucket_size;
            bit = index % bucket_size;

            /* Read data only portion of content */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                alt_mem = EXACT_MATCH_2m;
            } else
#endif
            {
                alt_mem = EXACT_MATCH_2_ENTRY_ONLYm;
            }
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           entry);

            /* Read hit bit portion of content */
            alt_mem = EXACT_MATCH_HIT_ONLYm;
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, bucket, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           tmp_entry);
            fval = soc_mem_field32_get(unit, alt_mem, tmp_entry, hit_fields[bit]);
            soc_mem_field32_set(unit, mem, entry, HIT_0f, fval);
            break;

        default:
            return -1;
    }

    return 0;
}

int
soc_internal_exact_match_write(pcid_info_t * pcid_info, soc_mem_t mem,
                               uint32 addr, uint32 *entry)
{
    int         unit;
    int         offset;
    uint32      tmp_entry[SOC_MAX_MEM_WORDS];
    uint32      fval;
    int         bit;
    int         bucket;
    int         index;
    int		bucket_size;
    int         block_info_index;
    soc_block_t block;
    uint8       acc_type;
    soc_mem_t   alt_mem;
    uint32      alt_addr;
    static soc_field_t hit_fields[] = { HIT_0f, HIT_1f };

    unit = pcid_info->unit;
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;

    bucket_size = 2;

    /*
     * Fix exact_match_minfo using info from EXACT_MATCH_*m table
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        soc_EXACT_MATCH_DUMMYm_fields[0].len =
        soc_EXACT_MATCH_DUMMYm_fields[1].len =
        soc_mem_entry_bits(unit, EXACT_MATCH_2m);
    }
    else
#endif
    {
        soc_EXACT_MATCH_DUMMYm_fields[0].len =
        soc_EXACT_MATCH_DUMMYm_fields[1].len =
        soc_mem_entry_bits(unit, EXACT_MATCH_2_ENTRY_ONLYm);
    }
    soc_EXACT_MATCH_DUMMYm_fields[0].bp = 0;
    soc_EXACT_MATCH_DUMMYm_fields[1].bp =
        soc_EXACT_MATCH_DUMMYm_fields[0].len;

    switch(mem) {
        case EXACT_MATCH_4m:
            /* Uses two slots per entry */
            index = offset * 2;
            bucket = index / bucket_size;

            /* Write data only portion of content */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                alt_mem = EXACT_MATCH_2m;
            } else
#endif
            {
                alt_mem = EXACT_MATCH_2_ENTRY_ONLYm;
            }
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            /* Write lower half of data only entry */
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index, &acc_type);
            soc_meminfo_field_get(mem, &exact_match_minfo, entry, _ENTRY0f,
                                  tmp_entry, SOC_MAX_MEM_WORDS);
            soc_internal_extended_write_mem(pcid_info, block, acc_type, alt_addr,
                                            tmp_entry);
            /* Write upper half of data only entry */
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index + 1, &acc_type);
            soc_meminfo_field_get(mem, &exact_match_minfo, entry, _ENTRY1f,
                                  tmp_entry, SOC_MAX_MEM_WORDS);
            soc_internal_extended_write_mem(pcid_info, block, acc_type, alt_addr,
                                            tmp_entry);

            /* Write hit bit portion of content */
            alt_mem = EXACT_MATCH_HIT_ONLYm;
            sal_memset(tmp_entry, 0,
                       soc_mem_entry_words(unit, alt_mem) * sizeof(uint32));
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, bucket, &acc_type);
            fval = soc_mem_field32_get(unit, mem, entry, HIT_0f);
            soc_mem_field32_set(unit, alt_mem, tmp_entry, HIT_0f, fval);
            fval = soc_mem_field32_get(unit, mem, entry, HIT_1f);
            soc_mem_field32_set(unit, alt_mem, tmp_entry, HIT_1f, fval);
            soc_internal_extended_write_mem(pcid_info, block, acc_type, alt_addr,
                                            tmp_entry);
            break;

        case EXACT_MATCH_2m:
            /* Uses one slot per entry */
            index = offset;
            bucket = index / bucket_size;
            bit = index % bucket_size;

            /* Write data only portion of content */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                alt_mem = EXACT_MATCH_2m;
            } else
#endif
            {
                alt_mem = EXACT_MATCH_2_ENTRY_ONLYm;
            }
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, index, &acc_type);
            soc_meminfo_field_get(mem, &exact_match_minfo, entry, _ENTRY0f,
                                  tmp_entry, SOC_MAX_MEM_WORDS);
            soc_internal_extended_write_mem(pcid_info, block, acc_type, alt_addr,
                                            tmp_entry);

            /* Write hit bit portion of content */
            alt_mem = EXACT_MATCH_HIT_ONLYm;
            sal_memset(tmp_entry, 0,
                       soc_mem_entry_words(unit, alt_mem) * sizeof(uint32));
            block_info_index = SOC_MEM_BLOCK_ANY(unit, alt_mem);
            block = SOC_BLOCK2SCH(unit, block_info_index);
            alt_addr = soc_mem_addr_get(pcid_info->unit, alt_mem, 0,
                                        block_info_index, bucket, &acc_type);
            soc_internal_extended_read_mem(pcid_info, block, acc_type, alt_addr,
                                           tmp_entry);
            fval = soc_mem_field32_get(unit, mem, entry, HIT_0f);
            soc_mem_field32_set(unit, alt_mem, tmp_entry, hit_fields[bit], fval);
            soc_internal_extended_write_mem(pcid_info, block, acc_type, alt_addr,
                                            tmp_entry);
            break;

        default:
            return -1;
    }

    return 0;
}

STATIC int
soc_internal_exact_match_size_get(pcid_info_t *pcid_info, void *entry,
                                  soc_mem_t *mem, int *num_vbits)
{
    int unit = pcid_info->unit;
    int key_type;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        key_type = soc_mem_field32_get(unit, EXACT_MATCH_2m, entry, KEY_TYPEf);
    }
    else
#endif
    {
        key_type = soc_mem_field32_get(unit, EXACT_MATCH_2m, entry, KEY_TYPE_0f);
    }
    switch (key_type) {
         case TH_FPEM_HASH_KEY_TYPE_128B:
         case TH_FPEM_HASH_KEY_TYPE_160B:
            *num_vbits = 1;
            *mem = EXACT_MATCH_2m;
            break;
         case TH_FPEM_HASH_KEY_TYPE_320B:
            *num_vbits = 2;
            *mem = EXACT_MATCH_4m;
            break;
         default:
            return -1;
    }

    return 0;
}

STATIC int
soc_internal_exact_match_bucket_get(pcid_info_t *pcid_info, int bank,
                                    void *entry)
{
    int     bucket;
    int     unit = pcid_info->unit;
    int     block;
    uint8   acc_type;
    uint32  addr;
    int     hash_offset, use_lsb, index;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int             block_info_index;
    uint32          tmp[SOC_MAX_MEM_WORDS];
#endif
    soc_reg_t reg;
    uint32  rval[2];
    uint32 (*entry_hash_proc)(int unit, int bank, int hash_offset,
                              int use_lsb, uint32 *entry);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        static const soc_field_t fields[] = {
            B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf,
            B3_HASH_OFFSETf
        };
        static const soc_field_t fpem_fields[] = {
            LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
            LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
            LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
            LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf
        };

        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_HASH_CONTROLm);
        block = SOC_BLOCK2SCH(unit, block_info_index);

        entry_hash_proc = soc_tomahawk3_exact_match_entry_hash;
        addr = soc_mem_addr_get(unit, EXACT_MATCH_HASH_CONTROLm, 0,
                                block_info_index, 0, &acc_type);
        soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                       addr, tmp);
        index = soc_mem_field32_get(
                                unit, EXACT_MATCH_HASH_CONTROLm, tmp,
                                            fpem_fields[bank]);

        use_lsb = 0;
        block_info_index = SOC_MEM_BLOCK_ANY(unit, UFT_SHARED_BANKS_CONTROLm);
        block = SOC_BLOCK2SCH(unit, block_info_index);
        addr = soc_mem_addr_get(unit, UFT_SHARED_BANKS_CONTROLm, 0,
                                block_info_index, 0, &acc_type);
        soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                       addr, tmp);
        hash_offset = soc_mem_field32_get(
                                unit, UFT_SHARED_BANKS_CONTROLm, tmp,
                                            fields[index]);
        bucket = entry_hash_proc(unit, bank, hash_offset, use_lsb, entry);

    }
    else
#endif
    {
        static const soc_field_t fields[] = {
            BANK2_HASH_OFFSETf, BANK3_HASH_OFFSETf, BANK4_HASH_OFFSETf,
            BANK5_HASH_OFFSETf
        };
        static const soc_field_t fpem_fields[] = {
            FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
            FPEM_ENTRY_BANK_3f
        };

        entry_hash_proc = soc_th_exact_match_entry_hash;
        addr = soc_reg_addr_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
        /*
         * COVERITY
         *
         * the below coverity error (overrun-buffer-val) is marked as false because we know that
         * the size of ISS_LOG_TO_PHY_BANK_MAPr (<=64 bits) means that the overrun will never be exercised
         * in the code below (the macros called in soc_internal_extended_read_reg() are aware of the size of
         * ISS_LOG_TO_PHY_BANK_MAPr and how much data it can access in rval).
 */
        /* coverity[overrun-buffer-val : FALSE] */
        soc_internal_extended_read_reg(pcid_info, block, acc_type, addr, rval);
        index = soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, rval[0],
                                  fpem_fields[bank]);

        use_lsb = 0;
        reg = SHARED_TABLE_HASH_CONTROLr;
        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
        /*
         * COVERITY
         *
         * the below coverity error (overrun-buffer-val) is marked as false because we know that
         * the size of SHARED_TABLE_HASH_CONTROLr (<=64 bits) means that the overrun will never be exercised
         * in the code below (the macros called in soc_internal_extended_read_reg() are aware of the size of
         * SHARED_TABLE_HASH_CONTROLr and how much data it can access in rval).
 */
        /* coverity[overrun-buffer-val : FALSE] */
        soc_internal_extended_read_reg(pcid_info, block, acc_type, addr, rval);
        hash_offset = soc_reg_field_get(unit, reg, rval[0], fields[index]);
        bucket = entry_hash_proc(unit, bank, hash_offset, use_lsb, entry);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "bank %d bucket %d\n"), bank, bucket));
    }
    return bucket;
}

STATIC void
soc_internal_exact_match_bank_map_get(pcid_info_t *pcid_info,
                                      uint32 *bank_map, int *bank_start,
                                      int *bank_inc, int *do_first_fit)
{
    uint32 rval[2];
    int    unit = pcid_info->unit;
    int    block;
    uint8  acc_type;
    uint32 addr;

    uint32 shared_bank_map;
    int bit_count;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        uint32          tmp[SOC_MAX_MEM_WORDS];
        int             block_info_index;

        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_HASH_CONTROLm);
        block = SOC_BLOCK2SCH(unit, block_info_index);

        addr = soc_mem_addr_get(unit, EXACT_MATCH_HASH_CONTROLm, 0,
                                block_info_index, 0, &acc_type);
        soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                       addr, tmp);
        /* hardcode shared_bank_map because the value is 0 in pcid */
        /*shared_bank_map = soc_mem_field32_get(
                                unit, EXACT_MATCH_HASH_CONTROLm, tmp,
                                            HASH_TABLE_BANK_CONFIGf);*/
        /* calculate number of 1's in shared_bank_map */
        shared_bank_map = 0xf;
        bit_count = shared_bank_map - ((shared_bank_map >> 1) & 0x5);
        bit_count = ((bit_count & 0xc) >> 2) + (bit_count & 0x3);

        *bank_map = (1 << bit_count) - 1;
        *bank_start = 0;
        *bank_inc = 1;
        if (do_first_fit != NULL) {
            *do_first_fit = FALSE;
        }
    }
    else
#endif
    {
        addr = soc_reg_addr_get(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &block,
                                &acc_type);
        soc_internal_extended_read_reg(pcid_info, block, acc_type, addr, rval);
        shared_bank_map = soc_reg_field_get(unit, ISS_BANK_CONFIGr, rval[0],
                                            FPEM_ENTRY_BANK_CONFIGf);
        /* calculate number of 1's in shared_bank_map */
        bit_count = shared_bank_map - ((shared_bank_map >> 1) & 0x5);
        bit_count = ((bit_count & 0xc) >> 2) + (bit_count & 0x3);

        *bank_map = (1 << bit_count) - 1;
        *bank_start = 0;
        *bank_inc = 1;
        if (do_first_fit != NULL) {
            *do_first_fit = FALSE;
        }
    }
}

STATIC void
soc_internal_exact_match_bank_size_get(pcid_info_t *pcid_info, int bank,
                                       int *entries_per_bank,
                                       int *entries_per_row,
                                       int *entries_per_bucket,
                                       int *bank_base, int *bucket_offset)
{
    /*
     * entry index =
     *     bank_base + bucket * entries_per_row + bucket_offset + slot;
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(pcid_info->unit)) {
        /* hardcode because hash_bank_info_get will call soc_mem_read, which
         * cannot be done from pcid.  Assume passing bank = 0 */
        if(entries_per_bank != NULL) {
            *entries_per_bank = 16384;
        }
        if(entries_per_row != NULL) {
            *entries_per_row  = 2;
        }
        if(entries_per_bucket != NULL) {
            *entries_per_bucket = 2;
        }
        if(bank_base != NULL) {
            *bank_base = bank * 16384;
        }
        if(bucket_offset != NULL) {
            *bucket_offset = 0;
        }

/*        soc_tomahawk3_hash_bank_info_get(pcid_info->unit, EXACT_MATCH_2m,
                                         bank, entries_per_bank,
                                         entries_per_row,
                                         entries_per_bucket,
                                         bank_base, bucket_offset);
*/
    }
    else
#endif
    {
        soc_tomahawk_hash_bank_info_get(pcid_info->unit, EXACT_MATCH_2m,
                                         bank, entries_per_bank,
                                         entries_per_row,
                                         entries_per_bucket,
                                         bank_base, bucket_offset);
    }
}

int
soc_internal_exact_match_ins(pcid_info_t *pcid_info, uint32 inv_bank_map,
                             void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    uint32          ent[SOC_MAX_MEM_WORDS];
    int             rv;
    uint32          bank_map;
    int             bank, entries_per_bank, bank_start, bank_inc, free_bank;
    int             bucket, entries_per_row, bank_base, free_bucket;
    int             slot, entries_per_bucket, bucket_offset, free_slot;
    int             sub_slot;
    int             index, free_index;
    int             first_free_slot, free_slot_count, do_first_fit;
    int             best_free_slot_count, best_entries_per_bank;
    int             num_vbits = 0;
    int             validslot_cnt;
    soc_mem_t       mem;
    int             sub_fields[] = { _ENTRY0f, _ENTRY1f };
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             block_info_index;
    soc_block_t     block;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "EXACT_MATCH Insert\n")));

    rv = soc_internal_exact_match_size_get(pcid_info, entry, &mem,
                                           &num_vbits);
    if ((rv < 0) || (num_vbits == 0)) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_2m);
    block = SOC_BLOCK2SCH(unit, block_info_index);

    free_index = -1;

    soc_internal_exact_match_bank_map_get(pcid_info, &bank_map, &bank_start,
                                          &bank_inc, &do_first_fit);
    bank_map &= ~inv_bank_map;

    best_free_slot_count = 0;
    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_exact_match_bank_size_get(pcid_info, bank,
                                               &entries_per_bank,
                                               &entries_per_row,
                                               &entries_per_bucket,
                                               &bank_base, &bucket_offset);

        bucket = soc_internal_exact_match_bucket_get(pcid_info, bank, entry);

        free_slot_count = 0;
        first_free_slot = -1;
        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    }

                }
                else
#endif
                {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    }
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_exact_match(unit, entry, ent) == 0) {
                    index /= num_vbits;
                    addr = soc_mem_addr_get(unit, mem, 0, block_info_index,
                                            index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "write block %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                            block, acc_type, bank, bucket, slot, index));

                    /* Overwrite the existing entry */
                    soc_internal_extended_write_mem(pcid_info, block,
                                                    acc_type, addr, entry);

                    /* Copy old entry immediately after response word */
                    memcpy(&result[1], ent, soc_mem_entry_bytes(unit, mem));
                    result[0] = 0;
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                    genresp_v2->index = index;
                    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                    return 0;
                }
            } else if (validslot_cnt == 0) {
                free_slot_count++;
                if (first_free_slot == -1) {
                    first_free_slot = slot;
                }
            }
        }
        if (free_slot_count > 0) {
            if (do_first_fit) {
                if (free_index != -1) {
                    continue;
                }
            } else {
                if (free_slot_count < best_free_slot_count) {
                    continue;
                } else if (free_slot_count == best_free_slot_count) {
                    if (entries_per_bank < best_entries_per_bank) {
                        continue;
                    } else if (entries_per_bank == best_entries_per_bank) {
                        if (free_index != -1 && bank > free_bank) {
                            continue;
                        }
                    }
                }
                best_free_slot_count = free_slot_count;
                best_entries_per_bank = entries_per_bank;
            }
            free_index = (index + 1 - entries_per_bucket + first_free_slot) /
                num_vbits;
            free_bank = bank;
            free_bucket = bucket;
            free_slot = first_free_slot;
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, mem, 0, block_info_index, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write block %d acc_type %d bank %d bucket %d, "
                                "slot %d, index %d\n"),
                                block, acc_type, free_bank, free_bucket, free_slot, index));

        /* Write the entry */
        soc_internal_extended_write_mem(pcid_info, block, acc_type, addr,
                                        entry);

        result[0] = 0;
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_INSERTED;
        genresp_v2->index = index;
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

        return 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Bucket full\n")));
    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_FULL;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}

int
soc_internal_exact_match_del(pcid_info_t *pcid_info, uint32 inv_bank_map,
                             void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    uint32          ent[SOC_MAX_MEM_WORDS];
    int             rv;
    uint32          bank_map;
    int             bank, bank_start, bank_inc;
    int             bucket, entries_per_row, bank_base;
    int             slot, entries_per_bucket, bucket_offset;
    int             sub_slot;
    int             index;
    int             num_vbits = 0;
    int             validslot_cnt;
    soc_mem_t       mem;
    int             sub_fields[] = { _ENTRY0f, _ENTRY1f };
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             block_info_index;
    soc_block_t     block;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EXACT_MATCH Delete\n")));

    rv = soc_internal_exact_match_size_get(pcid_info, entry, &mem,
                                           &num_vbits);
    if ((rv < 0) || (num_vbits == 0)) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_2m);
    }
    else
#endif
    {
        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_2m);
    }
    block = SOC_BLOCK2SCH(unit, block_info_index);

    soc_internal_exact_match_bank_map_get(pcid_info, &bank_map, &bank_start,
                                          &bank_inc, NULL);
    bank_map &= ~inv_bank_map;

    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

         soc_internal_exact_match_bank_size_get(pcid_info, bank, NULL,
                                               &entries_per_row,
                                               &entries_per_bucket,
                                               &bank_base, &bucket_offset);

        bucket = soc_internal_exact_match_bucket_get(pcid_info, bank, entry);

        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    } else {
                        break;
                    }
                }
                else
#endif
                {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    } else {
                        break;
                    }
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_exact_match(unit, entry, ent) == 0) {
                    index /= num_vbits;
                    addr = soc_mem_addr_get(unit, mem, 0, block_info_index,
                                            index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "delete block %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                            block, acc_type, bank, bucket, slot, index));

                    /* Copy entry immediately after response word */
                    memcpy(&result[1], ent, soc_mem_entry_bytes(unit, mem));

                    /* Invalidate entry */
                    memset(tmp, 0, sizeof(tmp));
                    soc_internal_extended_write_mem(pcid_info, block,
                                                    acc_type, addr,
                                                    (uint32 *)tmp);

                    result[0] = 0;
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                    genresp_v2->index = index;
                    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                    return 0;
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_exact_match_del: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}
int
soc_internal_exact_match_lkup(pcid_info_t * pcid_info, uint32 inv_bank_map,
                              void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    uint32          ent[SOC_MAX_MEM_WORDS];
    int             rv;
    uint32          bank_map;
    int             bank, bank_start, bank_inc;
    int             bucket, entries_per_row, bank_base;
    int             slot, entries_per_bucket, bucket_offset;
    int             sub_slot;
    int             index;
    int             num_vbits = 0;
    int             validslot_cnt;
    soc_mem_t       mem;
    int             sub_fields[] = { _ENTRY0f, _ENTRY1f };
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             block_info_index;
    soc_block_t     block;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EXACT_MATCH Lookup\n")));

    rv = soc_internal_exact_match_size_get(pcid_info, entry, &mem,
                                              &num_vbits);
    if ((rv < 0) || (num_vbits == 0)) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_2m);
    }
    else
#endif
    {
        block_info_index = SOC_MEM_BLOCK_ANY(unit, EXACT_MATCH_2m);
    }
    block = SOC_BLOCK2SCH(unit, block_info_index);

    soc_internal_exact_match_bank_map_get(pcid_info, &bank_map, &bank_start,
                                          &bank_inc, NULL);
    bank_map &= ~inv_bank_map;

    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_exact_match_bank_size_get(pcid_info, bank, NULL,
                                               &entries_per_row,
                                               &entries_per_bucket,
                                               &bank_base, &bucket_offset);

        bucket = soc_internal_exact_match_bucket_get(pcid_info, bank, entry);

        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            BASE_VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    } else {
                        break;
                    }
                }
                else
#endif
                {
                    addr = soc_mem_addr_get(unit, EXACT_MATCH_2m, 0,
                                            block_info_index, index, &acc_type);
                    soc_internal_extended_read_mem(pcid_info, block, acc_type,
                                                   addr, tmp);
                    if (soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_0f) &&
                        soc_mem_field32_get(unit, EXACT_MATCH_2m, tmp,
                                            VALID_1f)) {
                        soc_meminfo_field_set(mem, &exact_match_minfo, ent,
                                              sub_fields[sub_slot], tmp);
                        validslot_cnt++;
                    } else {
                        break;
                    }
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_exact_match(unit, entry, ent) == 0) {
                    index /= num_vbits;
                    addr = soc_mem_addr_get(unit, mem, 0, block_info_index,
                                            index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "found block %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                            block, acc_type, bank, bucket, slot, index));

                    /* Copy entry immediately after response word */
                    memcpy(&result[1], ent, soc_mem_entry_bytes(unit, mem));

                    result[0] = 0;
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                    genresp_v2->index = index;
                    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                    return 0;
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_exact_match_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}
#endif /* BCM_TOMAHAWK_SUPPORT */
