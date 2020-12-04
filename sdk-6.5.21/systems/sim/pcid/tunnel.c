/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tunnel.c
 *
 * Provides:
 *      soc_internal_tunnel_entry_ins
 *
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/drv.h>
#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>

#ifdef BCM_XGS_SWITCH_SUPPORT
#ifdef BCM_FIREBOLT_SUPPORT
/*
 * Tomahawk3 L3_TUNNEL Table Simulation
 *
 * The L3_TUNNEL table is hashed.
 *
 * There are 3 view of the same
 *      L3_TUNNEL, L3_TUNNEL_2, L3_TUNNEL_4
 */
enum {
        ENT3f,
        ENT2f,
        ENT1f,
        ENT0f
} soc_L3_TUNNEL_DUMMYm_ftype;

static soc_field_info_t soc_L3_TUNNEL_DUMMYm_fields[] = {
    { ENT3f, 92, 276, SOCF_LE },
    { ENT2f, 92, 184, SOCF_LE },
    { ENT1f, 92, 92, SOCF_LE },
    { ENT0f, 92, 0, SOCF_LE },
};
static soc_mem_info_t l3_tunnel_minfo = {
                        /* mem            L3_TUNNELXXXm */
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
                        /* data_bits */ 368,
                        /* bytes       */ 46,
                        /* nFields     */ 4, /* Used */
                        /* *fields     */ soc_L3_TUNNEL_DUMMYm_fields, /* Used*/
                        };

int
soc_internal_tunnel_entry_size_get(pcid_info_t *pcid_info, void *entry,
                                 soc_mem_t *mem, int *num_vbits)
{
    int unit = pcid_info->unit;

    if (SOC_IS_TOMAHAWK3(unit) &&
        SOC_MEM_FIELD_VALID(unit, L3_TUNNEL_SINGLEm, KEY_TYPEf)) {
        int key_type;

        key_type = soc_mem_field32_get(unit, L3_TUNNEL_SINGLEm,
                                       entry, KEY_TYPEf);
        switch (key_type) {
            case TH3_TUNNEL_HASH_KEY_TYPE_MPLS:
                *num_vbits = 1;
                *mem = L3_TUNNEL_SINGLEm;
                break;

            case TH3_TUNNEL_HASH_KEY_TYPE_V4:
                *num_vbits = 2;
                *mem = L3_TUNNEL_DOUBLEm;
                break;

            case TH3_TUNNEL_HASH_KEY_TYPE_V6:
                *num_vbits = 4;
                *mem = L3_TUNNEL_QUADm;
                break;

            default:
                return -1;
        }
        return 0;
    } else {
        return -1;
    }
}

STATIC void
soc_internal_tunnel_bank_map_get(pcid_info_t *pcid_info, uint32 *bank_map,
                               int *bank_start, int *bank_inc,
                               int *do_first_fit)
{
    *bank_start = 0;
    *bank_inc = 1;
    *bank_map = soc_feature(pcid_info->unit, soc_feature_dual_hash)? 0x3 : 0x1;

    if (do_first_fit) {
        *do_first_fit = FALSE;
    }
}

STATIC void
soc_internal_tunnel_bank_size_get(pcid_info_t *pcid_info, int bank,
                                int *entries_per_bank, int *entries_per_row,
                                int *entries_per_bucket, int *bank_base,
                                int *bucket_offset)
{
    /*
     * entry index =
     *     bank_base + bucket * entries_per_row + bucket_offset + slot;
     */
    if (SOC_IS_TOMAHAWK3(pcid_info->unit)) {
        /* coverity[check_return] */
        soc_tomahawk3_hash_bank_info_get(pcid_info->unit, L3_TUNNEL_SINGLEm,
                                             bank, entries_per_bank,
                                             entries_per_row,
                                             entries_per_bucket,
                                             bank_base, bucket_offset);
    } else {
        /*
         * first half of entries (slot 0) in each bucket are for bank 0
         * the other half of entries (slot 1) in each bucket are for bank 1
         */
        if (entries_per_bank != NULL) {
            *entries_per_bank =
                soc_mem_index_count(pcid_info->unit, L3_TUNNEL_SINGLEm) / 2;
        }
        *entries_per_row = SOC_L3_TUNNEL_BUCKET_SIZE;
        *entries_per_bucket = *entries_per_row / 2;
        *bank_base = 0;
        *bucket_offset = bank * *entries_per_bucket;
    }
}

STATIC int
soc_internal_tunnel_hash(pcid_info_t * pcid_info, uint8 bank, void *entry)
{
    int     index = 0;

    if (SOC_IS_TOMAHAWK3(pcid_info->unit)) {
        
        index = soc_tomahawk3_tunnel_bank_entry_hash(
                    pcid_info->unit, bank, (uint32*) entry);
    }

    return index;
}

int
soc_internal_tunnel_entry_ins(pcid_info_t *pcid_info, uint8 inv_bank_map,
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

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "L3_TUNNEL Insert\n")));

    rv = soc_internal_tunnel_entry_size_get(pcid_info, entry, &mem, &num_vbits);
    if (rv < 0) {
        return rv;
    }

    sal_memset(ent, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(tmp, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    blk = SOC_MEM_BLOCK_ANY(unit, L3_TUNNEL_SINGLEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    free_index = -1;

    soc_internal_tunnel_bank_map_get(
        pcid_info, &bank_map, &bank_start, &bank_inc, &do_first_fit);
    bank_map &= ~inv_bank_map;

    best_free_slot_count = 0;
    /* only 2 banks supported*/
    for (bank = bank_start; bank_map != 0; bank += bank_inc) {
        if (!(bank_map & (1 << bank))) {
            continue;
        }
        bank_map &= ~(1 << bank);

        soc_internal_tunnel_bank_size_get(pcid_info, bank, &entries_per_bank,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        bucket = soc_internal_tunnel_hash(pcid_info, bank, entry);

        /*
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "bucket:%d, bank#%d, entry/bank:%d,"
                        " entry/row:%d, entry/bucket:%d, base:%d, offset:%d\n"),
                            bucket, bank, entries_per_bank, entries_per_row,
                                entries_per_bucket, bank_base, bucket_offset));
        */

        free_slot_count = 0;
        first_free_slot = -1;

        for (slot = 0; slot < entries_per_bucket; slot += num_vbits) {
            validslot_cnt = 0;
            for (sub_slot = 0; sub_slot < num_vbits; sub_slot++) {
                index = bank_base + bucket * entries_per_row + bucket_offset +
                    slot + sub_slot;
                addr = soc_mem_addr_get(unit, L3_TUNNEL_SINGLEm, 0, blk, index,
                                        &acc_type);
                soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr,
                                               tmp);
                if (soc_mem_field32_get(unit, L3_TUNNEL_SINGLEm, tmp, BASE_VALIDf)) {
                    soc_meminfo_field_set(mem, &l3_tunnel_minfo, ent,
                                          ss_field[sub_slot], tmp);
                    validslot_cnt++;
                }
            }

            if (validslot_cnt == num_vbits) {
                if (_soc_mem_cmp_l3_tunnel(unit, entry, ent) == 0) {
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

#endif /* BCM_FIREBOLT_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
#endif /* BCM_TOMAHAWK3_SUPPORT */
