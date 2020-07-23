/*! \file rm_tcam_prio_atomicity.c
 *
 * Low Level Functions for Atomicity support of TCAMs
 * This file contains low level functions for Atomicity support
 * of TCAMs during TCAM entry Update.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private variables
 */
static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                           [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS]
                           [BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/*
 * This routine updates the existing entry along with
 * changing its priority (if any). This is called only
 * for prio_only type TCAMs.
 */
static int
bcmptm_rm_tcam_prio_only_entry_prio_update(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{

    int *entry_hash = NULL;
    uint32_t hash_val = 0;
    uint32_t target_index = -1;
    int rv = SHR_E_NONE;
    uint8_t num_key_rows = 0;
    uint32_t idx = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint8_t reserve_used = 0;
    int8_t entry_type = 0;
    uint32_t found_index = 0;
    uint8_t lookup_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Find Index to Insert the TCAM entry */
    rv = bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                       iomd,
                                                       1,
                                                       &target_index,
                                                       &reserve_used,
                                                       NULL);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
    if (rv == SHR_E_RESOURCE) {
        /*call compress API and call allocate again */
        reserve_used = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_compress(unit, iomd));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                           iomd,
                                                           1,
                                                           &target_index,
                                                           &reserve_used,
                                                           NULL));
    }
    /* Insert the new entry at target_idx. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_write(unit,
                                              iomd,
                                              1,
                                              target_index));

    /* Since original entry may have got moved, lookup to find found_index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_lookup(unit,
                                               iomd->req_flags,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               iomd->pt_dyn_info,
                                               iomd->req_info,
                                               iomd->rsp_info,
                                               iomd->rsp_ltid,
                                               iomd->rsp_flags,
                                               &found_index,
                                               iomd));


    /* Clear the data in SW and HW state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_clear(unit,
                                              iomd,
                                              0,
                                              found_index));

    /* Update the SW state in two steps. */
    tcam_info = iomd->tcam_info;
    entry_info = iomd->entry_info_arr.entry_info[0][0];
    entry_hash = iomd->entry_hash_ptr;
    hash_val = iomd->entry_hash_value;

    /* 1. Update the TCAM entry information. */
    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        num_key_rows = 1;
    }

    for (idx = target_index; idx < (target_index + num_key_rows); idx++) {
        entry_info[idx].entry_pri = iomd->req_info->entry_pri;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type =
                        iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    }

    /* 2. Update the TCAM SW hash table. */
    entry_info[target_index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = target_index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                             iomd,
                                             target_index));


    if (tcam_info->shr_info_count != 0) {
       for (idx = target_index;
            idx < (target_index + num_key_rows);
            idx++) {
           SHR_IF_ERR_VERBOSE_EXIT
               (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                            iomd,
                                                            0,
                                                            idx));
       }
    }
    if (reserve_used == 1) {
        entry_type =
             (int8_t)iomd->ltid_info->hw_entry_info->key_type_val[0][0];
        lookup_id = iomd->entry_attrs->lookup_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                                     iomd,
                                                     found_index,
                                                     entry_type,
                                                     lookup_id,
                                                     FALSE));
    }

    iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;
    iomd->target_index = target_index;
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_entry_info(unit, iomd, 0));
     }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine updates the existing entry along with
 * changing its priority (if any). This is called only
 * for prio_eid type TCAMs.
 */
static int
bcmptm_rm_tcam_prio_eid_entry_prio_update(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t found_index = 0;
    uint32_t new_target_index = 0;
    uint8_t reserved_used = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Reserve an index to insert the TCAM entry */
    iomd->prio_change = TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        ((bcmptm_rm_tcam_prio_eid_entry_index_allocate(unit,
                                                       iomd,
                                                       &reserved_used)));
    iomd->prio_change = FALSE;
    new_target_index = iomd->target_index;

    /* Insert the new entry at target_idx only in HW. */
    iomd->hw_only = TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));
    iomd->hw_only = FALSE;

    /*
     * Since original entry may have got moved, lookup to
     * find found_index.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_lookup(unit,
                                          iomd->req_flags,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->pt_dyn_info,
                                          iomd->req_info,
                                          iomd->rsp_info,
                                          iomd->rsp_ltid,
                                          iomd->rsp_flags,
                                          &found_index,
                                          iomd));
    /* Clear the data in SW and HW state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_clear(unit, iomd));

    /* Insert the new entry at target_idx only in software. */
    iomd->sw_only = TRUE;
    iomd->target_index = new_target_index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));
    iomd->sw_only = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
    iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;
    same_key_info[unit].prev_index = found_index;
    same_key_info[unit].index = new_target_index;
    iomd->target_index = found_index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_free_count_incr(unit, iomd));
    iomd->target_index = new_target_index;

    if (reserved_used == 1) {
        /* update FP group's and shared groups reserved index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_update_reserve_entry_index(unit,
                                                     iomd,
                                                     iomd->reserved_index,
                                                     found_index));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines does an entry (key + data) update operation for
 * PRIO_ONLY type TCAMs
 */
static int
bcmptm_rm_tcam_prio_only_entry_update(int unit,
                                      uint8_t update_type,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_req_t tmp_req_info;
    uint8_t mode = 0;
    int num_parts = -1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = iomd->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }
    hw_entry_info = &iomd->ltid_info->hw_entry_info[mode];
    if (hw_entry_info->view_type)  {
        num_parts = (hw_entry_info->num_key_rows /
                     hw_entry_info->view_type);
    } else  {
        num_parts = hw_entry_info->num_key_rows;
    }

    if (num_parts < hw_entry_info->num_data_rows) {
        num_parts = hw_entry_info->num_data_rows;
    }

    /*
     * Trigger SW atomicity either if
     * 1. HW doesn't support atomicity or
     * 2. HW supports atomicity but entry modification
     *    is in either key or both data + key and num key parts is more then 1.
     *    Refer below table where atomicity can't be achieved
     *    even when HW supports it.
     *                Agrr      |     Non_aggr
     *   ekw        1  | >1     |     1   | >1
     *   -------------------------------------
     *   key only   Y  | N      |     Y   | N
     *   key+Data   Y  | N      |     N   | N
     *   Data only  Y  | Y      |     Y   | Y
     */

    if ((iomd->ltid_info->rm_more_info->hw_atomicity_support == 0) ||
        (iomd->ltid_info->rm_more_info->hw_atomicity_support &&
        (update_type != BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
        ((num_parts > 1) ||
        (iomd->ltid_info->non_aggr && (update_type == 0))))) {
        /* Entry update using backup entry creation */
        SHR_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_entry_prio_update(unit, iomd));
    } else {
        /* HW Atomicity can be acheived, update entry at the same index */
        /* Read exiting entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                 iomd,
                                                 iomd->target_index));

        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_attrs = iomd->entry_attrs;
        tmp_req_info.ekw = iomd->rsp_info->rsp_ekw;
        tmp_req_info.edw = iomd->rsp_info->rsp_edw;
        tmp_req_info.rsp_ekw_buf_wsize = iomd->req_info->rsp_ekw_buf_wsize;
        tmp_req_info.rsp_edw_buf_wsize = iomd->req_info->rsp_edw_buf_wsize;

        /* Delete entry at found_index in SW only mode. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_clear(unit,
                                            iomd,
                                            BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY,
                                            iomd->target_index));
        /* Update the entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_write(unit,
                                                  iomd,
                                                  0,
                                                  iomd->target_index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                             iomd,
                                             iomd->target_index));

    iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;

exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines does an entry (key + data) update operation for
 * PRIO_EID type TCAMs
 */
static int
bcmptm_rm_tcam_prio_eid_entry_update(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{

    bcmptm_rm_tcam_req_t tmp_req_info;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    num_parts = iomd->num_parts;

    /*
     * Trigger SW atomicity either if
     * 1. HW doesn't support atomicity or
     * 2. HW supports atomicity but entry modification
     *    is in either key or both data + key and num key parts is more then 1.
     *    Refer below table where atomicity can't be achieved
     *    even when HW supports it.
     *                Agrr      |     Non_aggr
     *   ekw        1  | >1     |     1   | >1
     *   -------------------------------------
     *   key only   Y  | N      |     Y   | N
     *   key+Data   Y  | N      |     N   | N
     *   Data only  Y  | Y      |     Y   | Y
     */

    if ((iomd->ltid_info->rm_more_info->hw_atomicity_support == 0) ||
        (iomd->ltid_info->rm_more_info->hw_atomicity_support &&
        (iomd->update_type != BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
        ((num_parts > 1) || (iomd->ltid_info->non_aggr &&
        (iomd->update_type == 0))))) {
        /* Entry update using backup entry creation */
        SHR_ERR_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_prio_update(unit, iomd));
    } else {
        /* HW Atomicity can be acheived, update entry at the same index */
        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_attrs = iomd->entry_attrs;
        tmp_req_info.entry_id = iomd->req_info->entry_id;

        /* Delete entry at found_index in SW only mode. */
        iomd->sw_only = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_clear(unit, iomd));
        iomd->sw_only = FALSE;

        /* Update the entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
    iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/*
 * This routine helps to identify if the update requested
 * for an entry is a change in data portion and key portion
 * not getting updated.
 * The output variable update_type = 2 for data portion only update.
 * The output variable update_type = 1 for key portion only change.
 * The output variable update_type = 0 for both key and data change.
 */
int
bcmptm_rm_tcam_update_type_get(int unit,
                               uint8_t *update_type,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t part = 0;
    uint8_t word = 0;
    uint16_t key_length = 0;
    uint16_t data_length = 0;
    uint32_t num_words = 0;
    uint32_t rsp_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t rsp_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t req_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t req_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    int num_parts = -1;
    uint8_t offset = 0;
    uint16_t num_data_offsets = 0;
    uint16_t num_key_offsets = 0;
    uint16_t start_bit = 0;
    uint16_t end_bit = 0;

    SHR_FUNC_ENTER(unit);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                 iomd,
                                                 iomd->target_index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_lt_hw_entry_info_get(unit,
                                             iomd->ltid_info,
                                             iomd->entry_attrs,
                                             &hw_entry_info));

    key_length = hw_entry_info->key_size;
    /* If data change is found set data only indicator */
    if (hw_entry_info->tlb_format) {
        for (part = 0; part < hw_entry_info->num_data_rows; part++) {
            num_data_offsets = hw_entry_info->tlb_num_data_offsets[part];
            for (offset = 0; offset < num_data_offsets; offset++) {
                sal_memset(rsp_edw, 0, sizeof(rsp_edw));
                sal_memset(req_edw, 0, sizeof(req_edw));
                start_bit =
                    hw_entry_info->tlb_data_start_bit_offset[part][offset];
                end_bit =
                    hw_entry_info->tlb_data_end_bit_offset[part][offset];
                data_length = end_bit - start_bit + 1;
                if (iomd->ltid_info->non_aggr) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_data_get(unit,
                                        iomd->rsp_info->rsp_edw[part],
                                        start_bit,
                                        data_length,
                                        rsp_edw));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_data_get(unit,
                                        iomd->req_info->edw[part],
                                        start_bit,
                                        data_length,
                                        req_edw));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_data_get(unit,
                                        iomd->rsp_info->rsp_ekw[part],
                                        start_bit,
                                        data_length,
                                        rsp_edw));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_data_get(unit,
                                        iomd->req_info->ekw[part],
                                        start_bit,
                                        data_length,
                                        req_edw));
                }

                num_words = BCMDRD_BYTES2WORDS((data_length + 7) / 8);
                for (word = 0; word < num_words; word++) {
                    if (req_edw[word] != rsp_edw[word]) {
                        *update_type = BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE;
                        break;
                    }
                }
                if (*update_type == BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) {
                    break;
                }
            }
            if (*update_type == BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) {
                break;
            }
        }
    } else {
        for (part = 0; part < hw_entry_info->num_data_rows; part++) {
            sal_memset(rsp_edw, 0, sizeof(rsp_edw));
            sal_memset(req_edw, 0, sizeof(req_edw));
            data_length = hw_entry_info->data_end_bit[part]  -
                      hw_entry_info->data_start_bit + 1;

            if (iomd->ltid_info->non_aggr) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->rsp_info->rsp_edw[part],
                                    0,
                                    data_length,
                                    rsp_edw));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->req_info->edw[part],
                                    0,
                                    data_length,
                                    req_edw));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->rsp_info->rsp_ekw[part],
                                    hw_entry_info->data_start_bit,
                                    data_length,
                                    rsp_edw));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->req_info->ekw[part],
                                    hw_entry_info->data_start_bit,
                                    data_length,
                                    req_edw));
            }

            num_words = BCMDRD_BYTES2WORDS((data_length + 7) / 8);
            for (word = 0; word < num_words; word++) {
                if (req_edw[word] != rsp_edw[word]) {
                    *update_type = BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE;
                    break;
                }
            }
        }
    }

    /* If key change is found unset data only indicator */
    if (hw_entry_info->view_type)  {
        num_parts = (hw_entry_info->num_key_rows /
                     hw_entry_info->view_type);
    } else if (iomd->ltid_info->rm_more_info->flags &
               BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_parts = hw_entry_info->num_key_rows / 2;
    } else {
        num_parts = hw_entry_info->num_key_rows;
    }

    if (hw_entry_info->tlb_format) {
        for (part = 0; part < num_parts; part++) {
            num_key_offsets = hw_entry_info->tlb_num_key_offsets[part];
            for (offset = 0; offset < num_key_offsets; offset++) {
                sal_memset(rsp_ekw, 0, sizeof(rsp_ekw));
                sal_memset(req_ekw, 0, sizeof(req_ekw));
                start_bit =
                    hw_entry_info->tlb_key_start_bit_offset[part][offset];
                end_bit =
                    hw_entry_info->tlb_key_end_bit_offset[part][offset];
                key_length = end_bit - start_bit + 1;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->rsp_info->rsp_ekw[part],
                                    start_bit,
                                    key_length,
                                    rsp_ekw));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_data_get(unit,
                                    iomd->req_info->ekw[part],
                                    start_bit,
                                    key_length,
                                    req_ekw));

                num_words = BCMDRD_BYTES2WORDS((key_length + 7) / 8);
                for (word = 0; word < num_words; word++) {
                    if (req_ekw[word] != rsp_ekw[word]) {
                        if (*update_type &
                            BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) {
                            *update_type = 0;
                        } else {
                            *update_type =
                                 BCMPTM_RM_TCAM_ENTRY_KEY_ONLY_UPDATE;
                        }
                        if (!(iomd->ltid_info->pt_type ==
                              LT_PT_TYPE_TCAM_PRI_ONLY)) {
                            iomd->update_type = *update_type;
                        }
                        SHR_EXIT();
                    }
                }
            }
        }
    } else {
        for (part = 0; part < num_parts; part++) {
            sal_memset(rsp_ekw, 0, sizeof(rsp_ekw));
            sal_memset(req_ekw, 0, sizeof(req_ekw));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_data_get(unit,
                                iomd->rsp_info->rsp_ekw[part],
                                0,
                                key_length,
                                rsp_ekw));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_data_get(unit,
                                iomd->req_info->ekw[part],
                                0,
                                key_length,
                                req_ekw));

            num_words = BCMDRD_BYTES2WORDS((key_length + 7) / 8);
            for (word = 0; word < num_words; word++) {
                if (req_ekw[word] != rsp_ekw[word]) {
                    if (*update_type &
                        BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) {
                        *update_type = 0;
                    } else {
                        *update_type =
                             BCMPTM_RM_TCAM_ENTRY_KEY_ONLY_UPDATE;
                    }
                    if (!(iomd->ltid_info->pt_type ==
                          LT_PT_TYPE_TCAM_PRI_ONLY)) {
                        iomd->update_type = *update_type;
                    }
                    SHR_EXIT();
                }
            }
        }
    }

    /* If no  changes are found, return as Exists */
    if (*update_type == 0) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine is called to update only the data portion
 * in the TCAM
 */
int
bcmptm_rm_tcam_entry_update_data_only(int unit,
                            bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t part = 0;
    uint32_t width = 0;
    uint16_t data_length = 0;
    uint8_t slice_row = 0;
    uint8_t slice_col = 0;
    uint32_t sindex = -1;
    int num_words = -1;
    uint64_t rm_flags = 0;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    const bcmltd_sid_t **sid_array = NULL;

    SHR_FUNC_ENTER(unit);

    ltid_info = iomd->ltid_info;
    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part], 0 ,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    /* Set the data only view */
    rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

    if (!(ltid_info->non_aggr)) {
         width = ltid_info->num_ekw;
         /* Extract data portion */
         for (part = 0; part < width; part++) {
             slice_col = 0;
             if (ltid_info->pt_type != LT_PT_TYPE_FP ) {
                  SHR_IF_ERR_VERBOSE_EXIT
                      (bcmptm_rm_tcam_tindex_to_sindex(unit,
                                                       iomd,
                                                       rm_flags,
                                                       iomd->target_index,
                                                       &sindex,
                                                       &slice_row,
                                                       &slice_col));
              } else {
                  SHR_IF_ERR_VERBOSE_EXIT
                      (bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                          iomd,
                                                          rm_flags,
                                                          iomd->target_index,
                                                          part,
                                                          &slice_row,
                                                          &sindex));
              }
              slice_col += part;
              sid_array = ltid_info->hw_entry_info->sid_data_only;
              num_words = bcmdrd_pt_entry_wsize(unit,
                                    sid_array[slice_row][slice_col]);

              data_length =
                    (ltid_info->hw_entry_info->data_end_bit[part] -
                     ltid_info->hw_entry_info->data_start_bit + 1);

              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmptm_rm_tcam_data_get(unit,
                             iomd->req_info->ekw[part],
                             ltid_info->hw_entry_info->data_start_bit,
                             data_length,
                             entry_words_row[unit][part]));
         }
    } else {
        width = iomd->ltid_info->num_edw;
        /* Copy data portion from request */
        for (part = 0; part < width; part++) {
            slice_col = 0;
            if (ltid_info->pt_type != LT_PT_TYPE_FP ) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_tindex_to_sindex(unit,
                                                      iomd,
                                                      rm_flags,
                                                      iomd->target_index,
                                                      &sindex,
                                                      &slice_row,
                                                      &slice_col));
             } else {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                         iomd,
                                                         rm_flags,
                                                         iomd->target_index,
                                                         part,
                                                         &slice_row,
                                                         &sindex));
             }
             slice_col += part;

             sid_array = iomd->ltid_info->hw_entry_info->sid_data_only;
             num_words = bcmdrd_pt_entry_wsize(unit,
                                   sid_array[slice_row][slice_col]);

             sal_memcpy(entry_words_row[unit][part],
                        iomd->req_info->edw[part],
                        (sizeof(uint32_t) * num_words));
        }
    }

    /*  Write the associated data to index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_hw_write(unit,
                                 iomd,
                                 rm_flags,
                                 iomd->target_index,
                                 entry_words_row[unit]));

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                 iomd,
                                                 iomd->target_index));
    } else {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
    }
    iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;

exit:
    SHR_FUNC_EXIT();
}

/*
 * This is the main routine called in case of an existing entry
 * in the TCAM to do an update of its related information.
 * Update could be for key portion, data portion or priority
 */
int
bcmptm_rm_tcam_prio_entry_update(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t update_type = 0;
    int rv = SHR_E_NONE;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info;

    SHR_FUNC_ENTER(unit);

    ltid_info = iomd->ltid_info;

    /* Identify if its a priority change */
    if (iomd->req_info->entry_pri != iomd->rsp_info->rsp_entry_pri) {
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_entry_prio_update(unit, iomd));
        } else {
            SHR_ERR_EXIT
                (bcmptm_rm_tcam_prio_eid_entry_prio_update(unit, iomd));
        }
    }

    /* Identify if its a key or data change */
    rv = (bcmptm_rm_tcam_update_type_get(unit, &update_type, iomd));
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if ((ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY)
        && (!(update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE))) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = iomd->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    /* Data only update only when data_only SID is available */
    if ((update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
        bcmdrd_feature_is_real_hw(unit) &&
        (ltid_info->hw_entry_info[mode].sid_data_only != NULL) &&
        !(ltid_info->hw_entry_info[mode].non_aggr)) {
        SHR_ERR_EXIT
            (bcmptm_rm_tcam_entry_update_data_only(unit, iomd));
    } else {
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_entry_update(unit,
                                                       update_type,
                                                       iomd));
        } else {
            SHR_ERR_EXIT
                (bcmptm_rm_tcam_prio_eid_entry_update(unit, iomd));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

