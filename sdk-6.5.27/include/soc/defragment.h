/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Routines for defragmenting member tables.
 */

#ifndef _SOC_DEFRAGMENT_H
#define _SOC_DEFRAGMENT_H

#include <sal/types.h>
#include <soc/mcm/enum_types.h>

/* Defragmentation block group ID initialization value. */
#define SOC_DEFRAGMENT_BLK_GRP_INVALID (-1)

/*
 * Typedef:
 *     soc_defragment_block_t
 *
 * Purpose:
 *     Indexed table block location and size details. The group ID value is
 *     set and valid only for in use blocks. For free blocks, it will be
 *     initialized to -1.
 *
 */
typedef struct soc_defragment_block_s {
    int base_ptr; /* Starting entry index of the block */
    int size;     /* Number of entries in the block */
    int group;    /* The group this block belongs to */
} soc_defragment_block_t;

typedef struct soc_defragment_member_op_s {
    int (*member_copy) (int unit, int src_index, int dst_index);
    int (*member_clear) (int unit, int index);
} soc_defragment_member_op_t;

typedef struct soc_defragment_group_op_s {
    int (*group_base_ptr_update) (int unit, int group, int base_ptr);
} soc_defragment_group_op_t;

/*
 * Typedef:
 *     soc_defragment_blk_mov_seq_t
 *
 * Purpose:
 *     Indexed table defragmentation block move sequence.
 *
 */
typedef struct soc_defragment_blk_mov_seq_s {
    /* Total number of moves required for this block. */
    uint32 mov_count;

    /*
     * The list of destination base pointer locations to be used for moving
     * the block while defragmenting the table. For blocks that undergo
     * multi-hop move sequence, the previous destination base_ptr location in
     * this list is the new source base_ptr location for retrieving the block
     * and moving it to the next destination base_ptr location provided in this
     * list for this block. The last base_ptr value in this list is the final
     * destination location for this block.
     */
    int *base_ptr;

    /* Total number of entries in this block. */
    int size;
} soc_defragment_blk_mov_seq_t;

/*
 * Typedef:
 *     soc_defragment_mem_info_t
 *
 * Purpose:
 *     Defragmentation memory details.
 *
 */
typedef struct soc_defragment_mem_info_s {
    /* Memory with the block base_ptr and size fields. */
    soc_mem_t base_mem;

    /* Block Base index field enum value. */
    soc_field_t base_ptr_field;

    /* Block size field enum value. */
    soc_field_t size_field;

    /* Memory to be defragmented. */
    soc_mem_t dfg_mem;

    /* Start defragmenting the dfg_mem table from this entry index. */
    int first_entry_index;

    /* Stop defragmenting the dfg_mem table at this entry index. */
    int last_entry_index;
} soc_defragment_mem_info_t;

extern void
soc_defragment_block_t_init(
    soc_defragment_block_t *blk);

extern void
soc_defragment_mem_info_t_init(
    soc_defragment_mem_info_t *mem_info);

extern int
soc_defragment_sequence_get(
    int unit,
    soc_defragment_mem_info_t mem_info,
    uint32 inuse_count,
    soc_defragment_block_t *inuse_blk,
    uint32 *moved_blk_count,
    soc_defragment_block_t **blk_cur_loc,
    soc_defragment_blk_mov_seq_t **blk_mov_seq,
    uint32 *free_count,
    soc_defragment_block_t **free_blk,
    soc_defragment_block_t *largest_free_blk);

extern int
soc_defragment_block_cmp(
    void *a,
    void *b);

extern int soc_defragment(int unit, int block_count,
        soc_defragment_block_t *block_array,
        soc_defragment_block_t *reserved_block,
        int member_table_size,
        soc_defragment_member_op_t *member_op,
        soc_defragment_group_op_t *group_op,
        int free_block_base_ptr,
        int *moved_cnt);
extern int soc_defragment_block_move(int unit, int src_base_ptr,
    int dst_base_ptr, int block_size, soc_defragment_member_op_t *member_op,
    int group, soc_defragment_group_op_t *group_op);
#endif /* !_SOC_DEFRAGMENT_H */
