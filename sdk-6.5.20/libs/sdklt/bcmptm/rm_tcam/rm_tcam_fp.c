/*! \file rm_tcam_fp.c
 *
 * Low Level Functions for FP based TCAMs
 * This file contains low level functions for FP based TCAMs
 * Initialization for the FP sub module
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
#include "bcmptm/bcmptm_ser_misc_internal.h"
#include "rm_tcam.h"
#include "rm_tcam_prio_eid.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "rm_tcam_fp_utils.h"
#include "rm_tcam_fp_pdd.h"
#include "rm_tcam_fp_sbr.h"
#include "rm_tcam_fp_psg.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_rm_tcam_fp_group_init(int unit,
                             uint8_t pipe,
                             bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    start_ptr = (uint8_t *)(stage_ptr);

    group_ptr = (bcmptm_rm_tcam_fp_group_t*) (start_ptr
                 + stage_ptr->group_info_seg);

    for (idx = 0;idx < stage_ptr->num_groups; idx++) {
        sal_memset(group_ptr, 0, sizeof(bcmptm_rm_tcam_fp_group_t));
        group_ptr->next_shared_group_id = -1;
        group_ptr++;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_slice_init(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint8_t pipe_id,
                             bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_slice_t *slice_ptr = NULL; /* Slice info. */
    uint8_t slice = 0;                    /* Slice iterator. */
    int mem_sz;            /* Memory allocation buffer size. */
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint8_t num_slices = 0;
    int *slice_depth = NULL;
    bcmdrd_sid_t sid_list[2];
    uint32_t slice_group_id;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    uint8_t mode = 0;
    uint32_t slice_min_count = 0;
    uint32_t slice_max_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    /* Initialize stage instance slices pointer. */
    start_ptr = (uint8_t *)(stage_ptr);

    slice_ptr = (bcmptm_rm_tcam_slice_t*) (start_ptr
                 + stage_ptr->slice_seg);

    /* Initialze stage slice data structure. */
    mem_sz = stage_ptr->num_slices * sizeof(bcmptm_rm_tcam_slice_t);
    sal_memset(slice_ptr, 0, mem_sz);

    /* Fetch the number of slices per pipe in this stage */
    num_slices = (stage_ptr->num_slices);

    for (slice = 0; slice < num_slices; slice++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(unit,
                                               ltid,
                                               ltid_info,
                                               slice,
                                               pipe_id,
                                               ref_grp_mode,
                                               &num_entries_per_slice));
        /* Find smaller and large slice sizes. */
        if ((slice_min_count == 0) ||
            (num_entries_per_slice < slice_min_count)) {
            slice_min_count = num_entries_per_slice;
        }
        if (num_entries_per_slice > slice_max_count) {
            slice_max_count = num_entries_per_slice;
        }
    }

    /* Initialize stage slices info. */
    for (slice = 0; slice < num_slices; slice++) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                    unit, ltid, ltid_info,
                                    slice, pipe_id, ref_grp_mode,
                                    &num_entries_per_slice));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_presel_entries_per_slice_get(
                                       unit, ltid, ltid_info, pipe_id,
                                       slice,
                                       &num_presels_per_slice));

        slice_ptr->slice_id = slice;
        slice_ptr->num_entries = num_entries_per_slice;
        slice_ptr->num_presel_entries = num_presels_per_slice;
        if (ltid_info->rm_more_info->slice_info[slice].slice_flags
                                    & BCMPTM_SLICE_FLAG_DOUBLE_CAPABLE) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_MODE_DOUBLE_CAPABLE;
        }
        if (ltid_info->rm_more_info->slice_info[slice].slice_flags
                                    & BCMPTM_SLICE_FLAG_TRIPLE_CAPABLE) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_MODE_TRIPLE_CAPABLE;
        }
        for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
                mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
                    mode++) {
            slice_ptr->next_slice_id_offset[mode] = 0;
            slice_ptr->prev_slice_id_offset[mode] = 0;
            slice_ptr->left_slice_id_offset[mode] = 0;
            slice_ptr->right_slice_id_offset[mode] = 0;
        }
        slice_ptr->primary_grp_id = -1;

        /* Enaable Intra Slice & auto expansion based on stage flags. */
        if (stage_ptr->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_INTRASLICE_CAPABLE;
        }
        if (stage_ptr->flags & BCMPTM_STAGE_AUTOEXPANSION) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION;
        }
        /* Set slice type. */
        if (num_entries_per_slice == slice_min_count) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_TYPE_SMALL;
        }
        if (num_entries_per_slice == slice_max_count) {
            slice_ptr->slice_flags |= BCMPTM_SLICE_TYPE_LARGE;
        }
        slice_ptr = slice_ptr + 1;
    }

    if ((stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) &&
        ltid_info->rm_more_info[0].flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) {
        slice_ptr = (bcmptm_rm_tcam_slice_t*) (start_ptr
                                 + stage_ptr->slice_seg);
        for (slice = 0; slice < num_slices - 1; slice++) {
            for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
                 mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
                 mode++) {
                slice_ptr[slice].next_slice_id_offset[mode] = 1;
            }
        }
    }

    if (stage_ptr->flags & BCMPTM_STAGE_MULTI_VIEW_TCAM) {
        SHR_ALLOC(slice_depth,
                  (num_slices * sizeof(int)),
                  "bcmptmRmtcamSliceDepth");
        if (slice_depth == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        slice_ptr = (bcmptm_rm_tcam_slice_t*) (start_ptr
                     + stage_ptr->slice_seg);

        for (slice = 0; slice < num_slices; slice++) {
            slice_depth[slice] = slice_ptr->num_entries;
            slice_ptr = slice_ptr + 1;
        }

        /* 5/0 Half/Single wide mode. */
        sid_list[0] = ltid_info->hw_entry_info[5].sid[0][0];
        sid_list[1] = ltid_info->hw_entry_info[0].sid[0][0];

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_ser_slice_group_register(unit, slice_depth, num_slices,
                                            sid_list, 2,
                                            BCMPTM_TCAM_SLICE_DISABLED, 0,
                                            &slice_group_id));
        stage_ptr->slice_group_id = slice_group_id;
    }

exit:
    SHR_FREE(slice_depth);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_lt_config_init(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t pipe_id,
                                 bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    bcmptm_rm_tcam_lt_config_t *lt_config_ptr = NULL; /* Lt config info. */
    uint32_t num_groups_per_pipe = 0;
    int mem_sz;                        /* Memory allocation buffer size. */
    uint32_t lt_idx;                  /* Logical Table iterator.         */
    int pipe = 0;


    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);


    lt_config_ptr = (bcmptm_rm_tcam_lt_config_t*) (((uint8_t *)stage_ptr)
                     + stage_ptr->lt_config_seg);

    /* num_pipes * num groups per pipe * size of structure */
    mem_sz = stage_ptr->num_pipes *
             stage_ptr->num_groups * sizeof(bcmptm_rm_tcam_lt_config_t);

    sal_memset(lt_config_ptr, 0, mem_sz);

    /* Initialize for all pipes and all logical tables. */
    for (pipe = 0; pipe < stage_ptr->num_pipes; pipe++) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info, pipe, &num_groups_per_pipe));
        for (lt_idx = 0; lt_idx < num_groups_per_pipe; lt_idx++) {
            /* Initialize Logical Table ID. */
           lt_config_ptr->lt_id = lt_idx;
           if (stage_ptr->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
               lt_config_ptr->lt_id = lt_idx + (num_groups_per_pipe * pipe);
           }
           lt_config_ptr = lt_config_ptr + 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_single_slice
 * Purpose:
 *     Validate and return  slice count if the group can
 *     expand to Single or Intra Slice Double wide mode group.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     group mode    - (IN) group mode
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_validate_single_slice(int unit,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          uint32_t group_mode,
                                          uint8_t num_slices,
                                          bcmptm_rm_tcam_slice_t *slice_fc,
                                          uint8_t *slice_count,
                                          uint32_t *max_entries)
{
    uint8_t slice_idx;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    *slice_count = 0;
    *max_entries = 0;
    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < num_slices; slice_idx++) {

        if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA) &&
            (ltid_info->rm_more_info[0].slice_info[slice_idx].isdw_no_support
            == 1)) {
            slice_fc = slice_fc + 1;
            continue;
        }

        /* check the in USE flag of slice */
        if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
            (*slice_count)++;
            *max_entries += slice_fc->num_entries;
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}
/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_double_slice
 * Purpose:
 *     Validate and return slice count if it can be
 *     expanded to double wide mode group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     group mode    - (IN) group mode
 *     stage_fc      - (IN) Field stage structure.
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_validate_double_slice(int unit,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint32_t group_mode,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice_count,
                               uint32_t *max_entries)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    uint8_t slice_bundle_size;
    uint8_t slice_ratio = 1;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;
    *slice_count = 0;
    *max_entries = 0;
    if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) ||
        ((stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE))) {
        slice_ratio = 2;
    }
    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < (num_slices - 1); slice_idx++) {
        /*
         * Double wide group is created using consecutive slices within a memory
         * macro. But consecutive slices across a memory macro boundary cannot be
         * paried to create a double wide group. Skip slices 2, 5, 8 and 11 from
         * allocating for Double Wide Group base slice.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 2) {
            /* For legacy Arch slice should be even */
            if (!(stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL)
                    && (slice_idx % 2)) {
                slice_fc = slice_fc + 1;
                continue;
            }
            if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) &&
                (ltid_info->rm_more_info[0].slice_info[slice_idx].isdw_no_support
                    == 1)) {
                slice_fc = slice_fc + 1;
                continue;
            }
            /* check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL
                    && (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    (*slice_count)++;
                    *max_entries += (slice_fc->num_entries / slice_ratio);
                    slice_fc = slice_fc + 1;
                    slice_idx = slice_idx + 1;
                }
            }
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_triple_slice
 * Purpose:
 *     Validate and return slice count if it can be
 *     expanded to triple wide mode group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     stage_fc      - (IN) Field stage structure.
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN) Field Slice structure. Update flags
 *     slice_count   - (OUT) slice count.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_validate_triple_slice(int unit,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice_count,
                               uint32_t *max_entries)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bcmptm_rm_tcam_slice_t *slice_next_next = NULL;
    uint8_t slice_bundle_size;
    uint8_t slice_ratio = 1;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;
    *slice_count = 0;
    *max_entries = 0;
    if (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
        slice_ratio = 2;
    }
    /* Loop through the slice segment and return the slice
     * number to use
     */
    /* As we need 3 contiguous slices, we can stop the loop at
     * Max slices - 2 as after this there can never exist 3
     * contiguous slices
     */
    for (slice_idx = 0; slice_idx < (num_slices - 2); slice_idx++) {
        /*
         * Triple wide group is created using consecutive slices within a memory
         * macro. But consecutive slices across a memory macro boundary cannot be
         * paried to create a Triple wide group.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 3) {
            /* check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL
                    && (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    /* check whether the next slice is free */
                    slice_next_next = slice_next + 1;
                    if (slice_next_next != NULL
                        && (!(slice_next_next->slice_flags
                        & BCMPTM_SLICE_IN_USE))) {
                        (*slice_count)++;
                        *max_entries += (slice_fc->num_entries / slice_ratio);
                        slice_fc = slice_fc + 2;
                        slice_idx = slice_idx + 2;
                    }
                }
            }
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief calculate the number of slices the given group can auto expand
 *
 * \param [in] unit BCM device number.
 * \param [in] ltid Logical Table Enum
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] entry_attrs entry attrs structure
 * \param [out] slice_count number of slices
 * \param [out] max_entries max number of entries
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
static int
bcmptm_rm_tcam_fp_validate_group_auto_expansion(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   bcmptm_rm_tcam_entry_attrs_t *entry_attrs,
                                   uint8_t *slice_count,
                                   uint32_t *max_entries)
{
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attrs->pipe_id,
                                   entry_attrs->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attrs->pipe_id,
                                   entry_attrs->stage_id,
                                   &slice_fc));
        /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attrs->pipe_id,
                                          entry_attrs->group_id,
                                          stage_fc,
                                          &fg));

    if (BCMPTM_RM_TCAM_GRP_MODE_HALF == fg->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_SINGLE == fg->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode) {
        /* count the number of single slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_single_slice(unit,
                                          ltid_info, fg->group_mode, num_slices,
                                          slice_fc, slice_count, max_entries));
        if (bcmptm_rm_tcam_fp_half_mode_supported(unit, ltid_info) == TRUE &&
            BCMPTM_RM_TCAM_GRP_MODE_HALF != fg->group_mode) {
            *max_entries = (*max_entries) / 2 ;
        }

        if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode) {
            *max_entries = (*max_entries) / 2 ;
        }
    } else  if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == fg->group_mode
        || (BCMPTM_RM_TCAM_GRP_MODE_QUAD == fg->group_mode
        && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        /* count the number of double slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_double_slice(unit,
                                          ltid_info,fg->group_mode,
                                          stage_fc, num_slices,
                                          slice_fc, slice_count, max_entries));

    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == fg->group_mode
            && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* count the number of triple slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_triple_slice(unit,
                                          ltid_info,
                                          stage_fc, num_slices,
                                          slice_fc, slice_count, max_entries));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }

 exit:
    SHR_FUNC_EXIT();

}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_calc_mem_required(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    void *mem_size)
{
    int size = 0;
    uint8_t num_slices = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint32_t max_num_entries_per_slice = 0;
    uint32_t max_num_presels_per_slice = 0;
    uint8_t num_pipes = 0;
    uint32_t *total_size = mem_size;
    uint8_t pipe = 0;
    uint8_t slice_id = 0;
    uint64_t stage_flags = 0;
    uint8_t presel_group_count = 0;
    uint16_t sbr_profiles_count = 0;
    uint16_t pdd_profiles_count = 0;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                          ltid, ltid_info, &num_pipes));

    /* This should always be the first structure
     * memory allocation for every LTID
     */
    /* memory required to save Transaction related info */
    size += sizeof(bcmptm_rm_tcam_trans_info_t);

    size += sizeof(bcmptm_rm_tcam_fp_stage_t);
    size += (sizeof(bcmptm_fp_entry_seg_bmp_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_stage_flags_get(unit,
                        ltid, ltid_info, &stage_flags));

    /* Tile information */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_count_get(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               &presel_group_count));
    size += (presel_group_count * sizeof(uint32_t));

    /* SBR Profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_sbr_profiles_count_get(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               &sbr_profiles_count));
    size += (sbr_profiles_count * sizeof(uint32_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(unit,
                                      ltid, ltid_info, pipe,
                                      &num_groups_per_pipe));
    size += (num_groups_per_pipe *
             sizeof(bcmptm_rm_tcam_fp_group_t));

    /* LT config info is common for both EM and Ingress stages */
    if (!(stage_flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        size += (num_groups_per_pipe * num_pipes *
                 sizeof(bcmptm_rm_tcam_lt_config_t));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                    ltid, ltid_info, pipe, &num_slices));

    size += (num_slices * sizeof(bcmptm_rm_tcam_slice_t));

    max_num_entries_per_slice = 0;
    max_num_presels_per_slice = 0;

    for (slice_id = 0; slice_id < num_slices; slice_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                        unit, ltid, ltid_info,
                                        slice_id, pipe, ref_grp_mode,
                                        &num_entries_per_slice));

        if (max_num_entries_per_slice < num_entries_per_slice) {
            max_num_entries_per_slice = num_entries_per_slice;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                         ltid, ltid_info, pipe,
                                         slice_id,
                                         &num_presels_per_slice));

          if (max_num_presels_per_slice < num_presels_per_slice) {
              max_num_presels_per_slice = num_presels_per_slice;
          }
    }

    size += ((max_num_entries_per_slice * num_slices)
               * sizeof(int));
    size += ((max_num_entries_per_slice * num_slices)
            * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));

    size += (((max_num_presels_per_slice * num_slices)
               / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)
               * sizeof(int));
    size += ((max_num_presels_per_slice * num_slices)
            * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    /*
     * Get the PDD profiles per pipe to allocate memory
     * to save the reference counts.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_pdd_profiles_count_get(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               &pdd_profiles_count));
    size += (pdd_profiles_count * sizeof(uint32_t));

    *total_size += size;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_init_metadata_cb(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t pipe,
                                   void *ha_blk,
                                   uint8_t sub_component_id)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *presel_entry_info = NULL;
    int  *presel_entry_hash = NULL;
    uint8_t num_slices = 0;
    uint8_t total_num_slices = 0;
    uint32_t total_num_groups = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint32_t max_num_entries_per_slice = 0;
    uint32_t max_num_presels_per_slice = 0;
    uint32_t num_entries = 0;
    uint32_t num_presels = 0;
    uint8_t num_pipes = 0;
    uint8_t slice_bundle_size = 0;
    uint64_t stage_flags = 0;
    uint32_t idx = 0;
    uint8_t slice_id = 0;
    int rv = SHR_E_NONE;
    uint8_t num_presel_groups = 0;
    uint16_t num_sbr = 0;
    uint16_t num_pdd = 0;
    uint8_t total_presel_group_count = 0;
    uint16_t total_sbr_count = 0;
    uint16_t total_pdd_count = 0;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_fp_entry_seg_bmp_t *entry_seg_bmp;
    uint32_t offset = 0;
    uint32_t trans_info_size = 0;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ha_blk, SHR_E_PARAM);

    offset = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                                    BCMMGMT_RM_TCAM_COMP_ID,
                                    sub_component_id,
                                    offset,
                                    sizeof(bcmptm_rm_tcam_trans_info_t), 1,
                                    BCMPTM_RM_TCAM_TRANS_INFO_T_ID));
    start_ptr = (uint8_t *)ha_blk;
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + offset);
    trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;
    trans_info->tbl_inst = pipe;
    trans_info_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                                    BCMMGMT_RM_TCAM_COMP_ID,
                                    sub_component_id,
                                    offset,
                                    sizeof(bcmptm_rm_tcam_fp_stage_t), 1,
                                    BCMPTM_RM_TCAM_FP_STAGE_T_ID));

    stage_ptr = (bcmptm_rm_tcam_fp_stage_t *)(start_ptr + offset);
    sal_memset(stage_ptr, 0, sizeof(bcmptm_rm_tcam_fp_stage_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   &num_pipes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(unit,
                                                         ltid,
                                                         ltid_info,
                                                         &slice_bundle_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_stage_flags_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   &stage_flags));
    stage_ptr->num_pipes = num_pipes;
    stage_ptr->slice_bundle_size = slice_bundle_size;
    stage_ptr->flags = stage_flags;

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(unit,
                                                    ltid,
                                                    ltid_info,
                                                    pipe,
                                                    &num_groups_per_pipe));
     total_num_groups += num_groups_per_pipe;

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                     ltid,
                                                     ltid_info,
                                                     pipe,
                                                     &num_slices));
     total_num_slices += num_slices;

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_fp_presel_group_count_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   pipe,
                                                   &num_presel_groups));
     total_presel_group_count += num_presel_groups;

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_fp_sbr_profiles_count_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   pipe,
                                                   &num_sbr));
     total_sbr_count += num_sbr;

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_fp_pdd_profiles_count_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe,
                                                &num_pdd));
     total_pdd_count += num_pdd;
     for (slice_id = 0; slice_id < num_slices; slice_id++) {
          SHR_IF_ERR_VERBOSE_EXIT
              (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(unit,
                                                 ltid,
                                                 ltid_info,
                                                 slice_id,
                                                 0,
                                                 ref_grp_mode,
                                                 &num_entries_per_slice));

          if (max_num_entries_per_slice < num_entries_per_slice) {
              max_num_entries_per_slice = num_entries_per_slice;
          }

          SHR_IF_ERR_VERBOSE_EXIT
              (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                 ltid,
                                                 ltid_info,
                                                 0,
                                                 slice_id,
                                                 &num_presels_per_slice));

          if (max_num_presels_per_slice < num_presels_per_slice) {
              max_num_presels_per_slice = num_presels_per_slice;
          }
     }

    if (total_num_slices == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    stage_ptr->num_groups = total_num_groups;
    stage_ptr->num_slices = total_num_slices;
    stage_ptr->num_sbr_profiles = total_sbr_count;
    stage_ptr->num_pdd_profiles = total_pdd_count;
    stage_ptr->num_presel_groups = total_presel_group_count;

    num_entries = max_num_entries_per_slice * stage_ptr->num_slices;
    num_presels = max_num_presels_per_slice * stage_ptr->num_slices;

    stage_ptr->entry_hash_size = (num_entries);
    stage_ptr->presel_entry_hash_size =
               (num_presels / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);

    stage_ptr->max_entries_per_slice =
               (num_entries / stage_ptr->num_slices);
    stage_ptr->max_presel_entries_per_slice =
               (num_presels / stage_ptr->num_slices);

    offset += sizeof(bcmptm_rm_tcam_fp_stage_t);

    /* Report entry info segment bitmap of all pipes. */
    stage_ptr->entry_seg_bmp_offset = (offset - trans_info_size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_fp_entry_seg_bmp_t),
                              1,
                              BCMPTM_FP_ENTRY_SEG_BMP_T_ID));
    stage_ptr->entry_seg_bmp_size =
        (sizeof(bcmptm_fp_entry_seg_bmp_t));
    offset += stage_ptr->entry_seg_bmp_size;

    /* Report slices segment of all slices in all pipes. */
    stage_ptr->slice_seg = (offset - trans_info_size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_slice_t),
                              stage_ptr->num_slices,
                              BCMPTM_RM_TCAM_SLICE_T_ID));
    stage_ptr->slice_seg_size =
        (stage_ptr->num_slices * sizeof(bcmptm_rm_tcam_slice_t));
    offset  += stage_ptr->slice_seg_size;

    /* Report groups segment of maximum groups(can be created in all pipes) */
    stage_ptr->group_info_seg = (offset - trans_info_size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_fp_group_t),
                              stage_ptr->num_groups,
                              BCMPTM_RM_TCAM_FP_GROUP_T_ID));
    stage_ptr->group_info_seg_size =
        (stage_ptr->num_groups * sizeof(bcmptm_rm_tcam_fp_group_t));
    offset += stage_ptr->group_info_seg_size;

    /* LT config info is common for both EM and Ingress stages */
    /*
     * Allocate for pipes * num_groups_per_pipe as group_id is
     * used to save the priority in NON presel TCAM LT's.
     */
    if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        stage_ptr->lt_config_seg = (offset - trans_info_size);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit,
                                  BCMMGMT_RM_TCAM_COMP_ID,
                                  sub_component_id,
                                  offset,
                                  sizeof(bcmptm_rm_tcam_lt_config_t),
                                  (stage_ptr->num_pipes * stage_ptr->num_groups),
                                  BCMPTM_RM_TCAM_LT_CONFIG_T_ID));
        stage_ptr->lt_config_seg_size =
            (stage_ptr->num_groups *
             stage_ptr->num_pipes *
             sizeof(bcmptm_rm_tcam_lt_config_t));
        offset += stage_ptr->lt_config_seg_size;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t),
                              (num_entries + num_presels),
                              BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID));
    stage_ptr->entry_info_seg = (offset - trans_info_size);
    stage_ptr->entry_info_seg_size =
        (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    offset += stage_ptr->entry_info_seg_size;

    stage_ptr->presel_entry_info_seg = (offset - trans_info_size);
    stage_ptr->presel_entry_info_seg_size =
        (num_presels * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    offset += stage_ptr->presel_entry_info_seg_size;

    stage_ptr->entry_hash_seg = (offset - trans_info_size);
    stage_ptr->entry_hash_seg_size = (stage_ptr->entry_hash_size * sizeof(uint32_t));
    offset += stage_ptr->entry_hash_seg_size;

    stage_ptr->presel_entry_hash_seg = (offset - trans_info_size);
    stage_ptr->presel_entry_hash_seg_size =
        (stage_ptr->presel_entry_hash_size * sizeof(uint32_t));
    offset += stage_ptr->presel_entry_hash_seg_size;

    if (stage_ptr->num_presel_groups) {
        stage_ptr->presel_group_ref_count_seg = (offset - trans_info_size);
        stage_ptr->presel_group_ref_count_seg_size =
            stage_ptr->num_presel_groups * sizeof(uint32_t);
        offset += stage_ptr->presel_group_ref_count_seg_size;
    }

    if (stage_ptr->num_sbr_profiles) {
        stage_ptr->sbr_ref_count_seg = (offset - trans_info_size);
        stage_ptr->sbr_ref_count_seg_size =
            (stage_ptr->num_sbr_profiles * sizeof(uint32_t));
        offset += stage_ptr->sbr_ref_count_seg_size;
    }

    if (stage_ptr->num_pdd_profiles) {
        stage_ptr->pdd_ref_count_seg = (offset - trans_info_size);
        stage_ptr->pdd_ref_count_seg_size =
            (stage_ptr->num_pdd_profiles * sizeof(uint32_t));
        offset += stage_ptr->pdd_ref_count_seg_size;
    }

    /* slice init */
    rv = bcmptm_rm_tcam_fp_slice_init(unit, ltid, ltid_info, pipe, stage_ptr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* group init */
    rv = bcmptm_rm_tcam_fp_group_init(unit, pipe, stage_ptr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Lt config init */
    /* LT config info is common for both EM and Ingress stages */
    if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        rv = bcmptm_rm_tcam_fp_lt_config_init(unit, ltid,
                                              ltid_info, pipe, stage_ptr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* entry info init */
    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)
                 ((uint8_t *)stage_ptr + stage_ptr->entry_info_seg);

    for (idx = 0; idx < num_entries; idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    /* hash init */
    entry_hash = (int *)((uint8_t *)stage_ptr + stage_ptr->entry_hash_seg);
    for (idx = 0; idx < stage_ptr->entry_hash_size; idx++) {
        entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

    /* presel entry info init */
    presel_entry_info =
        (bcmptm_rm_tcam_prio_eid_entry_info_t*) ((uint8_t *)stage_ptr
             + stage_ptr->presel_entry_info_seg);

    for (idx = 0; idx < num_presels; idx++) {
        presel_entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        presel_entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        presel_entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        presel_entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    /* presel hash init */
    presel_entry_hash = (int *) ((uint8_t *)stage_ptr
                               + stage_ptr->presel_entry_hash_seg);
    for (idx = 0; idx < stage_ptr->presel_entry_hash_size; idx++) {
        presel_entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

    entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t *)
                     ((uint8_t *)stage_ptr +
                      stage_ptr->entry_seg_bmp_offset);
    sal_memset(&entry_seg_bmp[0], 0, sizeof(bcmptm_fp_entry_seg_bmp_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_ref_count_init(unit, stage_ptr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_sbr_profiles_ref_count_init(unit, stage_ptr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_pdd_profiles_ref_count_init(unit, stage_ptr));

    trans_info->state_size = offset;



    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_update_entry_type_wb(int unit,
                  bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bool reserved_found = false;
    bool valid_group_present = false;
    uint16_t idx;
    int num_entries = 0;
    int num_presels = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *presel_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    /* Fetch the group segment start pointer */
    group_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_ptr)
                  + stage_ptr->group_info_seg);

    /* Loop through the group segment to get the
     * group info requested
     */
    for (idx = 0; idx < stage_ptr->num_groups; idx++) {
        if (group_ptr->valid == 1) {
            valid_group_present = true;
            if (group_ptr->reserve_entry_index_exists == true) {
                reserved_found = true;
                break;
            }
        }
        group_ptr = group_ptr + 1;
    }

    /* group found with reserve entry. */
    if (reserved_found) {
        SHR_EXIT();
    }


    num_entries = stage_ptr->max_entries_per_slice
                    * stage_ptr->num_slices;
    num_presels = stage_ptr->max_presel_entries_per_slice
                    * stage_ptr->num_slices;

    /* entry info init */
    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)
                 ((uint8_t *)stage_ptr + stage_ptr->entry_info_seg);

    /*
     * If no groups are present check the first entry's entry_type, if it
     * is invalid, it is ISSU after reserve entry implementation release
     * to reserve entry implemetation release. No need to update entry type.
     */
    if (valid_group_present == false &&
        entry_info[0].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID &&
        entry_info[0].entry_id == BCMPTM_RM_TCAM_EID_INVALID &&
        entry_info[0].entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) {
        SHR_EXIT();
    }

    /*
     * ISSU from non reserve entry implementation release to
     * reserved entry implementation release
     */
    for (idx = 0; idx < num_entries; idx++) {
        if (entry_info[idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID &&
            entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
        }
    }

    /* presel entry info init */
    presel_entry_info =
        (bcmptm_rm_tcam_prio_eid_entry_info_t*) ((uint8_t *)stage_ptr
             + stage_ptr->presel_entry_info_seg);

    for (idx = 0; idx < num_presels; idx++) {
      if (presel_entry_info[idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID
          &&
          presel_entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
          presel_entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
      }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_init_metadata_wb(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t pipe,
                                   void *ha_blk,
                                   uint8_t sub_component_id)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    uint8_t num_slices = 0;
    uint8_t total_num_slices = 0;
    uint32_t total_num_groups = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint32_t max_num_entries_per_slice = 0;
    uint32_t max_num_presels_per_slice = 0;
    uint32_t num_entries = 0;
    uint32_t num_presels = 0;
    uint8_t num_pipes = 0;
    uint8_t slice_bundle_size = 0;
    uint64_t stage_flags = 0;
    uint8_t slice_id = 0;
    uint8_t num_presel_groups = 0;
    uint16_t num_sbr = 0;
    uint16_t num_pdd = 0;
    uint8_t total_presel_group_count = 0;
    uint16_t total_sbr_count = 0;
    uint16_t total_pdd_count = 0;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint32_t offset = 0;
    uint32_t issu_offset = 0;
    uint32_t trans_info_size = 0;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ha_blk, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    offset = 0;
    /* Get trans_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_TRANS_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    start_ptr = (uint8_t *)ha_blk;
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr);
    if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    trans_info_size = sizeof(bcmptm_rm_tcam_trans_info_t);
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_FP_STAGE_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    stage_ptr = (bcmptm_rm_tcam_fp_stage_t *)(start_ptr + offset);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   &num_pipes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(unit,
                                                         ltid,
                                                         ltid_info,
                                                         &slice_bundle_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_stage_flags_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   &stage_flags));
    /*
     * Ensure that num_pipes and slice_bundle_size is not changed
     * during warmboot.
     */
    if (stage_ptr->num_pipes != num_pipes ||
        stage_ptr->slice_bundle_size != slice_bundle_size) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(unit,
                                                   ltid,
                                                   ltid_info,
                                                   pipe,
                                                   &num_groups_per_pipe));
    total_num_groups += num_groups_per_pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                    ltid,
                                                    ltid_info,
                                                    pipe,
                                                    &num_slices));
    total_num_slices += num_slices;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_count_get(unit,
                                                  ltid,
                                                  ltid_info,
                                                  pipe,
                                                  &num_presel_groups));
    total_presel_group_count += num_presel_groups;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_sbr_profiles_count_get(unit,
                                                  ltid,
                                                  ltid_info,
                                                  pipe,
                                                  &num_sbr));
    total_sbr_count += num_sbr;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_pdd_profiles_count_get(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               &num_pdd));
    total_pdd_count += num_pdd;
    for (slice_id = 0; slice_id < num_slices; slice_id++) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(unit,
                                                ltid,
                                                ltid_info,
                                                slice_id,
                                                pipe,
                                                ref_grp_mode,
                                                &num_entries_per_slice));

         if (max_num_entries_per_slice < num_entries_per_slice) {
             max_num_entries_per_slice = num_entries_per_slice;
         }

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe,
                                                slice_id,
                                                &num_presels_per_slice));

         if (max_num_presels_per_slice < num_presels_per_slice) {
             max_num_presels_per_slice = num_presels_per_slice;
         }
    }

    if (total_num_slices == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Ensure that num_groups and num_slices is not changed
     * during warmboot.
     */
    if (stage_ptr->num_groups != total_num_groups ||
        stage_ptr->num_slices != total_num_slices ||
        stage_ptr->num_sbr_profiles != total_sbr_count ||
        stage_ptr->num_pdd_profiles != total_pdd_count ||
        stage_ptr->num_presel_groups != total_presel_group_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    num_entries = max_num_entries_per_slice * stage_ptr->num_slices;
    num_presels = max_num_presels_per_slice * stage_ptr->num_slices;

    /*
     * Ensure that num_entries and hash size is not changed
     * during warmboot.
     */
    if ((stage_ptr->entry_hash_size != (num_entries)) ||
        (stage_ptr->presel_entry_hash_size !=
         (num_presels / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) ||
        (stage_ptr->max_entries_per_slice !=
         (num_entries / stage_ptr->num_slices)) ||
        (stage_ptr->max_presel_entries_per_slice !=
         (num_presels / stage_ptr->num_slices))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset += sizeof(bcmptm_rm_tcam_fp_stage_t);
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_FP_ENTRY_SEG_BMP_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    stage_ptr->entry_seg_bmp_offset = (offset - trans_info_size);

    offset += (sizeof(bcmptm_fp_entry_seg_bmp_t));
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_SLICE_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    stage_ptr->slice_seg = (offset - trans_info_size);

    offset  += (stage_ptr->num_slices * sizeof(bcmptm_rm_tcam_slice_t));
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_FP_GROUP_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    stage_ptr->group_info_seg = (offset - trans_info_size);

    offset += (stage_ptr->num_groups * sizeof(bcmptm_rm_tcam_fp_group_t));
    if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        issu_offset = 0;
        bcmissu_struct_info_get(unit,
                                BCMPTM_RM_TCAM_LT_CONFIG_T_ID,
                                BCMMGMT_RM_TCAM_COMP_ID,
                                sub_component_id,
                                &issu_offset);
        if (offset != issu_offset) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        stage_ptr->lt_config_seg = (offset - trans_info_size);
        offset += (stage_ptr->num_pipes *
                   stage_ptr->num_groups * sizeof(bcmptm_rm_tcam_lt_config_t));
    }

    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    stage_ptr->entry_info_seg = (offset - trans_info_size);
    offset += (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    stage_ptr->presel_entry_info_seg = (offset - trans_info_size);
    offset += (num_presels * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));

    stage_ptr->entry_hash_seg = (offset - trans_info_size);
    offset += (stage_ptr->entry_hash_size * sizeof(uint32_t));

    stage_ptr->presel_entry_hash_seg = (offset - trans_info_size);
    offset += (stage_ptr->presel_entry_hash_size * sizeof(uint32_t));

    if (stage_ptr->num_presel_groups) {
        stage_ptr->presel_group_ref_count_seg = (offset - trans_info_size);
        offset += stage_ptr->num_presel_groups * sizeof(uint32_t);
    }

    if (stage_ptr->num_sbr_profiles) {
        stage_ptr->sbr_ref_count_seg = (offset - trans_info_size);
        offset += (stage_ptr->num_sbr_profiles * sizeof(uint32_t));
    }

    if (stage_ptr->num_pdd_profiles) {
        stage_ptr->pdd_ref_count_seg = (offset - trans_info_size);
        offset += (stage_ptr->num_pdd_profiles * sizeof(uint32_t));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_update_entry_type_wb(unit,
                                                stage_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_init_metadata(int unit,
                                bool warm,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    uint8_t pipe = 0;
    uint8_t *start_ptr = NULL;
    uint8_t sub_comp_id = 0;
    bcmptm_rm_tcam_md_t *lt_md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_md_get(unit, ltid, &lt_md));


    if (warm == TRUE) {
        for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
            sub_comp_id = lt_md->active_sub_component_id[pipe];
            start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata_wb(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               start_ptr,
                                               sub_comp_id));
            sub_comp_id = lt_md->backup_sub_component_id[pipe];
            start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata_wb(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               start_ptr,
                                               sub_comp_id));
        }
    } else {
       for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
            sub_comp_id = lt_md->active_sub_component_id[pipe];
            start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata_cb(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               start_ptr,
                                               sub_comp_id));
            sub_comp_id = lt_md->backup_sub_component_id[pipe];
            start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata_cb(unit,
                                               ltid,
                                               ltid_info,
                                               pipe,
                                               start_ptr,
                                               sub_comp_id));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_copy_metadata(int unit,
                                void *old_ptr,
                                void *new_ptr)
{
    uint32_t offset = 0;
    uint8_t *start_ptr_old = (uint8_t*)old_ptr;
    uint8_t *start_ptr_new = (uint8_t*)new_ptr;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr_old = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr_new = NULL;
    void *to_ptr = NULL;
    void *from_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    offset = 0;
    stage_ptr_old = (bcmptm_rm_tcam_fp_stage_t*)
        (start_ptr_old + offset + sizeof(bcmptm_rm_tcam_trans_info_t));
    stage_ptr_new = (bcmptm_rm_tcam_fp_stage_t*)
        (start_ptr_new + offset + sizeof(bcmptm_rm_tcam_trans_info_t));

    /*
     * Ensure that num_pipes is not changed
     * during dynamic LT update.
     */
    if (stage_ptr_old->num_pipes != stage_ptr_new->num_pipes) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Copy entry_seg_bmp data. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->entry_seg_bmp_offset);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->entry_seg_bmp_offset);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->entry_seg_bmp_size);

    /* Copy slice_seg data. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->slice_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->slice_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->slice_seg_size);

    /* Copy group_info_seg. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->group_info_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->group_info_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->group_info_seg_size);

    /* Copy lt_config_seg info. */
    if (!(stage_ptr_old->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {

        to_ptr = (void *)((uint8_t *)stage_ptr_new +
                          stage_ptr_new->lt_config_seg);
        from_ptr = (void *)((uint8_t *)stage_ptr_old +
                            stage_ptr_old->lt_config_seg);
        sal_memcpy(to_ptr,
                   from_ptr,
                   stage_ptr_old->lt_config_seg_size);
    }

    /* Copy entry_hash_seg. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->entry_hash_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->entry_hash_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->entry_hash_seg_size);

    /* Copy entry_info_seg data. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->entry_info_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->entry_info_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->entry_info_seg_size);

    /* Copy presel_entry_hash_seg data. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->presel_entry_hash_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->presel_entry_hash_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->presel_entry_hash_seg_size);

    /* Copy presel_entry_info_seg data. */
    to_ptr = (void *)((uint8_t *)stage_ptr_new +
                      stage_ptr_new->presel_entry_info_seg);
    from_ptr = (void *)((uint8_t *)stage_ptr_old +
                        stage_ptr_old->presel_entry_info_seg);
    sal_memcpy(to_ptr,
               from_ptr,
               stage_ptr_old->presel_entry_info_seg_size);
    if (stage_ptr_old->num_presel_groups) {
        /* Copy presel_group_ref_count_seg data. */
        to_ptr = (void *)((uint8_t *)stage_ptr_new +
                          stage_ptr_new->presel_group_ref_count_seg);
        from_ptr = (void *)((uint8_t *)stage_ptr_old +
                            stage_ptr_old->presel_group_ref_count_seg);
        sal_memcpy(to_ptr,
                   from_ptr,
                   stage_ptr_old->presel_group_ref_count_seg_size);
    }
    if (stage_ptr_old->num_sbr_profiles) {
        /* Copy sbr_ref_count_seg data. */
        to_ptr = (void *)((uint8_t *)stage_ptr_new +
                          stage_ptr_new->sbr_ref_count_seg);
        from_ptr = (void *)((uint8_t *)stage_ptr_old +
                            stage_ptr_old->sbr_ref_count_seg);
        sal_memcpy(to_ptr,
                   from_ptr,
                   stage_ptr_old->sbr_ref_count_seg_size);
    }
    if (stage_ptr_old->num_pdd_profiles) {
        /* Copy pdd_ref_count_seg data. */
        to_ptr = (void *)((uint8_t *)stage_ptr_new +
                          stage_ptr_new->pdd_ref_count_seg);
        from_ptr = (void *)((uint8_t *)stage_ptr_old +
                            stage_ptr_old->pdd_ref_count_seg);
        sal_memcpy(to_ptr,
                   from_ptr,
                   stage_ptr_old->pdd_ref_count_seg_size);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_stage_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_fp_stage_t **stage_fc)
{
    void *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    /* Fetch the tcam information for the given TCAM LTID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_info_get(unit,
                                 ltid,
                                 pipe_id,
                                 ltid_info,
                                 &tcam_info));

    *stage_fc = (bcmptm_rm_tcam_fp_stage_t *)((uint8_t *)tcam_info
                              + sizeof(bcmptm_rm_tcam_trans_info_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_slice_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  uint8_t *num_slices,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_slice_t **slice_fc)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          stage_id,
                                          &stage_fc));

    /* Fetch the number of slices in this stage */
    *num_slices = (stage_fc->num_slices);

    /* For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    /* Fetch the slice segment start pointer */
    *slice_fc = (bcmptm_rm_tcam_slice_t*) (((uint8_t *)stage_fc)
                 + stage_fc->slice_seg);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_tcam_fp_group_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  uint32_t group_id,
                  bcmptm_rm_tcam_fp_stage_t *stage_fc,
                  bcmptm_rm_tcam_fp_group_t **group_fc)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bool found = false;
    uint16_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(group_fc, SHR_E_PARAM);

    /* Fetch the group segment start pointer */
    group_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    /* Loop through the group segment to get the
     * group info requested
     */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((group_id == group_ptr->group_id)
             && (group_ptr->valid == 1)) {
            *group_fc = group_ptr;
            found = true;
            break;
        }
        group_ptr = group_ptr + 1;
    }

    /* if group not found. */
    if (!found) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_stage_attr_stage_flags_get(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint64_t *stage_flags)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_flags, SHR_E_PARAM);

    *stage_flags = ltid_info->rm_more_info[0].fp_flags;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slices_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t *num_slices)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_slices, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }
    *num_slices = 0;
    if (ltid_info->rm_more_info[0].slice_count != NULL) {
        *num_slices = ltid_info->rm_more_info[0].slice_count[pipe_id];
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_stage_attr_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_entries_per_slice)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_entries_per_slice, SHR_E_PARAM);
    *num_entries_per_slice = 0;

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id]) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if ((ltid_info->hw_entry_info[grp_mode].num_key_rows == 0)
        && (ltid_info->hw_entry_info[grp_mode].num_data_rows
        == 0)) {
        *num_entries_per_slice = 0;
    } else if ((grp_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA ||
               grp_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) &&
               (ltid_info->rm_more_info[0].slice_info[slice_id].isdw_no_support == 1)) {
        *num_entries_per_slice = 0;
    } else {

        switch (grp_mode) {
            case BCMPTM_RM_TCAM_GRP_MODE_HALF:
                 *num_entries_per_slice =
                   ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id] * 2;
                 break;
            case BCMPTM_RM_TCAM_GRP_MODE_SINGLE:
            case BCMPTM_RM_TCAM_GRP_MODE_DBLINTER:
            case BCMPTM_RM_TCAM_GRP_MODE_TRIPLE:
                 *num_entries_per_slice =
                   ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];
                 break;
            case BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA:
            case BCMPTM_RM_TCAM_GRP_MODE_QUAD:
                 *num_entries_per_slice =
                  ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];

                 if ((ltid_info->rm_more_info[0].fp_flags)
                      & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
                     *num_entries_per_slice = (*num_entries_per_slice)/2;
                 }
                 break;
            default:
                 SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slice_start_end_index_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  uint32_t *start_index,
                                  uint32_t *end_index)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(start_index, SHR_E_PARAM);
    SHR_NULL_CHECK(end_index, SHR_E_PARAM);

    *start_index = 0;
    *end_index = 0;

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if ((slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id])
        || !(ltid_info->rm_more_info[0].slice_info[slice_id].valid)) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *start_index = ltid_info->rm_more_info[0].slice_info[slice_id].start_index;
    *end_index = ltid_info->rm_more_info[0].slice_info[slice_id].end_index;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_pipes_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *num_pipes)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_pipes, SHR_E_PARAM);

    *num_pipes = ltid_info->rm_more_info[0].pipe_count;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *slice_bundle_size)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_bundle_size, SHR_E_PARAM);

    *slice_bundle_size = ltid_info->rm_more_info[0].bundle_size;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_groups_per_pipe)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_groups_per_pipe, SHR_E_PARAM);


    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    *num_groups_per_pipe = ltid_info->rm_more_info[0].group_count[pipe_id];

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_fp_lt_config_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_lt_config_t **lt_config)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmltd_sid_t ing_ltid;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          stage_id,
                                          &stage_fc));

    /* EM and IFP share same lt config info */
    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
        if (ltid_info->rm_more_info->shared_lt_count) {
            ing_ltid = ltid_info->rm_more_info->shared_lt_info[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                                  ing_ltid,
                                                  ltid_info,
                                                  pipe_id,
                                                  stage_id,
                                                  &stage_fc));
        }
    }

    /* Fetch the slice segment start pointer */
    *lt_config = (bcmptm_rm_tcam_lt_config_t*) (((uint8_t *)stage_fc)
                       + stage_fc->lt_config_seg);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_presel_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t slice_id,
                                  uint32_t *num_presel_entries_per_slice)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_presel_entries_per_slice, SHR_E_PARAM);

    *num_presel_entries_per_slice = 0;

    /*
     * For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id]) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (ltid_info->rm_more_info[0].fp_flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        *num_presel_entries_per_slice =
           (ltid_info->rm_more_info[0].presel_count[pipe_id][slice_id]);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_supported_slices_per_mode_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  int8_t *slice_id,
                                  uint8_t *num_slices)
{
    int slice_count = 0;
    int slice_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (ltid_info->rm_more_info[0].slice_count == NULL) {
        slice_count = 0;
    } else if ((ltid_info->hw_entry_info[grp_mode].num_key_rows == 0)
        && (ltid_info->hw_entry_info[grp_mode].num_data_rows
        == 0)) {
        *num_slices = 0;
    } else {
        slice_count = 0;
        for (slice_idx = 0; slice_idx <
            ltid_info->rm_more_info[0].slice_count[pipe_id]; slice_idx++) {
            if ((grp_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA ||
                grp_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD)) {
                if (!(ltid_info->rm_more_info[0].slice_info[slice_idx].isdw_no_support == 1)) {
                    slice_id[slice_count] = slice_idx;
                    slice_count += 1;
                }
            } else {
                slice_id[slice_count] = slice_idx;
                slice_count += 1;
            }
        }
    }
    *num_slices = slice_count;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_groups_per_mode_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_groups_per_mode)
{

    int div_factor[BCMPTM_RM_TCAM_GRP_MODE_COUNT] = {1, 1, 2, 3, 2, 1};
    int slice_count = 0;
    int slice_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_groups_per_mode, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }
    if ((ltid_info->hw_entry_info[grp_mode].num_key_rows == 0)
        && (ltid_info->hw_entry_info[grp_mode].num_data_rows
        == 0)) {
        *num_groups_per_mode = 0;
    } else {
        slice_count = ltid_info->rm_more_info[0].slice_count[pipe_id];

        if ((grp_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA ||
             grp_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD)) {
            slice_count = 0;
            for (slice_idx = 0; slice_idx <
                ltid_info->rm_more_info[0].slice_count[pipe_id]; slice_idx++) {
                if (!(ltid_info->rm_more_info[0].slice_info[slice_idx].isdw_no_support == 1)) {
                    slice_count += 1;
                }
            }
        }
        *num_groups_per_mode =
            ((slice_count / ltid_info->rm_more_info[0].bundle_size)
              * (ltid_info->rm_more_info[0].bundle_size
              / div_factor[grp_mode]));
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_action_resolution_ids_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_action_res_ids)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_action_res_ids, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    *num_action_res_ids = ltid_info->rm_more_info[0].num_action_res_ids;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_entries_per_group_get(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmptm_rm_tcam_entry_attrs_t *entry_attrs,
                               bool auto_expansion,
                               uint32_t *max_entries_per_group)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    uint32_t free_entries = 0;
    uint8_t slice_count = 0;
    bcmptm_rm_tcam_entry_iomd_t iomd;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attrs->pipe_id,
                                          entry_attrs->stage_id,
                                          &stage_fc));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attrs->pipe_id,
                                          entry_attrs->group_id,
                                          stage_fc,
                                          &fg));
    iomd.group_ptr = (void *)fg;
    iomd.ltid = ltid;
    iomd.ltid_info = ltid_info;
    iomd.entry_attrs = (void *)entry_attrs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_slice_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          &iomd.num_slices,
                                          entry_attrs->pipe_id,
                                          entry_attrs->stage_id,
                                          &slice_fc));
    iomd.slice_fc = slice_fc;
    iomd.free_entries = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_fp_entry_free_count_get(unit, &iomd));
    free_entries = iomd.free_entries;
    if (auto_expansion == FALSE) {
        *max_entries_per_group = free_entries - 1;
    } else {
        /* Caliculate the number of slices this group can expand to */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_validate_group_auto_expansion(unit,
                                                   ltid,
                                                   ltid_info,
                                                   entry_attrs,
                                                   &slice_count,
                                                   max_entries_per_group));
        /*
         * Get the number of entries that can be created in this
         * group mode with out auto expansion of group
         */
        *max_entries_per_group = free_entries + *max_entries_per_group - 1;
    }

 exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_fp_em_slice_info_update(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    uint16_t idx = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;
    uint8_t slice_id = 0;
    uint8_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          -1, /* pipe_id */
                                          0,
                                          &stage_fc));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_slice_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          &num_slices,
                                          -1, /* pipe_id */
                                          stage_fc->stage_id,
                                          &slice_fc));

    /* keep slices in auto expanded state */
    slice_id = 0;
    for (idx = 0; idx < num_slices -1; idx++) {
        for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
             mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
             mode++) {
            slice_fc[idx].next_slice_id_offset[mode] = 1;
        }
        if (idx != 0) {
            slice_fc[idx].slice_flags = slice_fc[slice_id].slice_flags;
            slice_fc[idx].primary_grp_id = slice_fc[slice_id].primary_grp_id;
            slice_fc[idx].free_presel_entries =
                    slice_fc[slice_id].free_presel_entries;
        }
    }
    slice_fc[idx].slice_flags = slice_fc[slice_id].slice_flags;
    slice_fc[idx].primary_grp_id = slice_fc[slice_id].primary_grp_id;
    slice_fc[idx].free_presel_entries =
                slice_fc[slice_id].free_presel_entries;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entries_exist(int unit,
                  bcmltd_sid_t ltid,
                  uint8_t *valid_entries)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t idx;
    bcmptm_rm_tcam_md_t *lt_md = NULL;
    uint8_t pipe = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));

    /*
     * Get num pipes from tcam_md ptr and check groups in all pipes
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_md_get(unit, ltid, &lt_md));


    for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {

       /* Fetch the corresponding stage info, stage_id is not required */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                             ltid,
                                             &ltid_info,
                                             pipe, /*pipe_id */
                                             0,
                                             &stage_fc));

       /* Fetch the group segment start pointer */
       group_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                     + stage_fc->group_info_seg);

       *valid_entries = 0;
       /*
        * Loop through the group segment to get the
        * group info requested
        */
       for (idx =0;idx < stage_fc->num_groups; idx++) {
           if ((group_ptr->valid == 1)) {
               *valid_entries = 1;
               break;
           }
           group_ptr = group_ptr + 1;
       }
       if (*valid_entries == 1) {
           break;
       }
    }

exit:
    SHR_FUNC_EXIT();
}
