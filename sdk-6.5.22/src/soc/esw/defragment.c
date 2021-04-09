/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * Provides generic routines for defragmenting member tables.
 */

#include <soc/defragment.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#include <shared/shr_pb.h>

/*
 * Typedef:
 *     _soc_dfg_blk_mov_seq_t
 *
 * Purpose:
 *    Source and destination base_ptr location details to perform a block
 *    move operation.
 */
typedef struct _soc_dfg_blk_mov_seq_s {
    /* Move block from source base_ptr location. */
    int src_base_ptr;
    /* Move blok to destination base_ptr location. */
    int dst_base_ptr;
    /*
     * Pointer to next move location of this block, if multiple moves are
     * required for this block during defragmentation.
     */
    struct _soc_dfg_blk_mov_seq_s *next;
} _soc_dfg_blk_mov_seq_t;

/*
 * Typedef:
 *     _soc_dfg_seq_s
 *
 * Purpose:
 *    The defragmentation sequence for in use blocks in the table.
 */
typedef struct _soc_dfg_seq_s {
    /* Total number of moves required for this in-use block. */
    uint32 mov_count;
    /* block size. */
    int size;
    /* Group assocated with this block. */
    int group;
    /* Block final base pointer. */
    int final_base_ptr;
    /* Pointer to the block move sequence linked-list. */
    _soc_dfg_blk_mov_seq_t *mv_seq;
} _soc_dfg_seq_t;

/*
 * Function:
 *    _soc_defragment_array_print
 *
 * Purpose:
 *    Print the list of blocks in the defragmentation block array.
 *
 * Parameters:
 *    unit      - (IN)  Unit number.
 *    count     - (IN)  Number of blocks in the blk_arr.
 *    blk_arr   - (IN)  Pointer to the defragmentation block array.
 *
 * Returns:
 *    SOC_E_NONE - Success
 */
STATIC int
_soc_defragment_array_print(int unit,
                            uint32 count,
                            soc_defragment_block_t *blk_arr)
{
    uint32 b = 0;        /* Block index iterator. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */

    if (count && blk_arr) {
        pb = shr_pb_create();
        shr_pb_printf(pb,
                      "Defrag: Sorted in-use blocks array elements:\n"
                      "--------------------------------------------\n");
        for (b = 0; b < count; b++) {
            shr_pb_printf(pb,
                          "%4d. group=%-4d base_ptr=%-5d size=%-4d\n",
                          b,
                          blk_arr[b].group,
                          blk_arr[b].base_ptr,
                          blk_arr[b].size);
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s"),
                     shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *    _soc_defragment_dfg_seq_cleanup
 *
 * Purpose:
 *    Cleanup defragmentation move sequence memory allocations.
 *
 * Parameters:
 *    unit              - (IN) Unit number.
 *    inuse_count       - (IN) Total in use blocks count.
 *    sorted_inuse_blks - (IN) Pointer to sorted in-use blocks array.
 *    _dfg_seq          - (IN) Pointer to the table defragmentation sequence.
 *
 * Returns:
 *    None.
 */
STATIC void
_soc_defragment_dfg_seq_cleanup(int unit,
                                uint32 inuse_count,
                                soc_defragment_block_t *sorted_inuse_blks,
                                _soc_dfg_seq_t **_dfg_seq)
{
    uint32 b = 0; /* Block index iterator. */
    _soc_dfg_blk_mov_seq_t *bmov = NULL; /* Ptr to blk move linked list. */
    _soc_dfg_blk_mov_seq_t *del = NULL; /* Node to be deleted. */

    /* Free sorted in use blocks memory. */
    if (sorted_inuse_blks) {
        sal_free(sorted_inuse_blks);
        sorted_inuse_blks = NULL;
    }
    if (_dfg_seq) {
        for (b = 0; b < inuse_count; b++) {
            /*
             * Traverse the list and free block move sequence memory, if the
             * move count (mov_count) is not zero.
             */
            if (_dfg_seq[b] != NULL) {
                if (_dfg_seq[b]->mov_count > 0 && _dfg_seq[b]->mv_seq) {
                    bmov = _dfg_seq[b]->mv_seq;
                    while (bmov->next) {
                        del = bmov->next;
                        bmov->next = del->next;
                        sal_free(del);
                        del = NULL;
                        _dfg_seq[b]->mov_count--;
                    }
                    /* Free the bmov node memory. */
                    sal_free(bmov);
                }
                sal_free(_dfg_seq[b]);
                _dfg_seq[b] = NULL;
            }
        }
        sal_free(_dfg_seq);
    }
    return;
}

/*
 * Function:
 *    _soc_defragment_dfg_seq_blk_mov_add
 *
 * Purpose:
 *    Add a block move sequence for an in-use block to its block move sequence
 *    linked list.
 *
 * Parameters:
 *    unit              - (IN) Unit number.
 *    _dfg_seq          - (IN) Pointer to blks defragmentation sequence array.
 *    inuse_count       - (IN) Total in use blocks count.
 *    sorted_inuse_blks - (IN) Pointer to sorted in-use blocks array.
 *    bindex            - (IN) Index of this block in _dfg_seq array.
 *    src_base_ptr      - (IN) Block's source base pointer index value.
 *    dst_base_ptr      - (IN) Block's destination base pointer index value.
 *
 * Returns:
 *    SOC_E_PARAM    - Invalid input parameter.
 *    SOC_E_INTERNAL - Encountered an exception condition.
 *    SOC_E_MEMORY   - On memory allocation failure.
 *    SOC_E_NONE     - Success
 */
STATIC int
_soc_defragment_dfg_seq_blk_mov_add(int unit,
                                    _soc_dfg_seq_t **_dfg_seq,
                                    uint32 inuse_count,
                                    soc_defragment_block_t *sorted_inuse_blks,
                                    uint32 bindex,
                                    int src_base_ptr,
                                    int dst_base_ptr)
{
    _soc_dfg_seq_t *bdseq = NULL; /* Pointer to blks defrag move seq. */
    _soc_dfg_blk_mov_seq_t *bmov = NULL; /* Pointer to blk move seq.*/
    _soc_dfg_blk_mov_seq_t *new = NULL; /* Pointer to a new blk move seq. */

    /* Validate input parameters. */
    if (_dfg_seq == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: Null _dfg_seq param - rv=%s\n"),
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (bindex >= inuse_count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: bindex=%u >= inuse_count=%u "
                              "- rv=%s\n"),
                   bindex,
                   inuse_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }

    bdseq = _dfg_seq[bindex];
    if (bdseq == NULL) {
        _dfg_seq[bindex] = sal_alloc(sizeof(*bdseq),
                                     "socdefragmentBlkDfgSeq");
        if (_dfg_seq[bindex] == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: socdefragmentBlkDfgSeq - rv=%s\n"),
                       soc_errmsg(SOC_E_MEMORY)));
            return (SOC_E_MEMORY);
        }
        sal_memset(_dfg_seq[bindex], 0, sizeof(*bdseq));
        _dfg_seq[bindex]->group = sorted_inuse_blks[bindex].group;
        _dfg_seq[bindex]->size = sorted_inuse_blks[bindex].size;
        _dfg_seq[bindex]->final_base_ptr = dst_base_ptr;

        /* Allocate head node memory. */
        new = sal_alloc(sizeof(*new), "socdefragmentBlkMvSeq");
        if (new == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: socdefragmentBlkMvSeq - rv=%s\n"),
                       soc_errmsg(SOC_E_MEMORY)));
            return (SOC_E_MEMORY);
        }
        sal_memset(new, 0, sizeof(*new));
        /* Update the block move details. */
        new->src_base_ptr = src_base_ptr;
        new->dst_base_ptr = dst_base_ptr;
        _dfg_seq[bindex]->mv_seq = new;
        _dfg_seq[bindex]->mov_count++;
        return (SOC_E_NONE);
    }

    /* Get the block move sequence head node pointer. */
    bmov = _dfg_seq[bindex]->mv_seq;
    while (bmov) {
        if (bmov->next == NULL) {
            /* Allocate head node memory. */
            new = sal_alloc(sizeof(*new), "socdefragmentBlkMvSeq");
            if (new == NULL) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error: socdefragmentBlkMvSeq - rv=%s\n"),
                           soc_errmsg(SOC_E_MEMORY)));
                return (SOC_E_MEMORY);
            }
            sal_memset(new, 0, sizeof(*new));
            /* Update the block move details. */
            new->src_base_ptr = src_base_ptr;
            new->dst_base_ptr = dst_base_ptr;
            /* Update the final base_ptr value for this block. */
            _dfg_seq[bindex]->final_base_ptr = dst_base_ptr;
            bmov->next = new;
            /* Increment the block move count for this block. */
            bdseq->mov_count++;
            break;
        } else {
            bmov = bmov->next;
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *    _soc_defragment_dfg_seq_blk_skip
 *
 * Purpose:
 *    Update the table defragmentation sequence for an in use block that has
 *    no moves associated with it and is skipped during defragmentation.
 *
 * Parameters:
 *    unit              - (IN) Unit number.
 *    _dfg_seq          - (IN) Pointer to the table defragmentation sequence.
 *    inuse_count       - (IN) Total in use blocks count.
 *    sorted_inuse_blks - (IN) Pointer to sorted in-use blocks array.
 *    bindex            - (IN) Index of this block in _dfg_seq array.
 *
 * Returns:
 *    SOC_E_PARAM  - Invalid input parameter.
 *    SOC_E_MEMORY - On memory allocation failure.
 *    SOC_E_NONE   - Success
 */
STATIC int
_soc_defragment_dfg_seq_blk_skip(int unit,
                                 _soc_dfg_seq_t **_dfg_seq,
                                 uint32 inuse_count,
                                 soc_defragment_block_t *sorted_inuse_blks,
                                 uint32 bindex)
{
    _soc_dfg_seq_t *bdseq = NULL; /* Pointer to blocks defrag move seq. */

    /* Validate input parameters. */
    if (_dfg_seq == NULL || sorted_inuse_blks == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: _dfg_seq or sorted_inuse_blks null ptr "
                              "- rv=%s\n"),
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (bindex >= inuse_count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: bindex=%u >= inuse_count=%u "
                              "- rv=%s\n"),
                   bindex,
                   inuse_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }

    bdseq = _dfg_seq[bindex];
    if (bdseq == NULL) {
        _dfg_seq[bindex] = sal_alloc(sizeof(*bdseq),
                                     "socdefragmentBlkSkipDfgSeq");
        if (_dfg_seq[bindex] == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: socdefragmentBlkSkipDfgSeq "
                                  "- rv=%s\n"),
                       soc_errmsg(SOC_E_MEMORY)));
            return (SOC_E_MEMORY);
        }
        sal_memset(_dfg_seq[bindex], 0, sizeof(*bdseq));
        _dfg_seq[bindex]->group = sorted_inuse_blks[bindex].group;
        _dfg_seq[bindex]->size = sorted_inuse_blks[bindex].size;
        /*
         * As this block is not being moved, it's final base_ptr is same
         * as it's current base_ptr value.
         */
        _dfg_seq[bindex]->final_base_ptr = sorted_inuse_blks[bindex].base_ptr;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *    _soc_defragment_mov_seq_out_cleanup
 *
 * Purpose:
 *    Cleanup defragmentation move sequence memory allocations.
 *
 * Parameters:
 *    unit        - (IN) Unit number.
 *    blks_moved  - (IN) Number of in use blocks that have move operation
 *                       associated with them to defragment the table.
 *    bcur_loc    - (IN) Pointer to the current location of the in use
 *                       blocks that need to be moved to defragment the
 *                       table.
 *    bmov_seq    - (IN) Pointer to the the defragmentation block move
 *                       sequence array.
 *
 * Returns:
 *    None.
 */
STATIC void
_soc_defragment_mov_seq_out_cleanup(int unit,
                                    uint32 blks_moved,
                                    soc_defragment_block_t *bcur_loc,
                                    soc_defragment_blk_mov_seq_t *bmov_seq)
{
    uint32 b; /* Block index iterator. */

    if (bcur_loc) {
        sal_free(bcur_loc);
        bcur_loc = NULL;
    }
    if (blks_moved > 0 && bmov_seq) {
        for (b = 0; b < blks_moved; b++) {
            if (bmov_seq[b].mov_count && bmov_seq[b].base_ptr) {
                sal_free(bmov_seq[b].base_ptr);
                bmov_seq[b].base_ptr = NULL;
            }
        }
        sal_free(bmov_seq);
        bmov_seq = NULL;
    }
    return;
}

/*
 * Function:
 *    _soc_defragment_sequence_print
 *
 * Purpose:
 *    Print the defragmentation sequence for all the in use blocks.
 *
 * Parameters:
 *    unit        - (IN)  Unit number.
 *    inuse_count - (IN)  Total in use blocks count.
 *    _dfg_seq    - (IN)  Pointer to the table defragmentation sequence.
 *    blks_moved  - (IN)  Number of in use blocks that have move operation
 *                        associated with them to defragment the table.
 *
 * Returns:
 *    SOC_E_PARAM - Invalid input parameter.
 *    SOC_E_NONE  - Success
 */
STATIC int
_soc_defragment_sequence_print(int unit,
                               uint32 inuse_count,
                               _soc_dfg_seq_t **_dfg_seq,
                               uint32 blks_moved)
{
    _soc_dfg_seq_t *bdseq = NULL; /* Block defagmentation sequence. */
    _soc_dfg_blk_mov_seq_t *mseq = NULL; /* Move sequence. */
    uint32 b = 0;     /* Block index iterator. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */

    /* Validate input parameters. */
    if (inuse_count == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: inuse_count=%u - rv=%s\n"),
                   inuse_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (_dfg_seq == NULL || blks_moved == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: _dfg_seq == NULL || blks_moved=%u "
                              "- rv=%s\n"),
                   blks_moved,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    pb = shr_pb_create();
    shr_pb_printf(pb,
                  "_dfg_seq: inuse_count=%-4u blks_moved=%-4u\n"
                  "------------------------------------------\n",
                  inuse_count,
                  blks_moved);
    for (b = 0; b < inuse_count; b++) {
        bdseq = _dfg_seq[b];
        if (bdseq) {
            shr_pb_printf(pb,
                          "%4u. group=%-4d final.base_ptr=%-5d size=%-4d "
                          "mov_count=%-2u\n",
                          b,
                          bdseq->group,
                          bdseq->final_base_ptr,
                          bdseq->size,
                          bdseq->mov_count);
            if (bdseq->mov_count > 0) {
                shr_pb_printf(pb, "        mv_seq: [");
                mseq = bdseq->mv_seq;
                while (mseq) {
                    shr_pb_printf(pb,
                                  " src:%-5d->dst=%-5d ",
                                  mseq->src_base_ptr,
                                  mseq->dst_base_ptr);
                    mseq = mseq->next;
                }
                shr_pb_printf(pb, "]\n");
            }
            shr_pb_printf(pb, "\n");
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s"),
                 shr_pb_str(pb)));
    shr_pb_destroy(pb);
    return SOC_E_NONE;
}

/*
 * Function:
 *    _soc_defragment_out_sequence_print
 *
 * Purpose:
 *    Print the final defragmentation sequence along with the list of free
 *    blocks that would be available in the table after defragmenting it.
 *
 * Parameters:
 *    unit            - (IN) Unit number.
 *    moved_blk_count - (IN) Number of in use blocks moved to defragment the
 *                           table, also the size of the bcur_loc and
 *                           bmseq arrays.
 *    bcur_loc        - (IN) Pointer to the current location of the in use
 *                           blocks that need to be moved to defragment the
 *                           table.
 *    bmseq           - (IN) Pointer to the the defragmentation block move
 *                           sequence array.
 *    free_count      - (IN) Total number of free blocks, also the size of
 *                           free_blk array.
 *    free_blk        - (IN) Pointer to the list of free blocks in the
 *                           defragmented table.
 *    lfree_blk       - (IN) Largest free block available after defragmenting
 *                           the table.
 *
 * Returns:
 *    SOC_E_PARAM - Invalid input parameter.
 *    SOC_E_NONE  - Success
 */
STATIC int
_soc_defragment_out_sequence_print(int unit,
                                   uint32 moved_blk_count,
                                   soc_defragment_block_t *bcur_loc,
                                   soc_defragment_blk_mov_seq_t *bmseq,
                                   uint32 free_count,
                                   soc_defragment_block_t *free_blk,
                                   soc_defragment_block_t lfree_blk)
{
    uint32 b = 0;     /* Block index iterator. */
    uint32 m = 0;     /* Move index iterator. */
    shr_pb_t *pb = NULL; /* Debug print buffer. */

    if (moved_blk_count == 0 || bcur_loc == NULL || bmseq == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: moved_blk_count=%u, bcur_loc or "
                              "bmseq is null - rv=%s\n"),
                   moved_blk_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }

    pb = shr_pb_create();

    shr_pb_printf(pb,
                  "Defrag: bcur_loc[] + blk_mov_seq[] moved_blk_count=%-4u\n"
                  "-------------------------------------------------------\n",
                  moved_blk_count);
    for (b = 0; b < moved_blk_count; b++) {
        shr_pb_printf(pb,
                      "%4u. cur: group=%-4d base_ptr=%-5d size=%-4d mov: "
                      "count=%-1u size=%-4d\n",
                      b,
                      bcur_loc[b].group,
                      bcur_loc[b].base_ptr,
                      bcur_loc[b].size,
                      bmseq[b].mov_count,
                      bmseq[b].size);
        shr_pb_printf(pb,"        dst.base_ptr[");
        for (m = 0; m < bmseq[b].mov_count; m++) {
            shr_pb_printf(pb,
                          " %2u.%-5d ",
                          m,
                          bmseq[b].base_ptr[m]);
        }
        shr_pb_printf(pb, "]\n\n");
    }

    shr_pb_printf(pb,
                  "Free blks Total: count=%-4u, largest.free_blk: group=%-4d "
                  "base_ptr=%-5d size=%-4d\n",
                  free_count,
                  lfree_blk.group,
                  lfree_blk.base_ptr,
                  lfree_blk.size);
    for (b = 0; b < free_count; b++) {
        shr_pb_printf(pb,
                      "%4u. group=%-4d base_ptr=%-5d size=%-4d\n",
                      b,
                      free_blk[b].group,
                      free_blk[b].base_ptr,
                      free_blk[b].size);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s"),
                 shr_pb_str(pb)));
    shr_pb_destroy(pb);
    return (SOC_E_NONE);
}

/*
 * Function:
 *    _soc_defragment_output_get
 *
 * Purpose:
 *    Translate compiled defragmentation move sequence of in use blocks to
 *    the required output format. Also determine the list of free blocks
 *    expected to be available in the defragmented table and the largest
 *    free block among these free block. Allocate memory for the output
 *    parameters and populate the output values.
 *
 * Parameters:
 *    unit              - (IN)  Unit number.
 *    mem_info          - (IN)  Defragmentation memories/table information.
 *    blks_moved        - (IN)  Number of in use blocks moved to defragment the
 *                              table.
 *    _dfg_seq          - (IN)  Pointer to the table defragmentation sequence.
 *    inuse_count       - (IN)  Size of sorted_inuse_blks array.
 *    sorted_inuse_blks - (IN)  The list of in use blocks in dfg_mem.
 *    moved_blk_count   - (OUT) Number of in use blocks moved to defragment the
 *                              table, this is the size of the blk_cur_loc and
 *                              blk_mov_seq arrays.
 *    blk_cur_loc       - (OUT) Pointer to array of in use blocks moved to
 *                              defragment the table, the blocks in this
 *                              array are sorted by their base_ptr value in
 *                              ascending order.
 *    blk_mov_seq       - (OUT) Pointer to the block move sequence for each
 *                              in use blocks in blk_cur_loc array out
 *                              parameter.
 *    free_count        - (OUT) Pointer to variable to set and return the size
 *                              of free_blk array.
 *    free_blk          - (OUT) Pointer to set and return the list of free
 *                              blocks that expected to be available upon
 *                              defragmenting the table. The blocks in this
 *                              array are sorted in ascending by their
 *                              base_ptr value.
 *    largest_free_blk  - (OUT) Pointer to set and return the largest free
 *                              block availalbe among the free blocks in
 *                              free_blk array. This out parameter is set only
 *                              when atleast one free block is available in the
 *                              defragmented table i.e. free_count >= 1.
 *
 * Returns:
 *    SOC_E_PARAM  - Invalid input parameter.
 *    SOC_E_MEMORY - On memory allocation failure.
 *    SOC_E_NONE   - Operation successful.
 */
STATIC int
_soc_defragment_output_get(int unit,
                           soc_defragment_mem_info_t mem_info,
                           uint32 blks_moved,
                           _soc_dfg_seq_t **_dfg_seq,
                           uint32 inuse_count,
                           soc_defragment_block_t *sorted_inuse_blks,
                           uint32 *moved_blk_count,
                           soc_defragment_block_t **blk_cur_loc,
                           soc_defragment_blk_mov_seq_t **blk_mov_seq,
                           uint32 *free_count,
                           soc_defragment_block_t **free_blk,
                           soc_defragment_block_t *largest_free_blk)
{
    soc_defragment_block_t *bcur_loc = NULL; /* Block current location array. */
    soc_defragment_blk_mov_seq_t *bmov_seq = NULL; /* Block move sequence. */
    _soc_dfg_seq_t *bdseq = NULL; /* Blocks defrag sequence array. */
    _soc_dfg_blk_mov_seq_t *bmv_info = NULL; /* Blk move seqence. */
    uint32 b;          /* In-use block index iterator. */
    uint32 bidx = 0;   /* Moved block index iterator. */
    uint32 mv_idx = 0; /* Move index. */
    soc_defragment_block_t fblk; /* Free block. */
    soc_defragment_block_t *fblk_arr = NULL; /* Free block array. */
    uint32 fblk_count = 0; /* Free blocks count. */

    /* Validate input parameters. */
    if (blks_moved == 0 || inuse_count == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: blks_moved=%u inuse_count=%u - rv=%s\n"),
                   blks_moved,
                   inuse_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (sorted_inuse_blks == NULL || _dfg_seq == NULL || moved_blk_count == NULL
        || blk_cur_loc == NULL || blk_mov_seq == NULL || free_count == NULL
        || free_blk == NULL || largest_free_blk == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: Null pointer - rv=%s\n"),
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }

    /* Allocate memory to store moved block's current location. */
    bcur_loc = sal_alloc(sizeof(*bcur_loc) * blks_moved,
                         "socdefragmentBlkCurLocArr");
    if (bcur_loc == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: socdefragmentBlkCurLocArr - rv=%s\n"),
                   soc_errmsg(SOC_E_MEMORY)));
        return (SOC_E_MEMORY);
    }
    sal_memset(bcur_loc, 0, sizeof(*bcur_loc) * blks_moved);

    /* Allocate memory to store the block's move sequence. */
    bmov_seq = sal_alloc(sizeof(*bmov_seq) * blks_moved,
                         "socdefragmentBlkMovSeqArr");
    if (bmov_seq == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: socdefragmentBlkMovSeqArr - rv=%s\n"),
                   soc_errmsg(SOC_E_MEMORY)));
        sal_free(bcur_loc);
        bcur_loc = NULL;
        return (SOC_E_MEMORY);
    }
    sal_memset(bmov_seq, 0, sizeof(*bmov_seq) * blks_moved);

    /* Initialize the free block base_ptr. */
    soc_defragment_block_t_init(&fblk);
    fblk.base_ptr = mem_info.first_entry_index;

    /*
     * Iterate over the in use blocks and capture their move sequence if
     * the block has been moved.
     */
    for (b = 0, bidx = 0; b < inuse_count; b++) {
        bdseq = _dfg_seq[b];
        if (bdseq == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error: _dfg_seq[%u] ptr is null "
                                      "- rv=%s\n"),
                           b,
                           soc_errmsg(SOC_E_INTERNAL)));
            /* Cleanup resources allcoated in this function. */
            _soc_defragment_mov_seq_out_cleanup(unit,
                                                blks_moved,
                                                bcur_loc,
                                                bmov_seq);
            /* This pointer must be valid for all in use blocks. */
            return SOC_E_INTERNAL;
        }

        if (bdseq && bdseq->mov_count > 0) {
            /* Get the current location of this in use block. */
            bcur_loc[bidx] = sorted_inuse_blks[b];
            bmov_seq[bidx].size = bdseq->size;
            bmov_seq[bidx].mov_count = bdseq->mov_count;

            bmov_seq[bidx].base_ptr = sal_alloc(
                                            (sizeof(int)
                                             * bmov_seq[bidx].mov_count),
                                            "socdefragmentBlkMovSeq");
            if (bmov_seq[bidx].base_ptr == NULL) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error: socdefragmentBlkMovSeq b=%u bidx="
                                      "%u - rv=%s\n"),
                           b,
                           bidx,
                           soc_errmsg(SOC_E_MEMORY)));
                /* Cleanup resources allcoated in this function. */
                _soc_defragment_mov_seq_out_cleanup(unit,
                                                    blks_moved,
                                                    bcur_loc,
                                                    bmov_seq);
                return (SOC_E_MEMORY);
            }
            sal_memset(bmov_seq[bidx].base_ptr,
                       0,
                       sizeof(int) * bmov_seq[bidx].mov_count);
            mv_idx = 0;
            bmv_info = bdseq->mv_seq;
            while (bmv_info) {
                bmov_seq[bidx].base_ptr[mv_idx++] = bmv_info->dst_base_ptr;
                /* Store the new base_ptr of this in use block. */
                bmv_info = bmv_info->next;
            }
            bidx++;
        }
        fblk.size = bdseq->final_base_ptr - fblk.base_ptr;
        if (fblk.size) {
            fblk_count++;
        }
        /* Move the free block base pointer beyound this in use block. */
        fblk.base_ptr = bdseq->final_base_ptr + bdseq->size;
    }

    fblk.size = (mem_info.last_entry_index + 1) - fblk.base_ptr;
    if (fblk.size) {
        fblk_count++;
    }

    if (fblk_count) {
        /* Allocate memory to store the free blocks. */
        fblk_arr = sal_alloc(sizeof(*fblk_arr) * fblk_count,
                             "socdefragmentFblkArr");
        if (fblk_arr == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: socdefragmentFblkArr - rv=%s\n"),
                       soc_errmsg(SOC_E_MEMORY)));
            /* Cleanup resources allcoated in this function. */
            _soc_defragment_mov_seq_out_cleanup(unit,
                                                blks_moved,
                                                bcur_loc,
                                                bmov_seq);
            return (SOC_E_MEMORY);
        }
        sal_memset(fblk_arr, 0, sizeof(*fblk_arr) * fblk_count);

        soc_defragment_block_t_init(&fblk);
        fblk.base_ptr = mem_info.first_entry_index;
        for (b = 0, bidx = 0; b < inuse_count; b++) {
            /* Determine if there is a gap and no. of entries in this gap. */
            fblk.size = _dfg_seq[b]->final_base_ptr - fblk.base_ptr;
            if (fblk.size && bidx < fblk_count) {
                if (fblk.size > largest_free_blk->size) {
                    *largest_free_blk = fblk;
                }
                soc_defragment_block_t_init(&fblk_arr[bidx]);
                fblk_arr[bidx].base_ptr = fblk.base_ptr;
                fblk_arr[bidx++].size = fblk.size;
            }
            fblk.base_ptr = _dfg_seq[b]->final_base_ptr + _dfg_seq[b]->size;
        }
        fblk.size = (mem_info.last_entry_index + 1) - fblk.base_ptr;
        if (fblk.size && bidx < fblk_count) {
            if (fblk.size > largest_free_blk->size) {
                *largest_free_blk = fblk;
            }
            soc_defragment_block_t_init(&fblk_arr[bidx]);
            fblk_arr[bidx].base_ptr = fblk.base_ptr;
            fblk_arr[bidx++].size = fblk.size;
        }
    }

    /* Set the output parameters. */
    *moved_blk_count = blks_moved;
    *blk_cur_loc = bcur_loc;
    *blk_mov_seq = bmov_seq;
    *free_count = fblk_count;
    *free_blk = fblk_arr;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_defragment_block_cmp
 * Purpose:
 *      Compare two blocks of member table entries based on their starting
 *      index (base_ptr).
 * Parameters:
 *      a - (IN) First block.
 *      b - (IN) Second block.
 * Returns:
 *      -1 if a < b, 0 if a = b, 1 if a > b.
 */
int
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

/*
 * Function:
 *    soc_defragment_block_t_init
 *
 * Purpose:
 *    Initialize soc_defragment_block_t structure member variables.
 *
 * Parameters:
 *    blk - (IN/OUT) Pointer to a soc_defragment_block_t structure.
 *
 * Returns:
 *    NONE.
 */
void
soc_defragment_block_t_init(soc_defragment_block_t *blk)
{
    if (blk) {
        /* Initialize structure member variables to zero. */
        sal_memset(blk, 0, sizeof(*blk));
        blk->group = SOC_DEFRAGMENT_BLK_GRP_INVALID;
    }
    return;
}

/*
 * Function:
 *    soc_defragment_mem_info_t_init
 *
 * Purpose:
 *    Initialize soc_defragment_mem_info_t structure member variables.
 *
 * Parameters:
 *    minfo - (IN/OUT) Pointer to a soc_defragment_mem_info_t structure.
 *
 * Returns:
 *    NONE.
 */
void
soc_defragment_mem_info_t_init(soc_defragment_mem_info_t *minfo)
{
    if (minfo) {
        /* Initialize structure members to default values. */
        minfo->base_mem = INVALIDm;
        minfo->base_ptr_field = INVALIDf;
        minfo->size_field = INVALIDf;
        minfo->dfg_mem = INVALIDm;
        minfo->first_entry_index = 0;
        minfo->last_entry_index = 0;
    }
    return;
}

/*
 * Function:
 *    soc_defragment_sequence_get
 *
 * Purpose:
 *    Executes the defragmentation algorithm on dfg_mem memory, by using the
 *    in use blocks array input provided by the callee function. To perform
 *    defragmentation of the table, free entries must be avaiable to move these
 *    in use blocks and compress them together. On successful execution of the
 *    defragmentation algorithm, this function returns the defragmentation block
 *    move sequence for all in use blocks that have to be moved for
 *    defragmenting the table. It also returns the list of free blocks and the
 *    largest free block among these free blocks that would be avaiable in this
 *    table after executing the defragmentation sequence of this table.
 *
 * Parameters:
 *    unit              - (IN)  Unit number.
 *    mem_info          - (IN)  Defragmentation memories/table information.
 *    inuse_count       - (IN)  Size of inuse_blk array.
 *    inuse_blk         - (IN)  The list of in use blocks in dfg_mem.
 *    moved_blk_count   - (OUT) Number of in use blocks moved to defragment the
 *                              table, this is the size of the blk_cur_loc and
 *                              blk_mov_seq arrays.
 *    blk_cur_loc       - (OUT) Pointer to array of in use blocks moved to
 *                              defragment the table, the blocks in this
 *                              array are sorted by their base_ptr value in
 *                              ascending order.
 *    blk_mov_seq       - (OUT) Pointer to the block move sequence for each
 *                              in use blocks in blk_cur_loc array out
 *                              parameter.
 *    free_count        - (OUT) Pointer to variable to set and return the size
 *                              of free_blk array.
 *    free_blk          - (OUT) Pointer to set and return the list of free
 *                              blocks that expected to be available upon
 *                              defragmenting the table. The blocks in this
 *                              array are sorted in ascending by their
 *                              base_ptr value.
 *    largest_free_blk  - (OUT) Pointer to set and return the largest free
 *                              block availalbe among the free blocks in
 *                              free_blk array. This out parameter is set only
 *                              when atleast one free block is available in the
 *                              defragmented table i.e. free_count >= 1.
 *
 * Returns:
 *    SOC_E_PARAM    - Invalid input parameter.
 *    SOC_E_MEMORY   - On memory allocation failure.
 *    SOC_E_RESOURCE - Not enough free entries in the table to perform table
 *                     defragmentation operation.
 *    SOC_E_INTERNAL - Encountered an exception condition.
 *    SOC_E_NONE     - Operation successful.
 */
int
soc_defragment_sequence_get(int unit,
                            soc_defragment_mem_info_t mem_info,
                            uint32 inuse_count,
                            soc_defragment_block_t *inuse_blk,
                            uint32 *moved_blk_count,
                            soc_defragment_block_t **blk_cur_loc,
                            soc_defragment_blk_mov_seq_t **blk_mov_seq,
                            uint32 *free_count,
                            soc_defragment_block_t **free_blk,
                            soc_defragment_block_t *largest_free_blk)
{
    _soc_dfg_seq_t **_dfg_seq = NULL; /* Defrag sequence for in-use blks. */
    soc_defragment_block_t *sorted_inuse_blks = NULL; /* Sorted inuse blks
                                                         array. */
    soc_defragment_block_t fblk; /* Free block. */
    soc_defragment_block_t gap;  /* Gap between two consecutive inuse blocks. */
    soc_defragment_block_t cur;  /* Current in use block. */
    soc_defragment_block_t rsvd; /* Reserved block. */
    uint32 b; /* In-use block index iterator. */
    uint32 blks_moved = 0; /* In-use blocks moved count. */
    uint8 rsvd_in_gap = 0; /* Reserved block entries are now part of the gap
                              block. */
    shr_pb_t *rsvd_pb = NULL; /* Reserved block print buffer. */
    int rv = SOC_E_INTERNAL; /* Return value. */
    int max_inuse_bsize = 0; /* Largest in-use block size. */

    /* Validate input parameters before processing further. */
    if (mem_info.base_mem == INVALIDm
        || mem_info.dfg_mem == INVALIDm
        || (mem_info.last_entry_index > soc_mem_index_max(
                                            unit, mem_info.dfg_mem))
        || (mem_info.first_entry_index < soc_mem_index_min(
                                            unit, mem_info.dfg_mem))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: Invalid mem_info params - rv=%s\n"),
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (inuse_count == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: inuse_count=%u - rv=%s\n"),
                   inuse_count,
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }
    if (inuse_blk == NULL || moved_blk_count == NULL
        || blk_cur_loc == NULL || blk_mov_seq == NULL
        || free_count == NULL || free_blk == NULL
        || largest_free_blk == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: null pointer input - rv=%s\n"),
                   soc_errmsg(SOC_E_PARAM)));
        return (SOC_E_PARAM);
    }

    sorted_inuse_blks = sal_alloc(sizeof(*sorted_inuse_blks) * inuse_count,
                                  "socdefragmentSortedInuseBlkArr");
    if (sorted_inuse_blks == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: socdefragmentDfgSeqArr - rv=%s\n"),
                   soc_errmsg(SOC_E_MEMORY)));
        return (SOC_E_MEMORY);
    }
    sal_memcpy(sorted_inuse_blks,
               inuse_blk,
               sizeof(*sorted_inuse_blks) * inuse_count);

    /*
     * Sort the input in-use blocks array elements based on their base_ptr
     * index value before any further processing.
     */
    _shr_sort(sorted_inuse_blks,
              inuse_count,
              sizeof(*sorted_inuse_blks),
              soc_defragment_block_cmp);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Verb: (Done) Sorted inuse_blks count=%u "
                            "- rv=%d\n"),
                 inuse_count,
                 _soc_defragment_array_print(unit,
                                             inuse_count,
                                             sorted_inuse_blks)));
    soc_defragment_block_t_init(&fblk);
    soc_defragment_block_t_init(&rsvd);
    /*
     * Initialize the free block base_ptr to the start of the table
     * i.e. first_entry_index, provided as input to this function.
     */
    fblk.base_ptr = mem_info.first_entry_index;
    /*
     * Determine the largest free block available in this table before
     * defragmentation and use this free block as the reserved block.
     */
    for (b = 0; b < inuse_count; b++) {
        fblk.size = sorted_inuse_blks[b].base_ptr - fblk.base_ptr;
        if (fblk.size > rsvd.size) {
            /* Largest free block is used as the reserved block. */
            rsvd = fblk;
        }
        /* Move the free block base_ptr beyond the current in-use block. */
        fblk.base_ptr = sorted_inuse_blks[b].base_ptr
                            + sorted_inuse_blks[b].size;
        /* Check and store the size of the largest in use block. */
        if (sorted_inuse_blks[b].size > max_inuse_bsize) {
            max_inuse_bsize = sorted_inuse_blks[b].size;
        }
    }
    /*
     * Determine the free block size after the last in-use block until the last
     * entry of this table.
     */
    fblk.size = (mem_info.last_entry_index + 1) - fblk.base_ptr;
    if (fblk.size > rsvd.size) {
        /* Largest free block is used as the reserved block. */
        rsvd = fblk;
    }

    /*
     * If there are no free entries and hence no reserved block, then
     * defragmenting the table is not possible as entries/room is required
     * to move the in use blocks by following the make-before-break sequence.
     * Hence return E_RESOURCE error from this function.
     */
    if (rsvd.size == 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "rsvd.size=%d - rv=%s\n"),
                     rsvd.size,
                     soc_errmsg(SOC_E_RESOURCE)));
        sal_free(sorted_inuse_blks);
        sorted_inuse_blks = NULL;
        return (SOC_E_RESOURCE);
    }

    /* Initialize out parameters. */
    *moved_blk_count = 0;
    *blk_cur_loc = NULL;
    *blk_mov_seq = NULL;
    *free_count = 0;
    *free_blk = NULL;
    soc_defragment_block_t_init(largest_free_blk);

    /*
     * Allocate memory to store the defragmentation sequence of in use blocks
     * that have to be moved for defragmenting the table.
     */
    _dfg_seq = sal_alloc(sizeof(*_dfg_seq) * inuse_count,
                         "socdefragmentDfgSeqArr");
    if (_dfg_seq == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error: socdefragmentDfgSeqArr - rv=%s\n"),
                   soc_errmsg(SOC_E_MEMORY)));
        sal_free(sorted_inuse_blks);
        sorted_inuse_blks = NULL;
        return (SOC_E_MEMORY);
    }
    sal_memset(_dfg_seq, 0, sizeof(*_dfg_seq) * inuse_count);

    soc_defragment_block_t_init(&gap);
    /*  Initialize the gap base_ptr value to start defragmenting the table. */
    gap.base_ptr = mem_info.first_entry_index;

    /*
     * Create print buffer to track and display reserved block movement if
     * logging is enabled.
     */
    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
        rsvd_pb = shr_pb_create();
        shr_pb_printf(rsvd_pb,
                      "rsvd_blk movement trace:\n"
                      "-----------------------\n"
                      "@start : base_ptr=%-5d size=%-4d group=%-4d\n",
                      rsvd.base_ptr,
                      rsvd.size,
                      rsvd.group);
    }

    /* Compress the gaps between in-use blocks. */
    for (b = 0; b < inuse_count; b++) {
        soc_defragment_block_t_init(&cur);
        cur = sorted_inuse_blks[b];

        /*
         * Compute the gap size between the current in-use block (base_ptr) and
         * the current gap.base_ptr index.
         */
        gap.size = cur.base_ptr - gap.base_ptr;

        if (cur.base_ptr > rsvd.base_ptr
            && gap.base_ptr <= rsvd.base_ptr) {
            if (gap.base_ptr <= rsvd.base_ptr
                && gap.size >= cur.size
                && gap.size >= rsvd.size) {
                /* The current gap block includes the rsvd block entries. */
                rsvd_in_gap = 1;
            } else if (gap.size < max_inuse_bsize) {
                /*
                 * Skip the reserved block and move the gap block base_ptr to
                 * the entry index after this reserved block.
                 */
                gap.base_ptr = rsvd.base_ptr + rsvd.size;
                /* Recompute the gap size based on this new location. */
                gap.size = cur.base_ptr - gap.base_ptr;
            }
        }

        if (gap.size == 0) {
            /* No gap so move on to the next block. */
            gap.base_ptr = cur.base_ptr + cur.size;
            rv = _soc_defragment_dfg_seq_blk_skip(unit,
                                                  _dfg_seq,
                                                  inuse_count,
                                                  sorted_inuse_blks,
                                                  b);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error: _soc_defragment_dfg_seq_blk_skip"
                                      "(b=%u) - rv=%s\n"),
                           b,
                           soc_errmsg(rv)));
                if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                    shr_pb_destroy(rsvd_pb);
                }
                (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                      inuse_count,
                                                      sorted_inuse_blks,
                                                      _dfg_seq);
                return rv;
            }
        } else if (gap.size > 0) {
            if (cur.size <= gap.size) {
                rv = _soc_defragment_dfg_seq_blk_mov_add(unit,
                                                         _dfg_seq,
                                                         inuse_count,
                                                         sorted_inuse_blks,
                                                         b,
                                                         cur.base_ptr,
                                                         gap.base_ptr);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Error: "
                                          "_soc_defragment_dfg_seq_blk_mov_add"
                                          "(b=%u) - rv=%s\n"),
                               b,
                               soc_errmsg(rv)));
                    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                        shr_pb_destroy(rsvd_pb);
                    }
                    (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                          inuse_count,
                                                          sorted_inuse_blks,
                                                          _dfg_seq);
                    return rv;
                }
                if (rsvd_in_gap) {
                    rsvd_in_gap = 0;
                    rsvd.base_ptr = gap.base_ptr + cur.size;
                    rsvd.size = (cur.base_ptr + cur.size) - rsvd.base_ptr;

                    /* Capture reserved block movement. */
                    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                        shr_pb_printf(rsvd_pb,
                                      "%4u. moved_to: (base_ptr=%-5d "
                                      "size=%-4d group=%-4d) by inuse_blk@ "
                                      "(base_ptr=%-5d size=%-4d "
                                      "group=%-4d)\n",
                                      b,
                                      rsvd.base_ptr,
                                      rsvd.size,
                                      rsvd.group,
                                      gap.base_ptr,
                                      cur.size,
                                      cur.group);
                    }
                }
                gap.base_ptr += cur.size;
                blks_moved++;
            } else if (cur.size <= rsvd.size) {
                rv = _soc_defragment_dfg_seq_blk_mov_add(unit,
                                                         _dfg_seq,
                                                         inuse_count,
                                                         sorted_inuse_blks,
                                                         b,
                                                         cur.base_ptr,
                                                         rsvd.base_ptr);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Error: "
                                          "_soc_defragment_dfg_seq_blk_mov_add"
                                          "(b=%u) cur.base_ptr=%d rsvd.base_ptr"
                                          "=%d cur.size=%d - rv=%s\n"),
                               b,
                               cur.base_ptr,
                               rsvd.base_ptr,
                               cur.size,
                               soc_errmsg(rv)));
                    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                        shr_pb_destroy(rsvd_pb);
                    }
                    (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                          inuse_count,
                                                          sorted_inuse_blks,
                                                          _dfg_seq);
                    return rv;
                }
                rv = _soc_defragment_dfg_seq_blk_mov_add(unit,
                                                         _dfg_seq,
                                                         inuse_count,
                                                         sorted_inuse_blks,
                                                         b,
                                                         rsvd.base_ptr,
                                                         gap.base_ptr);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Error: "
                                          "_soc_defragment_dfg_seq_blk_mov_add"
                                          "(b=%u) rsvd.base_ptr=%d gap.base_ptr"
                                          "=%d cur.size=%d - rv=%s\n"),
                               b,
                               rsvd.base_ptr,
                               gap.base_ptr,
                               cur.size,
                               soc_errmsg(rv)));
                    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                        shr_pb_destroy(rsvd_pb);
                    }
                    (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                          inuse_count,
                                                          sorted_inuse_blks,
                                                          _dfg_seq);
                    return rv;
                }
                gap.base_ptr += cur.size;
                blks_moved++;
            } else {
                /*
                 * Gap not big enough to hold the current in use block. Skip
                 * this block and move the gap.base_ptr to the entry after
                 * this in-use block.
                 */
                gap.base_ptr = cur.base_ptr + cur.size;
                rv = _soc_defragment_dfg_seq_blk_skip(unit,
                                                      _dfg_seq,
                                                      inuse_count,
                                                      sorted_inuse_blks,
                                                      b);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Error: "
                                          "_soc_defragment_dfg_seq_blk_skip"
                                          "(b=%u) - rv=%s\n"),
                               b,
                               soc_errmsg(rv)));
                    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                        shr_pb_destroy(rsvd_pb);
                    }
                    (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                          inuse_count,
                                                          sorted_inuse_blks,
                                                          _dfg_seq);
                    return rv;
                }
            }
        } else {
            /* Gap size should never be negaitve. */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: -ve gap.size=%d for "
                                  "sorted_inuse_blks[%u]: group=%d base_ptr=%d "
                                  "size=%d - rv=%s\n"),
                       gap.size,
                       b,
                       cur.group,
                       cur.base_ptr,
                       cur.size,
                       soc_errmsg(SOC_E_INTERNAL)));
            if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
                shr_pb_destroy(rsvd_pb);
            }
            (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                  inuse_count,
                                                  sorted_inuse_blks,
                                                  _dfg_seq);
            return (SOC_E_INTERNAL);
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_COMMON | BSL_VERBOSE)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s"),
                     shr_pb_str(rsvd_pb)));
        shr_pb_destroy(rsvd_pb);
    }

    if (blks_moved > 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                  "Verb: (Done) Defrag sequence print "
                                  "- rv=%d\n"),
                     _soc_defragment_sequence_print(unit,
                                                    inuse_count,
                                                    _dfg_seq,
                                                    blks_moved)));
        rv = _soc_defragment_output_get(unit,
                                        mem_info,
                                        blks_moved,
                                        _dfg_seq,
                                        inuse_count,
                                        sorted_inuse_blks,
                                        moved_blk_count,
                                        blk_cur_loc,
                                        blk_mov_seq,
                                        free_count,
                                        free_blk,
                                        largest_free_blk);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error: _soc_defragment_output_get() - "
                                  "rv=%s\n"),
                       soc_errmsg(rv)));
            (void)_soc_defragment_dfg_seq_cleanup(unit,
                                                  inuse_count,
                                                  sorted_inuse_blks,
                                                  _dfg_seq);
            return rv;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                  "Verb: (Done) blk_cur_loc[] + blk_mov_seq[] "
                                  " output array print - rv=%d\n"),
                     _soc_defragment_out_sequence_print(unit,
                                                        *moved_blk_count,
                                                        *blk_cur_loc,
                                                        *blk_mov_seq,
                                                        *free_count,
                                                        *free_blk,
                                                        *largest_free_blk)));
    } else {
        /*
         * No blocks were moved and the table could not be defragmented
         * as the free block/s are not big enough to fit and move the in use
         * blocks of this table. The callee application could free up additional
         * entries in this table e.g. by destroying some in use groups (freeing
         * corresponding member table entries) and retry defragmenting the
         * table.
         */
        rv = SOC_E_RESOURCE;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                  "Verb: blks_moved=%u - rv=%s\n"),
                     blks_moved,
                     soc_errmsg(rv)));
    }
    /* Cleanup resources allocated for storing the defragmentation sequence. */
    (void)_soc_defragment_dfg_seq_cleanup(unit,
                                          inuse_count,
                                          sorted_inuse_blks,
                                          _dfg_seq);
    return (rv);
}
