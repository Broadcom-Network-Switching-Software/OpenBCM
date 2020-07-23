/*! \file rm_tcam_prio_only_core.c
 *
 * Low level functions to read, write, move, clear, compare
 * prio only TCAM LT entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_crc.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include <bcmptm/bcmptm_rm_tcam_fp.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private variables
 */
static uint32_t g_buf_one[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];

static uint32_t g_buf_two[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];

static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                           [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS]
                           [BCMPTM_MAX_PT_FIELD_WORDS];

static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

#define IS_TARGET_ENTRY_ELIGIBLE(entry_info,idx,entry_type)               \
           ((entry_info[idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) && \
           ((entry_info[idx].entry_type == entry_type) ||                 \
           (entry_info[idx].entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID)))


/*******************************************************************************
 * Private Functions
 */
STATIC int
bcmptm_shared_update_get_key_type_prio( int unit,
                                        bcmltd_sid_t target_ltid,
                                        bcmptm_rm_tcam_lt_info_t *target_ltid_info,
                                        uint32_t target_index,
                                        int pipe_id,
                                        uint8_t half_mode,
                                        uint16_t *key_type_val,
                                        int *prio)
{
    bcmptm_rm_tcam_prio_only_shr_info_t *shared_info = NULL;
    uint16_t shared_info_count = 0;
    uint16_t idx;
    uint16_t count = 0;
    uint16_t shr_key_type;
    uint32_t shared_index;
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bcmltd_sid_t shr_ltid;
    bcmptm_rm_tcam_prio_only_info_t *shr_tcam_info;
    bcmptm_rm_tcam_prio_only_entry_info_t *shr_entry_info;
    uint8_t shared_half_mode = 0;
    uint8_t next_considered = 0;
    uint8_t mode_different = 0;
    uint32_t offset =0;
    uint32_t num_entries;

    /* Log the  function entry */
    SHR_FUNC_ENTER(unit);

    /*
     * If an LT entry is deleted at particular index, corresponding index
     * in LTs shared LTs should be freed up.But in some cases this should
     * not be done.
     *
     * For example 3 different LTs are sharing two physocal TCAMs as below.
     * LT1 is mapped to TCAM1 and TCAM2 pair in width expanded mode.
     * LT2 is mapped to TCAM1 is single wide mode.
     * LT3 is mapped to TCAM2 is single wide mode.
     *  --------------------------     -------------
     * |            LT1           |   |     LT2     |
     * |   -------      -------   |   |   -------   |
     * |  |       |    |       |  |   |  |       |  |
     * |  |       |    |       |  |   |  |       |  |
     * |  |       |    |       |  |   |  |       |  |
     * |  | TCAM1 |    | TCAM2 |  |   |  | TCAM1 |  |
     * |  |       |    |       |  |   |  |       |  |
     * |  |       |    |       |  |   |  |       |  |
     * |  |       |    |       |  |   |  |       |  |
     * |   -------      -------   |   |   -------   |
     *  --------------------------     -------------
     *                                 -------------
     *                                |     LT3     |
     *                                |   -------   |
     *                                |  |       |  |
     *                                |  |       |  |
     *                                |  |       |  |
     *                                |  | TCAM2 |  |
     *                                |  |       |  |
     *                                |  |       |  |
     *                                |  |       |  |
     *                                |   -------   |
     *                                 -------------
     *
     *
     * If an entry is created in LT1 at index I1 coreesponding
     * index in LT2 and LT3 should be blocked. Similarly if
     * an entry created in LT2 or LT3 at index I1, the corresponding
     * index in LT1 should be blocked.
     * Note: To block an index in LT1, corresponding index should be
     * occupied by LT entry in either LT2 or LT3 but not in both.
     *
     * If an entry is deleted in LT1 at index I1 coreesponding
     * index in LT2 and LT3 should be released. Similarly if
     * an entry deleted in LT2 and LT3 at index I1, the corresponding
     * index in LT1 should be released.
     * Note: To release an index in LT1, the LT entries at corresponding
     * index in both LT2 and LT3 should be deleted.
     *
     * When an entry is inserted or deleted at a particular index in
     * one LT, This function is used to identify key_type_val and prio
     * values to be updated in the LTs shared LT tcam entry info at
     * the corresponding index in shared LT.
     */


    /*
     * Input is target index which is filtered based on overlap.
     * In the case of clear we need to make sure that we are clearing
     * only when everything is free in an index In case corresponding index
     * is used by another LT we need to keep this index reserved for that LT.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_shared_info_get(unit,
                                              target_ltid,
                                              target_ltid_info,
                                              pipe_id,
                                              &shared_info,
                                              &shared_info_count));
    for (idx = 0; idx < shared_info_count; idx++) {
        shared_half_mode = FALSE;
        next_considered = 0;
        mode_different = 0;
        if ((shared_info[idx].start_index > target_index) ||
            (shared_info[idx].end_index < target_index)) {
            /*
             * The index which is going to be reserved is
             * out of range of this particular shared info
 */
            continue;
        }
        shr_ltid = shared_info[idx].shr_ltid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shr_ltid_info),
                                          NULL,
                                          NULL));

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_check_butterfly_mode(unit,
                                                  shr_ltid,
                                                  &shr_ltid_info,
                                                  &shared_half_mode));
        offset = (target_index -shared_info[idx].start_index);
        if (1 == half_mode  && 0 == shared_half_mode) {
            offset = offset / 2;
            mode_different = 1;
        } else if (0 == half_mode  && 1 == shared_half_mode) {
            offset = offset * 2;
            next_considered = 1;
            mode_different = 1;
        }
        shared_index = (offset  + shared_info[idx].shr_start_index);

        shr_key_type = shr_ltid_info.hw_entry_info->key_type_val[0][0];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                    shr_ltid,
                                                    &shr_ltid_info,
                                                    pipe_id,
                                                    &shr_tcam_info));
        /* Get the target LTID TCAM info. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                     shr_ltid,
                                                     &shr_ltid_info,
                                                     pipe_id,
                                                     &shr_entry_info));

        if (shr_entry_info[shared_index].entry_type == shr_key_type) {
            /*
             * This means this LT owns this target index.
             * In case of clear the key_type_val should be moved to the
             * new LT. Eg Double and single sharing same space and
             * Single deleted when both indexes are used by single.
             * Double's entry info should be marked reserved with
             * second single Once we identify a target index we go through
             * all its shared space and make sure that no one is owning
             * any entry in it shared space by checking the key type.
             */
            count++;
            *key_type_val = shr_key_type;
            *prio = shr_entry_info[shared_index].entry_pri;
            if (*prio == BCMPTM_RM_TCAM_PRIO_INVALID) {
                *prio = 0;
            }
        }
        if (next_considered == 1) {
            shared_index++;
            if (shr_entry_info[shared_index].entry_type == shr_key_type) {
                /*
                 * This means this LT owns this target index.
                 * In case of clear the key_type_val should be moved to the
                 * new LT. Eg Double and single sharing same space and
                 * Single deleted when both indexes are used by single.
                 * Double's entry info should be marked reserved with
                 * second single Once we identify a target index we go through
                 * all its shared space and make sure that no one is owning
                 * any entry in it shared space by checking the key type.
                 */
                count++;
                *key_type_val = shr_key_type;
                *prio = shr_entry_info[shared_index].entry_pri;
                if (*prio == BCMPTM_RM_TCAM_PRIO_INVALID) {
                    *prio = 0;
                }
            }
        }
        /* If view type is different no need to block data entries */
        if (target_ltid_info->hw_entry_info->view_type) {
            continue;
        }

        if ((mode_different == 1)
            && (shared_info[idx].shr_flags & BCMPTM_RM_TCAM_BOTH_KEY_KEY0)) {
            offset = (target_index -shared_info[idx].start_index);
            shared_index = (offset + shared_info[idx].shr_start_index);
            if (half_mode == 1) {
               /*
                * Current LT is half mode and shared LT is not half mode.
                * If index is greater than the half we don't need to block
                * for data.Check if index is greater than the valid entry
                * indices of the current LT.
 */
                num_entries = (shared_info[idx].end_index -
                               shared_info[idx].start_index);
                if (offset > (num_entries / 2)) {
                    continue;
                }
                /*
                 * Need to block data also  This will block one additional
                 * entry at exact index location due to data
 */
            }
            if (shr_entry_info[shared_index].entry_type == shr_key_type) {
                count++;
                *key_type_val = shr_key_type;
                *prio = shr_entry_info[shared_index].entry_pri;
                if (*prio == BCMPTM_RM_TCAM_PRIO_INVALID) {
                    *prio = 0;
                }
            }
        }
    }
    if (count == 0) {
        /*
         * Free this entry id when no one else is owning
         * the target index
         */
        *key_type_val = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
        *prio = BCMPTM_RM_TCAM_PRIO_INVALID;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_only_entry_info_clear_sw(int unit,
                                             bcmptm_rm_tcam_entry_iomd_t *iomd,
                                             uint32_t from_idx,
                                             uint32_t num_key_rows)
{
    uint32_t idx;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];

    /* Clear the TCAM entry information at from_idx. */
    for (idx = from_idx; idx < (from_idx + num_key_rows); idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    if (tcam_info->shr_info_count != 0) {
        for (idx = from_idx; idx < (from_idx + num_key_rows); idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                             iomd,
                                                             TRUE,
                                                             idx));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_only_entry_clear_hw(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        uint32_t index)
{
    int part;
    uint32_t rm_flags;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;

    /* Update the HW with NULL data. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        sal_memset(entry_words[unit][part],
                   0,
                   sizeof(entry_words[unit][part]));
        entry_words_row[unit][part] = entry_words[unit][part];
    }

    if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     entry_words_row[unit]));
    } else {
        /* First write the associated tcam key to index. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     entry_words_row[unit]));
        /* Next write the data words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     entry_words_row[unit]));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_shared_info_update(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd,
                                            bool clear,
                                            uint32_t index)
{
    uint16_t idx = 0;
    uint16_t shared_info_count = 0;
    bcmltd_sid_t target_ltid;
    uint32_t target_index = 0;
    bcmptm_rm_tcam_lt_info_t target_ltid_info;
    bcmptm_rm_tcam_prio_only_shr_info_t *shared_info = NULL;
    bcmptm_rm_tcam_prio_only_info_t *target_tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *target_entry_info = NULL;
    bcmptm_rm_tcam_trans_info_t *ltid_trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;
    uint8_t is_half_mode = 0;
    uint8_t shared_half_mode = 0;
    uint16_t update_key_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    int update_prio = BCMPTM_RM_TCAM_PRIO_INVALID;
    bool is_next_index_considered = FALSE;
    uint16_t num_entries = 0;
    uint32_t offset = 0;
    uint8_t mode_different = 0;
    int8_t target_entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_info = iomd->entry_info_arr.entry_info[0][0];

    /* Fetch the shared TCAM info of this LTID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_shared_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  &shared_info,
                                                  &shared_info_count));
    /*
     * For half mode LT's policy is written at index "index*2",
     * whereas for LT's sharing the same TCAM's, policy is written
     * at "index" location.
     * To avoid overlapping of policy's of different LT's,
     * we should
     * 1) Block the TCAM index and index*2 for LT's not supporting half mode
     *   when an insert of entry is happened at index in half mode supported LT.
     * 2) Block the TCAM index and index/2 for LT's supporting half mode
     *   when an insert of entry is happened at index in  LT's not supporting
     *   half mode.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_check_butterfly_mode(unit,
                                             iomd->ltid,
                                             iomd->ltid_info,
                                             &is_half_mode));

    /*
     * For half mode we consider number of entries is num_entries * 2 as
     * each physical entry can hold 2 logical entries.
     */
    update_prio = entry_info[index].entry_pri;
    update_key_type = entry_info[index].entry_type;
    if (update_prio == BCMPTM_RM_TCAM_PRIO_INVALID) {
        update_prio = 0;
    }

    for (idx = 0; idx < shared_info_count; idx++) {
        mode_different = 0;
        is_next_index_considered = 0;
        shared_half_mode = 0;
        /*
         * Check if the LTID TCAM index falls under any of
         * the shared TCAM index range.
         */
        if (shared_info[idx].start_index <= index &&
            shared_info[idx].end_index >= index) {
            /*
             * Target index is TCAM index in target LTID that
             * is mapped to TCAM index in the current LTID.
             */
            target_ltid = shared_info[idx].shr_ltid;
            sal_memset(&target_ltid_info,
                       0,
                       sizeof(bcmptm_rm_tcam_lt_info_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ptrm_info_get(unit,
                                              target_ltid,
                                              0,
                                              0,
                                              ((void *)&target_ltid_info),
                                              NULL,
                                              NULL));
            target_entry_type =
                   target_ltid_info.hw_entry_info->key_type_val[0][0];

            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_check_butterfly_mode(unit,
                                                      target_ltid,
                                                      &target_ltid_info,
                                                      &shared_half_mode));

            offset = (index -shared_info[idx].start_index);
            if (1 == is_half_mode  && 0 == shared_half_mode) {
                offset = offset / 2;
                mode_different = 1;
            } else if (0 == is_half_mode  && 1 == shared_half_mode) {
                offset = offset * 2;
                is_next_index_considered = TRUE;
                mode_different = 1;
            }
            target_index = (offset  + shared_info[idx].shr_start_index);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                        target_ltid,
                                                        &target_ltid_info,
                                                        iomd->pipe_id,
                                                        &target_tcam_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                         target_ltid,
                                                         &target_ltid_info,
                                                         iomd->pipe_id,
                                                         &target_entry_info));
            if (clear == TRUE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_shared_update_get_key_type_prio(unit,
                                                            target_ltid,
                                                            &target_ltid_info,
                                                            target_index,
                                                            iomd->pipe_id,
                                                            shared_half_mode,
                                                            &update_key_type,
                                                            &update_prio));
            } else {
                if (target_entry_type ==
                    target_entry_info[target_index].entry_type) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            /* Update the target TCAM info. */
            target_entry_info[target_index].entry_pri = update_prio;
            target_entry_info[target_index].entry_type = update_key_type;
            target_entry_info[target_index].offset =
                                            BCMPTM_RM_TCAM_OFFSET_INVALID;

            /* Update the transaction state of the target LTID. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_info_get(unit,
                                         target_ltid,
                                         iomd->pipe_id,
                                         &target_ltid_info,
                                         (void *)&ltid_trans_info));
            if (ltid_trans_info->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                ltid_trans_info->trans_state = BCMPTM_RM_TCAM_STATE_UC_A;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_trans_info_global_get(unit,
                                               &trans_info_global));
                if (trans_info_global->trans_state ==
                                       BCMPTM_RM_TCAM_STATE_IDLE) {
                    trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_UC_A;
                }
            }

            if (is_next_index_considered == TRUE) {
                /*
                 * Shared lt is in half mode so need to block
                 * 2 indexes.
                 */
                target_index += 1;
                if (clear == TRUE) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_shared_update_get_key_type_prio(unit,
                                                           target_ltid,
                                                           &target_ltid_info,
                                                           target_index,
                                                           iomd->pipe_id,
                                                           shared_half_mode,
                                                           &update_key_type,
                                                           &update_prio));
                } else {
                    if (target_entry_type ==
                        target_entry_info[target_index].entry_type) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }
                /* Update the target TCAM info. */
                target_entry_info[target_index].entry_pri = update_prio;
                target_entry_info[target_index].entry_type = update_key_type;
                target_entry_info[target_index].offset =
                                                BCMPTM_RM_TCAM_OFFSET_INVALID;
            }

            /*
             * If either of the LT's i.e., main LT where the entry is inserted
             * or shared LT are in half mode, enter the if condition.
             * Code to block the data. WAR for chips which uses consecutive
             * locations for half mode and full mode data entries. A full mode
             * or half mode entry can potentially block another entry. This
             * index will be exactly same as the index input to this function.
             * Compute this index.
             */
            if (iomd->ltid_info->hw_entry_info->view_type) {
                continue;
            }

            if ((mode_different == 1) &&
                (shared_info[idx].shr_flags & BCMPTM_RM_TCAM_BOTH_KEY_KEY0)) {
                offset = (index -shared_info[idx].start_index);
                target_index = (offset + shared_info[idx].shr_start_index);
                if (is_half_mode == 1) {
                   /*
                    * Current LT is half mode and shared LT is not half mode.
                    * If index is greater than the half we don't need to block
                    * for data. Check if index is greater than the valid entry
                    * indices of the current LT.
 */
                    num_entries = (shared_info[idx].end_index -
                                   shared_info[idx].start_index);
                    if (offset > (num_entries / 2)) {
                        continue;
                    }
                    /*
                     * Need to block data also  This will block one additional
                     * entry at exact index location due to data
                     */
                }
               /*
                * Target index is TCAM index in target LTID that is
                * mapped to TCAM index in the current LTID.
                */
                if (clear == TRUE) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_shared_update_get_key_type_prio(unit,
                                                           target_ltid,
                                                           &target_ltid_info,
                                                           target_index,
                                                           iomd->pipe_id,
                                                           shared_half_mode,
                                                           &update_key_type,
                                                           &update_prio));
                } else {
                    if (target_entry_type ==
                        target_entry_info[target_index].entry_type) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }
                target_entry_info[target_index].entry_pri = update_prio;
                target_entry_info[target_index].entry_type = update_key_type;
                target_entry_info[target_index].offset =
                                                BCMPTM_RM_TCAM_OFFSET_INVALID;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

/* Calculate the hash value for the entry key and data */
int
bcmptm_rm_tcam_prio_only_entry_hash_value_get(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    uint32_t **ekw,
                                    uint32_t **edw,
                                    uint32_t *hash_val)
{
    uint8_t part;
    uint16_t num_bits = 0;
    uint32_t total_bits;
    int num_parts = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(hash_val, SHR_E_PARAM);

    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    /* Calculate the number of bits for hashing */
    total_bits = 0;
    if (ltid_info->hw_entry_info->view_type)  {
        num_parts = (ltid_info->hw_entry_info->num_key_rows /
                     ltid_info->hw_entry_info->view_type);
    } else if (ltid_info->rm_more_info->flags &
               BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_parts = ltid_info->hw_entry_info->num_key_rows / 2;
    } else {
        num_parts = ltid_info->hw_entry_info->num_key_rows;
    }
    for (part = 0; part < num_parts; part++) {
        num_bits = ltid_info->hw_entry_info->key_size;
        bcmdrd_field_get(ekw[part], 0, (num_bits - 1), g_buf_one[unit]);
        bcmdrd_field_set(g_buf_two[unit],
                         total_bits,
                         total_bits + (num_bits -1),
                         g_buf_one[unit]);
        total_bits += num_bits;
    }

    *hash_val = shr_crc16b(0,
                           (uint8_t *)&g_buf_two[unit],
                           total_bits);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                BCMPTM_RM_TCAM_GLOBAL_PIPE,
                                                &tcam_info));

    *hash_val = (*hash_val %
                (tcam_info->entry_hash_size /
                 tcam_info->num_lookups));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function used to verify if the key is same as the one
 * present in the index provided.
 */
int
bcmptm_rm_tcam_prio_only_entry_compare(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd,
                               uint32_t index)
{
    int rv = SHR_E_NONE;
    uint8_t part;
    uint8_t num_bytes = 0;
    uint16_t num_bits = 0;
    uint32_t rm_flags = 0;
    int num_parts = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part],
                    0,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    tcam_info = iomd->tcam_info;

    req_flags = iomd->req_flags;
    iomd->req_flags = 0;
    if (tcam_info->flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        /* Read the entry words from from_idx. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    index,
                                    entry_words_row[unit]));
    } else {
        /* Now move TCAM only portion of LTID. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        /* Read the TCAM Only words from PTCache. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    index,
                                    entry_words_row[unit]));
    }
    iomd->req_flags = req_flags;

    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    if (iomd->ltid_info->hw_entry_info->view_type)  {
        num_parts = (iomd->ltid_info->hw_entry_info->num_key_rows /
                     iomd->ltid_info->hw_entry_info->view_type);
    } else if (tcam_info->num_lookups == 2) {
        num_parts = iomd->ltid_info->hw_entry_info->num_key_rows / 2;
    } else  {
        num_parts = iomd->ltid_info->hw_entry_info->num_key_rows;
    }
      for (part = 0; part < num_parts; part++) {
        num_bits = iomd->ltid_info->hw_entry_info->key_size;
        bcmdrd_field_get(entry_words[unit][part],
                         0,
                         (num_bits - 1),
                         g_buf_one[unit]);
        bcmdrd_field_get(iomd->req_info->ekw[part],
                         0,
                         (num_bits - 1),
                         g_buf_two[unit]);
        num_bytes = num_bits / 8;
        num_bits = num_bits % 8;

        if (num_bits) {
            num_bytes++;
        }
        rv = sal_memcmp(g_buf_one[unit], g_buf_two[unit], num_bytes);
        if (rv) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (iomd->req_flags == 0) {
            iomd->req_flags = req_flags;
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * Function used to verify if the request key is same as the one
 * present in the index provided in traverse snapshot
 */
int
bcmptm_rm_tcam_traverse_prio_only_entry_compare(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                uint32_t index,
                                                bcmptm_rm_tcam_req_t *req_info,
                                                uint32_t **key_db,
                                                uint16_t key_size)
{
    int rv = SHR_E_NONE;
    uint8_t part;
    uint8_t num_bytes = 0;
    uint16_t num_bits = 0;
    uint8_t num_key_rows = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(key_db, SHR_E_PARAM);

    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    num_bits = ltid_info->hw_entry_info->key_size;

    num_bytes = (num_bits + 7) / 8;

    num_key_rows = ltid_info->num_ekw;
    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_key_rows = num_key_rows/2;
    }
    for (part = 0; part < num_key_rows; part++) {
        SHR_NULL_CHECK(key_db[part], SHR_E_PARAM);
        bcmdrd_field_get((key_db[part] + (index * key_size)),
                         0, (num_bits - 1), g_buf_one[unit]);

        bcmdrd_field_get(req_info->ekw[part], 0, (num_bits - 1),
                         g_buf_two[unit]);

        rv = sal_memcmp(g_buf_one[unit], g_buf_two[unit], num_bytes);
        if (rv) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Move TCAM entry from one index to the other index.
 * Both HW and SW state will be updated.
 */
int
bcmptm_rm_tcam_prio_only_entry_move(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t partition,
                                   uint32_t prefix,
                                   uint32_t from_idx,
                                   uint32_t to_idx,
                                   void *attrs,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part = 0;
    uint32_t hash_val;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t num_key_rows = 0;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];
    entry_hash = iomd->entry_hash_ptr;

    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows =
            iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    /* If the entry is empty no need to move. */
    if (entry_info[from_idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID ||
       (iomd->ltid_info->hw_entry_info->entry_boundary != 0 &&
        from_idx % iomd->ltid_info->hw_entry_info->entry_boundary != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_info_clear_sw(unit,
                                                          iomd,
                                                          from_idx,
                                                          num_key_rows));
        SHR_EXIT();
    }

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        sal_memset(entry_words[unit][part],
                   0,
                   sizeof(entry_words[unit][part]));
        entry_words_row[unit][part] = entry_words[unit][part];
    }

    if (tcam_info->flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        /* Read the entry words from from_idx. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    from_idx,
                                    entry_words_row[unit]));
        /* Write the entry words to to_idx. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     to_idx,
                                     entry_words_row[unit]));
    } else {
        /* First move associated data portion of LTID. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        /* Read the Associated data words from PTcache. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    from_idx,
                                    entry_words_row[unit]));

        /* Write the associated data words to PTcache.  */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     to_idx,
                                     entry_words_row[unit]));
        /* Now move TCAM only portion of LTID. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        /* Read the TCAM Only words from PTCache. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    from_idx,
                                    entry_words_row[unit]));
        /* Write the TCAM only words to PTCache. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     to_idx,
                                     entry_words_row[unit]));
    }

    /* Update the SW state in three steps. */

    /* 1. Update the TCAM entry information at to_idx. */
    entry_info[to_idx].offset = entry_info[from_idx].offset;

    if (ltid_info->rm_more_info->flags &
                   BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    for (idx = to_idx; idx < (to_idx + num_key_rows); idx++) {
        entry_info[idx].entry_pri = entry_info[from_idx].entry_pri;
        entry_info[idx].entry_type =
                  iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    }

    /* 2. Update the the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit,
                                                       iomd->ltid,
                                                       iomd->ltid_info,
                                                       entry_words_row[unit],
                                                       NULL,
                                                       &hash_val));

    curr_offset = entry_hash[hash_val];
    while (curr_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (curr_offset != (int)from_idx) {
            prev_offset = curr_offset;
            curr_offset = entry_info[curr_offset].offset;
        } else {
            break;
        }
    }

    if ((curr_offset == BCMPTM_RM_TCAM_OFFSET_INVALID)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = to_idx;
    } else {
        entry_hash[hash_val] = to_idx;
    }

    /* Clear the HW state for from_idx */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_clear_hw(unit, iomd, from_idx));

    if (tcam_info->shr_info_count != 0) {
        for (idx = to_idx; idx < (to_idx + num_key_rows); idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                             iomd,
                                                             FALSE,
                                                             idx));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_info_clear_sw(unit,
                                                      iomd,
                                                      from_idx,
                                                      num_key_rows));

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_find_loops(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Write the entry words to a particular TCAM index.
 * Updates both HW and SW state
 */
int
bcmptm_rm_tcam_prio_only_entry_write(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t hw_only,
                                     int index)

{
    int *entry_hash = NULL;
    uint32_t hash_val = 0;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t num_key_rows = 0;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];
    entry_hash = iomd->entry_hash_ptr;

    /* Update the HW */
    if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     iomd->req_info->ekw));
    } else {

        /*  First write the associated data to index. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     iomd->req_info->edw));

        /* Next write the key words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     index,
                                     iomd->req_info->ekw));
    }

    if (hw_only) {
        SHR_EXIT();
    }
    /* Update the SW state in two steps. */

    /* 1. Update the TCAM entry information. */
    entry_info[index].entry_pri = iomd->req_info->entry_pri;
    entry_info[index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;

    /* 2. Update the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit,
                                                       iomd->ltid,
                                                       iomd->ltid_info,
                                                       iomd->req_info->ekw,
                                                       NULL,
                                                       &hash_val));

    entry_info[index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = index;

    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    for (idx = index; idx < (index + num_key_rows); idx++) {
        entry_info[idx].entry_pri = iomd->req_info->entry_pri;
        entry_info[idx].entry_type =
                  iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    }
    if (tcam_info->shr_info_count != 0) {
        for (idx = index; idx < (index + num_key_rows); idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                             iomd,
                                                             FALSE,
                                                             idx));
        }
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_find_loops(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Read the entry words from a particular TCAM index.
 */
int
bcmptm_rm_tcam_prio_only_entry_read(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    int index)
{
    int rv = SHR_E_NONE;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;

    /* Read the TCAM entry words. */
    if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        /* Read the  the entry words from index using aggregated view. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    index,
                                    iomd->rsp_info->rsp_ekw);

        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /* Read the  the entry words from index using TCAM only view. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    index,
                                    iomd->rsp_info->rsp_ekw);

        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* Read the  the data words from index using DATA only view. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    index,
                                    iomd->rsp_info->rsp_edw);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (iomd->pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        iomd->rsp_info->rsp_entry_pipe = 0;
    } else {
        iomd->rsp_info->rsp_entry_pipe = iomd->pt_dyn_info->tbl_inst;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Clear a TCAM entry at specific index. Involves writing
 * all zeros to a particulat TCAM index. Updates both HW
 * and SW state.
 */
int
bcmptm_rm_tcam_prio_only_entry_clear(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t entry_update_flags,
                                     uint32_t index)
{
    int *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t hash_val = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t num_key_rows = 0;
    uint32_t idx = 0;
    uint8_t hw_only = 0;
    uint8_t sw_only = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];
    entry_hash = iomd->entry_hash_ptr;

    sw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY;
    hw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY;

    /* if entry is invalid no need to clear it. */
    if (entry_info[index].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID &&
        !hw_only) {
        SHR_EXIT();
    }

    if (!sw_only) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_clear_hw(unit, iomd, index));
        if (hw_only) {
            SHR_EXIT();
        }
    }

    /* Update the SW state in two steps. */
    /* 1. Update the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit,
                                                       iomd->ltid,
                                                       iomd->ltid_info,
                                                       iomd->req_info->ekw,
                                                       NULL,
                                                       &hash_val));

    curr_offset = entry_hash[hash_val];
    while (curr_offset != (int)index) {
        prev_offset = curr_offset;
        curr_offset = entry_info[curr_offset].offset;
    }

    if (curr_offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = entry_info[curr_offset].offset;
    } else {
        entry_hash[hash_val] = entry_info[curr_offset].offset;
    }

    /* 2. Update the TCAM entry information. */
    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    for (idx = index; idx < (index + num_key_rows); idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    if (tcam_info->shr_info_count != 0) {
        for (idx = index; idx < (index + num_key_rows); idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                             iomd,
                                                             TRUE,
                                                             idx));
        }
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_find_loops(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *  Move the range of TCAM entries one index up or down.
 */
int
bcmptm_rm_tcam_prio_only_entry_move_range(int unit,
                                          bcmptm_rm_tcam_entry_iomd_t *iomd,
                                          uint32_t idx_start,
                                          uint32_t idx_end,
                                          uint8_t entry_type)
{
    int rv = SHR_E_NONE;
    int idx = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    int count = 0;
    int num_key_rows = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info;
    bool reserved_move = false;
    uint32_t from_idx = 0;
    uint32_t to_idx = 0;
    int32_t reserved_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];

    if (iomd->entry_attrs->lookup_id == 1) {
        reserved_index = tcam_info->reserved_entry_index1;
    } else {
        reserved_index = tcam_info->reserved_entry_index2;
    }


    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    /* Shift entries in down ward direction(i.e. one index up). */
    if (idx_end > idx_start) {
        for (idx = (idx_end - 1);
            (idx >= (int)idx_start) && (idx < (int)idx_end);
            idx--) {
            /*
             * Condition to check for eligibility for move
             * source entry should be of the same type - first check
             * Desitntaion location should be free checked by priority
             * and entry type invalid or should be a reserved entry
             * (checked by combination of entry type same and priority
             * invalid. IS_TARGET_ELIGIBLE macro checks this.
             */
            if ((entry_info[idx].entry_type == entry_type) &&
               (IS_TARGET_ENTRY_ELIGIBLE(entry_info,
                                        (idx + num_key_rows + count),
                                        entry_type))) {
                from_idx = idx - num_key_rows + 1;
                to_idx = idx + 1 + count;
                if (reserved_index == (int32_t)from_idx) {
                    if (entry_info[from_idx].entry_pri ==
                        BCMPTM_RM_TCAM_PRIO_INVALID) {
                        reserved_move = true;
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }
                rv = bcmptm_rm_tcam_entry_move(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               0,
                                               from_idx,
                                               to_idx,
                                               iomd->entry_attrs,
                                               iomd->pt_dyn_info,
                                               iomd);
                idx = idx - num_key_rows + 1;
                count = 0;
                if (reserved_move == true) {
                    /*
                     * If reserved entry is moved update the reserve entry
                     * This will happen during new index allocation
                     * since reserve entry is not used for new index allocation.
                     * so it can be from entry
                     */
                    bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                                  iomd,
                                                  to_idx,
                                                  entry_type,
                                                  iomd->entry_attrs->lookup_id,
                                                  FALSE);
                    reserved_move = false;
                }
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Shift entries in upward direction(i.e. one index down). */
    if (idx_end < idx_start) {
        for (idx = idx_end + num_key_rows; idx <
             (int)idx_start+num_key_rows; idx++) {
            /*
             * Condition to check for eligibility for move
             * source entry should be of the same type - first check
             * Desitntaion location should be free checked by priority
             * and entry type invalid or should be a reserved entry
             * (checked by combination of entry type same and priority
             * invalid. IS_TARGET_ELIGIBLE macro checks this.
             */
            if ((entry_info[idx].entry_type == entry_type) &&
                (IS_TARGET_ENTRY_ELIGIBLE(entry_info,
                                         (idx - num_key_rows - count),
                                         entry_type))) {
                from_idx = idx;
                to_idx = idx - num_key_rows - count;
                if (reserved_index == (int32_t)from_idx) {
                    if (entry_info[from_idx].entry_pri ==
                        BCMPTM_RM_TCAM_PRIO_INVALID) {
                        reserved_move = true;
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }
                rv = bcmptm_rm_tcam_entry_move(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               0,
                                               idx,
                                               idx - num_key_rows - count,
                                               iomd->entry_attrs,
                                               iomd->pt_dyn_info,
                                               iomd);
                idx = idx + num_key_rows - 1;
                count = 0;
                if (reserved_move == true) {
                    /*
                     * If reserved entry is moved update the reserve entry
                     * This will happen during new index allocation
                     * since reserve entry is not used for new index
                     * allocation. So it can be from entry
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                                iomd,
                                                to_idx,
                                                entry_type,
                                                iomd->entry_attrs->lookup_id,
                                                FALSE));
                    reserved_move = false;
                }
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
