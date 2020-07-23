/*! \file bcmptm_rm_hash_traverse.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement traversal support for hash
 * table.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_entry_search.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_req_handler.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_traverse.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_narrow_mode_mgmt.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH
#define RM_HASH_MAX_PT_SNAP_NUM                      2
#define RM_HASH_MAX_PIPE_SNAP_NUM                    16

/*******************************************************************************
 * Typedefs
 */
/* ! \brief Structure used for storing an entry snapshot. */
typedef struct rm_hash_ent_snap_s {
    /*! \brief Buffer used for storing an entry snapshot. */
    uint32_t ent_words[BCMPTM_MAX_PT_ENTRY_WORDS];
} rm_hash_ent_snap_t;

/*
 * ! \brief Structure used for describing the snapshot info for a table
 * instance i.e. a pipe.
 */
typedef struct rm_hash_pipe_snap_s {
    /*! \brief Max possible entry count for a table instance. */
    uint32_t ent_max;

    /*! \brief Count of entries found for a table instance. */
    uint32_t ent_cnt;

    /*! \brief Buffer containing pointers to the sorted snapshot
     * for a table instance.
     */
    rm_hash_ent_snap_t **ent_snap_base;

    /*! \brief Buffer containing the entry snapshot. */
    rm_hash_ent_snap_t ent_snap_array[1];
} rm_hash_pipe_snap_t;

/*
 * ! \brief Structure describing the snapshot info for an LT mapping instance.
 */
typedef struct rm_hash_pt_snap_s {
    /*! \brief The widest sid of the physical table. */
    bcmdrd_sid_t wsid;

    /*! \brief Number of key formats on this physical tables. */
    int num_key_fmts;

    /*! \brief The key format of the physical table. */
    const bcmptm_rm_hash_key_format_t *key_fmt[RM_HASH_MAX_HW_ENTRY_INFO_COUNT];

    /*! \brief Group index of the key format. */
    uint8_t grp_idx[RM_HASH_MAX_HW_ENTRY_INFO_COUNT];

    /*! \brief Count of table instance snapshot for the LT. */
    int pipe_snap_cnt;

    /*! \brief Snapshot array for all the table instances. */
    rm_hash_pipe_snap_t *pipe_snap_array[1];
} rm_hash_pt_snap_t;

/*
 * ! \brief Structure used for describing the snapshot info for an LT.
 */
typedef struct rm_hash_map_travs_info_s {
    /*! \brief Count of separated PT snapshots. */
    uint8_t num_pt_snap;

    /*! \brief Pointer to traverse info for each PT snapshot. */
    rm_hash_pt_snap_t *pt_snap_array[RM_HASH_MAX_PT_SNAP_NUM];
} rm_hash_map_travs_info_t;

/*
 * ! \brief Structure used for describing the snapshot info list.
 */
typedef struct rm_hash_snap_list_s {
    /*! \brief Pointer to rm_hash_pt_snap_t structure. */
    rm_hash_pt_snap_t *pt_snap_p;

    /*! \brief Pointer to rm_hash_pipe_snap_t structure. */
    rm_hash_pipe_snap_t *pipe_snap_p;

    /*! \brief Pointer to pointer of rm_hash_ent_snap_t structure. */
    rm_hash_ent_snap_t **start_ent_snap_p;

    /*! \brief Pipe number. */
    int pipe;
} rm_hash_snap_list_t;

/*
 * ! \brief Structure describing the current entry location in traversal
 *          without snapshot.
 */
typedef struct rm_hash_travs_loc_s {
    /*! \brief Cache for current map index. */
    uint8_t map_index;

    /*! \brief Cache for current SID. */
    bcmdrd_sid_t sid;

    /*! \brief Cache for current entry index. */
    uint32_t ent_index;

    /*! \brief Cache for current pipe. */
    uint8_t pipe;

    /*! \brief Cache for current narrow mode info. */
    rm_hash_nme_info_t nme_info;
} rm_hash_travs_loc_t;

/*******************************************************************************
 * Private variables
 */
/*! \brief Cache for current map index during LT travsesal. */
static rm_hash_travs_loc_t travs_curr_loc[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Get the overlay sid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical hash table sid.
 * \param [out] ovly_sid Physical hash table overlay sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ovly_sid_get(int unit,
                     bcmdrd_sid_t sid,
                     bcmdrd_sid_t *ovly_sid)
{
    const bcmptm_overlay_info_t *ovly_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_overlay_info_get(unit, sid, &ovly_info));
    if ((ovly_info->mode != BCMPTM_OINFO_MODE_NONE)) {
        SHR_IF_ERR_EXIT
            (ovly_info->mode != BCMPTM_OINFO_MODE_HASH ?
                                SHR_E_INTERNAL : SHR_E_NONE);
        *ovly_sid = ovly_info->w_sid;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare the key portion of two entries.
 *
 * This function extracts and compares the key fields from the entry buffer.
 *
 * \param [in] e_words_a Entry a to be compared.
 * \param [in] e_words_b Entry b to be compared.
 * \param [in] key_fields Pointer to bcmlrd_hw_field_list_t structure of a LT.
 *
 * \retval 1  The former is larger than the later.
 * \retval -1 The former is smaller than the later.
 * \retval 0  The former is equal to the later.
 */
static int
rm_hash_travs_key_compare(const uint32_t *e_words_a,
                          const uint32_t *e_words_b,
                          const bcmlrd_hw_field_list_t *key_fields)
{
    uint8_t idx;
    int sbit, ebit;
    uint32_t key_field_a[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t key_field_b[BCMPTM_MAX_PT_FIELD_WORDS];

    for (idx = 0; idx < key_fields->num_fields; idx++) {
        uint16_t num_words;
        int word;
        sal_memset(key_field_a, 0, sizeof(key_field_a));
        sal_memset(key_field_b, 0, sizeof(key_field_b));
        sbit = key_fields->field_start_bit[idx];
        ebit = key_fields->field_start_bit[idx] + key_fields->field_width[idx] - 1;
        bcmdrd_field_get(e_words_a, sbit, ebit, key_field_a);
        bcmdrd_field_get(e_words_b, sbit, ebit, key_field_b);
        num_words = (key_fields->field_width[idx] + 31) / 32;
        for (word = num_words - 1; word >= 0; word--) {
            if (key_field_a[word] > key_field_b[word]) {
                return 1;
            }
            if (key_field_a[word] < key_field_b[word]) {
                return -1;
            }
        }
    }
    return 0;
}

/*!
 * \brief Get the first element of sorted entry snapshot list.
 *
 * This function get the first element of sorted entry snapshot list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_snap Pointer to rm_hash_pt_snap_t structure.
 * \param [in] pipe Table instance.
 * \param [out] ent_snap_base Buffer to record the first elem of sorted snap list.
 *
 * \retval NONE.
 */
static void
rm_hash_ent_snap_base_get(int unit,
                          rm_hash_pt_snap_t *pt_snap,
                          int pipe,
                          rm_hash_ent_snap_t ***ent_snap_base)
{
    rm_hash_pipe_snap_t *pipe_snap;

    pipe_snap = pt_snap->pipe_snap_array[pipe];
    *ent_snap_base = pipe_snap->ent_snap_base;
}

/*!
 * \brief Get the physical table sanpshot.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] map_travs_info Pointer to rm_hash_map_travs_info_t structure.
 * \param [out] pt_snap Pointer to rm_hash_pt_snap_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_snap_get(int unit,
                    bcmptm_rm_hash_req_t *req_info,
                    bcmptm_rm_hash_lt_info_t *lt_info,
                    rm_hash_map_travs_info_t *map_travs_info,
                    rm_hash_pt_snap_t **pt_snap)
{
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    rm_hash_pt_snap_t *tmp_pt_snap = NULL;
    bcmdrd_sid_t ovly_sid = INVALIDm, wsid = INVALIDm;
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    tmp_pt_snap = map_travs_info->pt_snap_array[0];

    if (map_travs_info->num_pt_snap > 1) {
        /*
         * There are multiple separated physical table snapshots,
         * and each of them will be iterated. This will happen for some CTH LT
         * on some old devices. Choose the correct one based on the requested info.
         * In this case, group will always be 0.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_get(unit,
                                           req_info->entry_attrs,
                                           lt_info,
                                           0,
                                           &key_format));
        SHR_NULL_CHECK(key_format, SHR_E_PARAM);
        /* Get the 1st view info for this key format. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_view_info_get(unit,
                                                     lt_info,
                                                     0,
                                                     key_format,
                                                     0,
                                                     &view_info));
        wsid = view_info->sid[0];
        SHR_IF_ERR_EXIT
            (rm_hash_ovly_sid_get(unit, wsid, &ovly_sid));
        if (ovly_sid != INVALIDm) {
            wsid = ovly_sid;
        }
        for (idx = 0; idx < map_travs_info->num_pt_snap; idx++) {
            tmp_pt_snap = map_travs_info->pt_snap_array[idx];
            if (tmp_pt_snap->wsid == wsid) {
                /* The correct traverse info has been found. */
                break;
            }
        }
        if (idx == map_travs_info->num_pt_snap) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
    *pt_snap = tmp_pt_snap;

exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Get the entry snapshot pointer for the given entry key.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [out] ent_snap_ptr Pointer to buffer containing entry snapshot.
 * \param [out] mmap_idx Buffer recording the matched group.
 * \param [out] pipe Buffer recording the matched pipe.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_snap_ptr_get(int unit,
                         rm_hash_lt_ctrl_t *lt_ctrl,
                         rm_hash_req_context_t *req_ctxt,
                         rm_hash_ent_snap_t ***ent_snap_ptr,
                         uint8_t *mmap_idx,
                         int *pipe)
{
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint8_t grp_idx, map_idx;
    uint32_t start, end, idx, mid;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    rm_hash_ent_snap_t **ent_snap_base = NULL, **curr = NULL;
    int cmp_result, pipe_cnt, pipe_idx;
    uint32_t num_ent_snap = 0;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;
    const bcmlrd_hw_field_list_t *key_fields = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    bcmptm_rm_hash_entry_attrs_t *entry_attrs = req_info->entry_attrs;

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        /*
         * For some older devices, the number of maps and groups is always 1.
         * The number of instances in a group/map may greater than 1.
         * And for some new devices, the number of instances in a map/group
         * may be greater than 1, but only 1 instance will be snapshoted in
         * traversal.
         */
        grp_idx = lt_ctrl->map_info[map_idx].grp_idx[0];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_get(unit, req_info->entry_attrs,
                                           lt_info, grp_idx, &key_format));
        SHR_NULL_CHECK(key_format, SHR_E_PARAM);
        if (key_format->hw_key_fields != NULL) {
            key_fields = key_format->hw_key_fields;
        } else if ((entry_attrs != NULL) && (entry_attrs->d_entry_attrs != NULL)) {
            key_fields = entry_attrs->d_entry_attrs->hw_key_fields;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_req_ent_view_info_get(unit, req_info, lt_info,
                                                  grp_idx, key_format,
                                                  &view_info));
        if (view_info == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (key_format->key_type_val != NULL) {
            bcmptm_rm_hash_key_type_fill(unit,
                                         key_format->key_type_val[0],
                                         view_info,
                                         req_info->entry_words);
        }
        map_travs_info = lt_ctrl->travs_info[map_idx];
        /*
         * grp_idx will be refered only when multiple physical table snapshots
         * are created for a map. This will happen for some CTH LT on some old
         * devices.
         */
        SHR_IF_ERR_EXIT
            (rm_hash_pt_snap_get(unit, req_info, lt_info,
                                 map_travs_info, &pt_snap));
        pipe_cnt = pt_snap->pipe_snap_cnt;
        pipe_idx = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;
        if (pipe_idx >= pipe_cnt) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        rm_hash_ent_snap_base_get(unit, pt_snap, pipe_idx, &ent_snap_base);
        pipe_snap = pt_snap->pipe_snap_array[pipe_idx];
        num_ent_snap = pipe_snap->ent_cnt;
        start = idx = mid = 0;
        end = num_ent_snap - 1;
        while (idx < num_ent_snap) {
            mid = (start + end) / 2;
            curr = ent_snap_base + mid;
            cmp_result = rm_hash_travs_key_compare(req_info->entry_words,
                                                   (*curr)->ent_words,
                                                   key_fields);
            if (cmp_result == 0) {
                *ent_snap_ptr = curr;
                *mmap_idx = map_idx;
                *pipe = pipe_idx;
                SHR_EXIT();
            } else if (cmp_result < 0) {
                end = mid - 1;
                if ((mid == 0) || (end < start)) {
                    break;
                }
            } else {
                start = mid + 1;
                if ((mid == num_ent_snap) || (start > end)) {
                    break;
                }
            }
            idx++;
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free memory space for the snapshot of a LT.
 *
 * This function takes snapshot for the specified LT for the following
 * GET_NEXT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_travs_snapshot_free(int unit,
                            rm_hash_lt_ctrl_t *lt_ctrl)
{
    int map_idx, pt_idx, pipe_idx, num_pt_snap;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_travs_info = lt_ctrl->travs_info[map_idx];
        if (map_travs_info != NULL) {
            num_pt_snap = map_travs_info->num_pt_snap;
            for (pt_idx = 0; pt_idx < num_pt_snap; pt_idx++) {
                pt_snap = map_travs_info->pt_snap_array[pt_idx];
                if (pt_snap != NULL) {
                    for (pipe_idx = 0; pipe_idx < pt_snap->pipe_snap_cnt; pipe_idx++) {
                        if (pt_snap->pipe_snap_array[pipe_idx] != NULL) {
                            sal_free(pt_snap->pipe_snap_array[pipe_idx]);
                            pt_snap->pipe_snap_array[pipe_idx] = NULL;
                        }
                    }
                    /*
                     * All the table instance snapshots has been released.
                     * Release traverse info for this LT.
                     */
                    sal_free(pt_snap);
                    map_travs_info->pt_snap_array[pt_idx] = NULL;
                }
            }

            sal_free(map_travs_info);
            lt_ctrl->travs_info[map_idx] = NULL;
        }
    }
}

/*!
 * \brief Get snapshot memory ratio between narrow mode entry and base entry.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] e_nm Entry narrow mode.
 * \param [in] ratio Buffer to store the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_snapshot_ratio_get(int unit,
                           rm_hash_entry_narrow_mode_t e_nm,
                           uint8_t *ratio)
{
    SHR_FUNC_ENTER(unit);

    switch (e_nm) {
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE:
        *ratio = 1;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF:
        *ratio = 2;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD:
        *ratio = 3;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate memory space for the snapshot of a LT.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_snapshot_alloc(int unit,
                             rm_hash_lt_ctrl_t *lt_ctrl,
                             bcmptm_rm_hash_req_t *req_info)
{
    rm_hash_pt_snap_t *pt_snap = NULL;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    uint8_t grp_idx, map_idx, idx, fmt_idx, key_fmt_idx, num_insts = 0;
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    bcmdrd_sid_t nsid = INVALIDm;
    bcmdrd_sid_t ovly_sid = INVALIDm, wsid = INVALIDm;
    int pipe_cnt = 0, pt_idx, num_pt_snap, pipe_idx;
    uint32_t ent_idx_cnt = 0;
    uint32_t pt_snap_size = 0, pipe_snap_size = 0;
    rm_hash_pipe_snap_t * pipe_snap = NULL;
    bool snapshot_allocated = FALSE;
    rm_hash_fmt_info_t *fmt_info = NULL;
    uint8_t ratio;

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        /*
         * On some older devices, for CTH LT, a map will contain all the valid
         * formats. LT traverse operation will iterate each of the formats.
         * On some new devices, some LTs support multiple lookups, and each
         * lookup will correspond to an instance. In this case, traversal on
         * one instance will be enough, as all the instances have the same bit
         * field mapping.
         */
        num_insts = lt_ctrl->map_info[map_idx].num_insts;
        if (req_info->entry_attrs == NULL) {
            /*
             * For non-CTH LT, only single snapshot is required. Even
             * there are multiple key formats in a map, the entry
             * installed corresponding to this group will be identical.
             */
            num_insts = 1;
        }
        for (idx = 0; idx < num_insts; idx++) {
            /* Get key format information on this group. */
            fmt_idx    = lt_ctrl->map_info[map_idx].fmt_idx[idx];
            fmt_info   = &lt_ctrl->fmt_info[fmt_idx];
            key_format = fmt_info->key_format;
            SHR_NULL_CHECK(key_format, SHR_E_PARAM);

            /*
             * Get the 1st view info for this key format. The group index and
             * key format have strict corresponding relation.
             */
            grp_idx = lt_ctrl->map_info[map_idx].grp_idx[idx];
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_format_view_info_get(unit,
                                                         lt_info,
                                                         grp_idx,
                                                         key_format,
                                                         0,
                                                         &view_info));
            nsid = view_info->sid[0];
            wsid = view_info->sid[0];
            SHR_IF_ERR_EXIT
                (rm_hash_ovly_sid_get(unit, wsid, &ovly_sid));
            if (ovly_sid != INVALIDm) {
                wsid = ovly_sid;
            }
            if (lt_ctrl->travs_info[map_idx] == NULL) {
                map_travs_info = sal_alloc(sizeof(rm_hash_map_travs_info_t),
                                           "bcmptmRmhashLtTraverseInfo");
                if (map_travs_info == NULL) {
                    SHR_ERR_EXIT(SHR_E_MEMORY);
                }
                sal_memset(map_travs_info, 0, sizeof(rm_hash_map_travs_info_t));
                lt_ctrl->travs_info[map_idx] = map_travs_info;
            }
            /*
             * Check if the snapshot for the actual physical table has been
             * allocated.
             */
            map_travs_info = lt_ctrl->travs_info[map_idx];
            snapshot_allocated = FALSE;
            for (pt_idx = 0; pt_idx < map_travs_info->num_pt_snap; pt_idx++) {
                pt_snap = map_travs_info->pt_snap_array[pt_idx];
                if ((pt_snap != NULL) && (pt_snap->wsid == wsid)) {
                    /*
                     * The table snapshot for the specified SID has been
                     * allocated.
                     */
                    snapshot_allocated = TRUE;
                    break;
                }
            }
            if (snapshot_allocated == FALSE) {
                pipe_cnt = bcmdrd_pt_num_tbl_inst(unit, nsid);
                if (pipe_cnt < 0) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                if (lt_ctrl->is_per_pipe == FALSE) {
                    /*
                     * The LT is configured to work in global mode (even the
                     * physical table has unique access type).
 */
                    pipe_cnt = 1;
                }
                pt_snap_size = sizeof(rm_hash_pipe_snap_t *) * pipe_cnt +
                               sizeof(rm_hash_pt_snap_t);
                pt_snap = sal_alloc(pt_snap_size,
                                    "bcmptmRmhashLtInstTraverseInfo");
                if (pt_snap == NULL) {
                    SHR_ERR_EXIT(SHR_E_MEMORY);
                }
                sal_memset(pt_snap, 0, pt_snap_size);
                num_pt_snap = map_travs_info->num_pt_snap;
                map_travs_info->pt_snap_array[num_pt_snap] = pt_snap;
                map_travs_info->num_pt_snap++;
                pt_snap->wsid = wsid;
                pt_snap->pipe_snap_cnt = pipe_cnt;

                /*
                 * The maximum possible entry number is used.
                 * In future once we have count of entries in LT - then we can
                 * use that to allocate the entry.
                 */
                ent_idx_cnt = bcmptm_pt_index_count(unit, nsid);
                SHR_IF_ERR_EXIT
                    (rm_hash_snapshot_ratio_get(unit, lt_ctrl->e_nm, &ratio));
                ent_idx_cnt = ent_idx_cnt * ratio;
                pipe_snap_size = (sizeof(rm_hash_ent_snap_t) +
                                  sizeof(rm_hash_ent_snap_t *)) *
                                 ent_idx_cnt + sizeof(rm_hash_pipe_snap_t);

                for (pipe_idx = 0; pipe_idx < pipe_cnt; pipe_idx++) {
                    pipe_snap = sal_alloc(pipe_snap_size,
                                          "bcmptmRmhashTableInstanceSnapshot");
                    if (pipe_snap == NULL) {
                        SHR_ERR_EXIT(SHR_E_MEMORY);
                    }
                    sal_memset(pipe_snap, 0, pipe_snap_size);
                    pipe_snap->ent_max = ent_idx_cnt;
                    pipe_snap->ent_snap_base = (rm_hash_ent_snap_t **)(pipe_snap->ent_snap_array + ent_idx_cnt);
                    pt_snap->pipe_snap_array[pipe_idx] = pipe_snap;
                }
            }
            /* Check if the key format has been recorded in the snapshot. */
            for (key_fmt_idx = 0; key_fmt_idx < pt_snap->num_key_fmts; key_fmt_idx++) {
                if (pt_snap->key_fmt[key_fmt_idx] == key_format) {
                    break;
                }
            }
            if (key_fmt_idx == pt_snap->num_key_fmts) {
                pt_snap->key_fmt[key_fmt_idx] = key_format;
                pt_snap->grp_idx[key_fmt_idx] = grp_idx;
                pt_snap->num_key_fmts++;
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        rm_hash_travs_snapshot_free(unit, lt_ctrl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Sort the entry snapshot array.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_snap_array Pointer to rm_hash_ent_snap_t * structure.
 * \param [in] num_elems Number of element in the array.
 * \param [in] key_fields Pointer to bcmlrd_hw_field_list_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_merge_sort(int unit,
                         rm_hash_ent_snap_t **ent_snap_array,
                         int num_elems,
                         const bcmlrd_hw_field_list_t *key_fields)
{
    int left, rght, rend, size;
    int i, j, m, result;
    rm_hash_ent_snap_t **tmp_elems = NULL;

    SHR_FUNC_ENTER(unit);

    tmp_elems = sal_alloc(num_elems * sizeof(rm_hash_ent_snap_t *),
                          "bcmptmRmhashLtTraverseInfo");
    if (tmp_elems == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    for (size = 1; size < num_elems; size *= 2) {
        for (left = 0; left + size < num_elems; left += size * 2 ) {
            rght = left + size;
            rend = rght + size;
            if (rend > num_elems) {
                rend = num_elems;
            }
            m = left;
            i = left;
            j = rght;
            while (i < rght && j < rend) {
                result = rm_hash_travs_key_compare(ent_snap_array[i]->ent_words,
                                                   ent_snap_array[j]->ent_words,
                                                   key_fields);
                if (result < 0) {
                    tmp_elems[m] = ent_snap_array[i];
                    i++;
                } else {
                    tmp_elems[m] = ent_snap_array[j];
                    j++;
                }
                m++;
            }
            while (i < rght) {
                tmp_elems[m] = ent_snap_array[i];
                i++;
                m++;
            }
            while (j < rend) {
                tmp_elems[m] = ent_snap_array[j];
                j++;
                m++;
            }
            for (m = left; m < rend; m++) {
                ent_snap_array[m] = tmp_elems[m];
            }
        }
    }

exit:
    if (tmp_elems != NULL) {
        sal_free(tmp_elems);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Take the snapshot of a LT.
 *
 * This function takes snapshot for the specified LT for the following
 * GET_NEXT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_snapshot_build(int unit,
                             rm_hash_req_context_t *req_ctxt,
                             rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint8_t grp_idx, map_idx, view_idx, num_views = 0;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmlrd_hw_field_list_t *key_fields = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    bcmdrd_sid_t sid, grp_sid;
    int index_min, index_max, ent_index;
    int pipe_cnt = 0, pipe_idx;
    uint32_t ent_index_cnt = 0;
    rm_hash_ent_snap_t **ent_snap_base = NULL;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    rm_hash_ent_snap_t *ent_snap = NULL;
    uint32_t *ent_snap_words = NULL;
    bcmptm_rm_hash_entry_attrs_t *entry_attrs = req_ctxt->req_info->entry_attrs;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], rbank_cnt, rbank = 0;
    uint8_t ltid_loc_list[RM_HASH_NARROW_ENTRY_LITD_NUM] = {0};
    uint8_t num_ltids;
    rm_hash_entry_narrow_mode_t e_nm;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_travs_info = lt_ctrl->travs_info[map_idx];
        if (map_travs_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        grp_idx = lt_ctrl->map_info[map_idx].grp_idx[0];
        SHR_IF_ERR_EXIT
            (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                                 map_travs_info, &pt_snap));
        /*
         * Get key format information on this pt snapshot when there are
         * multiple key formats on it. This will happen for some CTH LT on
         * some old devices.
         */
        if (pt_snap->num_key_fmts > 1) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_format_get(unit,
                                               req_ctxt->req_info->entry_attrs,
                                               lt_info,
                                               grp_idx,
                                               &key_format));
        } else {
            key_format = pt_snap->key_fmt[0];
            grp_idx    = pt_snap->grp_idx[0];
        }

        SHR_NULL_CHECK(key_format, SHR_E_PARAM);
        if (key_format->hw_key_fields != NULL) {
            key_fields = key_format->hw_key_fields;
        } else if ((entry_attrs != NULL) && (entry_attrs->d_entry_attrs != NULL)) {
            key_fields = entry_attrs->d_entry_attrs->hw_key_fields;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        bcmptm_rm_hash_key_format_num_view_get(unit, key_format, &num_views);
        for (view_idx = 0; view_idx < num_views; view_idx++) {
            /* Group index and key format has strict corresponding relation. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_format_view_info_get(unit,
                                                         lt_info,
                                                         grp_idx,
                                                         key_format,
                                                         view_idx,
                                                         &view_info));
            sid = *view_info->sid;
            index_min = bcmptm_pt_index_min(unit, sid);
            ent_index_cnt = bcmptm_pt_index_count(unit, sid);
            index_max = index_min + (int)ent_index_cnt - 1;
            /*
             * It is assumed that narrow mode entry will be always mapped onto
             * the single view.
             */
            if ((lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                (lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, sid, &grp_sid));
                bcmptm_rm_hash_sid_rbank_list_get(unit, grp_sid, pt_info, rbank_list,
                                                  &rbank_cnt);
                if (rbank_cnt > 1) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
                rbank = rbank_list[0];
            }
            pipe_cnt = pt_snap->pipe_snap_cnt;
            for (pipe_idx = 0; pipe_idx < pipe_cnt; pipe_idx++) {
                pipe_snap = pt_snap->pipe_snap_array[pipe_idx];
                if (pipe_snap == NULL) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                rm_hash_ent_snap_base_get(unit, pt_snap, pipe_idx, &ent_snap_base);
                for (ent_index = index_min; ent_index <= index_max; ent_index++) {
                    bool cache_valid = FALSE;
                    bcmltd_sid_t rsp_ltid;
                    uint16_t dfield_format_id;
                    uint32_t rsp_flags;
                    bcmbd_pt_dyn_info_t dyn_info;

                    if (lt_ctrl->is_per_pipe == FALSE) {
                        /* The physical table works in global mode. */
                        dyn_info.tbl_inst = -1;
                    } else {
                        dyn_info.tbl_inst = pipe_idx;
                    }

                    dyn_info.index = ent_index;
                    ent_snap = &pipe_snap->ent_snap_array[pipe_snap->ent_cnt];
                    ent_snap_words = ent_snap->ent_words;
                    sal_memset(ent_snap, 0, sizeof(*ent_snap));
                    SHR_IF_ERR_EXIT
                        (bcmptm_cmdproc_read(unit,
                                             BCMPTM_REQ_FLAGS_NO_FLAGS,
                                             0,
                                             sid,
                                             &dyn_info,
                                             NULL, /* misc_info */
                                             BCMPTM_MAX_PT_ENTRY_WORDS,
                                             ent_snap_words,
                                             &cache_valid,
                                             &rsp_ltid,
                                             &dfield_format_id,
                                             &rsp_flags));
                    if (cache_valid == FALSE) {
                        continue;
                    }
                    if (rsp_ltid == lt_ctrl->ltid) {
                        /*
                         * Relying on ltid in PTcache, and ltid in PTcache is
                         * the requested LTID.
                         */
                        ent_snap_base[pipe_snap->ent_cnt] = ent_snap;
                        pipe_snap->ent_cnt++;
                    }
                    if ((lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                        (lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
                        uint32_t nm_ent[BCMPTM_MAX_PT_FIELD_WORDS];
                        uint8_t ltid_idx;

                        /* Check the narrow mode ltid further. */
                        SHR_IF_ERR_EXIT
                            (bcmptm_rm_hash_matched_ltid_get(unit,
                                                             lt_ctrl->ltid,
                                                             pt_info,
                                                             rbank,
                                                             pipe_idx,
                                                             ent_index,
                                                             ltid_loc_list,
                                                             &num_ltids,
                                                             &e_nm));

                        sal_memcpy(nm_ent, ent_snap_words, sizeof(nm_ent));
                        sal_memset(ent_snap_words, 0, sizeof(nm_ent));
                        for (ltid_idx = 0; ltid_idx < num_ltids; ltid_idx++) {
                            ent_snap = &pipe_snap->ent_snap_array[pipe_snap->ent_cnt];
                            SHR_IF_ERR_EXIT
                                (bcmptm_rm_hash_nm_entry_extract(unit,
                                                                 pt_info,
                                                                 rbank,
                                                                 e_nm,
                                                                 ltid_loc_list[ltid_idx],
                                                                 nm_ent,
                                                                 ent_snap->ent_words));
                            ent_snap_base[pipe_snap->ent_cnt] = ent_snap;
                            pipe_snap->ent_cnt++;
                        }
                    }
                }
                if (pipe_snap->ent_cnt > 0) {
                    rm_hash_travs_merge_sort(unit,
                                             ent_snap_base,
                                             pipe_snap->ent_cnt,
                                             key_fields);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the count of the entry to be traversed.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [out] ent_cnt Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_ent_cnt_get(int unit,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_req_context_t *req_ctxt,
                          uint32_t *ent_cnt)
{
    uint8_t map_idx;
    uint32_t tmp_cnt = 0;
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    int pipe_cnt, pipe_idx;

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_travs_info = lt_ctrl->travs_info[map_idx];
        if (map_travs_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT
            (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                                 map_travs_info, &pt_snap));
        pipe_cnt = pt_snap->pipe_snap_cnt;
        for (pipe_idx = 0; pipe_idx < pipe_cnt; pipe_idx++) {
            pipe_snap = pt_snap->pipe_snap_array[pipe_idx];
            tmp_cnt += pipe_snap->ent_cnt;
        }
    }
    *ent_cnt = tmp_cnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the first physical table snapshot.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [out] pt_snap Buffer to hold the result.
 * \param [out] pipe Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_first_pt_snap_get(int unit,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_req_context_t *req_ctxt,
                          rm_hash_pt_snap_t **pt_snap,
                          int *pipe)
{
    uint8_t map_idx;
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_pt_snap_t *tmp_pt_snap = NULL;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    int pipe_cnt, pipe_idx;

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_travs_info = lt_ctrl->travs_info[map_idx];
        if (map_travs_info != NULL) {
            SHR_IF_ERR_EXIT
                (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                                     map_travs_info,
                                     &tmp_pt_snap));
            if (tmp_pt_snap != NULL) {
                pipe_cnt = tmp_pt_snap->pipe_snap_cnt;
                for (pipe_idx = 0; pipe_idx < pipe_cnt; pipe_idx++) {
                    pipe_snap = tmp_pt_snap->pipe_snap_array[pipe_idx];
                    if ((pipe_snap != NULL) && (pipe_snap->ent_cnt > 0)) {
                        /* LT has entry in this group. */
                        *pt_snap = tmp_pt_snap;
                        *pipe = pipe_idx;
                        SHR_EXIT();
                    }
                }
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_FIRST opcode with snapshot.
 *
 * This function handle GET_FIRST opcode
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_first_with_snap(int unit,
                            rm_hash_req_context_t *req_ctxt,
                            size_t rsp_entry_buf_wsize,
                            uint32_t *rsp_entry_words,
                            bcmptm_rm_hash_rsp_t *rsp_info,
                            bcmltd_sid_t *rsp_ltid,
                            uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;
    int pipe, rv = SHR_E_NONE;
    uint32_t ent_cnt = 0;
    rm_hash_ent_snap_t **ent_snap_base = NULL;
    rm_hash_ent_snap_t *ent_snap = NULL;
    bcmptm_rm_hash_req_t tmp_req_info;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    uint8_t map_index = 0;
    rm_hash_nme_info_t nme_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    rv = rm_hash_first_pt_snap_get(unit, lt_ctrl, req_ctxt, &pt_snap, &pipe);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    /*
     * Examine if this is used for terminating the previous traversal
     * prematurely.
     */
    if ((pt_snap != NULL) && (pt_snap->pipe_snap_array[pipe] != NULL)) {
        rm_hash_travs_snapshot_free(unit, lt_ctrl);
    }

    /*
     * The snapshot for the specified table instance needs to
     * be allocated and built.
     */
    SHR_IF_ERR_EXIT
        (rm_hash_travs_snapshot_alloc(unit, lt_ctrl, req_ctxt->req_info));
    SHR_IF_ERR_EXIT
        (rm_hash_travs_snapshot_build(unit, req_ctxt, lt_ctrl));
    SHR_IF_ERR_EXIT
        (rm_hash_travs_ent_cnt_get(unit, lt_ctrl, req_ctxt, &ent_cnt));
    if (ent_cnt == 0) {
        /* There is no valid entry for this LT. */
        rm_hash_travs_snapshot_free(unit, lt_ctrl);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_first_pt_snap_get(unit, lt_ctrl, req_ctxt, &pt_snap, &pipe));
    /* Retrieve the first entry snapshot (smallest entry) address */
    rm_hash_ent_snap_base_get(unit, pt_snap, pipe, &ent_snap_base);
    ent_snap = *ent_snap_base;
    /*
     * Lookup the first entry snapshot in the PTcache for data portion.
     * Stuff the req_info before invoking lookup operation.
     */
    sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
    tmp_req_info.entry_words = ent_snap->ent_words;
    tmp_req_info.same_key = FALSE;
    tmp_req_info.entry_attrs = req_ctxt->req_info->entry_attrs;
    if (lt_ctrl->is_per_pipe == FALSE ) {
        dyn_info.tbl_inst = -1;
    } else {
        dyn_info.tbl_inst = pipe;
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lookup(unit,
                               req_ctxt->req_flags,
                               req_ctxt->req_ltid,
                               &dyn_info,
                               &tmp_req_info,
                               rsp_entry_buf_wsize,
                               rsp_entry_words,
                               rsp_info,
                               rsp_ltid,
                               rsp_flags,
                               &map_index,
                               &nme_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pipe snapshot list.
 *
 * This function get pipe snapshot list.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] curr_map_idx Current group index.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] curr_pipe Current pipe.
 * \param [in] curr_ent_snap_p Current entry snapshot.
 * \param [in] pipe_cnt Pipe number.
 * \param [out] snap_list Pointer to rm_hash_snap_list_t object.
 * \param [out] snap_cnt Total number of pipe snapshots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_pipe_snap_list_get(int unit,
                                 rm_hash_req_context_t *req_ctxt,
                                 uint8_t curr_map_idx,
                                 rm_hash_lt_ctrl_t *lt_ctrl,
                                 bcmptm_rm_hash_lt_info_t *lt_info,
                                 int curr_pipe,
                                 rm_hash_ent_snap_t **curr_ent_snap_p,
                                 int pipe_cnt,
                                 rm_hash_snap_list_t *snap_list,
                                 int *snap_cnt)
{
    uint8_t map_idx;
    rm_hash_map_travs_info_t *tmp_travs_info = NULL;
    rm_hash_pt_snap_t *pt_snap = NULL;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    int pipe_idx, idx = 0;
    rm_hash_ent_snap_t **ent_snap_base = NULL;

    SHR_FUNC_ENTER(unit);

    tmp_travs_info = lt_ctrl->travs_info[curr_map_idx];
    SHR_IF_ERR_EXIT
        (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                             tmp_travs_info, &pt_snap));
    pipe_snap = pt_snap->pipe_snap_array[curr_pipe];
    rm_hash_ent_snap_base_get(unit, pt_snap, curr_pipe, &ent_snap_base);
    if (((curr_ent_snap_p - ent_snap_base) < ((int)pipe_snap->ent_cnt - 1))) {
        /*
         * There are still some entries in current (group, pipe) need to be examined.
         * Next entry will be the next entry in the snapshot of current (group, pipe).
         */
        snap_list[idx].pipe_snap_p = pipe_snap;
        snap_list[idx].pt_snap_p = pt_snap;
        snap_list[idx].pipe = curr_pipe;
        snap_list[idx].start_ent_snap_p = curr_ent_snap_p + 1;
        idx++;
    }

    /* Get all the snapshots for the current pipe. */
    for (map_idx = curr_map_idx + 1; map_idx < lt_ctrl->num_maps; map_idx++) {
        tmp_travs_info = lt_ctrl->travs_info[map_idx];
        SHR_IF_ERR_EXIT
            (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                                 tmp_travs_info, &pt_snap));
        pipe_snap = pt_snap->pipe_snap_array[curr_pipe];
        if (pipe_snap->ent_cnt > 0) {
            snap_list[idx].pipe_snap_p = pipe_snap;
            snap_list[idx].pt_snap_p = pt_snap;
            snap_list[idx].pipe = curr_pipe;
            rm_hash_ent_snap_base_get(unit, pt_snap, curr_pipe, &ent_snap_base);
            snap_list[idx].start_ent_snap_p = ent_snap_base;
            idx++;
        }
    }

    /* Get all the snapshots for the remaining pipes.  */
    for (pipe_idx = curr_pipe + 1; pipe_idx < pipe_cnt; pipe_idx++) {
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            tmp_travs_info = lt_ctrl->travs_info[map_idx];
            SHR_IF_ERR_EXIT
                (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                                     tmp_travs_info, &pt_snap));
            pipe_snap = pt_snap->pipe_snap_array[pipe_idx];
            if (pipe_snap->ent_cnt > 0) {
                snap_list[idx].pipe_snap_p = pipe_snap;
                snap_list[idx].pt_snap_p = pt_snap;
                snap_list[idx].pipe = pipe_idx;
                rm_hash_ent_snap_base_get(unit, pt_snap, pipe_idx, &ent_snap_base);
                snap_list[idx].start_ent_snap_p = ent_snap_base;
                idx++;
            }
        }
    }
    *snap_cnt = idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get next entry for GET_NEXT opcode.
 *
 * This function get next entry for GET_NEXT opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] snap_list Pointer to rm_hash_snap_list_t structure.
 * \param [in] snap_cnt Number fo snapshots.
 * \param [in] rsp_entry_buf_wsize Size of the response buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_pipe_snap_next_entry_get(int unit,
                                       rm_hash_req_context_t *req_ctxt,
                                       rm_hash_lt_ctrl_t *lt_ctrl,
                                       rm_hash_snap_list_t *snap_list,
                                       int snap_cnt,
                                       size_t rsp_entry_buf_wsize,
                                       uint32_t *rsp_entry_words,
                                       bcmptm_rm_hash_rsp_t *rsp_info,
                                       bcmltd_sid_t *rsp_ltid,
                                       uint32_t *rsp_flags)
{
    int idx, tmp_rv;
    rm_hash_snap_list_t *curr_snap = snap_list;
    rm_hash_pt_snap_t *pt_snap = NULL;
    rm_hash_pipe_snap_t *pipe_snap = NULL;
    rm_hash_ent_snap_t **ent_snap_ptr = NULL, **ent_snap_base = NULL;
    rm_hash_ent_snap_t *ent_snap = NULL;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    bcmptm_rm_hash_req_t tmp_req_info;
    uint8_t map_index = 0;
    rm_hash_nme_info_t nme_info;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < snap_cnt; idx++, curr_snap++) {
        pt_snap = curr_snap->pt_snap_p;
        pipe_snap = curr_snap->pipe_snap_p;
        ent_snap_ptr = curr_snap->start_ent_snap_p;
        rm_hash_ent_snap_base_get(unit, pt_snap, curr_snap->pipe, &ent_snap_base);

        for (; (ent_snap_ptr - ent_snap_base) < (int)pipe_snap->ent_cnt; ent_snap_ptr++) {
            ent_snap = *ent_snap_ptr;

            sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
            tmp_req_info.entry_words = ent_snap->ent_words;
            tmp_req_info.same_key = FALSE;
            tmp_req_info.entry_attrs = req_ctxt->req_info->entry_attrs;

            if (lt_ctrl->is_per_pipe == TRUE) {
                dyn_info.tbl_inst = curr_snap->pipe;
            }
            tmp_rv = bcmptm_rm_hash_lookup(unit,
                                           req_ctxt->req_flags,
                                           req_ctxt->req_ltid,
                                           &dyn_info,
                                           &tmp_req_info,
                                           rsp_entry_buf_wsize,
                                           rsp_entry_words,
                                           rsp_info,
                                           rsp_ltid,
                                           rsp_flags,
                                           &map_index,
                                           &nme_info);
            if (tmp_rv == SHR_E_NOT_FOUND) {
                /*
                 * The entry has been deleted after the snapshot is created.
                 * This is an expected behavior, and the next element will be
                 * retrieved and evaluated.
                 */
                continue;
            } else {
                /*
                 * Return when the lookup succeeds, i.e. the entry exists,
                 * or a non-NOT_FOUND error code is encountered.
                 */
                SHR_ERR_EXIT(tmp_rv);
            }
        }
    }
    /* All the entries have been deleted. */
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_NEXT opcode.
 *
 * This function handle GET_NEXT opcode
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_next_with_snap(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           size_t rsp_entry_buf_wsize,
                           uint32_t *rsp_entry_words,
                           bcmptm_rm_hash_rsp_t *rsp_info,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmptm_rm_hash_lt_info_t *lt_info = NULL;
    int pipe;
    int tmp_rv;
    rm_hash_pt_snap_t *pt_snap = NULL;
    rm_hash_ent_snap_t **ent_snap_ptr = NULL;
    uint8_t mmap_idx = 0;
    rm_hash_map_travs_info_t *map_travs_info = NULL;
    rm_hash_snap_list_t snap_list[RM_HASH_MAX_PIPE_SNAP_NUM];
    int snap_cnt = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    lt_info = &lt_ctrl->lt_info;

    tmp_rv = rm_hash_first_pt_snap_get(unit, lt_ctrl, req_ctxt, &pt_snap, &pipe);
    SHR_IF_ERR_EXIT_EXCEPT_IF(tmp_rv, SHR_E_NOT_FOUND);
    if ((pt_snap == NULL) || (pt_snap->pipe_snap_array[pipe] == NULL)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    /*
     * User must specify the "curr" entry key for GET_NEXT operation.
     * Search the key in the snapshot.
     */
    tmp_rv = rm_hash_ent_snap_ptr_get(unit, lt_ctrl, req_ctxt,
                                      &ent_snap_ptr, &mmap_idx, &pipe);
    if (tmp_rv == SHR_E_NOT_FOUND) {
        /*
         * User specifies a non-existing entry key,
         * snapshot will be released in this case.
         */
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "A non-existing entry was specified...\n")));
        /* DO NOT implicitly release the snapshot. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_EXIT(tmp_rv);
    /*
     * Get the group within which the specified entry exists
     * and corresponding entry snapshot base.
     */
    map_travs_info = lt_ctrl->travs_info[mmap_idx];
    SHR_IF_ERR_EXIT
        (rm_hash_pt_snap_get(unit, req_ctxt->req_info, lt_info,
                             map_travs_info, &pt_snap));

    SHR_IF_ERR_EXIT
        (rm_hash_travs_pipe_snap_list_get(unit, req_ctxt, mmap_idx, lt_ctrl,
                                          lt_info, pipe, ent_snap_ptr,
                                          pt_snap->pipe_snap_cnt,
                                          snap_list, &snap_cnt));
    if (snap_cnt == 0) {
        /*
         * The "curr" entry snap is already the last entry snapshot,
         * the snapshot will be released in this case.
         */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "curr key is the last...\n")));
        /* DO NOT implicitly release the snapshot. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    tmp_rv = rm_hash_travs_pipe_snap_next_entry_get(unit, req_ctxt, lt_ctrl,
                                                    snap_list, snap_cnt,
                                                    rsp_entry_buf_wsize,
                                                    rsp_entry_words,
                                                    rsp_info, rsp_ltid,
                                                    rsp_flags);
    if (tmp_rv == SHR_E_NONE) {
        SHR_EXIT();
    } else if (tmp_rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "There is no valid entry...\n")));
        /* DO NOT implicitly release the snapshot. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        SHR_ERR_EXIT(tmp_rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get valid narrow mode LT entry.
 *
 * This function get valid narrow mode LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] sid Physical table symbol id.
 * \param [in] pipe Table pipe number.
 * \param [in] be_index Base entry index.
 * \param [in] start_nme_idx Start narrow mode entry index within the base entry.
 * \param [in] e_nm Entry narrow mode.
 * \param [out] entry_found Buffer to store the etry search result.
 * \param [out] rsp_entry_words Buffer to store the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_valid_narrow_entry(int unit,
                               rm_hash_req_context_t *req_ctxt,
                               bcmdrd_sid_t sid,
                               int pipe,
                               int be_index,
                               uint8_t start_nme_idx,
                               rm_hash_entry_narrow_mode_t e_nm,
                               bool *entry_found,
                               uint32_t *rsp_entry_words)
{
    rm_hash_pt_info_t *pt_info = NULL;
    bcmdrd_sid_t grp_sid;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], rbank_cnt, rbank, idx;
    bcmltd_sid_t nme_ltid;

    SHR_FUNC_ENTER(unit);

    *entry_found = FALSE;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_grp_sid_get(unit, sid, &grp_sid));
    bcmptm_rm_hash_sid_rbank_list_get(unit, grp_sid, pt_info,
                                      rbank_list,
                                      &rbank_cnt);
    rbank = rbank_list[0];
    for (idx = start_nme_idx; idx < RM_HASH_NARROW_ENTRY_LITD_NUM; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_nme_ltid_get(unit, pt_info, rbank, pipe,
                                         be_index, idx, &nme_ltid));
        if (nme_ltid == req_ctxt->req_ltid) {
            uint32_t nm_ent[BCMPTM_MAX_PT_FIELD_WORDS];

            sal_memcpy(nm_ent, rsp_entry_words, sizeof(nm_ent));
            sal_memset(rsp_entry_words, 0, sizeof(nm_ent));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_nm_entry_extract(unit, pt_info, rbank,
                                                 e_nm, idx, nm_ent,
                                                 rsp_entry_words));
            *entry_found = TRUE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the first valid entry in a specific pipe for an SID.
 *
 * This function get the first valid entry in a pipe for an SID.
 *
 * \param [in] unit Unit number.
 * \param [in] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] tbl_inst Table instance used for read PTcache.
 * \param [in] start_index Entry index from where to search valid entry.
 * \param [in] rsp_entry_buf_wsize Size of the response buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_valid_entry_in_pipe(int unit,
                                const bcmptm_rm_hash_view_info_t *view_info,
                                rm_hash_req_context_t *req_ctxt,
                                int tbl_inst,
                                int start_index,
                                rm_hash_nme_info_t *start_nme_info,
                                size_t rsp_entry_buf_wsize,
                                uint32_t *rsp_entry_words,
                                bcmptm_rm_hash_rsp_t *rsp_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags)
{
    bcmdrd_sid_t sid;
    int index_min, index_max, index;
    uint32_t ent_index_cnt;
    bool cache_valid = FALSE;
    bcmltd_sid_t tmp_ltid;
    uint16_t dfield_format_id;
    uint32_t tmp_rsp_flags;
    bcmbd_pt_dyn_info_t dyn_info;
    uint8_t start_nme_idx;
    int pipe;
    bool entry_found = FALSE;

    SHR_FUNC_ENTER(unit);

    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    sid = view_info->sid[0];
    index_min = bcmptm_pt_index_min(unit, sid);
    ent_index_cnt = bcmptm_pt_index_count(unit, sid);
    index_max = index_min + (int)ent_index_cnt - 1;

    dyn_info.tbl_inst = tbl_inst;
    for (index = start_index; index <= index_max; index++) {
        dyn_info.index = index;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 req_ctxt->req_flags,
                                 0,
                                 sid,
                                 &dyn_info,
                                 NULL,
                                 rsp_entry_buf_wsize,
                                 rsp_entry_words,
                                 &cache_valid,
                                 &tmp_ltid,
                                 &dfield_format_id,
                                 &tmp_rsp_flags));
        if (cache_valid == FALSE) {
            continue;
        }
        if (tmp_ltid == req_ctxt->req_ltid) {
            /* The first valid entry has been found. */
            if ((req_ctxt->req_flags & BCMLT_ENT_ATTR_GET_FROM_HW) &&
                (view_info->hit_info != NULL)) {
                /*
                 * Get hit bit information for those hash tables which have
                 * separated physical hit tables.
                 */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_hit_bit_get(unit,
                                                req_ctxt,
                                                &dyn_info,
                                                view_info->hit_info,
                                                rsp_entry_words));
            }
            rsp_info->rsp_entry_index[0] = index;
            rsp_info->rsp_entry_sid[0] = sid;
            rsp_info->rsp_entry_count = 1;
            rsp_info->rsp_dfield_format_id = dfield_format_id;
            rsp_info->rsp_entry_pipe = (tbl_inst == -1)? 0 : tbl_inst;
            *rsp_ltid = req_ctxt->req_ltid;
            *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
            SHR_EXIT();
        }
        if (start_nme_info != NULL) {
            /* For narrow mode LT */
            if (index == start_index) {
                start_nme_idx = start_nme_info->nm_loc;
            } else {
                start_nme_idx = 0;
            }
            SHR_IF_ERR_EXIT
                (rm_hash_get_valid_narrow_entry(unit,
                                                req_ctxt,
                                                sid,
                                                pipe,
                                                index,
                                                start_nme_idx,
                                                start_nme_info->e_nm,
                                                &entry_found,
                                                rsp_entry_words));
            if (entry_found == TRUE) {
                rsp_info->rsp_entry_index[0] = index;
                rsp_info->rsp_entry_sid[0] = sid;
                rsp_info->rsp_entry_count = 1;
                rsp_info->rsp_dfield_format_id = dfield_format_id;
                rsp_info->rsp_entry_pipe = (tbl_inst == -1)? 0 : tbl_inst;
                *rsp_ltid = req_ctxt->req_ltid;
                *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
                SHR_EXIT();
            } else {
                continue;
            }
        }
    }
    /* There is no valid entry in this pipe on this SID. */
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the first valid entry for a physical table.
 *
 * This function get the first valid entry for a physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] num_pipes Number of pipes to be traversed.
 * \param [in] curr_pipe Current pipe to be traversed.
 * \param [in] start_index Entry index from where to search valid entry.
 * \param [in] start_nme_info Pointer to rm_hash_nme_info_t structure.
 * \param [in] rsp_entry_buf_wsize Size of the response buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_valid_entry_in_tbl(int unit,
                               const bcmptm_rm_hash_view_info_t *view_info,
                               rm_hash_req_context_t *req_ctxt,
                               uint8_t num_pipes,
                               uint8_t curr_pipe,
                               int start_index,
                               rm_hash_nme_info_t *start_nme_info,
                               size_t rsp_entry_buf_wsize,
                               uint32_t *rsp_entry_words,
                               bcmptm_rm_hash_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags)
{
    int tmp_rv, srch_index;
    uint8_t srch_pipe;

    SHR_FUNC_ENTER(unit);

    if (num_pipes == 1) {
        /* The LT works in global mode. */
        tmp_rv = rm_hash_get_valid_entry_in_pipe(unit,
                                                 view_info,
                                                 req_ctxt,
                                                 -1,
                                                 start_index,
                                                 start_nme_info,
                                                 rsp_entry_buf_wsize,
                                                 rsp_entry_words,
                                                 rsp_info,
                                                 rsp_ltid,
                                                 rsp_flags);
        if (tmp_rv == SHR_E_NONE) {
            SHR_EXIT();
        } else {
            /*
             * Either some unexpected error occurs, or there is
             * no valid entry in the remaining range of the physical table.
             * Return in either case.
             */
            SHR_ERR_EXIT(tmp_rv);
        }
    } else if (num_pipes > 1) {
        srch_index = start_index;
        srch_pipe = curr_pipe;
        do {
            /* The LT works in pipe unique mode. Firstly iterare current pipe. */
            tmp_rv = rm_hash_get_valid_entry_in_pipe(unit,
                                                     view_info,
                                                     req_ctxt,
                                                     srch_pipe,
                                                     srch_index,
                                                     start_nme_info,
                                                     rsp_entry_buf_wsize,
                                                     rsp_entry_words,
                                                     rsp_info,
                                                     rsp_ltid,
                                                     rsp_flags);
            if (tmp_rv == SHR_E_NONE) {
                SHR_EXIT();
            } else if (tmp_rv != SHR_E_NOT_FOUND) {
                /* Some unexpected error occurs. Return directly. */
                SHR_ERR_EXIT(tmp_rv);
            } else {
                /* Try to find valid entry in the remaining pipes. */
                srch_index = bcmptm_pt_index_min(unit, view_info->sid[0]);
                srch_pipe++;
            }
        } while (srch_pipe < num_pipes);
        /* There is no valid entry on all the pipes. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current entry location.
 *
 * This function gets the current entry location for the given entry key.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] curr_loc Pointer to rm_hash_travs_loc_t structure recording
 *              the entry location result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_curr_entry_location_get(int unit,
                                rm_hash_req_context_t *req_ctxt,
                                rm_hash_lt_ctrl_t *lt_ctrl,
                                rm_hash_travs_loc_t *curr_loc)
{
    uint32_t tmp_rsp_flags, ent_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint8_t map_idx, grp_idx, map_index = 0;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmlrd_hw_field_list_t *key_fields = NULL;
    bcmptm_rm_hash_entry_attrs_t *entry_attrs = req_ctxt->req_info->entry_attrs;
    int result;
    bcmptm_rm_hash_rsp_t tmp_rsp_info;
    bcmltd_sid_t tmp_rsp_ltid;
    rm_hash_pt_info_t *pt_info = NULL;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], rbank_cnt, rbank;
    rm_hash_nme_info_t nme_info;
    bcmltd_sid_t nme_ltid;
    bcmdrd_sid_t grp_sid;
    uint32_t nme_ent_words[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    if (((req_ctxt->pt_dyn_info->tbl_inst == -1) ||
         (req_ctxt->pt_dyn_info->tbl_inst == travs_curr_loc[unit].pipe)) &&
         (entry_attrs == NULL))  {
        bool cache_valid = FALSE;
        bcmltd_sid_t ltid;
        uint16_t dfield_format_id;
        uint32_t rsp_flags;
        bcmbd_pt_dyn_info_t dyn_info;

        map_idx = travs_curr_loc[unit].map_index;
        grp_idx = lt_ctrl->map_info[map_idx].grp_idx[0];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_get(unit,
                                           req_ctxt->req_info->entry_attrs,
                                           &lt_ctrl->lt_info,
                                           grp_idx,
                                           &key_format));
        SHR_NULL_CHECK(key_format, SHR_E_PARAM);
        if (key_format->hw_key_fields != NULL) {
            key_fields = key_format->hw_key_fields;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        dyn_info.tbl_inst = req_ctxt->pt_dyn_info->tbl_inst;
        dyn_info.index = travs_curr_loc[unit].ent_index;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 BCMPTM_REQ_FLAGS_NO_FLAGS,
                                 0,
                                 travs_curr_loc[unit].sid,
                                 &dyn_info,
                                 NULL, /* misc_info */
                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                 ent_words,
                                 &cache_valid,
                                 &ltid,
                                 &dfield_format_id,
                                 &rsp_flags));
        if (cache_valid == TRUE) {
            if (ltid == lt_ctrl->ltid) {
                result = rm_hash_travs_key_compare(req_ctxt->req_info->entry_words,
                                                   ent_words,
                                                   key_fields);
                if (result == 0) {
                    *curr_loc = travs_curr_loc[unit];
                    SHR_EXIT();
                }
            }
            if ((lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                (lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, travs_curr_loc[unit].sid, &grp_sid));
                bcmptm_rm_hash_sid_rbank_list_get(unit,
                                                  grp_sid,
                                                  pt_info,
                                                  rbank_list,
                                                  &rbank_cnt);
                rbank = rbank_list[0];
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_nme_ltid_get(unit,
                                                 pt_info,
                                                 rbank,
                                                 travs_curr_loc[unit].pipe,
                                                 travs_curr_loc[unit].ent_index,
                                                 travs_curr_loc[unit].nme_info.nm_loc,
                                                 &nme_ltid));
                if (nme_ltid == lt_ctrl->ltid) {
                    /* Ltid matches, further check if the entry is identical. */
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_nm_entry_extract(unit,
                                                         pt_info,
                                                         rbank,
                                                         travs_curr_loc[unit].nme_info.e_nm,
                                                         travs_curr_loc[unit].nme_info.nm_loc,
                                                         ent_words,
                                                         nme_ent_words));
                    result = rm_hash_travs_key_compare(req_ctxt->req_info->entry_words,
                                                       nme_ent_words,
                                                       key_fields);
                    if (result == 0) {
                        *curr_loc = travs_curr_loc[unit];
                        SHR_EXIT();
                    }
                }
            }
        }
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lookup(unit,
                               req_ctxt->req_flags,
                               req_ctxt->req_ltid,
                               req_ctxt->pt_dyn_info,
                               req_ctxt->req_info,
                               BCMPTM_MAX_PT_ENTRY_WORDS,
                               ent_words,
                               &tmp_rsp_info,
                               &tmp_rsp_ltid,
                               &tmp_rsp_flags,
                               &map_index,
                               &nme_info));
    curr_loc->sid = tmp_rsp_info.rsp_entry_sid[0];
    curr_loc->ent_index = tmp_rsp_info.rsp_entry_index[0];
    curr_loc->pipe = tmp_rsp_info.rsp_entry_pipe;
    curr_loc->map_index = map_index;
    curr_loc->nme_info = nme_info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the list of view info for a LT to PT mapping group.
 *
 * This function gets the list of SIDs for a LT to PT mapping group.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_index Index of LT to PT mapping group.
 * \param [out] view_info_list Pointer to buffer recording the view info list.
 * \param [out] num_view_info Pointer to buffer recording the view info number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_map_info_view_get(int unit,
                             bcmptm_rm_hash_req_t *req_info,
                             rm_hash_lt_ctrl_t *lt_ctrl,
                             uint8_t map_index,
                             const bcmptm_rm_hash_view_info_t **view_info_list,
                             uint8_t *num_view_info)
{
    uint8_t inst = 0, grp_idx, idx, num_views = 0;
    rm_hash_map_info_t *map_info;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;

    SHR_FUNC_ENTER(unit);

    map_info = &lt_ctrl->map_info[map_index];
    if ((map_info->num_insts > 1) && (req_info->entry_attrs != NULL)) {
        /*
         * When there are multiple instances, the correct view
         * needs to be selected.
         */
        grp_idx = map_info->grp_idx[0];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_get(unit,
                                           req_info->entry_attrs,
                                           &lt_ctrl->lt_info,
                                           grp_idx,
                                           &key_format));
        for (idx = 0; idx < map_info->num_insts; idx++) {
            if (key_format == map_info->key_format[idx]) {
                inst = idx;
                break;
            }
        }
        if (idx == map_info->num_insts) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    key_format = map_info->key_format[inst];
    grp_idx = map_info->grp_idx[inst];
    bcmptm_rm_hash_key_format_num_view_get(unit, key_format, &num_views);
    for (idx = 0; idx < num_views; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_view_info_get(unit,
                                                     &lt_ctrl->lt_info,
                                                     grp_idx,
                                                     key_format,
                                                     idx,
                                                     &view_info));
        view_info_list[idx] = view_info;
    }
    *num_view_info = num_views;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search a valid entry starting from the current location.
 *
 * This function searches a valid entry starting from the current location.
 * This is an essential operation for GET_FIRST and GET_NEXT during traversal
 * without snapshot.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] curr_loc Pointer to rm_hash_travs_loc_t structure.
 * \param [in] rsp_entry_buf_wsize wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_valid_entry_search(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           rm_hash_lt_ctrl_t *lt_ctrl,
                           rm_hash_travs_loc_t *curr_loc,
                           size_t rsp_entry_buf_wsize,
                           uint32_t *rsp_entry_words,
                           bcmptm_rm_hash_rsp_t *rsp_info,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags)
{
    uint8_t start_map = 0, map_idx, view_idx;
    bool search_started = TRUE, get_next_context = FALSE;
    int tmp_rv, start_ent_index = 0;
    uint8_t start_pipe = 0, num_pipes;
    bcmdrd_sid_t sid;
    const bcmptm_rm_hash_view_info_t *view_info_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT];
    uint8_t num_view = 0, num_searched_sids = 0;
    rm_hash_nme_info_t start_nme_info, *pnme_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&start_nme_info, 0, sizeof(start_nme_info));
    if (curr_loc != NULL) {
        get_next_context = TRUE;
        start_map = curr_loc->map_index;
        search_started = FALSE;
    }
    if ((curr_loc == NULL) &&
        ((lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
         (lt_ctrl->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD))) {
        start_nme_info.e_nm = lt_ctrl->e_nm;
        start_nme_info.nm_loc = 0;
        start_nme_info.ltid = req_ctxt->req_ltid;
        pnme_info = &start_nme_info;
    }

    for (map_idx = start_map; map_idx < lt_ctrl->num_maps; map_idx++) {
        SHR_IF_ERR_EXIT
            (rm_hash_lt_map_info_view_get(unit,
                                          req_ctxt->req_info,
                                          lt_ctrl,
                                          map_idx,
                                          view_info_list,
                                          &num_view));
        for (view_idx = 0; view_idx < num_view; view_idx++) {
            sid = view_info_list[view_idx]->sid[0];
            if (search_started == FALSE) {
                if (sid != curr_loc->sid) {
                    continue;
                } else {
                    search_started = TRUE;
                }
            }

            if (search_started == TRUE) {
                start_ent_index = bcmptm_pt_index_min(unit, sid);
                start_pipe = 0;
                if ((get_next_context == TRUE) && (num_searched_sids == 0)) {
                    start_nme_info = curr_loc->nme_info;
                    if (curr_loc->nme_info.e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) {
                        if (curr_loc->nme_info.nm_loc == 0) {
                            start_nme_info.nm_loc = 1;
                            start_ent_index = curr_loc->ent_index;
                        } else {
                            start_nme_info.nm_loc = 0;
                            start_ent_index = curr_loc->ent_index + 1;
                        }
                        pnme_info = &start_nme_info;
                    } else if (curr_loc->nme_info.e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD) {
                        if (curr_loc->nme_info.nm_loc == 0) {
                            start_nme_info.nm_loc = 1;
                            start_ent_index = curr_loc->ent_index;
                        } else if (curr_loc->nme_info.nm_loc == 1) {
                            start_nme_info.nm_loc = 2;
                            start_ent_index = curr_loc->ent_index;
                        } else {
                            start_nme_info.nm_loc = 0;
                            start_ent_index = curr_loc->ent_index + 1;
                        }
                        pnme_info = &start_nme_info;
                    } else {
                        start_ent_index = curr_loc->ent_index + 1;
                    }
                    start_pipe = curr_loc->pipe;
                }
                num_pipes = 1;
                if (lt_ctrl->is_per_pipe == TRUE) {
                    num_pipes = bcmdrd_pt_num_tbl_inst(unit, sid);
                }

                tmp_rv = rm_hash_get_valid_entry_in_tbl(unit,
                                                        view_info_list[view_idx],
                                                        req_ctxt,
                                                        num_pipes,
                                                        start_pipe,
                                                        start_ent_index,
                                                        pnme_info,
                                                        rsp_entry_buf_wsize,
                                                        rsp_entry_words,
                                                        rsp_info,
                                                        rsp_ltid,
                                                        rsp_flags);
                if (tmp_rv == SHR_E_NONE) {
                    /* Cache the entry location information. */
                    travs_curr_loc[unit].map_index = map_idx;
                    travs_curr_loc[unit].sid = rsp_info->rsp_entry_sid[0];
                    travs_curr_loc[unit].ent_index = rsp_info->rsp_entry_index[0];
                    travs_curr_loc[unit].pipe = rsp_info->rsp_entry_pipe;
                    SHR_EXIT();
                } else if (tmp_rv != SHR_E_NOT_FOUND) {
                    SHR_ERR_EXIT(tmp_rv);
                } else {
                    num_searched_sids++;
                    continue;
                }
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_FIRST opcode without snapshot.
 *
 * This function handles GET_FIRST opcode without snapshot.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] rsp_entry_buf_wsize wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_first_without_snap(int unit,
                               rm_hash_req_context_t *req_ctxt,
                               size_t rsp_entry_buf_wsize,
                               uint32_t *rsp_entry_words,
                               bcmptm_rm_hash_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmptm_rm_hash_req_t tmp_req_info;
    uint8_t map_index = 0;
    rm_hash_lt_flex_ctr_info_t *fc_info;
    rm_hash_nme_info_t nme_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    fc_info = &lt_ctrl->flex_ctr_info;

    SHR_IF_ERR_EXIT
        (rm_hash_valid_entry_search(unit,
                                    req_ctxt,
                                    lt_ctrl,
                                    NULL,
                                    rsp_entry_buf_wsize,
                                    rsp_entry_words,
                                    rsp_info,
                                    rsp_ltid,
                                    rsp_flags));
    if (fc_info->entry_index_cb != NULL) {
        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_words = rsp_entry_words;
        tmp_req_info.same_key = FALSE;
        tmp_req_info.entry_attrs = req_ctxt->req_info->entry_attrs;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lookup(unit,
                                   req_ctxt->req_flags,
                                   req_ctxt->req_ltid,
                                   req_ctxt->pt_dyn_info,
                                   &tmp_req_info,
                                   rsp_entry_buf_wsize,
                                   rsp_entry_words,
                                   rsp_info,
                                   rsp_ltid,
                                   rsp_flags,
                                   &map_index,
                                   &nme_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_NEXT opcode without snapshot.
 *
 * This function handles GET_NEXT opcode without snapshot.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] rsp_entry_buf_wsize wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_get_next_without_snap(int unit,
                              rm_hash_req_context_t *req_ctxt,
                              size_t rsp_entry_buf_wsize,
                              uint32_t *rsp_entry_words,
                              bcmptm_rm_hash_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_travs_loc_t curr_loc;
    bcmptm_rm_hash_req_t tmp_req_info;
    uint8_t map_index = 0;
    rm_hash_lt_flex_ctr_info_t *fc_info;
    rm_hash_nme_info_t nme_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    fc_info = &lt_ctrl->flex_ctr_info;

    SHR_IF_ERR_EXIT
        (rm_hash_curr_entry_location_get(unit,
                                         req_ctxt,
                                         lt_ctrl,
                                         &curr_loc));

    SHR_IF_ERR_EXIT
        (rm_hash_valid_entry_search(unit,
                                    req_ctxt,
                                    lt_ctrl,
                                    &curr_loc,
                                    rsp_entry_buf_wsize,
                                    rsp_entry_words,
                                    rsp_info,
                                    rsp_ltid,
                                    rsp_flags));
    if (fc_info->entry_index_cb != NULL) {
        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_words = rsp_entry_words;
        tmp_req_info.same_key = FALSE;
        tmp_req_info.entry_attrs = req_ctxt->req_info->entry_attrs;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lookup(unit,
                                   req_ctxt->req_flags,
                                   req_ctxt->req_ltid,
                                   req_ctxt->pt_dyn_info,
                                   &tmp_req_info,
                                   rsp_entry_buf_wsize,
                                   rsp_entry_words,
                                   rsp_info,
                                   rsp_ltid,
                                   rsp_flags,
                                   &map_index,
                                   &nme_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup resource allocated in traversal with snapshot.
 *
 * \param [in] req_flags Flags from PTM.
 * \param [in] req_ctxt Requested context information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_done(int unit, rm_hash_req_context_t *req_ctxt)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    rm_hash_travs_snapshot_free(unit, lt_ctrl);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the requested context information.
 *
 * \param [in] req_flags Flags from PTM.
 * \param [in] req_ltid Requested ltid.
 * \param [in] pt_dyn_info Dynamic information.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [out] req_ctxt Requested context information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_req_ctxt_init(uint64_t req_flags,
                      bcmltd_sid_t req_ltid,
                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                      bcmptm_rm_hash_req_t *req_info,
                      rm_hash_req_context_t *req_ctxt)
{
    req_ctxt->req_flags = req_flags;
    req_ctxt->pt_dyn_info = pt_dyn_info;
    req_ctxt->req_ltid = req_ltid;
    req_ctxt->req_info = req_info;
}

/*******************************************************************************
 * Public Functions
 */
int bcmptm_rm_hash_travs_cmd_proc(int unit,
                                  uint64_t req_flags,
                                  uint32_t cseq_id,
                                  bcmltd_sid_t req_ltid,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  bcmptm_op_type_t req_op,
                                  bcmptm_rm_hash_req_t *req_info,
                                  size_t rsp_entry_buf_wsize,
                                  uint32_t *rsp_entry_words,
                                  bcmptm_rm_hash_rsp_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags)
{
    int tmp_rv;
    rm_hash_req_context_t req_ctxt;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info->entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    rm_hash_req_ctxt_init(req_flags,
                          req_ltid,
                          pt_dyn_info,
                          req_info,
                          &req_ctxt);

    if (req_flags & BCMLT_ENT_ATTR_TRAVERSE_DONE) {
        SHR_IF_ERR_EXIT
            (rm_hash_travs_done(unit, &req_ctxt));
        SHR_EXIT();
    }

    switch (req_op){
    case BCMPTM_OP_GET_FIRST:
        if (req_flags & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) {
            tmp_rv = rm_hash_get_first_with_snap(unit,
                                                 &req_ctxt,
                                                 rsp_entry_buf_wsize,
                                                 rsp_entry_words,
                                                 rsp_info,
                                                 rsp_ltid,
                                                 rsp_flags);
        } else {
            tmp_rv = rm_hash_get_first_without_snap(unit,
                                                    &req_ctxt,
                                                    rsp_entry_buf_wsize,
                                                    rsp_entry_words,
                                                    rsp_info,
                                                    rsp_ltid,
                                                    rsp_flags);
        }
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_NEXT:
        if (req_flags & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) {
            tmp_rv = rm_hash_get_next_with_snap(unit,
                                                &req_ctxt,
                                                rsp_entry_buf_wsize,
                                                rsp_entry_words,
                                                rsp_info,
                                                rsp_ltid,
                                                rsp_flags);
        } else {
            tmp_rv = rm_hash_get_next_without_snap(unit,
                                                   &req_ctxt,
                                                   rsp_entry_buf_wsize,
                                                   rsp_entry_words,
                                                   rsp_info,
                                                   rsp_ltid,
                                                   rsp_flags);
        }
        if ((tmp_rv == SHR_E_NOT_FOUND) || (tmp_rv == SHR_E_UNAVAIL)) {
            SHR_ERR_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}
