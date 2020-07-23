/*! \file bcmptm_ser_misc.c
 *
 * Functions for TCAM slice mode or entry mode change.
 *
 * Functions to judge whether entry index is valid or not.
 *
 * This file can't call any functions in SER component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
 * Includes
 */
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>
#include <sal/sal_thread.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmissu/issu_api.h>
#include <bcmptm/generated/bcmptm_ser_local.h>

#include <bcmptm/bcmptm_ser_misc_internal.h>
/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*! Get one bit value from bit_map, 1 or 0 */
#define BCMPTM_SER_BITGET(bit_map, i) \
        (SHR_BITGET(bit_map, i) ? 1 : 0)

/*!
  * \brief Count of entries of one PT
  */
#define BCMPTM_SER_INDEX_COUNT(unit, sid) \
    (1 + bcmdrd_pt_index_max(unit, sid) - bcmdrd_pt_index_min(unit, sid))

/*! Number of tcam owning slice modes */
#define BCMPTM_SER_TCAM_NUM   4

/*! TCAM is L3_DEFIPm/L3_DEFIP_PAIR_128m */
#define BCMPTM_SER_IS_OVERLAY_TCAM          (1 << 0)

/*! Data struction of slice info is used */
#define BCMPTM_SER_SLICE_INFO_USED          (1 << 1)


/*! Max size of L3_DEFIP_PAIR_128m of all chips */
#define BCMPTM_SER_OVERLAY_WIDE_VIEW_SIZE    (DEPTH_WIDE_ENTRY_BITMAP * 32)

/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Private variables
 */
/*! Slice information of TCAMs */
static bcmptm_ser_slices_info_t *tcam_slice_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Mutex to protect slice information of TCAMs */
static sal_mutex_t tcam_slice_mode_mutex[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_SER_TCAM_NUM];

/*! Next slice group id, maximum value of next_slice_group_id is BCMPTM_SER_TCAM_NUM */
static uint32_t next_slice_group_id[BCMDRD_CONFIG_MAX_UNITS];

/*! Slice group id of overlay TCAM */
static uint32_t overlay_slice_group_id[BCMDRD_CONFIG_MAX_UNITS];

/*!
* For wide slices of L3_DEFIPm/L3_DEFIP_PAIR_128m,
* an entry can be used either as narrow, or as wide.
* If one entry is used as narrow mode, the value of corresponding bit is '1'.
* Otherwise, the value of corresponding bit is '0'.
*/
static SHR_BITDCL *overlay_wide_slice_bitmap[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Defines
 */
/*! For wide slices of L3_DEFIP_PAIR_128m, set one or more bits as narrow entry */
#define OVERLAY_WIDE_SLICE_ENTRY_BITMAP_SET(unit, start_index, index_num) \
    SHR_BITSET_RANGE(overlay_wide_slice_bitmap[unit], start_index, index_num)

/*! For wide slices of L3_DEFIP_PAIR_128m, set one or more bits as wide entry */
#define OVERLAY_WIDE_SLICE_ENTRY_BITMAP_CLR(unit, start_index, index_num) \
    SHR_BITCLR_RANGE(overlay_wide_slice_bitmap[unit], start_index, index_num)

/*! Get one bit from entry bitmap. 1: narrow entry, 0: wide entry */
#define OVERLAY_WIDE_SLICE_ENTRY_BITMAP_GET(unit, index) \
    BCMPTM_SER_BITGET(overlay_wide_slice_bitmap[unit], index)

/*! Slice information is registered by user */
#define OVERLAY_SLICE_INFO_REGISTERED(unit) \
    (BCMPTM_SER_TCAM_NUM > overlay_slice_group_id[unit])

/*! Check whether 'group_id' is overlay group id or not */
#define IS_OVERLAY_SLICE_GROUP_ID(unit, group_id) \
    (OVERLAY_SLICE_INFO_REGISTERED(unit) && \
     group_id == overlay_slice_group_id[unit])

/*! Check whether 'group_id' is valid or not */
#define SLICE_GROUP_ID_IS_INVALID(unit, group_id) \
    ((group_id >= next_slice_group_id[unit]) || (group_id >= BCMPTM_SER_TCAM_NUM))

/*! Slice information of one 'group_id' */
#define TCAM_SLICE_INFO(unit, group_id) \
    (&tcam_slice_info[unit][group_id])

/*! Slice information of one 'group_id' */
#define SLICE_MODE_MUTEX(unit, group_id) \
    tcam_slice_mode_mutex[unit][group_id]

/*! Slice information of the overlay TCAM */
#define OVERLAY_TCAM_SLICE_INFO(unit) \
    (TCAM_SLICE_INFO(unit, overlay_slice_group_id[unit]))

/*! Slice mode info array of the overlay TCAM */
#define OVERLAY_TCAM_SLICE_MODE_ARRAY(unit) \
    (OVERLAY_TCAM_SLICE_INFO(unit)->slice_mode_array)

/*! Narrow sid of the overlay tcam*/
#define NARROW_SID_OF_OVERLAY_TCAM_GROUP(unit) \
    (OVERLAY_TCAM_SLICE_INFO(unit)->sid_array[BCMPTM_SER_NARROW_INDEX])

/*! Wide sid of the overlay tcam */
#define WIDE_SID_OF_OVERLAY_TCAM_GROUP(unit) \
    (OVERLAY_TCAM_SLICE_INFO(unit)->sid_array[BCMPTM_SER_WIDE_INDEX])

/*! Number of slice of the overay TCAM */
#define SLICE_NUM_OF_OVERLAY_TCAM(unit) \
    (OVERLAY_TCAM_SLICE_INFO(unit)->slice_num)

/*! Number of slice of a TCAM */
#define SLICE_NUM_OF_TCAM(unit, group_id) \
    (TCAM_SLICE_INFO(unit, group_id)->slice_num)

/*! Slice mode array of a TCAM */
#define SLICE_MODE_ARRAY(unit, group_id) \
    (TCAM_SLICE_INFO(unit, group_id)->slice_mode_array)

/*! Depth of a narrow slice */
#define SLICE_DEPTH_NARROW(unit, slice_mode_array, slice_id) \
    (slice_mode_array[slice_id].slice_depth[BCMPTM_SER_NARROW_INDEX])

/*! Depth of a wide slice */
#define SLICE_DEPTH_WIDE(unit, slice_mode_array, slice_id) \
    (slice_mode_array[slice_id].slice_depth[BCMPTM_SER_WIDE_INDEX])

/*! Mode of a slice */
#define SLICE_MODE(unit, slice_mode_array, slice_id) \
    (slice_mode_array[slice_id].slice_mode)

/*! Traverse tcam_slice_info[unit] */
#define TCAM_SLICE_INFO_ITER(unit, slice_info, id) \
    for (id = 0, slice_info = &tcam_slice_info[unit][0]; \
         id < next_slice_group_id[unit]; \
         id++, slice_info++)

/*! Map narrow index to wide index */
#define NARROW_INDEX_MAP_TO_WIDE_INDEX(narrow_index, slice_depth_narrow, slice_depth_wide) \
    (((narrow_index / (slice_depth_narrow * 2)) * (slice_depth_wide * 2)) + \
     (narrow_index % slice_depth_narrow))

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_overlay_info_print used to print overlay entry bitmap information
 */
static void
bcmptm_ser_overlay_info_print(int unit)
{
    int i = 0, index_count = 0, valid;
    bcmdrd_sid_t sid;
    int wide_entry_num = 0;


    sid = WIDE_SID_OF_OVERLAY_TCAM_GROUP(unit);
    index_count = BCMPTM_SER_INDEX_COUNT(unit, sid);

    LOG_CLI((BSL_META_U(unit,
                        "Overlay tcam wide entry bitmap:")));
    for (i = 0; i < index_count; i++) {
        if ((i % 64) == 0) {
            LOG_CLI((BSL_META_U(unit, "\n [%4d-%4d]: "),
                     i, (i + 64)));
        }
        valid = OVERLAY_WIDE_SLICE_ENTRY_BITMAP_GET(unit, i);
        LOG_CLI((BSL_META_U(unit,
                            "%d"), valid ? 0 : 1));
        if (valid == 0) {
            wide_entry_num++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\nNumber[%d]/[%d] of entry used as %s!!!\n"),
             wide_entry_num, index_count, bcmdrd_pt_sid_to_name(unit, sid)));
}

/******************************************************************************
 * bcmptm_ser_slice_param_check used to check parameters from outside caller.
 */
static int
bcmptm_ser_slice_param_check(int unit, int *slice_depth, int num_slice,
                             bcmdrd_sid_t *sid_list, int num_sid)
{
    int i = 0, j = 0;
    int depth1 = 0, depth0 = 0, swap_depth, narrow_depth = 0;
    bcmdrd_sid_t swap_sid;

    if (next_slice_group_id[unit] >= BCMPTM_SER_TCAM_NUM) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "There are %d TCAM being registered\n"),
                   next_slice_group_id[unit]));
        return SHR_E_FULL;
    }
    if (slice_depth == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "slice_depth == NULL\n")));
        return SHR_E_PARAM;
    }
    if ((num_slice <= 1) || (num_slice % 2) || (num_slice > BCMPTM_SER_MAX_SLICE_NUM)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Num_slice <= 1, or num_slice is odd number, or num_slice > %d\n"),
                   BCMPTM_SER_MAX_SLICE_NUM));
        return SHR_E_PARAM;
    }
    if (sid_list == NULL || num_sid <= 1 || num_sid > BCMPTM_SER_SLICE_MODE_NUM) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "sid_list == NULL, or num_sid <= 1, or num_sid > BCMPTM_SER_SLICE_MODE_NUM\n")));
        return SHR_E_PARAM;
    }
    for (i = 0; i < num_slice; i++) {
        narrow_depth += slice_depth[i];
    }
    /* narrow view is located at sid_list[0], wide view is located at sid_list[1] */
    /*Sort sid_list[] by depth of each view */
    for (i = 0; i < num_sid; i++) {
        depth0 = BCMPTM_SER_INDEX_COUNT(unit, sid_list[i]);
        for (j = i + 1; j < num_sid; j++) {
            depth1 = BCMPTM_SER_INDEX_COUNT(unit, sid_list[j]);
            if (depth0 == depth1) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Depth[%d] of [%s] should not be equal to depth[%d] of [%s]\n"),
                           depth1, bcmdrd_pt_sid_to_name(unit, sid_list[j]),
                           depth0, bcmdrd_pt_sid_to_name(unit, sid_list[i])));
                return SHR_E_PARAM;
            } else if (depth0 < depth1) {
                swap_sid = sid_list[i];
                sid_list[i] = sid_list[j];
                sid_list[j] = swap_sid;
            } else {
                swap_depth = depth0;
                depth0 = depth1;
                depth1 = swap_depth;
            }
            /* should be zero */
            if ((depth1 % depth0)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Depth[%d] of [%s] mod depth[%d] of [%s] should be zero\n"),
                           depth1, bcmdrd_pt_sid_to_name(unit, sid_list[j]),
                           depth0, bcmdrd_pt_sid_to_name(unit, sid_list[i])));
                return SHR_E_PARAM;
            }
        }
    }
    /* narrow view in sid_list[0], wide view in sid_list[1] */
    depth0 = BCMPTM_SER_INDEX_COUNT(unit, sid_list[0]);
    if (narrow_depth != depth0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Depth of [%s] is [%d], but sum of all slice depth is [%d]\n"),
                   bcmdrd_pt_sid_to_name(unit, sid_list[0]), depth0, narrow_depth));
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_slice_ratio_calc used to calculate depth ratio between one view and narrow view.
 */
static int
bcmptm_ser_slice_ratio_calc(int unit, bcmdrd_sid_t *sid_list,
                            int num_sid, uint32_t *depth_ratio)
{
    int i, narrow_depth, depth;

    /* narrow view is located at sid_list[0] */
    narrow_depth = BCMPTM_SER_INDEX_COUNT(unit, sid_list[0]);
    for (i = 0; i < num_sid; i++) {
        depth = BCMPTM_SER_INDEX_COUNT(unit, sid_list[i]);
        /* depth_ratio can be 1, 2 etc. */
        depth_ratio[i] = narrow_depth / depth;
        if (depth_ratio[i] < 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Depth ratio [%d] between [%s] and [%s] should not be less than 1\n"),
                       depth_ratio[i], bcmdrd_pt_sid_to_name(unit, sid_list[0]),
                       bcmdrd_pt_sid_to_name(unit, sid_list[i])));
            return SHR_E_FAIL;
        }
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_tcam_entry_bitmap_init used to initialize entry bitmap for overlay TCAM.
 */
static int
bcmptm_ser_tcam_entry_bitmap_init(int unit, uint32_t new_group_id,
                                   bool same_depth, uint32_t depth_wide,
                                   bcmptm_rm_slice_mode_t slice_mode)
{
    if (!same_depth) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Depth of every slice of overlay TCAM"
                              " is different\n")));
        return SHR_E_PARAM;
    }
    if (depth_wide > BCMPTM_SER_OVERLAY_WIDE_VIEW_SIZE) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Max size of wide view is [%d] < [%d]\n"),
                   BCMPTM_SER_OVERLAY_WIDE_VIEW_SIZE, depth_wide));
        return SHR_E_INTERNAL;
    }
    if (OVERLAY_SLICE_INFO_REGISTERED(unit) &&
        new_group_id != overlay_slice_group_id[unit] ) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "slice group id of overlay TCAM"
                              " should not be changed!\n")));
        return SHR_E_EXISTS;
    }
    if (slice_mode == BCMPTM_TCAM_SLICE_NARROW) {
        OVERLAY_WIDE_SLICE_ENTRY_BITMAP_SET(unit, 0, depth_wide);
    } else {
        OVERLAY_WIDE_SLICE_ENTRY_BITMAP_CLR(unit, 0, depth_wide);
    }
    overlay_slice_group_id[unit] = new_group_id;

    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_slice_info_node_init used to initialize new slice information node.
 */
static bcmptm_ser_slices_info_t *
bcmptm_ser_slice_info_node_init(int unit, int *slice_depth, int num_slice,
                                bcmdrd_sid_t *sid_list, int num_sid,
                                bcmptm_rm_slice_mode_t slice_mode,
                                int old_group_id,
                                int is_overlay_cam, uint32_t *slice_group_id)
{
    bcmptm_ser_slices_info_t *slices_info_node = NULL;
    bcmptm_ser_slice_mode_t *slice_mode_ptr = NULL;
    sal_mutex_t mode_mutex;
    int i = 0, rv = SHR_E_NONE;
    int depth = 0, slice_depth_narrow = 0, slice_depth_wide = 0;
    uint32_t depth_ratio[BCMPTM_SER_SLICE_MODE_NUM];
    uint32_t new_group_id = 0, depth_wide = 0;
    bool same_depth = TRUE;

    rv = bcmptm_ser_slice_ratio_calc(unit, sid_list, num_sid, depth_ratio);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    if (old_group_id < 0) {
        new_group_id = next_slice_group_id[unit];
    } else {
        new_group_id = (uint32_t)old_group_id;
    }
    *slice_group_id = new_group_id;
    slices_info_node = TCAM_SLICE_INFO(unit, new_group_id);

    sal_memset(slices_info_node, 0, sizeof(bcmptm_ser_slices_info_t));

    mode_mutex = SLICE_MODE_MUTEX(unit, new_group_id);

    if (mode_mutex == NULL) {
        mode_mutex = sal_mutex_create("slice_mode_mutex");
        if (mode_mutex == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create slice mode mutex.\n")));
            return NULL;
        }
    }
    SLICE_MODE_MUTEX(unit, new_group_id) = mode_mutex;

    slices_info_node->sid_num = num_sid;
    for (i = 0; i < num_sid; i++) {
        /* index 0: narrow view, 1: wide view */
        slices_info_node->sid_array[i] = sid_list[i];
    }
    slices_info_node->slice_num = num_slice;

    slice_mode_ptr = slices_info_node->slice_mode_array;
    for (i = 0; i < num_slice; i++) {
        slice_mode_ptr[i].slice_mode = slice_mode;
        depth = slice_depth[i];
        /* index 0: narrow view, 1: wide view */
        slice_depth_narrow = depth / depth_ratio[BCMPTM_SER_NARROW_INDEX];
        slice_depth_wide = depth / depth_ratio[BCMPTM_SER_WIDE_INDEX];
        SLICE_DEPTH_NARROW(unit, slice_mode_ptr, i) = slice_depth_narrow;
        SLICE_DEPTH_WIDE(unit, slice_mode_ptr, i) = slice_depth_wide;
        depth_wide += slice_depth_wide;
        if (i > 0 && (slice_depth[i] != slice_depth[i - 1])) {
            same_depth = FALSE;
        }
    }
    if (is_overlay_cam) {
        rv = bcmptm_ser_tcam_entry_bitmap_init(unit, new_group_id,
                                               same_depth, depth_wide,
                                               slice_mode);
        if (SHR_FAILURE(rv)) {
            return NULL;
        }
        slices_info_node->flags |= BCMPTM_SER_IS_OVERLAY_TCAM;
    }
    slices_info_node->flags |= BCMPTM_SER_SLICE_INFO_USED;
    /* must last operation */
    if (old_group_id < 0) {
        next_slice_group_id[unit]++;
    }
    return slices_info_node;
}

/******************************************************************************
 * bcmptm_ser_slice_info_find_by_sid used to find slice mode information by sid.
 */
static bcmptm_ser_slices_info_t *
bcmptm_ser_slice_info_find_by_sid(int unit, bcmdrd_sid_t sid,
                                  int *mode, uint32_t *group_id)
{
    bcmptm_ser_slices_info_t *slices_info_node = NULL;
    uint32_t group_id_index = 0, sid_index = 0, num = 0, find = 0;

    TCAM_SLICE_INFO_ITER(unit, slices_info_node, group_id_index) {
        num = slices_info_node->sid_num;
        for (sid_index = 0; sid_index < num; sid_index++) {
            if (slices_info_node->sid_array[sid_index] == sid) {
                find = 1;
                break;
            }
        }
        if (find) {
            *mode = sid_index;
            *group_id = group_id_index;
            return slices_info_node;
        }
    }
    *group_id = 0;
    return NULL;
}

/******************************************************************************
 * bcmptm_ser_slice_info_init used to insert slice mode information for a PT.
 */
static bcmptm_ser_slices_info_t *
bcmptm_ser_slice_info_init(int unit, int *slice_depth, int num_slice,
                           bcmdrd_sid_t *sid_list, int num_sid,
                           bcmptm_rm_slice_mode_t slice_mode, int is_overlay_cam,
                           uint32_t *slice_group_id)
{
    bcmptm_ser_slices_info_t *slices_info_node = NULL;
    int i = 0;
    uint32_t group_id_index = 0;
    int old_group_id = -1;

    TCAM_SLICE_INFO_ITER(unit, slices_info_node, group_id_index) {
        if ((uint8_t)num_sid != slices_info_node->sid_num) {
            continue;
        }
        /* index 0: narrow, index 1: wide */
        for (i = 0; i < num_sid; i++) {
            if (sid_list[i] != slices_info_node->sid_array[i]) {
                break;
            }
        }
        if (i > 0) {
            if ((num_sid >= 2) && (i < num_sid)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Just find part of SIDs in sid_array\n")));
                return NULL;
            }
            /* found */
            old_group_id = group_id_index;
            break;
        }
    }
    slices_info_node = NULL;
    /* allocate new node, if old_slice_group_id != -1, use value of old_slice_group_id as slice_group_id */
    slices_info_node = bcmptm_ser_slice_info_node_init(unit, slice_depth, num_slice,
                                                       sid_list, num_sid, slice_mode,
                                                       old_group_id,
                                                       is_overlay_cam, slice_group_id);
    if (slices_info_node == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to init tcam slice information\n")));
        return NULL;
    }
    return slices_info_node;
}

/******************************************************************************
 * Public Functions
 */
/******************************************************************************
* bcmptm_ser_slice_group_register
 */
int
bcmptm_ser_slice_group_register(int unit,
                                int *slice_depth, int num_slice,
                                bcmdrd_sid_t *sid_list, int num_sid,
                                bcmptm_rm_slice_mode_t slice_mode,
                                int is_overlay_cam,

                                uint32_t *slice_group_id)
{
    bcmptm_ser_slices_info_t *slices_info_node = NULL;
    bcmdrd_sid_t sid_cache[BCMPTM_SER_SLICE_MODE_NUM];
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(slice_group_id, SHR_E_PARAM);

    for (i = 0; i < BCMPTM_SER_SLICE_MODE_NUM && i < num_sid; i++) {
        sid_cache[i] = sid_list[i];
    }
    SHR_IF_ERR_EXIT(
        bcmptm_ser_slice_param_check(unit, slice_depth, num_slice,
                                     sid_cache, num_sid));

    slices_info_node = bcmptm_ser_slice_info_init(unit, slice_depth, num_slice,
                                                  sid_cache, num_sid, slice_mode,
                                                  is_overlay_cam, slice_group_id);
    SHR_NULL_CHECK(slices_info_node, SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_slice_mode_mutex_info
 */
int
bcmptm_ser_slice_mode_mutex_info(int unit, uint32_t slice_group_id,
                                 sal_mutex_t *slice_mode_mutex)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(slice_mode_mutex, SHR_E_PARAM);

    if (SLICE_GROUP_ID_IS_INVALID(unit, slice_group_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *slice_mode_mutex = SLICE_MODE_MUTEX(unit, slice_group_id);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_entry_mode_mutex_info, entry_mode_mutex and slice_mode_mutex are the same mutex
 */
int
bcmptm_ser_entry_mode_mutex_info(int unit, uint32_t *slice_group_id,
                                 sal_mutex_t *entry_mode_mutex)
{
    SHR_FUNC_ENTER(unit);

    if (!OVERLAY_SLICE_INFO_REGISTERED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }
    /* entry mode mutex and slice mode mutex is the same one */
    *slice_group_id = overlay_slice_group_id[unit];

    *entry_mode_mutex = SLICE_MODE_MUTEX(unit, *slice_group_id);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_slice_mode_change
 */
int
bcmptm_ser_slice_mode_change(int unit,
                             bcmptm_rm_slice_change_t *slice_mode_change_info)
{
    int slice_even, slice_odd, slice_num;
    bcmptm_ser_slice_mode_t *slice_mode_ptr = NULL;
    bcmptm_rm_slice_mode_t slice_mode = 0;
    uint32_t start_index_wide = 0;
    uint32_t slice_depth_wide = 0;
    uint32_t group_id = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(slice_mode_change_info, SHR_E_PARAM);

    slice_even = slice_mode_change_info->slice_num0;
    slice_odd = slice_mode_change_info->slice_num1;

    group_id = slice_mode_change_info->slice_group_id;
    if (SLICE_GROUP_ID_IS_INVALID(unit, group_id)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Inputting slice_group_id is invalid!\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    slice_mode_ptr = SLICE_MODE_ARRAY(unit, group_id);
    SHR_NULL_CHECK(slice_mode_ptr, SHR_E_FAIL);

    slice_num = SLICE_NUM_OF_TCAM(unit, group_id);
    if (slice_even >= slice_num || slice_odd >= slice_num) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Inputting slice ID is invalid!\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    slice_mode = slice_mode_change_info->slice_mode;

    if (!IS_OVERLAY_SLICE_GROUP_ID(unit, group_id)) {
        /* update. slice_even or slice_odd can be '-1' for IFPm etc. */
        if (slice_even >= 0) {
            SLICE_MODE(unit, slice_mode_ptr, slice_even) = slice_mode;
        }
        if (slice_odd >= 0) {
            SLICE_MODE(unit, slice_mode_ptr, slice_odd) = slice_mode;
        }
    } else {
        /* Two slices are paired for wide view of overlay tcam table, such as L3_DEFIPm */
        if (slice_even < 0 || slice_odd < 0 || ((slice_even + 1) != slice_odd)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        SLICE_MODE(unit, slice_mode_ptr, slice_even) = slice_mode;
        SLICE_MODE(unit, slice_mode_ptr, slice_odd) = slice_mode;

        slice_depth_wide = SLICE_DEPTH_WIDE(unit, slice_mode_ptr, slice_even);
        /* all slices have the same depth */
        start_index_wide = slice_depth_wide * slice_even;
        /* two slices */
        slice_depth_wide *= 2;
        if (slice_mode == BCMPTM_TCAM_SLICE_NARROW) {
            OVERLAY_WIDE_SLICE_ENTRY_BITMAP_SET(unit, start_index_wide, slice_depth_wide);
        } else {
            OVERLAY_WIDE_SLICE_ENTRY_BITMAP_CLR(unit, start_index_wide, slice_depth_wide);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_entry_mode_change: to change mode of one entry of overlay tcam.
 */
int
bcmptm_ser_entry_mode_change(int unit,
                             bcmptm_rm_entry_mode_change_t *slice_mode_change_info)
{
    int entry_mode = 0;
    int index = (int)slice_mode_change_info->index;
    bcmptm_rm_op_t op_type = slice_mode_change_info->op_type;
    uint32_t wide_index= 0, slice_id = 0;
    uint32_t slice_depth_narrow = 0, slice_depth_wide = 0;
    bcmptm_ser_slice_mode_t *slice_mode_ptr = NULL;
    bcmptm_rm_slice_mode_t slice_mode;

    SHR_FUNC_ENTER(unit);

    if (!OVERLAY_SLICE_INFO_REGISTERED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (op_type == BCMPTM_RM_OP_EM_NARROW) {
        entry_mode = 1;
    } else if (op_type == BCMPTM_RM_OP_EM_WIDE) {
        entry_mode = 0;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    slice_mode_ptr = OVERLAY_TCAM_SLICE_MODE_ARRAY(unit);
    /* all slices have the same depth */
    slice_depth_wide = SLICE_DEPTH_WIDE(unit, slice_mode_ptr, 0);
    slice_depth_narrow = SLICE_DEPTH_NARROW(unit, slice_mode_ptr, 0);
    if ((slice_depth_wide == 0 && entry_mode==0) ||
        (slice_depth_narrow == 0 && entry_mode==1)) {

        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Slice depth should not be zero. "
                              "Wide depth [%d], narrow depth [%d]!\n"),
                   slice_depth_wide, slice_depth_narrow));
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }
    if (entry_mode) {
        slice_id = index / slice_depth_narrow;
    } else {
        slice_id = index / slice_depth_wide;
    }
    if (slice_id >= SLICE_NUM_OF_OVERLAY_TCAM(unit)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Index [%d] is in slice [%d] which ID"
                              " is larger than number of slice[%d]!\n"),
                   index, slice_id, SLICE_NUM_OF_OVERLAY_TCAM(unit)));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    slice_mode = SLICE_MODE(unit, slice_mode_ptr, slice_id);

    if (slice_mode == BCMPTM_TCAM_SLICE_NARROW) {
        /* wide mode */
        if (entry_mode == 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Index %d in narrow slice should"
                                  " not be used as wide mode!\n"),
                       index));
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        } else {
            SHR_EXIT();
        }
    }
    if (entry_mode) {
        wide_index = NARROW_INDEX_MAP_TO_WIDE_INDEX(index,
                                                    slice_depth_narrow,
                                                    slice_depth_wide);
        OVERLAY_WIDE_SLICE_ENTRY_BITMAP_SET(unit, wide_index, 1);
    } else {
        wide_index = index;
        OVERLAY_WIDE_SLICE_ENTRY_BITMAP_CLR(unit, wide_index, 1);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_slice_index_adjust used by TCAM scan
 */
int
bcmptm_ser_slice_index_adjust(int unit, bcmdrd_sid_t sid,
                              int *index, int *stride)
{
    bcmptm_ser_slices_info_t *slices_info = NULL;
    int mode_index = 0;
    bcmptm_ser_slice_mode_t *slice_mode_ptr = NULL;
    uint32_t slice_num = 0, i = 0, group_id = 0;
    uint32_t slice_max_index = 0, slice_min_index = 0;
    uint32_t index_tmp = 0;
    /* sort by depth of PT view. For example: depth of narrow view is larger than depth of wide view */
    bcmptm_rm_slice_mode_t rm_slice_mode[] = {
        BCMPTM_TCAM_SLICE_NARROW,
        BCMPTM_TCAM_SLICE_WIDE
    };

    SHR_FUNC_ENTER(unit);

    slices_info = bcmptm_ser_slice_info_find_by_sid(unit, sid, &mode_index, &group_id);
    /* need not to adjust */
    if (slices_info == NULL) {
        return SHR_E_NOT_FOUND;
    }

    if (mode_index >= BCMPTM_SER_SLICE_MODE_NUM) {
        /* If there is new slice mode, the new mode should be added to
        * bcmptm_ser_slice_mode_t.
        */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "So far there are only [%d] slice modes,"
                              " maybe new slice mode is added!\n"),
                   BCMPTM_SER_SLICE_MODE_NUM));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    slice_num = slices_info->slice_num;
    slice_mode_ptr = slices_info->slice_mode_array;

    slice_max_index = 0;
    index_tmp = (uint32_t)(*index);
    for (i = 0; i < slice_num; i++) {
        slice_max_index += slice_mode_ptr->slice_depth[mode_index];
        if ((index_tmp < slice_max_index) &&
            (rm_slice_mode[mode_index] == slice_mode_ptr->slice_mode)) {
            break;
        }
        slice_mode_ptr++;
    }
    slice_max_index -= 1;

    if (i == slice_num) {
        /* over */
        *index = (int)slice_max_index + 1;
        *stride = 0;
    } else {
        slice_min_index = 1 + slice_max_index;
        slice_min_index -= slice_mode_ptr->slice_depth[mode_index];
        if(slice_min_index <= index_tmp){
            /* a port of slice */
            *stride = slice_max_index - index_tmp + 1;
        } else {
            /* whole slice */
            *index = (int)slice_min_index;
            *stride = slice_mode_ptr->slice_depth[mode_index];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_overlay_index_stride_get if index is N and *stride is M, indexes N...(N+M-1)
* have the same entry mode.
 */
int
bcmptm_ser_overlay_index_stride_get(int unit, bcmdrd_sid_t sid,
                                    int index, int *stride)
{
    int rv = SHR_E_NONE, entry_mode = 1;
    uint32_t index_narrow = 0xffffffff, index_wide = 0xffffffff;
    uint32_t slice_id = 0, slice_offset = 0;
    uint32_t wide_slice_depth = 0, narrow_slice_depth = 0;
    bcmptm_ser_slice_mode_t *slice_mode_ptr = NULL;
    bcmptm_rm_slice_mode_t slice_mode = BCMPTM_TCAM_SLICE_NARROW;

    SHR_FUNC_ENTER(unit);

    if (!OVERLAY_SLICE_INFO_REGISTERED(unit)) {
        rv = SHR_E_INIT;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if ((NARROW_SID_OF_OVERLAY_TCAM_GROUP(unit) != sid) &&
         (WIDE_SID_OF_OVERLAY_TCAM_GROUP(unit) != sid)) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_EXIT(rv);
    }

    slice_mode_ptr = OVERLAY_TCAM_SLICE_MODE_ARRAY(unit);
    /* all slice have the same depth */
    narrow_slice_depth = SLICE_DEPTH_NARROW(unit, slice_mode_ptr, 0);
    wide_slice_depth = SLICE_DEPTH_WIDE(unit, slice_mode_ptr, 0);
    entry_mode = (NARROW_SID_OF_OVERLAY_TCAM_GROUP(unit) == sid) ? 1 : 0;
    /* 1: narrow mode */
    if (entry_mode) {
        index_narrow = (uint32_t)(index);
        slice_id = index_narrow / narrow_slice_depth;
    } else {
        index_wide = (uint32_t)(index);
        slice_id = index_wide / wide_slice_depth;
    }
    if (slice_id >= SLICE_NUM_OF_OVERLAY_TCAM(unit)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Index [%d] is in slice [%d] which ID"
                              " is larger than number of slice [%d]!\n"),
                   index, slice_id, SLICE_NUM_OF_OVERLAY_TCAM(unit)));
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    slice_mode = SLICE_MODE(unit, slice_mode_ptr, slice_id);
    /* 1: narrow mode */
    if (entry_mode) {
        if (slice_mode == BCMPTM_TCAM_SLICE_NARROW) {
            slice_offset = index_narrow % narrow_slice_depth;
            *stride = narrow_slice_depth - slice_offset;
        } else {
            index_wide = NARROW_INDEX_MAP_TO_WIDE_INDEX(index_narrow,
                                                        narrow_slice_depth,
                                                        wide_slice_depth);
            if (OVERLAY_WIDE_SLICE_ENTRY_BITMAP_GET(unit, index_wide)) {
                *stride = 1;
            } else {
                *stride = 0;
            }
        }
    } else {
        if (slice_mode == BCMPTM_TCAM_SLICE_NARROW) {
            *stride = 0;
        } else if (OVERLAY_WIDE_SLICE_ENTRY_BITMAP_GET(unit, index_wide)) {
            *stride = 0;
        } else {
            *stride = 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_paired_tcam_index_valid
 */
int
bcmptm_ser_paired_tcam_index_valid(int unit, bcmdrd_sid_t sid,
                                   int index, int *valid)
{
    int rv = SHR_E_NONE;
    int index_adjust = index, index_count = 0;
    bcmptm_ser_slices_info_t *slices_info = NULL;
    int mode_index = BCMPTM_SER_NARROW_INDEX;
    uint32_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    *valid = 0;
    slices_info =  bcmptm_ser_slice_info_find_by_sid(unit, sid, &mode_index,
                                                     &group_id);
    /* not found, not paired TCAM table */
    if (slices_info == NULL) {
        *valid = 1;
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    if (IS_OVERLAY_SLICE_GROUP_ID(unit, group_id)) {
        rv = bcmptm_ser_overlay_index_stride_get(unit, sid,
                                                 index, &index_count);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (index_count) {
            *valid = 1;
        }
    } else {
        rv = bcmptm_ser_slice_index_adjust(unit, sid,
                                           &index_adjust, &index_count);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (index_adjust == index) {
            *valid = 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_slice_mode_info_init allocate HA memory space is used to save slice mode information
 */
int
bcmptm_ser_slice_mode_info_init(int unit, bool warm)
{
    void *slice_info = NULL, *overlay_bitmap = NULL;
    uint32_t alloc_size = 0;
    uint32_t alloc_size_bitmap = sizeof(bcmptm_ser_entry_view_bitmap_t);
    int i = 0, overlay_found = 0, rv;
    sal_mutex_t slice_mode_mutex;
    bcmptm_ser_entry_view_bitmap_t *entry_bitmap = NULL;

    alloc_size = sizeof(bcmptm_ser_slices_info_t) * BCMPTM_SER_TCAM_NUM;

    slice_info = shr_ha_mem_alloc(unit,
                                  BCMMGMT_SER_COMP_ID,
                                  BCMPTM_HA_SUBID_SER_SLICES_INFO,
                                  "serSlicesInfo",
                                  &alloc_size);
    if (slice_info == NULL ||
        alloc_size < (sizeof(bcmptm_ser_slices_info_t) * BCMPTM_SER_TCAM_NUM)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to allocate HA memory space for slices_info.\n")));
        return SHR_E_MEMORY;
    }
    if (!warm) {
        sal_memset(slice_info, 0, alloc_size);
        rv = bcmissu_struct_info_report(unit, BCMMGMT_SER_COMP_ID,
                                              BCMPTM_HA_SUBID_SER_SLICES_INFO,
                                              0, sizeof(bcmptm_ser_slices_info_t),
                                              BCMPTM_SER_TCAM_NUM,
                                              BCMPTM_SER_SLICES_INFO_T_ID);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to report slices info to ISSU.\n")));
            return rv;
        }
    }
    overlay_bitmap = shr_ha_mem_alloc(unit,
                                      BCMMGMT_SER_COMP_ID,
                                      BCMPTM_HA_SUBID_SER_OVERLAY_BITMAP,
                                      "serOverlayBmp",
                                      &alloc_size_bitmap);
    if (overlay_bitmap == NULL ||
        alloc_size_bitmap < sizeof(bcmptm_ser_entry_view_bitmap_t)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to allocate HA memory space"
                              " for overlay entry bitmap.\n")));
        return SHR_E_MEMORY;
    }
    if (!warm) {
        sal_memset(overlay_bitmap, 0, alloc_size_bitmap);
        rv = bcmissu_struct_info_report(unit, BCMMGMT_SER_COMP_ID,
                                        BCMPTM_HA_SUBID_SER_OVERLAY_BITMAP,
                                        0, sizeof(bcmptm_ser_entry_view_bitmap_t), 1,
                                        BCMPTM_SER_ENTRY_VIEW_BITMAP_T_ID);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to report entry view bitmap to ISSU.\n")));
            return rv;
        }
    }
    tcam_slice_info[unit] = (bcmptm_ser_slices_info_t *)slice_info;
    entry_bitmap = (bcmptm_ser_entry_view_bitmap_t *)overlay_bitmap;
    overlay_wide_slice_bitmap[unit] = entry_bitmap->entry_view_bitmap;

    next_slice_group_id[unit] = 0;
    for (i = 0; i < BCMPTM_SER_TCAM_NUM; i++) {
        if (tcam_slice_info[unit][i].flags & BCMPTM_SER_SLICE_INFO_USED) {
            next_slice_group_id[unit] = i + 1;
        }
        if (tcam_slice_info[unit][i].flags & BCMPTM_SER_IS_OVERLAY_TCAM) {
            overlay_slice_group_id[unit] = i;
            overlay_found = 1;
        }
        slice_mode_mutex = NULL;
        slice_mode_mutex = sal_mutex_create("slice_mode_mutex");
        if (slice_mode_mutex == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create slice mode mutex.\n")));
            bcmptm_ser_slice_mode_info_deinit(unit, warm);
            return SHR_E_MEMORY;
        }
        SLICE_MODE_MUTEX(unit, i) = slice_mode_mutex;
    }
    if (overlay_found == 0) {
        overlay_slice_group_id[unit] = BCMPTM_SER_TCAM_NUM;
    }
    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_slice_mode_info_deinit free memory space is used to save slice mode information
 */
void
bcmptm_ser_slice_mode_info_deinit(int unit, bool warm)
{
    uint32_t i = 0;

    for (i = 0; i < BCMPTM_SER_TCAM_NUM; i++) {
        if (SLICE_MODE_MUTEX(unit, i) != NULL) {
            sal_mutex_destroy(SLICE_MODE_MUTEX(unit, i));
            SLICE_MODE_MUTEX(unit, i) = NULL;
        }
    }
    if (!warm) {
        tcam_slice_info[unit] = NULL;
        overlay_wide_slice_bitmap[unit] = NULL;
    }
    next_slice_group_id[unit] = 0;
    overlay_slice_group_id[unit] = BCMPTM_SER_TCAM_NUM;
}

/******************************************************************************
* bcmptm_ser_slice_mode_mutex_get
 */
int
bcmptm_ser_slice_mode_mutex_get(int unit, bcmdrd_sid_t sid, sal_mutex_t *slice_mode_mutex)
{
    bcmptm_ser_slices_info_t *slice_info_ptr = NULL;
    int sid_index = 0;
    uint32_t slice_info_index = 0;

    slice_info_ptr = bcmptm_ser_slice_info_find_by_sid(unit, sid, &sid_index,
                                                       &slice_info_index);
    if (slice_info_ptr == NULL) {
        return SHR_E_NOT_FOUND;
    }
    *slice_mode_mutex = SLICE_MODE_MUTEX(unit, slice_info_index);
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_slice_mode_info_print used to print slices information
 */
void
bcmptm_ser_slice_mode_info_print(int unit, int overlay_info_print)
{
    bcmptm_ser_slices_info_t *slices_info_node = NULL;
    int i = 0, num = 0;
    uint32_t  group_id = 0;
    bcmptm_ser_slice_mode_t *slice_mode_array = NULL;
    char *slice_mode_str[] = { "invalid", "disable", "narrow", "wide" };

    LOG_CLI((BSL_META_U(unit,
                        "------------------------------------\n")));

    TCAM_SLICE_INFO_ITER(unit, slices_info_node, group_id){
        num = slices_info_node->sid_num;
        LOG_CLI((BSL_META_U(unit,
                            " Number of SID is [%d], PT is:\n"), num));
        for (i = 0; i < num; i++) {
            LOG_CLI((BSL_META_U(unit,
                                " [%s] "),
                     bcmdrd_pt_sid_to_name(unit, slices_info_node->sid_array[i])));
        }
        num = slices_info_node->slice_num;
        LOG_CLI((BSL_META_U(unit,
                            " Number of slice is [%d]\n"), num));
        slice_mode_array = slices_info_node->slice_mode_array;
        for (i = 0; i < num; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "Mode of slice[%d] is [%s],"
                                " slice depth of narrow view is [%d], wide view is [%d]\n"),
                     (i), slice_mode_str[SLICE_MODE(unit, slice_mode_array, i)],
                     SLICE_DEPTH_NARROW(unit, slice_mode_array, i), SLICE_DEPTH_WIDE(unit, slice_mode_array, i)));
        }
        if (!IS_OVERLAY_SLICE_GROUP_ID(unit, group_id)) {
            continue;
        }
        if (overlay_info_print) {
            bcmptm_ser_overlay_info_print(unit);
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n Slice group ID is [%d]\n\n"), group_id));
    }
}

