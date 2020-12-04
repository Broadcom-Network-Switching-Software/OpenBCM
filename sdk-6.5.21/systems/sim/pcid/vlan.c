/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 *      
 * Provides:
 *      soc_internal_vlan_xlate_hash
 *      soc_internal_vlan_xlate_dual_banks
 *      soc_internal_vlan_xlate_entry_read
 *      soc_internal_vlan_xlate_entry_write
 *      soc_internal_vlan_xlate_entry_ins
 *      soc_internal_vlan_xlate_entry_del
 *      soc_internal_vlan_xlate_entry_lkup
 *      soc_internal_egr_vlan_xlate_hash
 *      soc_internal_egr_vlan_xlate_dual_banks
 *      soc_internal_egr_vlan_xlate_entry_read
 *      soc_internal_egr_vlan_xlate_entry_write
 *      soc_internal_egr_vlan_xlate_entry_ins
 *      soc_internal_egr_vlan_xlate_entry_del
 *      soc_internal_egr_vlan_xlate_entry_lkup
 *      
 * Requires:    
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"

#ifdef BCM_TRX_SUPPORT

int
soc_internal_vlan_xlate_hash(pcid_info_t * pcid_info, 
                             vlan_xlate_entry_t *entry, int dual)
{
    uint32          tmp_hs[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           at, key[8];
    int             index, bits;
    soc_block_t     blk; 

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("VLAN_XLATE hash\n")));

    soc_internal_extended_read_reg(pcid_info, blk, at,
          soc_reg_addr_get(pcid_info->unit, VLAN_XLATE_HASH_CONTROLr,
                           REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE,
                           &blk, &at),
          tmp_hs);
    if (dual) {
        hash_sel = soc_reg_field_get(pcid_info->unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs[0], HASH_SELECT_Bf);
    } else {
        hash_sel = soc_reg_field_get(pcid_info->unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs[0], HASH_SELECT_Af);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(pcid_info->unit)) {
        bits = soc_td2_vlan_xlate_base_entry_to_key(pcid_info->unit, dual, entry,
                                                    key);
        index = soc_td2_vlan_xlate_hash(pcid_info->unit, hash_sel, bits, entry,
                                        key);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        bits = soc_tr_vlan_xlate_base_entry_to_key(pcid_info->unit, entry,
                                                   key);
        index = soc_tr_vlan_xlate_hash(pcid_info->unit, hash_sel, bits, entry,
                                       key);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_vlan_xlate_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                   int *dual, int *slot_min, int *slot_max)
{
    /* Dual hash always enabled for VLAN_XLATE */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_vlan_xlate_entry_read(pcid_info_t * pcid_info, uint32 addr,
                                   vlan_xlate_entry_t *entry)
{
    int index = (addr & 0x3fff);
    int             blk;             
    soc_block_t     sblk;
    uint8           at;
    uint32          address;

    blk = SOC_MEM_BLOCK_ANY(pcid_info->unit, VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(pcid_info->unit, blk);

    address = soc_mem_addr_get(pcid_info->unit, VLAN_XLATEm, 0, 
                               blk, index, &at);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("vlan_xlate read addr=0x%x\n"), addr));

    soc_internal_extended_read_mem(pcid_info, sblk, 0, address, 
                                   (uint32 *) entry);

    return 0;
}

int
soc_internal_vlan_xlate_entry_write(pcid_info_t * pcid_info, uint32 addr,
                                    vlan_xlate_entry_t *entry)
{
    int index = (addr & 0x3fff);
    int             blk;             
    soc_block_t     sblk;
    uint8           at;
    uint32          address;

    blk = SOC_MEM_BLOCK_ANY(pcid_info->unit, VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(pcid_info->unit, blk);
    address = soc_mem_addr_get(pcid_info->unit, VLAN_XLATEm, 0, 
                               blk, index, &at);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("vlan_xlate write addr=0x%x\n"), addr));

    soc_internal_extended_write_mem(pcid_info, sblk, 0, address, 
                                    (uint32 *) entry);

    return 0;
}

int
soc_internal_vlan_xlate_entry_ins(pcid_info_t *pcid_info, uint8 banks,
                                  vlan_xlate_entry_t *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "VLAN_XLAEGT Insert\n")));

    bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_vlan_xlate_dual_banks(pcid_info, banks, &dual, &slot_min,
                                           &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, VLAN_XLATEm, 0, blk, index, &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, (uint32 *)entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, VLAN_XLATEm));

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                    genresp->index = index;
                }
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, VLAN_XLATEm, 0, blk, index, &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
                                        (uint32 *)entry);

        result[0] = 0;
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            genresp_v2->type = SCHAN_GEN_RESP_TYPE_INSERTED;
            genresp_v2->index = index;
        } else {
            genresp->type = SCHAN_GEN_RESP_TYPE_INSERTED;
            genresp->index = index;
        }
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

        return 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Bucket full\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_FULL;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_FULL;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}

int
soc_internal_vlan_xlate_entry_del(pcid_info_t * pcid_info, uint8 banks,
                                  vlan_xlate_entry_t *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "VLAN_XLATE Delete\n")));

    bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_vlan_xlate_dual_banks(pcid_info, banks, &dual,
                                           &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, VLAN_XLATEm, 0, blk, index, &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, VLAN_XLATEm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, VLAN_XLATEm, tmp, VALIDf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_DELETED;
                    genresp->index = index;
                }
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_vlan_xlate_entry_del: Not found\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_vlan_xlate_entry_lkup(pcid_info_t * pcid_info, uint8 banks,
                                   vlan_xlate_entry_t *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "VLAN_XLATE Lookup\n")));

    bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_vlan_xlate_dual_banks(pcid_info, banks, &dual,
                                           &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, VLAN_XLATEm, 0, blk, index, &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, VLAN_XLATEm));

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_FOUND;
                    genresp->index = index;
                }

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_vlan_xlate_entry_lkup: Not found\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_egr_vlan_xlate_hash(pcid_info_t * pcid_info,
                                 egr_vlan_xlate_entry_t *entry, int dual)
{
    uint32          tmp_hs[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           at, key[8];
    int             index, bits;
    soc_block_t     blk;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("EGR_VLAN_XLATE hash\n")));

    soc_internal_extended_read_reg(pcid_info, blk, at,
          soc_reg_addr_get(pcid_info->unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                           REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE,
                           &blk, &at),
          tmp_hs);
    if (dual) {
        hash_sel = soc_reg_field_get(pcid_info->unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs[0], HASH_SELECT_Bf);
    } else {
        hash_sel = soc_reg_field_get(pcid_info->unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs[0], HASH_SELECT_Af);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(pcid_info->unit)) {
        bits = soc_td2_egr_vlan_xlate_base_entry_to_key(pcid_info->unit, dual,
                                                        entry, key);
        index = soc_td2_egr_vlan_xlate_hash(pcid_info->unit, hash_sel, bits,
                                           entry, key);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        bits = soc_tr_egr_vlan_xlate_base_entry_to_key(pcid_info->unit,
                                                       entry, key);
        index = soc_tr_egr_vlan_xlate_hash(pcid_info->unit, hash_sel, bits,
                                           entry, key);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_egr_vlan_xlate_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                       int *dual, int *slot_min, int *slot_max)
{
    /* Dual hash always enabled for EGR_VLAN_XLATE */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_egr_vlan_xlate_entry_ins(pcid_info_t *pcid_info, uint8 banks,
                                      egr_vlan_xlate_entry_t *entry,
                                      uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VLAN_XLATE Insert\n")));

    bucket = soc_internal_egr_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_egr_vlan_xlate_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_egr_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VLAN_XLATEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, (uint32 *)entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VLAN_XLATEm));

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                    genresp->index = index;
                }
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, EGR_VLAN_XLATEm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
                                        (uint32 *)entry);

        result[0] = 0;
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            genresp_v2->type = SCHAN_GEN_RESP_TYPE_INSERTED;
            genresp_v2->index = index;
        } else {
            genresp->type = SCHAN_GEN_RESP_TYPE_INSERTED;
            genresp->index = index;
        }
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

        return 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Bucket full\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_FULL;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_FULL;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}

int
soc_internal_egr_vlan_xlate_entry_del(pcid_info_t * pcid_info, uint8 banks,
                                      egr_vlan_xlate_entry_t *entry,
                                      uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VLAN_XLATE Delete\n")));

    bucket = soc_internal_egr_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_egr_vlan_xlate_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_egr_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VLAN_XLATEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VLAN_XLATEm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, EGR_VLAN_XLATEm, tmp, VALIDf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_DELETED;
                    genresp->index = index;
                }
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_egr_vlan_xlate_entry_del: Not found\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_egr_vlan_xlate_entry_lkup(pcid_info_t * pcid_info, uint8 banks,
                                       egr_vlan_xlate_entry_t *entry,
                                       uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_t *genresp = (schan_genresp_t *)result;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VLAN_XLATE Lookup\n")));

    bucket = soc_internal_egr_vlan_xlate_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_egr_vlan_xlate_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VLAN_XLATEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket =
                soc_internal_egr_vlan_xlate_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VLAN_XLATEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VLAN_XLATEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VLAN_XLATEm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VLAN_XLATEm));

                result[0] = 0;
                if (soc_feature(unit, soc_feature_new_sbus_format) &&
                    !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                    genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                    genresp_v2->index = index;
                } else {
                    genresp->type = SCHAN_GEN_RESP_TYPE_FOUND;
                    genresp->index = index;
                }
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_egr_vlan_xlate_entry_lkup: Not found\n")));

    result[0] = 0;
    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    } else {
        genresp->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    }
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

#ifdef BCM_TRIDENT2_SUPPORT
int
soc_internal_ing_vp_vlan_member_hash(pcid_info_t *pcid_info, void *entry,
                                     int dual)
{
    soc_reg_t       reg;
    soc_field_t     field;
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           key[8];
    int             index, bits;
    soc_block_t     blk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("ING_VP_VLAN_MEMBERSHIP hash\n")));

    reg = ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr;
    field = dual ? HASH_SELECT_Bf : HASH_SELECT_Af;

    addr = soc_reg_addr_get(pcid_info->unit, reg, REG_PORT_ANY, 0,
                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
    soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, tmp);
    hash_sel = soc_reg_field_get(pcid_info->unit, reg, tmp[0], field);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

    bits = soc_td2_ing_vp_vlan_member_base_entry_to_key(pcid_info->unit, dual,
                                                        entry, key);
    index = soc_td2_ing_vp_vlan_member_hash(pcid_info->unit, hash_sel, bits,
                                            entry, key);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_ing_vp_vlan_member_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                           int *dual, int *slot_min,
                                           int *slot_max)
{
    /* Dual hash always enabled for ING_VP_VLAN_MEMBERSHIP */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_ing_vp_vlan_member_ins(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_VP_VLAN_MEMBERSHIP Insert\n")));

    bucket = soc_internal_ing_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_ing_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_ing_vp_vlan_member_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_VP_VLAN_MEMBERSHIPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, ING_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
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
soc_internal_ing_vp_vlan_member_del(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_VP_VLAN_MEMBERSHIP Delete\n")));

    bucket = soc_internal_ing_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_ing_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_ing_vp_vlan_member_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_VP_VLAN_MEMBERSHIPm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, ING_VP_VLAN_MEMBERSHIPm, tmp, VALIDf,
                                    0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_ing_vp_vlan_member_del: "
                            "Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_ing_vp_vlan_member_lkup(pcid_info_t *pcid_info, uint8 banks,
                                     void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_VP_VLAN_MEMBERSHIP Lookup\n")));

    bucket = soc_internal_ing_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_ing_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket =
                soc_internal_ing_vp_vlan_member_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_VP_VLAN_MEMBERSHIPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_ing_vp_vlan_member_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_egr_vp_vlan_member_hash(pcid_info_t *pcid_info, void *entry,
                                     int dual)
{
    soc_reg_t       reg;
    soc_field_t     field;
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           key[8];
    int             index, bits;
    soc_block_t     blk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("EGR_VP_VLAN_MEMBERSHIP hash\n")));

    reg = EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr;
    field = dual ? HASH_SELECT_Bf : HASH_SELECT_Af;

    addr = soc_reg_addr_get(pcid_info->unit, reg, REG_PORT_ANY, 0,
                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
    soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, tmp);
    hash_sel = soc_reg_field_get(pcid_info->unit, reg, tmp[0], field);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

    bits = soc_td2_egr_vp_vlan_member_base_entry_to_key(pcid_info->unit, dual,
                                                        entry, key);
    index = soc_td2_egr_vp_vlan_member_hash(pcid_info->unit, hash_sel, bits,
                                            entry, key);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_egr_vp_vlan_member_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                           int *dual, int *slot_min,
                                           int *slot_max)
{
    /* Dual hash always enabled for EGR_VP_VLAN_MEMBERSHIP */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_egr_vp_vlan_member_ins(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VP_VLAN_MEMBERSHIP Insert\n")));

    bucket = soc_internal_egr_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_egr_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_egr_vp_vlan_member_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VP_VLAN_MEMBERSHIPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, EGR_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
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
soc_internal_egr_vp_vlan_member_del(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VP_VLAN_MEMBERSHIP Delete\n")));

    bucket = soc_internal_egr_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_egr_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_egr_vp_vlan_member_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VP_VLAN_MEMBERSHIPm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, EGR_VP_VLAN_MEMBERSHIPm, tmp, VALIDf,
                                    0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_egr_vp_vlan_member_del: "
                            "Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_egr_vp_vlan_member_lkup(pcid_info_t *pcid_info, uint8 banks,
                                     void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "EGR_VP_VLAN_MEMBERSHIP Lookup\n")));

    bucket = soc_internal_egr_vp_vlan_member_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_egr_vp_vlan_member_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, EGR_VP_VLAN_MEMBERSHIPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket =
                soc_internal_egr_vp_vlan_member_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, EGR_VP_VLAN_MEMBERSHIPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, EGR_VP_VLAN_MEMBERSHIPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, EGR_VP_VLAN_MEMBERSHIPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_egr_vp_vlan_member_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_ing_dnat_address_type_hash(pcid_info_t *pcid_info, void *entry,
                                        int dual)
{
    soc_reg_t       reg;
    soc_field_t     field;
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           key[8];
    int             index, bits;
    soc_block_t     blk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("ING_DNAT_ADDRESS_TYPE hash\n")));

    reg = ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr;
    field = dual ? HASH_SELECT_Bf : HASH_SELECT_Af;

    addr = soc_reg_addr_get(pcid_info->unit, reg, REG_PORT_ANY, 0,
                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
    soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, tmp);
    hash_sel = soc_reg_field_get(pcid_info->unit, reg, tmp[0], field);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

    bits = soc_td2_ing_dnat_address_type_base_entry_to_key(pcid_info->unit, dual,
                                                           entry, key);
    index = soc_td2_ing_dnat_address_type_hash(pcid_info->unit, hash_sel, bits,
                                               entry, key);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_ing_dnat_address_type_dual_banks(pcid_info_t *pcid_info,
                                              uint8 banks, int *dual,
                                              int *slot_min, int *slot_max)
{
    /* Dual hash always enabled for ING_DNAT_ADDRESS_TYPE */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_ing_dnat_address_type_ins(pcid_info_t *pcid_info, uint8 banks,
                                       void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_DNAT_ADDRESS_TYPE Insert\n")));

    bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_ing_dnat_address_type_dual_banks(pcid_info, banks, &dual,
                                                      &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_DNAT_ADDRESS_TYPEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry,
                                                             TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_DNAT_ADDRESS_TYPEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_DNAT_ADDRESS_TYPEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_DNAT_ADDRESS_TYPEm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_DNAT_ADDRESS_TYPEm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, ING_DNAT_ADDRESS_TYPEm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
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
soc_internal_ing_dnat_address_type_del(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_DNAT_ADDRESS_TYPE Delete\n")));

    bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_ing_dnat_address_type_dual_banks(pcid_info, banks, &dual,
                                                      &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_DNAT_ADDRESS_TYPEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry,
                                                             TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_DNAT_ADDRESS_TYPEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_DNAT_ADDRESS_TYPEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_DNAT_ADDRESS_TYPEm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_DNAT_ADDRESS_TYPEm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, ING_DNAT_ADDRESS_TYPEm, tmp, VALIDf,
                                    0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_ing_dnat_address_type_del: "
                            "Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_ing_dnat_address_type_lkup(pcid_info_t *pcid_info, uint8 banks,
                                     void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ING_DNAT_ADDRESS_TYPE Lookup\n")));

    bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_ing_dnat_address_type_dual_banks(pcid_info, banks, &dual,
                                                      &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ING_DNAT_ADDRESS_TYPEm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_ing_dnat_address_type_hash(pcid_info, entry,
                                                             TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ING_DNAT_ADDRESS_TYPEm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ING_DNAT_ADDRESS_TYPEm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ING_DNAT_ADDRESS_TYPEm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ING_DNAT_ADDRESS_TYPEm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_ing_dnat_address_type_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_l2_endpoint_id_hash(pcid_info_t *pcid_info, void *entry,
                                 int dual)
{
    soc_reg_t       reg;
    soc_field_t     field;
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           key[8];
    int             index, bits;
    soc_block_t     blk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("L2_ENDPOINT_ID hash\n")));

    reg = L2_ENDPOINT_ID_HASH_CONTROLr;
    field = dual ? HASH_SELECT_Bf : HASH_SELECT_Af;

    addr = soc_reg_addr_get(pcid_info->unit, reg, REG_PORT_ANY, 0,
                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
    soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, tmp);
    hash_sel = soc_reg_field_get(pcid_info->unit, reg, tmp[0], field);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

    bits = soc_td2_l2_endpoint_id_base_entry_to_key(pcid_info->unit, dual,
                                                    entry, key);
    index = soc_td2_l2_endpoint_id_hash(pcid_info->unit, hash_sel, bits, entry,
                                        key);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_l2_endpoint_id_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                       int *dual, int *slot_min, int *slot_max)
{
    /* Dual hash always enabled for L2_ENDPOINT_ID */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_l2_endpoint_id_ins(pcid_info_t *pcid_info, uint8 banks,
                                void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L2_ENDPOINT_ID Insert\n")));

    bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_l2_endpoint_id_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, L2_ENDPOINT_IDm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, L2_ENDPOINT_IDm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, L2_ENDPOINT_IDm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, L2_ENDPOINT_IDm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, L2_ENDPOINT_IDm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, L2_ENDPOINT_IDm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
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
soc_internal_l2_endpoint_id_del(pcid_info_t *pcid_info, uint8 banks,
                                void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L2_ENDPOINT_ID Delete\n")));

    bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_l2_endpoint_id_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, L2_ENDPOINT_IDm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, L2_ENDPOINT_IDm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, L2_ENDPOINT_IDm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, L2_ENDPOINT_IDm, entry, tmp) == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, L2_ENDPOINT_IDm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, L2_ENDPOINT_IDm, tmp, VALIDf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_l2_endpoint_id_del: "
                            "Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_l2_endpoint_id_lkup(pcid_info_t *pcid_info, uint8 banks,
                                 void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "L2_ENDPOINT_ID Lookup\n")));

    bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_l2_endpoint_id_dual_banks(pcid_info, banks, &dual,
                                               &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, L2_ENDPOINT_IDm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_l2_endpoint_id_hash(pcid_info, entry, TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, L2_ENDPOINT_IDm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, L2_ENDPOINT_IDm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, L2_ENDPOINT_IDm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, L2_ENDPOINT_IDm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_l2_endpoint_id_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_endpoint_queue_map_hash(pcid_info_t *pcid_info, void *entry,
                                     int dual)
{
    soc_reg_t       reg;
    soc_field_t     field;
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             hash_sel;
    uint8           key[8];
    int             index, bits;
    soc_block_t     blk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("ENDPOINT_QUEUE_MAP hash\n")));

    reg = ENDPOINT_QUEUE_MAP_HASH_CONTROLr;
    field = dual ? HASH_SELECT_Bf : HASH_SELECT_Af;

    addr = soc_reg_addr_get(pcid_info->unit, reg, REG_PORT_ANY, 0,
                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
    soc_internal_extended_read_reg(pcid_info, blk, acc_type, addr, tmp);
    hash_sel = soc_reg_field_get(pcid_info->unit, reg, tmp[0], field);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("hash_sel%s %d\n"), dual ? "(b)" : "(a)", hash_sel));

    bits = soc_td2_endpoint_queue_map_base_entry_to_key(pcid_info->unit, dual,
                                                        entry, key);
    index = soc_td2_endpoint_queue_map_hash(pcid_info->unit, hash_sel, bits,
                                            entry, key);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("index %d\n"), index));

    return index;
}

void
soc_internal_endpoint_queue_map_dual_banks(pcid_info_t *pcid_info, uint8 banks,
                                           int *dual, int *slot_min,
                                           int *slot_max)
{
    /* Dual hash always enabled for ENDPOINT_QUEUE_MAP */
    *dual = TRUE;

    if (*dual) {
        switch (banks) {
        case 0:
            return; /* Nothing to do here */
        case 1:
            *slot_min = 4;
            *slot_max = 7;
            break;
        case 2:
            *slot_min = 0;
            *slot_max = 3;
            break;
        default:
            *slot_min = 0;
            *slot_max = -1;
            break;
        }
    }

    return;
}

int
soc_internal_endpoint_queue_map_ins(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot, free_index;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ENDPOINT_QUEUE_MAP Insert\n")));

    bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_endpoint_queue_map_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ENDPOINT_QUEUE_MAPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if it should overwrite an existing entry */
    free_index = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ENDPOINT_QUEUE_MAPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ENDPOINT_QUEUE_MAPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ENDPOINT_QUEUE_MAPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "write sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, index / 8, index % 8, index));

                /* Overwrite the existing entry */
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, entry);

                /* Copy old entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ENDPOINT_QUEUE_MAPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_REPLACED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        } else {
            if (free_index == -1) {
                free_index = index;
            }
        }
    }

    /* Find first unused slot in bucket */
    if (free_index != -1) {
        index = free_index;
        addr = soc_mem_addr_get(unit, ENDPOINT_QUEUE_MAPm, 0, blk, index,
                                &acc_type);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "write sblk %d acc_type %d bucket %d, slot %d, "
                                "index %d\n"),
                     sblk, acc_type, index / 8, index % 8, index));

        /* Write the new entry */
        soc_internal_extended_write_mem(pcid_info, sblk, acc_type, addr,
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
soc_internal_endpoint_queue_map_del(pcid_info_t *pcid_info, uint8 banks,
                                    void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ENDPOINT_QUEUE_MAP Delete\n")));

    bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(pcid_info->unit, soc_feature_dual_hash)) {
        soc_internal_endpoint_queue_map_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ENDPOINT_QUEUE_MAPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ENDPOINT_QUEUE_MAPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ENDPOINT_QUEUE_MAPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ENDPOINT_QUEUE_MAPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ENDPOINT_QUEUE_MAPm));

                /* Invalidate entry */
                soc_mem_field32_set(unit, ENDPOINT_QUEUE_MAPm, tmp, VALIDf, 0);
                soc_internal_extended_write_mem(pcid_info, sblk, acc_type,
                                                addr, tmp);

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_DELETED;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_endpoint_queue_map_del: "
                            "Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;
    return 0;
}

int
soc_internal_endpoint_queue_map_lkup(pcid_info_t *pcid_info, uint8 banks,
                                     void *entry, uint32 *result)
{
    uint32          tmp[SOC_MAX_MEM_WORDS];
    int             index = 0, bucket, slot;
    int             slot_min = 0, slot_max = 7, dual = FALSE;
    int             unit = pcid_info->unit;
    schan_genresp_v2_t *genresp_v2 = (schan_genresp_v2_t *)result;
    int             blk;
    soc_block_t     sblk;
    uint8           acc_type;
    uint32          addr;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "ENDPOINT_QUEUE_MAP Lookup\n")));

    bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry, FALSE);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "bucket %d\n"), bucket));

    if (soc_feature(unit, soc_feature_dual_hash)) {
        soc_internal_endpoint_queue_map_dual_banks(pcid_info, banks, &dual,
                                                   &slot_min, &slot_max);
    }
    blk = SOC_MEM_BLOCK_ANY(unit, ENDPOINT_QUEUE_MAPm);
    sblk = SOC_BLOCK2SCH(unit, blk);

    /* Check if entry exists */
    for (slot = slot_min; slot <= slot_max; slot++) {
        if (dual && (slot == 4)) {
            bucket = soc_internal_endpoint_queue_map_hash(pcid_info, entry,
                                                          TRUE);
        }
        index = bucket * 8 + slot;
        addr = soc_mem_addr_get(unit, ENDPOINT_QUEUE_MAPm, 0, blk, index,
                                &acc_type);
        soc_internal_extended_read_mem(pcid_info, sblk, acc_type, addr, tmp);
        if (soc_mem_field32_get(unit, ENDPOINT_QUEUE_MAPm, tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, ENDPOINT_QUEUE_MAPm, entry, tmp)
                == 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "found sblk %d acc_type %d bucket %d, "
                                        "slot %d, index %d\n"),
                             sblk, acc_type, bucket, slot, index));

                /* Copy entry immediately after response word */
                memcpy(&result[1], tmp,
                       soc_mem_entry_bytes(unit, ENDPOINT_QUEUE_MAPm));

                result[0] = 0;
                genresp_v2->type = SCHAN_GEN_RESP_TYPE_FOUND;
                genresp_v2->index = index;
                PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

                return 0;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_internal_endpoint_queue_map_lkup: Not found\n")));

    result[0] = 0;
    genresp_v2->type = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST;

    return 0;
}

#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* BCM_TRX_SUPPORT */
