/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3x.c
 *
 * Provides:
 *      soc_internal_l3x_read
 *      soc_internal_l3x_write
 *      soc_internal_l3x_init
 *      soc_internal_l3x_del
 *      soc_internal_l3x_ins
 *
 * Requires:    
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/drv.h>
#include <soc/l3x.h>
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"

#ifdef BCM_XGS_SWITCH_SUPPORT
#ifdef BCM_FIREBOLT_SUPPORT
/* 
 * Firebolt L3 Table Simulation
 *
 * The L3 table is hashed.
 *
 * L3_ENTRY_IPV4_UNICAST,L3_ENTRY_IPV4_MULTICAST,
 * L3_ENTRY_IPV6_UNICAST, L3_ENTRY_IPV6_MULTICAST
 * are "virtual" table which aggregates the
 * L3_ENTRY_HIT_ONLY | L3_ENTRY_ONLY
 * tables.
 */
enum {
        ENT3f,
        ENT2f,
        ENT1f,
        ENT0f
} soc_L3_ENTRY_DUMMYm_ftype;

static soc_field_info_t soc_L3_ENTRY_DUMMYm_fields[] = {
    { ENT3f, 96, 288, SOCF_LE },
    { ENT2f, 96, 192, SOCF_LE },
    { ENT1f, 96, 96, SOCF_LE },
    { ENT0f, 96, 0, SOCF_LE },
};
static soc_mem_info_t l3_minfo = {
                        /* mem            L3_XXXm */
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
                        /* data_bits */ 384,
                        /* bytes       */ 25,
                        /* nFields     */ 4, /* Used */
                        /* *fields     */ soc_L3_ENTRY_DUMMYm_fields, /* Used */
                        };

#define     L3_ENTRY_IS_ALIGNED(o, a) (((o) % (a)) == 0)

int
soc_internal_l3x2_read(pcid_info_t * pcid_info, soc_mem_t mem, uint32 addr,
                      uint32 *entry)
{
    int         unit;
    int         offset;
    uint32      ent[SOC_MAX_MEM_WORDS];
    int         bit;
    int         bucket;
    int         index;
    int 	bucket_size;
    int         blk;
    soc_block_t sblk;
    uint8       at = 0;

    unit = pcid_info->unit;
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;

    memset(entry, 0, 4 * soc_mem_entry_words(unit, mem));
    memset(ent, 0, 4 * soc_mem_entry_words(unit, mem));

    bucket_size = L3_BUCKET_SIZE(unit);

    /*
     * Fix l3_minfo using info from L3_ENTRY_ONLYm table
     */    
    soc_L3_ENTRY_DUMMYm_fields[3].len = soc_L3_ENTRY_DUMMYm_fields[2].len = 
    soc_L3_ENTRY_DUMMYm_fields[1].len = soc_L3_ENTRY_DUMMYm_fields[0].len = 
                            soc_mem_entry_bits(unit, L3_ENTRY_ONLYm);
    soc_L3_ENTRY_DUMMYm_fields[3].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 0;
    soc_L3_ENTRY_DUMMYm_fields[2].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 1;
    soc_L3_ENTRY_DUMMYm_fields[1].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 2;
    soc_L3_ENTRY_DUMMYm_fields[0].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 3;

    /*
     * We could assert on V6 and IPMC bits depending which table
     * is being read/written. But that won't be good for diags.
     */    
    switch(mem) {
        case L3_ENTRY_IPV4_MULTICASTm:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
                /* Uses two slots per entry */
                index = offset * 2;
                bucket = index / bucket_size;
                bit = (index % bucket_size);
                blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
                sblk = SOC_BLOCK2SCH(unit, blk);
                soc_internal_extended_read_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                           blk, index + 0, &at), entry);

                soc_internal_extended_read_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                           blk, index + 1, &at), ent);
                soc_meminfo_field_set(mem, &l3_minfo, entry, ENT1f, ent);

                blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
                sblk = SOC_BLOCK2SCH(unit, blk);
                soc_internal_extended_read_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                           blk, bucket, &at), ent);
                soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm, entry,
                                    HIT_0f, (ent[0] >> (bit + 0)) & 1);
                soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm, entry,
                                    HIT_1f, (ent[0] >> (bit + 1)) & 1);
                break;
            }
#endif
            /* fallthrough */
        case L3_ENTRY_IPV4_UNICASTm:
            /* Uses one slot per entry */
            index = offset;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), entry);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, entry,
                                HITf, (ent[0] >> bit) & 1);
                
            break;
        case L3_ENTRY_IPV6_UNICASTm:
            /* Uses two slots per entry */
            index = offset * 2;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), entry);

            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 1, &at), ent);
            soc_meminfo_field_set(mem, &l3_minfo, entry, ENT1f, ent);

            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_UNICASTm, entry,
                                HIT_0f, (ent[0] >> (bit + 0)) & 1);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_UNICASTm, entry,
                                HIT_1f, (ent[0] >> (bit + 1)) & 1);
            break;
        case L3_ENTRY_IPV6_MULTICASTm:
            /* Uses four slots per entry */
            index = offset * 4;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), entry);

            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 1, &at), ent);
            soc_meminfo_field_set(mem, &l3_minfo, entry, ENT1f, ent);

            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 2, &at), ent);
            soc_meminfo_field_set(mem, &l3_minfo, entry, ENT2f, ent);

            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 3, &at), ent);
            soc_meminfo_field_set(mem, &l3_minfo, entry, ENT3f, ent);

            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_MULTICASTm, entry,
                                HIT_0f, (ent[0] >> (bit + 0)) & 1);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_MULTICASTm, entry,
                                HIT_1f, (ent[0] >> (bit + 1)) & 1);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_MULTICASTm, entry,
                                HIT_2f, (ent[0] >> (bit + 2)) & 1);
            soc_mem_field32_set(unit, L3_ENTRY_IPV6_MULTICASTm, entry,
                                HIT_3f, (ent[0] >> (bit + 3)) & 1);
            break;
        default: return(-1);
    }
    return(0);
}

int
soc_internal_l3x2_write(pcid_info_t * pcid_info, soc_mem_t mem, uint32 addr,
                       uint32 *entry)
{
    int         unit;
    int         offset;
    uint32      ent[SOC_MAX_MEM_WORDS];
    int         bit;
    int         bucket;
    int         index;
    int         hit;
    int		bucket_size;
    int         blk;
    soc_block_t sblk;
    uint8       at = 0;

    unit = pcid_info->unit;
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;
    memset(ent, 0, 4 * soc_mem_entry_words(unit, mem));

    bucket_size = L3_BUCKET_SIZE(unit);

    /*
     * Fix l3_minfo using info from L3_ENTRY_ONLYm table
     */    
    soc_L3_ENTRY_DUMMYm_fields[3].len = soc_L3_ENTRY_DUMMYm_fields[2].len = 
    soc_L3_ENTRY_DUMMYm_fields[1].len = soc_L3_ENTRY_DUMMYm_fields[0].len = 
                            soc_mem_entry_bits(unit, L3_ENTRY_ONLYm);
    soc_L3_ENTRY_DUMMYm_fields[3].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 0;
    soc_L3_ENTRY_DUMMYm_fields[2].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 1;
    soc_L3_ENTRY_DUMMYm_fields[1].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 2;
    soc_L3_ENTRY_DUMMYm_fields[0].bp = soc_L3_ENTRY_DUMMYm_fields[0].len * 3;

    /*
     * We could assert on V6 and IPMC bits depending which table
     * is being read/written. But that won't be good for diags.
     */    
    switch(mem) {
        case L3_ENTRY_IPV4_MULTICASTm:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
                /* Uses two slots per entry */
                index = offset * 2;
                bucket = index / bucket_size;
                bit = (index % bucket_size);
                blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
                sblk = SOC_BLOCK2SCH(unit, blk);
                soc_meminfo_field_get(mem, &l3_minfo, entry, ENT0f, ent, SOC_MAX_MEM_WORDS);
                soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                           blk, index + 0, &at), ent);

                soc_meminfo_field_get(mem, &l3_minfo, entry, ENT1f, ent, SOC_MAX_MEM_WORDS);
                soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                           blk, index + 1, &at), ent);
    
                blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
                sblk = SOC_BLOCK2SCH(unit, blk);
                soc_internal_extended_read_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                           blk, bucket, &at), ent);
                ent[0] &= (~((0x3 << (bit))));
                hit = soc_mem_field32_get(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          entry, HIT_0f);
                ent[0] |= ((hit & 1) << (bit + 0));
                hit = soc_mem_field32_get(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          entry, HIT_1f);
                ent[0] |= ((hit & 1) << (bit + 1));
                soc_internal_extended_write_mem(pcid_info, sblk, at,
                          soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                           blk, bucket, &at), ent);

                break;
            }
#endif
            /* fallthrough */
        case L3_ENTRY_IPV4_UNICASTm:
            /* Uses one slot per entry */
            index = offset;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            ent[0] &= (~((0x1 << (bit))));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm,
                                      entry, HITf);
            ent[0] |= ((hit  & 1) << (bit));
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
                
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, entry, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), entry);
            break;
        case L3_ENTRY_IPV6_UNICASTm:
            /* Uses two slots per entry */
            index = offset * 2;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT0f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), ent);

            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT1f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 1, &at), ent);

            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            ent[0] &= (~((0x3 << (bit))));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_UNICASTm,
                                      entry, HIT_0f);
            ent[0] |= ((hit & 1) << (bit + 0));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_UNICASTm,
                                      entry, HIT_1f);
            ent[0] |= ((hit & 1) << (bit + 1));
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);

            break;
        case L3_ENTRY_IPV6_MULTICASTm:
            /* Uses four slots per entry */
            index = offset * 4;
            bucket = index / bucket_size;
            bit = (index % bucket_size);
            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT0f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 0, &at), ent);

            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT1f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 1, &at), ent);

            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT2f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 2, &at), ent);

            soc_meminfo_field_get(mem, &l3_minfo, entry, ENT3f, ent, SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm, ent, HITf, 0);
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, 
                                       blk, index + 3, &at), ent);

            blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_HIT_ONLYm);
            sblk = SOC_BLOCK2SCH(unit, blk);
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                                      entry, HIT_0f);
            ent[0] &= (~((0xf << (bit))));
            ent[0] |= ((hit & 1) << (bit + 0));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                                      entry, HIT_1f);
            ent[0] |= ((hit & 1) << (bit + 1));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                                      entry, HIT_2f);
            ent[0] |= ((hit & 1) << (bit + 2));
            hit = soc_mem_field32_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                                      entry, HIT_3f);
            ent[0] |= ((hit & 1) << (bit + 3));
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                      soc_mem_addr_get(unit, L3_ENTRY_HIT_ONLYm, 0, 
                                       blk, bucket, &at), ent);
            break;
        default: return(-1);
    }
    return(0);
}

int
soc_internal_l3x2_entry_size_get(pcid_info_t *pcid_info, void *entry,
                                 soc_mem_t *mem, int *num_vbits)
{
    int unit = pcid_info->unit;

#ifdef BCM_TRX_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_ONLYm, KEY_TYPEf)) {
        int key_type;

        key_type = soc_mem_field32_get(unit, L3_ENTRY_ONLYm,
                                       entry, KEY_TYPEf);
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            switch (key_type) {
            case TH_L3_HASH_KEY_TYPE_V4UC:
            case TH_L3_HASH_KEY_TYPE_TRILL:
                *num_vbits = 1;
                *mem = L3_ENTRY_IPV4_UNICASTm;
                break;
            case TH_L3_HASH_KEY_TYPE_V4UC_EXT:
            case TH_L3_HASH_KEY_TYPE_V4MC:
            case TH_L3_HASH_KEY_TYPE_DST_NAT:
            case TH_L3_HASH_KEY_TYPE_DST_NAPT:
                *num_vbits = 2;
                *mem = L3_ENTRY_IPV4_MULTICASTm;
                break;
            case TH_L3_HASH_KEY_TYPE_V6UC:
                *num_vbits = 2;
                *mem = L3_ENTRY_IPV6_UNICASTm;
                break;
            case TH_L3_HASH_KEY_TYPE_V6UC_EXT:
            case TH_L3_HASH_KEY_TYPE_V6MC:
                *num_vbits = 4;
                *mem = L3_ENTRY_IPV6_MULTICASTm;
                break;
            default:
                return -1;
            }
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            switch (key_type) {
            case TD2_L3_HASH_KEY_TYPE_V4UC:
            case TD2_L3_HASH_KEY_TYPE_TRILL:
            case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
            case TD2_L3_HASH_KEY_TYPE_FCOE_HOST:
            case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP:
                *num_vbits = 1;
                *mem = L3_ENTRY_IPV4_UNICASTm;
                break;
            case TD2_L3_HASH_KEY_TYPE_V4UC_EXT:
            case TD2_L3_HASH_KEY_TYPE_V4MC:
            case TD2_L3_HASH_KEY_TYPE_V4L2MC:
            case TD2_L3_HASH_KEY_TYPE_V4L2VPMC:
            case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT:
            case TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT:
            case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT:
            case TD2_L3_HASH_KEY_TYPE_DST_NAT:
            case TD2_L3_HASH_KEY_TYPE_DST_NAPT:
                *num_vbits = 2;
                *mem = L3_ENTRY_IPV4_MULTICASTm;
                break;
            case TD2_L3_HASH_KEY_TYPE_V6UC:
                *num_vbits = 2;
                *mem = L3_ENTRY_IPV6_UNICASTm;
                break;
            case TD2_L3_HASH_KEY_TYPE_V6UC_EXT:
            case TD2_L3_HASH_KEY_TYPE_V6MC:
            case TD2_L3_HASH_KEY_TYPE_V6L2MC:
            case TD2_L3_HASH_KEY_TYPE_V6L2VPMC:
                *num_vbits = 4;
                *mem = L3_ENTRY_IPV6_MULTICASTm;
                break;
            default:
                return -1;
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            switch (key_type) {
            case TR_L3_HASH_KEY_TYPE_V4UC:
                *num_vbits = 1;
                *mem = L3_ENTRY_IPV4_UNICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_V4MC:
                *num_vbits = SOC_IS_TR_VL(unit) ? 2 : 1;
                *mem = L3_ENTRY_IPV4_MULTICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_V6UC:
                *num_vbits = 2;
                *mem = L3_ENTRY_IPV6_UNICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_V6MC:
                *num_vbits = 4;
                *mem = L3_ENTRY_IPV6_MULTICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_LMEP:
                *num_vbits = 1;
                *mem = L3_ENTRY_IPV4_UNICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_RMEP:
                *num_vbits = 1;
                *mem = L3_ENTRY_IPV4_UNICASTm;
                break;
            case TR_L3_HASH_KEY_TYPE_TRILL:
                *num_vbits = 1;
                *mem = L3_ENTRY_ONLYm;
                break;
            default:
                return -1;
            }
        }
    } else
#endif /* BCM_TRX_SUPPORT */
    if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, V6f)) {
        if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
            *mem = L3_ENTRY_IPV6_MULTICASTm;
            *num_vbits = 4;
        } else {
            *mem = L3_ENTRY_IPV6_UNICASTm;
            *num_vbits = 2;
        }
    } else {
        *num_vbits = 1;
        if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
            *mem = L3_ENTRY_IPV4_MULTICASTm;
        } else {
            *mem = L3_ENTRY_IPV4_UNICASTm;
        }
    }

    return 0;
}

STATIC int
soc_internal_l3x2_bucket_get(pcid_info_t *pcid_info, int bank, void *entry)
{
    uint32  rval[SOC_MAX_MEM_WORDS];
    int     bucket, hash_sel;
    int     unit = pcid_info->unit;
    int     blk;
    uint8   acc_type;
    uint32  addr;

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        int hash_offset, use_lsb, index;
        soc_reg_t reg;
        static const soc_field_t fields[] = {
            BANK2_HASH_OFFSETf, BANK3_HASH_OFFSETf, BANK4_HASH_OFFSETf,
            BANK5_HASH_OFFSETf, BANK6_HASH_OFFSETf, BANK7_HASH_OFFSETf,
            BANK8_HASH_OFFSETf, BANK9_HASH_OFFSETf
        };
        int is_shared_bank;
        uint32 (*entry_hash_proc)(int unit, int bank, int hash_offset,
                               int use_lsb, uint32 *entry);

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            static const soc_field_t l3_fields[] = {
                L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
                L3_ENTRY_BANK_7f
            };

            entry_hash_proc = soc_th_l3x_entry_hash;
            if (bank < 4) {
                is_shared_bank = FALSE;
                index = bank + 4;
            } else {
                is_shared_bank = TRUE;
                addr = soc_reg_addr_get(unit, ISS_LOG_TO_PHY_BANK_MAPr,
                                        REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_NONE,
                                        &blk, &acc_type);
                soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr,
                                               rval);
                index = soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr,
                                          rval[0], l3_fields[bank - 4]);
            }
        } else
#endif /*TOMAHAWK_SUPPORT */

#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            static const soc_field_t l3_fields[] = {
                L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
                L3_ENTRY_BANK_7f
            };
            entry_hash_proc = soc_ap_l3x_entry_hash;
            if (bank < 4) {
                is_shared_bank = FALSE;
                index = bank + 4;
            } else {
                is_shared_bank = TRUE;
                addr = soc_reg_addr_get(unit, ISS_LOG_TO_PHY_BANK_MAPr,
                                        REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_NONE,
                                        &blk, &acc_type);
                soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr,
                                               rval);
                index = soc_reg_field_get(unit, ISS_LOG_TO_PHY_BANK_MAPr,
                                          rval[0], l3_fields[bank - 4]);
           }
        } else
#endif /*BCM_APACHE_SUPPORT */ 

        {
            entry_hash_proc = soc_td2_l3x_entry_hash;
            is_shared_bank = bank < 6;
            index = bank - 2;
        }

        reg = L3_TABLE_HASH_CONTROLr;
        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
        soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, rval);
        use_lsb = soc_reg_field_get(unit, reg, rval[0], HASH_ZERO_OR_LSBf);

        if (is_shared_bank) {
            reg = SHARED_TABLE_HASH_CONTROLr;
            addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                    SOC_REG_ADDR_OPTION_NONE, &blk,
                                    &acc_type);
            soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr,
                                           rval);
        }
        hash_offset = soc_reg_field_get(unit, reg, rval[0], fields[index]);
        bucket = entry_hash_proc(unit, bank, hash_offset, use_lsb, entry);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        hash_sel = -1;
        if (bank == 1) {
            addr = soc_reg_addr_get(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY,
                                    0, SOC_REG_ADDR_OPTION_NONE,
                                    &blk, &acc_type);
            soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr,
                                           rval);
            if (soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr, rval[0],
                                  ENABLEf)) {
                hash_sel = soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                                             rval[0], HASH_SELECTf);
            }
        }
        if (hash_sel == -1) {
            addr = soc_reg_addr_get(unit, HASH_CONTROLr, REG_PORT_ANY, 0,
                                    SOC_REG_ADDR_OPTION_NONE, &blk,
                                    &acc_type);
            soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr,
                                           rval);
            hash_sel = soc_reg_field_get(unit, HASH_CONTROLr, rval[0],
                                         L3_HASH_SELECTf);
        }

        bucket = soc_fb_l3x_entry_hash(unit, hash_sel, entry);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bank %d bucket %d\n"), bank, bucket));

    return bucket;
}

STATIC void
soc_internal_l3x2_bank_map_get(pcid_info_t *pcid_info, uint32 *bank_map,
                               int *bank_start, int *bank_inc,
                               int *do_first_fit)
{
    uint32 rval[SOC_MAX_MEM_WORDS];
    int    unit = pcid_info->unit;
    int    blk;
    uint8  acc_type;
    uint32 addr;

#ifdef BCM_MONTEREY_SUPPORT
     if (SOC_IS_MONTEREY(pcid_info->unit)) {
         *bank_map = 0x3c0;
         *bank_start = 9;
         *bank_inc = -1;
        if (do_first_fit != NULL) {
            *do_first_fit = FALSE;
        }
     }  else 
#endif 
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_TOMAHAWKX(pcid_info->unit) || SOC_IS_APACHE(unit)) {
        uint32 shared_bank_map;
        int bit_count;

#ifdef BCM_APACHE_SUPPORT
     if (SOC_IS_APACHE(pcid_info->unit)) {
          addr = soc_reg_addr_get(unit, L3_ISS_BANK_CONFIGr, REG_PORT_ANY, 0,
               SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
          soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, rval);
          shared_bank_map = soc_reg_field_get(unit, L3_ISS_BANK_CONFIGr,
               rval[0], L3_ENTRY_BANK_CONFIGf);
     } else
#endif
     {	
          addr = soc_reg_addr_get(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0,
               SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
          soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, rval);
          shared_bank_map = soc_reg_field_get(unit, ISS_BANK_CONFIGr,
               rval[0], L3_ENTRY_BANK_CONFIGf);
     }
        /* calculate number of 1's in shared_bank_map */
        bit_count = shared_bank_map - ((shared_bank_map >> 1) & 0x5);
        bit_count = ((bit_count & 0xc) >> 2) + (bit_count & 0x3);

        *bank_map = (1 << (bit_count + 4)) - 1;
        *bank_start = 0;
        *bank_inc = 1;
        if (do_first_fit != NULL) {
            *do_first_fit = FALSE;
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(pcid_info->unit)) {
        int    mode;

        addr = soc_reg_addr_get(unit, L3_TABLE_HASH_CONTROLr, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
        soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, rval);
        mode = soc_reg_field_get(unit, L3_TABLE_HASH_CONTROLr, rval[0], MODEf);
        switch (mode) {
        case 1:
            *bank_map = 0x3e0;
            break;
        case 2:
            *bank_map = 0x3f0;
            break;
        case 3:
            *bank_map = 0x3f8;
            break;
        default:
            *bank_map = 0x3c0;
        }
        *bank_start = 9;
        *bank_inc = -1;
        if (do_first_fit != NULL) {
            *do_first_fit = FALSE;
        }
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        *bank_map = 0x3;
        *bank_start = 0;
        *bank_inc = 1;
        addr = soc_reg_addr_get(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
        soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, rval);
        if (do_first_fit != NULL) {
            *do_first_fit =
                soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr, rval[0],
                                  INSERT_LEAST_FULL_HALFf) ? FALSE : TRUE;
        }
    }
}

STATIC void
soc_internal_l3x2_bank_size_get(pcid_info_t *pcid_info, int bank,
                                int *entries_per_bank, int *entries_per_row,
                                int *entries_per_bucket, int *bank_base,
                                int *bucket_offset)
{
    /*
     * entry index =
     *     bank_base + bucket * entries_per_row + bucket_offset + slot;
     */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(pcid_info->unit)) {
        (void)soc_apache_hash_bank_info_get(pcid_info->unit, L3_ENTRY_ONLYm,
                                            bank, entries_per_bank,
                                            entries_per_row,
                                            entries_per_bucket,
                                            bank_base, bucket_offset);
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */


#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(pcid_info->unit)) {
        (void)soc_tomahawk_hash_bank_info_get(pcid_info->unit, L3_ENTRY_ONLYm,
                                             bank, entries_per_bank,
                                             entries_per_row,
                                             entries_per_bucket,
                                             bank_base, bucket_offset);
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(pcid_info->unit)) {
        (void)soc_trident2_hash_bank_info_get(pcid_info->unit, L3_ENTRY_ONLYm,
                                              bank, entries_per_bank,
                                              entries_per_row,
                                              entries_per_bucket,
                                              bank_base, bucket_offset);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        /*
         * first half of entries (slot 0) in each bucket are for bank 0
         * the other half of entries (slot 1) in each bucket are for bank 1
         */
        if (entries_per_bank != NULL) {
            *entries_per_bank =
                soc_mem_index_count(pcid_info->unit, L3_ENTRY_ONLYm) / 2;
        }
        *entries_per_row = SOC_L3X_BUCKET_SIZE(pcid_info->unit);
        *entries_per_bucket = *entries_per_row / 2;
        *bank_base = 0;
        *bucket_offset = bank * *entries_per_bucket;
    }
}

int
soc_internal_l3x2_entry_ins(pcid_info_t *pcid_info, uint32 inv_bank_map,
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
    int             num_vbits;
    int             validslot_cnt;
    soc_mem_t       mem;
    int             ss_field[] = {ENT0f, ENT1f, ENT2f, ENT3f};
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *) result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L3_ENTRY Insert\n")));

    rv = soc_internal_l3x2_entry_size_get(pcid_info, entry, &mem, &num_vbits);
    if (rv < 0) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    free_index = -1;

    soc_internal_l3x2_bank_map_get(pcid_info, &bank_map, &bank_start,
                                   &bank_inc, &do_first_fit);
    bank_map &= ~inv_bank_map;

    best_free_slot_count = 0;
    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_l3x2_bank_size_get(pcid_info, bank, &entries_per_bank,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        bucket = soc_internal_l3x2_bucket_get(pcid_info, bank, entry);
        free_slot_count = 0;
        first_free_slot = -1;
        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
                addr = soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, blk, index,
                                        &acc_type);
                soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr,
                                               tmp);
                if (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, tmp, VALIDf)) {
                    soc_meminfo_field_set(mem, &l3_minfo, ent,
                                          ss_field[sub_slot], tmp);
                    validslot_cnt++;
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_l3x2(unit, entry, ent) == 0) {
                    if (num_vbits > 0) {
                        index /= num_vbits;
                    }
                    addr = soc_mem_addr_get(unit, mem, 0, blk, index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "write sblk %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                 sblk, acc_type, bank, bucket, slot, index));

                    /* Overwrite the existing entry */
                    soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                    addr, entry);

                    if (soc_feature(unit, soc_feature_generic_table_ops)) {
                        /* Copy old entry immediately after response word */
                        memcpy(&result[1], ent,
                               soc_mem_entry_bytes(unit, mem));
                        result[0] = 0;
                        if (soc_feature(unit, soc_feature_new_sbus_format) &&
                            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                            genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                            genresp_v2->index = index;
                        } else {
                            genresp->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                            genresp->index = index;
                        }
                    }
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
        addr = soc_mem_addr_get(unit, mem, 0, blk, index, &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bank %d bucket %d, "
                                "slot %d, index %d\n"),
                     sblk, acc_type, free_bank, free_bucket, free_slot, index));

        /* Write the entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
                                        entry);

        if (soc_feature(unit, soc_feature_generic_table_ops)) {
            result[0] = 0;
            if (soc_feature(unit, soc_feature_new_sbus_format) &&
                !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_INSERTED;
                genresp_v2->index = index;
            } else {
                genresp->type = SCHAN_GEN_RESP_TYPE_INSERTED;
                genresp->index = index;
            }
        }
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

        return 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Bucket full\n")));
    if (soc_feature(unit, soc_feature_generic_table_ops)) {
        result[0] = 0;
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            genresp_v2->type = SCHAN_GEN_RESP_TYPE_FULL;
        } else {
            genresp->type = SCHAN_GEN_RESP_TYPE_FULL;
        }
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}

int
soc_internal_l3x2_entry_del(pcid_info_t *pcid_info, uint32 inv_bank_map,
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
    int             num_vbits;
    int             validslot_cnt;
    soc_mem_t       mem;
    int             ss_field[] = {ENT0f, ENT1f, ENT2f, ENT3f};
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *) result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L3_ENTRY Delete\n")));

    rv = soc_internal_l3x2_entry_size_get(pcid_info, entry, &mem, &num_vbits);
    if (rv < 0) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    soc_internal_l3x2_bank_map_get(pcid_info, &bank_map, &bank_start,
                                   &bank_inc, NULL);
    bank_map &= ~inv_bank_map;

    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_l3x2_bank_size_get(pcid_info, bank, NULL,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        bucket = soc_internal_l3x2_bucket_get(pcid_info, bank, entry);

        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
                addr = soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, blk, index,
                                        &acc_type);
                soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr,
                                               tmp);
                if (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, tmp, VALIDf)) {
                    soc_meminfo_field_set(mem, &l3_minfo, ent,
                                          ss_field[sub_slot], tmp);
                    validslot_cnt++;
                } else {
                    break;
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_l3x2(unit, entry, ent) == 0) {
                    if (num_vbits > 0) {
                        index /= num_vbits;
                    }
                    addr = soc_mem_addr_get(unit, mem, 0, blk, index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "delete sblk %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                 sblk, acc_type, bank, bucket, slot, index));

                    if (soc_feature(unit, soc_feature_generic_table_ops)) {
                        /* Copy entry immediately after response word */
                        memcpy(&result[1], ent,
                               soc_mem_entry_bytes(unit, mem));
                    }

                    /* Invalidate entry */
                    memset(tmp, 0, sizeof(tmp));
                    soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                    addr, (uint32 *)tmp);

                    if (soc_feature(unit, soc_feature_generic_table_ops)) {
                        result[0] = 0;
                        if (soc_feature(unit, soc_feature_new_sbus_format) &&
                            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                            genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                            genresp_v2->index = index;
                        } else {
                            genresp->type = SCHAN_GEN_RESP_TYPE_DELETED;
                            genresp->index = index;
                        }
                    }
                    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                    return 0;
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_l3x2_entry_del: Not found\n")));

    if (soc_feature(unit, soc_feature_generic_table_ops)) {
        result[0] = 0;
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
        } else {
            genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
        }
    } else {
        result[0] = 0xffffffff;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}
int
soc_internal_l3x2_entry_lkup(pcid_info_t * pcid_info, uint32 inv_bank_map,
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
    int             num_vbits;
    int             validslot_cnt;
    int             index_bit_offset;
    int             entry_dw;
    soc_mem_t       mem;
    int             ss_field[] = {ENT0f, ENT1f, ENT2f, ENT3f};
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *) result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *) result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L3_ENTRY Lookup\n")));

    rv = soc_internal_l3x2_entry_size_get(pcid_info, entry, &mem, &num_vbits);
    if (rv < 0) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    blk = SOC_MEM_BLOCK_ANY(unit, L3_ENTRY_ONLYm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    soc_internal_l3x2_bank_map_get(pcid_info, &bank_map, &bank_start,
                                   &bank_inc, NULL);
    bank_map &= ~inv_bank_map;

    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_l3x2_bank_size_get(pcid_info, bank, NULL,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        bucket = soc_internal_l3x2_bucket_get(pcid_info, bank, entry);

        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
                addr = soc_mem_addr_get(unit, L3_ENTRY_ONLYm, 0, blk, index,
                                        &acc_type);
                soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr,
                                               tmp);
                if (soc_mem_field32_get(unit, L3_ENTRY_ONLYm, tmp, VALIDf)) {
                    soc_meminfo_field_set(mem, &l3_minfo, ent,
                                          ss_field[sub_slot], tmp);
                    validslot_cnt++;
                } else {
                    break;
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_l3x2(unit, entry, ent) == 0) {
                    if (num_vbits > 0) {
                        index /= num_vbits;
                    }
                    addr = soc_mem_addr_get(unit, mem, 0, blk, index, &acc_type);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "found sblk %d acc_type %d bank %d "
                                            "bucket %d, slot %d, index %d\n"),
                                 sblk, acc_type, bank, bucket, slot, index));

                    if (soc_feature(unit, soc_feature_generic_table_ops)) {
                        /* Copy entry immediately after response word */
                        memcpy(&result[1], ent,
                               soc_mem_entry_bytes(unit, mem));

                        result[0] = 0;
                        if (soc_feature(unit, soc_feature_new_sbus_format) &&
                            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                            genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                            genresp_v2->index = index;
                        } else {
                            genresp->type = SCHAN_GEN_RESP_TYPE_FOUND;
                            genresp->index = index;
                        }
                    } else {
                        entry_dw = soc_mem_entry_words(unit, mem);
                        index_bit_offset = soc_mem_entry_bits(unit, mem) % 32;
                        for(slot = 0; slot < entry_dw - 1; slot++) {
                            result[slot] = ent[slot];
                        }
                        if (index_bit_offset) {
                            result[slot] =
                                (ent[slot] & ((1 << index_bit_offset) - 1)) |
                                ((index & ((1 << (32 - index_bit_offset)) - 1))
                                 << index_bit_offset);
                            result[slot + 1] =
                                (index >> (32 - index_bit_offset)) &
                                ((soc_mem_index_max(unit, mem)) >>
                                (32 - index_bit_offset));
                        } else {
                            result[slot] = index;
                            result[slot + 1] = 0;
                        }
                    }
                    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                    return 0;
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_l3x2_entry_lkup: Not found\n")));

    if (soc_feature(unit, soc_feature_generic_table_ops)) {
        result[0] = 0;
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
        } else {
            genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
        }
    } else {
        result[0] = 0xffffffff;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}
#endif /* BCM_FIREBOLT_SUPPORT */
#endif
