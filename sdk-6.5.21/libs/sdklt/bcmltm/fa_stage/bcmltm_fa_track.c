/*! \file bcmltm_fa_track.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Tracking Index calculations
 *
 * This file contains the LTM FA stage functions for Logical Table
 * Tracking Index calculations.
 * The Tracking Index is a combination of the device parameters
 * required to identify a unique device resource corresponding to
 * a unique set of LT API Key fields.
 *
 * The Key fields are copied or Transformed into the device physical
 * parameters.  These may include memory index, register array index,
 * register port, pipeline number, PT select, and overlay select.
 * However many there may be, the resulting Tracking Index should
 * fit within a single 32-bit value.
 *
 * The Tracking Index is the offset into the in-use, the
 * global in-use, and the valid entry bitmaps.
 *
 * The format of the Tracking Index is zero or more fields
 * appended together to form a single 32-bit value.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief An FA tree node to determine Tracking index for w/Allocation LTs.
 *
 * An FA tree node which calculates the Tracking index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.  One set of Working Buffer physical field values are
 * updated in such a case.
 * This function is only applicable for Index w/Allocation
 * Logical Tables.
 *
 * This core implementation handles either local and global inuse bitmaps.
 *
 * \param [in] unit Unit number.
 * \param [in] global If TRUE, this is a shared LT with a global in-use bitmap.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_alloc_track_core(int unit,
                                bool global,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t next_bit;
    SHR_BITDCL *inuse_bitmap;
    SHR_BITDCL *valid_bitmap;
    bcmltm_track_index_t *track_info;
    uint32_t tracking_index, track_max, track_mask;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    /* Alloc means INSERT, so check for max entries. */
    if (lt_state->entry_count >= lt_state->max_entry_count) {
        /* INSERT must fail, so stop processing */
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    tracking_index = ltm_buffer[track_info->track_index_offset];

    if (ltm_buffer[track_info->index_absent_offset] == BCMLTM_INDEX_ABSENT) {
        /* Determine available index */

        /* Preserve the non-allocatable portion of the Tracking Index */
        track_mask = track_info->track_alloc_mask;
        track_max = (tracking_index & ~track_mask) | track_mask;

        /* Retrieve the necessary inuse bitmap */
        if (global) {
            global_ltid = lt_md->params->global_inuse_ltid;

            /* Global LT state */
            SHR_IF_ERR_EXIT
                (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

            SHR_IF_ERR_EXIT
                (bcmltm_state_data_get(unit, global_lt_state,
                                       BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                                       (void *) &inuse_bitmap));
        } else {
            SHR_IF_ERR_EXIT
                (bcmltm_state_data_get(unit, lt_state,
                                       BCMLTM_STATE_TYPE_INUSE_BITMAP,
                                       (void *) &inuse_bitmap));
        }

        /* Retrieve the valid bitmap for this LT */
        SHR_IF_ERR_EXIT
            (bcmltm_state_data_get(unit, lt_state,
                                   BCMLTM_STATE_TYPE_VALID_BITMAP,
                                   (void *) &valid_bitmap));

        
        for (next_bit = tracking_index;
             next_bit <= track_max;
             next_bit++) {
            /* Check valid bitmap for legal entry */
            if (!SHR_BITGET(valid_bitmap, next_bit)) {
                continue;
            }
            /* Check in-use bitmap for next free entry */
            if (!SHR_BITGET(inuse_bitmap, next_bit)) {
                break;
            }
        }

        /* No free entry found */
        if (next_bit > track_max) {
            /*
             * Checked for max entries above.  There must be
             * entries in shared LTs.
             */
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

        tracking_index = next_bit;

        /* Update the WB Tracking Index location with the search result */
        ltm_buffer[track_info->track_index_offset] = tracking_index;
    } else {
        track_max = track_info->track_index_max;

        if (tracking_index > track_max) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_wbc_to_track(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;
    bool allocating = FALSE;

    SHR_FUNC_ENTER(unit);

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    field_num = track_info->track_index_field_num;
    tracking_index = 0;

    if ((track_info->index_absent_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[track_info->index_absent_offset] ==
         BCMLTM_INDEX_ABSENT)) {
        /* Allocation required, skip missing key fields. */
        allocating = TRUE;
    }

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        if (allocating && cur_field->allocatable) {
            /* This field is missing, skip processing */
            continue;
        }
        field_val = ltm_buffer[cur_field->field_offset];
        if (field_val > cur_field->field_mask) {
            /* This error should have been caught by field range checks. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        tracking_index |= (field_val << cur_field->field_shift);
    }

    ltm_buffer[track_info->track_index_offset] = tracking_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_track_to_wbc(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    field_num = track_info->track_index_field_num;
    tracking_index = ltm_buffer[track_info->track_index_offset];

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        field_val = (tracking_index >> cur_field->field_shift) &
            cur_field->field_mask;
        ltm_buffer[cur_field->field_offset] = field_val;
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_alloc_track_to_wbc(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);
    if (ltm_buffer[track_info->index_absent_offset] != BCMLTM_INDEX_ABSENT) {
        /* Nothing to do */
        return SHR_E_NONE;
    }

    field_num = track_info->track_index_field_num;
    tracking_index = ltm_buffer[track_info->track_index_offset];

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        if (!cur_field->allocatable) {
            /* Do not overwrite non-allocatable device parameters */
            continue;
        }
        field_val = (tracking_index >> cur_field->field_shift) &
            cur_field->field_mask;
        ltm_buffer[cur_field->field_offset] = field_val;
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_traverse_track(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    uint32_t next_bit;
    SHR_BITDCL *inuse_bitmap;
    bcmltm_track_index_t *track_info;
    uint32_t tracking_index, track_max;

    SHR_FUNC_ENTER(unit);

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    tracking_index = ltm_buffer[track_info->track_index_offset];
    track_max = track_info->track_index_max;

    /*
     * If this is the first TRAVERSE op, then the current minimum values
     * of the API Key fields should be placed in the physical parameters
     * so bcmltm_fa_node_wbc_to_track can produce the minimum
     * Tracking Index for use here.
     * Do not increment in that case.
     */
    if (lt_entry->in_fields != NULL) {
        /* Not the first TRAVERSE, so increment tracking_index */
        tracking_index++;
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    
    for (next_bit = tracking_index;
         next_bit <= track_max;
         next_bit++) {
        /* Search in-use bitmap for next entry */
        if (SHR_BITGET(inuse_bitmap, next_bit)) {
            break;
        }
    }

    if (next_bit > track_max) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update the WB Tracking Index location with the search result */
    ltm_buffer[track_info->track_index_offset] = next_bit;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_alloc_track(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie)
{
    return bcmltm_fa_node_alloc_track_core(unit, FALSE, lt_md, lt_state,
                                           ltm_buffer, node_cookie);
}

int
bcmltm_fa_node_global_alloc_track(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    return bcmltm_fa_node_alloc_track_core(unit, TRUE, lt_md, lt_state,
                                           ltm_buffer, node_cookie);
}
