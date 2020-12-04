/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tcam.c
 * Purpose:     TR3 specific ESM code.
 * Requires:
 */

#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/triumph3.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/tcam/tcamtype1.h>
#include <soc/er_tcam.h>

#ifdef BCM_TRIUMPH3_SUPPORT

/*
 * sorted order of TCAM partitions in ascending order of entry size, per
 * entry type (L2, L3, ACL). Order of L2 and L3 partitions can change
 * depending on database duplication
 */
uint32 tr3_tcam_partition_order[] = {
    TCAM_PARTITION_FWD_L2,       /* Triumph */
    TCAM_PARTITION_FWD_L2_WIDE,  /* Triumph3 */
    TCAM_PARTITION_FWD_IP4_UCAST,/* Triumph3 */
    TCAM_PARTITION_FWD_IP4_UCAST_WIDE, /* Triumph3 */
    TCAM_PARTITION_FWD_IP4,      /* Triumph */
    TCAM_PARTITION_FWD_IP6U,     /* Triumph */
    TCAM_PARTITION_FWD_IP6_128_UCAST, /* Triumph3 */
    TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE, /* Triumph3 */
    TCAM_PARTITION_FWD_IP6,      /* Triumph */
    TCAM_PARTITION_ACL80,        /* Triumph3 */
    TCAM_PARTITION_ACL_L2C,      /* Triumph */
    TCAM_PARTITION_ACL_IP4C,     /* Triumph */
    TCAM_PARTITION_ACL_IP6C,     /* Triumph */
    TCAM_PARTITION_ACL160,       /* Triumph3 */
    TCAM_PARTITION_ACL_L2,       /* Triumph */
    TCAM_PARTITION_ACL_IP4,      /* Triumph */
    TCAM_PARTITION_ACL_L2IP4,      /* Triumph */
    TCAM_PARTITION_ACL320,       /* Triumph3 */
    TCAM_PARTITION_ACL_IP6S,     /* Triumph */
    TCAM_PARTITION_ACL_IP6F,     /* Triumph */
    TCAM_PARTITION_ACL_L2IP6,      /* Triumph */
    TCAM_PARTITION_ACL480,       /* Triumph3 */
    0,                     /* to be populated with duplicate partitons if any */
    0,
    0,
    0
};

uint32 tr3_tcam_partition_order_num_entries = sizeof(tr3_tcam_partition_order)/
                                              sizeof(uint32);

/* record offset of start of duplicate partitions here */
uint32 tr3_dup_part_offset = 22;
uint32 tr3_dup_part_count;

static tr3_worst_fme_t tr3_worst_fme[SOC_MAX_NUM_DEVICES];

int
soc_tr3_esm_get_worst_fme(int unit)
{
    return tr3_worst_fme[unit].idx;
}

int
soc_tr3_esm_init_read_config(int unit)
{
    soc_control_t *soc;
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int table_size[TCAM_PARTITION_COUNT];
    int i, count;
    int ext_l2_duplicated = 0;
    int ext_ipv4_duplicated = 0;
    int ext_ipv6u_duplicated = 0;
    int ext_ipv6_duplicated = 0;
    int ext_l2_bulk_enable = 1, ext_l2_hit_bit_enable = 1;
    int ext_l3_hit_bit_enable = 1, ext_acl80_policy_width=5;
    int ext_acl160_policy_width=5, ext_acl320_policy_width=5;
    int ext_acl480_policy_width=5;
    int ext_policy_width, dup_count=0;

    soc = SOC_CONTROL(unit);
    tr3_dup_part_count = 0;

    sal_memset(table_size, 0, sizeof(table_size));

    table_size[TCAM_PARTITION_FWD_L2] =
        soc_property_get(unit, spn_EXT_L2_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_L2_WIDE] =
        soc_property_get(unit, spn_EXT_L2_WIDE_FWD_TABLE_SIZE, 0);

    table_size[TCAM_PARTITION_FWD_IP4_UCAST] =
        soc_property_get(unit, spn_EXT_IP4_HOST_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP4_UCAST_WIDE] =
        soc_property_get(unit, spn_EXT_IP4_HOST_WIDE_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP4] =
        soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6U] =
        soc_property_get(unit, spn_EXT_IP6U_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6_128_UCAST] =
        soc_property_get(unit, spn_EXT_IP6_HOST_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE] =
        soc_property_get(unit, spn_EXT_IP6_HOST_WIDE_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6] =
        soc_property_get(unit, spn_EXT_IP6_FWD_TABLE_SIZE, 0);

    table_size[TCAM_PARTITION_ACL_L2] =
        soc_property_get(unit, spn_EXT_L2_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP4] =
        soc_property_get(unit, spn_EXT_IP4_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6S] =
        soc_property_get(unit, spn_EXT_IP6S_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6F] =
        soc_property_get(unit, spn_EXT_IP6F_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2C] =
        soc_property_get(unit, spn_EXT_L2C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP4C] =
        soc_property_get(unit, spn_EXT_IP4C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6C] =
        soc_property_get(unit, spn_EXT_IP6C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2IP4] =
        soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2IP6] =
        soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL80] =
        soc_property_get(unit, spn_EXT_ACL80_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL160] =
        soc_property_get(unit, spn_EXT_ACL160_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL320] =
        soc_property_get(unit, spn_EXT_ACL320_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL480] =
        soc_property_get(unit, spn_EXT_ACL480_TABLE_SIZE, 0);

    ext_l2_duplicated = soc_property_get(unit, spn_EXT_L2_TABLE_DUPLICATED, 0);
    ext_ipv4_duplicated = soc_property_get(unit, spn_EXT_IP4_TABLE_DUPLICATED, 
                                           0);
    ext_ipv6u_duplicated = soc_property_get(unit, spn_EXT_IP6U_TABLE_DUPLICATED,
                                           0);
    ext_ipv6_duplicated = soc_property_get(unit, spn_EXT_IP6_TABLE_DUPLICATED, 
                                           0);

    /* reorder partitions if there are duplicated databases */
        

    /* L2 partition always comes first. So fake it as duplicate as far
     * as ordering is concerned 
     */
    dup_count = 2;
    tr3_tcam_partition_order[0] = TCAM_PARTITION_FWD_L2;
    tr3_tcam_partition_order[1] = TCAM_PARTITION_FWD_L2_WIDE;
    if (ext_l2_duplicated) {
        table_size[TCAM_PARTITION_FWD_L2_DUP] = 
                          ROUND_UP_M(table_size[TCAM_PARTITION_FWD_L2], 4096) +
                       ROUND_UP_M(table_size[TCAM_PARTITION_FWD_L2_WIDE], 4096);
        tr3_tcam_partition_order[tr3_dup_part_offset + tr3_dup_part_count] = 
                                                    TCAM_PARTITION_FWD_L2_DUP;
        tr3_dup_part_count++;
    }
    if (ext_ipv4_duplicated) {
        table_size[TCAM_PARTITION_FWD_IP4_DUP] = 
                   ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP4_UCAST], 4096) + 
               ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP4_UCAST_WIDE], 4096) +
                           ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP4], 4096);
        tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP4_UCAST;
        tr3_tcam_partition_order[dup_count++] = 
                                TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
        tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP4;
        tr3_tcam_partition_order[tr3_dup_part_offset + tr3_dup_part_count] = 
                                                    TCAM_PARTITION_FWD_IP4_DUP;
        tr3_dup_part_count++;
    }
    if (ext_ipv6u_duplicated) {
        table_size[TCAM_PARTITION_FWD_IP6U_DUP] = 
                          ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP6U], 4096);
        tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP6U;
        tr3_tcam_partition_order[tr3_dup_part_offset + tr3_dup_part_count] = 
                                                    TCAM_PARTITION_FWD_IP6U_DUP;
        tr3_dup_part_count++;
    }
    if (ext_ipv6_duplicated) {
        table_size[TCAM_PARTITION_FWD_IP6_DUP] = 
               ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP6_128_UCAST], 2048) + 
           ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE], 2048) +
                           ROUND_UP_M(table_size[TCAM_PARTITION_FWD_IP6], 2048);
        tr3_tcam_partition_order[dup_count++]= TCAM_PARTITION_FWD_IP6_128_UCAST;
        tr3_tcam_partition_order[dup_count++] = 
                              TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
        tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP6;
        tr3_tcam_partition_order[tr3_dup_part_offset + tr3_dup_part_count] = 
                                                    TCAM_PARTITION_FWD_IP6_DUP;
        tr3_dup_part_count++;
    }
    if (tr3_dup_part_count) {
        /* fill remaining part of partition_order array */
        /* whatever is not duplicated comes here */
        if (!ext_ipv4_duplicated) {
            tr3_tcam_partition_order[dup_count++]=TCAM_PARTITION_FWD_IP4_UCAST;
            tr3_tcam_partition_order[dup_count++] =
                                             TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP4;
        }
        if (!ext_ipv6u_duplicated) {
            tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP6U;
        }
        if (!ext_ipv6_duplicated) {
            tr3_tcam_partition_order[dup_count++] = 
                                               TCAM_PARTITION_FWD_IP6_128_UCAST;
            tr3_tcam_partition_order[dup_count++] = 
                                        TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            tr3_tcam_partition_order[dup_count++] = TCAM_PARTITION_FWD_IP6;
        }
    }

    count = 0;
    for (i = TCAM_PARTITION_RAW + 1; i < TCAM_PARTITION_COUNT; i++) {
        count += table_size[i];
    }

    /* No table is configured, done! */
    if (!count) {
        SOC_FEATURE_CLEAR(unit, soc_feature_esm_support);
        SOC_FEATURE_CLEAR(unit, soc_feature_etu_support);
        SOC_FEATURE_CLEAR(unit, soc_feature_esm_correction);
        return SOC_E_NONE;
    }

    if (soc->tcam_info == NULL) {
        soc->tcam_info = sal_alloc(sizeof(soc_tcam_info_t), "tcam info");
        if (soc->tcam_info == NULL) {
            SOC_FEATURE_CLEAR(unit, soc_feature_esm_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_etu_support);
            SOC_FEATURE_CLEAR(unit, soc_feature_esm_correction);
            return SOC_E_MEMORY;
        }
        /* Clear ESM information */
        sal_memset(soc->tcam_info, 0, sizeof(soc_tcam_info_t));
    }

    tcam_info = soc->tcam_info;
    partitions = tcam_info->partitions;

    for (i = TCAM_PARTITION_RAW + 1; i < TCAM_PARTITION_COUNT; i++) {
        tcam_info->partitions[i].num_entries = table_size[i];
    }
    if (table_size[TCAM_PARTITION_FWD_L2_WIDE]) {
        if (table_size[TCAM_PARTITION_FWD_L2]) {
            partitions[TCAM_PARTITION_FWD_L2].flags |= 
                                                TCAM_PARTITION_FLAGS_COALESCE; 
        }
    }
    if (table_size[TCAM_PARTITION_FWD_IP4_UCAST]) {
        if (table_size[TCAM_PARTITION_FWD_IP4_UCAST_WIDE] || 
            table_size[TCAM_PARTITION_FWD_IP4]) {
            partitions[TCAM_PARTITION_FWD_IP4_UCAST].flags |= 
                                                TCAM_PARTITION_FLAGS_COALESCE; 
        }
    }
    if (table_size[TCAM_PARTITION_FWD_IP4_UCAST_WIDE]) {
        if (table_size[TCAM_PARTITION_FWD_IP4]) {
            partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].flags |= 
                                                TCAM_PARTITION_FLAGS_COALESCE; 
        }
    }
    if (table_size[TCAM_PARTITION_FWD_IP6_128_UCAST]) {
        if (table_size[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE] || 
            table_size[TCAM_PARTITION_FWD_IP6]) {
            partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].flags |= 
                                                TCAM_PARTITION_FLAGS_COALESCE; 
        }
    }
    if (table_size[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE]) {
        if (table_size[TCAM_PARTITION_FWD_IP6]) {
            partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].flags |= 
                                                TCAM_PARTITION_FLAGS_COALESCE; 
        }
    }

    /* get l2 acl mode. Default to non NL7K mode */
    tcam_info->mode = soc_property_get(unit, spn_EXT_TCAM_MODE, 0);

    /* configure associated data information here */
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_L2, 0, 0, 0, 
                               ext_l2_bulk_enable, ext_l2_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L2);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_L2_WIDE, 0, 1, 0, 
                               ext_l2_bulk_enable, ext_l2_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L2);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_L2_DUP, 0, 0, 0, 
                               0, 0, 
                               TCAM_PARTITION_FLAGS_NO_AD);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP4_UCAST, 0, 0, 
                               ET_PA_AD_L3_ENTRY_UCAST, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP4_UCAST_WIDE, 0, 3, 
                               ET_PA_AD_L3_ENTRY_UCAST, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP4, 0, 0, 
                               ET_PA_AD_L3_ENTRY_DEFIP, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP4_DUP, 0, 0, 0, 
                               0, 0, 
                               TCAM_PARTITION_FLAGS_NO_AD);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6U, 0, 0, 
                               ET_PA_AD_L3_ENTRY_DEFIP, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6U_DUP, 0, 0, 0, 
                               0, 0, 
                               TCAM_PARTITION_FLAGS_NO_AD);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6_128_UCAST, 1, 0, 
                               ET_PA_AD_L3_ENTRY_UCAST, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE, 1, 3, 
                               ET_PA_AD_L3_ENTRY_UCAST, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6, 1, 0, 
                               ET_PA_AD_L3_ENTRY_DEFIP, 0, 
                               ext_l3_hit_bit_enable, 
                               TCAM_PARTITION_FLAGS_TYPE_L3);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_FWD_IP6_DUP, 0, 0, 0, 
                               0, 0, 
                               TCAM_PARTITION_FLAGS_NO_AD);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL80, 0, 
                               ext_acl80_policy_width, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL160, 1, 
                               ext_acl160_policy_width, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL320, 2, 
                               ext_acl320_policy_width, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL480, 3, 
                               ext_acl480_policy_width, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    /* TR2 type ACLs: */
    /* policy width is 0-based internally */
    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_L2_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_L2, 2, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_IP4_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_IP4, 2, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_IP6S_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_IP6S, 3, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_IP6F_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_IP6F, 3, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_L2C_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_L2C, 1, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_IP4C_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_IP4C, 1, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_IP6C_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_IP6C, 1, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                        spn_EXT_L2IP4_ACL_TABLE_POLICY_WIDTH,1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_L2IP4, 2, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);

    ext_policy_width = soc_property_get(unit, 
                                       spn_EXT_L2IP6_ACL_TABLE_POLICY_WIDTH, 1);
    SET_TCAM_PARTITION_AD_INFO(TCAM_PARTITION_ACL_L2IP6, 3, 
                               ext_policy_width - 1, 0, 0, 
                               0, TCAM_PARTITION_FLAGS_TYPE_ACL);


    /* get tcams' mdio port ids */
    tcam_info->mdio_port_tcam0 = soc_property_suffix_num_get(unit, 0, 
                                            spn_PORT_PHY_ADDR, "ext_tcam", 1);
    tcam_info->mdio_port_tcam1 = soc_property_suffix_num_get(unit, 1, 
                                            spn_PORT_PHY_ADDR, "ext_tcam", 2);
    return SOC_E_NONE;
}

/*
 * Calculate the AD memory size requirements for L2, L3 and ACL, so ISM can
 * allocate this memory for ESM
 */
int soc_tr3_esm_get_ism_mem_size(int unit, uint32 mem_type, uint32 *size)
{
    soc_tcam_info_t *tcam_info;
    uint32 num_ad_entries = 0;

    if (size == NULL) {
        return SOC_E_PARAM;
    }

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        *size = 0;
        return SOC_E_NONE;
    }
    switch (mem_type) {
    case SOC_ISM_MEM_ESM_L2:
        num_ad_entries = tcam_info->partitions[TCAM_PARTITION_FWD_L2].num_entries
            + 2 * tcam_info->partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries; 
        break;
    case SOC_ISM_MEM_ESM_L3:
        num_ad_entries = 
                tcam_info->partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries
     + 4 * tcam_info->partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries
              + tcam_info->partitions[TCAM_PARTITION_FWD_IP4].num_entries
              + tcam_info->partitions[TCAM_PARTITION_FWD_IP6U].num_entries
           + tcam_info->partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries
     +4*tcam_info->partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries
              + tcam_info->partitions[TCAM_PARTITION_FWD_IP6].num_entries;
        break;
    case SOC_ISM_MEM_ESM_ACL:
        num_ad_entries = 
           tcam_info->partitions[TCAM_PARTITION_ACL80].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL80].ad_info.ad_width + 1) +
           tcam_info->partitions[TCAM_PARTITION_ACL160].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL160].ad_info.ad_width + 1) +
           tcam_info->partitions[TCAM_PARTITION_ACL320].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL320].ad_info.ad_width + 1) +
           tcam_info->partitions[TCAM_PARTITION_ACL480].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL480].ad_info.ad_width + 1) +
           tcam_info->partitions[TCAM_PARTITION_ACL_L2C].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_L2C].ad_info.ad_width + 1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_IP4C].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_IP4C].ad_info.ad_width +1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_IP6C].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_IP6C].ad_info.ad_width +1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_L2].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_L2].ad_info.ad_width +1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_IP4].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_IP4].ad_info.ad_width +1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_L2IP4].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_L2IP4].ad_info.ad_width+1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_IP6S].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_IP6S].ad_info.ad_width+1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_IP6F].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_IP6F].ad_info.ad_width+1)+
           tcam_info->partitions[TCAM_PARTITION_ACL_L2IP6].num_entries *
           (tcam_info->partitions[TCAM_PARTITION_ACL_L2IP6].ad_info.ad_width+1);

        if (SAL_BOOT_QUICKTURN || SAL_BOOT_SIMULATION) {
            /* running close to full, increase to avoid spillover */
            num_ad_entries *= 4;
        }
        break;
    default:
        *size = 0;
        return SOC_E_PARAM;
    }
    *size = ((num_ad_entries + 11)/12 + 1023)/1024;
    /* ESM can only use ISM banks of size min 4K. Align size to 4K boundary */
    if (*size) {
        /* Alignment needed only when the *size is not at 4 boundary */
        if (((*size)%4) != 0) {
            *size += (4 - (*size)%4);
        }
        *size *= 1024;
    }
    return SOC_E_NONE;
}

uint32 tr3_tcam_access_debug=0;
uint32 tr3_tcam_sim =0;

/* dbus is always 80 bytes, i.e, 20 words */
/* ibus is one word per tcam */
int
soc_triumph3_tcam_access(int unit, int op_type, int num_inst, int num_pre_nop,
                        int num_post_nop, uint32 *dbus, int *ibus, 
						int num_bits, int num_rsps)
{
    int j, retry;
    int eop_val;
    int num_long_words, rv;
    uint32 tcam_raw_control_word=0;
    uint32 *start_word;
    uint32 status; /* status2=0, status3=0; */
    etu_tx_raw_req_data_word_entry_t etu_data;
    etu_dbg_rx_raw_rsp_entry_t raw_rsp;
    int raw_data_fields[10] = {DW0f, DW1f, DW2f, DW3f, DW4f, DW5f, DW6f, DW7f,
                               DW8f, DW9f};

    /* dbus data maps to data words and ibus data maps to control word for 
     * nl11k 
     */

    /* clear start and complete bits in raw_req first */
    rv = READ_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, &tcam_raw_control_word);
    SOC_IF_ERROR_RETURN(rv);
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
	                  &tcam_raw_control_word, COMPLETEf, 0);
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
	                  &tcam_raw_control_word, STARTf, 0);
    rv = WRITE_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, tcam_raw_control_word);
    SOC_IF_ERROR_RETURN(rv);

#if 1
    if (tr3_tcam_access_debug) {
        LOG_CLI((BSL_META_U(unit,
                            "initial ETU_SBUS_CMD_ERR register state:\n")));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO1r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO1:0x%08x\n"), status));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO2r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO2:0x%08x\n"), status));
    }
#endif

    /* num_inst is always 1 */
    /* put data words into raw data words.*/
    start_word = dbus;
    for (j = 0; j < 10; j++) {
        soc_mem_field_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, (uint32 *)&etu_data, 
                          raw_data_fields[j], start_word);
        start_word += 2;
	}
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ETU_TX_RAW_REQ_DATA_WORDm, 
                        MEM_BLOCK_ALL, 0, &etu_data));

#if 1
    if (tr3_tcam_access_debug) {
        LOG_CLI((BSL_META_U(unit,
                            "after dw wr ETU_SBUS_CMD_ERR register state:\n")));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO1r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO1:0x%08x\n"), status));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO2r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO2:0x%08x\n"), status));
    }
#endif
	/* put control word into raw control words => set start bit */
		
    /* set CAPTURE bit */
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, CAPTUREf, 1);
	/* nl11k opcode */
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, OPCf, ibus[0]);

    /* num nops */
    if (num_pre_nop) {
		soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					      &tcam_raw_control_word, NUM_NOPf, num_pre_nop);
		soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, NOP_PRE_PSTf, 1);
	} else if (num_post_nop) {
		soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, NUM_NOPf, num_post_nop);
		soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, NOP_PRE_PSTf, 0);
	} else {
		soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, NUM_NOPf, 0);
    }

	/* number of datawords */
	num_long_words = (num_bits + 63)/64;
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, NUM_DWf, num_long_words);
		
	/* EOP field. Assume modul0 8 bytes */
    if ((op_type == TCAM_TR3_OP_REG_DB_X_READ)||(op_type==TCAM_TR3_OP_WRITE)||
    (op_type == TCAM_TR3_OP_CB_WR_CMP1)||(op_type==TCAM_TR3_OP_CB_WR_CMP2)){
        eop_val = 8;
    } else {
        eop_val = ((num_bits % 64) + 7)/8;
        if (eop_val == 0) eop_val = 8;
    }
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, EOPf, eop_val);

	/* Number of response indices */
#ifdef not_def
	if ((op_type == TCAM_TR_OP_SINGLE_SEARCH1) ||
         (op_type == TCAM_TR_OP_SINGLE_SEARCH0)) {
			soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, TOTAL_NUM_RSPf, 1);
		} else if (op_type == TCAM_TR_OP_PARALLEL_SEARCH) {
			soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, TOTAL_NUM_RSPf, 4);
		} else {
			soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
						  &tcam_raw_control_word, TOTAL_NUM_RSPf, 0);
        }
#endif
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, TOTAL_NUM_RSPf, num_rsps);

    /* not using the optimization of endian flipping */
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, NOT_SEARCHf, 1);

	/* complete flag */
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, COMPLETEf, 0);

    /* start flag */
	soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 
					  &tcam_raw_control_word, STARTf, 1);

	/* start the tcam operation */
    rv = WRITE_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, tcam_raw_control_word);

#if 1
    if (tr3_tcam_access_debug) {
        LOG_CLI((BSL_META_U(unit,
                            "after 1st cw wr ETU_SBUS_CMD_ERR register state:\n")));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO1r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO1:0x%08x\n"), status));
        SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO2r(unit, &status));
        LOG_CLI((BSL_META_U(unit,
                            "ETU_SBUS_CMD_ERR_INFO2:0x%08x\n"), status));
    }
#endif

    /* check if operation completed */
    if (SOC_SUCCESS(rv)) {
        for (retry = 2000; retry; retry--) {
            rv = READ_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, &tcam_raw_control_word);
            if (tr3_tcam_access_debug) {
                LOG_CLI((BSL_META_U(unit,
                                    "ETU_TX_RAW_REQ_CONTROL_WORD readback: 0x%08x\n"),
                         tcam_raw_control_word));
            }
            if (SOC_FAILURE(rv)) {
                break;
            }
#if 0
            if (soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, status, 
								TX_RAW_REQ_DONEf)) {
                break;
            }
#else
            if (soc_reg_field_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, tcam_raw_control_word, 
								COMPLETEf)) {
                break;
            }
#endif
            if(tr3_tcam_sim) {
                sal_usleep(1);
            } else {
                sal_udelay(5);
            }
        }
        if (!retry) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_triumph3_tcam_access: Instruction timeout\n")));
            if (tr3_tcam_sim) { return SOC_E_NONE; }
            rv = SOC_E_TIMEOUT;
        }
		rv = READ_ETU_GLOBAL_INTR_STSr(unit, &status);
        if (tr3_tcam_access_debug) {
            LOG_CLI((BSL_META_U(unit,
                                "ETU_GLOBAL_INTR_STS: 0x%08x\n"), status));
        }
        if (!(soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, status, TX_RAW_REQ_DONEf))) rv = SOC_E_FAIL; 
#if 0
        else {
            soc_reg_field_set(unit, ETU_GLOBAL_INTR_STSr, &status, TX_RAW_REQ_DONEf, 0);
            if (status) {
                SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO1r(unit, &status3));
                if (tr3_tcam_access_debug) {
                    LOG_CLI((BSL_META_U(unit,
                                        "ETU_SBUS_CMD_ERR_INFO1:0x%08x\n"), status3));
                }
                SOC_IF_ERROR_RETURN(READ_ETU_SBUS_CMD_ERR_INFO2r(unit, &status3));
                if (tr3_tcam_access_debug) {
                    LOG_CLI((BSL_META_U(unit,
                                        "ETU_SBUS_CMD_ERR_INFO2:0x%08x\n"), status3));
                }

                rv = SOC_E_FAIL;
            }
            /* prepare to clear corresponding bit in ETU_GLOBAL_INTR_CLEAR */
            soc_reg_field_set(unit, ETU_GLOBAL_INTR_STSr, &status2, TX_RAW_REQ_DONEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ETU_GLOBAL_INTR_CLEARr(unit, status2));
        }
#endif
    }

    /* return responses */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_DBG_RX_RAW_RSPm, MEM_BLOCK_ANY, 
						0, &raw_rsp));
    /* order of fields seems to vary from one regsfile to another */
#if 1
    soc_mem_field_get(unit, ETU_DBG_RX_RAW_RSPm, (uint32 *) &raw_rsp, DW0f,
                      &dbus[0]);
    soc_mem_field_get(unit, ETU_DBG_RX_RAW_RSPm, (uint32 *) &raw_rsp, DW1f,
                      &dbus[2]);
#else
    /* This is going to be the newer regs file. */
    soc_mem_field_get(unit, ETU_DBG_RX_RAW_RSPm, (uint32 *) &raw_rsp, DW1f,
                      &dbus[0]);
    soc_mem_field_get(unit, ETU_DBG_RX_RAW_RSPm, (uint32 *) &raw_rsp, DW0f,
                      &dbus[2]);
#endif
    return SOC_E_NONE;
}

int _soc_write_et_pa_xlat_entry(int unit, int idx, soc_et_pa_xlat_t *entry)
{
    et_pa_xlat_entry_t et_pa_entry;

    sal_memset(&et_pa_entry, 0, sizeof(et_pa_entry));
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, ET_WIDTHf,
                        entry->et_width);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, AD_WIDTHf,
                        entry->ad_width);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, ENTRY_TYPEf,
                        entry->entry_type);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, L2_BULK_ENf,
                        entry->l2_bulk_enable);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, HBIT_ENf,
                        entry->hit_bit_enable);

    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, 
                        HBIT_PA_BASEf, entry->hit_bit_pa_base);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, AD_BAf, 
                        entry->ad_ba);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, AD_RA_BASEf,
                        entry->ad_ra_base);
    soc_mem_field32_set(unit, ET_PA_XLATm, (uint32 *) &et_pa_entry, AD_CAf, 
                        entry->ad_ca);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ET_PA_XLATm, MEM_BLOCK_ALL, 
                                      idx, &et_pa_entry));
    return SOC_E_NONE;
}

typedef struct esm_key_to_field_ {
    /* waste one entry, use only 1 to 11 for convenience*/ 
    uint8 esm_key_subtype;
    uint8 f_selects[12]; 
    uint8 flags_field_not_present;
    uint8 mux_sel_flags[16];
    uint8 total_num_cwords;
    uint8 rsp_index_count[3];
    uint8 eop_cw[3];
    uint8 num_dwords_cw[3];
    uint8 context_addr_lsb_cw[3];
    uint8 opcode_cw[3];
    uint8 rsp_index_count_cw[3];
    uint8 lookup_type_rsp[4];
    uint8 drop_pkt_when_adm_ctl_deny;
    uint8 optional_cost[3];
    uint8 cost_cw[3];
    uint8 last_cword_is_optional;
    uint8 hwtl_exp_rsp_ptr;
} esm_key_to_field_t;

uint32 f_select_field[12] = {0, F1_SELECTf, 0, F3_SELECTf,
                              F4_SELECTf, F5_SELECTf, F6_SELECTf, F7_SELECTf,
                              F8_SELECTf, F9_SELECTf, F10_SELECTf, F11_SELECTf};
uint32 rsp_index_count_field[3] = {RSP_INDEX_COUNT_CW0f, RSP_INDEX_COUNT_CW1f,
                                   RSP_INDEX_COUNT_CW2f};
uint32 eop_cw_field[3] = {EOP_CW0f, EOP_CW1f, EOP_CW2f};
uint32 context_addr_lsb_cw_field[3] = {CONTEXT_ADDR_LSB_CW0f, 
                                  CONTEXT_ADDR_LSB_CW1f, CONTEXT_ADDR_LSB_CW2f};
uint32 num_dwords_field[3] = {NUM_DWORDS_CW0f, NUM_DWORDS_CW1f, NUM_DWORDS_CW2f};
uint32 opcode_cw_field[3] = {OPCODE_CW0f, OPCODE_CW1f, OPCODE_CW2f};
uint32 cost_cw_field[3] = {COST0f, COST1f, COST2f};
uint32 lookup_type_rsp_field[4] = {LOOKUP_TYPE_RSP_0f, LOOKUP_TYPE_RSP_1f,
                                   LOOKUP_TYPE_RSP_2f, LOOKUP_TYPE_RSP_3f};
uint32 flags_field[16] = {MUX_SEL_FLAG0f, MUX_SEL_FLAG1f, MUX_SEL_FLAG2f,
                          MUX_SEL_FLAG3f, MUX_SEL_FLAG4f, MUX_SEL_FLAG5f,
                          MUX_SEL_FLAG6f, MUX_SEL_FLAG7f, MUX_SEL_FLAG8f, 
                          MUX_SEL_FLAG9f, MUX_SEL_FLAG10f, MUX_SEL_FLAG11f, 
                          MUX_SEL_FLAG12f, MUX_SEL_FLAG13f, MUX_SEL_FLAG14f, 
                          MUX_SEL_FLAG15f} ;

int _soc_write_esm_key_id_to_field_map_entry(int unit, int idx, 
                                         esm_key_to_field_t *key_map)
{
    int i;
    esm_key_id_to_field_mapper_entry_t key_id;

    sal_memset(&key_id, 0, sizeof(key_id));

    /* L2 DA, SA as per spreadsheet */
    for (i = 1; i < 12; i++) {
        if (i == 2) continue;
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, f_select_field[i],
                            key_map->f_selects[i]);
    }
    
    if (!key_map->flags_field_not_present) {
        for (i = 0; i < 16; i++) {
            soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                                (uint32 *)&key_id, flags_field[i],
                                key_map->mux_sel_flags[i]);
        }
    }

    soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, (uint32 *)&key_id,
                            ESM_KEY_SUBTYPEf, key_map->esm_key_subtype);
    soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, (uint32 *)&key_id,
                    FLAGS_FIELD_NOT_PRESENTf, key_map->flags_field_not_present);

    soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, (uint32 *)&key_id,
                      TOTAL_NUM_CWORDSf, key_map->total_num_cwords);

    for (i = 0; i < 3; i++) {
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, rsp_index_count_field[i], 
                            key_map->rsp_index_count[i]);
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, eop_cw_field[i], 
                            key_map->eop_cw[i]);
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, context_addr_lsb_cw_field[i], 
                            key_map->context_addr_lsb_cw[i]);
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, num_dwords_field[i], 
                            key_map->num_dwords_cw[i]);
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, opcode_cw_field[i], 
                            key_map->opcode_cw[i]);
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, cost_cw_field[i], 
                            key_map->cost_cw[i]);
    }
    for (i = 0; i < 4; i++) {
        soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                            (uint32 *)&key_id, lookup_type_rsp_field[i], 
                            key_map->lookup_type_rsp[i]);
    }
    soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, (uint32 *)&key_id,
                        DROP_PKT_WHEN_ADM_CTL_DENYf, 
                        key_map->drop_pkt_when_adm_ctl_deny);
    soc_mem_field32_set(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, (uint32 *)&key_id,
                        HWTL_EXP_RSP_PTRf, key_map->hwtl_exp_rsp_ptr);

    /* update worst case FME */
    if ((key_map->cost_cw[0] + key_map->cost_cw[1] + key_map->cost_cw[2]) >
       (tr3_worst_fme[unit].cost0 + tr3_worst_fme[unit].cost1 + 
        tr3_worst_fme[unit].cost2)) {
        tr3_worst_fme[unit].cost0 = key_map->cost_cw[0];
        tr3_worst_fme[unit].cost1 = key_map->cost_cw[1];
        tr3_worst_fme[unit].cost2 = key_map->cost_cw[2];
        tr3_worst_fme[unit].idx = idx;
    }
    return soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ANY, idx, 
                        &key_id);
}

int soc_set_src_trunk_map_esm_srch_priority(int unit,
                                             int start_idx, int end_idx,
                                             int esm_srch_offset,
                                             int esm_srch_priority)
{
    source_trunk_map_table_entry_t *stm_entry;
    int         index, alloc_size, rv;        
    void       *buf;

    alloc_size = ((end_idx) - (start_idx) + 1) * sizeof((*(stm_entry)));
    buf        = soc_cm_salloc((unit), alloc_size, "source_trunk_map_table");

    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);
    rv = soc_mem_read_range((unit), (SOURCE_TRUNK_MAP_TABLEm),
                               MEM_BLOCK_ANY, (start_idx), (end_idx), buf);

    if (rv < 0) {
        soc_cm_sfree((unit), buf);
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return SOC_E_FAIL;
    }

    for (index = 0; index < (end_idx) - (start_idx) + 1; index++) {
        (stm_entry) =
                   soc_mem_table_idx_to_pointer((unit),
                                                (SOURCE_TRUNK_MAP_TABLEm),
                                                 source_trunk_map_table_entry_t *,
                                                 buf, index);

        soc_mem_field32_set((unit), (SOURCE_TRUNK_MAP_TABLEm), (stm_entry),
                         ESM_SEARCH_OFFSETf, (esm_srch_offset));

        /* port (l2 lookup) always wins */
        soc_mem_field32_set((unit), (SOURCE_TRUNK_MAP_TABLEm), (stm_entry),
                         ESM_SEARCH_PRIORITYf, (esm_srch_priority));
    }

    rv = soc_mem_write_range((unit), (SOURCE_TRUNK_MAP_TABLEm), MEM_BLOCK_ANY,
                                (start_idx), (end_idx), (void *) buf);
    soc_cm_sfree((unit), buf);
    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    if (rv < 0) {
         return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

typedef struct esm_pkt_id_tcam_entry_ {
    uint8 pkt_type;
    uint32 mask[2];
    uint8 l2_2nd_lkup_hit;
    uint8 l2_1st_lkup_hit;
    uint8 l3_key_type_2nd_lkup;
    uint8 l3_key_type_1st_lkup;
    uint8 l2_key_type_2nd_lkup;
    uint8 l2_key_type_1st_lkup;
    uint8 l3_lkup_status;
    uint8 l2_lkup_type_vlan_or_vfi;
    uint8 l3_iif_valid;
    uint8 svp_valid;
    uint8 udf_payload_id;
    uint8 my_stn_hit_and_enabled;
    uint8 tunnel_hit;
    uint8 tunnel_and_lpbk_type;
    uint8 l3_type;
    uint8 l2_payload;
    uint16 payload_type;
    uint8 valid;
} esm_pkt_id_tcam_entry_t;

int soc_set_pkt_id_tcam_entry(int unit, int idx, esm_pkt_id_tcam_entry_t *pkt_id)
{
    esm_pkt_type_id_entry_t tcam_entry;

    sal_memset(&tcam_entry, 0, sizeof(tcam_entry));
    /* set pkt_id tcam basic L2 search entry to pkt_type 0 */
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, PACKET_TYPEf, pkt_id->pkt_type);
    soc_mem_field_set(unit, ESM_PKT_TYPE_IDm, (uint32 *) &tcam_entry, MASKf,
                      pkt_id->mask);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                        L2_SECOND_LOOKUP_HITf, pkt_id->l2_2nd_lkup_hit);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                        L2_FIRST_LOOKUP_HITf, pkt_id->l2_1st_lkup_hit);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                     L3_KEY_TYPE_SECOND_LOOKUPf, pkt_id->l3_key_type_2nd_lkup);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                     L3_KEY_TYPE_FIRST_LOOKUPf, pkt_id->l3_key_type_1st_lkup);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                     L2_KEY_TYPE_SECOND_LOOKUPf, pkt_id->l2_key_type_2nd_lkup);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                     L2_KEY_TYPE_FIRST_LOOKUPf, pkt_id->l2_key_type_1st_lkup);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                     L3_LOOKUP_STATUSf, pkt_id->l3_lkup_status);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                 L2_LOOKUP_TYPE_VLAN_OR_VFIf, pkt_id->l2_lkup_type_vlan_or_vfi);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                      L3_IIF_VALIDf, pkt_id->l3_iif_valid);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, SVP_VALIDf,
                        pkt_id->svp_valid);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, UDF_PAYLOAD_IDf,
                        pkt_id->udf_payload_id);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                   MY_STATION_HIT_AND_ENABLEDf, pkt_id->my_stn_hit_and_enabled);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, TUNNEL_HITf,
                        pkt_id->tunnel_hit);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry,
                       TUNNEL_AND_LOOPBACK_TYPEf, pkt_id->tunnel_and_lpbk_type);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, L3_TYPEf,
                        pkt_id->l3_type);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, L2_PAYLOADf,
                        pkt_id->l2_payload);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, PAYLOAD_TYPEf,
                        pkt_id->payload_type);
    soc_mem_field32_set(unit, ESM_PKT_TYPE_IDm, &tcam_entry, VALIDf,
                        pkt_id->valid);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_PKT_TYPE_IDm, MEM_BLOCK_ANY,
                        idx, &tcam_entry));
    return SOC_E_NONE;
}

int soc_set_esm_search_profile_range(int unit, int idx_start, int idx_end, 
                                     int esm_eligible, int search_key_id)
{
    int rv, i, alloc_size;
    void *buf;
    esm_search_profile_entry_t *profile_entry;

    /* 32 entries per search offset */
    alloc_size = (idx_end-idx_start+1) * sizeof(esm_search_profile_entry_t);
    buf = soc_cm_salloc(unit, alloc_size, "esm_search_profile_range");
    if (buf == NULL) return SOC_E_MEMORY;

    for (i = idx_start; i <= idx_end; i++) {
        /* pointer to each entry : */
        profile_entry = soc_mem_table_idx_to_pointer
            (unit, ESM_SEARCH_PROFILEm,  esm_search_profile_entry_t *, 
             buf, (i - idx_start));
        soc_mem_field32_set(unit, ESM_SEARCH_PROFILEm, profile_entry, 
                            ESM_ELIGIBLEf, esm_eligible);
        soc_mem_field32_set(unit, ESM_SEARCH_PROFILEm, profile_entry, 
                            ESM_SEARCH_KEY_IDf, search_key_id);
    }
    rv = soc_mem_write_range(unit, ESM_SEARCH_PROFILEm, MEM_BLOCK_ANY, 
                             idx_start, idx_end, (void *) buf);
    soc_cm_sfree(unit, buf);
    return rv;
}
    

#ifdef later
int soc_triumph3_esm_test_init(int unit)
{
    esm_range_check_entry_t buf, idx_end;
    esm_key_to_field_t key_map_l2;
    int rv;

    soc_mem_field32_set(unit, ESM_RANGE_CHECKm, (uint32 *) buf, 
                            ESM_HWTL_SEARCH_KEYf, 0x1b1b2b2b);
    soc_mem_write(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ANY, 
                        21, &buf);
    buf[21]=0x1b1b2b2b; buf[20]=0x3b3b4b4b; buf[19]=0x5b4b5b5b; 
    buf[18]=0x6b6b7b7b;
    buf[17]=0x1a1a2a2a; buf[16]=0x3a3a4a4a; buf[15]=0x5a4a5a5a;
    buf[14]=0x6a6a7a7a;
    buf[13]=0x19192929; buf[12]=0x39394949; buf[11]=0x59495959;
    buf[10]=0x69697979;
    buf[9]=0x87868584; buf[8]=0x83828180; 
    buf[7]=0x73727270; 
    buf[6]=0x63626160; 
    buf[5]=0x51504140; 
    buf[4]=0x31302120; 
    buf[3]=0x1f1e1d1c; buf[2]=0x1b1a1918;buf[1]=0x17161514;
    buf[0]=0x13121110;

    idx_end = soc_mem_index_max(unit, ESM_RANGE_CHECKm);

    rv = soc_mem_write_range(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ANY, 
                             0, idx_end, (void *) buf);

    /* one test field mapper entry */
    sal_memset(&key_map_l2, 0, sizeof(key_map_l2));
    
    key_map_l2.f_selects[11] = F11_HWTL;
    key_map_l2.f_selects[8] = F8_HWTL;
    key_map_l2.f_selects[6] = F6_HWTL;
    key_map_l2.f_selects[4] = F4_HWTL;
    key_map_l2.f_selects[3] = F3_HWTL;
    key_map_l2.flags_field_not_present = 1;
    key_map_l2.total_num_cwords = 2;
    key_map_l2.rsp_index_count[0] = 1;
    key_map_l2.eop_cw[0] = 8;
    key_map_l2.context_addr_lsb_cw[0] = 0;
    key_map_l2.num_dwords_cw[0] = 2;
    key_map_l2.opcode_cw[0] = (1 << 6) | TR3_LTR_FWD_L2_DA;
    key_map_l2.rsp_index_count[1] = 1;
    key_map_l2.eop_cw[1] = 8;
    key_map_l2.eop_cw[2] = 8;
    key_map_l2.context_addr_lsb_cw[1] = 2;
    key_map_l2.num_dwords_cw[1] = 2;
    key_map_l2.opcode_cw[1] = (1 << 6) | TR3_LTR_FWD_L2_SA;
    key_map_l2.lookup_type_rsp[0] = KEY_GEN_RSP_TYPE_L2_DA; 
    key_map_l2.lookup_type_rsp[1] = KEY_GEN_RSP_TYPE_L2_SA; 
    key_map_l2.cost_cw[0] = 2; 
    key_map_l2.cost_cw[1] = 6; 
    key_map_l2.cost_cw[2] = 4; 
    key_map_l2.drop_pkt_when_adm_ctl_deny = 1;
    key_map_l2.hwtl_exp_rsp_ptr = 127;
    /* for test L2 */
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        31, &key_map_l2));

    return rv;
}
#endif

STATIC int 
_soc_tr3_set_dup_db_offset(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int part, dup_part, dup_db_offset;
    uint32 tmp2;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return 0;
    }
    SOC_IF_ERROR_RETURN(READ_ETU_CONFIG0r(unit, &tmp2));
    
    partitions = tcam_info->partitions;
    /* calculate duplicate database offset if there is one */
    part = 0;
    dup_part = 0;
    dup_db_offset = 0;
    if (partitions[TCAM_PARTITION_FWD_L2_DUP].num_entries) {
        soc_reg_field_set(unit, ETU_CONFIG0r, &tmp2, DUP_L2f, 1);
        part = TCAM_PARTITION_FWD_L2;
        dup_part = TCAM_PARTITION_FWD_L2_DUP;
    } 
    if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
        soc_reg_field_set(unit, ETU_CONFIG0r, &tmp2, DUP_IPV4f, 1);
        if (!part) {
            part = TCAM_PARTITION_FWD_IP4_UCAST;
            dup_part = TCAM_PARTITION_FWD_IP4_DUP;
        }
    } 
    if (partitions[TCAM_PARTITION_FWD_IP6U_DUP].num_entries) {
        soc_reg_field_set(unit, ETU_CONFIG0r, &tmp2, DUP_IPV6_64f, 1);
        if (!part) {
            part = TCAM_PARTITION_FWD_IP6U;
            dup_part = TCAM_PARTITION_FWD_IP6U_DUP;
        }
    } 
    if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
        soc_reg_field_set(unit, ETU_CONFIG0r, &tmp2, DUP_IPV6_128f, 1);
        if (!part) {
            part = TCAM_PARTITION_FWD_IP6_128_UCAST;
            dup_part = TCAM_PARTITION_FWD_IP6_DUP;
        }
    }
    if (part) {
        dup_db_offset = partitions[dup_part].tcam_base - 
                        partitions[part].tcam_base;
        dup_db_offset >>= 12;
    }
    soc_reg_field_set(unit, ETU_CONFIG0r, &tmp2, DUP_BLK_OFFSETf, 
                       dup_db_offset);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG0r(unit, tmp2));
    return SOC_E_NONE;
}

#ifdef for_later
/* given a type of lookup, this function returns the appropriate field
 * mapper entry offset to be programmed into the search profile table
 */
/* 1. If lookup type is L2, then
      if any of the L2 ACL partitions is non0 then return that FM entry.
      else return FM offset of L2 only entry.
   2. If lookup type is IPV4 then if any of the V4 ACL partitions in non0
      then return the FM entry for the first of these.
      else return the FM offset of V4 only entry.
   3. Same fo V6. with following difference: if v6_64 is configured, then
      consider only ACLIP6C partition. If v6_128 is configured then
      consider all the ACL IP6 partitions.

    This is only one behaviour for one section of the SEARCH_PROFILE table
    indexed by src_trunk_map index 1. Index 0 is non-ESM. Other behaviours
    can be define at other offsets (e.g, other ACLs for L2/L3) in this
   src_trunk_map section of the SEARCH_PROFILE table. Similarly other 
    behaviours can be defined for other sources (other than src_trunk_map).
*/
int _soc_get_fieldmapper_offset(int unit, int lookup_type)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int part, default_offset;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    
    partitions = tcam_info->partitions;
    switch (lookup_type) {
    case FM_LOOKUP_TYPE_L2:
        default_offset = KEY_GEN_OFFSET_L2;
        if (partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
            default_offset = KEY_GEN_OFFSET_TR2_ACL_L2C;
        } else {
            default_offset = KEY_GEN_OFFSET_TR2_ACL_L2;
        }
        break;
    case FM_LOOKUP_TYPE_IPV4:
        default_offset = KEY_GEN_OFFSET_IPV4_ONLY;
        if (partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
            default_offset = KEY_GEN_OFFSET_TR2_ACL_IP4C;
        } else if (partitions[TCAM_PARTITION_ACL_IP4].num_entries) {
            default_offset = KEY_GEN_OFFSET_TR2_ACL_IP4;
        } else if (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
            default_offset = KEY_GEN_OFFSET_TR2_ACL_L2IP4;
        }
        break;
    case FM_LOOKUP_TYPE_IPV6:
        if (partitions[TCAM_PARTITION_FWD_IP6U].num_entries) {
            default_offset = KEY_GEN_OFFSET_IPV6_64_ONLY;
            if (partitions[TCAM_PARTITION_ACL_IP6C].num_entries) {
                default_offset = KEY_GEN_OFFSET_TR2_ACL_IP6C;
            }
        } else {
            default_offset = KEY_GEN_OFFSET_IPV6_128_ONLY;
            if (partitions[TCAM_PARTITION_ACL_IP6S].num_entries) {
                default_offset = KEY_GEN_OFFSET_TR2_ACL_IP6S;
            } else if (partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
                default_offset = KEY_GEN_OFFSET_TR2_ACL_IP6F;
            } else if (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries) {
                default_offset = KEY_GEN_OFFSET_TR2_ACL_L2IP6;
            }
        }
        break;
    default:
        default_offset = KEY_GEN_OFFSET_L2;
    }
    return default_offset;
}
#endif

STATIC int
_soc_tr3_esm_init_pkt_type_tcam(int unit, soc_tcam_partition_t *partitions)
{
    esm_pkt_id_tcam_entry_t l2_entry_id;

    if (partitions == NULL) {
        return SOC_E_NONE;
    }

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_L2_VFI; /* L2 + SVP valid */
    l2_entry_id.l2_lkup_type_vlan_or_vfi = 1;
    l2_entry_id.l2_payload = 1;
    l2_entry_id.valid = 1;
    l2_entry_id.tunnel_and_lpbk_type = 2;
    l2_entry_id.mask[1] = 0x0; 
    l2_entry_id.mask[0] = 0x410000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 1, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_L2_VLAN; /* L2 + SGLP valid */
    l2_entry_id.l2_lkup_type_vlan_or_vfi = 0;
    l2_entry_id.l2_payload = 1;
    l2_entry_id.valid = 1;
    l2_entry_id.mask[1] = 0x40; 
    l2_entry_id.mask[0] = 0x10000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 2, &l2_entry_id));

    /* create an SVP valid variant */    
    l2_entry_id.pkt_type = FWD_TYPE_L2_VFI; /* L2 + SVP valid */
    l2_entry_id.l2_lkup_type_vlan_or_vfi = 1;  
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 3, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_L2_VFI; /* L2 + SVP valid */
    l2_entry_id.l2_lkup_type_vlan_or_vfi = 1;
    l2_entry_id.l2_payload = 0;
    l2_entry_id.valid = 1;
    l2_entry_id.tunnel_and_lpbk_type = 2;
    l2_entry_id.mask[1] = 0x40;
    l2_entry_id.mask[0] = 0x410000;
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 4, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_V4_BRIDGE; /* IPV4 l2 switching */
    l2_entry_id.l2_payload = 0;
    l2_entry_id.l3_type = 0;
    l2_entry_id.valid = 1;
    l2_entry_id.mask[1] = 0; 
    l2_entry_id.mask[0] = 0x8190000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 5, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_V6_BRIDGE; /* V6 l2 switching */
    l2_entry_id.l2_payload = 0;
    l2_entry_id.l3_type = 4;
    l2_entry_id.valid = 1;
    l2_entry_id.mask[1] = 0; 
    l2_entry_id.mask[0] = 0x8190000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 6, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_V4_ROUTE; /* IPV4 */
    l2_entry_id.l2_payload = 0;
    l2_entry_id.l3_type = 0;
    l2_entry_id.valid = 1;
    l2_entry_id.my_stn_hit_and_enabled = 1;
    l2_entry_id.mask[1] = 0; 
    l2_entry_id.mask[0] = 0x8190000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 7, &l2_entry_id));

    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_V6_ROUTE; /* V6 128 */
    l2_entry_id.l2_payload = 0;
    l2_entry_id.l3_type = 4;
    l2_entry_id.valid = 1;
    l2_entry_id.my_stn_hit_and_enabled = 1;
    l2_entry_id.mask[1] = 0; 
    l2_entry_id.mask[0] = 0x8190000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 8, &l2_entry_id));
    
    sal_memset(&l2_entry_id, 0, sizeof(l2_entry_id));
    l2_entry_id.pkt_type = FWD_TYPE_L2_VLAN; /* L2 + SGLP valid */
    l2_entry_id.l2_lkup_type_vlan_or_vfi = 0;
    l2_entry_id.l2_payload = 0;
    l2_entry_id.valid = 1;
    l2_entry_id.mask[1] = 0x40; 
    l2_entry_id.mask[0] = 0x10000; 
    SOC_IF_ERROR_RETURN(soc_set_pkt_id_tcam_entry(unit, 9, &l2_entry_id));
    
    return SOC_E_NONE;
}

/* populate the search profile table for the various sources. Currently
   only src_trunk_map source, 2 profiles are populated. There are 6 more
   profiles avail. Other sources are all available
*/
STATIC int 
_soc_tr3_esm_fill_search_profile(int unit) 
{

    int rv;
    /* profile 0, esm ineligible, which is also default */
    rv = soc_set_esm_search_profile_range(unit, SRCH_PROFILE_SRC_TRUNK_MAP_0, 
                                         SRCH_PROFILE_SRC_TRUNK_MAP_0+31, 0, 0);
    SOC_IF_ERROR_RETURN(rv);

    /* profile 1 for SRC_TRUNK_MAP: */
    rv = soc_set_esm_search_profile_range(unit, SRCH_PROFILE_SRC_TRUNK_MAP_1, 
                                         SRCH_PROFILE_SRC_TRUNK_MAP_1, 0, 0);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                               SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VLAN, 
                               SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_L2_VLAN, 1, 
                               KEY_GEN_OFFSET_L2);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VFI, 
                  SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_L2_VFI, 1, KEY_GEN_OFFSET_L2);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_ROUTE, 
           SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_V4_ROUTE, 1, KEY_GEN_OFFSET_IPV4_SA);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_BRIDGE, 
               SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_V4_BRIDGE, 1, KEY_GEN_OFFSET_L2);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
       SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_V6_ROUTE, 1, KEY_GEN_OFFSET_IPV6_128_SA);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_BRIDGE, 
             SRCH_PROFILE_SRC_TRUNK_MAP_1+FWD_TYPE_V6_BRIDGE, 1, KEY_GEN_OFFSET_L2);
    SOC_IF_ERROR_RETURN(rv);

    /*
     * ESM search profile for KEY_GEN_OFFSET_TR3_WAR_L2SA_ONLY is created for
     * Source Mac based  ESM lookup thorugh VFP Action
     * bcmFieldActionEsmSearchKeySrcMac purpose
     */
    rv = soc_set_esm_search_profile_range(unit,
                                      SRCH_PROFILE_SRC_VFP_BASE + 32 + FWD_TYPE_L2_VLAN,
                                      SRCH_PROFILE_SRC_VFP_BASE + 32 + FWD_TYPE_L2_VLAN, 1,
                                      KEY_GEN_OFFSET_TR3_WAR_L2SA_ONLY);

    return rv;
}

int 
soc_tr3_esm_pkt_type_to_slice_map_set(int unit, int pkt_type, int slice)
{
    int offset1, offset2, fwd_type1, fwd_type2, rv;
    int fwd_ipv4_present, fwd_l2_present, fwd_ipv6_present;
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;

#ifdef LATER
    int fwd_ipv6u_present;
#endif

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }

    partitions = tcam_info->partitions;

    fwd_l2_present = partitions[TCAM_PARTITION_FWD_L2].num_entries ||
                     partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries;
    fwd_ipv4_present = partitions[TCAM_PARTITION_FWD_IP4].num_entries ||
                       partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries ||
                      partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries;
    fwd_ipv6_present = partitions[TCAM_PARTITION_FWD_IP6].num_entries ||
                   partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries ||
                 partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries;

    switch (slice) {
    case TCAM_PARTITION_ACL_L2C:
        if(!fwd_l2_present) {
            offset1 = offset2 = KEY_GEN_OFFSET_TR2_ACL_L2C_ONLY;
        } else {
            offset1 = offset2 = KEY_GEN_OFFSET_TR2_ACL_L2C;
        }
        break;       
    case TCAM_PARTITION_ACL_L2: 
        if (!fwd_l2_present) {
            /* L2 ACL only */ 
            offset1 = offset2 = KEY_GEN_OFFSET_TR2_ACL_L2_ONLY;
        } else if (tcam_info->mode) {
            offset1 = KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SGLP;
            offset2 = KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SVP;
        } else {
            offset1 = KEY_GEN_OFFSET_TR2_ACL_L2_7K_SGLP;
            offset2 = KEY_GEN_OFFSET_TR2_ACL_L2_7K_SVP;
        }
        break;       
    case TCAM_PARTITION_ACL_IP4C:
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_IP4C : 
                  KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP4C_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY;
        break;       
    case TCAM_PARTITION_ACL_IP4: 
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_IP4 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP4_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY;
        break;       
    case TCAM_PARTITION_ACL_L2IP4:
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP4 : 
                  KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP4_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY;
        break;       
    case TCAM_PARTITION_ACL_IP6C:
        offset1 = fwd_ipv6_present ? KEY_GEN_OFFSET_TR2_ACL_IP6C : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6C_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY;
        break;       
    case TCAM_PARTITION_ACL_IP6S:
        offset1 = fwd_ipv6_present ? KEY_GEN_OFFSET_TR2_ACL_IP6S : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6S_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY;
        break;       
    case TCAM_PARTITION_ACL_IP6F:
        offset1 = fwd_ipv6_present ? KEY_GEN_OFFSET_TR2_ACL_IP6F : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6F_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY;
        break;       
    case TCAM_PARTITION_ACL_L2IP6:
        offset1 = fwd_ipv6_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP6 : 
                  KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2 : 
                  KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY;
        break;       
    default:
        return SOC_E_PARAM;
    }

    switch (pkt_type) {
    case PKT_TYPE_L2:
        fwd_type1 = FWD_TYPE_L2_VLAN;
        fwd_type2 = FWD_TYPE_L2_VFI;
        break;
    case PKT_TYPE_IPV4:
        fwd_type1 = FWD_TYPE_V4_ROUTE;
        fwd_type2 = FWD_TYPE_V4_BRIDGE;
        break;
    case PKT_TYPE_IPV6:
        fwd_type1 = FWD_TYPE_V6_ROUTE;
        fwd_type2 = FWD_TYPE_V6_BRIDGE;
        break;
    default: 
        return SOC_E_PARAM;
    }

    rv = soc_set_esm_search_profile_range(unit, 
                                        SRCH_PROFILE_SRC_TRUNK_MAP_1+fwd_type1, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1+fwd_type1, 1, offset1);
    SOC_IF_ERROR_RETURN(rv);
    rv = soc_set_esm_search_profile_range(unit, 
                                        SRCH_PROFILE_SRC_TRUNK_MAP_1+fwd_type2, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1+fwd_type2, 1, offset2);
    SOC_IF_ERROR_RETURN(rv);
    return SOC_E_NONE;
}

/*
 * Forwarding key value encoding:
 *   L2 (always 80-bit)
 *   IP4 (always 80-bit)
 *   IP6 (0:80-bit, 1:160-bit)
 * ACL key value encoding:
 *   L2 (0:disable, 1:320-bit, 2:160-bit)
 *   IP4 (0:disable, 1:320-bit, 2:160-bit, 3:L2+IP4, 4:L2)
 *   IP6 (0:disable, 1:480-bit, 2:480-bit, 3:160-bit, 4:L2+IP6, 5:L2)
 */
STATIC int
_soc_tr3_esm_init_set_esm_mode_per_port(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int fwd_ip6_key, acl_l2_key, acl_ip4_key, acl_ip6_key;
    int rv;
    int offset1, offset2, l2_fm_offset1 = KEY_GEN_OFFSET_TR2_ACL_L2_7K_SGLP;
    int l2_fm_offset2 = KEY_GEN_OFFSET_TR2_ACL_L2_7K_SVP;
    int fwd_l2_present, fwd_ipv4_present, fwd_ipv6_present;
    int fwd_ipv6u_present;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d: tr3_set_esm_mode_per_port: No tcam entries\n"), 
                 unit));
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;

    fwd_l2_present = partitions[TCAM_PARTITION_FWD_L2].num_entries ||
                     partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries;
    fwd_ipv4_present = partitions[TCAM_PARTITION_FWD_IP4].num_entries ||
                     partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries ||
                     partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries;
    fwd_ipv6_present = partitions[TCAM_PARTITION_FWD_IP6].num_entries ||
                   partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries ||
                  partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries;
    fwd_ipv6u_present = partitions[TCAM_PARTITION_FWD_IP6U].num_entries;

    fwd_ip6_key = soc_property_get(unit, spn_EXT_IP6_FWD_KEY, -1);
    acl_l2_key = soc_property_get(unit, spn_EXT_L2_ACL_KEY, -1);
    acl_ip4_key = soc_property_get(unit, spn_EXT_IP4_ACL_KEY, -1);
    acl_ip6_key = soc_property_get(unit, spn_EXT_IP6_ACL_KEY, -1);

    /* IP6 FWD key is not specified in config, figure it out from table size */
    if (fwd_ip6_key == -1) {
        fwd_ip6_key = 0; /* default 72-bit key (prefix length 64) */
        if (!partitions[TCAM_PARTITION_FWD_IP6U].num_entries) {
            if (partitions[TCAM_PARTITION_FWD_IP6].num_entries) {
                fwd_ip6_key = 1; /* 144-bit key (prefix length 128) */
            }
        }
    }

    /* L2 ACL key is not specified in config, figure it out from table size */
    if (acl_l2_key == -1) { /* L2 ACL key not specified in config */
        acl_l2_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_l2_key = 1; /* 288-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
            acl_l2_key = 2; /* 144-bit key */
        }
    }

    /* IP4 ACL key is not specified in config, figure it out from table size */
    if (acl_ip4_key == -1) { /* IP4 ACL key not specified in config */
        acl_ip4_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_IP4].num_entries) {
            acl_ip4_key = 1; /* 288-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
            acl_ip4_key = 2; /* 144-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
            acl_ip4_key = 3; /* 432-bit L2+L3 key */
        } else if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_ip4_key = 4; /* 288-bit L2 key */
        }
    }

    /* IP6 ACL key is not specified in config, figure it out from table size */
    if (acl_ip6_key == -1) { /* IP6 ACL key not specified in config */
        acl_ip6_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_IP6S].num_entries) {
            acl_ip6_key = 1; /* 360-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
            acl_ip6_key = 2; /* 432-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP6C].num_entries) {
            acl_ip6_key = 3; /* 144-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries) {
            acl_ip6_key = 4; /* 432-bit L2+L3 key */
        } else if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_ip6_key = 5; /* 288-bit L2 key */
        }
    }

    if (fwd_ip6_key < 0 || fwd_ip6_key > 1 ||
        acl_l2_key < 0 || acl_l2_key > 2 ||
        acl_ip4_key < 0 || acl_ip4_key > 4 ||
        acl_ip6_key < 0 || acl_ip6_key > 5) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "ESM init: unit %d incorrect key selection\n"),
                              unit));
        return SOC_E_PARAM;
    }

    if (tcam_info->mode) { /* 350 MHz */
        if (acl_ip6_key == 1 || acl_ip6_key == 2) {
            acl_ip6_key = 3;
        }
        l2_fm_offset1 = KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SGLP;
        l2_fm_offset2 = KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SVP;
    }

    if (fwd_ip6_key) {
        if (fwd_l2_present) {
            /* If L2 present, add SA to V6-128*/
            rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 1,
                            KEY_GEN_OFFSET_IPV6_128_SA);
        } else {
            /* set V6 lookup type to V6-128 */
            rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 1,
                            KEY_GEN_OFFSET_IPV6_128_ONLY);
        }
        SOC_IF_ERROR_RETURN(rv);
    } else {
        if (fwd_l2_present) {
            /* If L2 present, add SA to V6-64*/
            rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 1,
                            KEY_GEN_OFFSET_IPV6_64_SA);
        } else {
            rv = soc_set_esm_search_profile_range(unit, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
                            SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 1,
                            KEY_GEN_OFFSET_IPV6_64_ONLY);
        }
        SOC_IF_ERROR_RETURN(rv);
    }
    if (acl_l2_key) {
        if (acl_l2_key == 2) {
            /* point to acl_l2c fieldmapper */
            rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VLAN, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VLAN, 1,
                        fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_L2C :
                        KEY_GEN_OFFSET_TR2_ACL_L2C_ONLY);
            SOC_IF_ERROR_RETURN(rv);
        } else {
            /* point to acl_l2 fieldmapper */
            rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VLAN, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VLAN, 1,
                        fwd_l2_present ? l2_fm_offset1 : 
                        KEY_GEN_OFFSET_TR2_ACL_L2_ONLY);
            SOC_IF_ERROR_RETURN(rv);
            rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VFI, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_L2_VFI, 1,
                        fwd_l2_present ? l2_fm_offset2 :
                        KEY_GEN_OFFSET_TR2_ACL_L2_ONLY);
            SOC_IF_ERROR_RETURN(rv);
        }
    }
    switch (acl_ip4_key) {
    case 1: /* ACL_IP4, 288-bit */
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_IP4 :
                 KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP4_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY;
        break;
    case 2: /* ACL_IP4C, 144-bit */
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_IP4C :
                 KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP4C_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY;
        break;
    case 3: /* ACL_L2IP4, 432-bit */
        offset1 = fwd_ipv4_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP4 :
                 KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP4_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY;
        break;
    case 4: /* ACL_L2 or ACL_L2C */
        
        offset1 = -1; 
        /* pick the correct L2 ACL variant instead of forcing 7K */
        offset2 = l2_fm_offset1;
        break;
    default:
        offset1 = offset2 = -1;
        break;
    }
    if (offset1 > 0) {  
        rv = soc_set_esm_search_profile_range(unit, 
                         SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_ROUTE, 
                         SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_ROUTE, 1,
                         offset1);
        SOC_IF_ERROR_RETURN(rv);
    }
    if (offset2 > 0) {  
        rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_BRIDGE, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V4_BRIDGE, 1,
                        offset2);
        SOC_IF_ERROR_RETURN(rv);
    }
    switch (acl_ip6_key) {
    case 1: /* ACL_IP6S, 360-bit */
        offset1 = (!fwd_ipv6_present && !fwd_ipv6u_present) ? 
                   KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY :
                   (fwd_ip6_key ? KEY_GEN_OFFSET_TR2_ACL_IP6S :
                    KEY_GEN_OFFSET_TR2_ACL_IP6S_IP6U_FWD);
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6S_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY;
        break;
    case 2: /* ACL_IP6F, 432-bit */
        offset1 = (!fwd_ipv6_present && !fwd_ipv6u_present) ? 
                   KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY :
                   (fwd_ip6_key ? KEY_GEN_OFFSET_TR2_ACL_IP6F :
                    KEY_GEN_OFFSET_TR2_ACL_IP6F_IP6U_FWD);
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6F_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY;
        break;
    case 3: /* ACL_IP6C, 144-bit */
        offset1 = (!fwd_ipv6_present && !fwd_ipv6u_present) ? 
                   KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY :
                   (fwd_ip6_key ? KEY_GEN_OFFSET_TR2_ACL_IP6C :
                    KEY_GEN_OFFSET_TR2_ACL_IP6C_IP6U_FWD);
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_IP6C_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY;
        break;
    case 4: /* ACL_L2IP6, 432-bit */
        offset1 = fwd_ip6_key ? KEY_GEN_OFFSET_TR2_ACL_L2IP6 :
                 KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY;
        offset2 = fwd_l2_present ? KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2 :
                 KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY;
        break;
    case 5: /* ACL_L2 or ACL_L2C */
        offset1 = -1; offset2 = l2_fm_offset1;
        break;
    default:
        offset1 = -1;
        offset2 = -1;
        break;
    }
    if (offset1 > 0) {
        rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_ROUTE, 1,
                        offset1);
        SOC_IF_ERROR_RETURN(rv);
    }
    if (offset2 > 0) {
        rv = soc_set_esm_search_profile_range(unit, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_BRIDGE, 
                        SRCH_PROFILE_SRC_TRUNK_MAP_1 + FWD_TYPE_V6_BRIDGE, 1,
                        offset2);
        SOC_IF_ERROR_RETURN(rv);
    }
    return SOC_E_NONE;
}

#if 1

STATIC int 
_soc_triumph3_esm_init_set_et_pa_xlate(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int i, rv, hbit_base, order_idx, mem_rv=0;
    int part, dup_part, index, entries_per_blk, blks_per_part;
    int hbit_avail=1;
    int db_duplicated=0;
    uint32 dup_db_offset = 0;
    uint32 ism_bank_size_l2[20], max_hit_bits, tmp;
    uint32 ism_bank_size_l3[20], ism_bank_size_acl[20], *ism_bank_size=0;
    uint8 ism_banks_l2[20], ism_bank_count_l2;
    uint8 ism_banks_l3[20], ism_bank_count_l3;
    uint8 ism_banks_acl[20], ism_bank_count_acl, ism_bank_count, *ism_banks = 0;
    int cur_col;
    uint32 cur_bank, cur_row;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;
    SOC_IF_ERROR_RETURN(READ_ETU_CONFIG0r(unit, &tmp));
    if (soc_reg_field_get(unit, ETU_CONFIG0r, tmp, EN_LOCAL_SA_HBITf)) {
        max_hit_bits = 512*1024;
    } else {
        max_hit_bits = 1024*1024;
    }

    hbit_base = 0;

    rv = soc_ism_get_banks(unit, SOC_ISM_MEM_ESM_L2, ism_banks_l2, ism_bank_size_l2,
                           &ism_bank_count_l2);
#ifdef testing
    ism_banks_l3[0] = ism_banks_l2[5];
    ism_banks_l3[1] = ism_banks_l2[6];
    ism_banks_l3[2] = ism_banks_l2[7];
    ism_banks_l3[3] = ism_banks_l2[8];
    ism_banks_l3[4] = ism_banks_l2[9];
    ism_banks_acl[0] = ism_banks_l2[10];
    ism_banks_acl[1] = ism_banks_l2[11];
    ism_banks_acl[2] = ism_banks_l2[12];
    ism_banks_acl[3] = ism_banks_l2[13];
    ism_banks_acl[4] = ism_banks_l2[14];
    ism_bank_count_l2 = ism_bank_count_l3 = 5;
    ism_bank_count_acl = 10;
    ism_bank_size_l3[0] = ism_bank_size_l2[5];
    ism_bank_size_l3[1] = ism_bank_size_l2[6];
    ism_bank_size_l3[2] = ism_bank_size_l2[7];
    ism_bank_size_l3[3] = ism_bank_size_l2[8];
    ism_bank_size_l3[4] = ism_bank_size_l2[9];
    ism_bank_size_acl[0] = ism_bank_size_l2[10];
    ism_bank_size_acl[1] = ism_bank_size_l2[11];
    ism_bank_size_acl[2] = ism_bank_size_l2[12];
    ism_bank_size_acl[3] = ism_bank_size_l2[13];
    ism_bank_size_acl[4] = ism_bank_size_l2[14];
#else
    if (rv < 0) return SOC_E_FAIL;
    rv = soc_ism_get_banks(unit, SOC_ISM_MEM_ESM_L3, ism_banks_l3, ism_bank_size_l3,
                           &ism_bank_count_l3);
    if (rv < 0) return SOC_E_FAIL;
    rv = soc_ism_get_banks(unit, SOC_ISM_MEM_ESM_ACL, ism_banks_acl, ism_bank_size_acl, 
                           &ism_bank_count_acl);
    if (rv < 0) return SOC_E_FAIL;
#endif

    /* calculate duplicate database offset if there is one */
    part = 0;
    dup_part = 0;
    if (partitions[TCAM_PARTITION_FWD_L2_DUP].num_entries) {
        part = TCAM_PARTITION_FWD_L2;
        dup_part = TCAM_PARTITION_FWD_L2_DUP;
    } else if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
        part = TCAM_PARTITION_FWD_IP4_UCAST;
        dup_part = TCAM_PARTITION_FWD_IP4_DUP;
    } else if (partitions[TCAM_PARTITION_FWD_IP6U_DUP].num_entries) {
        part = TCAM_PARTITION_FWD_IP6U;
        dup_part = TCAM_PARTITION_FWD_IP6U_DUP;
    } else if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
        part = TCAM_PARTITION_FWD_IP6_128_UCAST;
        dup_part = TCAM_PARTITION_FWD_IP6_DUP;
    }
    if (part) {
        dup_db_offset = partitions[dup_part].tcam_base - 
                        partitions[part].tcam_base;
        dup_db_offset >>= 12;
    }

    cur_row = 0;
    cur_col = 11;
    cur_bank = 0;
    /*
     * go in reverse order so that row addresses within a bank will only go
     * in increasing order
     */
    for (order_idx = sizeof(tr3_tcam_partition_order)/sizeof(uint32)-1; 
         order_idx >= 0; order_idx--) {
        part = tr3_tcam_partition_order[order_idx];
        if ((part == 0) || (!partitions[part].num_entries) || 
            (partitions[part].flags & TCAM_PARTITION_FLAGS_NO_AD)) {
            db_duplicated = 0;
            continue;
        }
        switch (partitions[part].flags & TCAM_PARTITION_FLAGS_TYPE_MASK) {
        case TCAM_PARTITION_FLAGS_TYPE_ACL:
            /* acl partitions are assumed to be always last in the partition order */
            ism_banks = ism_banks_acl;
            ism_bank_size = ism_bank_size_acl;
            ism_bank_count = ism_bank_count_acl;
            db_duplicated = 0;
            break;
        case TCAM_PARTITION_FLAGS_TYPE_L3:
            if (ism_banks != ism_banks_l3) {
                cur_row = 0; 
                cur_col = 11;
                cur_bank = 0;
            }
            ism_banks = ism_banks_l3;
            ism_bank_size = ism_bank_size_l3;
            ism_bank_count = ism_bank_count_l3;
            switch (part) {
            case TCAM_PARTITION_FWD_IP4_UCAST:
            case TCAM_PARTITION_FWD_IP4_UCAST_WIDE:
            case TCAM_PARTITION_FWD_IP4:
                if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
                    db_duplicated = 1;
                } else {
                    db_duplicated = 0;
                }
                break;
            case TCAM_PARTITION_FWD_IP6U:
                if (partitions[TCAM_PARTITION_FWD_IP6U_DUP].num_entries) {
                    db_duplicated = 1;
                } else {
                    db_duplicated = 0;
                }
                break;
            case TCAM_PARTITION_FWD_IP6_128_UCAST:
            case TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE:
            case TCAM_PARTITION_FWD_IP6:
                if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
                    db_duplicated = 1;
                } else {
                    db_duplicated = 0;
                }
                break;
            default: 
                db_duplicated = 0;
                break;
            }
            break;
        case TCAM_PARTITION_FLAGS_TYPE_L2:
            if (ism_banks != ism_banks_l2) {
                cur_row = 0; 
                cur_col = 11;
                cur_bank = 0;
            }
            ism_banks = ism_banks_l2;
            ism_bank_size = ism_bank_size_l2;
            ism_bank_count = ism_bank_count_l2;
            if (partitions[TCAM_PARTITION_FWD_L2_DUP].num_entries) {
                db_duplicated = 1;
            } else {
                db_duplicated = 0;
            }
            break;
        default:
            return SOC_E_PARAM;
        }

        entries_per_blk = 4096 >> partitions[part].ad_info.et_width;

        blks_per_part = (partitions[part].num_entries + 
                         entries_per_blk - 1)/entries_per_blk;

        /* get index into ET_PA_XLAT table */
        index = partitions[part].tcam_base >> 12;

        for (i = 0; i < blks_per_part; i++) {
            /* configure hbit pa */
            if (partitions[part].ad_info.hit_bit_enable) {
                if (hbit_avail) {
                    partitions[part].ad_info.hit_bit_pa_base = hbit_base;
                    partitions[part].ad_info.hit_bit_enable = 1;
                    hbit_base += (2 - partitions[part].tcam_width_shift);
                } else {
                    partitions[part].ad_info.hit_bit_enable = 0;
                }
                if ((hbit_base << 11) >= max_hit_bits) {
                    /* flag error */
                    LOG_CLI((BSL_META_U(unit,
                                        "unit %d: ESM max no. of hit bits (%d) reached."),
                             unit, max_hit_bits));
                    LOG_CLI((BSL_META_U(unit,
                                        "Some entries may not have hit bits\n")));
                    hbit_avail = 0;
                }
            }
            
            if (i > 0) {
               SOC_TR3_INCR_AD_ADDR(cur_row, cur_col, cur_bank,
                                    partitions[part].ad_info.ad_width + 1, mem_rv);
            }

            /* check return value of ad increment here since we should check the
             * value only if more mem is needed
             */
            if (!SOC_SUCCESS(mem_rv)) {
               LOG_CLI((BSL_META_U(unit,
                                   "unit %d: No more et_pa_xlat memory. "
                                   "Partition = %d\n"), unit, part));
                return mem_rv;
            }

            /* wrap to new column if current column space may not be enough */
            if ((cur_col + 1) < (partitions[part].ad_info.ad_width + 1)) {
                SOC_TR3_INCR_AD_ADDR(cur_row, cur_col, cur_bank, cur_col + 1,mem_rv);
            } else if (partitions[part].ad_info.ad_width == 5) {
                /* for max width might still have to increment */
                if ((cur_col+1) % (partitions[part].ad_info.ad_width + 1))  {
                    /* cur_col != 11 or 5 */
                    if (cur_col > 8) {
                        /* fit starting to col. no. 8 */
                        SOC_TR3_INCR_AD_ADDR(cur_row, cur_col, cur_bank, 
                                            cur_col - 8, mem_rv);
                    } else if ((cur_col < 8) && (cur_col > 6)) {
                        SOC_TR3_INCR_AD_ADDR(cur_row, cur_col, cur_bank, 
                                            cur_col - 6, mem_rv);
                    }
                }
            } 

            if (!SOC_SUCCESS(mem_rv)) {
               LOG_CLI((BSL_META_U(unit,
                                   "unit %d: Could not get more et_pa_xlat memory. "
                                   "Partition = %d\n"), unit, part));
                return mem_rv;
            }

            /* configure ad_ba, ad_ra_base, ad_ca */
            partitions[part].ad_info.ad_ba = ism_banks[cur_bank];
            partitions[part].ad_info.ad_ra_base = cur_row;
            partitions[part].ad_info.ad_ca = cur_col;

            rv = _soc_write_et_pa_xlat_entry(unit, index + i, 
                             &partitions[part].ad_info);

            if (!SOC_SUCCESS(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    "unit %d: Failure writing et_pa_xlat entry. "
                                    "Partition = %d\n"), unit, part));
                return rv;

            }
            if (db_duplicated) {
                rv = _soc_write_et_pa_xlat_entry(unit, 
                                                dup_db_offset + index + i, 
                                                &partitions[part].ad_info);
                if (!SOC_SUCCESS(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "unit %d: Failure writing duplicated "
                                        "db et_pa_xlat entry. Partition = %d\n"),
                             unit, part));
                    return rv;

                }
            }
        }
    }
    return SOC_E_NONE;
}
#endif

int
_soc_triumph3_esm_init_adjust_mem_size(int unit)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int policy_width;

    sop = SOC_PERSIST(unit);
    memState = sop->memState;
    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "unit: %d No external tables configured\n"), unit));
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;

    /* L2 forwarding partition */
    memState[EXT_L2_ENTRY_1m].index_max = /* TCAM + SRAM */
        memState[EXT_L2_ENTRY_TCAMm].index_max = /* TCAM */
        memState[EXT_L2_ENTRY_DATA_ONLYm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_L2].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_L2m].index_max = 
        memState[EXT_LOC_SRC_HIT_BITS_L2m].index_max = 
        memState[EXT_DST_HIT_BITS_L2m].index_max = 
        memState[EXT_TCAM_VBIT_L2_ENTRY_1m].index_max = 
        (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 - 1;

    memState[EXT_L2_ENTRY_2m].index_max = /* TCAM + SRAM */
        memState[EXT_L2_ENTRY_TCAM_WIDEm].index_max = /* TCAM */
        memState[EXT_L2_ENTRY_DATA_ONLY_WIDEm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_L2_WIDEm].index_max = 
        memState[EXT_LOC_SRC_HIT_BITS_L2_WIDEm].index_max = 
        memState[EXT_DST_HIT_BITS_L2_WIDEm].index_max = 
        memState[EXT_TCAM_VBIT_L2_ENTRY_2m].index_max = /* TCAM */
        (partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries + 31) / 32 - 1;

    /* IP4 forwarding partition */
    memState[EXT_IPV4_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV4_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV4m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV4m].index_max = 
        memState[EXT_DST_HIT_BITS_IPV4m].index_max = 
        memState[EXT_TCAM_VBIT_DEFIP_IPV4m].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 - 1;

    memState[EXT_IPV4_UCASTm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV4_UCAST_TCAMm].index_max = /* TCAM + SRAM */
        memState[EXT_L3_UCAST_DATA_IPV4m].index_max = /* TCAM + SRAM */
        partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV4_UCASTm].index_max = 
        memState[EXT_DST_HIT_BITS_IPV4_UCASTm].index_max = 
        memState[EXT_TCAM_VBIT_IPV4_UCASTm].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries + 31) / 32 - 1;

    memState[EXT_IPV4_UCAST_WIDEm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV4_UCAST_WIDE_TCAMm].index_max = /* TCAM + SRAM */
        memState[EXT_L3_UCAST_DATA_WIDE_IPV4m].index_max = /* TCAM + SRAM */
        partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV4_UCAST_WIDEm].index_max = 
        memState[EXT_DST_HIT_BITS_IPV4_UCAST_WIDEm].index_max = 
        memState[EXT_TCAM_VBIT_IPV4_UCAST_WIDEm].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries + 31)/32 - 1;

    memState[EXT_IPV4_TCAMm].index_max = /* SRC hit bit */
        partitions[TCAM_PARTITION_FWD_IP4].num_entries + 
        partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries + 
        partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries - 1;

    /* IP6 64-bit prefix forwarding partition */
    memState[EXT_IPV6_64_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV6_64_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV6_64m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6U].num_entries - 1;

    memState[EXT_IPV6_64_TCAMm].index_max = /* TCAM */
        partitions[TCAM_PARTITION_FWD_IP6U].num_entries - 1;

    memState[EXT_SRC_HIT_BITS_IPV6_64m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_64m].index_max = /* DST hit bit */
        memState[EXT_TCAM_VBIT_DEFIP_IPV6_64m].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 - 1;

    /* IP6 128-bit prefix forwarding partition */
    memState[EXT_IPV6_128_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV6_128_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV6_128m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV6_128m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_128m].index_max = /* DST hit bit */
        memState[EXT_TCAM_VBIT_DEFIP_IPV6_128m].index_max =
        (partitions[TCAM_PARTITION_FWD_IP6].num_entries + 31)/32 - 1;

    memState[EXT_IPV6_128_UCASTm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV6_128_UCAST_TCAMm].index_max = /* TCAM */
        memState[EXT_L3_UCAST_DATA_IPV6_128m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV6_128_UCASTm].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_128_UCASTm].index_max = /* DST hit bit*/
        memState[EXT_TCAM_VBIT_IPV6_128_UCASTm].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries + 63)/64 - 1;

    memState[EXT_IPV6_128_UCAST_WIDEm].index_max =  /* TCAM + SRAM */
        memState[EXT_IPV6_128_UCAST_WIDE_TCAMm].index_max =  /* TCAM + SRAM */
        memState[EXT_L3_UCAST_DATA_WIDE_IPV6_128m].index_max =  /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV6_128_UCAST_WIDEm].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_128_UCAST_WIDEm].index_max=/* DST hitbit*/
        memState[EXT_TCAM_VBIT_IPV6_128_UCAST_WIDEm].index_max = 
        (partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries+63/64)-1;

    memState[EXT_IPV6_128_TCAMm].index_max = /* TCAM */
        partitions[TCAM_PARTITION_FWD_IP6].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries - 1;

    /* L2 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_L2m].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_L2].num_entries - 1;
    policy_width = soc_property_get(unit, spn_EXT_L2_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL288_L2_1Xm, 
                                  EXT_FP_POLICY_ACL288_L2_2Xm, 
                                  EXT_FP_POLICY_ACL288_L2_3Xm, 
                                  EXT_FP_POLICY_ACL288_L2_4Xm, 
                                  EXT_FP_POLICY_ACL288_L2_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_L2].num_entries - 1);

    /* IP4 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_IPV4m].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_IP4].num_entries - 1;
    policy_width =soc_property_get(unit, spn_EXT_IP4_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL288_IPV4_1Xm, 
                                  EXT_FP_POLICY_ACL288_IPV4_2Xm, 
                                  EXT_FP_POLICY_ACL288_IPV4_3Xm, 
                                  EXT_FP_POLICY_ACL288_IPV4_4Xm, 
                                  EXT_FP_POLICY_ACL288_IPV4_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_IP4].num_entries - 1);

    /* IP6 short form 360-bit ACL partition */
    memState[EXT_ACL360_TCAM_DATA_IPV6_SHORTm].index_max = /* TCAM data */
        partitions[TCAM_PARTITION_ACL_IP6S].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_IP6S_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL360_IPV6_SHORT_1Xm, 
                                  EXT_FP_POLICY_ACL360_IPV6_SHORT_2Xm, 
                                  EXT_FP_POLICY_ACL360_IPV6_SHORT_3Xm, 
                                  EXT_FP_POLICY_ACL360_IPV6_SHORT_4Xm, 
                                  EXT_FP_POLICY_ACL360_IPV6_SHORT_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_IP6S].num_entries - 1);

    /* IP6 full form 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_IPV6_LONGm].index_max = /* TCAM data */
        partitions[TCAM_PARTITION_ACL_IP6F].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_IP6F_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL432_IPV6_LONG_1Xm, 
                                  EXT_FP_POLICY_ACL432_IPV6_LONG_2Xm, 
                                  EXT_FP_POLICY_ACL432_IPV6_LONG_3Xm, 
                                  EXT_FP_POLICY_ACL432_IPV6_LONG_4Xm, 
                                  EXT_FP_POLICY_ACL432_IPV6_LONG_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_IP6F].num_entries - 1);

    /* L2 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_L2m].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_L2C].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_L2C_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL144_L2_1Xm, 
                                  EXT_FP_POLICY_ACL144_L2_2Xm, 
                                  EXT_FP_POLICY_ACL144_L2_3Xm, 
                                  EXT_FP_POLICY_ACL144_L2_4Xm, 
                                  EXT_FP_POLICY_ACL144_L2_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_L2C].num_entries - 1);

    /* IP4 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV4m].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_IP4C].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_IP4C_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL144_IPV4_1Xm, 
                                  EXT_FP_POLICY_ACL144_IPV4_2Xm, 
                                  EXT_FP_POLICY_ACL144_IPV4_3Xm, 
                                  EXT_FP_POLICY_ACL144_IPV4_4Xm, 
                                  EXT_FP_POLICY_ACL144_IPV4_6Xm, 
                                  policy_width, 
                            partitions[TCAM_PARTITION_ACL_IP4C].num_entries - 1);

    /* IP6 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV6m].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_IP6C].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_IP6C_ACL_TABLE_POLICY_WIDTH, 0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL144_IPV6_1Xm, 
                                  EXT_FP_POLICY_ACL144_IPV6_2Xm, 
                                  EXT_FP_POLICY_ACL144_IPV6_3Xm, 
                                  EXT_FP_POLICY_ACL144_IPV6_4Xm, 
                                  EXT_FP_POLICY_ACL144_IPV6_6Xm, 
                                  policy_width, 
                           partitions[TCAM_PARTITION_ACL_IP6C].num_entries - 1);

    /* L2+IP4 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV4m].index_max = /* TCAM data */
        memState[EXT_FP_POLICY_ACL432_L2_IPV4m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL_L2IP4].num_entries - 1;
    policy_width=soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_POLICY_WIDTH,0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL432_L2_IPV4_1Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV4_2Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV4_3Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV4_4Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV4_6Xm, 
                                  policy_width, 
                          partitions[TCAM_PARTITION_ACL_L2IP4].num_entries - 1);

    /* L2+IP6 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV6m].index_max = /* TCAM data */
        partitions[TCAM_PARTITION_ACL_L2IP6].num_entries - 1;
    policy_width = soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_POLICY_WIDTH,
                   0);
    SOC_TR3_SET_POLICY_TABLE_SIZE(EXT_FP_POLICY_ACL432_L2_IPV6_1Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV6_2Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV6_3Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV6_4Xm, 
                                  EXT_FP_POLICY_ACL432_L2_IPV6_6Xm, 
                                  policy_width, 
                          partitions[TCAM_PARTITION_ACL_L2IP6].num_entries - 1);

    /* 80-bit ACL partition */
    memState[EXT_ACL80_TCAMm].index_max = /* TCAM */
        memState[EXT_ACL80_FP_POLICY_1Xm].index_max = /* SRAM */
        memState[EXT_ACL80_FP_POLICY_2Xm].index_max = /* SRAM */
        memState[EXT_ACL80_FP_POLICY_3Xm].index_max = /* SRAM */
        memState[EXT_ACL80_FP_POLICY_4Xm].index_max = /* SRAM */
        memState[EXT_ACL80_FP_POLICY_6Xm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL80].num_entries - 1;
    memState[EXT_TCAM_VBIT_ACL80m].index_max = 
        (partitions[TCAM_PARTITION_ACL80].num_entries + 31 / 32) - 1;

    /* 160-bit ACL partition */
    memState[EXT_ACL160_TCAMm].index_max = /* TCAM */
        memState[EXT_ACL160_FP_POLICY_1Xm].index_max = /* SRAM */
        memState[EXT_ACL160_FP_POLICY_2Xm].index_max = /* SRAM */
        memState[EXT_ACL160_FP_POLICY_3Xm].index_max = /* SRAM */
        memState[EXT_ACL160_FP_POLICY_4Xm].index_max = /* SRAM */
        memState[EXT_ACL160_FP_POLICY_6Xm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL160].num_entries - 1;
    memState[EXT_TCAM_VBIT_ACL160m].index_max = 
        (partitions[TCAM_PARTITION_ACL160].num_entries + 31 / 32) - 1;

    /* 320-bit ACL partition */
    memState[EXT_ACL320_TCAMm].index_max = /* TCAM */
        memState[EXT_ACL320_FP_POLICY_1Xm].index_max = /* SRAM */
        memState[EXT_ACL320_FP_POLICY_2Xm].index_max = /* SRAM */
        memState[EXT_ACL320_FP_POLICY_3Xm].index_max = /* SRAM */
        memState[EXT_ACL320_FP_POLICY_4Xm].index_max = /* SRAM */
        memState[EXT_ACL320_FP_POLICY_6Xm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL320].num_entries - 1;
    memState[EXT_TCAM_VBIT_ACL320m].index_max = 
        (partitions[TCAM_PARTITION_ACL320].num_entries + 31 / 32) - 1;

    /* 480-bit ACL partition */
    /* this is a special 1-entry memory */
    memState[EXT_ACL480_TCAM_MASKm].index_max = 0; 
    memState[EXT_ACL360_TCAM_MASKm].index_max = 0; 
    memState[EXT_ACL432_TCAM_MASKm].index_max = 0; 
    memState[EXT_ACL360_TCAM_MASK_IPV6_SHORTm].index_max = 0; 
    memState[EXT_ACL432_TCAM_MASK_IPV6_LONGm].index_max = 0; 
    memState[EXT_ACL432_TCAM_MASK_L2_IPV6m].index_max = 0; 

    memState[EXT_ACL480_TCAM_DATAm].index_max = /* TCAM */
        memState[EXT_ACL480_FP_POLICY_1Xm].index_max = /* SRAM */
        memState[EXT_ACL480_FP_POLICY_2Xm].index_max = /* SRAM */
        memState[EXT_ACL480_FP_POLICY_3Xm].index_max = /* SRAM */
        memState[EXT_ACL480_FP_POLICY_4Xm].index_max = /* SRAM */
        memState[EXT_ACL480_FP_POLICY_6Xm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL480].num_entries - 1;
    memState[EXT_TCAM_VBIT_ACL480m].index_max = 
        (partitions[TCAM_PARTITION_ACL480].num_entries + 31 / 32) - 1;

    /* All L2+L3 forwarding entries */
    memState[EXT_SRC_HBITSm].index_max = /* SRC hit bit */
    memState[EXT_DST_HBITSm].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries+31)/32-1;

    /* All L2 */
    memState[EXT_LOC_SRC_HBITSm].index_max = /* Local src hit bit */
        (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries + 31) / 32 - 1;

    /* All L3 forwarding entries */
    memState[EXT_DEFIP_DATAm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6U].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6].num_entries - 1;

    memState[EXT_L3_UCAST_DATAm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries - 1;

    memState[EXT_L3_UCAST_DATA_WIDEm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries - 1;

    /* All ACL entries */
    memState[EXT_FP_POLICY_1Xm].index_max = /* SRAM */
    memState[EXT_FP_POLICY_2Xm].index_max = /* SRAM */
    memState[EXT_FP_POLICY_3Xm].index_max = /* SRAM */
    memState[EXT_FP_POLICY_4Xm].index_max = /* SRAM */
    memState[EXT_FP_POLICY_6Xm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_ACL_L2].num_entries + 
        partitions[TCAM_PARTITION_ACL_IP4].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
        partitions[TCAM_PARTITION_ACL_L2C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6C].num_entries + 
        partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
        partitions[TCAM_PARTITION_ACL_L2IP6].num_entries + 
        partitions[TCAM_PARTITION_ACL80].num_entries +
        partitions[TCAM_PARTITION_ACL160].num_entries +
        partitions[TCAM_PARTITION_ACL320].num_entries +
        partitions[TCAM_PARTITION_ACL480].num_entries - 1;

    memState[EXT_TCAM_VBITm].index_max = 1048575;
    return SOC_E_NONE;
}

#define SOC_TR3_BUILD_FM_ENTRY(fm_entry, esm_key_type, f11, f10, f9, f8, f7, \
                               f6, f5, f4, f3, \
                               f1, f2_15, f2_14, f2_13, f2_12, f2_11, f2_10, \
                               f2_9, f2_8, f2_7, f2_6, f2_5, f2_4, f2_3, f2_2, \
                               f2_1, f2_0, drop_adm_deny, rsp_type0, \
                               rsp_type1, rsp_type2, rsp_type3, cost0, \
                               cost1, cost2, cw0_ltr, cw1_ltr, cw2_ltr, \
                               num_cwords, rsp0_cnt, rsp1_cnt, rsp2_cnt, \
                               eop_cw0, eop_cw1, eop_cw2, ctx_addr_lsb0,\
                               ctx_addr_lsb1, ctx_addr_lsb2, cw0_dwords,\
                               cw1_dwords, cw2_dwords, cw0_opc, cw1_opc, \
                               cw2_opc, last_cw_opt, opt_cost0, opt_cost1,\
                               opt_cost2)\
do {\
    int nl_macro1, nl_macro2, nl_macro3;\
    sal_memset(&(fm_entry), 0, sizeof((fm_entry)));\
    (fm_entry).esm_key_subtype = (esm_key_type);\
    (fm_entry).f_selects[11] = (f11);\
    (fm_entry).f_selects[10] = (f10);\
    (fm_entry).f_selects[9] = (f9);\
    (fm_entry).f_selects[8] = (f8);\
    (fm_entry).f_selects[7] = (f7);\
    (fm_entry).f_selects[6] = (f6);\
    (fm_entry).f_selects[5] = (f5);\
    (fm_entry).f_selects[4] = (f4);\
    (fm_entry).f_selects[3] = (f3);\
    (fm_entry).f_selects[1] = (f1);\
    if (!(f2_15) && !(f2_14) && !(f2_13) && !(f2_12) && !(f2_11) && !(f2_10) \
         && !(f2_9) && !(f2_8) && !(f2_7) && !(f2_6) && !(f2_5) && !(f2_4) && \
        !(f2_3) && !(f2_2) && !(f2_1) && !(f2_0)) {\
        (fm_entry).flags_field_not_present = 1;\
    } else {\
        (fm_entry).mux_sel_flags[0] = (f2_0);\
        (fm_entry).mux_sel_flags[1] = (f2_1);\
        (fm_entry).mux_sel_flags[2] = (f2_2);\
        (fm_entry).mux_sel_flags[3] = (f2_3);\
        (fm_entry).mux_sel_flags[4] = (f2_4);\
        (fm_entry).mux_sel_flags[5] = (f2_5);\
        (fm_entry).mux_sel_flags[6] = (f2_6);\
        (fm_entry).mux_sel_flags[7] = (f2_7);\
        (fm_entry).mux_sel_flags[8] = (f2_8);\
        (fm_entry).mux_sel_flags[9] = (f2_9);\
        (fm_entry).mux_sel_flags[10] = (f2_10);\
        (fm_entry).mux_sel_flags[11] = (f2_11);\
        (fm_entry).mux_sel_flags[12] = (f2_12);\
        (fm_entry).mux_sel_flags[13] = (f2_13);\
        (fm_entry).mux_sel_flags[14] = (f2_14);\
        (fm_entry).mux_sel_flags[15] = (f2_15);\
    }\
    (fm_entry).drop_pkt_when_adm_ctl_deny = (drop_adm_deny);\
    (fm_entry).total_num_cwords = (num_cwords);\
    if ((rsp_type0)) {\
        (fm_entry).lookup_type_rsp[0] = (rsp_type0); \
    }\
    if ((rsp_type1)) {\
        (fm_entry).lookup_type_rsp[1] = (rsp_type1); \
    }\
    if ((rsp_type2)) {\
        (fm_entry).lookup_type_rsp[2] = (rsp_type2); \
    }\
    if ((rsp_type3)) {\
        (fm_entry).lookup_type_rsp[3] = (rsp_type3); \
    }\
    if ((cost0)) {\
        (fm_entry).cost_cw[0] = (cost0); \
    }\
    if ((cost1)) {\
        (fm_entry).cost_cw[1] = (cost1); \
    }\
    if (cost2) {\
        (fm_entry).cost_cw[2] = (cost2); \
    }\
    if (cw0_ltr >= 0) {\
        nl_macro1 = soc_tr3_build_ltr(unit, (cw0_ltr));\
        if (nl_macro1 < 0) {\
            return SOC_E_MEMORY;\
        }\
    }\
    if (cw1_ltr >= 0) {\
        nl_macro2 = soc_tr3_build_ltr(unit, (cw1_ltr));\
        if (nl_macro2 < 0) {\
            return SOC_E_MEMORY;\
        }\
    }\
    if (cw2_ltr >= 0) {\
        nl_macro3 = soc_tr3_build_ltr(unit, (cw2_ltr));\
        if (nl_macro3 < 0) {\
            return SOC_E_MEMORY;\
        }\
    }\
    if ((rsp0_cnt)) {\
        (fm_entry).rsp_index_count[0] = (rsp0_cnt)%4;\
        (fm_entry).eop_cw[0] = (eop_cw0);\
        (fm_entry).num_dwords_cw[0] = (cw0_dwords);\
        (fm_entry).opcode_cw[0] = ((cw0_opc) << 6) | nl_macro1;\
    }\
    (fm_entry).context_addr_lsb_cw[0] = (ctx_addr_lsb0);\
    if ((rsp1_cnt)) {\
        (fm_entry).rsp_index_count[1] = (rsp1_cnt);\
        (fm_entry).eop_cw[1] = (eop_cw1);\
        (fm_entry).num_dwords_cw[1] = (cw1_dwords);\
        (fm_entry).opcode_cw[1] = ((cw1_opc) << 6) | nl_macro2;\
    } else {\
        (fm_entry).eop_cw[1] = 8;\
    }\
    (fm_entry).context_addr_lsb_cw[1] = (ctx_addr_lsb1);\
    if ((rsp2_cnt)) {\
        (fm_entry).rsp_index_count[2] = (rsp2_cnt);\
        (fm_entry).eop_cw[2] = (eop_cw2);\
        (fm_entry).num_dwords_cw[2] = (cw2_dwords);\
        (fm_entry).opcode_cw[2] = ((cw2_opc) << 6) | nl_macro3;\
    } else {\
        (fm_entry).eop_cw[2] = 8;\
    }\
    (fm_entry).context_addr_lsb_cw[2] = (ctx_addr_lsb2);\
    (fm_entry).last_cword_is_optional = (last_cw_opt);\
    (fm_entry).optional_cost[2] = (opt_cost2); \
    (fm_entry).optional_cost[1] = (opt_cost1); \
    (fm_entry).optional_cost[0] = (opt_cost0); \
} while(0)

int 
_soc_tr3_build_fm_table(int unit, soc_tcam_info_t *tcam_info)
{
    soc_tcam_partition_t *partitions;
    esm_key_to_field_t key_map;
    int acl_key_type = 0;
    int fwd_l2_present, fwd_ipv4_present, fwd_ipv6_present;
    int fwd_ipv6u_present, fwd_ip6_key;
    int fwd_ipv6_configured, fwd_ipv6u_configured;

    partitions = tcam_info->partitions;

    fwd_l2_present = partitions[TCAM_PARTITION_FWD_L2].num_entries ||
                   partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries;
    fwd_ipv4_present = partitions[TCAM_PARTITION_FWD_IP4].num_entries ||
                   partitions[TCAM_PARTITION_FWD_IP4_UCAST].num_entries ||
                   partitions[TCAM_PARTITION_FWD_IP4_UCAST_WIDE].num_entries;
    fwd_ipv6_configured = partitions[TCAM_PARTITION_FWD_IP6].num_entries ||
                   partitions[TCAM_PARTITION_FWD_IP6_128_UCAST].num_entries ||
                 partitions[TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE].num_entries;
    fwd_ipv6u_configured = (partitions[TCAM_PARTITION_FWD_IP6U].num_entries != 0);

    /* 0: v6_64 fwding, 1: v6_128 fwding */
    fwd_ip6_key = soc_property_get(unit, spn_EXT_IP6_FWD_KEY, -1);
    if (fwd_ip6_key == -1) {
        /* default to v6_64 */
        fwd_ip6_key = 0;
        if (!fwd_ipv6u_configured) {
            if (fwd_ipv6_configured) {
                fwd_ip6_key = 1;
            }
        }
    }
    fwd_ipv6u_present = fwd_ipv6u_configured && !fwd_ip6_key;
    fwd_ipv6_present = fwd_ipv6_configured && fwd_ip6_key;

    if (fwd_l2_present) {
        if (!partitions[TCAM_PARTITION_FWD_L2_DUP].num_entries) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                               F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                               0, 0, 
                               F8_SA_DA_3210, 0, F6_DA_3210, 
                               0, F4_DA_54, 
                               F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                               0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_L2_DA, 
                               KEY_GEN_RSP_TYPE_L2_SA, 
                               0, 0, 
                               2, 6, 4, TR3_LTR_LIB_L2_FWD_DA, 
                               TR3_LTR_LIB_L2_FWD_SA, -1, 2, 1, 1,
                               0,  8, 8, 8, 0, 2, 6, 2, 2, 0, 1, 1, 0, 0, 
                               0, 0, 0);
        } else {
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                               F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                               0, 0, 
                               F8_SA_DA_3210, 0, F6_DA_3210, 
                               0, F4_DA_54, 
                               F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                               0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_L2_DA, 
                               KEY_GEN_RSP_TYPE_L2_SA, 
                               0, 0, 
                               1, 5, 2, TR3_LTR_LIB_L2_FWD_DUP, 
                               -1, -1, 1, 2, 0,
                               0, 8, 8, 8, 0, 4, 6, 4, 0, 0, 1, 0, 0, 0, 
                               0, 0, 0);
        }
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_L2, &key_map));
    }
    /* L3 keygen entries */
    sal_memset(&key_map, 0, sizeof(key_map));
    SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                       0, 0, 0, F8_SIPV4_DIPV4, 0, 0, 0, 0, F3_FLAGS_IP_1_VRF, 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                       KEY_GEN_RSP_TYPE_SIP, 0, 0, 
                       2, 6, 4, TR3_LTR_LIB_V4_ONLY_DIP, 
                       TR3_LTR_LIB_V4_ONLY_SIP, -1, 2, 1, 1,
                       0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                       0, 0, 0);
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV4_ONLY, &key_map));

    sal_memset(&key_map, 0, sizeof(key_map));
    SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                       0, F10_SIPV6_U64_DIPV6_U64, 0, 0, 0, 0, 0, 0, 
                       F3_FLAGS_IP_1_VRF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                       KEY_GEN_RSP_TYPE_SIP, 0, 0, 
                       2, 6, 4, TR3_LTR_LIB_V6_64_ONLY_DIP, 
                       TR3_LTR_LIB_V6_64_ONLY_SIP, -1, 2, 1, 1,
                       0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                       0, 0, 0);
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV6_64_ONLY, &key_map));

    sal_memset(&key_map, 0, sizeof(key_map));
    SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                       0, 0, F9_SIPV6, 0, 0, F6_SA_3210, 0, F4_FLAGS_IP_1_VRF, 
                       F3_FLAGS_IP_1_VRF, F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                       KEY_GEN_RSP_TYPE_SIP, 0, 0, 
                       2, 7, 4, TR3_LTR_LIB_V6_128_ONLY_DIP, 
                       TR3_LTR_LIB_V6_128_ONLY_SIP, -1, 2, 1, 1,
                       0, 8, 8, 8, 0, 3, 5, 3, 2, 0, 1, 1, 0, 0, 
                       0, 0, 0);
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV6_128_ONLY, &key_map));

    /* L2 + L3 keygens */
    if (!partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 0, 0, 
                        F8_SIPV4_DIPV4, 0, 0, 0, 0, F3_FLAGS_IP_1_VRF, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                        KEY_GEN_RSP_TYPE_SIP, KEY_GEN_RSP_TYPE_L2_SA, 0, 
                        2, 6, 4, TR3_LTR_LIB_V4_ONLY_DIP, 
                        TR3_LTR_LIB_V4_SIP_SA, -1, 2, 1, 2,
                        0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                        0, 0, 0);
    } else {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 0, 0, 
                        F8_SIPV4_DIPV4, 0, 0, 0, 0, F3_FLAGS_IP_1_VRF, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_SIP, 
                        KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_DIP, 0, 
                        1, 5, 3, TR3_LTR_LIB_V4_DUP, 
                        -1, -1, 1, 3, 0,
                        0, 8, 8, 8, 0, 4, 6, 4, 0, 0, 1, 0, 0, 0, 
                        0, 0, 0);
    }
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV4_SA, &key_map));

    /* L2 + L3 keygens */
    sal_memset(&key_map, 0, sizeof(key_map));
    if (!partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        0, 0, F9_SIPV6, F8_SA_3210_DA_3210, 0, F6_SA_54_DA_54, 
                        0, F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                        F3_FLAGS_IP_1_VRF, F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                        KEY_GEN_RSP_TYPE_SIP, KEY_GEN_RSP_TYPE_L2_SA, 0, 
                        2, 8, 4, TR3_LTR_LIB_V6_128_ONLY_DIP, 
                        TR3_LTR_LIB_V6_128_SIP_SA, -1, 2, 1, 2,
                        0, 8, 8, 8, 0, 3, 6, 3, 3, 0, 1, 1, 0, 0, 
                        0, 0, 0);
    } else {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        0, 0, F9_SIPV6, F8_SA_3210_DA_3210, 0, F6_SA_54_DA_54, 
                        0, F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                        F3_FLAGS_IP_1_VRF, F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_SIP, 
                        KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_DIP, 0, 
                        1, 7, 3, TR3_LTR_LIB_V6_128_DUP, 
                        -1, -1, 1, 3, 0,
                        0, 8, 8, 8, 0, 6, 6, 6, 0, 0, 1, 0, 0, 0, 
                        0, 0, 0);
    }
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV6_128_SA, &key_map));

    /* L2 + L3 keygens */
    sal_memset(&key_map, 0, sizeof(key_map));
    if (!partitions[TCAM_PARTITION_FWD_IP6U_DUP].num_entries) {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                        F10_SIPV6_U64_DIPV6_U64, 
                        0, 0, 0, 0, 
                        0, 0, F3_FLAGS_IP_1_VRF, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                        KEY_GEN_RSP_TYPE_SIP, KEY_GEN_RSP_TYPE_L2_SA, 0, 
                        2, 6, 4, TR3_LTR_LIB_V6_64_ONLY_DIP, 
                        TR3_LTR_LIB_V6_64_SIP_SA, -1, 2, 1, 2,
                        0, 8, 8, 8, 0, 2, 6, 2, 2, 0, 1, 1, 0, 0, 
                        0, 0, 0);
    } else {
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                        F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                        F10_SIPV6_U64_DIPV6_U64, 
                        0, 0, 0, 0, 
                        0, 0, F3_FLAGS_IP_1_VRF, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_SIP, 
                        KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_DIP, 0, 
                        1, 5, 3, TR3_LTR_LIB_V6_64_DUP, 
                        -1, -1, 1, 3, 0,
                        0, 8, 8, 8, 0, 4, 6, 4, 0, 0, 1, 0, 0, 0, 
                        0, 0, 0);
    }
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_IPV6_64_SA, &key_map));

    sal_memset(&key_map, 0, sizeof(key_map));
    SOC_TR3_BUILD_FM_ENTRY(key_map, 0,
                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                    0, 0,
                    F8_SA_DA_3210,
                    0, F6_DA_3210,
                    0, 0,
                    F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_L2_SA,
                    0, 0, 0,
                    3, 0, 0,
                    TR3_LTR_LIB_L2_FWD_SA, -1, -1, 1, 1,
                    0, 0, 8, 0, 0, 0, 0, 0, 2,
                    0, 0, 1, 0, 0, 0, 0, 0, 0);
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit,
                       KEY_GEN_OFFSET_TR3_WAR_L2SA_ONLY, &key_map));

    /* ACL7 + V6_128 + SA */
    if (partitions[TCAM_PARTITION_ACL480].num_entries) {
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_SA_OTAG_DA_ETHTYPE, 
                                0, F9_SIPV6, 
                                0, 
                                F7_L4_SRC_L4_DST, F6_OTAG_ITAG, 
                                0, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F1_DIPV6, F2_FLAG_RESERVED_0, 
                                F2_FLAG_RESERVED_0, 
                                F2_FLAG_RESERVED_0, F2_FLAG_RESERVED_0, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_TOS_7, 
                                F2_FLAG_TOS_6, 
                                F2_FLAG_TOS_5, 
                                F2_FLAG_TOS_4, 
                                F2_FLAG_TOS_3, 
                                F2_FLAG_TOS_2, 
                                F2_FLAG_TOS_1, 
                                F2_FLAG_TOS_0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                3, 10, 5, TR3_LTR_LIB_AFLSS0_DIP, 
                                TR3_LTR_LIB_AFLSS0_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 4, 7, 5, 3, 0, 1, 2, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS0, &key_map));

        /* AFLSS2: ACL480 + V6_64_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_SA_OTAG_DA_ETHTYPE, 
                                0, F9_SIPV6, 
                                0, 
                                F7_L4_SRC_L4_DST, F6_OTAG_ITAG, 
                                0, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F1_DIPV6, 0, 
                                0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_TOS_7, 
                                F2_FLAG_TOS_6, 
                                F2_FLAG_TOS_5, 
                                F2_FLAG_TOS_4, 
                                F2_FLAG_TOS_3, 
                                F2_FLAG_TOS_2, 
                                F2_FLAG_TOS_1, 
                                F2_FLAG_TOS_0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                3, 8, 5, TR3_LTR_LIB_AFLSS2_DIP, 
                                TR3_LTR_LIB_AFLSS2_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 2, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS2, &key_map));
    }

    if (partitions[TCAM_PARTITION_ACL320].num_entries) {
        /* AFLSS1: ACL320 + V4_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_SA_OTAG_DA_ETHTYPE, 
                                0, 0, 
                                0, 
                                F7_L4_SRC_L4_DST, F6_OTAG_ITAG, 
                                0, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 
                                0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                0, 0, 0, 0, 0, 0, 0, 0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                2, 7, 5, TR3_LTR_LIB_AFLSS1_DIP, 
                                TR3_LTR_LIB_AFLSS1_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 6, 3, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_AFLSS1, &key_map));

        /* AFLSS0A: ACL320 + V6_128_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, 0, F7_L4_SRC_L4_DST, 0, 
                                0, F4_FLAGS_IP_1_VRF, 0, 
                                F1_DIPV6, 0, 0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_TOS_7, F2_FLAG_TOS_6, F2_FLAG_TOS_5, 
                                F2_FLAG_TOS_4, F2_FLAG_TOS_3, F2_FLAG_TOS_2, 
                                F2_FLAG_TOS_1, F2_FLAG_TOS_0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                2, 8, 5, TR3_LTR_LIB_AFLSS0A_DIP, 
                                TR3_LTR_LIB_AFLSS0A_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 7, 3, 3, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS0A, &key_map));

        /* AFLSS2A: ACL320 + V6_64_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_SIPV6_U64_DIPV6_U64, 0, 0, 0, 0, 
                                F5_L4_SRC, F4_L4_DST, F3_FLAGS_IP_1_VRF, 
                                0, 0, 0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_TOS_7, F2_FLAG_TOS_6, F2_FLAG_TOS_5, 
                                F2_FLAG_TOS_4, F2_FLAG_TOS_3, F2_FLAG_TOS_2, 
                                F2_FLAG_TOS_1, F2_FLAG_TOS_0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                2, 6, 5, TR3_LTR_LIB_AFLSS2A_DIP, 
                                TR3_LTR_LIB_AFLSS2A_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS2A, &key_map));
    }
    
    if (partitions[TCAM_PARTITION_ACL160].num_entries) {
        /* AFLSS3: ACL160 + L2_FWD */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 0, F7_OTAG_ITAG, F6_DA_3210, 
                                0, F4_DA_54, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 1, KEY_GEN_RSP_TYPE_L2_DA, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_L2_SA, 
                                0, 2, 6, 4, TR3_LTR_LIB_L2_FWD_DA, 
                                TR3_LTR_LIB_AFLSS3_ACL_SA, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS3, &key_map));

        /* AFLSS1A: ACL160 + V4_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, F8_SIPV4_DIPV4, F7_L4_SRC_L4_DST, 0, 
                                0, F4_FLAGS_IP_1_VRF, 0, 
                                0, 0, 0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                0, 0, 0, 0, 0, 0, 0, 0,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                2, 6, 5, TR3_LTR_LIB_AFLSS1A_DIP, 
                                TR3_LTR_LIB_AFLSS1A_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS1A, &key_map));

        /* AFLSS1MP: ACL160 + V4_FWD + SA */
        sal_memset(&key_map, 0, sizeof(key_map));
        SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, F8_SIPV4_DIPV4, F7_L4_SRC_L4_DST, 0, 
                                0, F4_FLAGS_IP_1_VRF, 0, 
                                0, 0, 0, 0, 0, F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                0, 0, 0, 0, 0, 0, 0, 0,
                                1, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_DIP, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                1, 4, 3, TR3_LTR_LIB_AFLSS1AMP, 
                                -1, -1, 1, 4,
                                0, 0, 8, 8, 8, 0, 3, 3, 3, 0, 0, 1, 1, 0, 0, 
                                0, 0, 0);
        SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                            KEY_GEN_OFFSET_AFLSS1MP, &key_map));
    }

    /* FLSS0 */
    sal_memset(&key_map, 0, sizeof(key_map));
    SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                            F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                            0, F9_SIPV6, 0, F7_L4_SRC_L4_DST, 0, 
                            0, F4_FLAGS_IP_1_VRF, 
                            F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                            F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 
                            0, 0, 0, 0, 0, 0, 0, 0,
                            1, KEY_GEN_RSP_TYPE_DIP, 
                            KEY_GEN_RSP_TYPE_SIP, KEY_GEN_RSP_TYPE_L2_SA, 
                            0, 3, 10, 6, TR3_LTR_LIB_AFLSS0A_DIP, 
                            TR3_LTR_LIB_FLSS0_SIP, TR3_LTR_LIB_FLSS0_SA, 3, 1,
                            1, 1, 8, 8, 8, 0, 3, 5, 3, 2, 5, 1, 1, 1, 0, 
                            0, 0, 0);
    SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                        KEY_GEN_OFFSET_FLSS0, &key_map));

    /* TR2 L2_ACLC */
    if (partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
        if (fwd_ipv4_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, F8_OTAG_DA, F7_SIPV4, F6_DIPV4, 
                                F5_SGLP, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 0, 0, 0, F2_FLAG_MH_MIRROR_ONLY,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_SIP, 2, 7, 5, 
                                TR3_LTR_LIB_ACL_L2C_DIP, 
                                TR3_LTR_LIB_ACL_L2C_SA_ACL_SIP, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 2, 5, 2, 3, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2C_IP4, &key_map));
        }
        if (fwd_ipv6u_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_SIPV6_U64_DIPV6_U64, 0, F8_OTAG_DA, 0, 0, 
                                F5_SGLP, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 0, 0, 0, F2_FLAG_MH_MIRROR_ONLY,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_SIP, 2, 8, 5, 
                                TR3_LTR_LIB_ACL_L2C_DIPV6_64, 
                                TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_64, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2C_IP6U, &key_map));
        }
        if (fwd_ipv6_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 0, 0, 
                                F5_SGLP, F4_FLAGS_IP_1_VRF, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 0, 0, 0, F2_FLAG_MH_MIRROR_ONLY,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_SIP, 2, 10, 5, 
                                TR3_LTR_LIB_ACL_L2C_DIPV6_128, 
                                TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_128, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 4, 7, 4, 4, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2C_IP6, &key_map));
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, F8_OTAG_DA, 0, 0, 
                                F5_SGLP, F4_DA_54, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 0, 0, 0, F2_FLAG_MH_MIRROR_ONLY,
                                1, KEY_GEN_RSP_TYPE_L2_DA, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                0, 2, 6, 4, TR3_LTR_LIB_ACL_L2C_DA, 
                                TR3_LTR_LIB_ACL_L2C_SA_ACL, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 2, 4, 2, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2C, &key_map));
        } 
        if (!fwd_ipv4_present || !fwd_ipv6u_present || !fwd_ipv6_present ||
            !fwd_l2_present) {
            /* install ACL only */
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, F8_OTAG_DA, 0, 0, 
                                F5_SGLP, F4_DA_54, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 0, 0, 0, F2_FLAG_MH_MIRROR_ONLY,
                                1, KEY_GEN_RSP_TYPE_ACL, 
                                0, 0, 
                                0, 1, 5, 2, TR3_LTR_LIB_ACL_L2C_ONLY, 
                                -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 4, 6, 4, 0, 0, 1, 0, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2C_ONLY, &key_map));
        }
    }
    
    /* TR2 L2_ACL (non7k) */
    if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            /* TR2 L2_ACL (non7k) */
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_UDF1, 0, F8_OTAG_DA, F7_SGLP_DGLP, 
                                F6_CLASSID_ALL, F5_FLAGS_IP_1_VRF, F4_ITAG, 
                                F3_UDF_1_AND_2_VALID, 
                                0, 0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_INCOMING_TAG_STATUS_1, 
                                F2_FLAG_INCOMING_TAG_STATUS_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_ESM_KEY_SUBTYPE_0,
                                F2_FLAG_MY_STATION_HIT,
                                F2_FLAG_FORWARDING_TYPE_2,
                                F2_FLAG_FORWARDING_TYPE_1, F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_L2_DA, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                0, 2, 9, 5, TR3_LTR_LIB_ACL_L2_DA, 
                                TR3_LTR_LIB_ACL_L2_SA_ACL_N7K, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 5, 7, 6, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit,
                                KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SGLP, &key_map));
        
            /* create a SVP variant */
            key_map.f_selects[7] = F7_SVP_DVP;
            key_map.esm_key_subtype = 1; /* SVP gets bit 1 in flags */
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SVP, &key_map));
        
            /* TR2 L2_ACL (7k) */
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0,
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                                F10_UDF1, 0, F8_OTAG_DA, F7_SGLP_DGLP,
                                F6_CLASSID_ALL, F5_INT_RANGE_CHECK, F4_ITAG,
                                F3_UDF_1_AND_2_VALID,
                                0, 0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3,
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1,
                                F2_FLAG_L3_TYPE_0,
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15,
                                F2_FLAG_INCOMING_TAG_STATUS_1,
                                F2_FLAG_INCOMING_TAG_STATUS_0,
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0,
                                F2_FLAG_ESM_KEY_SUBTYPE_0,
                                F2_FLAG_MY_STATION_HIT,
                                F2_FLAG_FORWARDING_TYPE_2,
                                F2_FLAG_FORWARDING_TYPE_1, F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_L2_DA,
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL,
                                0, 2, 9, 5, TR3_LTR_LIB_ACL_L2_DA,
                                TR3_LTR_LIB_ACL_L2_SA_ACL, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 5, 7, 6, 2, 0, 1, 1, 0, 0,
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit,
                                KEY_GEN_OFFSET_TR2_ACL_L2_7K_SGLP, &key_map));
        
            key_map.f_selects[7] = F7_SVP_DVP;
            key_map.esm_key_subtype = 0; /* SGLP gets bit 0 in flags */
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_L2_7K_SVP, &key_map));
        }
        if (fwd_ipv6u_present) {
            /* only SGLP variant makes sense */
            sal_memset(&key_map, 0, sizeof(key_map));
            /* TR2 L2_ACL (non7k) */
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_SIPV6_U64_DIPV6_U64, F9_UDF1, F8_OTAG_DA, 
                                F7_SGLP_DGLP, F6_CLASSID_ALL, 
                                F5_FLAGS_IP_1_VRF,
                                F4_ITAG, 
                                F3_UDF_1_AND_2_VALID, 
                                0, 0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_INCOMING_TAG_STATUS_1, 
                                F2_FLAG_INCOMING_TAG_STATUS_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_ESM_KEY_SUBTYPE_0,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2,
                                F2_FLAG_FORWARDING_TYPE_1, F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_SIP, 2, 11, 5, 
                                TR3_LTR_LIB_ACL_L2_DIPV6_64, 
                                TR3_LTR_LIB_ACL_L2_SA_ACL_SIPV6_64, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 5, 7, 6, 3, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                           KEY_GEN_OFFSET_TR2_ACL_L2_V6U_NON7K_SGLP, &key_map));
        }

        if (!fwd_l2_present || !fwd_ipv6u_present) {
            int non7k = 0;
            if (tcam_info->mode) { /* 350 Mhz */
                non7k = 1;
            }
            /* only SGLP variant makes sense */
            sal_memset(&key_map, 0, sizeof(key_map));
            if (non7k) {
                /* TR2 L2_ACL non7k) */
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0,
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                                0, F9_UDF1, F8_OTAG_DA,
                                F7_SGLP_DGLP, F6_CLASSID_ALL,
                                F5_FLAGS_IP_1_VRF,
                                F4_ITAG,
                                F3_UDF_1_AND_2_VALID,
                                0, 0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3,
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1,
                                F2_FLAG_L3_TYPE_0,
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15,
                                F2_FLAG_INCOMING_TAG_STATUS_1,
                                F2_FLAG_INCOMING_TAG_STATUS_0,
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0,
                                F2_FLAG_ESM_KEY_SUBTYPE_0,
                                F2_FLAG_MY_STATION_HIT,
                                F2_FLAG_FORWARDING_TYPE_2,
                                F2_FLAG_FORWARDING_TYPE_1, F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_ACL,
                                0, 0, 0, 1, 8, 2,
                                TR3_LTR_LIB_ACL_L2_ONLY_N7K, -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 6, 7, 7, 0, 0, 1, 0, 0, 0,
                                0, 0, 0);
                SOC_IF_ERROR_RETURN(
                           _soc_write_esm_key_id_to_field_map_entry(unit,
                           KEY_GEN_OFFSET_TR2_ACL_L2_ONLY, &key_map));
            } else {
                /* TR2 L2_ACL (7k) */
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0,
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC,
                                0, F9_UDF1, F8_OTAG_DA,
                                F7_SGLP_DGLP, F6_CLASSID_ALL,
                                F5_INT_RANGE_CHECK,
                                F4_ITAG, 
                                F3_UDF_1_AND_2_VALID, 
                                0, 0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_INCOMING_TAG_STATUS_1, 
                                F2_FLAG_INCOMING_TAG_STATUS_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_ESM_KEY_SUBTYPE_0,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2,
                                F2_FLAG_FORWARDING_TYPE_1, F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_ACL, 
                                0, 0, 0, 1, 8, 2, 
                                TR3_LTR_LIB_ACL_L2_ONLY, -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 6, 7, 7, 0, 0, 1, 0, 0, 0, 
                                0, 0, 0);
                SOC_IF_ERROR_RETURN(
                            _soc_write_esm_key_id_to_field_map_entry(unit,
                            KEY_GEN_OFFSET_TR2_ACL_L2_ONLY, &key_map));
            }
        }
    }
    
    /* TR2 IPV4_ACL144 (ACL_IP4C) */ 
    if (partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
        sal_memset(&key_map, 0, sizeof(key_map));
        acl_key_type = 0;
        if (fwd_ipv4_present) {
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                   F11_SA_OTAG_DA_ETHTYPE, 0, 0, 
                                   F8_SIPV4_DIPV4, F7_L4_SRC_L4_DST, 0, 
                                   F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                   F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                   F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                   F2_FLAG_MH_MIRROR_ONLY, 
                                   F2_FLAG_MY_STATION_HIT, 
                                   F2_FLAG_FORWARDING_TYPE_2, 
                                   F2_FLAG_FORWARDING_TYPE_1, 
                                   F2_FLAG_L3_IIF_VALID,
                                   1, KEY_GEN_RSP_TYPE_DIP, 
                                   KEY_GEN_RSP_TYPE_ACL, 
                                   KEY_GEN_RSP_TYPE_SIP,KEY_GEN_RSP_TYPE_L2_SA, 
                                   2, 6, 5, TR3_LTR_LIB_ACL_IP4C_DIP, 
                                   TR3_LTR_LIB_ACL_IP4C_ACL_SIP_SA, -1, 2, 1, 3,
                                   0,  8, 8, 8, 0, 2, 6, 2, 3, 0, 1, 1, 0, 0, 
                                   0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4C, &key_map));
            acl_key_type ^= 1;
        } 
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            /* install IP4C ACL with l2 forwarding */
            
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                               F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                   0, 0, F8_SIPV4_DIPV4, F7_L4_SRC_L4_DST, 
                                   F6_OTAG_ITAG, F5_L3_PROTOCOL_TOS, 
                                   F4_FLAGS_IP_1_VRF, 
                                   F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                   F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                   F2_FLAG_MH_MIRROR_ONLY, 
                                   F2_FLAG_MY_STATION_HIT, 
                                   F2_FLAG_FORWARDING_TYPE_2, 
                                   F2_FLAG_FORWARDING_TYPE_1, 
                                   F2_FLAG_L3_IIF_VALID,
                                   1, KEY_GEN_RSP_TYPE_L2_SA, 
                                   KEY_GEN_RSP_TYPE_ACL, 
                                   KEY_GEN_RSP_TYPE_L2_DA, 0, 
                                   2, 8, 4, TR3_LTR_LIB_ACL_IP4C_L2_SA, 
                                   TR3_LTR_LIB_ACL_IP4C_L2_ACL_DA, -1, 2, 1,
                                   2, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                   0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4C_L2, &key_map));
            acl_key_type ^= 1;
        }
        if (!fwd_ipv4_present || !fwd_l2_present) {
            /* install IP4C ACL only */
           /* install IP4C ACL with no forwarding */
            
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                   F11_SA_OTAG_DA_ETHTYPE, 0, 0, 
                                   F8_SIPV4_DIPV4, F7_L4_SRC_L4_DST, 0, 
                                   F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                   F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                   F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                   F2_FLAG_MH_MIRROR_ONLY, 
                                   F2_FLAG_MY_STATION_HIT, 
                                   F2_FLAG_FORWARDING_TYPE_2, 
                                   F2_FLAG_FORWARDING_TYPE_1, 
                                   F2_FLAG_L3_IIF_VALID,
                                   1, KEY_GEN_RSP_TYPE_ACL, 
                                   0, 0, 0, 
                                   1, 5, 2, TR3_LTR_LIB_ACL_IP4C_ONLY, 
                                   -1, -1, 1, 1,
                                   0, 0, 8, 8, 8, 0, 5, 6, 5, 0, 0, 1, 0, 0, 0, 
                                   0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY, &key_map));
        }
    }

    /* TR2 IPV4_ACL (ACL_IP4) */
    if (partitions[TCAM_PARTITION_ACL_IP4].num_entries) {
        if (fwd_ipv4_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 
                                F8_SIPV4_DIPV4, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, F4_FLAGS_IP_1_VRF, 
                                F3_TOS_TTL, 
                                0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                2, 7, 5, TR3_LTR_LIB_ACL_IP4_DIP, 
                                TR3_LTR_LIB_ACL_IP4_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 6, 3, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4, &key_map));
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 
                                F8_SIPV4_DIPV4, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, 
                                F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                F3_TOS_TTL, 
                                0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_L2_DA, 
                                0, 2, 8, 4, TR3_LTR_LIB_ACL_IP4_L2_ACL_SA, 
                                TR3_LTR_LIB_ACL_IP4_L2_DA, -1, 2, 2,
                                1, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4_L2, &key_map));
        }
        if (!fwd_ipv4_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, 0, 
                                F8_SIPV4_DIPV4, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, 
                                F4_FLAGS_IP_1_VRF, 
                                F3_TOS_TTL, 
                                0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID,
                                1, KEY_GEN_RSP_TYPE_ACL, 
                                0, 0, 
                                0, 1, 4, 2, TR3_LTR_LIB_ACL_IP4_ONLY, 
                                -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 3, 5, 3, 0, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY, &key_map));
        } 
    }
    
    /* TR2 L2_IPV4_ACL (ACL_L2IP4) */
    /* Same F-selects and flags as ACL_IP4 above */
    if (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
        if (fwd_ipv4_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1){ 
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_SA_OTAG_DA_ETHTYPE, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_SGLP_DGLP, 
                                    F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                    F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                    0, F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_DIP, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                    KEY_GEN_RSP_TYPE_L2_SA, 
                                    2, 7, 5, TR3_LTR_LIB_ACL_IP4_DIP, 
                                    TR3_LTR_LIB_ACL_L2IP4_ACL_SIP_SA, -1, 2, 1,
                                    3, 0, 8, 8, 8, 0, 3, 6, 3, 2, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                    F5_INT_RANGE_CHECK, F4_FLAGS_IP_1_VRF, 
                                    F3_TOS_TTL, 
                                    0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                    F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                    F2_FLAG_L3_TYPE_0, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MH_MIRROR_ONLY, 
                                    F2_FLAG_L3_PROTOCOL_FN_3, 
                                    F2_FLAG_L3_PROTOCOL_FN_2, 
                                    F2_FLAG_L3_PROTOCOL_FN_1, 
                                    F2_FLAG_L3_PROTOCOL_FN_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_DIP, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                    KEY_GEN_RSP_TYPE_L2_SA, 
                                    2, 7, 5, TR3_LTR_LIB_ACL_IP4_DIP, 
                                    TR3_LTR_LIB_ACL_L2IP4_ACL_SIP_SA, -1, 2, 1,
                                    3, 0, 8, 8, 8, 0, 3, 6, 3, 2, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4, &key_map));
            }
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_SA_OTAG_DA_ETHTYPE, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_SGLP_DGLP, 
                                    F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                    F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST,
                                    0, F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_L2_DA,
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_L2_SA,
                                    0, 
                                    2, 8, 4, TR3_LTR_LIB_ACL_L2IP4_L2_DA, 
                                    TR3_LTR_LIB_ACL_L2IP4_L2_ACL_SA, -1, 2, 1,
                                    2, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4_L2, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                    F5_INT_RANGE_CHECK, 
                                    F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                    F3_TOS_TTL, 
                                    0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                    F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                    F2_FLAG_L3_TYPE_0, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MH_MIRROR_ONLY, 
                                    F2_FLAG_L3_PROTOCOL_FN_3, 
                                    F2_FLAG_L3_PROTOCOL_FN_2, 
                                    F2_FLAG_L3_PROTOCOL_FN_1, 
                                    F2_FLAG_L3_PROTOCOL_FN_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_L2_DA,
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_L2_SA,
                                    0, 
                                    2, 8, 4, TR3_LTR_LIB_ACL_L2IP4_L2_DA, 
                                    TR3_LTR_LIB_ACL_L2IP4_L2_ACL_SA, -1, 2, 1,
                                    2, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4_L2, &key_map));
            }
        }
        if (!fwd_ipv4_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_SA_OTAG_DA_ETHTYPE, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_SGLP_DGLP, 
                                    F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                    0, 
                                    0, F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_ACL, 
                                    0, 0, 0, 1, 6, 2, TR3_LTR_LIB_ACL_L2IP4_ONLY, 
                                    -1, -1, 1, 1,
                                    0, 0, 8, 8, 8, 0, 4, 6, 5, 0, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, 0, 
                                    F8_SIPV4_DIPV4, 
                                    F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                    F5_INT_RANGE_CHECK, F4_FLAGS_IP_1_VRF, 
                                    F3_TOS_TTL, 
                                    0, F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                    F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                    F2_FLAG_L3_TYPE_0, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MH_MIRROR_ONLY, 
                                    F2_FLAG_L3_PROTOCOL_FN_3, 
                                    F2_FLAG_L3_PROTOCOL_FN_2, 
                                    F2_FLAG_L3_PROTOCOL_FN_1, 
                                    F2_FLAG_L3_PROTOCOL_FN_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_ACL, 
                                    0, 0, 0, 1, 6, 2, TR3_LTR_LIB_ACL_L2IP4_ONLY, 
                                    -1, -1, 1, 1,
                                    0, 0, 8, 8, 8, 0, 4, 6, 5, 0, 0, 1, 1, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY, &key_map));
            }
        }
    }

    /* TR2 ACL144_IPV6 (ACL_IP6C */
    if (partitions[TCAM_PARTITION_ACL_IP6C].num_entries) {
        if (fwd_ipv6_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, 
                                0, 
                                0, 0, 
                                0, 
                                F4_OTAG, 
                                F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 2, 9, 5, 
                                TR3_LTR_LIB_ACL_IP6C_DIPV6_128, 
                                TR3_LTR_LIB_ACL_IP6C_ACL_SIPV6_128_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 6, 3, 4, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6C, &key_map));
        } 
        if (fwd_ipv6u_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_SIPV6_U64_DIPV6_U64, 0, 
                                0, 
                                0, 0, 
                                0, 
                                F4_OTAG, 
                                F3_FLAGS_IP_1_VRF, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 2, 7, 5, 
                                TR3_LTR_LIB_ACL_IP6C_DIP, 
                                TR3_LTR_LIB_ACL_IP6C_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 2, 4, 2, 3, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                              KEY_GEN_OFFSET_TR2_ACL_IP6C_IP6U_FWD, &key_map));
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                F10_SIPV6_U64_DIPV6_U64, 0, 
                                0, 
                                0, 0, 
                                0, 
                                F4_OTAG, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 1, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_L2_SA, KEY_GEN_RSP_TYPE_L2_DA, 
                                0, 2, 8, 4, 
                                TR3_LTR_LIB_ACL_IP6C_L2_ACL_SA, 
                                TR3_LTR_LIB_ACL_IP6C_L2_DA, -1, 2, 2,
                                1, 0, 8, 8, 8, 0, 4, 6, 4, 2, 0, 1, 1, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6C_L2, &key_map));
        }
        if (!fwd_ipv6_present || !fwd_ipv6u_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, 0, 
                                0, 
                                F10_SIPV6_U64_DIPV6_U64, 0, 
                                0, 
                                0, 0, 
                                0, 
                                F4_OTAG, 
                                0, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                0, 0, 1, KEY_GEN_RSP_TYPE_ACL, 
                                0, 0, 0, 1, 4, 2, 
                                TR3_LTR_LIB_ACL_IP6C_ONLY, 
                                -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 2, 4, 3, 0, 0, 1, 0, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY, &key_map));
        }
    }
    
    /* TR2 SHORT_IPV6_ACL (ACL_IP6S) */
    if (partitions[TCAM_PARTITION_ACL_IP6S].num_entries) {
        if (fwd_ipv6u_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_SA, 
                                F7_INT_RANGE_CHECK_L3_IIF, F6_L4_SRC_L4_DST, 
                                F5_CLASSID_SRC, F4_TOS_TTL, F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 3, 11, 5, 
                                TR3_LTR_LIB_ACL_IP6S_DIPV6_64, 
                                TR3_LTR_LIB_ACL_IP6S_ACL_SIPV6_64_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 7, 3, 6, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                               KEY_GEN_OFFSET_TR2_ACL_IP6S_IP6U_FWD, &key_map));
            acl_key_type ^= 1;
        } 
        if (fwd_ipv6_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_SA, 
                                F7_INT_RANGE_CHECK_L3_IIF, F6_L4_SRC_L4_DST, 
                                F5_CLASSID_SRC, F4_TOS_TTL, F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 2, 11, 5, 
                                TR3_LTR_LIB_ACL_IP6S_DIP, 
                                TR3_LTR_LIB_ACL_IP6S_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 7, 3, 6, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6S, &key_map));
            acl_key_type ^= 1;
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 
                                F7_INT_RANGE_CHECK_L3_IIF, F6_L4_SRC_L4_DST, 
                                F5_CLASSID_SRC, F4_TOS_TTL, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                F1_DIPV6, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, 
                                F2_FLAG_L3_TYPE_1, F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, 
                                KEY_GEN_RSP_TYPE_L2_DA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_L2_SA, 0, 2, 11, 4, 
                                TR3_LTR_LIB_ACL_IP6S_L2_DA, 
                                TR3_LTR_LIB_ACL_IP6S_L2_ACL_SA, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 4, 7, 5, 4, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6S_L2, &key_map));
            acl_key_type ^= 1;
        }
        if (!fwd_ipv6_present || !fwd_ipv6u_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_SA, 
                                F7_INT_RANGE_CHECK_L3_IIF, F6_L4_SRC_L4_DST, 
                                F5_CLASSID_SRC, F4_TOS_TTL, F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, 
                                F2_FLAG_L3_TYPE_1, F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_L3_PROTOCOL_FN_3, 
                                F2_FLAG_L3_PROTOCOL_FN_2, 
                                F2_FLAG_L3_PROTOCOL_FN_1, 
                                F2_FLAG_L3_PROTOCOL_FN_0, 
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, 
                                KEY_GEN_RSP_TYPE_ACL,
                                0, 0, 0,
                                1, 11, 2,
                                TR3_LTR_LIB_ACL_IP6S_ONLY, 
                                -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 7, 7, 9, 0, 0, 2, 0, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY, &key_map));
        }
    }
    
    /* TR2 FULL_IPV6_ACL (ACL_IP6F) */
    if (partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
        if (fwd_ipv6u_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, F4_TOS_TTL, 
                                F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, 0, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 3, 11, 5, 
                                TR3_LTR_LIB_ACL_IP6S_DIPV6_64, 
                                TR3_LTR_LIB_ACL_IP6F_ACL_SIPV6_64_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 7, 3, 6, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                              KEY_GEN_OFFSET_TR2_ACL_IP6F_IP6U_FWD, &key_map));
            acl_key_type ^= 1;
        } 
        if (fwd_ipv6_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, F4_TOS_TTL, 
                                F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, 0, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_DIP, 
                                KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                KEY_GEN_RSP_TYPE_L2_SA, 3, 11, 5, 
                                TR3_LTR_LIB_ACL_IP6S_DIP, 
                                TR3_LTR_LIB_ACL_IP6F_ACL_SIP_SA, -1, 2, 1,
                                3, 0, 8, 8, 8, 0, 3, 7, 3, 5, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6F, &key_map));
            acl_key_type ^= 1;
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, F4_TOS_TTL, 
                                F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                F1_DIPV6, 0, 0, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1, 
                                KEY_GEN_RSP_TYPE_L2_DA, KEY_GEN_RSP_TYPE_ACL, 
                                KEY_GEN_RSP_TYPE_L2_SA, 
                                0, 3, 11, 4, 
                                TR3_LTR_LIB_ACL_IP6F_L2_DA, 
                                TR3_LTR_LIB_ACL_IP6F_L2_ACL_SA, -1, 2, 1,
                                2, 0, 8, 8, 8, 0, 4, 7, 5, 4, 0, 1, 2, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6F_L2, &key_map));
            acl_key_type ^= 1;
        }
        if (!fwd_ipv6_present || !fwd_ipv6u_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                0, F9_SIPV6, F8_OTAG_DA, 
                                F7_L4_SRC_L4_DST, F6_CLASSID_ALL, 
                                F5_INT_RANGE_CHECK, F4_TOS_TTL, 
                                F3_FLAGS_IP_1_VRF, 
                                F1_DIPV6, 0, 0, 0, F2_FLAG_ESM_KEY_SUBTYPE_0, 
                                F2_FLAG_HG, F2_FLAG_L3_TYPE_3, 
                                F2_FLAG_L3_TYPE_2, F2_FLAG_L3_TYPE_1, 
                                F2_FLAG_L3_TYPE_0, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                F2_FLAG_MH_MIRROR_ONLY,
                                F2_FLAG_MY_STATION_HIT, 
                                F2_FLAG_FORWARDING_TYPE_2, 
                                F2_FLAG_FORWARDING_TYPE_1, 
                                F2_FLAG_L3_IIF_VALID, 1,
                                KEY_GEN_RSP_TYPE_ACL,
                                0, 0, 0,
                                2, 11, 2,
                                TR3_LTR_LIB_ACL_IP6F_ONLY, 
                                -1, -1, 1, 1,
                                0, 0, 8, 8, 8, 0, 7, 7, 9, 0, 0, 2, 0, 0, 0, 
                                0, 0, 0);
            SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY, &key_map));
        }
    }

    /* TR2 L2_IPV6_ACL (ACL_L2IP6) */
    if (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries) {
        if (fwd_ipv6_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            acl_key_type = 0;
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, F9_SIPV6, F8_IP_FLOW_V6_BITS_FLAGS_IPV4, 
                                    F7_SGLP_DGLP, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, F4_OTAG, 
                                    F3_FLAGS_IP_1_VRF, 
                                    F1_DIPV6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_DIP, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                    KEY_GEN_RSP_TYPE_L2_SA, 3, 10, 5, 
                                    TR3_LTR_LIB_ACL_IP6S_DIP, 
                                    TR3_LTR_LIB_ACL_L2IP6_ACL_SIP_SA, -1, 2, 1,
                                    3, 0, 8, 8, 8, 0, 3, 7, 3, 6, 0, 1, 2, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, F9_SIPV6, 0, 0, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, F4_OTAG, 
                                    F3_FLAGS_IP_1_VRF, 
                                    F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                    F2_FLAG_ESM_KEY_SUBTYPE_0, F2_FLAG_HG,
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_DIP, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_SIP, 
                                    KEY_GEN_RSP_TYPE_L2_SA, 3, 10, 5, 
                                    TR3_LTR_LIB_ACL_IP6S_DIP, 
                                    TR3_LTR_LIB_ACL_L2IP6_ACL_SIP_SA, -1, 2, 1,
                                    3, 0, 8, 8, 8, 0, 3, 7, 3, 5, 0, 1, 2, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6, &key_map));
            }
            acl_key_type ^= 1;
        }
        if (fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_SA_OTAG_DA_ETHTYPE, 
                                    0, F9_SIPV6, F8_IP_FLOW_V6_BITS_FLAGS_IPV4, 
                                    F7_SGLP_DGLP, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, 
                                    F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                    F3_FLAGS_IP_1_VRF, 
                                    F1_DIPV6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_L2_DA, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_L2_SA, 
                                    0, 3, 11, 4, 
                                    TR3_LTR_LIB_ACL_L2IP6_L2_DA,
                                    TR3_LTR_LIB_ACL_L2IP6_L2_ACL_SA, -1, 2, 1,
                                    2, 0, 8, 8, 8, 0, 7, 7, 8, 1, 0, 1, 2, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, F9_SIPV6, F8_OTAG_DA, 
                                    0, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, F4_FLAGS_IP_1_VRF, 
                                    F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST, 
                                    F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                    F2_FLAG_ESM_KEY_SUBTYPE_0, F2_FLAG_HG,
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID, 1,KEY_GEN_RSP_TYPE_L2_DA, 
                                    KEY_GEN_RSP_TYPE_ACL, KEY_GEN_RSP_TYPE_L2_SA, 
                                    0, 3, 11, 4, 
                                    TR3_LTR_LIB_ACL_L2IP6_L2_DA,
                                    TR3_LTR_LIB_ACL_L2IP6_L2_ACL_SA, -1, 2, 1,
                                    2, 0, 8, 8, 8, 0, 4, 7, 5, 4, 0, 1, 2, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2, &key_map));
            }
            acl_key_type ^= 1;
        }
        if (!fwd_ipv6_present || !fwd_l2_present) {
            sal_memset(&key_map, 0, sizeof(key_map));
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, F9_SIPV6, F8_IP_FLOW_V6_BITS_FLAGS_IPV4, 
                                    F7_SGLP_DGLP, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, F4_OTAG, 
                                    F3_FLAGS_IP_1_VRF, 
                                    F1_DIPV6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_13, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_12, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_11, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_10, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_9, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_8, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_7, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_6, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_2, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_1, 
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_0, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID,
                                    1, KEY_GEN_RSP_TYPE_ACL, 
                                    0, 0, 0, 2, 9, 2, 
                                    TR3_LTR_LIB_ACL_L2IP6_ONLY, 
                                    -1, -1, 1, 1,
                                    0, 0, 8, 8, 8, 0, 7, 7, 9, 0, 0, 2, 0, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2, &key_map));
            } else {
                SOC_TR3_BUILD_FM_ENTRY(key_map, acl_key_type, 
                                    F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC, 
                                    0, F9_SIPV6, 0, 0, F6_L4_SRC_L4_DST, 
                                    F5_L3_PROTOCOL_TOS, F4_OTAG, 
                                    F3_FLAGS_IP_1_VRF, 
                                    F1_DIPV6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                    F2_FLAG_ESM_KEY_SUBTYPE_0, F2_FLAG_HG,
                                    F2_FLAG_LOOKUP_STATUS_VECTOR_15, 
                                    F2_FLAG_MY_STATION_HIT, 
                                    F2_FLAG_FORWARDING_TYPE_2, 
                                    F2_FLAG_FORWARDING_TYPE_1, 
                                    F2_FLAG_L3_IIF_VALID, 1, KEY_GEN_RSP_TYPE_ACL, 
                                    0, 0, 0, 2, 9, 2, 
                                    TR3_LTR_LIB_ACL_L2IP6_ONLY, 
                                    -1, -1, 1, 1,
                                    0, 0, 8, 8, 8, 0, 7, 7, 8, 0, 0, 2, 0, 0, 0, 
                                    0, 0, 0);
                SOC_IF_ERROR_RETURN(_soc_write_esm_key_id_to_field_map_entry(unit, 
                                    KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY, &key_map));
            }
        }
    }
    return SOC_E_NONE;
}

int soc_triumph3_esm_init(int unit)
{
    uint32 esm_search_profile_base=0;
    /* 64 ports, 100 bits of source_trunk_map per port */
    int idx_end=63;
    int alloc_size, rv;
    uint8 *buf;
    uint32 tmp2=0;
    uint32 l2_entry_mask[2];
    ext_l2_entry_tcam_entry_t l2_entry;
    int i;
    int blks_per_dev;
    esm_range_check_entry_t *range_check_entry;
    esm_l3_protocol_fn_entry_t *l3_proto_fn_entry;
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int l2_fast_init_done = 0, l2_wide_fast_init_done = 0;
    int l2_start_tcam_idx, l2_num_tcams;
    
    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;
    blks_per_dev = tcam_info->blks_per_dev;

    SOC_IF_ERROR_RETURN(_soc_triumph3_esm_init_adjust_mem_size(unit));

    SOC_IF_ERROR_RETURN(READ_ESM_EXT_HITBIT_CONTROLr(unit, &tmp2));
    soc_reg_field_set(unit, ESM_EXT_HITBIT_CONTROLr, &tmp2, EN_SET_L2_DST_HITf, 
                      1);
    soc_reg_field_set(unit, ESM_EXT_HITBIT_CONTROLr, &tmp2,EN_SET_L2_SRC_HITf, 
                      1);
    soc_reg_field_set(unit, ESM_EXT_HITBIT_CONTROLr, &tmp2, EN_SET_L3_DST_HITf, 
                      1);
    soc_reg_field_set(unit, ESM_EXT_HITBIT_CONTROLr, &tmp2,EN_SET_L3_SRC_HITf, 
                      1);
    SOC_IF_ERROR_RETURN(WRITE_ESM_EXT_HITBIT_CONTROLr(unit, tmp2));

    SOC_IF_ERROR_RETURN(_soc_tr3_set_dup_db_offset(unit));
 
    /* MISC ACL fields */
#if 1
    idx_end = soc_mem_index_max(unit, ESM_RANGE_CHECKm);
    alloc_size = (idx_end+1) * sizeof(esm_range_check_entry_t);
    buf = soc_cm_salloc(unit, alloc_size, "esm_range_check");
    if (buf == NULL) return SOC_E_MEMORY;
    rv = soc_mem_read_range(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ANY, 
                            0, idx_end, buf);
    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        return SOC_E_FAIL;
    }
    for (i = 0; i < idx_end + 1; i++) {
        range_check_entry = soc_mem_table_idx_to_pointer
            (unit, ESM_RANGE_CHECKm,  esm_range_check_entry_t *, buf, i);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, range_check_entry, 
                            ENABLEf, 0);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, range_check_entry, 
                            FIELD_SELECTf, 0);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, range_check_entry, 
                            UPPER_BOUNDSf, 0);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, range_check_entry, 
                            LOWER_BOUNDSf, 0);
    }
    rv = soc_mem_write_range(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ANY, 
                             0, idx_end, (void *) buf);
    soc_cm_sfree(unit, buf);
    if (rv < 0 ) return SOC_E_FAIL;
#else
    soc_mem_clear(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ALL, TRUE);
#endif

#if 1
    /* ESM_L3_PROTOCOL_FN */
    idx_end = soc_mem_index_max(unit, ESM_L3_PROTOCOL_FNm);
    alloc_size = (idx_end+1) * sizeof(esm_l3_protocol_fn_entry_t);
    buf = soc_cm_salloc(unit, alloc_size, "esm_l3_protocol_fn");
    if (buf == NULL) return SOC_E_MEMORY;
    rv = soc_mem_read_range(unit, ESM_L3_PROTOCOL_FNm, MEM_BLOCK_ANY, 
                            0, idx_end, buf);
    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        return SOC_E_FAIL;
    }
    for (i = 0; i < idx_end + 1; i++) {
        l3_proto_fn_entry = soc_mem_table_idx_to_pointer
            (unit, ESM_L3_PROTOCOL_FNm,  esm_l3_protocol_fn_entry_t *, buf, i);
        soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, l3_proto_fn_entry, 
                            L3_PROTOCOL_FNf, 0);
    }
    rv = soc_mem_write_range(unit, ESM_L3_PROTOCOL_FNm, MEM_BLOCK_ANY, 
                             0, idx_end, (void *) buf);
    soc_cm_sfree(unit, buf);
    if (rv < 0 ) return SOC_E_FAIL;
#else
    soc_mem_clear(unit, ESM_L3_PROTOCOL_FNm, MEM_BLOCK_ALL, TRUE);
#endif

    /* ESM_CONTAINER_SELECT */
    tmp2 = 0;
    SOC_IF_ERROR_RETURN(WRITE_ESM_CONTAINER_SELECTr(unit, tmp2));

    /* program the ET_PA_XLAT entries */
    SOC_IF_ERROR_RETURN(_soc_triumph3_esm_init_set_et_pa_xlate(unit));

    /* initialize worst_case fme entry */
    tr3_worst_fme[unit].cost0 = 0; 
    tr3_worst_fme[unit].cost1 = 0; 
    tr3_worst_fme[unit].cost2 = 0; 
    tr3_worst_fme[unit].idx = 0; 
    SOC_IF_ERROR_RETURN(_soc_tr3_build_fm_table(unit, tcam_info));


    /* program ESM search profile base */
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base, 
                      SRC_TRUNK_MAPf, 5);
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base,
                      VLAN_XLATEf, 4);
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base, 
                      VFPf, 3);
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base, 
                      VLANf, 2);
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base, 
                      SOURCE_VPf, 1);
    soc_reg_field_set(unit, ESM_SEARCH_PROFILE_BASEr, &esm_search_profile_base, 
                      L3_IIFf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ESM_SEARCH_PROFILE_BASEr(unit, 
                        esm_search_profile_base)); 

    rv = _soc_tr3_esm_init_pkt_type_tcam(unit, partitions);
    SOC_IF_ERROR_RETURN(rv);

    rv = _soc_tr3_esm_fill_search_profile(unit);
    SOC_IF_ERROR_RETURN(rv);

    /* better init values, using TR2 ACL info: */
    rv = _soc_tr3_esm_init_set_esm_mode_per_port(unit);
    SOC_IF_ERROR_RETURN(rv);

    /* initialize L2 table to free  */
    sal_memset(&l2_entry, 0, sizeof(l2_entry));
    soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &l2_entry, FREEf, 1);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &l2_entry, MASK_FREEf, 0);
    l2_entry_mask[0]=0xffffffff;
    l2_entry_mask[1]=0xffff;
    soc_mem_field_set(unit, EXT_L2_ENTRY_TCAMm, (uint32 *) &l2_entry, 
                      MASK_MAC_ADDRf, l2_entry_mask);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &l2_entry, MASK_VLAN_IDf, 
                        0xfff);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &l2_entry, 
                        MASK_VLAN_ID_UNUSEDf, 0x3);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &l2_entry, 
            MASK_KEY_TYPEf, 0x1);

    /* 
     * If a TCAM is entirely allocated for L2/L2_WIDE partition,
     * then initialize that TCAM with fast init sequence to save time.
     * In case TCAM is not entirely allocated for L2/L2_WIDE, then it'll be 
     * fast inited using soc_tr3_tcam_fast_init() and then the L2
     * partitions will be initialized by soc_mem_write per each index
     */
    if (!(soc_mem_index_count(unit, EXT_L2_ENTRY_TCAMm) % 
                (blks_per_dev * TR3_ENTRIES_PER_BLK))) {
        if (!(partitions[TCAM_PARTITION_FWD_L2].tcam_base % 
                    (blks_per_dev * TR3_ENTRIES_PER_BLK))) {
            l2_num_tcams = soc_mem_index_count(unit, EXT_L2_ENTRY_TCAMm) /
                (blks_per_dev * TR3_ENTRIES_PER_BLK);
            if (l2_num_tcams) {
                l2_start_tcam_idx = 
                    partitions[TCAM_PARTITION_FWD_L2].tcam_base /
                    (blks_per_dev * TR3_ENTRIES_PER_BLK);
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_l2_fast_init(unit, 
                            l2_start_tcam_idx, l2_num_tcams, &l2_entry));
            }
            l2_fast_init_done = TRUE;
        }
    } 

    if (!(soc_mem_index_count(unit, EXT_L2_ENTRY_TCAM_WIDEm) % 
                (blks_per_dev * TR3_ENTRIES_PER_BLK))) {
        if (!(partitions[TCAM_PARTITION_FWD_L2_WIDE].tcam_base % 
                    (blks_per_dev * TR3_ENTRIES_PER_BLK))) {
            l2_num_tcams = soc_mem_index_count(unit, EXT_L2_ENTRY_TCAM_WIDEm) /
                (blks_per_dev * TR3_ENTRIES_PER_BLK);
            if (l2_num_tcams) {
                l2_start_tcam_idx = 
                    partitions[TCAM_PARTITION_FWD_L2_WIDE].tcam_base /
                    (blks_per_dev * TR3_ENTRIES_PER_BLK);
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_l2_fast_init(unit, 
                            l2_start_tcam_idx, l2_num_tcams, &l2_entry));
            }
            l2_wide_fast_init_done = TRUE;
        }
    } 

    /*
     * Fast Init the other TCAMs not covered in L2 fast init
     */
    if (!SOC_WARM_BOOT(unit)) {
        soc_tr3_tcam_fast_init(unit);
    }

    if (!l2_fast_init_done) {
        for (i = 0; i < soc_mem_index_count(unit, EXT_L2_ENTRY_TCAMm); i++) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, EXT_L2_ENTRY_TCAMm, 
                        MEM_BLOCK_ANY, i, &l2_entry));
        }
    }

    if (!l2_wide_fast_init_done) {
        for (i = 0; i < soc_mem_index_count(unit, EXT_L2_ENTRY_TCAM_WIDEm); 
             i++) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, EXT_L2_ENTRY_TCAM_WIDEm, 
                        MEM_BLOCK_ANY, i, &l2_entry));
        }
    }

    return SOC_E_NONE;
}

void
soc_triumph3_esm_init_mem_config(int unit)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;

    if (soc_property_get(unit, "AV_SIMULATION", 0)) {
        return;
    }

    sop = SOC_PERSIST(unit);
    memState = sop->memState;

    /* L2 forwarding partition */
    memState[EXT_L2_ENTRY_1m].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L2_ENTRY_2m].index_max = -1; /* TCAM */
    memState[EXT_L2_ENTRY_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_L2_ENTRY_TCAM_WIDEm].index_max = -1; /* TCAM */
    memState[EXT_L2_ENTRY_DATA_ONLYm].index_max = -1; /* SRAM */
    memState[EXT_L2_ENTRY_DATA_ONLY_WIDEm].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_L2m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_L2m].index_max = -1; /* DST hit bit */
    memState[EXT_SRC_HIT_BITS_L2_WIDEm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_L2_WIDEm].index_max = -1; /* DST hit bit */
    memState[EXT_LOC_SRC_HIT_BITS_L2m].index_max = -1; /* DST hit bit */
    memState[EXT_LOC_SRC_HIT_BITS_L2_WIDEm].index_max = -1; /* DST hit bit */
    memState[EXT_TCAM_VBIT_L2_ENTRY_1m].index_max = -1; 
    memState[EXT_TCAM_VBIT_L2_ENTRY_2m].index_max = -1; 

    /* IP4 forwarding partition */
    memState[EXT_IPV4_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV4_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV4m].index_max = -1; /* SRAM */
    memState[EXT_IPV4_UCASTm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV4_UCAST_TCAMm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L3_UCAST_DATA_IPV4m].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV4_UCAST_WIDEm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV4_UCAST_WIDE_TCAMm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L3_UCAST_DATA_WIDE_IPV4m].index_max = -1; /* TCAM + SRAM */
    memState[EXT_SRC_HIT_BITS_IPV4_UCASTm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV4_UCASTm].index_max = -1; /* DST hit bit */
    memState[EXT_SRC_HIT_BITS_IPV4_UCAST_WIDEm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV4_UCAST_WIDEm].index_max = -1; /* DST hit bit */
    memState[EXT_SRC_HIT_BITS_IPV4m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV4m].index_max = -1; /* DST hit bit */
    memState[EXT_TCAM_VBIT_DEFIP_IPV4m].index_max = -1; 
    memState[EXT_TCAM_VBIT_IPV4_UCASTm].index_max = -1; 
    memState[EXT_TCAM_VBIT_IPV4_UCAST_WIDEm].index_max = -1; 

    /* IP6 64-bit prefix forwarding partition */
    memState[EXT_IPV6_64_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_64_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV6_64m].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_IPV6_64m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_64m].index_max = -1; /* DST hit bit */
    memState[EXT_TCAM_VBIT_DEFIP_IPV6_64m].index_max = -1; 

    /* IP6 128-bit prefix forwarding partition */
    memState[EXT_IPV6_128_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_128_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV6_128m].index_max = -1; /* SRAM */
    memState[EXT_IPV6_128_UCASTm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_128_UCAST_TCAMm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L3_UCAST_DATA_IPV6_128m].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_128_UCAST_WIDEm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_128_UCAST_WIDE_TCAMm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L3_UCAST_DATA_WIDE_IPV6_128m].index_max = -1; /* TCAM + SRAM */
    memState[EXT_SRC_HIT_BITS_IPV6_128_UCASTm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_128_UCASTm].index_max = -1; /* DST hit bit */
    memState[EXT_SRC_HIT_BITS_IPV6_128_UCAST_WIDEm].index_max = 
                                                            -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_128_UCAST_WIDEm].index_max = 
                                                            -1; /* DST hit bit */
    memState[EXT_SRC_HIT_BITS_IPV6_128m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_128m].index_max = -1; /* DST hit bit */
    memState[EXT_TCAM_VBIT_IPV6_128_UCASTm].index_max = -1; 
    memState[EXT_TCAM_VBIT_IPV6_128_UCAST_WIDEm].index_max = -1; 
    memState[EXT_TCAM_VBIT_DEFIP_IPV6_128m].index_max = -1; 

    /* L2 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_L2m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL288_L2_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_L2_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_L2_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_L2_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_L2_6Xm].index_max = -1; /* SRAM */

    /* IP4 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_IPV4m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL288_IPV4_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_IPV4_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_IPV4_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_IPV4_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL288_IPV4_6Xm].index_max = -1; /* SRAM */

    /* IP6 short form 360-bit ACL partition */
    memState[EXT_ACL360_TCAM_DATA_IPV6_SHORTm].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORT_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORT_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORT_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORT_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORT_6Xm].index_max = -1; /* SRAM */

    /* IP6 full form 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_IPV6_LONGm].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONG_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONG_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONG_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONG_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONG_6Xm].index_max = -1; /* SRAM */

    /* L2 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_L2m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_L2_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_L2_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_L2_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_L2_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_L2_6Xm].index_max = -1; /* SRAM */

    /* IP4 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV4m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_IPV4_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV4_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV4_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV4_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV4_6Xm].index_max = -1; /* SRAM */

    /* IP6 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV6m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_IPV6_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV6_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV6_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV6_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL144_IPV6_6Xm].index_max = -1; /* SRAM */

    /* L2+IP4 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV4m].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4_6Xm].index_max = -1; /* SRAM */

    /* L2+IP6 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV6m].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6_6Xm].index_max = -1; /* SRAM */

    /* 80-bit ACL partition */
    memState[EXT_ACL80_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_ACL80_FP_POLICY_1Xm].index_max = -1; /* AD */
    memState[EXT_ACL80_FP_POLICY_2Xm].index_max = -1; /* AD */
    memState[EXT_ACL80_FP_POLICY_3Xm].index_max = -1; /* AD */
    memState[EXT_ACL80_FP_POLICY_4Xm].index_max = -1; /* AD */
    memState[EXT_ACL80_FP_POLICY_6Xm].index_max = -1; /* AD */

    /* 160-bit ACL partition */
    memState[EXT_ACL160_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_ACL160_FP_POLICY_1Xm].index_max = -1; /* AD */
    memState[EXT_ACL160_FP_POLICY_2Xm].index_max = -1; /* AD */
    memState[EXT_ACL160_FP_POLICY_3Xm].index_max = -1; /* AD */
    memState[EXT_ACL160_FP_POLICY_4Xm].index_max = -1; /* AD */
    memState[EXT_ACL160_FP_POLICY_6Xm].index_max = -1; /* AD */

    /* 320-bit ACL partition */
    memState[EXT_ACL320_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_ACL320_FP_POLICY_1Xm].index_max = -1; /* AD */
    memState[EXT_ACL320_FP_POLICY_2Xm].index_max = -1; /* AD */
    memState[EXT_ACL320_FP_POLICY_3Xm].index_max = -1; /* AD */
    memState[EXT_ACL320_FP_POLICY_4Xm].index_max = -1; /* AD */
    memState[EXT_ACL320_FP_POLICY_6Xm].index_max = -1; /* AD */

    /* 480-bit ACL partition */
    memState[EXT_ACL432_TCAM_MASKm].index_max = -1; /* TCAM */
    memState[EXT_ACL432_TCAM_MASK_L2_IPV6m].index_max = -1; 
    memState[EXT_ACL432_TCAM_MASK_IPV6_LONGm].index_max = -1; 
    memState[EXT_ACL360_TCAM_MASKm].index_max = -1; /* TCAM */
    memState[EXT_ACL360_TCAM_MASK_IPV6_SHORTm].index_max = -1; /* TCAM */
    memState[EXT_ACL480_TCAM_MASKm].index_max = -1; /* TCAM */
    memState[EXT_ACL480_TCAM_DATAm].index_max = -1; /* TCAM */
    memState[EXT_ACL480_FP_POLICY_1Xm].index_max = -1; /* AD */
    memState[EXT_ACL480_FP_POLICY_2Xm].index_max = -1; /* AD */
    memState[EXT_ACL480_FP_POLICY_3Xm].index_max = -1; /* AD */
    memState[EXT_ACL480_FP_POLICY_4Xm].index_max = -1; /* AD */
    memState[EXT_ACL480_FP_POLICY_6Xm].index_max = -1; /* AD */

    /* All L2 local src entries */
    memState[EXT_LOC_SRC_HBITSm].index_max = -1; /* SRC hit bit */

    /* All L2+L3 forwarding entries */
    memState[EXT_SRC_HBITSm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HBITSm].index_max = -1; /* DST hit bit */

    /* All L3 forwarding entries */
    memState[EXT_IPV4_TCAMm].index_max = -1; /* SRAM */
    memState[EXT_IPV6_64_TCAMm].index_max = -1; /* SRAM */
    memState[EXT_IPV6_128_TCAMm].index_max = -1; /* SRAM */
    memState[EXT_DEFIP_DATAm].index_max = -1; /* SRAM */
    memState[EXT_L3_UCAST_DATAm].index_max = -1; /* SRAM */
    memState[EXT_L3_UCAST_DATA_WIDEm].index_max = -1; /* SRAM */

    /* All ACL entries */
    memState[EXT_FP_POLICY_1Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_2Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_3Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_4Xm].index_max = -1; /* SRAM */
    memState[EXT_FP_POLICY_6Xm].index_max = -1; /* SRAM */

    memState[EXT_TCAM_VBIT_ACL80m].index_max = -1; /* TCAM */
    memState[EXT_TCAM_VBIT_ACL160m].index_max = -1; /* TCAM */
    memState[EXT_TCAM_VBIT_ACL320m].index_max = -1; /* TCAM */
    memState[EXT_TCAM_VBIT_ACL480m].index_max = -1; /* TCAM */
    memState[EXT_TCAM_VBITm].index_max = -1; /* TCAM */
}

/*
 * Function:
 *      soc_tr3_search_ext_mem
 * Purpose:
 *      Search triumph3 external memory L3 host tables for a key.
 * Parameters:
 *      mem  		     Memory to search. Specifying EXT_IPV4_UCASTm will 
 *                       will search ext V4 host tables and EXT_IPV6_128_UCASTm
 *                       will search ext V6 host tables.
 *                       On successful search the memory in which match occured
 *                       is returned.
 *      key            	     IN:  Data to search for
 *      index_ptr            OUT:
 *                           If entry found gets the location of the entry.
 * Returns:
 *	SOC_E_NOT_FOUND
 *			     Entry not found.  
 *	SOC_E_NONE	     Entry is found:  index_ptr gets location
 *	SOC_E_XXX	     If internal error occurs
 */
int
soc_tr3_search_ext_mem(int unit, soc_mem_t *mem, void *key, int *index_ptr)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    uint32 key_words[6], ip_addr_v4;
    int v6_key = 0;
    int rv=0, part, tmp;
    uint16 vrf_id;
    uint32 part_to_mem[6] = {EXT_IPV4_UCASTm, EXT_IPV4_UCAST_WIDEm, 0, 0,
                             EXT_IPV6_128_UCASTm, EXT_IPV6_128_UCAST_WIDEm};
    uint32 field[SOC_MAX_MEM_FIELD_WORDS];

    if ((key == NULL) || (index_ptr == NULL)) {
        return SOC_E_PARAM;
    }

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NOT_FOUND;
    }
    partitions = tcam_info->partitions;
    
    switch (*mem) {
    case EXT_IPV4_UCASTm:
    case EXT_IPV4_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP4;
        break;
    case EXT_IPV6_128_UCASTm:
    case EXT_IPV6_128_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP6;
        v6_key = 1;
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "Invalid memory %d\n"),  *mem));
        return SOC_E_PARAM;
    }

    sal_memset(key_words, 0, sizeof(key_words));

    /* v4/v6 need min of 6 bytes */
    if (!v6_key) {
#if 0
        uint8 *keyp;
#endif

        vrf_id =  soc_EXT_IPV4_TCAMm_field32_get(unit, (uint32 *)key, VRF_IDf);
        key_words[2] = 0;
        key_words[1] = MAKE_WORD(0, 0, vrf_id >> 8, vrf_id & 0xff);
        
        ip_addr_v4 = soc_EXT_IPV4_TCAMm_field32_get(unit, (uint32 *)key, IP_ADDRf);
#if 0
        key_words[0] = MAKE_WORD(keyp[0], keyp[1], keyp[2], keyp[3]);
#else
        key_words[0] = ip_addr_v4;
#endif
    } else {
#if 0
        uint32 *keyp;
#endif

        vrf_id = soc_EXT_IPV6_128_TCAMm_field32_get(unit, key, VRF_IDf);
        (void) soc_EXT_IPV6_128_TCAMm_field_get(unit, key, IP_ADDRf, field);
#if 0
        key_words[0] = MAKE_WORD(keyp[14], keyp[15], keyp[16], keyp[17]);
        key_words[1] = MAKE_WORD(keyp[10], keyp[11], keyp[12], keyp[13]);
        key_words[2] = MAKE_WORD(keyp[6], keyp[7], keyp[8], keyp[9]);
        key_words[3] = MAKE_WORD(keyp[2], keyp[3], keyp[4], keyp[5]);
        key_words[4] = MAKE_WORD(0, 0, keyp[0], keyp[1]);
#else
        key_words[0] = field[0];
        key_words[4] = field[1];
        key_words[2] = field[2];
        key_words[1] = field[3];
        key_words[3] = MAKE_WORD(0, 0, vrf_id >> 8, vrf_id & 0xff);
        
#endif
    }

    /* call underlying search function */
    rv = soc_tr3_tcam_search_entry(unit, part, 0, key_words, index_ptr, &tmp);
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Internal search request failed\n")));
        return rv;
    }
    tmp = *index_ptr;
    if (!(tmp & MAKE_FIELD(TR3_WRCMP_RSP_SMF, 1))) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Lookup failed\n")));
        return SOC_E_NOT_FOUND;
    }
    /* get the result index */
    tmp = MAKE_FIELD(TR3_WRCMP_RSP_RESULT, tmp);

    /* check which host table the index falls in */
    /* tmp -= partitions[TCAM_PARTITION_FWD_IP4_UCAST].tcam_base; */
    for (part = TCAM_PARTITION_FWD_IP4_UCAST; 
         part <= TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE; part++) {
        if (tmp < (partitions[part].tcam_base+(partitions[part].num_entries <<
            partitions[part].tcam_width_shift))) {
            *index_ptr = (tmp - partitions[part].tcam_base) 
                          >> partitions[part].tcam_width_shift;
            *mem = part_to_mem[part - TCAM_PARTITION_FWD_IP4_UCAST];
            if (*mem != 0) {
                return SOC_E_NONE;
            } else {
                return SOC_E_NOT_FOUND;
            }
        }
/*
        tmp -= (partitions[part].num_entries << 
                partitions[part].tcam_width_shift);
*/
    }
    LOG_CLI((BSL_META_U(unit,
                        "Exiting with miss\n")));
    return SOC_E_NOT_FOUND;
}


/*
 * Function:
 *      soc_tr3_tcam_partition_get
 * Purpose:
 *      Maps EXT_* mem to TCAM parition number
 * Parameters:
 *      mem       (IN)   Memory name in EXT_* view.
 *      partition (OUT)  Partition number corresponding to the EXT mem
 * Returns:
 *	SOC_E_PARAM      Invalid memory
 *	SOC_E_NONE       Success
 */
STATIC int
soc_tr3_tcam_partition_get(int unit, soc_mem_t mem, int *partition)
{
    int part;

    if (partition == NULL) {
        return SOC_E_PARAM;
    }

    switch (mem) {
        case EXT_IPV4_UCASTm:
            part = TCAM_PARTITION_FWD_IP4_UCAST;
            break;
        case EXT_IPV4_UCAST_WIDEm:
            part = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            break;
        case EXT_IPV4_DEFIPm:
            part = TCAM_PARTITION_FWD_IP4;
            break;
        case EXT_IPV6_64_DEFIPm:
            part = TCAM_PARTITION_FWD_IP6U;
            break;
        case EXT_IPV6_128_UCASTm:
            part = TCAM_PARTITION_FWD_IP6_128_UCAST;
            break;
        case EXT_IPV6_128_UCAST_WIDEm:
            part = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            break;
        case EXT_IPV6_128_DEFIPm:
            part = TCAM_PARTITION_FWD_IP6;
            break;
        case EXT_ACL144_TCAM_L2m:
            part = TCAM_PARTITION_ACL_L2C;
            break;
        case EXT_ACL144_TCAM_IPV4m:
            part = TCAM_PARTITION_ACL_IP4C;
            break;
        case EXT_ACL144_TCAM_IPV6m:
            part = TCAM_PARTITION_ACL_IP6C;
            break;
        case EXT_ACL288_TCAM_L2m:
            part = TCAM_PARTITION_ACL_L2;
            break;
        case EXT_ACL288_TCAM_IPV4m:
            part = TCAM_PARTITION_ACL_IP4;
            break;
        case EXT_ACL432_TCAM_DATA_L2_IPV4m:
            part = TCAM_PARTITION_ACL_L2IP4;
            break;
        case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
            part = TCAM_PARTITION_ACL_IP6S;
            break;
        case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
            part = TCAM_PARTITION_ACL_IP6F;
            break;
        case EXT_ACL432_TCAM_DATA_L2_IPV6m:
            part = TCAM_PARTITION_ACL_L2IP6;
            break;
        default:
            return (SOC_E_PARAM);
    }

    *partition = part;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_tr3_get_vbit
 * Purpose:
 *      Gets valid status for a given memory index
 * Parameters:
 *      mem       (IN)   Memory name in EXT_* view.
 *      index     (IN)   Location of the entry in the memory
 *      partition (OUT)  Partition number corresponding to the EXT mem
 * Returns:
 *	SOC_E_PARAM      Invalid memory.
 *      SOC_E_XXX        Internal Error in accessing the tcam
 *	SOC_E_NONE       Success.
 */
int
soc_tr3_get_vbit(int unit, soc_mem_t mem, int index, int *valid)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int addr, len, i, tmp, part, rv;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_tr3_tcam_partition_get(unit, mem, &part));

    

    /* Read from TCAM */

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;

    len = 1 << tcam_info->partitions[part].tcam_width_shift;

    /* we only do a max of 480 bits */
    if (len == 8) {
        len = 6;
    }

    *valid = 1;
    for (i = 0; i < len; i++) {
        sal_memset(dbus, 0, sizeof(dbus));

        /* normalize address for cascaded mode */
        tmp = (((addr+i) / (128 * 4096)) << 23) | ((addr+i) % (128 * 4096));

        /* set bit 25 of addr to 1 to indicate database read */
        dbus[1] = MAKE_WORD(GET_BYTE(tmp, 0), GET_BYTE(tmp, 1),
                            GET_BYTE(tmp, 2),
                            (GET_BYTE(tmp, 3) & 0x3) | 0x2);

        ibus[0] = TR3_OPCODE_RD_REG_DBX;

        rv = soc_triumph3_tcam_access(unit, TCAM_TR3_OP_REG_DB_X_READ,
                                      1, 0, 0, (uint32 *)dbus, ibus, 2*64, 1);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }

        *valid &= ((dbus[3] >> 12) & 1);
    }

    return SOC_E_NONE;
}


/*
 *  Set/Clear the vbit of an external tcam entry
 */
int
soc_tr3_set_vbit(int unit, soc_mem_t mem, int index, int val)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int part, rv = 0, duplicate = 0;
    int physical_index;
    uint32 vbit_data, shift_count;
    ext_tcam_vbit_entry_t vbits;
   

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;

    switch (mem) {
    case EXT_IPV4_UCASTm:
        part = TCAM_PARTITION_FWD_IP4_UCAST;
        shift_count = 0;
        if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV4_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
        shift_count = 0;
        if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV4_DEFIPm:
        part = TCAM_PARTITION_FWD_IP4;
        shift_count = 0;
        if (partitions[TCAM_PARTITION_FWD_IP4_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV6_64_DEFIPm:
        part = TCAM_PARTITION_FWD_IP6U;
        shift_count = 0;
        if (partitions[TCAM_PARTITION_FWD_IP6U_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV6_128_UCASTm:
        part = TCAM_PARTITION_FWD_IP6_128_UCAST;
        shift_count = 1;
        if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV6_128_UCAST_WIDEm:
        part = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
        shift_count = 1;
        if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_IPV6_128_DEFIPm:
        part = TCAM_PARTITION_FWD_IP6;
        shift_count = 1;
        if (partitions[TCAM_PARTITION_FWD_IP6_DUP].num_entries) {
            duplicate=1;
        }
        break;
    case EXT_ACL144_TCAM_L2m:
        part = TCAM_PARTITION_ACL_L2C;
        shift_count = 1;
        break;
    case EXT_ACL144_TCAM_IPV4m:
        part = TCAM_PARTITION_ACL_IP4C;
        shift_count = 1;
        break;
    case EXT_ACL144_TCAM_IPV6m:
        part = TCAM_PARTITION_ACL_IP6C;
        shift_count = 1;
        break;
    case EXT_ACL288_TCAM_L2m:
        part = TCAM_PARTITION_ACL_L2;
        shift_count = 3;
        break;
    case EXT_ACL288_TCAM_IPV4m:
        part = TCAM_PARTITION_ACL_IP4;
        shift_count = 3;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV4m:
        part = TCAM_PARTITION_ACL_L2IP4;
        shift_count = 3;
        break;
    case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
        part = TCAM_PARTITION_ACL_IP6S;
        shift_count = 5;
        break;
    case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
        part = TCAM_PARTITION_ACL_IP6F;
        shift_count = 5;
        break;
    case EXT_ACL432_TCAM_DATA_L2_IPV6m:
        part = TCAM_PARTITION_ACL_L2IP6;
        shift_count = 5;
        break;
    default:
        return SOC_E_PARAM;
    }

    physical_index = partitions[part].tcam_base + 
                                (index << partitions[part].tcam_width_shift);

    sal_memset(&vbits, 0, sizeof(vbits));

    /* need to set correct bit in the returned data and return */
    if (val & 1) {
        /* set higher bit of entry to avoid unintended consequences */
        
          vbit_data = (1 << ((physical_index % 32) + 
                            partitions[part].tcam_width_shift));
    } else {
        vbit_data = ~(1 | (1 << shift_count));
    }
    soc_mem_field32_set(unit, EXT_TCAM_VBITm, (uint32 *) &vbits, VBITf,
                        vbit_data);
    if (duplicate) {
        soc_mem_field32_set(unit, EXT_TCAM_VBITm, (uint32 *) &vbits, DUPLICATEf,
                            duplicate);
    }
    
    /* write back modified data */
    rv = soc_mem_write(unit, EXT_TCAM_VBITm, MEM_BLOCK_ALL, physical_index, 
                      (void *)&vbits);
    if (rv < 0) return rv;
    return SOC_E_NONE;
}

#ifdef later
int
soc_tr3_configure_rpf(int unit, bool enable)
{
    /* currently a global setting */
    if (enable) {
        /* move all ports to urpf profile */
    } else {
        /* move all ports to base profile */
    }
    return SOC_E_NONE;
}
#endif

#if 0

/*
   NL11K external TCAM init sequences 
 */
int soc_tr3_nl11k_init(int unit) {

#define MAX_WCL_NUM 5
#define MASTER_WCL_ID 2
#define TX_PLL_LOCK_WAIT_TIME_USECS 60000000
#define NL_METAFRAME_LENGTH 2048
#define NL11K_MDIO_ADDRESS  0x101 
#define NL11K_DEVICE_CNT 2  
#define RX_FIFO_THRESHOLD 0x181F
#define TX_FIFO_THRESHOLD 0x181F



#define NL11K_COMMON_CONFIG_STATUS_BLK 1

#define RX_LANE_ENABLE_0_REG           0x8100
#define RX_LANE_ENABLE_1_REG           0x8101
#define TX_LANE_ENABLE_0_REG           0x8102
#define CRX_LANE_ENABLE_0_REG          0x8103
#define CTX_LANE_ENABLE_0_REG          0x8104
#define CTX_LANE_ENABLE_1_REG          0x8105
#define RX_METAFRAME_LEN_REG           0x8106
#define TX_METAFRAME_LEN_REG           0x8107
#define LANE_SWAP_REG                  0x8108
#define RX_FIFO_THRESHOLD_REG          0x8109
#define TX_FIFO_THRESHOLD_REG          0x810A
#define TX_BURST_SHORT_REG             0x810B
#define GLOBAL_RX_TX_ENABLE_REG        0x810C
#define GLOBAL_SW_RESET_REG            0x811C
#define TX_SERDES_SQUELCH_11_00_REG    0x8117    
#define CTX_SERDES_SQUELCH_15_00_REG   0x8118
#define CTX_SERDES_SQUELCH_23_16_REG   0x8119
#define SERDES_SW_RESET_REG            0x811B
#define SPEED_MODE_SELECT_REG          0x811D
#define GEN_PURPOSE_STATUS_REG_2       0x8183

    uint32 etu_cfg4 = 0, temp32 = 0;
    uint16 temp16=0;
    soc_timeout_t    to;
    sal_usecs_t		timeout_usecs = TX_PLL_LOCK_WAIT_TIME_USECS;
    int  rv;
    /* int  cond;*/
    int index=0, temp1 = 0, temp2 = 0, temp3 = 0, cond;


    

    

    
    /*
     *-------------------------------------------------- 
     * Reset: 
     * ext_tcam, 
     * wcl, 
     * ilamac 
     *--------------------------------------------------
     */

    /* Reset tcam, first de-assert it(high) and then assert(pull low)
     */ 
    SOC_IF_ERROR_RETURN(READ_ETU_CONFIG4r(unit, &etu_cfg4));
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_SRST_Lf, 1);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_CRST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_SRST_Lf, 0);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_CRST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));

    /* Reset all wcl */
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        
        soc_reg_field_set(unit, WCL_CTLr, &temp32, PWRDWNf, 1);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, IDDQf, 1);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_HWf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_MDIOREGSf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_PLLf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, TXD1G_FIFO_RSTBf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, PLL_BYPASSf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, LCREF_ENf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, REFOUT_ENf, 0);

        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }

    /* Reset ILAMAC 
     */ 
    SOC_IF_ERROR_RETURN(READ_ETU_CONFIG4r(unit, &etu_cfg4));
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, ILAMAC_RX_SERDES_RST_f, 0);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, ILAMAC_RX_LBUS_RST_f, 0);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, ILAMAC_TX_SERDES_RST_f, 0);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, ILAMAC_TX_LBUS_RST_f, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));
  
   /* tx_serdes_refclk mux select. select one txck from 24 lanes. Default = 0x9     */
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, ILAMAC_TX_SERDES_REFCLK_SELf, 0x9);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));



    /* 
     *-------------------------------------------------- 
     * WCL Init Sequence
     *-------------------------------------------------- 
     */


    /*
      wcl_ctl_pwrdwn from etu controls both pwrdwn_tx and pwrdwn_rx,
      so if we want to turn-off rx only, then we will have to do it thro mdio
     */
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        soc_reg_field_set(unit, WCL_CTLr, &temp32, PWRDWNf, 0);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }

    /* wcl - master, slave clock assignment 
     */
    temp32 = 0;
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));

        temp1 = (index == MASTER_WCL_ID)? 0 : 1; /* lcref_en */ 
        temp2 = (index == MASTER_WCL_ID)? 1 : 0; /* refout_en */ 

        soc_reg_field_set(unit, WCL_CTLr, &temp32, LCREF_ENf, temp1);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, REFOUT_ENf, temp2);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }

    /* Sleep 1ms */
    sal_usleep(1000);


    /* wcl -  Deassert pwrdwn and iddq */
    temp32 = 0;
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        soc_reg_field_set(unit, WCL_CTLr, &temp32, PWRDWNf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &temp32, IDDQf, 0);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }
    /* wcl -  Deassert RSTB_HW */
    temp32 = 0;
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_HWf, 1);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }
    /* wcl - Deassert MDIOREGS */
    temp32 = 0;
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_MDIOREGSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }

    /* 
     *-------------------------------------------------- 
     * ESM SERDES INIT - wcmod driver??? - ESM serdes init - pll...
         to run the tests if mdio access is working
     *-------------------------------------------------- 
    */
    wcmod_esm_serdes_init(unit); 

    /* wcl - Deassert RSTB_PLL */
    temp32 = 0;
    for(index=0; index <= MAX_WCL_NUM; ++index) {
        SOC_IF_ERROR_RETURN(READ_WCL_CTLr(unit, index, &temp32));
        soc_reg_field_set(unit, WCL_CTLr, &temp32, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(WRITE_WCL_CTLr(unit, index, temp32));
    }

    /* Tx PLL Lock */
    temp32 = 0;
    soc_timeout_init(&to, timeout_usecs, 0);
    do {
        SOC_IF_ERROR_RETURN(READ_WCL_CUR_STSr(unit, &temp32));
        cond = temp32 & 0x3f000;
        if (cond || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    /* Check again */
    temp32 = 0;
    SOC_IF_ERROR_RETURN(READ_WCL_CUR_STSr(unit, &temp32));
    if((temp32 & 0x3f000) || SOC_FAILURE(rv)){
        return  SOC_E_INIT; /* PLL not locked, error */
    }

    /*
     *-------------------------------------------------- 
     * Configure ILAMAC 
     *-------------------------------------------------- 
     */
    SOC_IF_ERROR_RETURN(READ_ILAMAC_RX_CONFIGr(unit, &temp32));

    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, BIT_ORDER_INVERTf, 1);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_XOFF_MODEf, 0);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_RLIM_ENABLEf, 0);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_RDYOUT_THRESHf, 0);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_DISABLE_SKIPWORDf, 0);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_BURSTSHORTf, 1);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_BURSTMAXf, 1);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_MFRAMELEN_MINUS1f, NL_METAFRAME_LENGTH - 1);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, TX_ENABLEf, 1);
    soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &temp32, XON_RX_CH0f, 1);
    SOC_IF_ERROR_RETURN(WRITE_ILAMAC_TX_CONFIG0r(unit, temp32));
    
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &temp32, BIT_ORDER_INVERTf, 1);
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &temp32, RX_BURSTMAXf, 1);
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &temp32, RX_MFRAMELEN_MINUS1f, NL_METAFRAME_LENGTH - 1);
    SOC_IF_ERROR_RETURN(WRITE_ILAMAC_RX_CONFIGr(unit, temp32));

    /*
       Now that tx_serdes_clk is stable, we can bring tx_ilamac out of reset
       by first de-asserting lbus_rst, and then serdes_rst
    */
    SOC_IF_ERROR_RETURN(READ_ETU_CONFIG4r(unit, &temp32));

    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, ILAMAC_TX_LBUS_RST_f, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));
    
    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, ILAMAC_TX_SERDES_RST_f, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));

    /* Now connect ilamac_tx to serdes */
    soc_reg_field_set(unit, ETU_BIST_CTLr, &temp32, ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));



    

    /*
     * At this point, we should be sending out idle control words 
     * to NL (which is still in reset) 
     * We can de-assert SRST_L for NL only after we are sending 
     * idle control words
    */

    /*
     *-------------------------------------------------- 
     * NL11K init, section 6.6 NL datasheet
     *-------------------------------------------------- 
    */

    /* 
      1. Ensure NL's SRST, CRST are de-asserted (pulled high) 
      and then assert SRST, CRST.....skip this step as resets for 
      NL11K are already asserted at this point.
    */

#if 0
    /* Reset tcam, first de-assert it(high) and then assert(pull low) */
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_SRST_Lf, 1);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_CRST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));
    etu_cfg4=0;
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_SRST_Lf, 0);
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_cfg4, EXT_TCAM_CRST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, etu_cfg4));

#endif

    /* 2. Wait for 3.0usec and de-assert SRST_L */
    sal_usleep(3);
    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, EXT_TCAM_SRST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));

    /* 3. MDIO based device configuration */

    /* 
     *  Squelch the Transmit SerDes
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    TX_SERDES_SQUELCH_11_00_REG,
                                    0xfff);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_SERDES_SQUELCH_15_00_REG,
                                    0xffff);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_SERDES_SQUELCH_23_16_REG,
                                    0xff);
    }


    /* 
     *  Disable RX and TX PCS using Global RX / TX Enable register
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    GLOBAL_RX_TX_ENABLE_REG,
                                    0x00);
    }
    
    /*
     *  Reset SerDes-Initialization state machine, Core PLL and 
     *  Core Logic by writing 16'h7 to Global SW Reset register
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    GLOBAL_SW_RESET_REG,
                                    0x07);
    }

    /* 
     *  Configure MDIO registers 
 */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    GLOBAL_SW_RESET_REG,
                                    0x07);
    }
   
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
#if 0
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    LANE_SWAP_REG,
                                    NL_METAFRAME_LENGTH);
#endif
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    RX_METAFRAME_LEN_REG,
                                    NL_METAFRAME_LENGTH);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    TX_METAFRAME_LEN_REG,
                                    NL_METAFRAME_LENGTH);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    RX_FIFO_THRESHOLD_REG,
                                    RX_FIFO_THRESHOLD);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    TX_FIFO_THRESHOLD_REG,
                                    TX_FIFO_THRESHOLD);
#if 0
        /* TX Burst Short (0 means 8 bytes, 1 means 16 bytes) */
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    TX_BURST_SHORT_REG,
                                    0x1);
        /* Speed mode select, bits 2:0 select speed for rx serdes, 
           bits 6:4 select speed for tx serdes 
		   3'b100 means 6.250 Gbps, 3'b000 means 3.125 Gbps
         */
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    SPEED_MODE_SELECT_REG,
                                    0x00);
#endif
    }

#if 0
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        /* RX Lane Enable 0 (RX lane enable for lanes 15 to 0) */
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    RX_LANE_ENABLE_0_REG,
                                    0xffff);
        /* RX Lane Enable 1 (RX lane enable for lanes 23 to 16) */
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    RX_LANE_ENABLE_1_REG,
                                    0xff);
        /* TX Lane Enable 0 (TX lane enable for lanes 11 to 0) */
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    NL11K_COMMON_CONFIG_STATUS_BLK, 
                                    TX_LANE_ENABLE_0_REG,
                                    0xfff);

    }
#endif

    /* enable CTX lanes for DEV0, DEV1, DEV2 
       (all not_last devices) 
     */  
    temp1 = 0xffff;
    temp2 = 0xff;
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        if( index +1 == NL11K_DEVICE_CNT) {
            /* For the last device in cascade */
            temp1 = 0x0000;
            temp2 = 0x0000;
        }
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_LANE_ENABLE_0_REG,
                                    temp1);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_LANE_ENABLE_1_REG,
                                    temp2);

    }

    /*
       Enable CRX lanes for DEV1, DEV2, DEV3 
       (all not_first devices)
     */
     temp1 = 0xfff;
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        if(index == 0) {
            /* For the first device in cascade */
            temp1 = 0x0000;
        }
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CRX_LANE_ENABLE_0_REG,
                                    temp1);
    }

    /*  
        Deassert Core PLL and Core Logic reset, and 
        trigger the SerDes-Initialization state
    */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    GLOBAL_SW_RESET_REG,
                                    0x0);
    }



    /* Poll for assertion of bit[3], SerDes Reset Sequence Done, 
       in the General Purpose Status 
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_timeout_init(&to, timeout_usecs, 0);
        do {
            rv = soc_esw_miimc45_read(unit, NL11K_MDIO_ADDRESS + index,
                                        GEN_PURPOSE_STATUS_REG_2,
                                        &temp16);
            cond = temp16 & 0x8;
            if (cond || SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&to));

        /* Check again */
        temp16 = 0;
        rv = soc_esw_miimc45_read(unit, NL11K_MDIO_ADDRESS + index,
                                        GEN_PURPOSE_STATUS_REG_2,
                                        &temp16);
        if((temp16 & 0x8) || SOC_FAILURE(rv)){
            return  SOC_E_FAIL; /* Serdes Reset Sequence not done */
        }
    }

    /* 
       Enable RX and TX PCS using Global RX / TX Enable register
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    GLOBAL_RX_TX_ENABLE_REG,
                                    0x3);

    }

    /*
       Disable squelch for the Transmit SerDes using TX_SerDes_11_0_squelch,
       CTX_SerDes_15_0_squelch, and CTX_SerDes_23_16_squelch registers.
     */
    temp1 = temp2 = temp3 = 0;
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        if( index + 1 == NL11K_DEVICE_CNT) {
           /* For last device in cascade - 
              leave squelching enabled for CTX - 
              as these connect to nowhere 
            */
            temp1 = 0xfff;
            temp2 = 0xffff;
            temp3 = 0xff;
        }
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    TX_SERDES_SQUELCH_11_00_REG,
                                    temp1);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_SERDES_SQUELCH_15_00_REG,
                                    temp2);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    CTX_SERDES_SQUELCH_23_16_REG,
                                    temp3);
    }


    /*
       4. Wait for 2.5 mS and de-assert NL's CRST
     */
    temp32 = 0;
    sal_usleep(25000); /*2.5 msec*/
    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, EXT_TCAM_CRST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));

    /*
       5. Interlaken Look-Aside framing and synchronization. Idle control 
          words will be sent on the response port with XOFF until 
          synchronization is complete. It is expected that only idle control 
          words are sent on the RX port until XON is provided on the TX port.
     */

    /* 
       Rx portion of wcl must now be seeing idle words sent from NL and 
       so must be able to recover clock 
       - check if wcl has recovered rx_clk for each lane
     */
    temp32 = 0;
    soc_timeout_init(&to, timeout_usecs, 0);
    do {
        rv = READ_WCL_CUR_STSr(unit, &temp32);
        cond = temp32 & 0xfff;
        if (cond || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));

    /* Check again */
    temp32 = 0;
    rv = READ_WCL_CUR_STSr(unit, &temp32);
    if((temp32 & 0xfff) || SOC_FAILURE(rv)){
        return  SOC_E_FAIL; /* RXLANE_LOCK is not seen */
    }

    /*
       Once rx_lane_clk (rx_serdes_clk) is stable, we can bring rx_ilamac 
       out of reset by first de-asserting lbus_rst, and then serdes_rst
     */
    temp32 = 0;
    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, ILAMAC_RX_LBUS_RST_f, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));
    temp32 = 0;
    soc_reg_field_set(unit, ETU_CONFIG4r, &temp32, ILAMAC_RX_SERDES_RST_f, 0xfff);
    SOC_IF_ERROR_RETURN(WRITE_ETU_CONFIG4r(unit, temp32));


    /* Wait for rx_ilamac sync */
    temp32 = 0;
    soc_timeout_init(&to, timeout_usecs, 0);
    do {
        rv = READ_ILAMAC_RX_INTF_STATE0r(unit, &temp32);
        /*[27] = 1, [26]=1, [25,24]=0, [23:12] = 0xfff, [11:0] = 0xfff */
        cond = temp32 & 0xCffffff; 
        if (cond || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));

    /* Check again */
    temp32 = 0;
    rv = READ_ILAMAC_RX_INTF_STATE0r(unit, &temp32);
    if((temp32 & 0xCffffff) || SOC_FAILURE(rv)){
        return  SOC_E_FAIL; /* RXLANE_LOCK is not seen */
    }


    


    /*
       6. Assert, then deassert "Reset RX Satellite Sticky Registers" bit 
       in SerDes SW Reset register. 
     */
    for (index = 0; index < NL11K_DEVICE_CNT; ++index) {
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    SERDES_SW_RESET_REG,
                                    0x40);
        soc_esw_miimc45_write(unit, NL11K_MDIO_ADDRESS + index,
                                    SERDES_SW_RESET_REG,
                                    0x00);

    }


    /*
       7. Device is ready for normal operation.
     */

    

    return SOC_E_NONE;
}
#endif


#endif /* BCM_TRIUMPH3_SUPPORT */
