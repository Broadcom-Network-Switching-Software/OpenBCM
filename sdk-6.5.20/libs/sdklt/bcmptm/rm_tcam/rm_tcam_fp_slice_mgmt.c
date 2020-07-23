/*! \file rm_tcam_fp_slice_mgmt.c
 *
 * Low Level Functions for FP Slice Management
 * This file contains low level functions for FP based TCAMs
 * to do Slice allocate opertaions for Single/Double/Triple wide
 * groups.
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
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "rm_tcam_fp_pdd.h"
#include "rm_tcam_fp_sbr.h"
#include "rm_tcam_fp_psg.h"
#include "rm_tcam_fp_profiles.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/* Maximum value supported for a LT partition in hardware. */
#define FP_MAX_LT_PART_PRIO (15)

/* Local macros used for defining structure for passing
 * metadata across slice alloc functions
 */

/* Local macro indicating invalid bundles */
#define INVALID_BUNDLE       -1
/* Local macro indicating max number of bundles */
#define MAX_NO_OF_BUNDLES    8
/* Local macro indicating max number of slices */
#define MAX_NUM_SLICES       24
/* Local macro for invalid slice id */
#define INVALID_SLICE_ID     -1

#define RESERVE_SLICE(slice, sliceid, slice_mode) \
    { \
        slice[sliceid].slice_flags |= BCMPTM_SLICE_IN_USE; \
        slice[sliceid].slice_flags |= slice_mode; \
    }

#define CHECK_DOUBLE_PRESENT(slice, sliceid) \
    (slice[sliceid].slice_flags & BCMPTM_SLICE_MODE_DOUBLE)

#define CHECK_TRIPLE_PRESENT(slice, sliceid) \
    (slice[sliceid].slice_flags & BCMPTM_SLICE_MODE_TRIPLE)

#define CHECK_DOUBLE_CAPABLE(slice, sliceid) \
    (slice[sliceid].slice_flags & BCMPTM_SLICE_MODE_DOUBLE_CAPABLE)

#define CHECK_TRIPLE_CAPABLE(slice, sliceid) \
    (slice[sliceid].slice_flags & BCMPTM_SLICE_MODE_TRIPLE_CAPABLE)

#define CHECK_SLICE_IS_FREE(slice, sliceid) \
    ((slice[sliceid].slice_flags & BCMPTM_SLICE_IN_USE) == 0)

#define CHECK_SLICE_IN_USE(slice, sliceid) \
    (slice[sliceid].slice_flags & BCMPTM_SLICE_IN_USE)

#define SLICE_PART_OF_CURRENT_GROUP(sliceid, primary, slice_count) \
    ((sliceid >= primary) && (sliceid < (primary + slice_count)))



#define GET_BUNDLE(x, y)  (x / y)


typedef struct slice_mapping {
    /* Bit map for single wide used slices in bundle */
    uint8_t single_map[MAX_NO_OF_BUNDLES];
    /* Bit map for double wide used slices in bundle */
    uint8_t double_map[MAX_NO_OF_BUNDLES];
    /* Count of double wide used slices in bundle */
    uint8_t double_count[MAX_NO_OF_BUNDLES];
    /* Bit map for triple wide used slices in bundle */
    uint8_t triple_map[MAX_NO_OF_BUNDLES];
    /* Count of triple wide used slices in bundle */
    uint8_t triple_count[MAX_NO_OF_BUNDLES];
    /* Flag indicating whether bundle is eligible for move */
    uint8_t move_eligible[MAX_NO_OF_BUNDLES];
    /* number of bundles  */
    uint8_t num_bundles;
    /* number of slices  */
    uint8_t num_slices;
    /* number of slices free */
    uint8_t free_slice_count;
    /* Total double wide usage */
    uint8_t total_double_count;
    /* Total triple wide usage */
    uint8_t total_triple_count;
    /* Array of free slices */
    uint8_t free_slice[MAX_NUM_SLICES];
    /* Array of slice sizes */
    uint16_t slice_size[MAX_NUM_SLICES];
} slice_mapping_t;

static int non_single_move = 0;
/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Move the slice from src to dst for width > 1
 *
 * \param [in] unit BCM device number.
 * \param [in] slice slice array
 * \param [in] src src slice
 * \param [in] dst destination slice
 * \param [in] width width of the slice to be moved
 *
 * \retval SHR_E_NONE Success
 */

static int
bcmptm_rm_tcam_fp_move_slice(int unit,
                             bcmptm_rm_tcam_slice_t *slice,
                             int src,
                             int dst,
                             int width)
{
    int i;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Src slice:%d "
               "Dest slice:%d Width: %d\n"), src,dst,width));

    for (i = 0; i <= width; i++) {
        slice[src + i].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
        slice[src + i].slice_flags &= ~(BCMPTM_SLICE_MODE_SINGLE
                                      | BCMPTM_SLICE_MODE_DOUBLE
                                      | BCMPTM_SLICE_MODE_TRIPLE);
    }
    for (i = 0; i <= width; i++) {
        slice[dst + i].slice_flags |= (BCMPTM_SLICE_IN_USE);
        if (width == 2) {
            slice[dst + i].slice_flags |= BCMPTM_SLICE_MODE_DOUBLE;
        } else {
            slice[dst + i].slice_flags |= BCMPTM_SLICE_MODE_SINGLE;
        }
    }
    return SHR_E_NONE;
}


/*!
 * \brief Get the map and count for a particular width
 *
 * \param [in] unit BCM device number.
 * \param [in] s_count slice count
 * \param [in] slice_map source mapping data
 * \param [out] width_map map array for a particular width
 * \param [out] map_count map array for a particular width
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Width is not supported.
 */

static int
bcmptm_rm_tcam_get_map(int unit,
                       int s_count,
                       slice_mapping_t *slice_map,
                       uint8_t **width_map,
                       uint8_t **map_count)
{
    SHR_FUNC_ENTER(unit);
    if (s_count == 2) {
        *width_map = slice_map->double_map;
        *map_count = slice_map->double_count;
    } else if (s_count == 3) {
        *width_map = slice_map->triple_map;
        *map_count = slice_map->triple_count;
    } else {
        SHR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief   This function is used to move entries with width > 1
 *
 * \param [in] unit BCM device number.
 * \param [in] slice array
 * \param [in] s_count slice count
 * \param [in] slice_map source mapping data
 * \param [in] primary primary slice
 *
 * \return   SHR_E_NONE Success
 * \return   SHR_E_INTERNAL Internal Failure
 */

static int
bcmptm_rm_tcam_fp_move_entries_bundle(int unit,
                                      bcmptm_rm_tcam_slice_t *slice,
                                      int s_count,
                                      slice_mapping_t *slice_map,
                                      uint32_t primary)
{
    int slice_count;
    uint8_t bundle_index;
    int slice_index;
    int slice_no = 0;
    uint8_t *curr_map;
    uint8_t *curr_count;

    SHR_FUNC_ENTER(unit);

    slice_count = slice_map->num_slices / slice_map->num_bundles;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_get_map(unit,
                                s_count,
                                slice_map,
                                &curr_map,
                                &curr_count));
    for (bundle_index = 0;
         bundle_index < slice_map->num_bundles; bundle_index++) {
        if (slice_map->move_eligible[bundle_index] == 0) {
            continue;
        }
        if (GET_BUNDLE(primary, slice_count) == bundle_index) {
            continue;
        }
        if (curr_count[bundle_index] == 0) {
            continue;
        }
        for (slice_index = 0; slice_index < slice_count; slice_index++) {
            if (curr_map[bundle_index] & (1 << slice_index)) {
                slice_no = bundle_index * slice_count + slice_index;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_move_slice(unit,
                                                  slice,
                                                  slice_no,
                                                  primary,
                                                  s_count));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif This function clear the presel information from
 *\n soure slice  This is done while freeing the slice
 *\n after moved
 *
 * \param [in]unit BCM device number.
 * \param [in] iomd input output meta data
 * \param [in] slice_id source slice to be cleared
 * \param [in] entry_attr entry attributes for insert
 *
 * \return SHR_E_NONE Success
 */

static int
bcmptm_rm_tcam_fp_tcam_entries_clear(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t slice_id,
                                     bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint8_t idx;
    uint8_t num_tcam_sids_src = 1;
    uint8_t tile_id;
    uint16_t depth_inst_src = 0;
    bcmdrd_sid_t sid;
    uint32_t full_entry_words[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t rsp_flags = 0;
    int min_idx, max_idx, num_entries, tcam_idx;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;
    bcmltd_sid_t ltid;
    bcmptm_rm_tcam_lt_info_t *ltid_info;

    SHR_FUNC_ENTER(unit);
    ltid = iomd->ltid;
    ltid_info = iomd->ltid_info;
    pt_dyn_info.tbl_inst = entry_attr->pipe_id;
    rm_more_info = ltid_info->rm_more_info;
    for (idx = 0; idx < slice_id; idx++ ){
        tile_id = rm_more_info->slice_info[idx].tile_id;
        num_tcam_sids_src = 1;
        if (rm_more_info->tile_info != NULL)  {
            num_tcam_sids_src =
                rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
        }
        depth_inst_src += num_tcam_sids_src;
    }
    tile_id = rm_more_info->slice_info[idx].tile_id;
    if (rm_more_info->tile_info != NULL)  {
        num_tcam_sids_src =
            rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
    }
    hw_entry_info = &ltid_info->hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE];
    sid = hw_entry_info->sid[depth_inst_src][0];
    sal_memset(full_entry_words, 0, sizeof(full_entry_words));
    for (idx = 0; idx < num_tcam_sids_src;idx++) {
        min_idx = bcmptm_pt_index_min(unit, sid);
        max_idx = bcmptm_pt_index_max(unit, sid);
        num_entries = (max_idx - min_idx + 1);
        for (tcam_idx =0; tcam_idx < num_entries; tcam_idx++) {
            pt_dyn_info.index = tcam_idx + min_idx;
            sid = hw_entry_info->sid_data_only[depth_inst_src][0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                   0,
                                   iomd->trans_id,
                                   sid,
                                   &pt_dyn_info,
                                   NULL, /* misc_info */
                                   full_entry_words,
                                   1,
                                   1,
                                   1,
                                   BCMPTM_RM_OP_NORMAL,
                                   NULL,
                                   ltid,
                                   0,
                                   &rsp_flags));
            sid = hw_entry_info->sid[depth_inst_src + idx][0];
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_cmdproc_write(unit,
                                    0,
                                    iomd->trans_id,
                                    sid,
                                    &pt_dyn_info,
                                    NULL, /* misc_info */
                                    full_entry_words,
                                    1,
                                    1,
                                    1,
                                    BCMPTM_RM_OP_NORMAL,
                                    NULL,
                                    ltid,
                                    0,
                                    &rsp_flags));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function clear the presel information from
 *\n soure slice  This is done while freeing the slice
 *\n after moved
 *
 * \param [in] unit BCM device number.
 * \param [in] iomd input output meta data
 * \param [in] entry_attr entry attributes for insert
 * \param [in] source slice
 *
 * \return SHR_E_NONE Success
 */

static int
bcmptm_rm_tcam_fp_tcam_presel_entries_clear(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd,
                                            bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                            int src)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid;
    uint32_t full_entry_words[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t rsp_flags = 0;
    int min_idx, max_idx, num_entries, tcam_idx;
    bcmptm_rm_tcam_lt_info_t presel_ltid_info;
    bcmltd_sid_t  presel_ltid;
    uint32_t num_presel_per_slice;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;
    bcmltd_sid_t ltid;
    bcmptm_rm_tcam_lt_info_t *ltid_info;

    SHR_FUNC_ENTER(unit);
    ltid = iomd->ltid;
    ltid_info = iomd->ltid_info;
    pt_dyn_info.tbl_inst = entry_attr->pipe_id;
    if (ltid_info->rm_more_info->shared_lt_count) {
        presel_ltid = ltid_info->rm_more_info->shared_lt_info[0];
        sal_memset(&presel_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_ptrm_info_get(unit, presel_ltid, 0, 0,
                                         ((void *)&presel_ltid_info),
                                         NULL, NULL));
    } else {
        presel_ltid = ltid;
        presel_ltid_info = *ltid_info;
    }

    /* get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                   unit, ltid, ltid_info, entry_attr->pipe_id,
                                   src,
                                   &num_presel_per_slice));

    hw_entry_info =
        &presel_ltid_info.hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE];
    sid = hw_entry_info->sid[src][0];
    sal_memset(full_entry_words, 0, sizeof(full_entry_words));

    min_idx = bcmptm_pt_index_min(unit, sid);
    max_idx = bcmptm_pt_index_max(unit, sid);
    num_entries = (max_idx - min_idx + 1);

    for (tcam_idx =0; tcam_idx < num_entries; tcam_idx++) {
        pt_dyn_info.index = tcam_idx + min_idx;
        if (hw_entry_info->non_aggr == 1) {
            sid = hw_entry_info->sid_data_only[src][0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                   0,
                                   iomd->trans_id,
                                   sid,
                                   &pt_dyn_info,
                                   NULL, /* misc_info */
                                   full_entry_words,
                                   1,
                                   1,
                                   1,
                                   BCMPTM_RM_OP_NORMAL,
                                   NULL,
                                   ltid,
                                   0,
                                   &rsp_flags));
            }
            sid = hw_entry_info->sid[src][0];
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_cmdproc_write(unit,
                                    0,
                                    iomd->trans_id,
                                    sid,
                                    &pt_dyn_info,
                                    NULL, /* misc_info */
                                    full_entry_words,
                                    1,
                                    1,
                                    1,
                                    BCMPTM_RM_OP_NORMAL,
                                    NULL,
                                    ltid,
                                    0,
                                    &rsp_flags));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function copies the tcam information from
 *\n soure slice to destination slice in hardware and
 *\n software.
 *
 * \param [in] unit BCM device number.
 * \param [in] iomd input output metadata
 * \param [in] entry_attr entry attributes for insert
 * \param [in] src source slice
 * \param [in] dst destination slice
 *
 * \return SHR_E_NONE Success
 */

static int
bcmptm_rm_tcam_fp_tcam_entries_copy(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                    int src,
                                    int dst)
{
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint8_t idx;
    uint8_t num_tcam_sids_src = 1;
    uint8_t num_tcam_sids_dst = 1;
    uint8_t tile_id;
    uint16_t depth_inst_src = 0;
    uint16_t depth_inst_dst = 0;
    bcmdrd_sid_t sid;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    uint32_t full_entry_words[BCMPTM_MAX_PT_FIELD_WORDS];
    bool rsp_entry_cache_vbit = 0;
    uint16_t rsp_dfield_format_id;
    bcmltd_sid_t rsp_ltid=0;
    uint32_t rsp_flags = 0;
    int min_idx, max_idx, num_entries, tcam_idx;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;
    bcmltd_sid_t ltid;
    bcmptm_rm_tcam_lt_info_t *ltid_info;

    SHR_FUNC_ENTER(unit);
    ltid = iomd->ltid;
    ltid_info = iomd->ltid_info;
    pt_dyn_info.tbl_inst = entry_attr->pipe_id;
    rm_more_info = ltid_info->rm_more_info;
    for (idx = 0; idx < src; idx++ ){
        tile_id = rm_more_info->slice_info[idx].tile_id;
        if (rm_more_info->tile_info != NULL)  {
            num_tcam_sids_src =
                rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
        }
        depth_inst_src += num_tcam_sids_src;
    }
    tile_id = rm_more_info->slice_info[idx].tile_id;
    if (rm_more_info->tile_info != NULL)  {
        num_tcam_sids_src =
            rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
    }
    for (idx = 0; idx < dst; idx++ ){
        tile_id = rm_more_info->slice_info[idx].tile_id;
        if (rm_more_info->tile_info != NULL)  {
            num_tcam_sids_dst =
                rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
        }
        depth_inst_dst += num_tcam_sids_dst;
    }
    tile_id = rm_more_info->slice_info[idx].tile_id;
    if (rm_more_info->tile_info != NULL)  {
        num_tcam_sids_dst =
            rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
    }
    hw_entry_info = &ltid_info->hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE];
    sid = hw_entry_info->sid[depth_inst_src][0];
    for (idx = 0; idx < num_tcam_sids_dst;idx++) {
        min_idx = bcmptm_pt_index_min(unit, sid);
        max_idx = bcmptm_pt_index_max(unit, sid);
        num_entries = (max_idx - min_idx + 1);
        for (tcam_idx =0; tcam_idx < num_entries; tcam_idx++) {
            if (hw_entry_info->non_aggr == 1) {
                sid = hw_entry_info->sid_data_only[depth_inst_src + idx][0];
                sal_memset(full_entry_words, 0, sizeof(full_entry_words));
                pt_dyn_info.index = tcam_idx + min_idx;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cmdproc_read(unit,
                                      0,
                                      iomd->trans_id,
                                      sid,
                                      &pt_dyn_info,
                                      NULL, /* misc_info */
                                      entry_words_size,
                                      full_entry_words,
                                      &rsp_entry_cache_vbit,
                                      &rsp_ltid,
                                      &rsp_dfield_format_id,
                                      &rsp_flags));
                sid = hw_entry_info->sid_data_only[depth_inst_dst + idx][0];
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cmdproc_write(unit,
                                       0,
                                       iomd->trans_id,
                                       sid,
                                       &pt_dyn_info,
                                       NULL, /* misc_info */
                                       full_entry_words,
                                       1,
                                       1,
                                       1,
                                       BCMPTM_RM_OP_NORMAL,
                                       NULL,
                                       ltid,
                                       0,
                                       &rsp_flags));
            }
            sid = hw_entry_info->sid[depth_inst_src + idx][0];
            sal_memset(full_entry_words, 0, sizeof(full_entry_words));
            pt_dyn_info.index = tcam_idx + min_idx;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_read(unit,
                                  0,
                                  iomd->trans_id,
                                  sid,
                                  &pt_dyn_info,
                                  NULL, /* misc_info */
                                  entry_words_size,
                                  full_entry_words,
                                  &rsp_entry_cache_vbit,
                                  &rsp_ltid,
                                  &rsp_dfield_format_id,
                                  &rsp_flags));
            sid = hw_entry_info->sid[depth_inst_dst + idx][0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                   0,
                                   iomd->trans_id,
                                   sid,
                                   &pt_dyn_info,
                                   NULL, /* misc_info */
                                   full_entry_words,
                                   1,
                                   1,
                                   1,
                                   BCMPTM_RM_OP_NORMAL,
                                   NULL,
                                   ltid,
                                   0,
                                   &rsp_flags));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function copies the presel information from
 *\n soure slice to destination slice in hardware and
 *\n software. It returns entry info and hash segement
 *\n pointer to store in group structure
 *
 * \param [in] unit BCM device number.
 * \param [in] iomd input output metadata
 * \param [in] entry_attr entry attributes for insert
 * \param [in] src source slice
 * \param [in] dst destination slice
 * \param [out] presel_entry_info presel entry info pointer to update in grp struct
 * \param [out] presel_hash_seg presel hash seg pointer for update in grp struct
 * Returns:
 *    SHR_E_NONE
 */

static int
bcmptm_rm_tcam_fp_tcam_presel_entries_copy(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                                           bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                           int src,
                                           int dst,
                                           uint32_t *presel_entry_info,
                                           uint32_t *presel_hash_seg)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    uint32_t full_entry_words[BCMPTM_MAX_PT_FIELD_WORDS];
    bool rsp_entry_cache_vbit = 0;
    uint16_t rsp_dfield_format_id;
    bcmltd_sid_t rsp_ltid=0;
    uint32_t rsp_flags = 0;
    int min_idx, max_idx, num_entries, tcam_idx;
    bcmptm_rm_tcam_lt_info_t presel_ltid_info;
    bcmltd_sid_t  presel_ltid;
    uint32_t presel_entry_info_seg_src;
    uint32_t presel_entry_info_seg_dst;
    uint32_t presel_entry_hash_size;
    uint32_t presel_entry_hash_seg_src;
    uint32_t presel_entry_hash_seg_dst;
    void *src_ptr;
    void *dst_ptr;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;
    bcmltd_sid_t ltid;
    bcmptm_rm_tcam_lt_info_t *ltid_info;
    bcmptm_rm_tcam_fp_stage_t *stage_fc;

    SHR_FUNC_ENTER(unit);
    ltid = iomd->ltid;
    ltid_info = iomd->ltid_info;
    stage_fc = iomd->stage_fc;

    pt_dyn_info.tbl_inst = entry_attr->pipe_id;

    if (ltid_info->rm_more_info->shared_lt_count) {
        presel_ltid = ltid_info->rm_more_info->shared_lt_info[0];
        sal_memset(&presel_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                           presel_ltid,
                                           0,
                                           0,
                                           ((void *)&presel_ltid_info),
                                           NULL,
                                           NULL));
    } else {
        presel_ltid = ltid;
        presel_ltid_info = *ltid_info;
    }
    sid = presel_ltid_info. \
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].sid[src][0];
    min_idx = bcmptm_pt_index_min(unit, sid);
    max_idx = bcmptm_pt_index_max(unit, sid);
    num_entries = (max_idx - min_idx + 1);
    presel_entry_info_seg_src = (stage_fc->presel_entry_info_seg)
                           + ((stage_fc->max_presel_entries_per_slice
                           * src)
                           * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    presel_entry_info_seg_dst = (stage_fc->presel_entry_info_seg)
                           + ((stage_fc->max_presel_entries_per_slice
                           * dst)
                           * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));

    presel_entry_hash_size = (stage_fc->max_presel_entries_per_slice
                                  / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
    presel_entry_hash_seg_src = (stage_fc->presel_entry_hash_seg)
                           + ((presel_entry_hash_size
                                * src)
                            * sizeof(int));
    presel_entry_hash_seg_dst = (stage_fc->presel_entry_hash_seg)
                           + ((presel_entry_hash_size
                                * dst) * sizeof(int));
    src_ptr = (uint8_t *)stage_fc + presel_entry_info_seg_src;
    dst_ptr = (uint8_t *)stage_fc + presel_entry_info_seg_dst;
    sal_memcpy(dst_ptr, src_ptr,
        stage_fc->max_presel_entries_per_slice *
            sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));

    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t *)src_ptr;
    for (tcam_idx = 0;
        tcam_idx < stage_fc->max_presel_entries_per_slice; tcam_idx++) {
        entry_info[tcam_idx].entry_id =BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[tcam_idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[tcam_idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[tcam_idx].global_to_all_pipes = FALSE;
        entry_info[tcam_idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    src_ptr = (uint8_t *)stage_fc + presel_entry_hash_seg_src;
    dst_ptr = (uint8_t *)stage_fc + presel_entry_hash_seg_dst;
    sal_memcpy(dst_ptr, src_ptr, presel_entry_hash_size * sizeof(int));
    sal_memset((void *)src_ptr, 0,presel_entry_hash_size * sizeof(int));

    *presel_entry_info = presel_entry_info_seg_dst;
    *presel_hash_seg = presel_entry_hash_seg_dst;
    hw_entry_info =
        &presel_ltid_info.hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE];
    for (tcam_idx =0; tcam_idx < num_entries; tcam_idx++) {
        if (hw_entry_info->non_aggr == 1) {
            sid = hw_entry_info->sid_data_only[src][0];
            sal_memset(full_entry_words, 0, sizeof(full_entry_words));
            pt_dyn_info.index = tcam_idx + min_idx;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_read(unit,
                                  0,
                                  iomd->trans_id,
                                  sid,
                                  &pt_dyn_info,
                                  NULL, /* misc_info */
                                  entry_words_size,
                                  full_entry_words,
                                  &rsp_entry_cache_vbit,
                                  &rsp_ltid,
                                  &rsp_dfield_format_id,
                                  &rsp_flags));
            sid = hw_entry_info->sid_data_only[dst][0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                   0,
                                   iomd->trans_id,
                                   sid,
                                   &pt_dyn_info,
                                   NULL, /* misc_info */
                                   full_entry_words,
                                   1,
                                   1,
                                   1,
                                   BCMPTM_RM_OP_NORMAL,
                                   NULL,
                                   ltid,
                                   0,
                                   &rsp_flags));
        }
        sid = hw_entry_info->sid[src][0];
        sal_memset(full_entry_words, 0, sizeof(full_entry_words));
        pt_dyn_info.index = tcam_idx + min_idx;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cmdproc_read(unit,
                              0,
                              iomd->trans_id,
                              sid,
                              &pt_dyn_info,
                              NULL, /* misc_info */
                              entry_words_size,
                              full_entry_words,
                              &rsp_entry_cache_vbit,
                              &rsp_ltid,
                              &rsp_dfield_format_id,
                              &rsp_flags));
        sid = hw_entry_info->sid[dst][0];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cmdproc_write(unit,
                               0,
                               iomd->trans_id,
                               sid,
                               &pt_dyn_info,
                               NULL, /* misc_info */
                               full_entry_words,
                               1,
                               1,
                               1,
                               BCMPTM_RM_OP_NORMAL,
                               NULL,
                               ltid,
                               0,
                               &rsp_flags));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief This function takes the src and destination array
 *\n and moves src[i] slice to dst[i] slice. Total number
 *\n of slices to be moved also is taken as a paramter
 *
 * \param [in] unit BCM device number.
 * \param [in] iomd input output meta data structure
 * \param [in] src source slice array
 * \param [in] dst estination slice array
 * \param [in] num_moves number of moves needed.
 * Returns:
 *    SHR_E_NONE Success
 */


static int
bcmptm_rm_tcam_fp_move_single_slice(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    int *src,
                                    int *dst,
                                    int num_moves)
{
    int i;
    bcmptm_rm_tcam_entry_attrs_t pri_entry_attr;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr_temp;
    int next_slice_id = INVALID_SLICE_ID, prev_slice_id = INVALID_SLICE_ID;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    int16_t next_fg_id = -1;
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    uint32_t presel_einfo_seg = 0;
    uint32_t presel_hash_seg = 0;
    bcmptm_rm_tcam_slice_t *slice;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);
    slice = iomd->slice_fc;
    stage_fc = iomd->stage_fc;

    entry_attr_temp = iomd->entry_attrs;
    iomd->entry_attrs = &pri_entry_attr;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_lt_config_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              entry_attr_temp->pipe_id,
                                              stage_fc->stage_id,
                                              &lt_config));

    for (i = 0; i < num_moves; i++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
           (BSL_META_U(unit, "Slice id Src: %d "
           "Dest: %d\n"), src[i],dst[i]));
        prev_slice_id = INVALID_SLICE_ID;
        next_slice_id = INVALID_SLICE_ID;
        sal_memcpy(&pri_entry_attr,
            entry_attr_temp,sizeof(bcmptm_rm_tcam_entry_attrs_t));
        /* Change the group mode and group id. FP profile copy function
         * works using the group-id and using the primary slideid of the group
         * to copy the profiles. Use the primary group of the source slice
         * so that the function can be re-used
         */
        pri_entry_attr.group_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
        pri_entry_attr.group_id = slice[src[i]].primary_grp_id;
        /* Copy all the FP profiles from the source slice to destination */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_profiles_copy(unit,
                                             iomd,
                                             dst[i]));
       /* Copy the TCAM entries in the main TCAM */
       SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_tcam_entries_copy(unit,
                                            iomd,
                                            &pri_entry_attr,
                                            src[i],
                                            dst[i]));
        /* Copy the presel entries in the presel TCAM
         * associated with the slice
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_tcam_presel_entries_copy(unit,
                                                   iomd,
                                                   &pri_entry_attr,
                                                   src[i],
                                                   dst[i],
                                                   &presel_einfo_seg,
                                                   &presel_hash_seg));
        /*
         * Detach the src slice if it is an autoexpanded slice
         * Attach the destination slice in the autoexpanded case.
         */
        if (slice[src[i]].\
            prev_slice_id_offset[pri_entry_attr.group_mode] != 0) {
            prev_slice_id = src[i] +
                slice[src[i]].prev_slice_id_offset[pri_entry_attr.group_mode];
            slice[prev_slice_id].\
                next_slice_id_offset[pri_entry_attr.group_mode] =
                dst[i] - prev_slice_id;
        }
        if (slice[src[i]].\
            next_slice_id_offset[pri_entry_attr.group_mode] != 0) {
            next_slice_id = src[i] +
                slice[src[i]].next_slice_id_offset[pri_entry_attr.group_mode];
            slice[next_slice_id].\
                prev_slice_id_offset[pri_entry_attr.group_mode] =
                dst[i] - next_slice_id;
        }
        /* Update destination slice from src slice values */
        slice[dst[i]].slice_flags = slice[src[i]].slice_flags;
        slice[dst[i]].primary_grp_id = slice[src[i]].primary_grp_id;
        slice[dst[i]].free_entries = slice[src[i]].free_entries +
            slice[dst[i]].num_entries - slice[src[i]].num_entries;
        slice[dst[i]].free_presel_entries =  slice[src[i]].free_presel_entries;

        for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
            mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
            mode++) {
            slice[dst[i]].next_slice_id_offset[mode] = 0;
            slice[dst[i]].prev_slice_id_offset[mode] = 0;
            slice[dst[i]].left_slice_id_offset[mode] = 0;
            slice[dst[i]].right_slice_id_offset[mode] = 0;
        }
        /*
         * Attach destination slice at right point if auto
         * expanded slice
         */

        if (prev_slice_id != INVALID_SLICE_ID) {
            slice[dst[i]].prev_slice_id_offset[pri_entry_attr.group_mode] =
                prev_slice_id - dst[i];
        }
        if (next_slice_id != INVALID_SLICE_ID) {
            slice[dst[i]].next_slice_id_offset[pri_entry_attr.group_mode] =
                next_slice_id - dst[i];
        }
        /*
         * Get the first group id which is primary group sharing the slices
         * Update group primary slice if src slice is primary slice
         * Update partition priority for each of the groups sharing the slice.
         * Enable the slice.
         */
        next_fg_id = slice[dst[i]].primary_grp_id;
        do {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  pri_entry_attr.pipe_id,
                                                  next_fg_id,
                                                  stage_fc,
                                                  &fg));
            if (fg->primary_slice_id == src[i]) {
                fg->primary_slice_id = dst[i];
                fg->presel_entry_info_seg = presel_einfo_seg;
                fg->presel_entry_hash_seg = presel_hash_seg;
            }
            lt_config[fg->group_ltid].lt_part_map &= ~(1 << src[i]);
            lt_config[fg->group_ltid].lt_part_map |= (1 << dst[i]);

            lt_config[fg->group_ltid].lt_part_pri[dst[i]]
                     = lt_config[fg->group_ltid].lt_part_pri[src[i]];
            lt_config[fg->group_ltid].lt_part_pri[src[i]] = 0;
            /*
             * Set partition priority for the
             * group. Also set's the ING_CONFIGr (slice enable)
             */

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                                   iomd->ltid,
                                                   iomd->ltid_info,
                                                   fg,
                                                   entry_attr_temp->pipe_id,
                                                   entry_attr_temp->stage_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_slice_enable_set(unit,
                                                   iomd->ltid,
                                                   iomd->ltid_info,
                                                   pri_entry_attr.pipe_id,
                                                   pri_entry_attr.stage_id,
                                                   fg->group_mode,
                                                   dst[i],
                                                  1));
            next_fg_id = fg->next_shared_group_id;
        } while (next_fg_id != -1);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_slice_enable_set(unit,
                                                iomd->ltid,
                                                iomd->ltid_info,
                                                pri_entry_attr.pipe_id,
                                                pri_entry_attr.stage_id,
                                                pri_entry_attr.group_mode,
                                                src[i],
                                                0));
        slice[src[i]].primary_grp_id = -1;
        slice[src[i]].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
        slice[src[i]].slice_flags &= ~(BCMPTM_SLICE_MODE_SINGLE
                                                | BCMPTM_SLICE_MODE_DOUBLE
                                                | BCMPTM_SLICE_MODE_TRIPLE);
        slice[src[i]].free_entries = slice[src[i]].num_entries;
        slice[src[i]].free_presel_entries = slice[src[i]].num_presel_entries;
        /* Adjust slice offsets */
        for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
            mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
            mode++) {
            slice[src[i]].next_slice_id_offset[mode] = 0;
            slice[src[i]].prev_slice_id_offset[mode] = 0;
            slice[src[i]].left_slice_id_offset[mode] = 0;
            slice[src[i]].right_slice_id_offset[mode] = 0;
        }
        /*
         * Clear the source slice in this order.
         * First cleat the presel
         * Clear the FP profiles
         * Clear the main TCAM
         */
        if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_tcam_presel_entries_clear(unit,
                                                             iomd,
                                                             &pri_entry_attr,
                                                             src[i]));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_profiles_clear(unit,
                                              iomd,
                                              src[i]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_tcam_entries_clear(unit,
                                            iomd,
                                            src[i],
                                            &pri_entry_attr));
    }
    iomd->entry_attrs = entry_attr_temp;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief This function creates the slice map for all the slices
 *\n for the stage.slice map contains all the details regarding
 *\n slices and bundles. This data base will be used for further
 *\n process in the algorithm.
 *
 * \param [in] unit BCM device number.
 * \param [in] stage_fc stage structure
 * \param [in] slice slice array for the slices
 * \param [in] slice_map The computed slice related mapping data
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_INTERNAL No of slices or bundles more than array
 */

static int
bcmptm_rm_tcam_fp_find_slice_mapping(int unit,
                                     bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                     bcmptm_rm_tcam_slice_t *slice,
                                     slice_mapping_t *slice_map)
{
    int slice_id;
    int count = 0;
    int bundle_index;
    int bundle;
    int max_double_count;
    int max_triple_count;
    SHR_FUNC_ENTER(unit);
    sal_memset(slice_map,0,sizeof(slice_mapping_t));

    slice_map->num_slices = stage_fc->num_slices;
    slice_map->num_bundles = slice_map->num_slices/stage_fc->slice_bundle_size;
    if ((slice_map->num_slices > MAX_NUM_SLICES) ||
        (slice_map->num_bundles > MAX_NO_OF_BUNDLES)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    max_double_count = stage_fc->slice_bundle_size/2;
    max_triple_count = stage_fc->slice_bundle_size/3;

    for (slice_id = 0; slice_id < slice_map->num_slices; slice_id++) {
        bundle = slice_id / stage_fc->slice_bundle_size;
        bundle_index = slice_id % stage_fc->slice_bundle_size;
        slice_map->slice_size[slice_id] = slice[slice_id].num_entries;
        if (bundle_index == 0) {
            slice_map->move_eligible[bundle] = 1;
        }
        if (CHECK_SLICE_IS_FREE(slice,slice_id)) {
            slice_map->free_slice[count] = slice_id;
            count++;
        } else {
            if (CHECK_TRIPLE_PRESENT(slice,slice_id)) {
                slice_map->triple_map[bundle] |= (1 << bundle_index);
                slice_map->triple_count[bundle]++;
                if (slice_map->triple_count[bundle] >= max_triple_count) {
                    slice_map->move_eligible[bundle] = 0;
                }
                slice_map->total_triple_count++;
                slice_map->slice_size[slice_id + 1] =
                                slice[slice_id].num_entries;
                slice_map->slice_size[slice_id + 2] =
                                slice[slice_id].num_entries;
                slice_id += 2;
            } else if (CHECK_DOUBLE_PRESENT(slice,slice_id)) {
                slice_map->double_map[bundle] |= (1 << bundle_index);
                slice_map->double_count[bundle]++;
                if (slice_map->double_count[bundle] >= max_double_count) {
                    slice_map->move_eligible[bundle] = 0;
                }
                slice_map->slice_size[slice_id + 1] =
                                slice[slice_id].num_entries;
                slice_map->total_double_count++;
                slice_id++;
            } else {
                slice_map->single_map[bundle] |= (1 << bundle_index);
            }
        }
    }
    slice_map->free_slice_count = count;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function checks whether there are enough free slices
 *\n which are same or higher size as the current bundle slice size
 *\n which is selected for move
 *
 * \param [in] unit BCM device number.
 * \param [in] slice_map The computed slice related mapping data
 * \param [in] slice_count The count of number of slices needed
 * \param [in] primary primary slice selected in candidate bundle
 * \param [in] num_moves number of moves needed
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_RESOURCE Resource error
 */

static int
bcmptm_rm_tcam_fp_check_cand_bund_size(int unit,
                                       slice_mapping_t *slice_map,
                                       uint32_t slicecount,
                                       uint32_t primary,
                                       uint32_t num_moves)
{
    int free_index =0;
    uint32_t slice_id;
    uint32_t pos_moves = 0;

    SHR_FUNC_ENTER(unit);
    for (free_index = 0; free_index < slice_map->free_slice_count;
        free_index++) {
        slice_id = slice_map->free_slice[free_index];
        if (!(SLICE_PART_OF_CURRENT_GROUP(slice_id, primary, slicecount))) {
            if (slice_map->slice_size[slice_id] >=
                    (slice_map->slice_size[primary])) {
                pos_moves++;
            }
        }
    }
    if (pos_moves >= num_moves) {
        SHR_VERBOSE_EXIT(SHR_E_NONE);
    }
    SHR_VERBOSE_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function iterates through the slice map
 *\n and find out the candidate bundle where we can create
 *\n the new group. It go through all bundles and select the bundle
 *\n which can be freed for new group with minimum set of moves
 *
 * \param [in] unit BCM device number.
 * \param [in] slice slice database array
 * \param [in] slice_map The computed slice related mapping data
 * \param [in] slice_count count of number of slices needed.
 * \param [in] slice_mode slice_mode to check the slice capability
 * \param [in] primary primary slice in the bundle which can be freed
 * \param [in] c_bundle candidate bundle which is selected
 *
 * \return SHR_E_NONE Success
 */

static int
bcmptm_rm_tcam_fp_find_candidate_bundle(int unit,
                                        bcmptm_rm_tcam_slice_t *slice,
                                        slice_mapping_t *slice_map,
                                        int slice_count,
                                        int slice_mode,
                                        uint32_t *primary,
                                        int *c_bundle)
{
    int candidate_bundle = INVALID_BUNDLE;
    int candidate_primary = INVALID_SLICE_ID;
    int candidate_bundle_slice_count;
    int bundle_index;
    int rv;
    int slice_index;
    int curr_bundle_slice_count;
    int bundle_slice_count;
    int sliceid;
    int curr_count;
    int curr_primary;
    SHR_FUNC_ENTER(unit);

    bundle_slice_count = candidate_bundle_slice_count =
                            slice_map->num_slices/slice_map->num_bundles;
    for (bundle_index = 0;
             bundle_index <= slice_map->num_bundles; bundle_index++) {
        curr_bundle_slice_count = 0;
        curr_primary = INVALID_SLICE_ID;
        curr_count = 0;
        if (slice_map->move_eligible[bundle_index] == 1) {
            for (slice_index = 0;
                slice_index < bundle_slice_count; slice_index++) {
                sliceid = bundle_slice_count * bundle_index + slice_index;
                if (CHECK_SLICE_IS_FREE(slice,sliceid) ||
                   (slice_map->single_map[bundle_index]
                                & (1 << slice_index))) {
                    if (curr_primary == INVALID_SLICE_ID) {
                        if (slice[sliceid].slice_flags & slice_mode) {
                            curr_primary = sliceid;
                        }
                    }
                    if (curr_primary != INVALID_SLICE_ID) {
                        curr_count++;
                        if (slice_map->single_map[bundle_index]
                                            & (1 << slice_index)) {
                            curr_bundle_slice_count++;
                        }
                        if (curr_count == slice_count) {
                            break;
                        }
                    }
                }
                else {
                    curr_count = 0;
                    curr_bundle_slice_count = 0;
                    curr_primary = INVALID_SLICE_ID;
                }
            }
        }
        if (curr_count >= slice_count) {
            rv = bcmptm_rm_tcam_fp_check_cand_bund_size(unit,
                                                    slice_map,
                                                    slice_count,
                                                    curr_primary,
                                                    curr_bundle_slice_count);
            if ((curr_bundle_slice_count <= candidate_bundle_slice_count) &&
                   (rv == SHR_E_NONE)){
                candidate_bundle = bundle_index;
                candidate_bundle_slice_count = curr_bundle_slice_count;
                candidate_primary = curr_primary;
            }
        }
    }
    *primary = candidate_primary;
    *c_bundle = candidate_bundle;
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function is used to check whether there are any
 *\n eligible bundles in the slice_map. If there are no
 *\n eligible bundles function will return resource error
 *
 * \param unit BCM device number.
 * \param slice_map The computed slice related mapping data
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_RESOURCE No eligible bundle found
 */

static int
bcmptm_rm_tcam_fp_check_move_eligible(int unit,
                                      slice_mapping_t *slice_map)
{
    int bundle_index;
    SHR_FUNC_ENTER(unit);
    for (bundle_index = 0;
        bundle_index < slice_map->num_bundles; bundle_index++) {
        if (slice_map->move_eligible[bundle_index] == 1) {
            SHR_VERBOSE_EXIT(SHR_E_NONE);
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function is called from create free slice to recursively
 *\n move lower sized groups to other slices to create space.
 *\n Existing slices are re-arranged to create space.
 *
 * \param [in] unit BCM device number.
 * \param [in] slice slice strucutre array pointer
 * \param [in] slice_map The computed slice related mapping data
 * \param [in] num_mov No of moves based on number of source slices
 * \param [in] bundle Target bundle which need to be freed
 * \param [in] num_mov num of moves needed to create space for new group.
 * \param [in] primary Primary slice if allocated.
 * \param [in] slicecount slice count needed for the group.
 * \param [out] dst destination slice list
 * Returns:
 *    SHR_E_XXX
 */


static int
bcmptm_rm_tcam_fp_fill_dest_slice(int unit,
                                  bcmptm_rm_tcam_slice_t *slice,
                                  slice_mapping_t *slice_map,
                                  int bundle,
                                  int num_mov,
                                  int primary,
                                  int slicecount,
                                  int *dst)
{
    int free_index;
    int dst_index = 0;
    int slice_id;

    /* Log the function entry.*/
    SHR_FUNC_ENTER(unit);

    for (free_index = 0;
        free_index < slice_map->free_slice_count; free_index++) {
        slice_id = slice_map->free_slice[free_index];
        if (!(SLICE_PART_OF_CURRENT_GROUP(slice_id, primary, slicecount))) {
            if (slice_map->slice_size[slice_id] >=
                    slice_map->slice_size[primary]) {
                dst[dst_index++] = slice_id;
                if (dst_index == num_mov) {
                    break;
                }
            }
        }
    }
    if (dst_index != num_mov) {
        SHR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function is called from create free slice to find
 *\n the source and destination slice array for moving the slice
 *\n The array is calcuated based on target bundle and free slices
 *\n Src is computed by moving required number of slices, and destination
 *\n is computed by going over free slice list
 *
 * \param [in] unit BCM device number.
 * \param [in] slice slice strucutre array pointer
 * \param [in] bundle Target bundle which need to be freed
 * \param [in] req_slice_count No of slices that should be allocated for multimode.
 * \param [in] slice_map The computed slice related mapping data
 * \param [in] primary Primary slice allocated.
 * \param [out] src Src slice list
 * \param [out] dst destination slice list
 * \param [out] num_mov num of moves needed to create space for new group.
 *
 * \return SHR_E_NONE
 */

static int
bcmptm_rm_tcam_fp_move_entries(int unit,
                               bcmptm_rm_tcam_slice_t *slice,
                               int bundle,
                               int req_slice_count,
                               slice_mapping_t *slice_map,
                               int primary,
                               int *src,
                               int *dst,
                               int *num_mov)
{
    int slice_count;
    int src_index = 0;
    int slice_index;
    int slice_id;
    int c_slice_count = 0;
    int primary_bund = 0;

    SHR_FUNC_ENTER(unit);

    slice_count = slice_map->num_slices / slice_map->num_bundles;
    primary_bund = primary % slice_count;
    for (slice_index = primary_bund;
        slice_index < req_slice_count; slice_index++) {
        slice_id = slice_count * bundle + slice_index;
        if (slice_map->single_map[bundle] & (1 << slice_index)) {
            src[src_index++] = slice_id;
        }
        c_slice_count++;
        if (c_slice_count == req_slice_count) {
            break;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_fill_dest_slice(unit,
                                           slice,
                                           slice_map,
                                           bundle,
                                           src_index,
                                           primary,
                                           req_slice_count,
                                           dst));
    *num_mov = src_index;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function is called from create free slice to recursively
 *\n move lower sized groups to other slices to create space.
 *\n Existing slices are re-arranged to create space.
 *
 * \praram [in] unit BCM device number.
 * \praram [in] stage_fc stage structure pointer
 * \praram [in] slice slice strucutre array pointer
 * \praram [in] slice_count No of slices needed for width expanded slice
 * \praram [in] req_slice_count No of slices that should be allocated for multimode.
 * \praram [out] primary Primary slice if allocated.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_INTERNAL Function called for unsupported mode
 * \return SHR_E_RESOURCE Resource Error
 */

static int
bcmptm_rm_tcam_fp_create_space(int unit,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               bcmptm_rm_tcam_slice_t *slice,
                               uint32_t slice_count,
                               uint32_t req_slice_count,
                               uint32_t *primary)
{
    slice_mapping_t slice_map;
    int candidate_bundle = INVALID_BUNDLE;
    int mode_check;
    uint32_t primary_slice;

     SHR_FUNC_ENTER(unit);

    if (slice_count == 2) {
        mode_check = BCMPTM_SLICE_MODE_DOUBLE_CAPABLE;
    } else if (slice_count == 3) {
        mode_check = BCMPTM_SLICE_MODE_TRIPLE_CAPABLE;
    } else {
        SHR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    /*
     *   x x x x
     *   x x x x
     *   x x x x
     *   x x x
     *   x x x
     *   x x x
     *   x x x
     *   find all single wide occupied slices per row.
     *   find number of free slices per row.
     *   find the free slice mapping.
     *   find bundles where only (single wide + free) meets the number of
     *   slices required for the new group with primary having right mode
     *   ( potential candidates for movement)
     *   find the bundle which can create space with minimum number of moves.
     *   Count the free + single wide and the one with lowest number of single
     *   wide to be moved is the candidate
     *   Move the single slices in candidate slice to the free slice
     *   in other bundles.
     *   If the required number of slices is not moved,
     *      Move single wide within the bundle starting from one side.
     *      if no candidate bundle found.
     *      This means that there is no slice where we can find a slice with
     *      required number of free slices. Only way this can happen is because
     *      there is slice - 1 size occupying in every slice.
     *      recursively call the function with number of slices less by 1.
     *      Move the sliceno - 1 size group in newly created space.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_find_slice_mapping(unit,
                                              stage_fc,
                                              slice,
                                              &slice_map));

    if (slice_map.free_slice_count < slice_count) {
        SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_check_move_eligible(unit,&slice_map));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_find_candidate_bundle(unit,
                                                 slice,
                                                 &slice_map,
                                                 slice_count,
                                                 mode_check,
                                                 &primary_slice,
                                                 &candidate_bundle));
    if (candidate_bundle == INVALID_BUNDLE) {
        /*
         * In the Initial phase we are only supporting moving
         * single wide entries to create space. We are not enabling
         * the cases where double wide groups need to be moved around
         * followed by single groups to create space ( Mutli step space
         * creation  Will revist this if there is a need. Using a static
         * flag to disable untested code.
         */
        if (bcmptm_rm_tcam_fp_create_space(unit,
                                           stage_fc,
                                           slice,
                                           slice_count - 1,
                                           slice_count - 1,
                                           &primary_slice)
                                           != SHR_E_NONE) {
            SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_move_entries_bundle(unit,
                                                       slice,
                                                       slice_count - 1,
                                                       &slice_map,
                                                       primary_slice));
            SHR_VERBOSE_EXIT(SHR_E_NONE);
        }
    } else {
        if (primary != NULL) {
            *primary = primary_slice;
        }
        SHR_VERBOSE_EXIT(SHR_E_NONE);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function is called from slice allocate to
 *\n create space for a width expanded group by re-arranging the
 *\n current groups. width expanded groups needs consecutive slices
 *\n Existing slices are re-arranged to create space.
 *
 * \param [in] unit BCM device number.
 * \param [in] iomd input output meta data
 * \param [in] slice_count No of slices needed for width expanded slice
 * \param [in] req_slice_count No of slices that should be allocated for multimode.
 * \param [out] primary_slice Primary slice if allocated.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_RESOURCE Resource Error
 */

static int
bcmptm_rm_tcam_fp_create_free_slice(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    uint32_t slice_count,
                                    uint32_t req_slice_count,
                                    uint8_t *primary_slice)
{
    slice_mapping_t slice_map;
    int candidate_bundle = INVALID_BUNDLE;
    int num_moves = 0;
    int move_src[8];
    int move_dst[8];
    int mode_check;
    uint32_t primary;

     SHR_FUNC_ENTER(unit);

    if (slice_count == 2) {
        mode_check = BCMPTM_SLICE_MODE_DOUBLE_CAPABLE;
    } else if (slice_count == 3) {
        mode_check = BCMPTM_SLICE_MODE_TRIPLE_CAPABLE;
    } else {
        SHR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (non_single_move) {
        /*
         * This is currently unused. This function is used to create
         * space by moving multiwide entries to create space for a
         * wider entry
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_create_space(unit,
                                           iomd->stage_fc,
                                           iomd->slice_fc,
                                           slice_count,
                                           req_slice_count,
                                           NULL));
    }
    /*
     * Find the slice mapping data base. This will populate the
     * slice_map structure with all the information about the
     * bundles and occupied slices, free slices. Width expansion of the
     * used slices is also marked in the structure. This structure is
     * passed to further function for processing
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_find_slice_mapping(unit,
                                              iomd->stage_fc,
                                              iomd->slice_fc,
                                              &slice_map));
    /*
     * Check whether there are any bundles which are eligible for move
     * if not return resource error
    */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_check_move_eligible(unit, &slice_map));

    /*
     * This function will find the candidate slice.candidate slice
     * is the slice which is eligible for move based on size and use of
     * slices and which can be freed by minimum number of moves
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_find_candidate_bundle(unit,
                                                 iomd->slice_fc,
                                                 &slice_map,
                                                 slice_count,
                                                 mode_check,
                                                 &primary,
                                                 &candidate_bundle));
    if (candidate_bundle == INVALID_BUNDLE) {
        SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    /*
     * This function uses candidate bundle and slice_map as inputs
     * and figures out src and dst slice array and number of moves
     * needed.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_move_entries(unit,
                                        iomd->slice_fc,
                                        candidate_bundle,
                                        req_slice_count,
                                        &slice_map,
                                        primary,
                                        move_src,
                                        move_dst,
                                        &num_moves));
    /*
     * This function moves the single wide slices from the
     * src to destination identified in previous call
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_move_single_slice(unit,
                                             iomd,
                                             move_src,
                                             move_dst,
                                             num_moves));
    *primary_slice = (uint8_t)primary;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_grp_reserve_entry_clear(int unit,
                                          bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t local_idx = 0;
    int part;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->reserved_index == -1) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  iomd->reserved_index,
                                                  entry_info_ptr,
                                                  &local_idx));
    /*
     * If the group is multi wide group, clear reserve entry in all
     * width expanded slices.
     */
    for (part = 0; part < iomd->num_parts; part++) {

        entry_info = entry_info_ptr[part];

        if ((entry_info[local_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) &&
            (entry_info[local_idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) &&
            (entry_info[local_idx].entry_type == iomd->entry_type)) {
            entry_info[local_idx].entry_type
                        = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
        }
    }
    iomd->reserved_index = -1;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_grp_reserve_entry(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    int *reserve_index)
{
    uint32_t idx;/* Slice entry iterator. */
    uint32_t entry_info_idx;
    uint32_t end_idx = 0;
    uint32_t total_entry_info_depth = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t local_idx = 0;
    int count;
    int part;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    *reserve_index = -1;

    /* Find the total depth of this group */
    for (entry_info_idx = 0;
        entry_info_idx < iomd->entry_info_arr.num_depth_parts;
        entry_info_idx++) {
        total_entry_info_depth +=
            iomd->entry_info_arr.entry_info_depth[entry_info_idx];
    }

    end_idx = total_entry_info_depth;
    for (idx = 0; idx < end_idx; idx++) {
        /* Find an empty slot. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                              iomd,
                                              idx,
                                              entry_info_ptr,
                                              &local_idx));
        count = 0;
        /*
         * If the group is multi wide group, find a empty slot in all
         * width expanded slices.
         */
        for (part = 0; part < iomd->num_parts; part++) {

            entry_info = entry_info_ptr[part];

            if ((entry_info[local_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) &&
                (entry_info[local_idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) &&
                (entry_info[local_idx].entry_type
                            == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID)) {

                count++;
                /* Reserve index is found. Update entry_type in all parts */
                if (count == iomd->num_parts) {
                    for (part = 0; part < iomd->num_parts; part++) {
                        entry_info = entry_info_ptr[part];
                        entry_info[local_idx].entry_type = iomd->entry_type;
                    }
                    *reserve_index = idx;
                    break;
                }
            }
        }
        if (*reserve_index != -1) {
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_single_slice
 * Purpose:
 *     Validate and return a free slice if it fits for
 *     Single or Intra Slice Double wide mode group.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     iomd   - (IN) Input and Output metadata
 *     slice  - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_reserve_single_slice(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t *slice,
                                bool validate)
{
    uint8_t slice_idx = 0;
    bool found = FALSE;
    shr_error_t rv = SHR_E_NONE;
    bcmptm_rm_tcam_group_mode_t group_mode;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bool half_mode_supported = FALSE;
    const bcmptm_rm_tcam_slice_info_t *slice_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(slice, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    group_mode = iomd->entry_attrs->group_mode;
    slice_fc = iomd->slice_fc;

    /*
     * Loop through the slice segment and return the slice
     * number to use
     */
    slice_info = iomd->ltid_info->rm_more_info[0].slice_info;
    for (slice_idx = 0; slice_idx < iomd->num_slices; slice_idx++) {
        if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA) &&
            (slice_info[slice_idx].isdw_no_support == 1)) {
            slice_fc = slice_fc + 1;
            continue;
        }
        /* check the in USE flag of slice */
        if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
            if (!validate) {
                rv = bcmptm_rm_tcam_fp_profiles_reserve(unit,
                                                        iomd,
                                                        slice_idx,
                                                        1);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
            }
            if (SHR_SUCCESS(rv)) {
                if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                    BCMPTM_FP_FLAGS_USE_SMALL_SLICE))
                    && (slice_fc->slice_flags & BCMPTM_SLICE_TYPE_SMALL)) {
                    *slice = slice_idx;
                    found = TRUE;
                    break;
                } else if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                    BCMPTM_FP_FLAGS_USE_LARGE_SLICE))
                    && (slice_fc->slice_flags & BCMPTM_SLICE_TYPE_LARGE)) {
                    *slice = slice_idx;
                    found = TRUE;
                    break;
                } else if ((!(SHR_BITGET(iomd->entry_attrs->flags.w,
                   BCMPTM_FP_FLAGS_USE_SMALL_SLICE))) &&
                   (!(SHR_BITGET(iomd->entry_attrs->flags.w,
                   BCMPTM_FP_FLAGS_USE_LARGE_SLICE)))) {
                   *slice = slice_idx;
                   found = TRUE;
                   break;
                }
            }
        }
        slice_fc = slice_fc + 1;
    }
    /* If no free slice is found,
     * return E_RESOURCE error
     */
    if (found == FALSE) {
        /* In auto expansion, if requested size slice is not available,
         * fall back to normal slice allocation.
         */
        if (SHR_BITGET(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING)) {
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE);
            }
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE);
            }
            SHR_BITCLR(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_reserve_single_slice(unit,
                                                              iomd,
                                                              slice,
                                                              FALSE));
            SHR_EXIT();

        }
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    half_mode_supported =
        bcmptm_rm_tcam_fp_half_mode_supported(unit, iomd->ltid_info);
    if (half_mode_supported == TRUE) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }

    /*
     * Mark the slice flag to in use correct the number of entries
     * in slice if needed
     */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_fc->slice_flags |= BCMPTM_SLICE_MODE_SINGLE;
    if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }
    /*
     * No. of entries in half mode is double count from slice read
     * Base key width of single and half mode will have double the
     * number of entries in single wide with half width.
     */
    if (BCMPTM_RM_TCAM_GRP_MODE_HALF == group_mode) {
        slice_fc->num_entries = slice_fc->num_entries << 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_double_slice
 * Purpose:
 *     Validate and return primary slice idx if it can be
 *     used Double wide mode group.
 * Parameters:
 *     unit   - (IN  BCM device number.
 *     iomd   - (IN) Input and Output metadata
 *     slice  - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_reserve_double_slice(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t *slice,
                                bool validate)

{
    uint8_t slice_idx = 0;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bool found = FALSE;
    uint8_t slice_bundle_size = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmptm_rm_tcam_group_mode_t group_mode;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    const bcmptm_rm_tcam_slice_info_t *slice_info = NULL;
    bool half_mode_supported = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(slice, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;
    slice_bundle_size = stage_fc->slice_bundle_size;
    group_mode = iomd->entry_attrs->group_mode;
    slice_info = iomd->ltid_info->rm_more_info[0].slice_info;

    /*
     * Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < (iomd->num_slices - 1); slice_idx++) {
        /*
         * Double wide group is created using consecutive slices within
         * a memory macro.But consecutive slices across a memory macro
         * boundary cannot be paried to create a double wide group.
         * Skip slices 2, 5, 8 and 11 from allocating for Double Wide
         * Group base slice.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 2) {
            /*
             * If intra double wide group must start at even slice ids
             * starting from 0.
             */
            if ((!(stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) ||
                stage_fc->flags & BCMPTM_STAGE_DBLINTER_SLICE_MODULO_2) &&
                (slice_idx % 2)) {
                slice_fc = slice_fc + 1;
                continue;
            }
            if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) &&
                (slice_info[slice_idx].isdw_no_support == 1)) {
                slice_fc = slice_fc + 1;
                continue;
            }

            /* Check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* Check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL &&
                   (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    if (!validate) {
                       rv = bcmptm_rm_tcam_fp_profiles_reserve(unit,
                                                               iomd,
                                                               slice_idx,
                                                               2);
                       SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
                   }
                   if (SHR_SUCCESS(rv)) {
                       if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) &&
                           (slice_fc->slice_flags &
                            BCMPTM_SLICE_TYPE_SMALL)) {
                           *slice = slice_idx;
                           found = TRUE;
                           break;
                       } else if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE)) &&
                           (slice_fc->slice_flags &
                           BCMPTM_SLICE_TYPE_LARGE)) {
                           *slice = slice_idx;
                           found = TRUE;
                           break;
                       } else if ((!(SHR_BITGET(iomd->entry_attrs->flags.w,
                          BCMPTM_FP_FLAGS_USE_SMALL_SLICE))) &&
                          (!(SHR_BITGET(iomd->entry_attrs->flags.w,
                          BCMPTM_FP_FLAGS_USE_LARGE_SLICE)))) {
                          *slice = slice_idx;
                          found = TRUE;
                          break;
                       }
                   }
                }
            }
        }
        slice_fc = slice_fc + 1;
    }

    /* If no free slice is found, return E_RESOURCE error */
    if (found == FALSE) {
        /*
         * In auto expansion, if requested size slice is not available,
         * fall back to normal slice allocation.
         */
        if (SHR_BITGET(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING)) {
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE);
            }
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE);
            }
            SHR_BITCLR(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_reserve_double_slice(unit,
                                                              iomd,
                                                              slice,
                                                              FALSE));
            SHR_EXIT();

        }
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    half_mode_supported =
        bcmptm_rm_tcam_fp_half_mode_supported(unit, iomd->ltid_info);
    if (half_mode_supported == TRUE) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
        slice_next->num_entries = slice_next->num_entries >> 1;
    }

    /*
     * Mark the slice flag to in use correct the number of entries
     * in slice.
     */
    /* primary slice */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_fc->slice_flags |= BCMPTM_SLICE_MODE_DOUBLE;

    if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) ||
        (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA)) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }
    slice_fc->right_slice_id_offset[group_mode] = 1;

    /* secondary slice */
    slice_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_next->slice_flags |= BCMPTM_SLICE_MODE_DOUBLE;

    if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) ||
        (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA)) {
        slice_next->num_entries = slice_next->num_entries >> 1;
    }
    slice_next->left_slice_id_offset[group_mode] = -1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_triple_slice
 * Purpose:
 *     Validate and return primary slice idx if it can be
 *     used Double wide mode group.
 * Parameters:
 *     unit   - (IN  BCM device number.
 *     iomd   - (IN) Input and Output metadata
 *     slice  - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_reserve_triple_slice(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t *slice,
                                bool validate)
{
    uint8_t slice_idx = 0;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bcmptm_rm_tcam_slice_t *slice_next_next = NULL;
    bool found = FALSE;
    uint8_t slice_bundle_size = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmptm_rm_tcam_group_mode_t group_mode;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bool half_mode_supported = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(slice, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;
    group_mode = iomd->entry_attrs->group_mode;
    slice_bundle_size = stage_fc->slice_bundle_size;

    /*
     * Loop through the slice segment and return the slice
     * number to use
     */
    /*
     * As we need 3 contiguous slices, we can stop the loop at
     * Max slices - 2 as after this there can never exist 3
     * contiguous slices
     */
    for (slice_idx = 0; slice_idx < (iomd->num_slices - 2); slice_idx++) {
        /*
         * Triple wide group is created using consecutive slices
         * within a memory macro.But consecutive slices across a
         * memory macro boundary cannot be paried to create a
         * Triple wide group.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 3) {
            /* If triple wide group must start slice id divisible by 4. */
            if (stage_fc->flags & BCMPTM_STAGE_TRIPLE_SLICE_MODULO_4 &&
               (slice_idx % 4)) {
                slice_fc = slice_fc + 1;
                continue;
            }

            /* Check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* Check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL &&
                   (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    /* Check whether the next slice is free */
                    slice_next_next = slice_next + 1;
                    if (slice_next_next != NULL &&
                       (!(slice_next_next->slice_flags &
                          BCMPTM_SLICE_IN_USE))) {
                        if (!validate) {
                            rv = bcmptm_rm_tcam_fp_profiles_reserve(unit,
                                                                    iomd,
                                                                    slice_idx,
                                                                    3);
                            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
                       }
                       if (SHR_SUCCESS(rv)) {
                           if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                               BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) &&
                               (slice_fc->slice_flags &
                               BCMPTM_SLICE_TYPE_SMALL)) {
                               *slice = slice_idx;
                               found = TRUE;
                               break;
                           } else if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                               BCMPTM_FP_FLAGS_USE_LARGE_SLICE)) &&
                               (slice_fc->slice_flags &
                               BCMPTM_SLICE_TYPE_LARGE)) {
                               *slice = slice_idx;
                               found = TRUE;
                               break;
                           } else if ((!(SHR_BITGET(iomd->entry_attrs->flags.w,
                              BCMPTM_FP_FLAGS_USE_SMALL_SLICE)))
                              && (!(SHR_BITGET(iomd->entry_attrs->flags.w,
                              BCMPTM_FP_FLAGS_USE_LARGE_SLICE)))) {
                              *slice = slice_idx;
                              found = TRUE;
                              break;
                           }
                       }
                    }
                }
            }
        }
        slice_fc = slice_fc + 1;
    }
    /* If no free slice is found, return E_RESOURCE error */
    if (found == FALSE) {
        /*
         * In auto expansion, if requested size slice is not available,
         * fall back to normal slice allocation.
         */
        if (SHR_BITGET(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING)) {
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_SMALL_SLICE);
            }
            if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE)) {
                SHR_BITCLR(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_USE_LARGE_SLICE);
            }
            SHR_BITCLR(iomd->entry_attrs->flags.w,
                       BCMPTM_FP_FLAGS_AUTO_EXPANDING);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_reserve_triple_slice(unit,
                                                              iomd,
                                                              slice,
                                                              FALSE));
            SHR_EXIT();

        }
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    /*
     * Mark the slice flag to in use correct the number of
     * entries in slice
     */
    /* primary slice */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_fc->slice_flags |= BCMPTM_SLICE_MODE_TRIPLE;
    slice_fc->right_slice_id_offset[group_mode] = 1;

    /* Secondary slice */
    slice_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_next->slice_flags |= BCMPTM_SLICE_MODE_TRIPLE;
    slice_next->left_slice_id_offset[group_mode] = -1;
    slice_next->right_slice_id_offset[group_mode] = 1;

    /* Third slice */
    slice_next_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    slice_next_next->slice_flags |= BCMPTM_SLICE_MODE_TRIPLE;

    slice_next_next->left_slice_id_offset[group_mode] = -1;
    half_mode_supported =
        bcmptm_rm_tcam_fp_half_mode_supported(unit, iomd->ltid_info);
    if (half_mode_supported == TRUE) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
        slice_next->num_entries = slice_next->num_entries >> 1;
        slice_next_next->num_entries = slice_next_next->num_entries >> 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Validate and return primary slice idx based on group mode
 *
 * \param [in] unit BCM device unit number
 * \param [in] ltid Logical table ID.
 * \param [in] ltid_info Logical table ID device specific information.
 * \param [in] entry_attrs To be installed entry information passed
 *             from caller module.
 * \param [in] stage_fc Field stage structure.
 * \param [in] num_slices number of slices for this stage
 * \param [in] slice_fc  Field Slice structure. Update flags
 * \param [out] slice     Primary slice id.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_RESOURCE No slice availble to allocate
 */
static int
bcmptm_rm_tcam_fp_group_reserve_slice(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                                      uint8_t *slice)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_group_mode_t group_mode;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(slice, SHR_E_PARAM);

    slice_fc = iomd->slice_fc;
    stage_fc = iomd->stage_fc;

    group_mode = iomd->entry_attrs->group_mode;
    if (BCMPTM_RM_TCAM_GRP_MODE_HALF == group_mode ||
        BCMPTM_RM_TCAM_GRP_MODE_SINGLE == group_mode ||
        BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        /*
         * Return E_CONFIG if the requested group mode is
         * Intra-Slice Double and Slices are not Intra Slice
         * capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode &&
           !(slice_fc->slice_flags &
             BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_single_slice(unit,
                                                          iomd,
                                                          slice,
                                                          FALSE);
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == group_mode ||
              (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        /* Return E_CONFIG if the requested group mode is Quad
         * and Slices are not Intra Slice capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
           !(slice_fc->slice_flags & BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_double_slice(unit,
                                                          iomd,
                                                          slice,
                                                          FALSE);
    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_triple_slice(unit,
                                                          iomd,
                                                          slice,
                                                          FALSE);
    } else {
        rv = SHR_E_CONFIG;
    }

    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_check_slice_sharing
 * Purpose:
 *     Validate and return primary group id and slice_sharing flag
 *     if the newly created group can share the resources of
 *     existing group.
 * Parameters:
 *    unit           - (IN)  BCM device number.
 *    iomd           - (IN)  Input and Output metadata.
 *    grp_ptr        - (IN/OUT) Field group structure. Update flags
 *    pri_group_id   - (OUT) Primary group index
 *    slice_shared   - (OUT) Slice Shared TRUE/FALSE
 * Returns:
 *     SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_check_slice_sharing(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                                      bcmptm_rm_tcam_fp_group_t *grp_ptr,
                                      bool *slice_shared,
                                      int16_t *pri_group_id)
{
    uint32_t idx;
    bool sharing_feasible = FALSE;
    uint32_t num_presels_per_slice = 0;
    uint32_t total_presel_entries = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(grp_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_shared, SHR_E_PARAM);
    SHR_NULL_CHECK(pri_group_id, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               iomd->entry_attrs->pipe_id,
                                               0,
                                               &num_presels_per_slice));
    }

    sharing_feasible = 0;
    /* Over all presel entries cant go beyond 32 */
    /* if group has only default presel entry, tht group cant be shared. */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((grp_ptr[idx].valid == TRUE) &&
            (grp_ptr[idx].pipe_id == iomd->entry_attrs->pipe_id)) {
            /*
             * For EM no need to same priority groups. groups should belong
             * to same slice id. slice id is recevied from BCMFP,
             */
            if (((stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH &&
                grp_ptr[idx].primary_slice_id == iomd->entry_attrs->slice_id) ||
                (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) &&
                grp_ptr[idx].priority == iomd->entry_attrs->group_prio))) {

                if (grp_ptr[idx].flags & BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT
                    && stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
                if (grp_ptr[idx].flags & BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT) {
                    continue;
                }
                if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
                    if (grp_ptr[idx].group_ltid ==
                        iomd->entry_attrs->group_ltid &&
                        grp_ptr[idx].group_mode ==
                        iomd->entry_attrs->group_mode) {
                        /* Groups can be shared */
                        sharing_feasible = 1;
                        break;
                    }
                } else if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE &&
                    grp_ptr[idx].group_mode == iomd->entry_attrs->group_mode) {
                    /* groups can be shared */
                    sharing_feasible = 1;
                    break;
                }
                
                else {
                    /* number presel entries cant go beyond 32 */
                    total_presel_entries = grp_ptr[idx].num_presel_entries +
                                           iomd->entry_attrs->num_presel_ids;
                    if (total_presel_entries <= num_presels_per_slice) {
                        if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
                            /* No need of multimode checks for EM */
                            /* Groups can be shared */
                            sharing_feasible = 1;
                            break;
                        }
                        /*
                         * Sharing not possible if one is half mode
                         * and other is not half mode.
                         */
                        if ((iomd->entry_attrs->group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_HALF &&
                            grp_ptr[idx].group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_HALF) ||
                            (iomd->entry_attrs->group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_HALF &&
                            grp_ptr[idx].group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_HALF)) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "\nMulti Mode slice sharing "
                                "is not feasible with HALF mode group.\n"
                                "\nChange group mode to any one of SINGLE, "
                                "DBLINTER/TRIPLE OR\n"
                                "\nChange group priority to allocate "
                                "different slice\n")));
                            SHR_ERR_EXIT(SHR_E_CONFIG);
                        }
                        /*
                         * Sharing not possible if one is DBLINTRA mode
                         * and other is not DBLINTRA mode.
                         */
                        if ((iomd->entry_attrs->group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA &&
                            grp_ptr[idx].group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA) ||
                            (iomd->entry_attrs->group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA &&
                            grp_ptr[idx].group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA)) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "\nMulti Mode slice sharing "
                                "is not feasible with DBLINTRA mode group.\n"
                                "\nChange group mode to any one of SINGLE, "
                                "DBLINTER/TRIPLE OR\n"
                                "\nChange group priority to allocate "
                                "different slice\n")));
                            SHR_ERR_EXIT(SHR_E_CONFIG);
                        }
                        /*
                         * Sharing not possible if one is QUAD mode
                         * and other is not QUAD mode.
                         */
                        if ((iomd->entry_attrs->group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_QUAD &&
                            grp_ptr[idx].group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_QUAD) ||
                            (iomd->entry_attrs->group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_QUAD &&
                            grp_ptr[idx].group_mode !=
                            BCMPTM_RM_TCAM_GRP_MODE_QUAD)) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "\nMulti Mode slice sharing "
                                "is not feasible with DBLINTRA mode group.\n"
                                "\nChange group mode to any one of SINGLE, "
                                "DBLINTER/TRIPLE OR\n"
                                "\nChange group priority to allocate "
                                "different slice\n")));
                            SHR_ERR_EXIT(SHR_E_CONFIG);
                        }
                        /*
                         * Lower group mode group created first and
                         * new group is wider mode with same priority.
                         */
                        if (iomd->entry_attrs->group_mode >
                            grp_ptr[idx].group_mode) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "\nMulti Mode slice sharing "
                                "is feasible only if the widest mode group "
                                "entries are inserted first.\n"
                                "\nChange group priority to allocate "
                                "different set of slices\n")));
                            SHR_ERR_EXIT(SHR_E_CONFIG);
                        }

                        /* Groups can be shared */
                        sharing_feasible = 1;
                        break;
                    }
                }
            }
        }
    }
    if (sharing_feasible) {
        *slice_shared = TRUE;
        *pri_group_id =
             slice_fc[grp_ptr[idx].primary_slice_id].primary_grp_id;
    }
exit:
    SHR_FUNC_EXIT();
}

static int bcmptm_rm_tcam_update_shared_group(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                                           int16_t pri_group_id)
{
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    uint32_t idx;
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->entry_attrs->pipe_id,
                                          pri_group_id,
                                          iomd->stage_fc,
                                          &fg));
    while (fg->next_shared_group_id != -1) {
        idx = fg->next_shared_group_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->entry_attrs->pipe_id,
                                              idx,
                                              iomd->stage_fc,
                                              &fg));
    }
    fg->next_shared_group_id = iomd->entry_attrs->group_id;
exit:
    SHR_FUNC_EXIT();
}



/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_lt_priority_alloc
 * Purpose:
 *    Allocate logical table action priority for the group.
 * Parameters:
 *    unit        - (IN) BCM device number.
 *    lt_config   - (IN/OUT) update group's priority.
 *    num_groups  - (IN) number of groups per pipe
 *    fg_priority - (IN) group's input priority.
 *    lt_id       - (IN) Logical Table Identifier.
 * Returns:
 *    SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_lt_priority_alloc(int unit,
                                          bcmptm_rm_tcam_lt_config_t *lt_config,
                                          uint16_t num_groups,
                                          int fg_priority, int lt_id)
{
    int lt_idx;                                 /* Logical table iterator.   */
    bcmptm_rm_tcam_lt_config_t *lt_info = NULL;
                                                /* LT information.           */
    int action_prio = 0;                        /* LT action priority value. */
    int idx;                                    /* Iterator.                 */

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(lt_config, SHR_E_PARAM);

    /* make a copy of the lt_config structure to sort */
    SHR_ALLOC(lt_info, num_groups * sizeof(bcmptm_rm_tcam_lt_config_t),
                      "bcmptmRmtcamLtConfigInfo");

    for (idx =0; idx < num_groups; idx++){
        sal_memcpy(&lt_info[idx], &lt_config[idx],
                           sizeof(bcmptm_rm_tcam_lt_config_t));
    }
    /*
     * Initialize LT information for the new logical table ID, for it to be
     * in the correct location after the sort.
     */
    lt_info[lt_id].priority = fg_priority;
    lt_info[lt_id].lt_action_pri = 0;
    lt_info[lt_id].valid = TRUE;

    /* Sort the logical tables array based on group priority value. */
    sal_qsort(lt_info, num_groups, sizeof(bcmptm_rm_tcam_lt_config_t),
              bcmptm_rm_tcam_fp_lt_group_priority_compare);
    /*
     * Assign LT action priority based on the position of LT ID in the sorted
     * array.
     */
    action_prio = (num_groups - 1);
    for (idx = (num_groups - 1); idx >= 0; idx--) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_info[idx].lt_action_pri = action_prio--;
    }

    for (idx = 0; idx < num_groups; idx++) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_idx = lt_info[idx].lt_id;
        lt_config[lt_idx].lt_action_pri =
                    lt_info[idx].lt_action_pri;
    }

    SHR_FREE(lt_info);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_priority_set
 * Purpose:
 *    Update lt_config structure with group priority
 *    Also set's group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    pri_slice_idx  - (IN) Primary slice index
 *    entry_attr     - (IN) Entry attrs structure
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_priority_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t pri_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc,
                              bool slice_shared)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
    int parts_count = -1;                         /* Number of entry parts.   */
    int part;
    uint8_t slice, slice_prev, slice_number;      /* LT Slice number.         */
    int lt_part_prio = FP_MAX_LT_PART_PRIO;       /* LT part priority value.  */
    int lt_part_prio_sharing_group = FP_MAX_LT_PART_PRIO;
    uint32_t num_groups_per_pipe;
    int16_t pri_grp_id = -1;
    uint8_t  intraslice = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit,
                 ltid,
                 ltid_info,
                 entry_attr->pipe_id, stage_fc->stage_id,
                 &lt_config));

    /* Get number of groups per pipe */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info,
                          entry_attr->pipe_id,
                          &num_groups_per_pipe));

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* Allocate LT action priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_priority_alloc(unit,
                                   lt_config,
                                   (num_groups_per_pipe * stage_fc->num_pipes),
                                   entry_attr->group_prio,
                                   entry_attr->group_ltid));
        lt_config[entry_attr->group_ltid].valid = TRUE;
        lt_config[entry_attr->group_ltid].priority = entry_attr->group_prio;
        lt_config[entry_attr->group_ltid].group_ltid = entry_attr->group_ltid;
        lt_config[entry_attr->group_ltid].pipe_id = entry_attr->pipe_id;
    } else {
        /* Allocate LT action priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_priority_alloc(unit,
                                   lt_config,
                                   (stage_fc->num_groups * stage_fc->num_pipes),
                                   entry_attr->group_prio,
                                   entry_attr->group_id));
        lt_config[entry_attr->group_id].valid = TRUE;
        lt_config[entry_attr->group_id].priority = entry_attr->group_prio;
        lt_config[entry_attr->group_id].group_ltid = entry_attr->group_ltid;
        lt_config[entry_attr->group_id].pipe_id = entry_attr->pipe_id;
    }

    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        /* Get number of entry parts. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(
                                                   unit,
                                                   entry_attr->group_mode,
                                                   stage_fc->flags,
                                                   &parts_count));
        slice = pri_slice_idx;
        /* if the ressources are shared, get the primary_group_id */
        if (TRUE == slice_shared) {
            /* get the primary grp id from primary slice */
            pri_grp_id = slice_fc[slice].primary_grp_id;
            /* Initialize field group pointer. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  ltid,
                                                  ltid_info,
                                                  entry_attr->pipe_id,
                                                  pri_grp_id,
                                                  stage_fc,
                                                  &pri_fg));
        }

        if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
            (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
            intraslice = 1;
            parts_count = parts_count >> intraslice;
        }
        do
        {
            slice_prev = slice;
            /* Iterate over entry parts and program hardware for each slice. */
            for (part = 0; part < parts_count; part++) {

                /* Get slice number to which the entry part belongs to. */
                slice_number = slice + part;
                /* if groups are sharing resources, use the lt_pri from
                 * primary group
                 */
                if ((TRUE == slice_shared) && (pri_fg != NULL)) {
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        if (entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE
                            && pri_fg->group_mode != BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
                            if (slice_number == pri_slice_idx) {
                                lt_part_prio_sharing_group =
                                   lt_config[pri_fg->group_ltid].lt_part_pri[slice_number];
                                lt_config[entry_attr->group_ltid].lt_part_pri[slice_number] =
                                   lt_part_prio_sharing_group;

                                lt_part_prio_sharing_group--;
                            } else {
                                lt_config[entry_attr->group_ltid].lt_part_pri[slice_number] =
                                   lt_part_prio_sharing_group--;
                            }
                        } else {
                            lt_config[entry_attr->group_ltid].lt_part_pri[slice_number]
                                = lt_config[pri_fg->group_ltid].lt_part_pri[slice_number];
                        }
                    } else {
                        lt_config[entry_attr->group_id].lt_part_pri[slice_number]
                          = lt_config[pri_fg->group_id].lt_part_pri[slice_number];
                    }

                } else {

                    /* Set slice mode in hardware based on Group mode/flags. */
                    SHR_IF_ERR_VERBOSE_EXIT(
                        bcmptm_rm_tcam_fp_slice_enable_set(unit,
                                            ltid,
                                            ltid_info,
                                            entry_attr->pipe_id,
                                            entry_attr->stage_id,
                                            entry_attr->group_mode,
                                            slice_number, 1));
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        lt_config[entry_attr->group_ltid].lt_part_pri[slice_number]
                                                    = lt_part_prio;
                    } else {
                        lt_config[entry_attr->group_id].lt_part_pri[slice_number]
                                                    = lt_part_prio;
                    }
                }
                /*
                 * Initialize slice free entries count value to number of
                 * entries in the slice.
                 */
                if (-1 == slice_fc[slice_number].primary_grp_id) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             slice_number, entry_attr->pipe_id,
                                             entry_attr->group_mode,
                                             &num_entries_per_slice));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_presel_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             entry_attr->pipe_id,
                                             slice_number,
                                             &num_presels_per_slice));

                    slice_fc[slice_number].primary_grp_id =
                                   entry_attr->group_id;
                    slice_fc[slice_number].num_entries =
                                   num_entries_per_slice;
                    slice_fc[slice_number].free_entries =
                                   num_entries_per_slice;
                    slice_fc[slice_number].num_presel_entries =
                            num_presels_per_slice;
                    slice_fc[slice_number].free_presel_entries =
                            num_presels_per_slice;
                }

                /* Update slice LT_ID mapping in group's slice. */
                if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                    lt_config[entry_attr->group_ltid].lt_part_map
                      |= (1 << slice_number);
                } else {
                    lt_config[entry_attr->group_id].lt_part_map
                      |= (1 << slice_number);
                }
            }

            slice = slice + slice_fc[slice].next_slice_id_offset[entry_attr->group_mode];

        } while (slice != slice_prev);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_sindex_remap(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               void *entry_attr,
                               uint8_t slice_id,
                               uint32_t sindex,
                               uint32_t *remap_index)
{
    uint8_t idx = 0;
    uint32_t num_entries = 0;
    uint32_t slice_local_index = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_entry_attrs_t *fp_entry_attr = entry_attr;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(remap_index, SHR_E_PARAM);

    *remap_index = sindex;

    /* Accumulate the number of entries in all slices
     * till the target slice_id. */
    for (idx = 0; idx < slice_id; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                        unit, ltid, ltid_info,
                                        idx, fp_entry_attr->pipe_id, ref_grp_mode,
                                        &num_entries_per_slice));

            num_entries += num_entries_per_slice;
    }

    /* Get the number of entries in the target slice_id. */
    num_entries_per_slice = 0;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                   unit, ltid, ltid_info,
                                   slice_id, fp_entry_attr->pipe_id, ref_grp_mode,
                                   &num_entries_per_slice));
    if (num_entries) {
        slice_local_index = sindex % num_entries;
    } else {
        slice_local_index = sindex;
    }

    if (sindex % 2) {
       /* remap_index is addition of
        * (Sum of the entries in all slices till the targte slice),
        * (Index of the entry local to target slice),
        * (Number of entries in target slice divided by 2)
        */

       *remap_index = num_entries +
                      (slice_local_index / 2) +
                      (num_entries_per_slice / 2);
    } else {
       /* remap_index is addition of
        * (Sum of the entries in all slices till the targte slice),
        * (Index of the entry local to target slice)
        */
       *remap_index = num_entries +
                      (slice_local_index / 2);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_lt_part_prio_value_get
 * Purpose:
 *     Configure and return a LT Partition Priority
 *     for the given LT ID from a group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     lt_config    - (IN)  LT config Structure.
 *     fg_ltid      - (IN)  Field group LT id.
 *     num_slices   - (IN)  Number of slices
 *     lt_part_prio - (OUT) LT Partition priority value.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(int unit,
                                  bcmptm_rm_tcam_lt_config_t *lt_config,
                                  uint8_t fg_ltid, uint8_t num_slices,
                                  uint8_t *lt_part_prio)
{
    int part_prio = FP_MAX_LT_PART_PRIO;
    int rv = SHR_E_NONE;
    uint8_t slice;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(lt_part_prio, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(lt_config, SHR_E_PARAM);

    /* Parse the existing lt config for the group
     * and get the least lt part prio existing
     */
    for (slice = 0; slice < num_slices; slice++) {
        if (lt_config[fg_ltid].lt_part_pri[slice] != 0) {
            if (part_prio > lt_config[fg_ltid].lt_part_pri[slice]) {
                part_prio = lt_config[fg_ltid].lt_part_pri[slice];
            }
        }
    }

    /* Assign the least priority to the latest slice. */
    *lt_part_prio = part_prio - 1;
    if (*lt_part_prio <= 0) {
       rv = SHR_E_INTERNAL;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_partition_priority_set
 * Purpose:
 *    Update lt_config structure with partition priority
 *    Also set's group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    entry_attr          - (IN) primary group's Entry attrs structure
 *    new_slice_idx  - (IN) New slice index
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_partition_priority_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t new_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;                         /* Number of entry parts.   */
    int pri_parts_count = -1;                     /* Number of entry parts.   */
    int part;
    uint8_t slice_number;                         /* LT Slice number.         */
    uint8_t pri_slice_id;                         /* LT Slice number.         */
    uint8_t lt_part_prio = 0;                     /* LT part priority value.  */
    uint8_t num_slices;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    int8_t next_fg_id = -1;
    uint8_t num_groups = 0;
    uint8_t  intraslice = 0;
    uint8_t slice_id = 0;
    uint8_t new_slice_part = 0;
    uint8_t slice_id_prev = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                 entry_attr->pipe_id, stage_fc->stage_id,
                 &lt_config));
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* get number of slices for this pipe and stage */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_slices_count_get(unit, ltid,
                         ltid_info,
                         entry_attr->pipe_id,
                         &num_slices));
    /* calculate/get LT partition priority for new slice. */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(unit,
                              lt_config,
                              entry_attr->group_ltid,
                              num_slices, &lt_part_prio));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(unit,
                              lt_config,
                              entry_attr->group_id, num_slices, &lt_part_prio));
    }

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       entry_attr->group_mode, stage_fc->flags,
                                       &parts_count));

    if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
         || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->group_id,
                                          stage_fc,
                                          &fg));

    /* get the primary slice of the group */
    pri_slice_id = fg->primary_slice_id;

    /* get the first group id which is primary group sharing the slices */
    next_fg_id = slice_fc[pri_slice_id].primary_grp_id;

    do {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              ltid,
                                              ltid_info,
                                              entry_attr->pipe_id,
                                              next_fg_id,
                                              stage_fc,
                                              &fg));
        /* Get number of entry parts. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                           fg->group_mode, stage_fc->flags,
                                           &parts_count));

        if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
            (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
            (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
            intraslice = 1;
            parts_count = parts_count >> intraslice;
        }
        if (0 == num_groups) {
            pri_parts_count = parts_count;
        }
        if (entry_attr->group_mode != fg->group_mode &&
            fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
            parts_count = pri_parts_count;
        }

        /* if the group is already auto expanded,
         * go to the last slice and update the slice
         * offset to append the newly allocated slice/slices.
         *
         *   ----> parts
         *   |
         *   |
         *  depth (below is a representation of
         *         auto expanded triple wide group)
         *   ___     ___     ___
         *  |___|-->|___|-->|___|
         *    |       |       |
         *   _|_     _|_     _|_
         *  |_ _|-->|_ _|-->|_ _|
 */

         for (part = 0; part < parts_count; part++) {

            slice_id = fg->primary_slice_id + part;
            new_slice_part = new_slice_idx + part;
            do {
                /* go to the next slice if exists */
                slice_id_prev = slice_id;
                slice_id = slice_id +
                       slice_fc[slice_id].next_slice_id_offset[fg->group_mode];

            } while (slice_id < num_slices
                     && (slice_id_prev != slice_id));

            slice_fc[slice_id].next_slice_id_offset[fg->group_mode] =
                                 (new_slice_part - slice_id);

            slice_fc[new_slice_part].prev_slice_id_offset[fg->group_mode] =
                                 (slice_id - new_slice_part);

            if (fg->group_mode == entry_attr->group_mode) {
                /* update the right and left offset's too */
                slice_fc[new_slice_part].right_slice_id_offset[fg->group_mode] =
                slice_fc[new_slice_part].right_slice_id_offset[entry_attr->group_mode];
                slice_fc[new_slice_part].left_slice_id_offset[fg->group_mode] =
                slice_fc[new_slice_part].left_slice_id_offset[entry_attr->group_mode];
            } else if (parts_count < pri_parts_count) {
                /*double wide mode sharing with 3 wide mode */
                if (part == 0) {
                    slice_fc[new_slice_part].right_slice_id_offset[fg->group_mode] =
                    slice_fc[new_slice_part].right_slice_id_offset[entry_attr->group_mode];
                } else {
                    slice_fc[new_slice_part].left_slice_id_offset[fg->group_mode] =
                    slice_fc[new_slice_part].left_slice_id_offset[entry_attr->group_mode];
                }
            } else {
                /* Dont copy right and left offsets */
            }

            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {

                lt_config[fg->group_ltid].lt_part_map |= (1 << new_slice_part);

                lt_config[fg->group_ltid].lt_part_pri[new_slice_part]
                         = lt_config[fg->group_ltid].lt_part_pri[slice_id] - 1;
            } else {
                lt_config[fg->group_id].lt_part_map |= (1 << new_slice_part);

                lt_config[fg->group_id].lt_part_pri[new_slice_part]
                         = lt_config[fg->group_id].lt_part_pri[slice_id] - 1;
            }


            /* Get slice number to which the entry part belongs to. */
            slice_number = new_slice_part;

            if (0 == num_groups) {
                /* Set slice mode in hardware based on Group mode/flags. */
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_fp_slice_enable_set(unit, ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          fg->group_mode,
                                          slice_number, 1));
            }
            /*
             * Initialize the primary_group_id for the slice.
             * Initialize slice free entries count value to number of
             * entries in the slice.
             */
             if (-1 == slice_fc[slice_number].primary_grp_id) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             slice_number, entry_attr->pipe_id,
                                             entry_attr->group_mode,
                                             &num_entries_per_slice));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_presel_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             entry_attr->pipe_id,
                                             slice_number,
                                             &num_presels_per_slice));

                    slice_fc[slice_number].primary_grp_id =
                                   entry_attr->group_id;
                    slice_fc[slice_number].num_entries =
                                   num_entries_per_slice;
                    slice_fc[slice_number].free_entries =
                                   num_entries_per_slice;
                    slice_fc[slice_number].num_presel_entries =
                            num_presels_per_slice;
                    slice_fc[slice_number].free_presel_entries =
                            num_presels_per_slice;
            }
        }

        next_fg_id = fg->next_shared_group_id;

        /* Install LT partition Priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                           ltid, ltid_info, fg,
                                           entry_attr->pipe_id,
                                           entry_attr->stage_id));

        /* update the num entries in group structure */
        if (entry_attr->group_mode != fg->group_mode &&
                    fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
            fg->num_entries = fg->num_entries +
                        (slice_fc[new_slice_idx].num_entries) * pri_parts_count;
        } else {
            fg->num_entries = fg->num_entries +
                        (slice_fc[new_slice_idx].num_entries);
        }
        num_groups++;

    } while (next_fg_id != -1);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_partition_priority_reset
 * Purpose:
 *    Reset lt_config structure with partition priority
 *    Also Reset's group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    entry_attr     - (IN) primary group Entry attrs structure
 *    new_slice_idx  - (IN) New slice index
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_partition_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t new_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc,
                              uint8_t group_compress)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;      /* Number of entry parts.   */
    int part;
    uint8_t slice_number;      /* LT Slice number.         */
    uint8_t lt_part_prio = 0;  /* LT part priority value.  */
    uint8_t num_slices;
    int prev_slice_offset;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *pri_fg = NULL;
    uint8_t intraslice = 0;
    int32_t group_id = -1;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    mode = entry_attr->group_mode;

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                                        entry_attr->pipe_id,
                                        entry_attr->stage_id,
                                        &lt_config));
    /* get number of slices for this pipe and stage */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slices_count_get(unit, ltid, ltid_info,
                                        entry_attr->pipe_id,
                                        &num_slices));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &stage_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                        entry_attr->group_mode,
                                        stage_fc->flags,
                                        &parts_count));

    if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
         || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    /* Iterate over entry parts and program hardware for each slice. */
    for (part = 0; part < parts_count; part++) {

        /* Get slice number to which the entry part belongs to. */
        slice_number = new_slice_idx + part;

        /* to reset the num_entries in slice, we use the default value
         * of group_mode as 0 to get 512 entries
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             slice_number, entry_attr->pipe_id,
                                             ref_grp_mode,
                                             &num_entries_per_slice));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             entry_attr->pipe_id,
                                             slice_number,
                                             &num_presels_per_slice));
        /*
         * Initialize slice free entries count value to number of
         * entries in the slice.
         */
        if (slice_fc[slice_number].free_entries ==
                            slice_fc[slice_number].num_entries) {
            if (1 == group_compress) {
                group_id = slice_fc[slice_number].primary_grp_id;
                while (group_id != -1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                          ltid,
                                                          ltid_info,
                                                          entry_attr->pipe_id,
                                                          (uint32_t)group_id,
                                                          stage_fc,
                                                          &pri_fg));
                    /* decrease the group's num_entires by
                     * slice's num_entires.
                     */
                    if (part == 0) {

                        /* update the num entries in group structure */
                        if (entry_attr->group_mode != pri_fg->group_mode &&
                            pri_fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
                            pri_fg->num_entries = pri_fg->num_entries -
                            (slice_fc[new_slice_idx].num_entries) * parts_count;
                        } else {
                            pri_fg->num_entries = pri_fg->num_entries -
                                        (slice_fc[new_slice_idx].num_entries);
                        }
                    }
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        lt_config[pri_fg->group_ltid].lt_part_map
                                            &= ~(1 << slice_number);
                        lt_config[pri_fg->group_ltid].lt_part_pri[slice_number]
                                             = lt_part_prio;
                    } else {
                        lt_config[group_id].lt_part_map
                                            &= ~(1 << slice_number);
                        lt_config[group_id].lt_part_pri[slice_number]
                                             = lt_part_prio;
                    }
                    group_id = pri_fg->next_shared_group_id;
                }
            }
            /* Set slice mode in hardware based on Group mode/flags. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_slice_enable_set(unit, ltid, ltid_info,
                              entry_attr->pipe_id,
                              entry_attr->stage_id,
                              entry_attr->group_mode,
                              slice_number, 0));
            slice_fc[slice_number].primary_grp_id = -1;
            slice_fc[slice_number].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
            slice_fc[slice_number].slice_flags &= ~(BCMPTM_SLICE_MODE_SINGLE
                                                    | BCMPTM_SLICE_MODE_DOUBLE
                                                    | BCMPTM_SLICE_MODE_TRIPLE);
            slice_fc[slice_number].num_entries = num_entries_per_slice;
            slice_fc[slice_number].free_entries = num_entries_per_slice;
            slice_fc[slice_number].num_presel_entries = num_presels_per_slice;
            slice_fc[slice_number].free_presel_entries = num_presels_per_slice;

            /* Adjust slice offsets */
            for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE; mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
                 mode++) {
                slice_fc[slice_number].next_slice_id_offset[mode] = 0;
                prev_slice_offset = slice_fc[slice_number].prev_slice_id_offset[mode];

                if (prev_slice_offset !=0) {
                    slice_fc[slice_number + prev_slice_offset].next_slice_id_offset[mode]
                      = 0;
                    slice_fc[slice_number].prev_slice_id_offset[mode] = 0;
                }
                slice_fc[slice_number].left_slice_id_offset[mode] = 0;
                slice_fc[slice_number].right_slice_id_offset[mode] = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_per_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     fg            - (IN) Field group structure
 *     slice_ptr     - (IN) Field slice structure.
 *     entry_info    - (IN) Field entry structure.
 *     entry_attr    - (IN) Entry attrs structure
 *
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_per_group_compress(int unit,
                                     bcmptm_rm_tcam_slice_t *slice_ptr,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int eidx;
    int idx;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint8_t count_free_slices = 0;
    uint32_t entry_info_idx;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    int end_idx = 0;
    uint32_t total_entry_info_depth = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_fp_group_t *grp_ptr = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t local_idx = 0;
    int part = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_ptr, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;
    grp_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;

    /* Donot compress for multi mode groups. */
    if (iomd->multi_mode == TRUE) {
        SHR_EXIT();
    }

    eidx = -1;

    if (entry_attrs != NULL) {
       pt_dyn_info.tbl_inst = entry_attrs->pipe_id;
    } else {
       pt_dyn_info.tbl_inst = -1;
    }

    /* Find the total depth of this group */
    for (entry_info_idx = 0;
        entry_info_idx < iomd->entry_info_arr.num_depth_parts;
        entry_info_idx++) {
        total_entry_info_depth +=
            iomd->entry_info_arr.entry_info_depth[entry_info_idx];
    }

    end_idx = total_entry_info_depth;
    for (idx = 0; idx < end_idx; idx++) {
        /* Find an empty slot. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                              iomd,
                                              idx,
                                              entry_info_ptr,
                                              &local_idx));
        entry_info = entry_info_ptr[0];

        if (entry_info[local_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID
            && entry_info[local_idx].entry_type ==
                    BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) {
            if (eidx == -1) {
                eidx = idx;
             }
            continue;
        }
        if (eidx != -1) {
            /*
             * Move the current entry to empty index found
             * Move the entry in hardware.
             */
             SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_entry_move(unit,
                                           iomd->ltid,
                                           iomd->ltid_info,
                                           0,
                                           idx,
                                           eidx,
                                           (void *)entry_attrs,
                                           &pt_dyn_info, iomd));
             if (idx == iomd->reserved_index) {
                 /* Update FP group's and shared groups reserved index */
                 SHR_IF_ERR_VERBOSE_EXIT
                   (bcmptm_rm_tcam_fp_update_reserve_entry_index(unit,
                                                                 iomd,
                                                                 idx,
                                                                 eidx));
             } else {
                /* Increment the free count for idx logical id slice */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit,
                                                     idx,
                                                     iomd));

                /* Decrement the free count for eidx logical id slice */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_fp_entry_free_count_decr(unit,
                                                     eidx,
                                                     iomd));
             }

             /* Mark the entry_info at freed index to invalid type */
             for (part = 0; part < iomd->num_parts; part++) {
                 entry_info = entry_info_ptr[part];
                 entry_info[local_idx].entry_type =
                                 BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
             }

             idx = eidx;
             eidx = -1;
        }
    }
    /* Free the unused slices */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_free_unused_slices(unit,
                                                    iomd,
                                                    iomd->group_ptr,
                                                    &count_free_slices,
                                                    1));
    if (count_free_slices) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_entry_segment_compress(unit,
                                            iomd->ltid,
                                            iomd->ltid_info,
                                            grp_ptr->group_id,
                                            entry_attrs->pipe_id,
                                            entry_attrs->stage_id,
                                            count_free_slices * iomd->num_parts,
                                            0));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_tindex_to_sindex_extended(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd,
                                   uint32_t rm_flags,
                                   uint32_t logical_idx,
                                   uint8_t width_part,
                                   uint8_t *depth_part,
                                   uint32_t *phy_idx)
{
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t slice_id = 0;
    uint8_t prev_slice_id = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t total_entries_1 = 0;
    uint32_t total_entries_2 = 0;
    uint16_t depth_inst = 0;
    uint16_t num_depth_inst = 0;
    int min_index = 0;
    int max_index = 0;
    uint8_t skip_slice_count = 0;
    uint8_t idx = 0;
    uint8_t tile_id = 0;
    uint8_t num_tcam_sids = 0;
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    bcmdrd_sid_t sid = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = 0;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(depth_part, SHR_E_PARAM);
    SHR_NULL_CHECK(phy_idx, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;
    entry_attr = iomd->entry_attrs;
    stage_fc = iomd->stage_fc;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));


    slice_fc = iomd->slice_fc;
    fg = iomd->group_ptr;

    slice_id = fg->primary_slice_id;
    *depth_part = 0;
    prev_slice_id = -1;

    do {
        if (SHR_BITGET(entry_attr->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                   iomd->ltid,
                                                   ltid_info,
                                                   entry_attr->pipe_id,
                                                   slice_id,
                                                   &num_entries_per_slice));

        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(unit,
                                             iomd->ltid,
                                             ltid_info,
                                             slice_id,
                                             entry_attr->pipe_id,
                                             ref_grp_mode,
                                             &num_entries_per_slice));
        }

        if (logical_idx < num_entries_per_slice) {
            break;
        }

        prev_slice_id = slice_id;

        slice_id =
            (slice_id +
             slice_fc[slice_id].next_slice_id_offset[entry_attr->group_mode]);
        logical_idx = logical_idx - num_entries_per_slice;

    } while (prev_slice_id != slice_id);

    if (prev_slice_id == slice_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Single slice can be mapped to multiple physical SIDs.
     * Find the right depth instance for sid and sid_data_only
     * arrays in ltid_info for the given logical index.
     */
    depth_inst = 0;
    if ((stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) &&
        (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS)) {
        skip_slice_count = 1;
    } else {
        if (fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
            skip_slice_count = 1;
        } else if (fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
            skip_slice_count = 2;
        } else if (fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_TRIPLE ||
                   fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) {
            skip_slice_count = 4;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    rm_more_info = ltid_info->rm_more_info;
    for (idx = 0; idx < slice_id; idx += skip_slice_count) {
        tile_id = rm_more_info->slice_info[idx].tile_id;
        num_tcam_sids = 1;
        if (rm_more_info->tile_info != NULL)  {
            num_tcam_sids =
                rm_more_info->tile_info[tile_id].num_tcam_sids_per_slice;
        }
        depth_inst += num_tcam_sids;
    }

    hw_entry_info = &(ltid_info->hw_entry_info[fg->group_mode]);
    num_depth_inst = hw_entry_info->num_depth_inst;

    total_entries_1 = (logical_idx + 1);
    total_entries_2 = 0;
    for ( ; depth_inst < num_depth_inst; depth_inst++) {
        sid = hw_entry_info->sid[depth_inst][0];
        min_index = bcmptm_pt_index_min(unit, sid);
        max_index = bcmptm_pt_index_max(unit, sid);
        total_entries_2 += (max_index - min_index + 1);
        if (total_entries_2 < total_entries_1) {
            logical_idx -= (max_index - min_index + 1);
        } else {
            break;
        }
    }
    *depth_part = depth_inst;
    *phy_idx = logical_idx;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief To convert logical entry_id to
 * \ Physical entry id
 *
 * \param [in]  unit Logical device id
 * \param [in]  ltid Logical Table enum that is accessing the table
 * \param [in]  entry attrs structure
 * \param [in]  logical entry idx.
 * \param [in]  width part number.
 * \param [out] depth part number.
 * \param [out] physical entry idx.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
int
bcmptm_rm_tcam_fp_tindex_to_sindex(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd,
                                   uint32_t rm_flags,
                                   uint32_t logical_idx,
                                   uint8_t width_part,
                                   uint8_t *depth_part,
                                   uint32_t *phy_idx)
{
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t slice_id = 0;
    uint8_t prev_slice_id = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_data_entries_per_slice = 0;
    uint8_t  intraslice = 0;
    uint32_t start_index = 0;
    uint32_t end_index = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = 0;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(depth_part, SHR_E_PARAM);
    SHR_NULL_CHECK(phy_idx, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;
    entry_attr = iomd->entry_attrs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));

    if (ltid_info->rm_more_info->fp_flags &
        BCMPTM_STAGE_SLICE_WITH_MULTI_PT) {
        SHR_ERR_EXIT
            (bcmptm_rm_tcam_fp_tindex_to_sindex_extended(unit,
                                                         iomd,
                                                         rm_flags,
                                                         logical_idx,
                                                         width_part,
                                                         depth_part,
                                                         phy_idx));
    }

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;
    fg = iomd->group_ptr;

    /* Detect if a legacy arch based intraslice wit two TCAM's */
    if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA ||
         entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD)) {
        intraslice = 1;
    }

    slice_id = fg->primary_slice_id + (width_part >> intraslice);

    *depth_part = 0;

    prev_slice_id = -1;

    do {
        /* Check whether the entry is presel entry */
        if (SHR_BITGET(entry_attr->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
            /* Get the number of presel entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                   iomd->ltid,
                                                   ltid_info,
                                                   iomd->pipe_id,
                                                   slice_id,
                                                   &num_entries_per_slice));
            num_data_entries_per_slice = num_entries_per_slice;
        } else {
            /* Get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(unit,
                                           iomd->ltid,
                                           ltid_info,
                                           slice_id,
                                           iomd->pipe_id,
                                           entry_attr->group_mode,
                                           &num_data_entries_per_slice));
            num_entries_per_slice = num_data_entries_per_slice;
        }

        /* calculate the depth part and correct slice id */
        if (logical_idx < num_entries_per_slice) {
            break;
        }

        if (!SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
            (*depth_part)++;
        }

        prev_slice_id = slice_id;

        slice_id =
            (slice_id +
             slice_fc[slice_id].next_slice_id_offset[entry_attr->group_mode]);
        logical_idx = logical_idx - num_entries_per_slice;

    } while (prev_slice_id != slice_id);

    if (prev_slice_id == slice_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slice_start_end_index_get(unit,
                                                             iomd->ltid,
                                                             ltid_info,
                                                             slice_id,
                                                             iomd->pipe_id,
                                                             &start_index,
                                                             &end_index));

    /**************************************************************************
     *        IFP TCAM                              |  IFP PT
     **************************************************************************
     *        |Presel    |Presel    |               |Presel  |Presel    |
     *        |Intra     |non-Intra |Legacy         |Intra   |non-Intra |Legacy
     **************************************************************************
     *SINGLE  |idx       |idx       |idx            |idx     |idx       |idx
     **************************************************************************
     *INTRA   |idx/2     |NA        |idx,           |idx     |NA        |idx
     *                              |idx + se/2
     **************************************************************************
     *INTER   |idx1/2,   |idx1,     |idx1, idx2     |idx1    |idx1      |idx1
              |idx2/2    |idx2
     **************************************************************************
     *TRIPLE  |idx1/2,   |idx1,     |idx1,idx1+se/2 |idx1    |idx1      |idx1
     *        |idx2/2,   |idx2,
     *        |idx3/3    |idx3
     **************************************************************************
     *QUAD    |NA        |NA        |idx1, idx1+se/2|idx1    |idx1      |idx1
     *                              |idx2, idx2+se/2
 */

    /**************************************************************************
     *        VFP TCAM                 |  VFP PT
     **************************************************************************
     *                         Legacy
     **************************************************************************
     *SINGLE  |idx                     |idx
     **************************************************************************
     *INTRA   |idx,                    |idx
     *        |idx + se/2
     **************************************************************************
     *INTER   |idx1,                   |idx1
     *        |idx2
     **************************************************************************
     *TRIPLE  |NA                      |NA
     **************************************************************************
     *QUAD    |idx1, idx1+se/2         |idx1
     *        |idx2, idx2+se/2
 */

    /**************************************************************************
     *        EFP TCAM                 |  EFP PT
     **************************************************************************
     *                         Legacy
     **************************************************************************
     *SINGLE  |idx                     |idx
     **************************************************************************
     *INTRA   |NA                      |NA
     **************************************************************************
     *INTER   |idx1,                   |idx1
     *        |idx2
     **************************************************************************
     *TRIPLE  |NA                      |NA
     **************************************************************************
     *QUAD    |NA                      |NA
 */

    /* Calculate physical index */
    /*
     * For Wide mode number of entries in each slice is half
     * but not for policy table.
     */
    if ((rm_flags != BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) &&
        (BCMPTM_RM_TCAM_GRP_MODE_HALF != entry_attr->group_mode) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) {
        *phy_idx = (start_index / 2) + logical_idx;
    } else {
        *phy_idx = start_index + logical_idx;
    }

    /*
     * PRESEL will be inserted with LT as main TCAM. So current
     * ltid_info can't be relied on.
     */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        *phy_idx = (slice_id * num_data_entries_per_slice) + logical_idx;
    } else if (intraslice && width_part % 2) {
        *phy_idx = *phy_idx + ((end_index - start_index + 1) / 2);
    }

    /*
     * Remap the Index if required. To handle Special HW case.
     * RMTCAM index allocation algorithm always considers lowest
     * index as highest prioirty. remap allocated physical index
     * as per HW.
     */
    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_INDEX_REMAP &&
        entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_HALF &&
        !SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_sindex_remap(unit,
                                            iomd->ltid,
                                            ltid_info,
                                            (void *)entry_attr,
                                            slice_id,
                                            *phy_idx,
                                            phy_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_priority_reset
 * Purpose:
 *    Reset lt_config structure when a group is deleted
 *    Also clear group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    pri_slice_idx  - (IN) Primary slice index
 *    fg             - (IN) Field group structure.
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

int
bcmptm_rm_tcam_fp_group_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_fp_group_t *fg,
                              bcmptm_rm_tcam_slice_t *slice_fc)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;    /* Number of entry parts.   */
    int part;
    uint8_t slice_number;    /* LT Slice number.         */
    uint8_t depth_slice_idx; /* LT Slice number.         */
    uint8_t depth_part;      /* LT Slice number.         */
    uint8_t intraslice = 0;
    int curr_group_id;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                                        pipe_id, stage_fc->stage_id,
                                        &lt_config));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       fg->group_mode, stage_fc->flags,
                                       &parts_count));

    if ((stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == fg->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    depth_slice_idx = fg->primary_slice_id;
    depth_part = 0;

    do {

        depth_slice_idx = depth_slice_idx + depth_part;
        /* Iterate over entry parts and program hardware for each slice. */
        for (part = 0; part < parts_count; part++) {

            /* Get slice number to which the entry part belongs to. */
            slice_number = depth_slice_idx + part;

            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                lt_config[fg->group_ltid].lt_part_pri[slice_number] = 0;
            } else {
                lt_config[fg->group_id].lt_part_pri[slice_number] = 0;
            }

            /* update the primary group id, if primary group
             * is getting deleted
             */
             curr_group_id = fg->group_id;
            if (slice_fc[slice_number].primary_grp_id == curr_group_id) {
                slice_fc[slice_number].primary_grp_id =
                                               fg->next_shared_group_id;
            }
        }

        depth_part = slice_fc[depth_slice_idx].next_slice_id_offset[fg->group_mode];

    } while (depth_part !=0);

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        lt_config[fg->group_ltid].valid = FALSE;
        lt_config[fg->group_ltid].priority = 0;
        lt_config[fg->group_ltid].lt_part_map = 0;
        lt_config[fg->group_ltid].lt_action_pri = 0;
        lt_config[fg->group_ltid].group_ltid = 0;
        lt_config[fg->group_ltid].pipe_id = 0;
    } else {
        lt_config[fg->group_id].valid = FALSE;
        lt_config[fg->group_id].priority = 0;
        lt_config[fg->group_id].lt_part_map = 0;
        lt_config[fg->group_id].lt_action_pri = 0;
        lt_config[fg->group_id].group_ltid = 0;
        lt_config[fg->group_id].pipe_id = 0;
    }
    /* Install LT partition Priority. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                 ltid, ltid_info, fg,
                                 pipe_id, stage_fc->stage_id));
    fg->group_ltid = -1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_free_unused_slices
 * Purpose:
 *     Unallocate group unused slices.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     fg             - (IN) Field group structure.
 *     clear_presel   - (IN) Flag to free presel entires
 * Returns:
 *     SHR_E_XXX
 */
int
bcmptm_rm_tcam_fp_group_free_unused_slices(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                                           bcmptm_rm_tcam_fp_group_t *fg,
                                           uint8_t *count_free_slices,
                                           uint8_t group_compress)
{
    uint8_t fs = 0;
    uint8_t fs_next = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *pri_fg = NULL;
    int32_t pri_group_id = -1;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    int i = -1;
    uint8_t *unused_slices_arr = NULL;
    uint8_t count_unused_slices = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    SHR_ALLOC(unused_slices_arr,
              iomd->num_slices * sizeof(uint8_t), "bcmptmRmTcamUnusedSlices");

    sal_memset(unused_slices_arr, 0, iomd->num_slices * sizeof(uint8_t));

    pri_group_id = slice_fc[fs].primary_grp_id;
    fs_next =0;
    mode = iomd->entry_attrs->group_mode;
    fs = fg->primary_slice_id;

    /* Get the list of slices having 0 entries */

    do {
        fs = fs + fs_next;
        if (slice_fc[fs].num_entries == slice_fc[fs].free_entries) {
           unused_slices_arr[count_unused_slices] = fs;
           count_unused_slices++;
       }
       fs_next = slice_fc[fs].next_slice_id_offset[mode];
    } while (fs_next != 0);

    /* Go to the last slice in list and start freeing the slices */
    for (i= (count_unused_slices - 1); i >= 0 ; i--) {

        fs =  unused_slices_arr[i];
        if (slice_fc[fs].num_entries == slice_fc[fs].free_entries) {
            /*
             * If it a primary slice and some presel entries are
             * present, don't free the slice.
             */
            if (fs == fg->primary_slice_id &&
                (slice_fc[fs].num_presel_entries !=
                 slice_fc[fs].free_presel_entries)) {
                continue;
            }
            /* Count the freed slices */
            *count_free_slices = (*count_free_slices) + 1;
            /*
             * Decrease the group's num_entires by slice's num_entires
             * for group delete case here. num_entires will be corrected
             * for group compress case in group_partition_priority_reset
             * function for all the shared groups.
             */
            if (0 == group_compress) {
                fg->num_entries = fg->num_entries - slice_fc[fs].num_entries;
            }
            /* Reset selcodes for the slice */
            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_slice_selcodes_reset(unit,
                                                            iomd->ltid,
                                                            iomd->ltid_info,
                                                            iomd->entry_attrs,
                                                            fs));
            }

            /* clear the h/w presel entries */
            if ((stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
                (1 == group_compress)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_presel_entries_clear(unit,
                                                            iomd,
                                                            fs));
            }
            /* Reset the partition priority for the slice. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_partition_priority_reset(unit,
                                                            iomd->ltid,
                                                            iomd->ltid_info,
                                                            iomd->entry_attrs,
                                                            fs,
                                                            slice_fc,
                                                            group_compress));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_profiles_clear(unit, iomd, fs));
        }
    }

    if (*count_free_slices > 0 &&
        (1 == group_compress)) {
        /*
         * For group's sharing slices is compressed, and some
         * slices are freed, update the partition priority for
         * all the groups sharing and install the new partition
         * priorities.
         */
        while (pri_group_id != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  (uint32_t)pri_group_id,
                                                  stage_fc,
                                                  &pri_fg));
            /* Install LT partition Priority. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                                           iomd->ltid,
                                                           iomd->ltid_info,
                                                           pri_fg,
                                                           iomd->pipe_id,
                                                           iomd->stage_id));
            pri_group_id = pri_fg->next_shared_group_id;
        }
    }

exit:
    if (unused_slices_arr != NULL) {
        SHR_FREE(unused_slices_arr);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate whether enough resources are freed for the new
 *     slice allocation.
 *
 * \param [in]  unit Logical device id
 * \param [in]  iomd iomd structure.
 * \param [out] can_compress set to 1 if resources are avaiable for
 *              new group or auto expansion requested/
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
static
int bcmptm_rm_tcam_fp_group_compress_validate(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t *can_compress)
{
    bcmptm_rm_tcam_slice_t     *slice_ptr_cpy = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t  *shared_group_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    uint8_t num_slices;
    uint8_t slice_id;
    uint8_t slice = 0;
    uint8_t slice_id_prev;
    uint32_t idx;
    uint32_t num_entries_per_slice = 0;
    uint32_t free_entries = 0;
    uint8_t compress = 0;
    bool multi_mode = FALSE;
    int32_t shared_group_id = -1;
    uint32_t group_mode;
    uint32_t mode;
    int right_slice_id_offset = 0;
    int prev_slice_offset = 0;
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_entry_iomd_t *iomd_temp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    /* Initialize field slice pointer. */
    slice_ptr = (bcmptm_rm_tcam_slice_t *) iomd->slice_fc;
    num_slices = stage_fc->num_slices;

    SHR_ALLOC(slice_ptr_cpy,
              num_slices * sizeof(bcmptm_rm_tcam_slice_t),
              "bcmptmRmtcamSlicePtrCopy");
    sal_memcpy(slice_ptr_cpy, slice_ptr,
               num_slices * sizeof(bcmptm_rm_tcam_slice_t));

    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((grp_ptr[idx].valid == TRUE) &&
            (grp_ptr[idx].pipe_id == entry_attr->pipe_id)) {

            /* Get number of free entires per group */
            slice_id = grp_ptr[idx].primary_slice_id;

            /*
             * Do compression only for primary groups. It will take care of
             * freeing the resources for all shared groups if there are free
             * entries in the shared slices.
             */
            if ((uint32_t)slice_ptr_cpy[slice_id].primary_grp_id
                                != grp_ptr[idx].group_id) {
                continue;
            }

            multi_mode = FALSE;

            /* Check if it is not multi mode group */
            shared_group_id = grp_ptr[idx].next_shared_group_id;
            while (shared_group_id != -1) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_fp_group_info_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               entry_attr->pipe_id,
                                               shared_group_id,
                                               stage_fc,
                                               &shared_group_ptr));
                if (grp_ptr[idx].group_mode != shared_group_ptr->group_mode) {
                    multi_mode = TRUE;
                    break;
                }
                shared_group_id = shared_group_ptr->next_shared_group_id;
            }

            /* Group compress is not currently supported for multi mode grps */
            if (multi_mode == TRUE) {
                continue;
            }

            /* Get num entries per slice */
            /* Initialize the free count value to 0 */
            free_entries = 0;
            /*
             * Aggregate the free count for all depth slices corrsponding to
             * to the given group.
             */
            group_mode = grp_ptr[idx].group_mode;
            do {

                /* Add the free entries count in current slice */
                free_entries =
                    free_entries + slice_ptr_cpy[slice_id].free_entries;

                slice_id_prev = slice_id;

                /* Go to the next slice if exists */
                slice_id = slice_id +
                    slice_ptr_cpy[slice_id].next_slice_id_offset[group_mode];

            } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));

            /*
             * Get the num_entries_per_slice from last auto expanded slice,
             * as the first freed slice will be the last slice of group and
             * in some vairants/chips num_entries_per_slice varies from
             * slice to slice.
             */
            num_entries_per_slice = slice_ptr_cpy[slice_id].num_entries;

            /* Start with primary slice of the group */
            slice_id = grp_ptr[idx].primary_slice_id;

            /* Calculate whether a slice/slices can be freed
             * from this group */
            while (free_entries > num_entries_per_slice) {

                /*
                 * Free the last slice. Free the width expanded slices too.
                 * If more slices can be freed,
                 * update the slice structure accordingly
                 */
                do {
                    /* Add the free entries count in current slice */
                    slice_id_prev = slice_id;
                    /* Go to the next slice if exists */
                    slice_id = slice_id +
                      slice_ptr_cpy[slice_id].next_slice_id_offset[group_mode];

                } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));

                if (slice_id >= num_slices) {
                    /* Invalid slice ID */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                num_entries_per_slice = slice_ptr_cpy[slice_id].num_entries;

                if (free_entries < num_entries_per_slice) {
                /*
                 * Free_entries are less than num_entries_per_slice,
                 * this scenario will come when slices of different sizes
                 * (512, 256) are present in current groups auto expansion
                 */
                    break;
                }
                free_entries -= num_entries_per_slice;

                /* Check and free it this slice has width expansion slices */
                right_slice_id_offset = 0;
                do {
                    slice_id = slice_id + right_slice_id_offset;
                    slice_ptr_cpy[slice_id].primary_grp_id = -1;
                    slice_ptr_cpy[slice_id].slice_flags &=
                                        ~(BCMPTM_SLICE_IN_USE);
                    slice_ptr_cpy[slice_id].free_entries =
                                        slice_ptr_cpy[slice_id].num_entries;
                    slice_ptr_cpy[slice_id].free_presel_entries =
                                 slice_ptr_cpy[slice_id].num_presel_entries;
                    right_slice_id_offset =
                      slice_ptr_cpy[slice_id].right_slice_id_offset[group_mode];

                    /* Adjust slice offsets */
                    for (mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
                         mode < BCMPTM_RM_TCAM_GRP_MODE_COUNT;
                         mode++) {
                        slice_ptr_cpy[slice_id].next_slice_id_offset[mode] = 0;
                        prev_slice_offset =
                             slice_ptr_cpy[slice_id].prev_slice_id_offset[mode];

                        if (prev_slice_offset !=0) {
                            slice_ptr_cpy[(slice_id + prev_slice_offset)].
                                    next_slice_id_offset[mode] = 0;
                            slice_ptr_cpy[slice_id].prev_slice_id_offset[mode]
                                    = 0;
                        }
                        slice_ptr_cpy[slice_id].left_slice_id_offset[mode] = 0;
                        slice_ptr_cpy[slice_id].right_slice_id_offset[mode] = 0;
                    }
                    compress = 1;
                } while (right_slice_id_offset != 0);

            }
        }
    }

    if (compress == 1) {
        /*
         * Try allocating from this slice structure.
         * if sucess return can_compress =1 otherwise return 0
         */
        *can_compress = 0;
        SHR_ALLOC(iomd_temp,
                  sizeof(bcmptm_rm_tcam_entry_iomd_t),
                  "bcmptmRmTcamIomdTemp");
        sal_memcpy(iomd_temp, iomd,
                   sizeof(bcmptm_rm_tcam_entry_iomd_t));
        iomd_temp->slice_fc = slice_ptr_cpy;
        group_mode = entry_attr->group_mode;
        if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
            /* Reserve single slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_single_slice(unit,
                                                              iomd_temp,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *can_compress = 1;
            }
        } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == group_mode ||
              (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
            /* Reserve dbl slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_double_slice(unit,
                                                              iomd_temp,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *can_compress = 1;
            }

        } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {

            /* Reserve triple slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_triple_slice(unit,
                                                              iomd_temp,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *can_compress = 1;
            }
        }
    }

exit:
    if (iomd_temp != NULL) {
        SHR_FREE(iomd_temp);
    }
    SHR_FREE(slice_ptr_cpy);
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_free_last_slice(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint8_t num_slices;
    uint8_t slice_id;
    uint8_t slice_id_prev;
    uint32_t num_entries_per_slice = 0;
    uint32_t free_entries = 0;
    uint8_t compress = 0;

    SHR_FUNC_ENTER(unit);

    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;

    /* Initialize field slice pointer. */
    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;

    num_slices = iomd->num_slices;

    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) iomd->group_ptr;

    if (grp_ptr->valid == TRUE) {

        /* Get number of free entires per group */
        slice_id = grp_ptr->primary_slice_id;

        /* Initialize the free count value to 0 */
        free_entries = 0;
        /*
         * Aggregate the free count for all depth slices corrsponding to
         * to the given group.
         */
        do {
           /* Add the free entries count in current slice */
           free_entries = free_entries + slice_ptr[slice_id].free_entries;

           slice_id_prev = slice_id;
           /* Go to the next slice if exists */
           slice_id = slice_id +
                    slice_ptr[slice_id].next_slice_id_offset[grp_ptr->group_mode];

        } while (slice_id < num_slices
             && (slice_id != slice_id_prev));

        /* Get num entries from last slice */
        num_entries_per_slice = slice_ptr[slice_id].num_entries;

        slice_id = grp_ptr->primary_slice_id;

        /*
         * Calculate whether a slice/slices can be freed
         * from this group
         */
        if (free_entries > num_entries_per_slice) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_per_group_compress(unit,
                                                 slice_ptr,
                                                 iomd));
            compress = 1;
        }
    }

    if (compress == 1) {
        /* Install LT Action Priority. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                                                            iomd->ltid,
                                                            iomd->ltid_info,
                                                            iomd->pipe_id,
                                                            stage_fc));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_free_group_resources
 * Purpose:
 *    Release the group's resources when no entries are
 *    present in the group
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    iomd           - (IN) Input and Output metadata.
 * Returns:
 *    SHR_E_XXX
 */

int
bcmptm_rm_tcam_fp_free_group_resources(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t count_free_slices = 0;
    int32_t pri_grp_id = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_fp_group_t *pri_fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_ptr = NULL;
    int32_t curr_grp_id = 0;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    bool reserved_used = false;
    uint8_t primary_slice_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    mode = iomd->entry_attrs->group_mode;

    stage_fc = iomd->stage_fc;
    slice_ptr = iomd->slice_fc;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->entry_attrs->pipe_id,
                                          iomd->entry_attrs->group_id,
                                          stage_fc,
                                          &fg));
    primary_slice_id = fg->primary_slice_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_priority_reset(unit,
                                                iomd->ltid,
                                                iomd->ltid_info,
                                                iomd->pipe_id,
                                                stage_fc,
                                                fg,
                                                slice_ptr));

    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_free_unused_slices(unit,
                                                        iomd,
                                                        fg,
                                                        &count_free_slices,
                                                        FALSE));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_entry_segment_compress(unit,
                                   iomd->ltid,
                                   iomd->ltid_info,
                                   fg->group_id,
                                   iomd->pipe_id,
                                   iomd->stage_id,
                                   count_free_slices,
                                   1));
        /*
         * fg->group_id == pri_grp_id
         * Corresponding changes to pri_grp_id in slice structure is done in
         * bcmptm_rm_tcam_fp_group_priority_reset function
         * nothing to handle here
         */
        /* Get the primary group Id after deleting the current group */
        pri_grp_id = slice_ptr[fg->primary_slice_id].primary_grp_id;

    } else {

        pri_grp_id = slice_ptr[fg->primary_slice_id].primary_grp_id;
        curr_grp_id = fg->group_id;

        if (pri_grp_id == curr_grp_id
            && fg->next_shared_group_id == -1) {
            pri_grp_id = -1;
        }
    }

    /*
     * If the group's are sharing slices, remove the current
     * group pointer from the next_shared_group_id
     */
    if (pri_grp_id != -1) {
        /* Initialize field primary group pointer. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              (uint32_t)pri_grp_id,
                                              stage_fc,
                                              &pri_fg));

        /*
         * Traverse the sharing groups till the current group Id
         * is found.
         */
        curr_grp_id = fg->group_id;
        while (pri_fg->next_shared_group_id != curr_grp_id
               && pri_fg->next_shared_group_id != -1) {

            pri_grp_id = pri_fg->next_shared_group_id;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              (uint32_t)pri_grp_id,
                                              stage_fc,
                                              &pri_fg));
        }
        if (pri_fg->next_shared_group_id == curr_grp_id) {
            pri_fg->next_shared_group_id = fg->next_shared_group_id;
        }
    } else {
        /* Last group deletion. delete reserve entry */
        if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            /* Clear reserve entry */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_grp_reserve_entry_clear(unit,
                                                           iomd));
        }
    }

    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
        if (-1 == pri_grp_id) {
            /* Clear the in use flag and clear the left/right offsets */
            if (fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
                slice_ptr[0].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[0].free_presel_entries =
                                   slice_ptr[0].num_presel_entries;
                slice_ptr[0].left_slice_id_offset[mode] = 0;
                slice_ptr[0].right_slice_id_offset[mode] = 0;
                slice_ptr[1].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[1].free_presel_entries =
                                   slice_ptr[1].num_presel_entries;
                slice_ptr[1].left_slice_id_offset[mode] = 0;
                slice_ptr[1].right_slice_id_offset[mode] = 0;
            } else {
                slice_ptr[fg->primary_slice_id].slice_flags &=
                             ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[fg->primary_slice_id].free_presel_entries =
                             slice_ptr[fg->primary_slice_id].num_presel_entries;
                slice_ptr[fg->primary_slice_id].left_slice_id_offset[mode] = 0;
                slice_ptr[fg->primary_slice_id].right_slice_id_offset[mode] = 0;
            }
        }
    }

    fg->valid = FALSE;
    sal_memset(fg, 0, sizeof(bcmptm_rm_tcam_fp_group_t));
    /* Install LT Action Priority. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                                                        iomd->ltid,
                                                        iomd->ltid_info,
                                                        iomd->pipe_id,
                                                        stage_fc));

    /*
     * Only for multi mode sharing groups, we need to check if we can
     * clear reserve entry of a particular entry_type if no group of that
     * entry_type exist after deleting the current group.
     * For non-sharing groups, when last entry is deleted,
     * reserve entry clear is called. For same mode sharing groups,
     * all shared groups use same reserve entry index.
     * When last group of same mode sharing group's is deleted, reserve entry
     * will be cleared, till then reserve entry should not be cleared.
     */
     if (iomd->reserved_index != -1) {
        /* Get the primary group Id after deleting the current group */
        pri_grp_id = slice_ptr[primary_slice_id].primary_grp_id;

        if (pri_grp_id != -1) {
            do {
                /* Initialize field primary group pointer. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              (uint32_t)pri_grp_id,
                                              stage_fc,
                                              &pri_fg));
                /*
                 * Traverse the sharing groups to find whether any of the
                 * shared groups are using same reserve entry. If yes, no need
                 * to free reserve entry index. if no other group is using same
                 * reserve entry, clear the reserve entry index.
                 */

                 if (pri_fg->reserve_entry_index_exists == true &&
                     pri_fg->group_mode == mode &&
                     pri_fg->reserved_entry_index == iomd->reserved_index) {
                     reserved_used = true;
                     break;
                 } else {
                     pri_grp_id = pri_fg->next_shared_group_id;
                 }
            } while (pri_grp_id != -1);

            if (reserved_used == false) {
                /* Clear reserve entry */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_grp_reserve_entry_clear(unit,
                                                               iomd));

            }
        }
     }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_compress
 * Purpose:
 *     Compress the groups, which no longer has entries
 *     Compress the groups, if any slices can be freed depending
 *     on the number of entries present in it.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     attrs         - (IN) Entry attrs structure
 *
 * Returns:
 *     SHR_E_XXX
 */

int bcmptm_rm_tcam_fp_group_compress(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;          /* Slice iterator */
    uint8_t slice_id_prev;     /* Slice iterator */
    uint32_t idx;
    uint32_t num_entries_per_slice = 0;
    uint32_t free_entries = 0;
    uint32_t total_entries = 0;
    bcmptm_rm_tcam_entry_iomd_t *iomd_temp = NULL;
    int size =0;
    uint8_t compress = 0;
    uint8_t compress_valid = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Check whether compression will free enough slices for the
     * current group to create/auto expand. if not enough slices
     * are getting freed for the new allocation, we should not perform
     * the compression itself, as it wil change the hardware state
     * moving entries, but the trasaction will fail as new allocation
     * for group fails due to lack of resources.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_compress_validate(unit,
                                                   iomd,
                                                   &compress_valid));
    if (compress_valid == 0) {
        SHR_EXIT();
    }

    /* make a copy of the entry_attr structure*/
    SHR_ALLOC(entry_attr, sizeof(bcmptm_rm_tcam_entry_attrs_t),
                      "bcmptmRmtcamEntryAttrsCopy");
    sal_memcpy(entry_attr, iomd->entry_attrs,
                           sizeof(bcmptm_rm_tcam_entry_attrs_t));

    /* Reset the presel entry flag */
    SHR_BITCLR(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);

    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;
    /* Initialize field slice pointer. */

    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;
    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);
    size = sizeof(bcmptm_rm_tcam_entry_iomd_t);
    SHR_ALLOC(iomd_temp, size, "bcmptmRmTcamEntryIomdTemp");
    sal_memcpy(iomd_temp, iomd, size);
    iomd_temp->entry_attrs = (void *)entry_attr;

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((grp_ptr[idx].valid == TRUE) &&
            (grp_ptr[idx].pipe_id == entry_attr->pipe_id)) {
            /* get number of entires per group */
            total_entries = grp_ptr[idx].num_entries;
            /* get number of free entires per group */
            slice_id = grp_ptr[idx].primary_slice_id;
            /* Initialize the free count value to 0 */
            free_entries = 0;
            /* aggregate the free count for all depth slices corrsponding to
            * to the given group.
            */
            do {
               /* Add the free entries count in current slice */
               free_entries = free_entries + slice_ptr[slice_id].free_entries;

               slice_id_prev = slice_id;
               /* go to the next slice if exists */
               slice_id = slice_id +
                        slice_ptr[slice_id].next_slice_id_offset[grp_ptr[idx].group_mode];

            } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));

            /* Get num entries from last slice */
            num_entries_per_slice = slice_ptr[slice_id].num_entries;

            slice_id = grp_ptr[idx].primary_slice_id;

            /* Initialize the grp_ptr in entry_attr structure */
            entry_attr->group_id = grp_ptr[idx].group_id;
            entry_attr->group_prio = grp_ptr[idx].priority;
            entry_attr->group_mode = grp_ptr[idx].group_mode;
            entry_attr->group_ltid = grp_ptr[idx].group_ltid;
            iomd_temp->group_ptr = (void *)&grp_ptr[idx];
            if ((free_entries == total_entries) &&
                (slice_ptr[slice_id].num_presel_entries
                         == slice_ptr[slice_id].free_presel_entries)) {
                /*
                 * Group exists but no entries in it
                 * clear the slices associated with group
                 * change the group's valid value to false
                 * clear the primary_slice_id, num_entires etc
                 * reset the group priority and partition prio
                 */
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_fp_free_group_resources(unit, iomd));
                 compress = 1;
                 continue;
            }
            /* calculate whether a slice/slices can be freed
             * from this group */
            if (free_entries > num_entries_per_slice) {
                /* calculate the number of depth slice
                 * that can be freed
                  */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd_temp));

                bcmptm_rm_tcam_fp_per_group_compress(unit,
                                                     slice_ptr,
                                                     iomd_temp);
                compress = 1;
            }
        }
    }

    if (compress == 1) {
        /* Install LT Action Priority. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                                                            iomd->ltid,
                                                            iomd->ltid_info,
                                                            entry_attr->pipe_id,
                                                            stage_fc));
    }

exit:
    SHR_FREE(iomd_temp);
    SHR_FREE(entry_attr);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_update_slice_offsets
 * Purpose:
 *     Update slice offsets in case of multi wide group sharing
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     group mode    - (IN) group mode
 *     group mode    - (IN) group mode
 *     num_slices    - (IN) number of slices for this stage
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_update_slice_offsets(int unit,
                     bcmptm_rm_tcam_slice_t *slice_fc,
                     bcmptm_rm_tcam_group_mode_t pri_gmode,
                     bcmptm_rm_tcam_group_mode_t new_gmode,
                     uint8_t slice_count, /*pri slice count */
                     uint8_t pri_slice_id)
{
    uint8_t slice_depth = -1;
    uint8_t slice_curr = -1;
    uint8_t slice_id = -1;
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    slice_depth = pri_slice_id;

    if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == new_gmode) {

        do {

            slice_curr = slice_id = slice_depth;
            /*
             * Till the slice_count width wise, convert the right and left
             * offsets to next and prev offsets.
             */
            for (part = 0; part < slice_count; part++) {

                    slice_fc[slice_curr].next_slice_id_offset[new_gmode] =
                        slice_fc[slice_curr].right_slice_id_offset[pri_gmode];
                    if (part != 0) {
                        slice_fc[slice_curr].prev_slice_id_offset[new_gmode] =
                            slice_fc[slice_curr].left_slice_id_offset[pri_gmode];
                    }
                slice_curr = slice_curr +
                             slice_fc[slice_curr].right_slice_id_offset[pri_gmode];
            }

            /*
             * Get the first depth instance if present for the primary slice
             */
            slice_depth = slice_depth +
                          slice_fc[slice_depth].next_slice_id_offset[pri_gmode];

             /* if primary group is already auto expanded */
            if (slice_depth != slice_id) {

                /* Map the last width slice next offset to
                 * first auto expanded slice.
                 */
                slice_fc[slice_curr].next_slice_id_offset[new_gmode]
                                            = (slice_depth - slice_curr);
                slice_fc[slice_depth].prev_slice_id_offset[new_gmode]
                                            = (slice_curr - slice_depth);
            }
        } while (slice_depth != slice_id);
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == new_gmode &&
            BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == pri_gmode) {
        do {
            slice_curr = slice_id = slice_depth;

            /*
             * Till the slice_count width wise, copy the right, left,
             * next and prev offsets to new group mode.
             */
             slice_fc[slice_curr].right_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].right_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].left_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].left_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].next_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].next_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].prev_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].prev_slice_id_offset[pri_gmode];

             slice_curr =  slice_curr +
                 slice_fc[slice_curr].right_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].left_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].left_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].next_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].next_slice_id_offset[pri_gmode];

             slice_fc[slice_curr].prev_slice_id_offset[new_gmode] =
                 slice_fc[slice_curr].prev_slice_id_offset[pri_gmode];

             slice_depth = slice_depth +
                          slice_fc[slice_depth].next_slice_id_offset[pri_gmode];
        } while (slice_depth != slice_id);
    } else {
         /* control should not reach here */
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_slice_allocate(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_fp_group_t *grp_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    int16_t group_id = -1;
    bool reserve_entry_found = false;
    int reserve_index = -1;
    uint8_t slice_count = 0;
    uint8_t pri_slice_count = 0;
    uint16_t segment_id = 0;
    int pipe_id = 0;
    uint8_t slice = -1;
    uint8_t curr_slice_id = -1;
    uint8_t prev_slice_id = -1;
    bool slice_sharing = FALSE;
    bcmptm_rm_tcam_fp_group_t  *primary_fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *shared_fg = NULL;
    int16_t pri_grp_id = -1;
    bcmptm_rm_tcam_group_mode_t group_mode;
    bcmptm_rm_tcam_group_mode_t pri_group_mode;
    uint8_t idx =0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    group_mode = iomd->entry_attrs->group_mode;

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    grp_ptr = (bcmptm_rm_tcam_fp_group_t*)
              (((uint8_t *)stage_fc) + stage_fc->group_info_seg);

    rv = bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->entry_attrs->pipe_id,
                                          iomd->entry_attrs->group_id,
                                          stage_fc,
                                          &fg);
    /* Check the validity of group */
    if (rv != SHR_E_NOT_FOUND) {
       /*
        * Not a new group. Group already exists. incorrect call
        * to slice allocate
        */
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Find an empty slot for this group */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if (grp_ptr->valid == 0) {
            fg = grp_ptr;
            sal_memset(fg, 0, sizeof(bcmptm_rm_tcam_fp_group_t));
            break;
        }
        grp_ptr = grp_ptr + 1;
    }

    if (idx == stage_fc->num_groups) {
        /*
         * Number of groups supported in this stage and pipe
         * are exhausted.
         */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    grp_ptr = (bcmptm_rm_tcam_fp_group_t*)
              (((uint8_t *)stage_fc) + stage_fc->group_info_seg);

    /*
     * Check whether their exists a group already with same priority
     * in same stage and pipe. if exists, group's (new
     * and existing) can share the same resources
     */

    /* Groups with default presel entry can not share slices */
    rv = bcmptm_rm_tcam_fp_check_slice_sharing(unit,
                                               iomd,
                                               grp_ptr,
                                               &slice_sharing,
                                               &pri_grp_id);
    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    if (BCMPTM_RM_TCAM_GRP_MODE_HALF == group_mode ||
        BCMPTM_RM_TCAM_GRP_MODE_SINGLE == group_mode ||
        BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        slice_count = 1;
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == group_mode ||
        (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
        (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE))) {
        slice_count = 2;
    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == group_mode &&
        stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        slice_count = 3;
    }

    /*
     * If the primary group mode and current group_mode are different
     * but sharing is true, update the next, left, right offset's of
     * slice for the new mode.
     */
    if (slice_sharing == TRUE) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_fp_group_info_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               iomd->entry_attrs->pipe_id,
                                               pri_grp_id,
                                               stage_fc,
                                               &primary_fg));
        slice = primary_fg->primary_slice_id;
        pri_group_mode = primary_fg->group_mode;

        if (group_mode != pri_group_mode &&
            !(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {

            if (BCMPTM_RM_TCAM_GRP_MODE_HALF == pri_group_mode ||
                BCMPTM_RM_TCAM_GRP_MODE_SINGLE == pri_group_mode ||
                BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == pri_group_mode) {
                pri_slice_count = 1;
            } else if ((BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == pri_group_mode) ||
                (BCMPTM_RM_TCAM_GRP_MODE_QUAD == pri_group_mode &&
                (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE))) {
                pri_slice_count = 2;
            } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == pri_group_mode &&
                 stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                pri_slice_count = 3;
            }
            /*
             * Only 3 cases are supporting in slice sharing with
             * different group modes.
             * 1) primary group: triple and new group: single
             * 2) primary group: double and new group: single
             * 3) primary group: triple and new group: double
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_update_slice_offsets(unit,
                                                        slice_fc,
                                                        pri_group_mode,
                                                        group_mode,
                                                        pri_slice_count,
                                                        slice));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_profiles_reserve(unit,
                                                iomd,
                                                slice,
                                                slice_count));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_update_shared_group(unit,
                                               iomd,
                                               pri_grp_id));
    }

    if (slice_sharing == FALSE) {
        /* for EM, if group_mode is single check whether the slice ID
           is in USE. if it is already in use, return E_CONFIG.
           for EM, if group_mode is Double check whether both the slices
           are free. throw E_CONFIG otherwise
           */
        if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
            if (!(iomd->ltid_info->rm_more_info[0].flags &
                                   BCMPTM_RM_TCAM_UFT_HASH_BANKS)) {
                if (iomd->entry_attrs->group_mode ==
                                BCMPTM_RM_TCAM_GRP_MODE_QUAD) {
                    if (slice_fc[0].slice_flags & BCMPTM_SLICE_IN_USE
                        || slice_fc[1].slice_flags & BCMPTM_SLICE_IN_USE
                        || slice_fc[2].slice_flags & BCMPTM_SLICE_IN_USE
                        || slice_fc[3].slice_flags & BCMPTM_SLICE_IN_USE) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                    slice = 0;
                    slice_fc[0].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[0].free_presel_entries =
                            slice_fc[0].num_presel_entries;
                    slice_fc[1].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[1].free_presel_entries =
                            slice_fc[1].num_presel_entries;
                    slice_fc[2].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[2].free_presel_entries =
                            slice_fc[2].num_presel_entries;
                    slice_fc[3].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[3].free_presel_entries =
                            slice_fc[3].num_presel_entries;
                    slice_fc[0].right_slice_id_offset[group_mode] = 1;
                    slice_fc[1].left_slice_id_offset[group_mode] = -1;
                    slice_fc[2].left_slice_id_offset[group_mode] = -1;
                    slice_fc[3].left_slice_id_offset[group_mode] = -1;
                    slice_fc[0].primary_grp_id = iomd->entry_attrs->group_id;
                    slice_fc[1].primary_grp_id = iomd->entry_attrs->group_id;
                    slice_fc[2].primary_grp_id = iomd->entry_attrs->group_id;
                    slice_fc[3].primary_grp_id = iomd->entry_attrs->group_id;
                } else if (iomd->entry_attrs->group_mode ==
                            BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
                    if (slice_fc[0].slice_flags & BCMPTM_SLICE_IN_USE
                        || slice_fc[1].slice_flags & BCMPTM_SLICE_IN_USE) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                    slice = 0;
                    slice_fc[0].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[0].free_presel_entries =
                            slice_fc[0].num_presel_entries;
                    slice_fc[1].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[1].free_presel_entries =
                            slice_fc[1].num_presel_entries;
                    slice_fc[0].right_slice_id_offset[group_mode] = 1;
                    slice_fc[1].left_slice_id_offset[group_mode] = -1;
                    slice_fc[0].primary_grp_id = iomd->entry_attrs->group_id;
                    slice_fc[1].primary_grp_id = iomd->entry_attrs->group_id;
                } else {
                    slice = iomd->entry_attrs->slice_id;
                    if (!(slice_fc[slice].slice_flags & BCMPTM_SLICE_IN_USE)) {
                        slice_fc[slice].slice_flags |= BCMPTM_SLICE_IN_USE;
                        slice_fc[slice].free_presel_entries =
                                         slice_fc[slice].num_presel_entries;
                        slice_fc[slice].right_slice_id_offset[group_mode] = 0;
                        slice_fc[slice].left_slice_id_offset[group_mode] = 0;
                        slice_fc[slice].primary_grp_id =
                                        iomd->entry_attrs->group_id;
                    } else {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } else {
                /*
                 * All slices are in auto expanded mode for EM_FT
                 * as all slices are capable of multi wide entry
                 * and sharing resources.
                 */
                slice = 0;
                for (idx = 0; idx < stage_fc->num_slices; idx++) {
                    slice_fc[idx].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[idx].primary_grp_id =
                                  iomd->entry_attrs->group_id;
                    slice_fc[idx].free_presel_entries =
                                  slice_fc[idx].num_presel_entries;
                }
            }
        } else {
            rv = bcmptm_rm_tcam_fp_group_reserve_slice(unit,
                                                       iomd,
                                                       &slice);
            /* No resources */
            if (rv == SHR_E_RESOURCE) {
                if ((slice_count > 1) &&
                    (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
                    (iomd->ltid_info->rm_more_info->fp_flags &
                                BCMPTM_STAGE_SLICE_WITH_MULTI_PT)) {
                    /*
                     * Slice move algorithm implemented only for TD4 devices
                     * non TD4 devices will be taken up later
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_create_free_slice (unit,
                                                              iomd,
                                                              slice_count,
                                                              slice_count,
                                                              &slice));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_group_reserve_slice(unit,
                                                               iomd,
                                                               &slice));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            }
        }
    }

    /* Install selcodes in allocated slice */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_slice_selcodes_install(unit,
                                                      iomd->ltid,
                                                      iomd->ltid_info,
                                                      iomd->entry_attrs,
                                                      slice));
        /*
         * Incase shared slice was already autoexpanded copy selcodes
         * to all the expanded slices
         */
        if ((slice_sharing == TRUE) &&
            (slice_fc[slice].next_slice_id_offset[group_mode] != 0)) {
            curr_slice_id =
                slice + slice_fc[slice].next_slice_id_offset[group_mode];
            do {
                prev_slice_id = curr_slice_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_slice_selcodes_copy(unit,
                                                           iomd->ltid,
                                                           iomd->ltid_info,
                                                           iomd->entry_attrs,
                                                           slice,
                                                           curr_slice_id));
                curr_slice_id = curr_slice_id +
                     slice_fc[curr_slice_id].next_slice_id_offset[group_mode];
            } while ((curr_slice_id != prev_slice_id) &&
                     (curr_slice_id < iomd->num_slices));
        }
    }

    /* Initialize the fg structure with slice info */
    fg->group_id = iomd->entry_attrs->group_id;
    fg->primary_slice_id = slice;
    fg->priority = iomd->entry_attrs->group_prio;
    fg->group_mode = iomd->entry_attrs->group_mode;
    fg->group_ltid = iomd->entry_attrs->group_ltid;
    fg->num_presel_entries = iomd->entry_attrs->num_presel_ids;
    fg->pipe_id = iomd->entry_attrs->pipe_id;
    fg->num_entries = 0;
    fg->num_entries_ltid = 0;
    fg->next_shared_group_id = -1;
    fg->reserved_entry_index = -1;
    /*
     * Incase of warmboot from earlier version where reserve entry
     * implementation is not done, the below flag is used to
     * identify such scenario's and handle the groups which doesnt
     * have reserve_entry.
     */
    /*
     * Set Action prority and partition priority for the
     * group. Also set's the ING_CONFIGr (slice enable)
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_priority_set(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              stage_fc,
                                              iomd->entry_attrs,
                                              slice,
                                              slice_fc,
                                              slice_sharing));

    /* Install LT Action Priority. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                                                iomd->ltid,
                                                iomd->ltid_info,
                                                iomd->entry_attrs->pipe_id,
                                                stage_fc));

    /* Install LT partition Priority. */
    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  fg,
                                                  iomd->entry_attrs->pipe_id,
                                                  iomd->entry_attrs->stage_id));
    }
    pipe_id = iomd->entry_attrs->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_sharing == TRUE) {
        if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
            fg->num_presel_entries = primary_fg->num_presel_entries;
        } else {
            if ((iomd->entry_attrs->group_mode != primary_fg->group_mode &&
                iomd->entry_attrs->group_mode ==
                                   BCMPTM_RM_TCAM_GRP_MODE_SINGLE)) {
                fg->num_entries = primary_fg->num_entries * pri_slice_count;
            } else {
                fg->num_entries = primary_fg->num_entries;
            }
        }
        sal_memcpy(&(fg->entry_seg_bmp),
                   &(primary_fg->entry_seg_bmp),
                   sizeof(bcmptm_fp_entry_seg_bmp_t));
        fg->entry_info_seg = primary_fg->entry_info_seg;
        fg->entry_hash_size = primary_fg->entry_hash_size;
        fg->entry_hash_seg = primary_fg->entry_hash_seg;
        fg->presel_entry_info_seg = primary_fg->presel_entry_info_seg;
        fg->presel_entry_hash_size = primary_fg->presel_entry_hash_size;
        fg->presel_entry_hash_seg = primary_fg->presel_entry_hash_seg;

        if (iomd->entry_attrs->group_mode == primary_fg->group_mode) {
            /* Same mode groups can share the same reserve entry */
            fg->reserved_entry_index = primary_fg->reserved_entry_index;
            reserve_entry_found = true;
        } else {
            /*
             * 2 scenario's are possible
             *  1) There exists a shared group with same group mode as
             *     new group. In this case copy that shared group reserve
             *     entry index to new group and increment and copy
             *     shared_res_ent_group_count from existing group to new group.
             *  2) There is no existing group with same group mode as new group.
             *     Reserve a new entry with entry_type as iomd->entry_type and
             *     and update reserved_entry_index and shared_res_ent_group_count
             *     in new group.
             */
            /* Get next shared group */
            group_id = primary_fg->next_shared_group_id;
            while (group_id != (int16_t)iomd->entry_attrs->group_id &&
                   group_id != -1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_group_info_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->entry_attrs->pipe_id,
                                              group_id,
                                              stage_fc,
                                              &shared_fg));
                if (iomd->entry_attrs->group_mode == shared_fg->group_mode) {
                    /* Same mode groups can share the same reserve entry */
                    fg->reserved_entry_index = shared_fg->reserved_entry_index;
                    reserve_entry_found = true;
                    break;
                } else {
                    group_id = shared_fg->next_shared_group_id;
                }
            }
        }
    } else {
        if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            /* Set the valid bit */
            fg->valid = TRUE;
            fg->num_entries = slice_fc[slice].num_entries;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_fp_group_entry_segment_get(unit,
                                                      iomd->ltid,
                                                      iomd->ltid_info,
                                                      iomd->entry_attrs,
                                                      slice_count,
                                                      &segment_id));

            /*
             * Allocate segments for secondary slices in wider groups too,
             * to support sharing with a lower mode group having same priority
             */
            fg->entry_info_seg = (stage_fc->entry_info_seg)
                               + (((uint32_t)(stage_fc->max_entries_per_slice
                                  * segment_id))
                               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
            fg->entry_hash_size = (stage_fc->max_entries_per_slice);
            fg->entry_hash_seg = (stage_fc->entry_hash_seg)
                               + (((fg->entry_hash_size * segment_id))
                               * sizeof(int));
        }
        fg->presel_entry_info_seg = (stage_fc->presel_entry_info_seg)
                               + (((stage_fc->max_presel_entries_per_slice
                               * fg->primary_slice_id))
                               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
        fg->presel_entry_hash_size = (stage_fc->max_presel_entries_per_slice
                                      / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
        fg->presel_entry_hash_seg = (stage_fc->presel_entry_hash_seg)
                               + (((fg->presel_entry_hash_size
                                    * fg->primary_slice_id)) * sizeof(int));
    }

    fg->flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
    fg->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

    if (slice_fc[slice].slice_flags & BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION) {
        fg->flags = fg->flags | BCMPTM_RM_TCAM_F_AUTO_EXPANSION;
    }

    fg->valid = TRUE;
    iomd->group_ptr = fg;
    /*
     * Update iomd since new slice is allocated
     * Entry info array will change here
     */
    fg->reserve_entry_index_exists = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd));

    if (reserve_entry_found == false) {
       /*
        * There is no shared existing group with same mode
        * reserve an entry for entry update operations.
        */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_grp_reserve_entry(unit,
                                        iomd,
                                        &reserve_index));
        fg->reserved_entry_index = reserve_index;
        iomd->reserved_index = reserve_index;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Slice id:%d "
               "Mode :%d\n"), fg->primary_slice_id,fg->group_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To expand an existing group
 *  Allocate new slice/slices for existing ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table
 * \param [in] entry attrs structure
 * \param [out] updated the num_entires in
 *               field group structure
 * \param [out] new slice id allocated
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE No slice availble to expand
 */
int
bcmptm_rm_tcam_fp_slice_expand(int unit,
                     uint8_t *new_slice_id,
                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint8_t new_slice;         /* Slice iterator */
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
    bcmptm_rm_tcam_entry_attrs_t pri_entry_attr;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *temp_entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;
    /* Initialize field slice pointer. */
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    /* Initialize field group pointer. */
    fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;
    /*
     * Check whether the group under auto expansion
     * is indeed the right one
     */
    if (fg->group_id != entry_attr->group_id ||
        fg->priority != entry_attr->group_prio) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* construct entry_attr for primary fg */
    sal_memcpy(&pri_entry_attr, iomd->entry_attrs,
                    sizeof(bcmptm_rm_tcam_entry_attrs_t));

    if (fg->group_id != (uint32_t)slice_fc[fg->primary_slice_id].primary_grp_id) {
        /* groups are sharing and
         * auto expansion is called for non primary group.
         */

         /* use the primary fg for expanding slices */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_fp_group_info_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               entry_attr->pipe_id,
                                               slice_fc[fg->primary_slice_id].primary_grp_id,
                                               stage_fc,
                                               &pri_fg));

         pri_entry_attr.group_id = pri_fg->group_id;
         pri_entry_attr.group_mode  = pri_fg->group_mode;
         pri_entry_attr.group_ltid  = pri_fg->group_ltid;
    }


    
    if ((SHR_BITGET(entry_attr->flags.w,
        BCMPTM_FP_FLAGS_USE_SMALL_SLICE)) ||
        (SHR_BITGET(entry_attr->flags.w,
        BCMPTM_FP_FLAGS_USE_LARGE_SLICE))) {
        SHR_BITSET(pri_entry_attr.flags.w, BCMPTM_FP_FLAGS_AUTO_EXPANDING);
    }

    temp_entry_attrs = iomd->entry_attrs;
    iomd->entry_attrs = &pri_entry_attr;
    rv = bcmptm_rm_tcam_fp_group_reserve_slice(unit,
                                               iomd,
                                               &new_slice);
    iomd->entry_attrs = temp_entry_attrs;

    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }
    /* update the new_slice_id*/
    *new_slice_id = new_slice;

    /* Install selcodes in allocated slice */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_selcodes_copy(unit,
                                              iomd->ltid,iomd->ltid_info, &pri_entry_attr,
                                              fg->primary_slice_id,
                                              new_slice));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_profiles_copy(unit, iomd, new_slice));
    /*
     * Set partition priority for the
     * group. Also set's the ING_CONFIGr (slice enable)
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_partition_priority_set(unit, iomd->ltid,
            iomd->ltid_info,
            stage_fc, &pri_entry_attr, new_slice, slice_fc));

exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_rm_tcam_fp_half_mode_supported(int unit,
        bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;

    hw_entry_info = &(ltid_info->hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_HALF]);
    if (hw_entry_info->sid == NULL) {
        return FALSE;
    } else {
        return TRUE;
    }
}

int
bcmptm_rm_tcam_fp_max_entry_mode_get(int unit,
        bcmptm_rm_tcam_lt_info_t *ltid_info,
        bcmptm_rm_tcam_group_mode_t *grp_mode)
{
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    hw_entry_info = &(ltid_info->hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_HALF]);
    if (hw_entry_info->sid != NULL) {
        *grp_mode = BCMPTM_RM_TCAM_GRP_MODE_HALF;
    } else {
        *grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_tentative_entry_calculate(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                        uint32_t *entry_count)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_group_mode_t group_mode;
    uint8_t slice = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_count, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    size = (sizeof(bcmptm_rm_tcam_slice_t) * iomd->num_slices);
    SHR_ALLOC(slice_fc, size, "bcmptmRmTcamEntryTentativeCalc");
    sal_memcpy(slice_fc, iomd->slice_fc, size);

    iomd->slice_fc = slice_fc;
    group_mode = iomd->entry_attrs->group_mode;
    if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == group_mode ||
        BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        /*
         * Return E_CONFIG if the requested group mode is
         * Intra-Slice Double and Slices are not Intra Slice
         * capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode &&
           !(slice_fc->slice_flags &
             BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            *entry_count = 0;
            SHR_IF_ERR_EXIT(SHR_E_NONE);
        }
        while (rv != SHR_E_RESOURCE) {
            /* Reserve single slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_single_slice(unit,
                                                              iomd,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *entry_count += slice_fc[slice].num_entries;
            } else if (SHR_FAILURE(rv) && (rv != SHR_E_RESOURCE)) {
                SHR_ERR_EXIT(rv);
            }

        }
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == group_mode ||
              (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        /*
         * Return E_CONFIG if the requested group mode is Quad
         * and Slices are not Intra Slice capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode &&
           !(slice_fc->slice_flags &
             BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
        while (rv != SHR_E_RESOURCE) {
            /* Reserve single slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_double_slice(unit,
                                                              iomd,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *entry_count += slice_fc[slice].num_entries;
            } else if (SHR_FAILURE(rv) && (rv != SHR_E_RESOURCE)) {
                SHR_ERR_EXIT(rv);
            }
        }
    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == group_mode &&
               stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {

        while (rv != SHR_E_RESOURCE) {
            /* Reserve single slice for this new Field Group. */
            rv = bcmptm_rm_tcam_fp_group_reserve_triple_slice(unit,
                                                              iomd,
                                                              &slice,
                                                              TRUE);
            if (rv == SHR_E_NONE) {
                *entry_count += slice_fc[slice].num_entries;
            } else if (SHR_FAILURE(rv) && (rv != SHR_E_RESOURCE)) {
                SHR_ERR_EXIT(rv);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FREE(slice_fc);
    SHR_FUNC_EXIT();
}
