/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Provides generic routines for defragmenting member tables.
 */

#include <soc/defragment.h>
#include <soc/drv.h>

/*
 * Function:
 *      soc_defragment_block_cmp
 * Purpose:
 *      Compare two blocks of member table entries based on their starting
 *      index.
 * Parameters:
 *      a - (IN) First block. 
 *      b - (IN) Second block. 
 * Returns:
 *      -1 if a < b, 0 if a = b, 1 if a > b.
 */
STATIC int
soc_defragment_block_cmp(void *a, void *b)
{
    soc_defragment_block_t *block_a, *block_b;

    block_a = (soc_defragment_block_t *)a;
    block_b = (soc_defragment_block_t *)b;

    if (block_a->base_ptr < block_b->base_ptr) {
        return -1;
    }

    if (block_a->base_ptr == block_b->base_ptr) {
        return 0;
    }

    return 1;
}

/*
 * Function:
 *      soc_defragment_block_move
 * Purpose:
 *      Move a block of member table entries.
 * Parameters:
 *      unit         - (IN) Unit
 *      src_base_ptr - (IN) Starting index of the source block.
 *      dst_base_ptr - (IN) Starting index of the destination block.
 *      block_size   - (IN) Number of entries to move.
 *      member_op - (IN) Operations on member table entry
 *      group     - (IN) The group this block belongs to
 *      group_op  - (IN) Operations on group table entry
 * Returns:
 *      SOC_E_xxx
 */
int
soc_defragment_block_move(int unit, int src_base_ptr, int dst_base_ptr,
        int block_size, soc_defragment_member_op_t *member_op,
        int group, soc_defragment_group_op_t *group_op)
{
    int i;
    int rv;

    /* Copy source block to destination block */
    for (i = 0; i < block_size; i++) {
        rv = (*member_op->member_copy)(unit, src_base_ptr + i, dst_base_ptr + i);
        SOC_IF_ERROR_RETURN(rv);
    }

    /* Update group's base pointer to point to destination block */
    rv = (*group_op->group_base_ptr_update)(unit, group, dst_base_ptr);
    SOC_IF_ERROR_RETURN(rv);

    /* Clear source block */
    for (i = 0; i < block_size; i++) {
        rv = (*member_op->member_clear)(unit, src_base_ptr + i);
        SOC_IF_ERROR_RETURN(rv);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_defragment
 * Purpose:
 *      Defragment a member table.
 * Parameters:
 *      unit           - (IN) Unit
 *      block_count    - (IN) Number of blocks in block_array
 *      block_array    - (IN) Used blocks of member table entries
 *      reserved_block - (IN) Reserved block of member table entries used as
 *                            defragmentation buffer
 *      member_table_size - (IN) Number of entries in member table
 *      member_op - (IN) Operations on member table entry
 *      group_op  - (IN) Operations on group table entry
 *      free_block_base_ptr - (IN) pointer to the beginning of the member table
 * Returns:
 *      SOC_E_xxx
 */
int
soc_defragment(int unit, int block_count,
        soc_defragment_block_t *block_array,
        soc_defragment_block_t *reserved_block,
        int member_table_size,
        soc_defragment_member_op_t *member_op,
        soc_defragment_group_op_t *group_op,
        int free_block_base_ptr)
{
    soc_defragment_block_t *sorted_block_array = NULL;
    int reserved_block_base_ptr, reserved_block_size;
    int free_block_size;
    int gap_base_ptr, gap_size;
    int block_base_ptr, block_size;
    int max_block_size;
    int group;
    int i;
    int rv;

    if (0 == block_count) {
        /* If no member entries are used, there is no need to defragment. */
        return SOC_E_NONE;
    }

    if (NULL == block_array) {
        return SOC_E_PARAM;
    }

    if (NULL == reserved_block) {
        return SOC_E_PARAM;
    }

    if (NULL == group_op) {
        return SOC_E_PARAM;
    }

    if (NULL == member_op) {
        return SOC_E_PARAM;
    }

    /* Sort the block array by block's base pointer */
    sorted_block_array = sal_alloc(block_count * sizeof(soc_defragment_block_t),
            "sorted block array");
    if (NULL == sorted_block_array) {
        return SOC_E_MEMORY;
    }
    sal_memcpy(sorted_block_array, block_array,
            block_count * sizeof(soc_defragment_block_t));
    _shr_sort(sorted_block_array, block_count, sizeof(soc_defragment_block_t),
            soc_defragment_block_cmp);

    /* Gap compression should start from this base */
    gap_base_ptr = free_block_base_ptr;

    /* Get defragmentation buffer */
    if (0 == reserved_block->size) {
        /* If a reserved block is not given, find the largest free block of
         * entries in the member table and use it as the defragmentation
         * buffer.
         */
        reserved_block_size = 0;
        reserved_block_base_ptr = 0;
        for (i = 0; i < block_count; i++) {
            free_block_size = sorted_block_array[i].base_ptr -
                              free_block_base_ptr;
            if (free_block_size > reserved_block_size) {
                reserved_block_size = free_block_size;
                reserved_block_base_ptr = free_block_base_ptr;
            } 
            free_block_base_ptr = sorted_block_array[i].base_ptr +
                                  sorted_block_array[i].size;
        }

        /* Also need to compute the free block size between the end of the
         * final used block and the end of the member table.
         */
        free_block_size = member_table_size - free_block_base_ptr;
        if (free_block_size > reserved_block_size) {
            reserved_block_size = free_block_size;
            reserved_block_base_ptr = free_block_base_ptr;
        } 
    } else {
        reserved_block_size = reserved_block->size;
        reserved_block_base_ptr = reserved_block->base_ptr;
    }

    /* Find maximum block size */
    max_block_size = 0;
    for (i = 0; i < block_count; i++) {
        if (sorted_block_array[i].size > max_block_size) {
            max_block_size = sorted_block_array[i].size;
        }
    }

    /* Compress the gaps between used blocks */
    for (i = 0; i < block_count; i++) {
        block_base_ptr = sorted_block_array[i].base_ptr;
        block_size = sorted_block_array[i].size;
        group = sorted_block_array[i].group;

        gap_size = block_base_ptr - gap_base_ptr;
        if (block_base_ptr > reserved_block_base_ptr) {
            if (gap_base_ptr <= reserved_block_base_ptr) {
                if (0 == reserved_block->size) {
                    if (gap_size < max_block_size) {
                        /* Skip over the reserved block */
                        gap_base_ptr = reserved_block_base_ptr +
                                       reserved_block_size;
                        gap_size = block_base_ptr - gap_base_ptr;
                    }
                } else { 
                    gap_size = reserved_block_base_ptr - gap_base_ptr;
                    if (gap_size < block_size) {
                        /* Skip over the reserved block */
                        gap_base_ptr = reserved_block_base_ptr +
                                       reserved_block_size;
                        gap_size = block_base_ptr - gap_base_ptr;
                    }
                }
            }
        }

        if (gap_size == 0) {
            gap_base_ptr = block_base_ptr + block_size;
        } else if (gap_size > 0) {
            if (block_size <= gap_size) {
                /* Block fits into the gap. Move it directly into the gap. */
                rv = soc_defragment_block_move(unit, block_base_ptr,
                        gap_base_ptr, block_size, member_op, group, group_op);
                if (SOC_FAILURE(rv)) {
                    sal_free(sorted_block_array);
                    return rv;
                }

                /* Move gap_base_ptr */
                gap_base_ptr += block_size;

            } else if (block_size <= reserved_block_size) {
                /* Block is bigger than the gap but fits into the
                 * defragmentation buffer. Move it first into the
                 * defragmentation buffer, then into the gap.
                 */
                rv = soc_defragment_block_move(unit, block_base_ptr,
                        reserved_block_base_ptr, block_size, member_op,
                        group, group_op);
                if (SOC_FAILURE(rv)) {
                    sal_free(sorted_block_array);
                    return rv;
                }
                rv = soc_defragment_block_move(unit, reserved_block_base_ptr,
                        gap_base_ptr, block_size, member_op, group, group_op);
                if (SOC_FAILURE(rv)) {
                    sal_free(sorted_block_array);
                    return rv;
                }

                /* Move gap_base_ptr */
                gap_base_ptr += block_size;

            } else {
                /* Block is bigger than the gap and the defragmentation
                 * buffer. It cannot be moved. Skip over it.
                 */
                gap_base_ptr = block_base_ptr + block_size;
            }
        } else {
            /* Gap size shoud never be negative. */
            sal_free(sorted_block_array);
            return SOC_E_INTERNAL;
        }
    }

    sal_free(sorted_block_array);
    return SOC_E_NONE;
}

