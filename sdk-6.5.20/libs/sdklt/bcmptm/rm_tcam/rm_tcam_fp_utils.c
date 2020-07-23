/*! \file rm_tcam_fp_utils.c
 *
 * Util functions for FP based TCAMs
 * This file contains low level utility functions for FP based TCAMs
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
#include <bcmdrd/bcmdrd_types.h>
#include "rm_tcam.h"
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

#define BCMPTM_RM_TCAM_MAX_DEPTH_PARTS 12


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_prio_fp_entry_info_get(int unit, bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 void *attrs,
                                 bcmptm_rm_tcam_prio_eid_entry_info_t **entry_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attrs->pipe_id,
                                          entry_attrs->stage_id,
                                          &stage_ptr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attrs->pipe_id,
                                          entry_attrs->group_id,
                                          stage_ptr,
                                          &group_ptr));

    *entry_info = NULL;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        offset = group_ptr->presel_entry_info_seg;
    } else {
        offset = group_ptr->entry_info_seg;
    }

    *entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)
                   ((uint8_t *)stage_ptr + offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_hash_get(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    /* Fetch the TCAM information for the given ltid. */
    stage_ptr = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;
    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    iomd->entry_hash_ptr = NULL;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        offset = group_ptr->presel_entry_hash_seg;
    } else {
        offset = group_ptr->entry_hash_seg;
    }

    iomd->entry_hash_ptr= (int*)((uint8_t *)stage_ptr + offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_flags_get(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    iomd->tcam_flags = group_ptr->flags;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        hw_entry_info =
            &(iomd->ltid_info->hw_entry_info[group_ptr->group_mode]);
        if (hw_entry_info->non_aggr == 1) {
            iomd->tcam_flags |= BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
            iomd->tcam_flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            iomd->tcam_flags &= ~BCMPTM_RM_TCAM_F_AGGR_VIEW;
        } else {
            iomd->tcam_flags &= ~BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
            iomd->tcam_flags &= ~BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            iomd->tcam_flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
        }
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_get(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;

    slice_id = group_ptr->primary_slice_id;

    /* Initialize the free count value to 0 */
    iomd->free_entries = 0;
    /* presel entry */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        iomd->free_entries = slice_ptr[slice_id].free_presel_entries;
    } else {
        /* aggregate the free count for all depth slices corrsponding to
         * to the given group.
         */
        do {
            /* Add the free entries count in current slice */
            iomd->free_entries += slice_ptr[slice_id].free_entries;

            slice_id_prev = slice_id;
            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_ptr[slice_id].next_slice_id_offset[entry_attrs->group_mode];

        } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));
        iomd->last_slice_num_entries = slice_ptr[slice_id].num_entries;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_valid_count_get(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    uint32_t num_presels_per_slice = 0;
    bcmptm_rm_tcam_fp_group_t *shared_group_ptr = NULL;
    int16_t shared_group_id = -1;
    int16_t pri_group_id = -1;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
    uint8_t slice_id;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;
    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    if (group_ptr == NULL) {
        /* Group is not created yet.*/
        SHR_EXIT();
    }
    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;

    slice_id = group_ptr->primary_slice_id;

    pri_group_id = slice_ptr[slice_id].primary_grp_id;

    iomd->num_entries_ltid = 0;
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_presel_entries_per_slice_get(
                                              unit, iomd->ltid,
                                              iomd->ltid_info, entry_attrs->pipe_id,
                                              0,
                                              &num_presels_per_slice));
        iomd->num_entries = num_presels_per_slice;
        iomd->num_entries_ltid = group_ptr->num_presel_entries;

        shared_group_id = pri_group_id;
        while (shared_group_id != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  entry_attrs->pipe_id,
                                                  shared_group_id,
                                                  iomd->stage_fc,
                                                  &shared_group_ptr));
            if (group_ptr->group_mode != shared_group_ptr->group_mode) {
                iomd->multi_mode = TRUE;
                break;
            }
            shared_group_id = shared_group_ptr->next_shared_group_id;
        }
    } else {
        iomd->num_entries = group_ptr->num_entries;
        shared_group_id = pri_group_id;
        while (shared_group_id != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  entry_attrs->pipe_id,
                                                  shared_group_id,
                                                  iomd->stage_fc,
                                                  &shared_group_ptr));

            if (group_ptr->group_mode == shared_group_ptr->group_mode) {
                iomd->num_entries_ltid += shared_group_ptr->num_entries_ltid;
            } else {
                iomd->multi_mode = TRUE;
            }
            shared_group_id = shared_group_ptr->next_shared_group_id;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_hash_size_get(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        iomd->entry_hash_size= group_ptr->presel_entry_hash_size;
    } else {
        iomd->entry_hash_size = group_ptr->entry_hash_size;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_incr(int unit,
                                         uint32_t ent_logical_index,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;
    uint32_t num_entries_per_slice;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    /* Initialize field stage pointer. */
    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;

    slice_id = group_ptr->primary_slice_id;

    /* increment the entries count in current slice */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        slice_ptr[slice_id].free_presel_entries += 1;
        group_ptr->num_presel_entries -= 1;
    } else {
        group_ptr->num_entries_ltid -= 1;
        iomd->free_entries += 1;

        /* get the number of entries in that slice */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                       unit, iomd->ltid, iomd->ltid_info,
                                       slice_id, entry_attrs->pipe_id,
                                       entry_attrs->group_mode,
                                       &num_entries_per_slice));

        /* Traverse till the right depth */
        while (ent_logical_index >= num_entries_per_slice) {
            ent_logical_index = ent_logical_index - num_entries_per_slice;
            if (slice_ptr[slice_id].next_slice_id_offset[entry_attrs->group_mode] == 0) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            slice_id = slice_id + slice_ptr[slice_id].next_slice_id_offset[entry_attrs->group_mode];
            /* get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                           unit, iomd->ltid, iomd->ltid_info,
                                           slice_id, entry_attrs->pipe_id,
                                           entry_attrs->group_mode,
                                           &num_entries_per_slice));
        }
        /* update the free count for all slices corrsponding to
         * to the given group.
         */
        do {
            slice_ptr[slice_id].free_entries += 1;
            /* got he next slice if exists */
            slice_id_prev = slice_id;
            slice_id = slice_id +
                            slice_ptr[slice_id].right_slice_id_offset[entry_attrs->group_mode];

        } while (slice_id < num_slices
               && (slice_id != slice_id_prev));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_decr(int unit,
                                         uint32_t ent_logical_index,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;
    uint32_t num_entries_per_slice;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;


    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;
    slice_id = group_ptr->primary_slice_id;

    /* presel entry */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        slice_ptr[slice_id].free_presel_entries -= 1;
        group_ptr->num_presel_entries += 1;
    } else {
        group_ptr->num_entries_ltid += 1;
        iomd->free_entries -= 1;
        /* get the number of entries in that slice */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                   unit, iomd->ltid, iomd->ltid_info,
                                   slice_id, entry_attrs->pipe_id,
                                   entry_attrs->group_mode,
                                   &num_entries_per_slice));

        /* Traverse till the right depth */
        while (ent_logical_index >= num_entries_per_slice) {
            ent_logical_index = ent_logical_index - num_entries_per_slice;
            if (slice_ptr[slice_id].next_slice_id_offset[entry_attrs->group_mode] == 0) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            slice_id = slice_id + slice_ptr[slice_id].next_slice_id_offset[entry_attrs->group_mode];
            /* get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                       unit, iomd->ltid, iomd->ltid_info,
                                       slice_id, entry_attrs->pipe_id,
                                       entry_attrs->group_mode,
                                       &num_entries_per_slice));
        }

        /* update the free count for all slices corrsponding to
         * to the given group.
         */
        do {

            /* decrement the entries count in current slice */
            slice_ptr[slice_id].free_entries -= 1;
            slice_id_prev = slice_id;
            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_ptr[slice_id].right_slice_id_offset[entry_attrs->group_mode];

        } while (slice_id < num_slices
           && (slice_id != slice_id_prev));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmptm_rm_tcam_fp_entry_tcam_width_parts_count
 *
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    group_mode  - (IN)  Entry group mode
 *    group_mode  - (IN)  Stage flags
 *    part_count  - (OUT) Entry parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(int unit,
                                    uint32_t group_mode,
                                    uint32_t stage_flags,
                                    int *part_count)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_HALF ||
            group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
        *part_count = 1;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA) {
        *part_count = 2;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
        *part_count = 2;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_TRIPLE) {
        *part_count = 3;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) {
        *part_count = 4;
    } else {
        *part_count = 0;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count
 *
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    iomd        - (IN)  Input and Output metadata.
 *    part_count  - (OUT) Entry depth parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd,
                                         int *part_count)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t slice_id = 0;
    uint8_t num_slices = 0;
    bcmptm_rm_tcam_group_mode_t gmode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    rv = bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->entry_attrs->pipe_id,
                                          iomd->entry_attrs->group_id,
                                          iomd->stage_fc,
                                          &fg);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    slice_fc = iomd->slice_fc;
    *part_count = 0;
    slice_id = fg->primary_slice_id;
    gmode = iomd->entry_attrs->group_mode;
    do {
        /* Increment the depth count */
        (*part_count)++;
        /* Go to the next slice if exists */
        slice_id = slice_id +
                    slice_fc[slice_id].next_slice_id_offset[gmode];

    } while (slice_id < num_slices &&
            (0 != slice_fc[slice_id].next_slice_id_offset[gmode]));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_pipe_count_get(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t pipe_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    pipe_count = iomd->ltid_info->rm_more_info[0].pipe_count;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_pipe_config_get(unit,
                                        iomd->ltid,
                                        iomd->ltid_info,
                                        pipe_count,
                                        &(iomd->pipe_count)));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_total_count_get(int unit, bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *attrs,
                                         uint32_t *total_count)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(total_count, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t*) attrs;
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          &stage_fc));
    *total_count = (stage_fc->max_entries_per_slice
                       * (stage_fc->num_slices));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_iomd_entry_info_arr_update(int unit,
                        bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info_ptr,
                        bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    uint8_t slice_id;          /* Slice ID */
    uint8_t slice_id_prev;     /* Slice ID */
    uint8_t num_slices;        /* Number of Slices */
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    uint16_t num_entries = 0;
    uint16_t free_entries = 0;
    uint32_t num_entries_ltid = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    uint8_t depth_idx = 0;
    int width_parts = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info_iter = NULL;
    uint8_t i = 0;
    uint8_t  intraslice = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(entry_info_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *) iomd->entry_attrs;

    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;
    fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;
    /* Initialize field slice pointer. */

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT
          (bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       entry_attr->group_mode,
                                       stage_fc->flags,
                                       &width_parts));

    if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
          || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        width_parts  = width_parts >> intraslice;
    }

    iomd->entry_info_arr.num_width_parts = width_parts;
    iomd->num_parts = width_parts;
    iomd->entry_type = width_parts;

    slice_id = fg->primary_slice_id;

    /* Initialize field primary group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          entry_attr->pipe_id,
                                          slice_fc[slice_id].primary_grp_id,
                                          stage_fc,
                                          &pri_fg));
    if (entry_attr->group_mode != pri_fg->group_mode) {
        /* Get number of entry parts. */
        SHR_IF_ERR_VERBOSE_EXIT
          (bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       pri_fg->group_mode,
                                       stage_fc->flags,
                                       &width_parts));

        if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
            (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
            intraslice = 1;
            width_parts  = width_parts >> intraslice;
        }
    }
    iomd->entry_info_arr.pri_group_width_parts = width_parts;

    if (entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
        width_parts = iomd->num_parts;
    }

    depth_idx = 0;
    entry_info_iter = entry_info_ptr;
     do {
        num_entries = slice_fc[slice_id].num_entries;
        free_entries = slice_fc[slice_id].free_entries;
        num_entries_ltid = num_entries_ltid + (num_entries - free_entries);
        iomd->entry_info_arr.entry_info_depth[depth_idx] = num_entries;
        iomd->entry_info_arr.entry_info_free[depth_idx] = slice_fc[slice_id].free_entries;

        for (i = 0; i < width_parts; i++ ) {
            entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)(((uint8_t *)(entry_info_iter))
                               + ((i * stage_fc->max_entries_per_slice)
                               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));
            iomd->entry_info_arr.entry_info[depth_idx][i] = (void *) entry_info;
        }
        entry_info_iter = (bcmptm_rm_tcam_prio_eid_entry_info_t*)(((uint8_t *)(entry_info_iter))
                               + ((i * stage_fc->max_entries_per_slice)
                               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));
        slice_id_prev = slice_id;
        slice_id = slice_id +
                    slice_fc[slice_id].next_slice_id_offset[entry_attr->group_mode];
        depth_idx++;
    } while (slice_id < num_slices
                     && (slice_id_prev != slice_id));
    iomd->entry_info_arr.num_depth_parts = depth_idx;
    iomd->num_entries_ltid = num_entries_ltid;

exit:
    SHR_FUNC_EXIT();
}
