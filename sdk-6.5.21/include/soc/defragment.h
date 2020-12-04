/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Routines for defragmenting member tables.
 */

#ifndef _SOC_DEFRAGMENT_H
#define _SOC_DEFRAGMENT_H

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

extern int soc_defragment(int unit, int block_count,
        soc_defragment_block_t *block_array,
        soc_defragment_block_t *reserved_block,
        int member_table_size,
        soc_defragment_member_op_t *member_op,
        soc_defragment_group_op_t *group_op,
        int free_block_base_ptr);
extern int soc_defragment_block_move(int unit, int src_base_ptr,
    int dst_base_ptr, int block_size, soc_defragment_member_op_t *member_op,
    int group, soc_defragment_group_op_t *group_op);
#endif /* !_SOC_DEFRAGMENT_H */
