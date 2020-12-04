/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      tcam.c
 */

#include <soc/debug.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/er_tcam.h>
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif

/*
 * seer_config_ext_table_config (an actual config value?)
 * min_blk_num_acl (always 0?)
 * max_blk_num_acl
 * min_blk_num_lpm (always "max_blk_num_acl + 1"?)
 * max_blk_num_lpm (always max block?)
 * The nstart is calculated as:
 * nstart = nse_table_start / relative entry size
 * Assuming that the tcam is always partitioned into 2 tables, with
 * a 144-bit table goes from address 0, the only configuration parameter would
 * number of blocks in it.
 * min_blk_num_lpm = nse_144_blk_cnt;
 * nstart_72b_table = nse_144_blk_cnt * entries_per_block
 */

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
soc_triumph_tcam_init(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return tr3_tcam_init(unit);
    }
#endif
    return tr_tcam_init_type1(unit);
}

STATIC int
soc_triumph_tcam_write_entry(int unit, int part, int index,
                             uint32 *mask, uint32 *data)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_tcam_write_entry(unit, part, index, mask, data, 1);
    }
#endif
    return soc_tr_tcam_type1_write_entry(unit, part, index, mask, data);
}

STATIC int
soc_triumph_tcam_read_entry(int unit, int part, int index,
                            uint32 *mask, uint32 *data, int *valid)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_tcam_read_entry(unit, part, index, mask, data, valid);
    }
#endif
    return soc_tr_tcam_type1_read_entry(unit, part, index, mask, data, valid);
}

STATIC int
soc_triumph_tcam_search_entry(int unit, int part1, int part2, uint32 *data,
                              int *index1, int *index2)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_tcam_search_entry(unit, part1, part2, data, index1,
                                          index2);
    }
#endif
    return soc_tr_tcam_type1_search_entry(unit, part1, part2, data, index1,
                                          index2);
}

STATIC int
soc_triumph_tcam_write_reg(int unit, uint32 addr, uint32 d0_msb, uint32 d1,
                           uint32 d2_lsb)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_tcam_write_reg(unit, addr, d0_msb, d1, d2_lsb);
    }
#endif
    return soc_tr_tcam_type1_write_reg(unit, addr, d0_msb, d1, d2_lsb);
}

STATIC int
soc_triumph_tcam_read_reg(int unit, uint32 addr, uint32 *d0_msb, uint32 *d1,
                          uint32 *d2_lsb)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_tcam_read_reg(unit, addr, d0_msb, d1, d2_lsb);
    }
#endif
    return soc_tr_tcam_type1_read_reg(unit, addr, d0_msb, d1, d2_lsb);
}

STATIC int
soc_triumph_tcam_get_info(int u, int *type, int *subtype, int *dc_val,
                          soc_tcam_info_t **tcam_info)
{
    soc_control_t *soc;

    soc = SOC_CONTROL(u);
    if (soc->tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (type != NULL) {
        *type = 1;
    }

    if (subtype != NULL) {
        *subtype = ((soc_tcam_info_t *)soc->tcam_info)->subtype;

    }

    if (dc_val != NULL) {
        *dc_val = 1;
    }

    if (tcam_info != NULL) {
        *tcam_info = soc->tcam_info;
    }

    return SOC_E_NONE;
}

/*
 * Partition to tcam memory name mapping
 * TCAM_PARTITION_FWD_L2    EXT_L2_ENTRY_TCAM
 * TCAM_PARTITION_FWD_IP4   EXT_IPV4_DEFIP_TCAM
 * TCAM_PARTITION_FWD_IP6U  EXT_IPV6_64_DEFIP_TCAM
 * TCAM_PARTITION_FWD_IP6   EXT_IPV6_128_DEFIP_TCAM
 * TCAM_PARTITION_ACL_L2    EXT_ACL288_TCAM (part 1)
 * TCAM_PARTITION_ACL_IP4   EXT_ACL288_TCAM (part 2)
 * TCAM_PARTITION_ACL_IP6S  EXT_ACL360_TCAM_DATA, EXT_ACL360_TCAM_MASK
 * TCAM_PARTITION_ACL_IP6F  EXT_ACL432_TCAM_DATA, EXT_ACL432_TCAM_MASK (part 1)
 * TCAM_PARTITION_ACL_L2C   EXT_ACL144_TCAM (part 1)
 * TCAM_PARTITION_ACL_IP4C  EXT_ACL144_TCAM (part 2)
 * TCAM_PARTITION_ACL_IP6C  EXT_ACL144_TCAM (part 3)
 * TCAM_PARTITION_ACL_L2IP4 EXT_ACL432_TCAM_DATA, EXT_ACL432_TCAM_MASK (part 2)
 * TCAM_PARTITION_ACL_L2IP6 EXT_ACL432_TCAM_DATA, EXT_ACL432_TCAM_MASK (part 3)
 *
 * partition to assiciated data memory name mapping
 * TCAM_PARTITION_FWD_L2    EXT_L2_ENTRY_DATA
 * TCAM_PARTITION_FWD_IP4   EXT_DEFIP_DATA (part 1)
 * TCAM_PARTITION_FWD_IP6U  EXT_DEFIP_DATA (part 2)
 * TCAM_PARTITION_FWD_IP6   EXT_DEFIP_DATA (part 3)
 * TCAM_PARTITION_ACL_L2    EXT_FP_POLICY (part 1)
 * TCAM_PARTITION_ACL_IP4   EXT_FP_POLICY (part 2)
 * TCAM_PARTITION_ACL_IP6S  EXT_FP_POLICY (part 3)
 * TCAM_PARTITION_ACL_IP6F  EXT_FP_POLICY (part 4)
 * TCAM_PARTITION_ACL_L2C   EXT_FP_POLICY (part 5)
 * TCAM_PARTITION_ACL_IP4C  EXT_FP_POLICY (part 6)
 * TCAM_PARTITION_ACL_IP6C  EXT_FP_POLICY (part 7)
 * TCAM_PARTITION_ACL_L2IP4 EXT_FP_POLICY (part 8)
 * TCAM_PARTITION_ACL_L2IP6 EXT_FP_POLICY (part 9)
 *
 * Note: if there are multiple partitions in a single memory table, logical
 *       index calculation does not count padded entries. For example if the
 *       system is configured to have 100 ACL_L2 entries and 4096 ACL_IP4
 *       entries. The corresponding logical index for ACL_L2 in EXT_FP_POLICY
 *       is from 0 to 99, and the logical index for ACL_IP4 in EXT_FP_POLICY
 *       is from 100 to 4195 instead of 4096 to 8191, 3996 padded ACL_L2
 *       entries in both TCAM and SRAM are not counted.
 */
STATIC int
soc_triumph_tcam_part_index_to_mem_index(int unit,
                                         soc_tcam_partition_type_t part,
                                         int part_index,
                                         soc_mem_t mem,
                                         int *mem_index)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;

    if (part_index >= partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    switch (part) {
    case TCAM_PARTITION_FWD_L2:
        switch (mem) {
     /* case EXT_L2_ENTRYm: this view does not support read/write operation */
        case EXT_L2_ENTRY_TCAMm:
        case EXT_L2_ENTRY_DATAm:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_SRC_HIT_BITS_L2m:
        case EXT_DST_HIT_BITSm:
        case EXT_DST_HIT_BITS_L2m:
            *mem_index = part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP4:
        switch (mem) {
        case EXT_IPV4_DEFIPm:
        case EXT_IPV4_DEFIP_TCAMm:
        case EXT_DEFIP_DATAm:
        case EXT_DEFIP_DATA_IPV4m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV4m:
        case EXT_DST_HIT_BITS_IPV4m:
            *mem_index = part_index / 32;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6U:
        switch (mem) {
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_64_DEFIP_TCAMm:
        case EXT_DEFIP_DATA_IPV6_64m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV6_64m:
        case EXT_DST_HIT_BITS_IPV6_64m:
            *mem_index = part_index / 32;
            break;
        case EXT_DEFIP_DATAm:
            *mem_index = partitions[TCAM_PARTITION_FWD_IP4].num_entries +
                part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6:
        switch (mem) {
        case EXT_IPV6_128_DEFIPm:
        case EXT_IPV6_128_DEFIP_TCAMm:
        case EXT_DEFIP_DATA_IPV6_128m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV6_128m:
        case EXT_DST_HIT_BITS_IPV6_128m:
            *mem_index = part_index / 32;
            break;
        case EXT_DEFIP_DATAm:
            *mem_index = partitions[TCAM_PARTITION_FWD_IP4].num_entries +
                partitions[TCAM_PARTITION_FWD_IP6U].num_entries +
                part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2:
        switch (mem) {
        case EXT_ACL288_TCAMm:
        case EXT_ACL288_TCAM_L2m:
        case EXT_FP_POLICYm:
        case EXT_FP_POLICY_ACL288_L2m:
        case EXT_FP_CNTRm:
        case EXT_FP_CNTR_ACL288_L2m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8m:
        case EXT_FP_CNTR8_ACL288_L2m:
            *mem_index = part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP4:
        switch (mem) {
        case EXT_ACL288_TCAM_IPV4m:
        case EXT_FP_POLICY_ACL288_IPV4m:
        case EXT_FP_CNTR_ACL288_IPV4m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL288_IPV4m:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL288_TCAMm:
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6S:
        switch (mem) {
        case EXT_ACL360_TCAM_DATAm:
        case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
        case EXT_FP_POLICY_ACL360_IPV6_SHORTm:
        case EXT_FP_CNTR_ACL360_IPV6_SHORTm:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL360_IPV6_SHORTm:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL360_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6F:
        switch (mem) {
        case EXT_ACL432_TCAM_DATAm:
        case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
        case EXT_FP_POLICY_ACL432_IPV6_LONGm:
        case EXT_FP_CNTR_ACL432_IPV6_LONGm:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL432_IPV6_LONGm:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL432_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2C:
        switch (mem) {
        case EXT_ACL144_TCAMm:
        case EXT_ACL144_TCAM_L2m:
        case EXT_FP_POLICY_ACL144_L2m:
        case EXT_FP_CNTR_ACL144_L2m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL144_L2m:
            *mem_index = part_index / 8;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP4C:
        switch (mem) {
        case EXT_ACL144_TCAM_IPV4m:
        case EXT_FP_POLICY_ACL144_IPV4m:
        case EXT_FP_CNTR_ACL144_IPV4m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL144_IPV4m:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL144_TCAMm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                part_index;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6C:
        switch (mem) {
        case EXT_ACL144_TCAM_IPV6m:
        case EXT_FP_POLICY_ACL144_IPV6m:
        case EXT_FP_CNTR_ACL144_IPV6m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL144_IPV6m:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL144_TCAMm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                part_index;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4C].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2IP4:
        switch (mem) {
        case EXT_ACL432_TCAM_DATA_L2_IPV4m:
        case EXT_FP_POLICY_ACL432_L2_IPV4m:
        case EXT_FP_CNTR_ACL432_L2_IPV4m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL432_L2_IPV4m:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL432_TCAM_DATAm:
            *mem_index = partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                part_index;
            break;
        case EXT_ACL432_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6C].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2IP6:
        switch (mem) {
        case EXT_ACL432_TCAM_DATA_L2_IPV6m:
        case EXT_FP_POLICY_ACL432_L2_IPV6m:
        case EXT_FP_CNTR_ACL432_L2_IPV6m:
            *mem_index = part_index;
            break;
        case EXT_FP_CNTR8_ACL432_L2_IPV6m:
            *mem_index = part_index / 8;
            break;
        case EXT_ACL432_TCAM_DATAm:
            *mem_index = partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
                part_index;
            break;
        case EXT_ACL432_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICYm:
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
                partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP6C].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries + 7) / 8 +
                part_index / 8;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Following are a list of memory that may require index translation.
 * These tables are addressed by the raw TCAM index of its corresponding
 * TCAM table.
 *
 * EXT_SRC_HIT_BITS
 * EXT_SRC_HIT_BITS_L2 (software view)
 * EXT_SRC_HIT_BITS_IPV4 (software view)
 * EXT_SRC_HIT_BITS_IPV6_64 (software view)
 * EXT_SRC_HIT_BITS_IPV6_128 (software view)
 *
 * EXT_DST_HIT_BITS
 * EXT_DST_HIT_BITS_L2 (software view)
 * EXT_DST_HIT_BITS_IPV4 (software view)
 * EXT_DST_HIT_BITS_IPV6_64 (software view)
 * EXT_DST_HIT_BITS_IPV6_128 (software view)
 *
 * EXT_L2_ENTRY (TCAM + SRAM + HBIT combine view, no read/write support)
 * EXT_L2_ENTRY_TCAM
 * EXT_L2_ENTRY_DATA
 *
 * EXT_IPV4_DEFIP (TCAM + SRAM + HBIT combine view)
 * EXT_IPV6_64_DEFIP (TCAM + SRAM + HBIT combine view)
 * EXT_IPV6_128_DEFIP (TCAM + SRAM + HBIT combine view)
 *
 * EXT_IPV4_DEFIP_TCAM
 * EXT_IPV6_64_DEFIP_TCAM
 * EXT_IPV6_128_DEFIP_TCAM
 *
 * EXT_DEFIP_DATA
 * EXT_DEFIP_DATA_IPV4 (software view)
 * EXT_DEFIP_DATA_IPV6_64 (software view)
 * EXT_DEFIP_DATA_IPV6_128 (software view)
 *
 * EXT_ACL144_TCAM
 * EXT_ACL288_TCAM
 * EXT_ACL360_TCAM_DATA
 * EXT_ACL360_TCAM_MASK (has only 1 entry)
 * EXT_ACL432_TCAM_DATA
 * EXT_ACL432_TCAM_MASK (has only 1 entry)
 *
 * EXT_ACL288_TCAM_L2 (software view)
 * EXT_ACL288_TCAM_IPV4 (software view)
 * EXT_ACL360_TCAM_DATA_IPV6_SHORT (software view)
 * EXT_ACL432_TCAM_DATA_IPV6_LONG (software view)
 * EXT_ACL144_TCAM_L2 (software view)
 * EXT_ACL144_TCAM_IPV4 (software view)
 * EXT_ACL144_TCAM_IPV6 (software view)
 * EXT_ACL432_TCAM_DATA_L2_IPV4 (software view)
 * EXT_ACL432_TCAM_DATA_L2_IPV6 (software view)
 *
 * EXT_FP_POLICY
 * EXT_FP_POLICY_ACL288_L2 (software view)
 * EXT_FP_POLICY_ACL288_IPV4 (software view)
 * EXT_FP_POLICY_ACL360_IPV6_SHORT (software view)
 * EXT_FP_POLICY_ACL432_IPV6_LONG (software view)
 * EXT_FP_POLICY_ACL144_L2 (software view)
 * EXT_FP_POLICY_ACL144_IPV4 (software view)
 * EXT_FP_POLICY_ACL144_IPV6 (software view)
 * EXT_FP_POLICY_ACL432_L2_IPV4 (software view)
 * EXT_FP_POLICY_ACL432_L2_IPV6 (software view)
 *
 * EXT_FP_CNTR
 * EXT_FP_CNTR_ACL288_L2 (software view)
 * EXT_FP_CNTR_ACL288_IPV4 (software view)
 * EXT_FP_CNTR_ACL360_IPV6_SHORT (software view)
 * EXT_FP_CNTR_ACL432_IPV6_LONG (software view)
 * EXT_FP_CNTR_ACL144_L2 (software view)
 * EXT_FP_CNTR_ACL144_IPV4 (software view)
 * EXT_FP_CNTR_ACL144_IPV6 (software view)
 * EXT_FP_CNTR_ACL432_L2_IPV4 (software view)
 * EXT_FP_CNTR_ACL432_L2_IPV6 (software view)
 *
 * EXT_FP_CNTR8
 * EXT_FP_CNTR8_ACL288_L2 (software view)
 * EXT_FP_CNTR8_ACL288_IPV4 (software view)
 * EXT_FP_CNTR8_ACL360_IPV6_SHORT (software view)
 * EXT_FP_CNTR8_ACL432_IPV6_LONG (software view)
 * EXT_FP_CNTR8_ACL144_L2 (software view)
 * EXT_FP_CNTR8_ACL144_IPV4 (software view)
 * EXT_FP_CNTR8_ACL144_IPV6 (software view)
 * EXT_FP_CNTR8_ACL432_L2_IPV4 (software view)
 * EXT_FP_CNTR8_ACL432_L2_IPV6 (software view)
 */

STATIC int
soc_triumph_tcam_mem_index_to_raw_index(int unit, soc_mem_t mem,
                                        int mem_index, soc_mem_t *real_mem,
                                        int *raw_index)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int part;
    int entries_per_index;

    /* convert the software invented table to real table */
    switch (mem) {
    case EXT_DEFIP_DATA_IPV4m:
    case EXT_DEFIP_DATA_IPV6_64m:
    case EXT_DEFIP_DATA_IPV6_128m:
        *real_mem = EXT_DEFIP_DATAm;
        break;
    case EXT_SRC_HIT_BITS_L2m:
    case EXT_SRC_HIT_BITS_IPV4m:
    case EXT_SRC_HIT_BITS_IPV6_64m:
    case EXT_SRC_HIT_BITS_IPV6_128m:
        *real_mem = EXT_SRC_HIT_BITSm;
        break;
    case EXT_DST_HIT_BITS_L2m:
    case EXT_DST_HIT_BITS_IPV4m:
    case EXT_DST_HIT_BITS_IPV6_64m:
    case EXT_DST_HIT_BITS_IPV6_128m:
        *real_mem = EXT_DST_HIT_BITSm;
        break;
    case EXT_ACL144_TCAM_L2m:
    case EXT_ACL144_TCAM_IPV4m:
    case EXT_ACL144_TCAM_IPV6m:
        *real_mem = EXT_ACL144_TCAMm;
        break;
    case EXT_ACL288_TCAM_L2m:
    case EXT_ACL288_TCAM_IPV4m:
        *real_mem = EXT_ACL288_TCAMm;
        break;
    case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
        *real_mem = EXT_ACL360_TCAM_DATAm;
        break;
    case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
    case EXT_ACL432_TCAM_DATA_L2_IPV4m:
    case EXT_ACL432_TCAM_DATA_L2_IPV6m:
        *real_mem = EXT_ACL432_TCAM_DATAm;
        break;
    case EXT_FP_POLICY_ACL288_L2m:
    case EXT_FP_POLICY_ACL288_IPV4m:
    case EXT_FP_POLICY_ACL360_IPV6_SHORTm:
    case EXT_FP_POLICY_ACL432_IPV6_LONGm:
    case EXT_FP_POLICY_ACL144_L2m:
    case EXT_FP_POLICY_ACL144_IPV4m:
    case EXT_FP_POLICY_ACL144_IPV6m:
    case EXT_FP_POLICY_ACL432_L2_IPV4m:
    case EXT_FP_POLICY_ACL432_L2_IPV6m:
        *real_mem = EXT_FP_POLICYm;
        break;
    case EXT_FP_CNTR_ACL288_L2m:
    case EXT_FP_CNTR_ACL288_IPV4m:
    case EXT_FP_CNTR_ACL360_IPV6_SHORTm:
    case EXT_FP_CNTR_ACL432_IPV6_LONGm:
    case EXT_FP_CNTR_ACL144_L2m:
    case EXT_FP_CNTR_ACL144_IPV4m:
    case EXT_FP_CNTR_ACL144_IPV6m:
    case EXT_FP_CNTR_ACL432_L2_IPV4m:
    case EXT_FP_CNTR_ACL432_L2_IPV6m:
        *real_mem = EXT_FP_CNTRm;
        break;
    case EXT_FP_CNTR8_ACL288_L2m:
    case EXT_FP_CNTR8_ACL288_IPV4m:
    case EXT_FP_CNTR8_ACL360_IPV6_SHORTm:
    case EXT_FP_CNTR8_ACL432_IPV6_LONGm:
    case EXT_FP_CNTR8_ACL144_L2m:
    case EXT_FP_CNTR8_ACL144_IPV4m:
    case EXT_FP_CNTR8_ACL144_IPV6m:
    case EXT_FP_CNTR8_ACL432_L2_IPV4m:
    case EXT_FP_CNTR8_ACL432_L2_IPV6m:
        *real_mem = EXT_FP_CNTR8m;
        break;
    case EXT_L2_ENTRYm:
    case EXT_L2_ENTRY_TCAMm:
    case EXT_L2_ENTRY_DATAm:
    case EXT_IPV4_DEFIPm:
    case EXT_IPV4_DEFIP_TCAMm:
    case EXT_IPV6_64_DEFIPm:
    case EXT_IPV6_64_DEFIP_TCAMm:
    case EXT_IPV6_64_TCAMm:
    case EXT_IPV6_128_DEFIPm:
    case EXT_IPV6_128_DEFIP_TCAMm:
    case EXT_DEFIP_DATAm:
    case EXT_SRC_HIT_BITSm:
    case EXT_DST_HIT_BITSm:
    case EXT_ACL144_TCAMm:
    case EXT_ACL288_TCAMm:
    case EXT_ACL360_TCAM_DATAm:
    case EXT_ACL432_TCAM_DATAm:
    case EXT_FP_POLICYm:
    case EXT_FP_CNTRm:
    case EXT_FP_CNTR8m:
        *real_mem = mem;
        break;
    default:
        *real_mem = mem;
        *raw_index = mem_index;
        return SOC_E_NONE;
    }

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;
    part = TCAM_PARTITION_RAW;
    entries_per_index = 1;

    if (*real_mem == EXT_SRC_HIT_BITSm || *real_mem == EXT_DST_HIT_BITSm) {
        entries_per_index = 32;
    } else if (*real_mem == EXT_FP_CNTR8m) {
        entries_per_index = 8;
    }

    /* find partition and calculate logical index within partition */
    switch (mem) {
    case EXT_L2_ENTRYm:
    case EXT_L2_ENTRY_TCAMm:
    case EXT_L2_ENTRY_DATAm:
    case EXT_SRC_HIT_BITS_L2m:
    case EXT_DST_HIT_BITS_L2m:
        part = TCAM_PARTITION_FWD_L2;
        break;
    case EXT_IPV4_DEFIPm:
    case EXT_IPV4_DEFIP_TCAMm:
    case EXT_DEFIP_DATA_IPV4m:
    case EXT_SRC_HIT_BITS_IPV4m:
    case EXT_DST_HIT_BITS_IPV4m:
        part = TCAM_PARTITION_FWD_IP4;
        break;
    case EXT_IPV6_64_DEFIPm:
    case EXT_IPV6_64_DEFIP_TCAMm:
    case EXT_DEFIP_DATA_IPV6_64m:
    case EXT_SRC_HIT_BITS_IPV6_64m:
    case EXT_DST_HIT_BITS_IPV6_64m:
        part = TCAM_PARTITION_FWD_IP6U;
        break;
    case EXT_IPV6_128_DEFIPm:
    case EXT_IPV6_128_DEFIP_TCAMm:
    case EXT_DEFIP_DATA_IPV6_128m:
    case EXT_SRC_HIT_BITS_IPV6_128m:
    case EXT_DST_HIT_BITS_IPV6_128m:
        part = TCAM_PARTITION_FWD_IP6;
        break;
    case EXT_DEFIP_DATAm:
        for (part = TCAM_PARTITION_FWD_IP4; part < TCAM_PARTITION_FWD_IP6;
             part++) {
            if (mem_index < partitions[part].num_entries) {
                break;
            }
            mem_index -= partitions[part].num_entries;
        }
        break;
    case EXT_SRC_HIT_BITSm:
    case EXT_DST_HIT_BITSm:
        for (part = TCAM_PARTITION_FWD_L2; part < TCAM_PARTITION_FWD_IP6;
             part++) {
            if (mem_index < (partitions[part].num_entries + 31) / 32) {
                break;
            }
            mem_index -= (partitions[part].num_entries + 31) / 32;
        }
        break;
    case EXT_ACL144_TCAMm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
            part = TCAM_PARTITION_ACL_L2C;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2C].num_entries;
        if (mem_index < partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
            part = TCAM_PARTITION_ACL_IP4C;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_IP4C].num_entries;
        part = TCAM_PARTITION_ACL_IP6C;
        break;
    case EXT_ACL288_TCAMm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            part = TCAM_PARTITION_ACL_L2;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2].num_entries;
        part = TCAM_PARTITION_ACL_IP4;
        break;
    case EXT_ACL360_TCAM_DATAm:
        part = TCAM_PARTITION_ACL_IP6S;
        break;
    case EXT_ACL432_TCAM_DATAm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
            part = TCAM_PARTITION_ACL_IP6F;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_IP6F].num_entries;
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
            part = TCAM_PARTITION_ACL_L2IP4;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2IP4].num_entries;
        part = TCAM_PARTITION_ACL_L2IP6;
        break;
    case EXT_FP_POLICYm:
    case EXT_FP_CNTRm:
        for (part = TCAM_PARTITION_ACL_L2; part < TCAM_PARTITION_ACL_L2IP6;
             part++) {
            if (mem_index < partitions[part].num_entries) {
                break;
            }
            mem_index -= partitions[part].num_entries;
        }
        break;
    case EXT_FP_CNTR8m:
        for (part = TCAM_PARTITION_ACL_L2; part < TCAM_PARTITION_ACL_L2IP6;
             part++) {
            if (mem_index < (partitions[part].num_entries + 7) / 8) {
                break;
            }
            mem_index -= (partitions[part].num_entries + 7) / 8;
        }
        break;
    case EXT_ACL288_TCAM_L2m:
    case EXT_FP_POLICY_ACL288_L2m:
    case EXT_FP_CNTR_ACL288_L2m:
    case EXT_FP_CNTR8_ACL288_L2m:
        part = TCAM_PARTITION_ACL_L2;
        break;
    case EXT_ACL288_TCAM_IPV4m:
    case EXT_FP_POLICY_ACL288_IPV4m:
    case EXT_FP_CNTR_ACL288_IPV4m:
    case EXT_FP_CNTR8_ACL288_IPV4m:
        part = TCAM_PARTITION_ACL_IP4;
        break;
    case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORTm:
    case EXT_FP_CNTR_ACL360_IPV6_SHORTm:
    case EXT_FP_CNTR8_ACL360_IPV6_SHORTm:
        part = TCAM_PARTITION_ACL_IP6S;
        break;
    case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
    case EXT_FP_POLICY_ACL432_IPV6_LONGm:
    case EXT_FP_CNTR_ACL432_IPV6_LONGm:
    case EXT_FP_CNTR8_ACL432_IPV6_LONGm:
        part = TCAM_PARTITION_ACL_IP6F;
        break;
    case EXT_ACL144_TCAM_L2m:
    case EXT_FP_POLICY_ACL144_L2m:
    case EXT_FP_CNTR_ACL144_L2m:
    case EXT_FP_CNTR8_ACL144_L2m:
        part = TCAM_PARTITION_ACL_L2C;
        break;
    case EXT_ACL144_TCAM_IPV4m:
    case EXT_FP_POLICY_ACL144_IPV4m:
    case EXT_FP_CNTR_ACL144_IPV4m:
    case EXT_FP_CNTR8_ACL144_IPV4m:
        part = TCAM_PARTITION_ACL_IP4C;
        break;
    case EXT_ACL144_TCAM_IPV6m:
    case EXT_FP_POLICY_ACL144_IPV6m:
    case EXT_FP_CNTR_ACL144_IPV6m:
    case EXT_FP_CNTR8_ACL144_IPV6m:
        part = TCAM_PARTITION_ACL_IP6C;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV4m:
    case EXT_FP_POLICY_ACL432_L2_IPV4m:
    case EXT_FP_CNTR_ACL432_L2_IPV4m:
    case EXT_FP_CNTR8_ACL432_L2_IPV4m:
        part = TCAM_PARTITION_ACL_L2IP4;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV6m:
    case EXT_FP_POLICY_ACL432_L2_IPV6m:
    case EXT_FP_CNTR_ACL432_L2_IPV6m:
    case EXT_FP_CNTR8_ACL432_L2_IPV6m:
        part = TCAM_PARTITION_ACL_L2IP6;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    *raw_index = partitions[part].tcam_base +
        (mem_index * entries_per_index << partitions[part].tcam_width_shift);

    return SOC_E_NONE;
}
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
STATIC int
soc_triumph3_tcam_part_index_to_mem_index(int unit,
                                         soc_tcam_partition_type_t part,
                                         int part_index,
                                         soc_mem_t mem,
                                         int *mem_index)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;

    if (part_index >= partitions[part].num_entries) {
        return SOC_E_PARAM;
    }




    switch (part) {
    case TCAM_PARTITION_FWD_L2:
        switch (mem) {
        case EXT_L2_ENTRY_TCAMm:
        case EXT_L2_ENTRY_DATAm:
        case EXT_L2_ENTRY_DATA_ONLYm:
        case EXT_L2_ENTRY_DATA_ONLY_WIDEm:
        case EXT_L2_ENTRY_1m:
        case EXT_L2_ENTRY_2m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_SRC_HIT_BITS_L2m:
        case EXT_DST_HIT_BITSm:
        case EXT_DST_HIT_BITS_L2m:
        case EXT_LOC_SRC_HBITSm:
#ifdef sw_view
        case EXT_LOC_SRC_HIT_BITS_L2m:
#endif
            *mem_index = part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
        /* for now all IP entries share the same partition */
    case TCAM_PARTITION_FWD_IP4:
        switch (mem) {
        case EXT_IPV4_DEFIPm:
#if 0 /* not there for tr3 */
        case EXT_IPV4_DEFIP_TCAMm:
#endif
        case EXT_DEFIP_DATAm:
        case EXT_DEFIP_DATA_IPV4m:
        /* need aliases for V4 and V6es here */
        case EXT_L3_UCAST_DATAm:
        case EXT_L3_UCAST_DATA_WIDEm:
        case EXT_IPV4_TCAMm:
        case EXT_IPV4_UCASTm:
        case EXT_IPV4_UCAST_WIDEm:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV4m:
        case EXT_DST_HIT_BITS_IPV4m:
            *mem_index = part_index / 32;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6U:
        switch (mem) {
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_64_TCAMm:
        case EXT_DEFIP_DATA_IPV6_64m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV6_64m:
        case EXT_DST_HIT_BITS_IPV6_64m:
            *mem_index = part_index / 32;
            break;
        case EXT_DEFIP_DATAm:
            *mem_index = partitions[TCAM_PARTITION_FWD_IP4].num_entries +
                part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6:
        switch (mem) {
        case EXT_IPV6_128_DEFIPm:
#if 0
        case EXT_IPV6_128_DEFIP_TCAMm:
#endif
        case EXT_IPV6_128_TCAMm:
        case EXT_DEFIP_DATA_IPV6_128m:
            *mem_index = part_index;
            break;
        case EXT_SRC_HIT_BITS_IPV6_128m:
        case EXT_DST_HIT_BITS_IPV6_128m:
            *mem_index = part_index / 32;
            break;
        case EXT_DEFIP_DATAm:
        case EXT_L3_UCAST_DATAm:
        case EXT_L3_UCAST_DATA_WIDEm:
        case EXT_IPV6_128_UCASTm:
        case EXT_IPV6_128_UCAST_WIDEm:
            *mem_index = partitions[TCAM_PARTITION_FWD_IP4].num_entries +
                partitions[TCAM_PARTITION_FWD_IP6U].num_entries +
                part_index;
            break;
        case EXT_SRC_HIT_BITSm:
        case EXT_DST_HIT_BITSm:
            *mem_index =
                (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
                (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 +
                part_index / 32;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2:
        switch (mem) {
        case EXT_ACL320_TCAMm:
#ifdef sw_view
        case EXT_ACL320_TCAM_L2m:
#endif
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
#ifdef sw_view
        case EXT_FP_POLICY_ACL320_L2m:
#endif
            *mem_index = part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP4:
        switch (mem) {
#ifdef sw_view
        case EXT_ACL320_TCAM_IPV4m:
        case EXT_FP_POLICY_ACL320_IPV4m:
            *mem_index = part_index;
            break;
#endif
        case EXT_ACL320_TCAMm:
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
#if 0
        case EXT_FP_CNTRm:
#endif
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                part_index;
            break;
#if 0
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                part_index / 8;
            break;




#endif
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6S:
        switch (mem) {
        case EXT_ACL480_TCAM_DATAm:
#ifdef sw_view
        case EXT_ACL480_TCAM_DATA_IPV6_SHORTm:
        case EXT_FP_POLICY_ACL480_IPV6_SHORTm:
#endif
            *mem_index = part_index;
            break;
        case EXT_ACL480_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
#if 0
        case EXT_FP_CNTRm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                part_index;
            break;
        case EXT_FP_CNTR8m:
            *mem_index =
                (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
                (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
                part_index / 8;
            break;
#endif
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6F:
        switch (mem) {
        case EXT_ACL480_TCAM_DATAm:
#ifdef sw_view
        case EXT_ACL480_TCAM_DATA_IPV6_LONGm:
        case EXT_FP_POLICY_ACL480_IPV6_LONGm:
#endif
            *mem_index = part_index;
            break;
        case EXT_ACL480_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
        
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2C:
        switch (mem) {
        case EXT_ACL160_TCAMm:
#ifdef sw_view
        case EXT_ACL160_TCAM_L2m:
        case EXT_FP_POLICY_ACL160_L2m:
        case EXT_FP_CNTR_ACL160_L2m:
#endif
            *mem_index = part_index;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP4C:
        switch (mem) {
#ifdef sw_view
        case EXT_ACL160_TCAM_IPV4m:
        case EXT_FP_POLICY_ACL160_IPV4m:
            *mem_index = part_index;
            break;
#endif
        case EXT_ACL160_TCAMm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                part_index;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_IP6C:
        switch (mem) {
#ifdef sw_view
        case EXT_ACL160_TCAM_IPV6m:
        case EXT_FP_POLICY_ACL160_IPV6m:
            *mem_index = part_index;
            break;
#endif
        case EXT_ACL160_TCAMm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                part_index;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2IP4:
        switch (mem) {
#ifdef sw_view
        case EXT_ACL480_TCAM_DATA_L2_IPV4m:
        case EXT_FP_POLICY_ACL480_L2_IPV4m:
            *mem_index = part_index;
            break;
#endif
        case EXT_ACL480_TCAM_DATAm:
            *mem_index = partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                part_index;
            break;
        case EXT_ACL480_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_ACL_L2IP6:
        switch (mem) {
#ifdef sw_view
        case EXT_ACL480_TCAM_DATA_L2_IPV6m:
        case EXT_FP_POLICY_ACL432_L2_IPV6m:
            *mem_index = part_index;
            break;
#endif
        case EXT_ACL480_TCAM_DATAm:
            *mem_index = partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
                part_index;
            break;
        case EXT_ACL480_TCAM_MASKm:
            *mem_index = 0;
            break;
        case EXT_FP_POLICY_1Xm:
        case EXT_FP_POLICY_2Xm:
        case EXT_FP_POLICY_3Xm:
        case EXT_FP_POLICY_4Xm:
        case EXT_FP_POLICY_6Xm:
            *mem_index = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
                partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
                part_index;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

#define QT 1

STATIC int
soc_triumph3_tcam_mem_index_to_raw_index(int unit, soc_mem_t mem,
                                        int mem_index, soc_mem_t *real_mem,
                                        int *raw_index)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int part;
    int entries_per_index=0;
    int tcam_base_override=0;

    /* convert the software invented table to real table */
    switch (mem) {
    case EXT_L2_ENTRY_TCAM_WIDEm:
        *real_mem = EXT_L2_ENTRY_TCAMm;
        break;
    case EXT_DEFIP_DATA_IPV4m:
    case EXT_DEFIP_DATA_IPV6_64m:
    case EXT_DEFIP_DATA_IPV6_128m:
        *real_mem = EXT_DEFIP_DATAm;
        break;
    case EXT_L3_UCAST_DATA_IPV4m:
    case EXT_L3_UCAST_DATA_IPV6_128m:
        *real_mem = EXT_L3_UCAST_DATAm;
        break;
    case EXT_L3_UCAST_DATA_WIDE_IPV4m:
    case EXT_L3_UCAST_DATA_WIDE_IPV6_128m:
        *real_mem = EXT_L3_UCAST_DATA_WIDEm;
        break;
    case EXT_SRC_HIT_BITS_IPV6_128_UCASTm:
    case EXT_SRC_HIT_BITS_IPV6_128_UCAST_WIDEm:
        /* coverity[unterminated_case] */
    case EXT_SRC_HIT_BITS_IPV6_128m:
        entries_per_index = 64;
    case EXT_SRC_HIT_BITS_L2m:
    case EXT_SRC_HIT_BITS_L2_WIDEm:
    case EXT_SRC_HIT_BITS_IPV4_UCASTm:
    case EXT_SRC_HIT_BITS_IPV4_UCAST_WIDEm:
    case EXT_SRC_HIT_BITS_IPV4m:
    case EXT_SRC_HIT_BITS_IPV6_64m:
        *real_mem = EXT_SRC_HBITSm;
        break;
    case EXT_DST_HIT_BITS_IPV6_128_UCASTm:
    case EXT_DST_HIT_BITS_IPV6_128_UCAST_WIDEm:
        /* coverity[unterminated_case] */
    case EXT_DST_HIT_BITS_IPV6_128m:
        entries_per_index = 64;
    case EXT_DST_HIT_BITS_L2m:
    case EXT_DST_HIT_BITS_L2_WIDEm:
    case EXT_DST_HIT_BITS_IPV4_UCASTm:
    case EXT_DST_HIT_BITS_IPV4_UCAST_WIDEm:
    case EXT_DST_HIT_BITS_IPV4m:
    case EXT_DST_HIT_BITS_IPV6_64m:
        *real_mem = EXT_DST_HBITSm;
        break;
    case EXT_LOC_SRC_HIT_BITS_L2m:
    case EXT_LOC_SRC_HIT_BITS_L2_WIDEm:
        *real_mem = EXT_LOC_SRC_HBITSm;
        break;
    case EXT_ACL144_TCAM_L2m:
    case EXT_ACL144_TCAM_IPV4m:
    case EXT_ACL144_TCAM_IPV6m:
        *real_mem = EXT_ACL160_TCAMm;
        break;
    case EXT_ACL288_TCAM_L2m:
    case EXT_ACL288_TCAM_IPV4m:
    case EXT_ACL432_TCAM_DATA_L2_IPV4m:
        *real_mem = EXT_ACL320_TCAMm;
        break;
    case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
    case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
    case EXT_ACL432_TCAM_DATA_L2_IPV6m:
        *real_mem = EXT_ACL480_TCAM_DATAm;
        break;
    case EXT_FP_POLICY_ACL288_L2_1Xm:
    case EXT_FP_POLICY_ACL288_IPV4_1Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_1Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_1Xm:
    case EXT_FP_POLICY_ACL144_L2_1Xm:
    case EXT_FP_POLICY_ACL144_IPV4_1Xm:
    case EXT_FP_POLICY_ACL144_IPV6_1Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_1Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_1Xm:
    case EXT_ACL480_FP_POLICY_1Xm:
    case EXT_ACL320_FP_POLICY_1Xm:
    case EXT_ACL160_FP_POLICY_1Xm:
    case EXT_ACL80_FP_POLICY_1Xm:
        *real_mem = EXT_FP_POLICY_1Xm;
        break;
    case EXT_FP_POLICY_ACL288_L2_2Xm:
    case EXT_FP_POLICY_ACL288_IPV4_2Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_2Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_2Xm:
    case EXT_FP_POLICY_ACL144_L2_2Xm:
    case EXT_FP_POLICY_ACL144_IPV4_2Xm:
    case EXT_FP_POLICY_ACL144_IPV6_2Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_2Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_2Xm:
    case EXT_ACL480_FP_POLICY_2Xm:
    case EXT_ACL320_FP_POLICY_2Xm:
    case EXT_ACL160_FP_POLICY_2Xm:
    case EXT_ACL80_FP_POLICY_2Xm:
        *real_mem = EXT_FP_POLICY_2Xm;
        break;
    case EXT_FP_POLICY_ACL288_L2_3Xm:
    case EXT_FP_POLICY_ACL288_IPV4_3Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_3Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_3Xm:
    case EXT_FP_POLICY_ACL144_L2_3Xm:
    case EXT_FP_POLICY_ACL144_IPV4_3Xm:
    case EXT_FP_POLICY_ACL144_IPV6_3Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_3Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_3Xm:
    case EXT_ACL480_FP_POLICY_3Xm:
    case EXT_ACL320_FP_POLICY_3Xm:
    case EXT_ACL160_FP_POLICY_3Xm:
    case EXT_ACL80_FP_POLICY_3Xm:
        *real_mem = EXT_FP_POLICY_3Xm;
        break;
    case EXT_FP_POLICY_ACL288_L2_4Xm:
    case EXT_FP_POLICY_ACL288_IPV4_4Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_4Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_4Xm:
    case EXT_FP_POLICY_ACL144_L2_4Xm:
    case EXT_FP_POLICY_ACL144_IPV4_4Xm:
    case EXT_FP_POLICY_ACL144_IPV6_4Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_4Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_4Xm:
    case EXT_ACL480_FP_POLICY_4Xm:
    case EXT_ACL320_FP_POLICY_4Xm:
    case EXT_ACL160_FP_POLICY_4Xm:
    case EXT_ACL80_FP_POLICY_4Xm:
        *real_mem = EXT_FP_POLICY_4Xm;
        break;
    case EXT_FP_POLICY_ACL288_L2_6Xm:
    case EXT_FP_POLICY_ACL288_IPV4_6Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_6Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_6Xm:
    case EXT_FP_POLICY_ACL144_L2_6Xm:
    case EXT_FP_POLICY_ACL144_IPV4_6Xm:
    case EXT_FP_POLICY_ACL144_IPV6_6Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_6Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_6Xm:
    case EXT_ACL480_FP_POLICY_6Xm:
    case EXT_ACL320_FP_POLICY_6Xm:
    case EXT_ACL160_FP_POLICY_6Xm:
    case EXT_ACL80_FP_POLICY_6Xm:
        *real_mem = EXT_FP_POLICY_6Xm;
        break;
    case EXT_TCAM_VBIT_L2_ENTRY_1m:
    case EXT_TCAM_VBIT_L2_ENTRY_2m:
    case EXT_TCAM_VBIT_DEFIP_IPV4m:
    case EXT_TCAM_VBIT_DEFIP_IPV6_64m:
    case EXT_TCAM_VBIT_DEFIP_IPV6_128m:
    case EXT_TCAM_VBIT_IPV4_UCASTm:
    case EXT_TCAM_VBIT_IPV4_UCAST_WIDEm:
    case EXT_TCAM_VBIT_IPV6_128_UCASTm:
    case EXT_TCAM_VBIT_IPV6_128_UCAST_WIDEm:
    case EXT_TCAM_VBIT_ACL480m:
    case EXT_TCAM_VBIT_ACL320m:
    case EXT_TCAM_VBIT_ACL160m:
    case EXT_TCAM_VBIT_ACL80m:
        *real_mem = EXT_TCAM_VBITm;
        break;
    case EXT_IPV4_DEFIP_TCAMm:
    case EXT_IPV4_UCAST_TCAMm:
    case EXT_IPV4_UCAST_WIDE_TCAMm:
        *real_mem = EXT_IPV4_TCAMm;
        break;
    case EXT_IPV6_64_DEFIP_TCAMm:
        *real_mem = EXT_IPV6_64_TCAMm;
        break;
    case EXT_IPV6_128_DEFIP_TCAMm:
    case EXT_IPV6_128_UCAST_TCAMm:
    case EXT_IPV6_128_UCAST_WIDE_TCAMm:
        *real_mem = EXT_IPV6_128_TCAMm;
        break;

    case EXT_L2_ENTRY_TCAMm:
    case EXT_L2_ENTRY_DATA_ONLYm:
    case EXT_L2_ENTRY_DATA_ONLY_WIDEm:
    case EXT_L2_ENTRY_1m:
    case EXT_L2_ENTRY_2m:
    case EXT_IPV4_DEFIPm:
    case EXT_IPV4_TCAMm:
    case EXT_IPV4_UCASTm:
    case EXT_IPV4_UCAST_WIDEm:
    case EXT_IPV6_64_DEFIPm:
    case EXT_IPV6_64_TCAMm:
    case EXT_IPV6_128_DEFIPm:
    case EXT_IPV6_128_UCASTm:
    case EXT_IPV6_128_UCAST_WIDEm:
    case EXT_IPV6_128_TCAMm:
    case EXT_DEFIP_DATAm:
    case EXT_L3_UCAST_DATAm:
    case EXT_L3_UCAST_DATA_WIDEm:
    case EXT_SRC_HBITSm:
    case EXT_DST_HBITSm:
    case EXT_LOC_SRC_HBITSm:
    case EXT_ACL80_TCAMm:
    case EXT_ACL160_TCAMm:
    case EXT_ACL320_TCAMm:
    case EXT_ACL480_TCAM_MASKm:
    case EXT_ACL480_TCAM_DATAm:
    case EXT_FP_POLICY_1Xm:
    case EXT_FP_POLICY_2Xm:
    case EXT_FP_POLICY_3Xm:
    case EXT_FP_POLICY_4Xm:
    case EXT_FP_POLICY_6Xm:

        *real_mem = mem;
        break;

    case EXT_ACL432_TCAM_MASKm:
    case EXT_ACL360_TCAM_MASKm:
    case EXT_ACL360_TCAM_MASK_IPV6_SHORTm:
    case EXT_ACL432_TCAM_MASK_IPV6_LONGm:
    case EXT_ACL432_TCAM_MASK_L2_IPV6m:
        *real_mem = EXT_ACL480_TCAM_MASKm;
        *raw_index = 0;
        return SOC_E_NONE;

    default:
        *real_mem = mem;
        *raw_index = mem_index;
        return SOC_E_NONE;
    }

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        /* special case for debug:*/
        if (mem == *real_mem) {
            return SOC_E_NONE;
        }
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;
    part = TCAM_PARTITION_RAW;
    if (entries_per_index == 0) {
        entries_per_index = 1;

        switch (*real_mem) {
        case EXT_SRC_HBITSm:
        case EXT_DST_HBITSm:
        case EXT_LOC_SRC_HBITSm:
        case EXT_TCAM_VBITm:
        case EXT_TCAM_VBIT_L2_ENTRY_1m:
        case EXT_TCAM_VBIT_L2_ENTRY_2m:
        case EXT_TCAM_VBIT_IPV4_UCASTm:
        case EXT_TCAM_VBIT_IPV4_UCAST_WIDEm:
        case EXT_TCAM_VBIT_DEFIP_IPV6_64m:
        case EXT_TCAM_VBIT_ACL80m:
            entries_per_index = 32;
            break;
        case EXT_TCAM_VBIT_IPV6_128_UCASTm:
        case EXT_TCAM_VBIT_IPV6_128_UCAST_WIDEm:
        case EXT_TCAM_VBIT_DEFIP_IPV6_128m:
        case EXT_TCAM_VBIT_ACL160m:
            entries_per_index = 64;
            break;
        case EXT_TCAM_VBIT_ACL320m:
            entries_per_index = 128;
            break;
        case EXT_TCAM_VBIT_ACL480m:
            entries_per_index = 256;
            break;
        default:
            break;
        }
    }

    /* find partition and calculate logical index within partition */
    switch (mem) {
    case EXT_IPV4_UCASTm:
    case EXT_IPV4_UCAST_TCAMm:
    case EXT_L3_UCAST_DATA_IPV4m:
    case EXT_SRC_HIT_BITS_IPV4_UCASTm:
    case EXT_DST_HIT_BITS_IPV4_UCASTm:
    case EXT_TCAM_VBIT_IPV4_UCASTm:
        part = TCAM_PARTITION_FWD_IP4_UCAST;
        break;
    case EXT_IPV4_UCAST_WIDEm:
    case EXT_IPV4_UCAST_WIDE_TCAMm:
    case EXT_L3_UCAST_DATA_WIDE_IPV4m:
    case EXT_SRC_HIT_BITS_IPV4_UCAST_WIDEm:
    case EXT_DST_HIT_BITS_IPV4_UCAST_WIDEm:
    case EXT_TCAM_VBIT_IPV4_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
        break;
    case EXT_IPV4_DEFIPm:
    case EXT_IPV4_DEFIP_TCAMm:
        part = TCAM_PARTITION_FWD_IP4;
        break;
    case EXT_IPV6_128_UCASTm:
    case EXT_IPV6_128_UCAST_TCAMm:
    case EXT_L3_UCAST_DATA_IPV6_128m:
    case EXT_SRC_HIT_BITS_IPV6_128_UCASTm:
    case EXT_DST_HIT_BITS_IPV6_128_UCASTm:
    case EXT_TCAM_VBIT_IPV6_128_UCASTm:
        part = TCAM_PARTITION_FWD_IP6_128_UCAST;
        break;
    case EXT_IPV6_128_UCAST_WIDEm:
    case EXT_IPV6_128_UCAST_WIDE_TCAMm:
    case EXT_L3_UCAST_DATA_WIDE_IPV6_128m:
    case EXT_SRC_HIT_BITS_IPV6_128_UCAST_WIDEm:
    case EXT_DST_HIT_BITS_IPV6_128_UCAST_WIDEm:
    case EXT_TCAM_VBIT_IPV6_128_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
        break;
    case EXT_L3_UCAST_DATAm:
        part = TCAM_PARTITION_FWD_IP4_UCAST;
        if (mem_index < partitions[part].num_entries) {
            break;
        }
        mem_index -= partitions[part].num_entries;
        part = TCAM_PARTITION_FWD_IP6_128_UCAST;
        break;
    case EXT_L3_UCAST_DATA_WIDEm:
        part = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
        if (mem_index < partitions[part].num_entries) {
            break;
        }
        mem_index -= partitions[part].num_entries;
        part = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
        break;
    case EXT_L2_ENTRY_1m:
    case EXT_L2_ENTRY_TCAMm:
    case EXT_L2_ENTRY_DATA_ONLYm:
    case EXT_SRC_HIT_BITS_L2m:
    case EXT_DST_HIT_BITS_L2m:
    case EXT_LOC_SRC_HIT_BITS_L2m:
    case EXT_TCAM_VBIT_L2_ENTRY_1m:
        part = TCAM_PARTITION_FWD_L2;
        break;
    case EXT_L2_ENTRY_2m:
    case EXT_L2_ENTRY_TCAM_WIDEm:
    case EXT_L2_ENTRY_DATA_ONLY_WIDEm:
    case EXT_SRC_HIT_BITS_L2_WIDEm:
    case EXT_DST_HIT_BITS_L2_WIDEm:
    case EXT_LOC_SRC_HIT_BITS_L2_WIDEm:
    case EXT_TCAM_VBIT_L2_ENTRY_2m:
        part = TCAM_PARTITION_FWD_L2_WIDE;
        break;
    case EXT_DEFIP_DATA_IPV4m:
    case EXT_SRC_HIT_BITS_IPV4m:
    case EXT_DST_HIT_BITS_IPV4m:
    case EXT_TCAM_VBIT_DEFIP_IPV4m:
        part = TCAM_PARTITION_FWD_IP4;
        break;
    case EXT_IPV6_64_DEFIPm:
    case EXT_IPV6_64_DEFIP_TCAMm:
    case EXT_IPV6_64_TCAMm:
    case EXT_DEFIP_DATA_IPV6_64m:
    case EXT_SRC_HIT_BITS_IPV6_64m:
    case EXT_DST_HIT_BITS_IPV6_64m:
    case EXT_TCAM_VBIT_DEFIP_IPV6_64m:
        part = TCAM_PARTITION_FWD_IP6U;
        break;
    case EXT_IPV6_128_DEFIPm:
    case EXT_IPV6_128_DEFIP_TCAMm:
    case EXT_DEFIP_DATA_IPV6_128m:
    case EXT_SRC_HIT_BITS_IPV6_128m:
    case EXT_DST_HIT_BITS_IPV6_128m:
    case EXT_TCAM_VBIT_DEFIP_IPV6_128m:
        part = TCAM_PARTITION_FWD_IP6;
        break;
    case EXT_DEFIP_DATAm:
        part = TCAM_PARTITION_FWD_IP4; 
        if (mem_index < partitions[part].num_entries) {
            break;
        }
        mem_index -= partitions[part].num_entries;
        part = TCAM_PARTITION_FWD_IP6U;
        if (mem_index < partitions[part].num_entries) {
            break;
        }
        mem_index -= partitions[part].num_entries;
        part = TCAM_PARTITION_FWD_IP6;
        break;
    case EXT_LOC_SRC_HBITSm:
        if (mem_index < partitions[TCAM_PARTITION_FWD_L2].num_entries) {
            part = TCAM_PARTITION_FWD_L2;
        } else {
            mem_index -= (partitions[TCAM_PARTITION_FWD_L2].num_entries+31)/32;
            part = TCAM_PARTITION_FWD_L2_WIDE;
        }
        break;
    case EXT_SRC_HBITSm:
    case EXT_DST_HBITSm:
        for (part = TCAM_PARTITION_FWD_L2; part < TCAM_PARTITION_FWD_IP6;
             part++) {
            if (mem_index < (partitions[part].num_entries + 31) / 32) {
                break;
            }
            mem_index -= (partitions[part].num_entries + 31) / 32;
        }
        break;
    case EXT_IPV4_TCAMm:
        for (part = TCAM_PARTITION_FWD_IP4_UCAST; part < TCAM_PARTITION_FWD_IP4;
             part++) {
            if (mem_index < partitions[part].num_entries) {
                break;
            }
            mem_index -= partitions[part].num_entries;
        }
        break;
    case EXT_IPV6_128_TCAMm:
        for (part = TCAM_PARTITION_FWD_IP6_128_UCAST;
             part < TCAM_PARTITION_FWD_IP6; part++) {
            if (mem_index < partitions[part].num_entries) {
                break;
            }
            mem_index -= partitions[part].num_entries;
        }
        break;
    case EXT_ACL480_TCAM_MASKm:
    case EXT_ACL480_TCAM_DATAm:
        part = TCAM_PARTITION_ACL480;
        break;
    /* coverity[dead_error_begin : FALSE] */
    case EXT_ACL144_TCAMm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
            part = TCAM_PARTITION_ACL_L2C;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2C].num_entries;
        if (mem_index < partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
            part = TCAM_PARTITION_ACL_IP4C;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_IP4C].num_entries;
        part = TCAM_PARTITION_ACL_IP6C;
        break;
    /* coverity[dead_error_begin : FALSE] */
    case EXT_ACL288_TCAMm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            part = TCAM_PARTITION_ACL_L2;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2].num_entries;
        part = TCAM_PARTITION_ACL_IP4;
        break;
    /* coverity[dead_error_begin : FALSE] */
    case EXT_ACL360_TCAM_DATAm:
        part = TCAM_PARTITION_ACL_IP6S;
        break;
    /* coverity[dead_error_begin : FALSE] */
    case EXT_ACL432_TCAM_DATAm:
        if (mem_index < partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
            part = TCAM_PARTITION_ACL_IP6F;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_IP6F].num_entries;
        if (mem_index < partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
            part = TCAM_PARTITION_ACL_L2IP4;
            break;
        }
        mem_index -= partitions[TCAM_PARTITION_ACL_L2IP4].num_entries;
        part = TCAM_PARTITION_ACL_L2IP6;
        break;
    case EXT_ACL480_FP_POLICY_1Xm:
    case EXT_ACL480_FP_POLICY_2Xm:
    case EXT_ACL480_FP_POLICY_3Xm:
    case EXT_ACL480_FP_POLICY_4Xm:
    case EXT_ACL480_FP_POLICY_6Xm:
        part = TCAM_PARTITION_ACL480;
        break;
    case EXT_ACL320_FP_POLICY_1Xm:
    case EXT_ACL320_FP_POLICY_2Xm:
    case EXT_ACL320_FP_POLICY_3Xm:
    case EXT_ACL320_FP_POLICY_4Xm:
    case EXT_ACL320_FP_POLICY_6Xm:
    case EXT_ACL320_TCAMm:
        part = TCAM_PARTITION_ACL320;
        break;
    case EXT_ACL160_FP_POLICY_1Xm:
    case EXT_ACL160_FP_POLICY_2Xm:
    case EXT_ACL160_FP_POLICY_3Xm:
    case EXT_ACL160_FP_POLICY_4Xm:
    case EXT_ACL160_FP_POLICY_6Xm:
    case EXT_ACL160_TCAMm:
        part = TCAM_PARTITION_ACL160;
        break;
    case EXT_ACL80_FP_POLICY_1Xm:
    case EXT_ACL80_FP_POLICY_2Xm:
    case EXT_ACL80_FP_POLICY_3Xm:
    case EXT_ACL80_FP_POLICY_4Xm:
    case EXT_ACL80_FP_POLICY_6Xm:
    case EXT_ACL80_TCAMm:
        part = TCAM_PARTITION_ACL80;
        break;
    case EXT_FP_POLICY_1Xm:
    case EXT_FP_POLICY_2Xm:
    case EXT_FP_POLICY_3Xm:
    case EXT_FP_POLICY_4Xm:
    case EXT_FP_POLICY_6Xm:
#if 1
        for (part = TCAM_PARTITION_ACL80; part <= TCAM_PARTITION_ACL480; part++)
        {
            if (mem_index < partitions[part].num_entries) {
                break;
            }
            mem_index -= partitions[part].num_entries;
        }
#else
        part = TCAM_PARTITION_ACL480;
#endif
        break;
    case EXT_ACL288_TCAM_L2m:
    case EXT_FP_POLICY_ACL288_L2_1Xm:
    case EXT_FP_POLICY_ACL288_L2_2Xm:
    case EXT_FP_POLICY_ACL288_L2_3Xm:
    case EXT_FP_POLICY_ACL288_L2_4Xm:
    case EXT_FP_POLICY_ACL288_L2_6Xm:
        part = TCAM_PARTITION_ACL_L2;
        break;
    case EXT_ACL288_TCAM_IPV4m:
    case EXT_FP_POLICY_ACL288_IPV4_1Xm:
    case EXT_FP_POLICY_ACL288_IPV4_2Xm:
    case EXT_FP_POLICY_ACL288_IPV4_3Xm:
    case EXT_FP_POLICY_ACL288_IPV4_4Xm:
    case EXT_FP_POLICY_ACL288_IPV4_6Xm:
        part = TCAM_PARTITION_ACL_IP4;
        break;
    case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_1Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_2Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_3Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_4Xm:
    case EXT_FP_POLICY_ACL360_IPV6_SHORT_6Xm:
        part = TCAM_PARTITION_ACL_IP6S;
        break;
    case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_1Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_2Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_3Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_4Xm:
    case EXT_FP_POLICY_ACL432_IPV6_LONG_6Xm:
        part = TCAM_PARTITION_ACL_IP6F;
        break;
    case EXT_ACL144_TCAM_L2m:
    case EXT_FP_POLICY_ACL144_L2_1Xm:
    case EXT_FP_POLICY_ACL144_L2_2Xm:
    case EXT_FP_POLICY_ACL144_L2_3Xm:
    case EXT_FP_POLICY_ACL144_L2_4Xm:
    case EXT_FP_POLICY_ACL144_L2_6Xm:
        part = TCAM_PARTITION_ACL_L2C;
        break;
    case EXT_ACL144_TCAM_IPV4m:
    case EXT_FP_POLICY_ACL144_IPV4_1Xm:
    case EXT_FP_POLICY_ACL144_IPV4_2Xm:
    case EXT_FP_POLICY_ACL144_IPV4_3Xm:
    case EXT_FP_POLICY_ACL144_IPV4_4Xm:
    case EXT_FP_POLICY_ACL144_IPV4_6Xm:
        part = TCAM_PARTITION_ACL_IP4C;
        break;
    case EXT_ACL144_TCAM_IPV6m:
    case EXT_FP_POLICY_ACL144_IPV6_1Xm:
    case EXT_FP_POLICY_ACL144_IPV6_2Xm:
    case EXT_FP_POLICY_ACL144_IPV6_3Xm:
    case EXT_FP_POLICY_ACL144_IPV6_4Xm:
    case EXT_FP_POLICY_ACL144_IPV6_6Xm:
        part = TCAM_PARTITION_ACL_IP6C;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV4m:
    case EXT_FP_POLICY_ACL432_L2_IPV4_1Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_2Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_3Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_4Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV4_6Xm:
        part = TCAM_PARTITION_ACL_L2IP4;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV6m:
    case EXT_FP_POLICY_ACL432_L2_IPV6_1Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_2Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_3Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_4Xm:
    case EXT_FP_POLICY_ACL432_L2_IPV6_6Xm:
        part = TCAM_PARTITION_ACL_L2IP6;
        break;
    default:
        return SOC_E_INTERNAL;
    }

#ifdef QT
    /* coverity[dead_error_line] */
    *raw_index = (tcam_base_override ? tcam_base_override :
                  partitions[part].tcam_base) +
                 (mem_index * entries_per_index <<
                        partitions[part].tcam_width_shift);
#else
    *raw_index = partitions[part].tcam_base +
        (mem_index * entries_per_index << partitions[part].tcam_width_shift);
#endif

    return SOC_E_NONE;
}

/* currently needed only for ext_l2_entry_1 an ext_l2_entry_2 */
int
soc_triumph3_tcam_raw_index_to_mem_index(int unit, int raw_index, 
                                            soc_mem_t *mem, int *mem_index)
{
    /* EXT_l2_ENTRY_1 is always first memory */
    if (raw_index <= soc_mem_index_max(unit, EXT_L2_ENTRY_1m)) {
        *mem = EXT_L2_ENTRY_1m;
        *mem_index = raw_index;
        return SOC_E_NONE;
    }
    raw_index -= soc_mem_index_count(unit, EXT_L2_ENTRY_1m);
    if (raw_index <= soc_mem_index_max(unit, EXT_L2_ENTRY_2m)) {
        *mem = EXT_L2_ENTRY_2m;
        *mem_index = raw_index;
        return SOC_E_NONE;
    }
    /* no other memories supported for now */
    return SOC_E_FAIL;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
soc_tcam_init(int unit)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_init(unit);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_read_entry(int unit, int part, int index, uint32 *mask, uint32 *data,
                    int *valid)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_read_entry(unit, part, index, mask, data,
                                           valid);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_write_entry(int unit, int part, int index, uint32 *mask, uint32 *data)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_write_entry(unit, part, index, mask, data);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_set_valid(int unit, int part, int index, int valid)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_search_entry(int unit, int part1, int part2, uint32 *data,
                      int *index1, int *index2)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_search_entry(unit, part1, part2, data, index1,
                                             index2);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_write_ib(int unit, uint32 addr, uint32 data)
{
    return SOC_E_UNAVAIL;
}

int
soc_tcam_write_ima(int unit, uint32 addr, uint32 d0_msb, uint32 d1_lsb)
{
    return SOC_E_UNAVAIL;
}

int
soc_tcam_read_ima(int unit, uint32 addr, uint32 *d0_msb, uint32 *d1_lsb)
{
    return SOC_E_UNAVAIL;
}

int
soc_tcam_write_dbreg(int unit, uint32 addr, uint32 d0_msb, uint32 d1,
                     uint32 d2_lsb)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_write_reg(unit, addr, d0_msb, d1, d2_lsb);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_read_dbreg(int unit, uint32 addr, uint32 *d0_msb, uint32 *d1,
                    uint32 *d2_lsb)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_read_reg(unit, addr, d0_msb, d1, d2_lsb);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_get_info(int unit, int *type, int *subtype, int *dc_val,
                  soc_tcam_info_t **tcam_info)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        return soc_triumph_tcam_get_info(unit, type, subtype, dc_val,
                                         tcam_info);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_get_part_size(int unit, soc_tcam_partition_type_t part,
                       int *num_entries, int *num_entries_include_pad)
{
#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_esm_support)) {
        soc_tcam_partition_t *partitions;

        partitions =
            ((soc_tcam_info_t *)SOC_CONTROL(unit)->tcam_info)->partitions;

        if (part < TCAM_PARTITION_RAW || part >= TCAM_PARTITION_COUNT) {
            return SOC_E_PARAM;
        }
        if (num_entries != NULL) {
            *num_entries = partitions[part].num_entries;
        }
        if (num_entries_include_pad != NULL) {
            *num_entries_include_pad =
                partitions[part].num_entries_include_pad;
        }
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_part_index_to_mem_index(int unit, soc_tcam_partition_type_t part,
                                 int part_index, soc_mem_t mem, int *mem_index)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            return soc_triumph3_tcam_part_index_to_mem_index(unit, part, part_index,
                                                             mem, mem_index);
        }
#endif
        return soc_triumph_tcam_part_index_to_mem_index(unit, part, part_index,
                                                        mem, mem_index);
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_tcam_mem_index_to_raw_index(int unit, soc_mem_t mem, int mem_index,
                                soc_mem_t *real_mem, int *raw_index)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            return soc_triumph3_tcam_mem_index_to_raw_index(unit, mem, mem_index,
                                                       real_mem, raw_index);
        }
#endif
        return soc_triumph_tcam_mem_index_to_raw_index(unit, mem, mem_index,
                                                       real_mem, raw_index);
    }
#endif
    return SOC_E_UNAVAIL;
}

/* Given an external tcam index, map the raw index to an esm memory.*/
int
soc_tcam_raw_index_to_mem_index(int unit, int raw_index, soc_mem_t *mem, 
                                int *mem_index)
{
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            return soc_triumph3_tcam_raw_index_to_mem_index(unit, raw_index, 
                                                                mem, mem_index);
        }
#endif
    }
#endif
    return SOC_E_UNAVAIL;
}
