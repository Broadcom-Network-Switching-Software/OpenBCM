/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:     Tomahawk3 multicast replication implementation.
 */
#ifdef BCM_TOMAHAWK3_SUPPORT

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/l3.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk3.h>
#include <soc/flexport/tomahawk3_mmu_flexport.h>

STATIC int
_bcm_th3_repl_head_tbl_read(int unit, int index, uint32 *entry, uint32 *head_ptr)
{
    soc_mem_t repl_hd_tbl;

    repl_hd_tbl = MMU_REPL_HEAD_TBLm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                     index, entry));
    if (head_ptr) {
        *head_ptr = soc_mem_field32_get(unit, repl_hd_tbl, entry, HEAD_PTRf);
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_repl_head_tbl_write(int unit, int index, uint32 *entry)
{
    soc_mem_t repl_hd_tbl;
    repl_hd_tbl = MMU_REPL_HEAD_TBLm;
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                          index, entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_th3_repl_list_start_ptr_get
 * Purpose:
 *      Get replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_th3_repl_list_start_ptr_get(int unit, int repl_group, bcm_port_t port,
        int *start_ptr)
{
    soc_mem_t repl_group_table;
    soc_field_t member_bitmap_f, head_index_f;
    int member_bitmap_index;
    mmu_repl_group_info_tbl_entry_t repl_group_entry;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;
    soc_pbmp_t member_bitmap;
    int member_id, member;
    int head_index;
    uint32 repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];

    member_bitmap_f = MEMBER_BMPf;
    head_index_f = BASE_PTRf;

#ifdef INCLUDE_L3
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_to_aggid(unit, port, &member_bitmap_index));
    } else
#endif
    {
        /* Get member bitmap index */
        member_bitmap_index = port;
    }

    /* Get member bitmap */
    repl_group_table = MMU_REPL_GROUP_INFO_TBLm;

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                      repl_group, &repl_group_entry));
    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, (uint32 *)&repl_group_entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        SOC_PBMP_WORD_SET(member_bitmap, i, fldbuf[i]);
    }

    /* Check if port is in member bitmap */
    if (!SOC_PBMP_MEMBER(member_bitmap, member_bitmap_index)) {
        *start_ptr = 0;
        return BCM_E_NONE;
    }

    /* Get replication list start pointer */
    member_id = 0;
    SOC_PBMP_ITER(member_bitmap, member) {
        if (member == member_bitmap_index) {
            break;
        }
        member_id++;
    }
    head_index = member_id + soc_mem_field32_get(unit, repl_group_table,
            &repl_group_entry, head_index_f);

    SOC_IF_ERROR_RETURN
            (_bcm_th3_repl_head_tbl_read(unit, head_index,
                                     repl_head_entry, (uint32 *)start_ptr));


    return BCM_E_NONE;
}

STATIC int
_bcm_th3_initial_copy_count_set(int unit, int repl_head_ptr, int intf_count)
{
    /* each replication head entry is associated with a ICC entry*/
    int icc_entry_ptr;
    int icc_field_index;
    mmu_repl_member_icc_entry_t entry;
    soc_mem_t mem;
    int icc_val;
    soc_field_t icc_fields[] = {
                              ICC_VAL0f, ICC_VAL1f, ICC_VAL2f, ICC_VAL3f,
                              ICC_VAL4f, ICC_VAL5f, ICC_VAL6f, ICC_VAL7f,
                              ICC_VAL8f, ICC_VAL9f, ICC_VAL10f, ICC_VAL11f,
                              ICC_VAL12f, ICC_VAL13f, ICC_VAL14f, ICC_VAL15f,
                              ICC_VAL16f, ICC_VAL17f, ICC_VAL18f, ICC_VAL19f,
                              ICC_VAL20f, ICC_VAL21f, ICC_VAL22f, ICC_VAL23f,
                              ICC_VAL24f, ICC_VAL25f, ICC_VAL26f, ICC_VAL27f,
                              ICC_VAL28f, ICC_VAL29f, ICC_VAL30f, ICC_VAL31f,
                              ICC_VAL32f, ICC_VAL33f, ICC_VAL34f, ICC_VAL35f,
                              ICC_VAL36f, ICC_VAL37f, ICC_VAL38f, ICC_VAL39f,
                              ICC_VAL40f, ICC_VAL41f, ICC_VAL42f, ICC_VAL43f,
                              ICC_VAL44f, ICC_VAL45f, ICC_VAL46f, ICC_VAL47f,
                              ICC_VAL48f, ICC_VAL49f, ICC_VAL50f, ICC_VAL51f,
                              ICC_VAL52f, ICC_VAL53f, ICC_VAL54f, ICC_VAL55f,
                              ICC_VAL56f, ICC_VAL57f, ICC_VAL58f, ICC_VAL59f,
                              ICC_VAL60f, ICC_VAL61f, ICC_VAL62f, ICC_VAL63f,

                             };
    icc_entry_ptr = repl_head_ptr / TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY;
    icc_field_index = repl_head_ptr % TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY;
    /* ICC value is equal to inft_count for repl counts 1-31 */
    /* ICC value will be 0 for repl_count = 0 or > 31*/
    icc_val = intf_count > 31 ? 0 : intf_count;
    mem = MMU_REPL_MEMBER_ICCm;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, icc_entry_ptr, &entry));
    soc_MMU_REPL_MEMBER_ICCm_field32_set(unit, &entry,
                    icc_fields[icc_field_index], icc_val);

    SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, icc_entry_ptr, &entry));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_initial_copy_count_get(int unit, int repl_head_ptr, int *intf_count)
{
    /* each replication head entry is associated with a ICC entry*/
    int icc_entry_ptr;
    int icc_field_index;
    mmu_repl_member_icc_entry_t entry;
    soc_mem_t mem;
    soc_field_t icc_fields[] = {
                              ICC_VAL0f, ICC_VAL1f, ICC_VAL2f, ICC_VAL3f,
                              ICC_VAL4f, ICC_VAL5f, ICC_VAL6f, ICC_VAL7f,
                              ICC_VAL8f, ICC_VAL9f, ICC_VAL10f, ICC_VAL11f,
                              ICC_VAL12f, ICC_VAL13f, ICC_VAL14f, ICC_VAL15f,
                              ICC_VAL16f, ICC_VAL17f, ICC_VAL18f, ICC_VAL19f,
                              ICC_VAL20f, ICC_VAL21f, ICC_VAL22f, ICC_VAL23f,
                              ICC_VAL24f, ICC_VAL25f, ICC_VAL26f, ICC_VAL27f,
                              ICC_VAL28f, ICC_VAL29f, ICC_VAL30f, ICC_VAL31f,
                              ICC_VAL32f, ICC_VAL33f, ICC_VAL34f, ICC_VAL35f,
                              ICC_VAL36f, ICC_VAL37f, ICC_VAL38f, ICC_VAL39f,
                              ICC_VAL40f, ICC_VAL41f, ICC_VAL42f, ICC_VAL43f,
                              ICC_VAL44f, ICC_VAL45f, ICC_VAL46f, ICC_VAL47f,
                              ICC_VAL48f, ICC_VAL49f, ICC_VAL50f, ICC_VAL51f,
                              ICC_VAL52f, ICC_VAL53f, ICC_VAL54f, ICC_VAL55f,
                              ICC_VAL56f, ICC_VAL57f, ICC_VAL58f, ICC_VAL59f,
                              ICC_VAL60f, ICC_VAL61f, ICC_VAL62f, ICC_VAL63f,

                             };

    if(intf_count == NULL) {
        return BCM_E_PARAM;
    }
    icc_entry_ptr = repl_head_ptr / TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY;
    icc_field_index = repl_head_ptr % TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY;

    mem = MMU_REPL_MEMBER_ICCm;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, icc_entry_ptr, &entry));
    *intf_count = soc_MMU_REPL_MEMBER_ICCm_field32_get(unit, &entry,
                    icc_fields[icc_field_index]);

    return BCM_E_NONE;
}

/* Function:
 *	bcm_th3_repl_list_start_ptr_set
 * Purpose:
 *      Set replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - Replication list's start pointer.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_th3_repl_list_start_ptr_set(int unit, int repl_group, bcm_port_t port,
                int start_ptr, int intf_count)
{
    int add_member;
    soc_mem_t repl_group_table;
    soc_mem_t repl_head_table;
    soc_field_t member_bitmap_f, head_index_f;
    int member_bitmap_index;
    int pipe;
    mmu_repl_group_info_tbl_entry_t repl_group_entry;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;
    soc_pbmp_t old_member_bitmap;
    soc_pbmp_t new_member_bitmap;
    int old_member_count, new_member_count = 0;
    int old_head_index, new_head_index;
    int member_id, member, old_member_id, new_member_id;
    uint32 repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 old_repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];
    int member_bitmap_len;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    int itm, rv;
    int reserved_resources_used = 0;
    int reserve_multicast_resources;
    int repl_head_tbl_size;

    if (start_ptr > 0) {
        add_member = TRUE;
    } else {
        add_member = FALSE;
    }
    reserve_multicast_resources =
        soc_property_get(unit, spn_RESERVE_MULTICAST_RESOURCES, 0) ? 1 : 0;
    repl_head_tbl_size = 1 << soc_mem_field_length(unit,
            MMU_REPL_GROUP_INFO_TBLm, BASE_PTRf);
    

    member_bitmap_f = MEMBER_BMPf;
    head_index_f = BASE_PTRf;

#ifdef INCLUDE_L3
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_to_aggid(unit, port, &member_bitmap_index));
    } else
#endif
    {
        /* Get member bitmap index */
        member_bitmap_index = port;
    }

    member_bitmap_len = soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm,
                                             member_bitmap_f);
    if (member_bitmap_index >= member_bitmap_len) {
        return BCM_E_PARAM;
    }

    /* Get old member bitmap and old head index */
    pipe = 0;

    repl_head_table = MMU_REPL_HEAD_TBLm;

    repl_group_table = MMU_REPL_GROUP_INFO_TBLm;

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                      repl_group, &repl_group_entry));
    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, (uint32 *)&repl_group_entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        SOC_PBMP_WORD_SET(old_member_bitmap, i, fldbuf[i]);
    }
    SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
    SOC_PBMP_COUNT(old_member_bitmap, old_member_count);
    old_head_index = soc_mem_field32_get(unit, repl_group_table,
            &repl_group_entry, head_index_f);

    if (add_member) {
        /* Update REPL_HEAD table */
        sal_memset(repl_head_entry, 0, sizeof(repl_head_entry));
        soc_mem_field32_set(unit, repl_head_table, repl_head_entry,
                            HEAD_PTRf, start_ptr);
        if (SOC_PBMP_MEMBER(old_member_bitmap, member_bitmap_index)) {
            /* Port is already a member of the group */
            member_id = 0;
            SOC_PBMP_ITER(old_member_bitmap, member) {
                if (member == member_bitmap_index) {
                    break;
                }
                member_id++;
            }
            SOC_IF_ERROR_RETURN
                (_bcm_th3_repl_head_tbl_write(unit,
                 (old_head_index + member_id), repl_head_entry));
            BCM_IF_ERROR_RETURN
                (_bcm_th3_initial_copy_count_set(unit,
                          (old_head_index + member_id), intf_count));
            new_head_index = old_head_index;
        } else {
            SOC_PBMP_PORT_ADD(new_member_bitmap, member_bitmap_index);
            new_member_count = old_member_count + 1;
            rv = (bcm_th3_repl_head_alloc(unit,
                        0, new_member_count, &new_head_index));
            if (BCM_FAILURE(rv)) {
                if (reserve_multicast_resources == 0 || repl_group != 0) {
                    return rv;
                } else if (rv != BCM_E_RESOURCE) {
                    return rv;
                }
                /* Device is TH3, reserve multicast resources is 1 and
                 * delete has resource issue. */
                new_head_index = repl_head_tbl_size - 1 - member_bitmap_len;
                reserved_resources_used = 1;
            }
            old_member_id = 0;
            new_member_id = 0;
            SOC_PBMP_ITER(new_member_bitmap, member) {
                int old_icc = 0;
                if (member == member_bitmap_index) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_th3_repl_head_tbl_write(unit,
                                       (new_head_index + new_member_id),
                                       repl_head_entry));
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_initial_copy_count_set(unit,
                            (new_head_index + new_member_id), intf_count));
                } else {
                    SOC_IF_ERROR_RETURN
                        (_bcm_th3_repl_head_tbl_read(unit,
                                      (old_head_index + old_member_id),
                                      old_repl_head_entry, NULL));
                    SOC_IF_ERROR_RETURN(_bcm_th3_initial_copy_count_get(unit,
                                   (old_head_index + old_member_id), &old_icc));
                    SOC_IF_ERROR_RETURN
                        (_bcm_th3_repl_head_tbl_write(unit,
                                       (new_head_index + new_member_id),
                                       old_repl_head_entry));
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_initial_copy_count_set(unit,
                              (new_head_index + new_member_id), old_icc));
                    old_member_id++;
                }
                new_member_id++;
            }
        }

        /* Update REPL_GROUP entry */
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            fldbuf[i] = SOC_PBMP_WORD_GET(new_member_bitmap, i);
        }
        soc_mem_field_set(unit, repl_group_table, (uint32 *)&repl_group_entry,
                member_bitmap_f, fldbuf);
        soc_mem_field32_set(unit, repl_group_table, &repl_group_entry,
                head_index_f, new_head_index);

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_table, MEM_BLOCK_ALL,
                           repl_group, &repl_group_entry));
        if (!SAL_BOOT_SIMULATION) {
            /* mmu rqe Flush*/
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (itm_map & (1 << itm)) {
                    /* Issue RQE flush twice per each ITM*/
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                }
            }
        }

        /* Release old block of REPL_HEAD entries */
        if (old_member_count > 0 && old_head_index != new_head_index) {
            BCM_IF_ERROR_RETURN(bcm_th3_repl_head_free(unit,
                        pipe, old_head_index, old_member_count));
        }


    } else { /* Remove member from replication group */

        if (!SOC_PBMP_MEMBER(old_member_bitmap, member_bitmap_index)) {
            /* Port is not a member. Nothing more to do. */
            return BCM_E_NONE;
        }

        /* Update REPL_HEAD table */
        new_member_count = old_member_count - 1;
        if (new_member_count > 0) {
            rv = (bcm_th3_repl_head_alloc(unit,
                        0, new_member_count, &new_head_index));
            if (BCM_FAILURE(rv)) {
                if (reserve_multicast_resources == 0) {
                    return rv;
                } else if (rv != BCM_E_RESOURCE) {
                    return rv;
                }
                /* Device is TH3, reserve multicast resources is 1 and
                 * delete has resource issue. */
                new_head_index = repl_head_tbl_size - 1 - member_bitmap_len;
                reserved_resources_used = 1;
            }
            old_member_id = 0;
            new_member_id = 0;
            SOC_PBMP_ITER(old_member_bitmap, member) {
                if (member != member_bitmap_index) {
                    int old_icc = 0;
                    SOC_IF_ERROR_RETURN
                        (_bcm_th3_repl_head_tbl_read(unit,
                                      (old_head_index + old_member_id),
                                      old_repl_head_entry, NULL));
                    SOC_IF_ERROR_RETURN(_bcm_th3_initial_copy_count_get(unit,
                                   (old_head_index + old_member_id), &old_icc));
                    SOC_IF_ERROR_RETURN
                        (_bcm_th3_repl_head_tbl_write(unit,
                                       (new_head_index + new_member_id),
                                       old_repl_head_entry));
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_initial_copy_count_set(unit,
                            (new_head_index + new_member_id), old_icc));
                    new_member_id++;
                }
                old_member_id++;
            }
        } else {
            new_head_index = 0;
        }

        /* Update REPL_GROUP entry */
        SOC_PBMP_PORT_REMOVE(new_member_bitmap, member_bitmap_index);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            fldbuf[i] = SOC_PBMP_WORD_GET(new_member_bitmap, i);
        }
        soc_mem_field_set(unit, repl_group_table, (uint32 *)&repl_group_entry,
                member_bitmap_f, fldbuf);
        soc_mem_field32_set(unit, repl_group_table, &repl_group_entry,
                head_index_f, new_head_index);

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_table, MEM_BLOCK_ALL,
                           repl_group, &repl_group_entry));

        if (!SAL_BOOT_SIMULATION) {
            /* mmu rqe Flush*/
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (itm_map & (1 << itm)) {
                    /* Issue RQE flush twice per each ITM*/
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                }
            }
        }
        /* Release old block of REPL_HEAD entries */
        BCM_IF_ERROR_RETURN(bcm_th3_repl_head_free(unit,
                    pipe, old_head_index, old_member_count));
    }
    if (reserved_resources_used == 1) {
        /* When reserved resources are used, the entries need to moved to
         * regular space once old index is freed. */
        old_head_index = repl_head_tbl_size - 1 - member_bitmap_len;
        BCM_IF_ERROR_RETURN
            (bcm_th3_repl_head_alloc(unit,
                    pipe, new_member_count, &new_head_index));
        old_member_id = 0;
        new_member_id = 0;
        SOC_PBMP_ITER(new_member_bitmap, member) {
            if (member != member_bitmap_index) {
                SOC_IF_ERROR_RETURN
                    (_bcm_th3_repl_head_tbl_read(unit,
                                  (old_head_index + old_member_id),
                                  old_repl_head_entry, NULL));
                SOC_IF_ERROR_RETURN
                    (_bcm_th3_repl_head_tbl_write(unit,
                                   (new_head_index + new_member_id),
                                   old_repl_head_entry));
                new_member_id++;
            }
            old_member_id++;
        }
        /* Update REPL_GROUP pointing to HEAD INDEX */
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            fldbuf[i] = SOC_PBMP_WORD_GET(new_member_bitmap, i);
        }
        soc_mem_field_set(unit, repl_group_table, (uint32 *)&repl_group_entry,
                member_bitmap_f, fldbuf);
        soc_mem_field32_set(unit, repl_group_table, &repl_group_entry,
                head_index_f, new_head_index);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_table, MEM_BLOCK_ALL,
                           repl_group, &repl_group_entry));
        if (!SAL_BOOT_SIMULATION) {
            /* mmu rqe Flush*/
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (itm_map & (1 << itm)) {
                    /* Issue RQE flush twice per each ITM*/
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                   rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                   if (rv != BCM_E_NONE) {
                       return BCM_E_INTERNAL;
                   }
                }
            }
        }
    }

    return BCM_E_NONE;
}

#endif
