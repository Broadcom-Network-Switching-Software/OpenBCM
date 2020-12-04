/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The part of PCID that simulates the ism
 *
 * Requires:
 *
 * Provides:
 */

#include <shared/bsl.h>

#include <unistd.h>
#include <stdlib.h>
#include <soc/drv.h>
#include <sys/types.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/ism.h>
#include <soc/ism_hash.h>
#include <soc/cmic.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <soc/mcm/driver.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "ism.h"
#include "cmicsim.h"
#include "dma.h"

#ifdef BCM_ISM_SUPPORT

uint8 ism_init = 0;

extern uint32 soc_crc32b(uint8 *data, int data_nbits);
extern uint16 soc_crc16b(uint8 *data, int data_nbits);
extern soc_field_t _ism_table_bank_cfg_fld[];
extern soc_reg_t _ism_stage_hash_cfg_reg[];
extern soc_field_t _ism_stage_hash_cfg_fld[];

extern _soc_ism_t _soc_ism_info[SOC_MAX_NUM_DEVICES];
extern _soc_ism_hash_t _soc_ism_hash_info[SOC_MAX_NUM_DEVICES];
extern uint32 _soc_ism_bank_raw_sizes[];
/*
 * Macro used by memory accessor functions to fix order
 */
#define FIX_MEM_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BYTES2WORDS((m)->bytes)-1-(v) : \
                                (v))

static soc_reg_t _ism_table_bank_cfg_reg[] = {
    TABLE0_BANK_CONFIGr, TABLE1_BANK_CONFIGr,
    TABLE2_BANK_CONFIGr, TABLE3_BANK_CONFIGr,
    TABLE4_BANK_CONFIGr
};

void 
soc_internal_init_hash_state(pcid_info_t *pcid_info)
{
    int         t, s, b;
    uint32      rval, banks;
    uint32      data[SOC_MAX_MEM_WORDS];
    soc_block_t blk;
    uint8       at;

    /* Assign everything to ESM to begin with */
    for (b = 0; b < _SOC_ISM_MAX_BANKS; b++) {
        _soc_ism_bank_avail[pcid_info->unit][b] = SOC_ISM_MEM_ESM_L2;
    }
    for (t = 0; t < _SOC_ISM_MAX_TABLES; t++) {
        soc_internal_extended_read_reg(pcid_info, blk, at,
            soc_reg_addr_get(pcid_info->unit, _ism_table_bank_cfg_reg[t],
                             REG_PORT_ANY, 0,
                             SOC_REG_ADDR_OPTION_NONE, &blk, &at), data);
        rval = data[0];
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Table: %d = %x\n"), t, rval));
        for (s = 0; s < _SOC_ISM_MAX_STAGES; s++) {
            banks = soc_reg_field_get(pcid_info->unit, _ism_table_bank_cfg_reg[t],
                                     rval, _ism_table_bank_cfg_fld[s]);
            for (b = 0; b < _SOC_ISM_BANKS_PER_STAGE; b++) { 
                if (banks & (1 << b)) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META("Bank[%d] = mem[%d]\n"), (s * _SOC_ISM_BANKS_PER_STAGE) + b, t+1));
                    _soc_ism_bank_avail[pcid_info->unit][(s * _SOC_ISM_BANKS_PER_STAGE) + b] = t+1;
                }
            } 
        }
    }
} 

STATIC void
soc_internal_generic_hash_init(pcid_info_t *pcid_info)
{
    int chip, unit = pcid_info->unit;
    uint16 dev_id;
    
    SOC_ISM_INFO(unit) = &_soc_ism_info[unit];
    SOC_ISM_HASH_INFO(unit) = &_soc_ism_hash_info[unit];

    chip = soc_chip_type_to_index(pcid_info->opt_chip);
    if (pcid_info->opt_override_devid) {
        dev_id = pcid_info->opt_override_devid;
    } else {
        dev_id = soc_base_driver_table[chip]->pci_device;
    }

    switch (dev_id) {
    case BCM56648_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
        SOC_ISM_INFO(unit)->ism_mode = _ISM_SIZE_MODE_176;
        break;
    case BCM56543_DEVICE_ID:
        SOC_ISM_INFO(unit)->ism_mode = _ISM_SIZE_MODE_256;
        break;
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56346_DEVICE_ID:
    case BCM56345_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:
        SOC_ISM_INFO(unit)->ism_mode = _ISM_SIZE_MODE_96;
        break;
    case BCM56640_DEVICE_ID:
    case BCM56643_DEVICE_ID:
    case BCM56644_DEVICE_ID:
    default:
        SOC_ISM_INFO(unit)->ism_mode = _ISM_SIZE_MODE_512;
        break;
    }
    
    switch (SOC_ISM_INFO(unit)->ism_mode) {
    case _ISM_SIZE_MODE_256:
        SOC_ISM_INFO(unit)->bank_raw_sizes = _soc_ism_bank_raw_sizes_256;
        SOC_ISM_INFO(unit)->banks_per_stage = _SOC_ISM_BANKS_PER_STAGE;
        SOC_ISM_INFO(unit)->max_banks = _SOC_ISM_MAX_BANKS;
        SOC_ISM_INFO(unit)->total_banks = _SOC_ISM_TOTAL_BANKS;
        SOC_ISM_INFO(unit)->total_entries = _SOC_ISM_ENTRIES_PER_BKT *
                                            _soc_ism_bank_total(unit);
        SOC_ISM_INFO(unit)->max_raw_banks = _SOC_ISM_MAX_RAW_BANKS;
        SOC_ISM_INFO(unit)->real_bank_map = _soc_ism_real_bank_map;
        break;
    case _ISM_SIZE_MODE_176:
        SOC_ISM_INFO(unit)->bank_raw_sizes = _soc_ism_bank_raw_sizes_256_176;
        SOC_ISM_INFO(unit)->banks_per_stage = _SOC_ISM_BANKS_PER_STAGE_176;
        SOC_ISM_INFO(unit)->max_banks = _SOC_ISM_MAX_BANKS_176;
        SOC_ISM_INFO(unit)->total_banks = _SOC_ISM_TOTAL_BANKS_176;
        SOC_ISM_INFO(unit)->total_entries = _SOC_ISM_ENTRIES_PER_BKT *
                                            _soc_ism_bank_total(unit);
        SOC_ISM_INFO(unit)->max_raw_banks = _SOC_ISM_MAX_RAW_BANKS;
        SOC_ISM_INFO(unit)->real_bank_map = _soc_ism_real_bank_map_176;
        break;
    case _ISM_SIZE_MODE_96:
        SOC_ISM_INFO(unit)->bank_raw_sizes = _soc_ism_bank_raw_sizes_256_96;
        SOC_ISM_INFO(unit)->banks_per_stage = _SOC_ISM_BANKS_PER_STAGE_96;
        SOC_ISM_INFO(unit)->max_banks = _SOC_ISM_MAX_BANKS_96;
        SOC_ISM_INFO(unit)->total_banks = _SOC_ISM_TOTAL_BANKS_96;
        SOC_ISM_INFO(unit)->total_entries = _SOC_ISM_ENTRIES_PER_BKT *
                                            _soc_ism_bank_total(unit);
        SOC_ISM_INFO(unit)->max_raw_banks = _SOC_ISM_MAX_RAW_BANKS;
        SOC_ISM_INFO(unit)->real_bank_map = _soc_ism_real_bank_map_96;
        break;
    case _ISM_SIZE_MODE_512:
    default:
        SOC_ISM_INFO(unit)->bank_raw_sizes = _soc_ism_bank_raw_sizes;
        SOC_ISM_INFO(unit)->banks_per_stage = _SOC_ISM_BANKS_PER_STAGE;
        SOC_ISM_INFO(unit)->max_banks = _SOC_ISM_MAX_BANKS;
        SOC_ISM_INFO(unit)->total_banks = _SOC_ISM_TOTAL_BANKS;
        SOC_ISM_INFO(unit)->total_entries = _SOC_ISM_ENTRIES_PER_BKT *
                                            _soc_ism_bank_total(unit);
        SOC_ISM_INFO(unit)->max_raw_banks = _SOC_ISM_MAX_RAW_BANKS;
        SOC_ISM_INFO(unit)->real_bank_map = _soc_ism_real_bank_map;
        break;
    }


    /* Attach the hash info structures based upon chip type */
    _SOC_ISM_BANKS(unit) = _soc_ism_shb[unit];
    _SOC_ISM_SETS(unit) = _soc_ism_shms;
    _SOC_ISM_MEMS(unit) = _soc_ism_shm;
    _SOC_ISM_VIEWS(unit) = _soc_ism_shmv;
    _SOC_ISM_KEYS(unit) = _soc_ism_shk;
    
    soc_internal_init_hash_state(pcid_info);
    soc_ism_hash_init(unit);
    ism_init = 1;
}

static int
soc_internal_generic_hash(pcid_info_t *pcid_info, soc_mem_t mem, void *entry, 
                          int banks, uint8 op, uint32 *index, uint32 *result)
{
    int blk, unit = pcid_info->unit;
    uint8 found = 0, key[32], tmp_key[32], crc_key[32];
    uint8 at, idxinc = 1, num_flds, num_banks = 0;
    uint16 lsb, midx, num_bits;
    int32 bits, memidx, bucket = 0;
    uint32 tmp_hs[SOC_MAX_MEM_WORDS];
    uint32 idx, bidx = 0, zero_lsb, offset, mask;
    soc_block_t sblk;
    soc_hash_bank_t *shbank;
    _soc_ism_sbo_t sbo[_SOC_ISM_MAX_BANKS]; /* Dynamically created bank info based upon
                                               requested banks used for final
                                               bucket/index calculation */
    soc_field_t keyflds[MAX_FIELDS], lsbfld, vf = VALIDf;

    if (!ism_init) {
         soc_internal_generic_hash_init(pcid_info);
    }

    memidx = soc_ism_get_hash_mem_idx(unit, mem);
    if ((mem == INVALIDm) || (memidx < 0)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Invalid hash memory !!\n")));
        return SOC_E_PARAM;
    }
    if (!_SOC_ISM_MEMS(unit)[memidx].shms->num_banks) {
        return SOC_E_PARAM;
    }
    if (soc_generic_get_hash_key(unit, mem, entry, keyflds, &lsbfld, 
                                 &num_flds) == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Key field(s): ")));
        for (idx = 0; idx < num_flds; idx++) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("%d, "), keyflds[idx]));
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("\nLsb field: %d\n"), lsbfld));
        sal_memset(key, 0, sizeof(key));
        sal_memset(crc_key, 0, sizeof(crc_key));
        soc_ism_gen_key_from_keyfields(unit, mem, entry, keyflds, key, num_flds);
        num_bits = _SOC_ISM_MEMS(unit)[memidx].shms->max_key_bits;
        soc_ism_gen_crc_key_from_keyfields(unit, mem, entry, keyflds, crc_key, 
                                           num_flds, &num_bits);
        lsb = soc_mem_field32_get(unit, mem, entry, lsbfld);
    } else {
        LOG_CLI((BSL_META("Key field not found !!\n")));
        return -1;
    } 
    blk = SOC_MEM_BLOCK_ANY(unit, mem);
    sblk = SOC_BLOCK2SCH(unit, blk);
    shbank = _SOC_ISM_MEMS(unit)[memidx].shms->shb;
    bits = soc_mem_entry_bits(unit, mem);
    if (bits > (_SOC_ISM_ENTRY_BITS * 2)) {
        idxinc = 4;
    } else if (bits > _SOC_ISM_ENTRY_BITS) {
        idxinc = 2;
    }

    soc_internal_extended_read_reg(pcid_info, sblk, at, soc_reg_addr_get(unit, 
                                   _ism_table_bank_cfg_reg[0], REG_PORT_ANY, 0, 
                                   SOC_REG_ADDR_OPTION_NONE, &sblk, &at),
                                   tmp_hs);
    zero_lsb = soc_reg_field_get(unit, _ism_table_bank_cfg_reg[0],
                                 tmp_hs[0], HASH_ZERO_OR_LSBf);
    sal_memset(sbo, 0, sizeof(sbo));
    for (idx = 0; idx < _SOC_ISM_MEMS(unit)[memidx].shms->num_banks; idx++) {
        if (banks != -1) {
            /* only use the indicated bank */
            if (!((((int32)1) << shbank[idx].my_id) & banks)) {
                continue;
            }
            num_banks++;
        } else {
            num_banks++;
        }
        sbo[bidx].mode = -1;
        sbo[bidx].bidx = bidx;
        mask = soc_ism_get_hash_bucket_mask(shbank[idx].num_bkts);
        soc_internal_extended_read_reg(pcid_info, sblk, at,
                                       soc_reg_addr_get(unit, 
                                       _ism_stage_hash_cfg_reg[shbank[idx].my_id/_SOC_ISM_BANKS_PER_STAGE],
                                       REG_PORT_ANY, 0,
                                       SOC_REG_ADDR_OPTION_NONE, &sblk, &at),
                                       tmp_hs);
        offset = soc_reg_field_get(unit, _ism_stage_hash_cfg_reg[0],
                                   tmp_hs[0], 
                                   _ism_stage_hash_cfg_fld[shbank[idx].my_id%_SOC_ISM_BANKS_PER_STAGE]);
        bucket = soc_generic_gen_hash(unit, zero_lsb, num_bits, offset, 
                                      mask, crc_key, lsb);
        sbo[bidx].index = bucket;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Bank[%d]: bucket:%d\n"), bidx, bucket));
        sbo[bidx].index = (shbank[idx].base_entry/idxinc) + 
                         (sbo[bidx].index * (shbank[idx].bkt_size/idxinc));
        for (midx = 0; midx < shbank[idx].bkt_size/idxinc; midx++) {
            soc_internal_extended_read_mem(pcid_info, sblk, at,
                                           soc_mem_addr_get(unit, mem, 0, blk,
                                           sbo[idx].index+midx, &at), tmp_hs);
            if (!soc_mem_field_valid(unit, mem, VALIDf)) {
                vf = VALID_0f; 
            }
            if (soc_mem_field32_get(unit, mem, tmp_hs, vf)) {
                sal_memset(tmp_key, 0, sizeof(tmp_key));
                /* compare key */
                soc_ism_gen_key_from_keyfields(unit, mem, tmp_hs, keyflds, tmp_key, num_flds);
                if (!sal_memcmp(key, tmp_key, sizeof(key))) {
                    found++;
                    sbo[bidx].entry = midx;
                    sbo[bidx].stage = shbank[idx].my_id / _SOC_ISM_BANKS_PER_STAGE;
                    sbo[bidx].bank = shbank[idx].my_id % _SOC_ISM_BANKS_PER_STAGE;
                    sbo[bidx].mode = 1;
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META("Existing mem index: %d\n"), sbo[bidx].index+sbo[bidx].entry));
                    if (op != TABLE_INSERT_CMD_MSG) {
                        *index = sbo[bidx].index + midx;
                        if (op == TABLE_LOOKUP_CMD_MSG) { 
                            *result = SCHAN_GEN_RESP_TYPE_FOUND;
                        } else {
                            *result = SCHAN_GEN_RESP_TYPE_DELETED;
                        }
                        return SOC_E_NONE;
                    }
                    break;
                }
            } else if (op == TABLE_INSERT_CMD_MSG) {
                found++;
                sbo[bidx].entry = midx;
                sbo[bidx].stage = shbank[idx].my_id / _SOC_ISM_BANKS_PER_STAGE;
                sbo[bidx].bank = shbank[idx].my_id % _SOC_ISM_BANKS_PER_STAGE;
                sbo[bidx].mode = 0;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META("New mem index: %d\n"), sbo[bidx].index+sbo[bidx].entry));
                break;
            }
        }
        bidx++;
    }
    if (!found) {
        if (op == TABLE_INSERT_CMD_MSG) {
            *result = SCHAN_GEN_RESP_TYPE_FULL;
        } else {
            *result = SCHAN_GEN_RESP_TYPE_NOT_FOUND;
            return SOC_E_NONE;
        }
    } 
    /* Get the lowest valid/empty index of the lowest bank-id of the lowest stage-id */
    if (found > 1) {
        soc_ism_resolve_entry_index(sbo, num_banks); 
    }
    for (idx = 0; idx < num_banks; idx++) {
        if (sbo[idx].mode >= 0) {
            *index = sbo[idx].index + sbo[idx].entry;
            if (sbo[idx].mode) {
                *result = SCHAN_GEN_RESP_TYPE_REPLACED;
            } else {
                *result = SCHAN_GEN_RESP_TYPE_INSERTED;
            }
            break;
        }
    }
    return SOC_E_NONE;
}

int 
soc_internal_generic_hash_insert(pcid_info_t *pcid_info, soc_mem_t mem, int32 banks, 
                                 void *entry, uint32 *result)
{   
    int rc,     blk;             
    soc_block_t sblk;
    uint8       at;
    uint32      index, opres;
    int         unit = pcid_info->unit; 
    schan_genresp_v2_t *genresp = (schan_genresp_v2_t *) result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Mem: %d\n"), mem));
    rc = soc_internal_generic_hash(pcid_info, mem, entry, banks, TABLE_INSERT_CMD_MSG,
                                   &index, &opres);
    
    genresp->err_info = 0;
    if (rc) {
        genresp->type = SCHAN_GEN_RESP_TYPE_ERROR;
        genresp->index = -1;
    } else {
        genresp->type = opres;
        if (opres == SCHAN_GEN_RESP_TYPE_FULL) {
            genresp->index = -1;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Full\n")));
        } else {
            blk = SOC_MEM_BLOCK_ANY(unit, mem);
            sblk = SOC_BLOCK2SCH(unit, blk);
            genresp->index = index;
            soc_internal_extended_write_mem(pcid_info, sblk, at,
                                            soc_mem_addr_get(unit, mem, 0,
                                            blk, index, &at), (uint32 *)entry);
            if (opres == SCHAN_GEN_RESP_TYPE_REPLACED) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Replace ")));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Insert ")));
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Index: %d\n"), index));
        }
    }
    return SOC_E_NONE;
}

int 
soc_internal_generic_hash_lookup(pcid_info_t *pcid_info, soc_mem_t mem, int banks, 
                                 void *entry, uint32 *result)
{
    int rc;
    uint32 index;
    schan_genresp_v2_t *genresp = (schan_genresp_v2_t *) result;
    uint32 opres;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Mem: %d\n"), mem));
    rc = soc_internal_generic_hash(pcid_info, mem, entry, banks, TABLE_LOOKUP_CMD_MSG,
                                   &index, &opres);
    
    genresp->err_info = 0;
    if (rc) {
        genresp->type = SCHAN_GEN_RESP_TYPE_ERROR;
        genresp->index = -1;
    } else if (opres == SCHAN_GEN_RESP_TYPE_NOT_FOUND) {
        genresp->type = opres;
        genresp->index = -1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Not found\n")));
    } else {
        genresp->type = opres;
        genresp->index = index;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Found Index: %d\n"), index));
    }
    return SOC_E_NONE;
}

int 
soc_internal_generic_hash_delete(pcid_info_t *pcid_info, soc_mem_t mem, int banks, 
                                 void *entry, uint32 *result)
{
    int         rc, blk;             
    soc_block_t sblk;
    uint8       at;
    uint32      index, opres, tmp[SOC_MAX_MEM_WORDS];
    int         unit = pcid_info->unit; 
    schan_genresp_v2_t *genresp = (schan_genresp_v2_t *) result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Mem: %d\n"), mem));
    rc = soc_internal_generic_hash(pcid_info, mem, entry, banks, TABLE_DELETE_CMD_MSG,
                                   &index, &opres);
    
    genresp->err_info = 0;
    if (rc) {
        genresp->type = SCHAN_GEN_RESP_TYPE_ERROR;
        genresp->index = -1;
    } else if (opres == SCHAN_GEN_RESP_TYPE_NOT_FOUND) {
        genresp->type = opres;
        genresp->index = -1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Not found\n")));
    } else {
        genresp->type = opres;
        genresp->index = index;
        blk = SOC_MEM_BLOCK_ANY(unit, mem);
        sblk = SOC_BLOCK2SCH(unit, blk);
        soc_internal_extended_read_mem(pcid_info, sblk, at,
                                        soc_mem_addr_get(unit, mem, 0,
                                        blk, index, &at), (uint32 *)entry);
        if (soc_mem_field_valid(unit, mem, VALIDf)) {
            soc_mem_field32_set(unit, mem, tmp, VALIDf, 0);
        } else {
            soc_mem_field32_set(unit, mem, tmp, VALID_0f, 0);
            if (soc_mem_field_valid(unit, mem, VALID_1f)) {
                soc_mem_field32_set(unit, mem, tmp, VALID_1f, 0);
            }
            if (soc_mem_field_valid(unit, mem, VALID_2f)) {
                soc_mem_field32_set(unit, mem, tmp, VALID_2f, 0);
            }
            if (soc_mem_field_valid(unit, mem, VALID_3f)) {
                soc_mem_field32_set(unit, mem, tmp, VALID_3f, 0);
            }
        }  
        soc_internal_extended_write_mem(pcid_info, sblk, at,
                                        soc_mem_addr_get(unit, mem, 0,
                                        blk, index, &at), (uint32 *)tmp);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Deleted Index: %d\n"), index));
    }
    return SOC_E_NONE;
}

#endif

