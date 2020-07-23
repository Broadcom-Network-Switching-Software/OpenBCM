/*! \file rm_tcam.c
 *
 * Low Level Functions for TCAM resource manager.
 * This file contains low level functions used across all types
 * (priority or prefix based) of TCAM entry management.
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "rm_tcam_fp_entry_mgmt.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <shr/shr_ha.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Typedefs
 */
/* Global data structure to hold memory offsets of all LTIDs on
 * all Logical device ids
 */
typedef struct req_info_actual_s {
    uint32_t **ekw;
    uint32_t **edw;
} req_info_actual_t;

typedef struct req_info_updated_s {
    uint32_t updated_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t updated_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *ekw[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    uint32_t *edw[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
} req_info_updated_t;


/*******************************************************************************
 * Global variables
 */
req_info_actual_t actual_req_info[BCMDRD_CONFIG_MAX_UNITS];

req_info_updated_t updated_req_info[BCMDRD_CONFIG_MAX_UNITS];

/* To hold previously looked up index */
bcmptm_rm_tcam_same_key_data_t same_key_info[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief LT index traverse callback function
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table id
 * \param [in] user_data memory address to data provided by caller to
 *             bcmptm_ltindex_traverse.
 *
 * \retval SHR_E_NONE Success
 */

typedef int (*bcmptm_ltindex_traverse_cb)(int unit,
                                          int lt_index,
                                          void *user_data);

/* Function pointers to different operations of prority based TCAMs */
static bcmptm_rm_tcam_funct_t prio_funct_ptr =
    {
         bcmptm_rm_tcam_prio_eid_entry_insert,
         bcmptm_rm_tcam_prio_eid_entry_lookup,
         bcmptm_rm_tcam_prio_eid_entry_delete,
         bcmptm_rm_tcam_prio_eid_entry_move,
         bcmptm_rm_tcam_traverse_info_entry_get_first,
         bcmptm_rm_tcam_traverse_info_entry_get_next,
         bcmptm_rm_tcam_prio_eid_tcam_get_table_info,
    };

static bcmptm_rm_tcam_funct_t prio_only_funct_ptr =
    {
         bcmptm_rm_tcam_prio_only_entry_insert,
         bcmptm_rm_tcam_prio_only_entry_lookup,
         bcmptm_rm_tcam_prio_only_entry_delete,
         bcmptm_rm_tcam_prio_only_entry_move,
         bcmptm_rm_tcam_traverse_info_entry_get_first,
         bcmptm_rm_tcam_traverse_info_entry_get_next,
         bcmptm_rm_tcam_get_table_info,
    };
static bcmptm_rm_tcam_funct_t fp_funct_ptr =
    {
         bcmptm_rm_tcam_fp_entry_insert,
         bcmptm_rm_tcam_fp_entry_lookup,
         bcmptm_rm_tcam_fp_entry_delete,
         bcmptm_rm_tcam_fp_entry_move,
         NULL,
         NULL,
         bcmptm_rm_tcam_fp_get_table_info,
    };



/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_tcam_check_butterfly_mode(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *is_half_mode)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    *is_half_mode = 0;

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
        *is_half_mode = 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/* To extract bits from a particular position to a particular length */
int
bcmptm_rm_tcam_data_get(int unit,
                        uint32_t *in_data,
                        uint16_t start_pos,
                        uint16_t len,
                        uint32_t *out_data)
{
    int current_word, bit_pos, result_word;
    int i;
    int max_size = BCMPTM_MAX_PT_FIELD_WORDS;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_data, SHR_E_PARAM);
    SHR_NULL_CHECK(out_data, SHR_E_PARAM);

    /* Input parameters check. */
    if ((NULL == in_data) || (NULL == out_data) ||
        ((start_pos + len) > (BCMPTM_MAX_PT_FIELD_WORDS * 32))) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    current_word = start_pos / 32;
    bit_pos = start_pos - current_word * 32;

    sal_memset(out_data, 0, max_size);

    for (i = 0, result_word = 0; i < len;
         i += 32, current_word++, result_word++) {

        out_data[result_word] = in_data[current_word] >> bit_pos;

        if ((bit_pos != 0) && ((bit_pos + (len - i)) > 32)) {
            out_data[result_word] |= (in_data[current_word + 1]
                                     << (32 - bit_pos));
        }
    }
    /* This check may be redundant as -1 = 0xffffffff */
    if (i != len) {
        i -= 32;
        out_data[result_word - 1] &= ((1 << (len - i)) - 1);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Fetch the hw entry information of an LT based on LT entries mode. */
int
bcmptm_rm_tcam_lt_hw_entry_info_get(int unit,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  void *attrs,
                  const bcmptm_rm_tcam_hw_entry_info_t **hw_entry_info)
{
    bcmptm_rm_tcam_group_mode_t mode;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_entry_info, SHR_E_PARAM);

    if ((attrs != NULL) && (ltid_info->pt_type == LT_PT_TYPE_FP)) {
       entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    *hw_entry_info = &(ltid_info->hw_entry_info[mode]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Fetch the number of physical indexs needed to write the entry
 * \n for the given Logical Table id.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] num_index_req physical indexes needed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
int
bcmptm_rm_tcam_num_index_required_get(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t *num_index_req)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(num_index_req, SHR_E_PARAM);

    if (ltid_info->rm_more_info->flags &
            BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        if (ltid_info->hw_entry_info->view_type)  {
            *num_index_req = ltid_info->hw_entry_info->num_key_rows
                / ltid_info->hw_entry_info->view_type;
        } else  {
            *num_index_req = ltid_info->hw_entry_info->num_key_rows;
        }
    } else {
        *num_index_req = 1;
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_tindex_to_sindex(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint32_t rm_flags,
                                uint32_t tindex,
                                uint32_t *sindex,
                                uint8_t *slice_row,
                                uint8_t *slice_column)
{
    uint8_t s_row = 0;
    uint8_t s_column = 0;
    uint16_t min_idx = 0;
    uint16_t max_idx = 0;
    uint32_t num_entries = 0;
    bcmdrd_sid_t sid = 0;
    uint8_t num_lookups = 1;
    bool slice_row_found = FALSE;
    uint8_t num_key_rows = 0;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    const bcmltd_sid_t **sid_array = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_row, SHR_E_PARAM);
    SHR_NULL_CHECK(sindex, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;

    if (rm_flags & BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
        num_key_rows = ltid_info->hw_entry_info->num_data_rows;
    } else {
        num_key_rows = ltid_info->hw_entry_info->num_key_rows;
    }

    if ((ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) &&
        !(rm_flags & BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW)) {
        *sindex = tindex / 2;
    } else {
        *sindex = tindex;
    }

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_lookups = 2;
        num_key_rows = num_key_rows / 2;
    }

    /*
     * For XX_PAIR_TCAM type sid- to correct the mapping of
     * tindex to sindex.
     */
    if (ltid_info->hw_entry_info->view_type &&
        !(ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE)) {
        *sindex = tindex / ltid_info->hw_entry_info->view_type;
    }

    *slice_column = s_column;

    do {
        for (s_row = 0; s_row < ltid_info->hw_entry_info->num_depth_inst;
             s_row++) {

              if ((rm_flags & BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) &&
                  (ltid_info->rm_more_info->flags &
                   BCMPTM_RM_TCAM_HALF_MODE)) {
                  sid_array = ltid_info->hw_entry_info->sid_data_only;
                  sid = sid_array[s_row][s_column];
              } else {
                  sid_array = ltid_info->hw_entry_info->sid;
                  sid = sid_array[s_row][s_column];
              }
              min_idx = bcmptm_pt_index_min(unit, sid);
              max_idx = bcmptm_pt_index_max(unit, sid);
              num_entries = (max_idx - min_idx + 1);

             if ((*sindex) > (num_entries - 1)) {

                 (*sindex) -= num_entries;
                 /*
                  * Same SID is present at 2 SID array rows for half mode
                  * in ptrm yml file. Ignore the same s_row for half mode.
                  */
                if ((ltid_info->rm_more_info->flags &
                     BCMPTM_RM_TCAM_HALF_MODE)) {
                    s_row++;
                }
             } else {
                *slice_row = s_row;
                slice_row_found = TRUE;
                break;
             }
        }
        if (slice_row_found == TRUE) {
            break;
        }
        s_row = 0;
        s_column += num_key_rows;
        *slice_column = s_column;
        num_lookups--;

    } while (num_lookups > 0);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the total number of entries in a TCAM
 */
int
bcmptm_rm_tcam_depth_get(int unit,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         uint32_t *num_entries)
{
    uint8_t row = 0;
    uint8_t col = 0;
    uint16_t min_idx = 0;
    uint16_t max_idx = 0;
    bcmdrd_sid_t sid;
    bool invalid_sid = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    *num_entries = 0;
    for (row = 0; row < ltid_info->hw_entry_info->num_depth_inst; row++) {
        /*
         * During dynamic update of LT resources, there
         * might be a possibility of misconfiguration. If there is
         * any INVALIDm in sid info for multi-wide LT's, consider
         * ENTRY's only till the row which is before the INVAILDm row.
         */
        for (col = 0; col < ltid_info->hw_entry_info->num_key_rows; col++) {
            if (ltid_info->hw_entry_info->sid[row][col] == INVALIDm) {
                /* Some bank/SID's are not correctly configured by USER */
                LOG_VERBOSE(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                      "Misconfiguration of dynamic banks for ltid:%d.\n"),
                      ltid));
                invalid_sid = TRUE;
                break;
            }
        }
        if (invalid_sid == TRUE) {
            *num_entries = 0;
            break;
        }
        sid = ltid_info->hw_entry_info->sid[row][0];
        min_idx = bcmptm_pt_index_min(unit, sid);
        max_idx = bcmptm_pt_index_max(unit, sid);
        *num_entries += (max_idx - min_idx + 1);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get the start and end index, in logical space,
 * of a physical table if LT depth expands to mtultiple
 * physical tables.
 */
int
bcmptm_rm_tcam_logical_space_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t sid_row,
                                 uint16_t *start_idx,
                                 uint16_t *end_idx)
{
    uint8_t row = 0;
    uint8_t col = 0;
    uint16_t min_idx = 0;
    uint16_t max_idx = 0;
    bcmdrd_sid_t sid;
    uint16_t num_entries = 0;
    bool invalid_sid = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(start_idx, SHR_E_PARAM);
    SHR_NULL_CHECK(end_idx, SHR_E_PARAM);

    if (ltid_info->hw_entry_info->num_depth_inst <= sid_row) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (row = 0; row < sid_row; row++) {
        /*
         * During dynamic update of LT resources, there
         * might be a possibility of misconfiguration. If there is
         * any INVALIDm in sid info for multi-wide LT's, consider
         * ENTRY's only till the row which is before the INVAILDm row.
         */
        for (col = 0; col < ltid_info->hw_entry_info->num_key_rows; col++) {
            if (ltid_info->hw_entry_info->sid[row][col] == INVALIDm) {
                /* Some bank/SID's are not correctly configured by USER */
                LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                      "Misconfiguration of dynamic banks for ltid:%d.\n"),
                      ltid));
                invalid_sid = TRUE;
                break;
            }
        }
        if (invalid_sid == TRUE) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        sid = ltid_info->hw_entry_info->sid[row][0];
        min_idx = bcmptm_pt_index_min(unit, sid);
        max_idx = bcmptm_pt_index_max(unit, sid);
        num_entries += (max_idx - min_idx + 1);
    }
    sid = ltid_info->hw_entry_info->sid[row][0];
    for (col = 0; col < ltid_info->hw_entry_info->num_key_rows; col++) {
        if (ltid_info->hw_entry_info->sid[row][col] == INVALIDm) {
            /* Some bank/SID's are not correctly configured by USER */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                      "Misconfiguration of dynamic banks for ltid:%d.\n"),
                      ltid));
            invalid_sid = TRUE;
            break;
        }
    }
    if (invalid_sid == TRUE) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    min_idx = bcmptm_pt_index_min(unit, sid);
    max_idx = bcmptm_pt_index_max(unit, sid);

    *start_idx = num_entries;
    *end_idx = num_entries + (max_idx - min_idx);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the active pipe count of LT. Thought LT is a per pipe
 * resource, It may not operate in per pipe mode.
 */
int
bcmptm_rm_tcam_num_pipes_get(int unit,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         uint8_t *num_pipes)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_pipes, SHR_E_PARAM);

    *num_pipes = ltid_info->rm_more_info[0].pipe_count;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_update_full_hw_entry(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd,
                              uint32_t index,
                              uint32_t *full_entry_words,
                              uint32_t *entry_words)
{
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint32_t g_buf_one[BCMPTM_MAX_PT_FIELD_WORDS];
    bcmptm_rm_tcam_hw_field_list_t *src_hw_fields = NULL;
    bcmptm_rm_tcam_hw_field_list_t *dest_hw_fields = NULL;
    uint8_t i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;

    if (index % 2 == 0) {
        dest_hw_fields = &(tcam_info->hw_field_list[0]);
    } else {
        dest_hw_fields = &(tcam_info->hw_field_list[1]);
    }

    src_hw_fields = &(tcam_info->hw_field_list[0]);

    for (i = 0; i < dest_hw_fields->num_fields; i++) {
        sal_memset(g_buf_one, 0,
                sizeof(g_buf_one));

        bcmdrd_field_get(entry_words,
                         src_hw_fields->field_start_bit[i],
                         (src_hw_fields->field_start_bit[i] +
                         src_hw_fields->field_width[i] - 1),
                         g_buf_one);
        bcmdrd_field_set(full_entry_words,
                         dest_hw_fields->field_start_bit[i],
                         (dest_hw_fields->field_start_bit[i] +
                         dest_hw_fields->field_width[i] - 1),
                         g_buf_one);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_half_mode_read(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd,
                              uint32_t index,
                              uint32_t *entry_words)
{
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint32_t entry_words_copy[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t g_buf_one[BCMPTM_MAX_PT_FIELD_WORDS];
    bcmptm_rm_tcam_hw_field_list_t *src_hw_fields = NULL;
    bcmptm_rm_tcam_hw_field_list_t *dest_hw_fields = NULL;
    uint8_t i = 0;
    uint16_t key_size = 0;
    bcmltd_sid_t sid = 0;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    sid = iomd->ltid_info->hw_entry_info[0].sid[0][0];
    key_size = bcmdrd_pt_entry_wsize(unit, sid);
    size = (key_size * sizeof(uint32_t));
    sal_memset(entry_words_copy, 0 , size);

    tcam_info = iomd->tcam_info;
    if (index % 2 == 0) {
        src_hw_fields = &(tcam_info->hw_field_list[0]);
    } else {
        src_hw_fields = &(tcam_info->hw_field_list[1]);
    }

    dest_hw_fields = &(tcam_info->hw_field_list[0]);

    for (i = 0; i < dest_hw_fields->num_fields; i++) {
        sal_memset(g_buf_one, 0, sizeof(g_buf_one));
        bcmdrd_field_get(entry_words,
                         src_hw_fields->field_start_bit[i],
                         (src_hw_fields->field_start_bit[i] +
                         src_hw_fields->field_width[i] - 1),
                         g_buf_one);
        bcmdrd_field_set(entry_words_copy,
                         dest_hw_fields->field_start_bit[i],
                         (dest_hw_fields->field_start_bit[i] +
                         dest_hw_fields->field_width[i] - 1),
                         g_buf_one);
    }

    sal_memset(entry_words, 0, size);

    bcmdrd_field_set(entry_words,
                     0,
                     ((key_size * 32) - 1),
                     entry_words_copy);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_hw_read(int unit,
                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                       uint32_t rm_flags,
                       uint32_t index,
                       uint32_t **entry_words)
{
    uint32_t sindex = 0;
    uint8_t part = 0;
    uint8_t slice_row = 0;
    uint8_t slice_col = 0;
    bcmdrd_sid_t sid = 0;
    int num_parts = -1;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    const bcmltd_sid_t **sid_array = NULL;
    bool rsp_entry_cache_vbit = FALSE;
    uint16_t rsp_dfield_format_id = 0;
    size_t words_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;
    entry_attrs = iomd->entry_attrs;
    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
        words_size = iomd->req_info->rsp_edw_buf_wsize;
    } else {
        words_size = iomd->req_info->rsp_ekw_buf_wsize;
        /* This includes AGGR_VIEW and TCAM view */
        num_parts = ltid_info->hw_entry_info[mode].num_key_rows;
        /* For XX_PAIR_TCAM num of parts should be 1. */
        if (ltid_info->hw_entry_info->view_type) {
            num_parts =
                (num_parts / ltid_info->hw_entry_info->view_type);
        }
    }

    if (ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_parts = num_parts / 2;
    }

    for (part = 0; part < num_parts; part++) {
        slice_col = 0;
        if (ltid_info->pt_type != LT_PT_TYPE_FP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_tindex_to_sindex(unit,
                                                 iomd,
                                                 rm_flags,
                                                 index,
                                                 &sindex,
                                                 &slice_row,
                                                 &slice_col));
            /* WIDTH expansion use same index for other parts */
            if (ltid_info->rm_more_info->flags &
                BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
                iomd->pt_dyn_info->index = sindex + part;
            } else {
                iomd->pt_dyn_info->index = sindex;
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                    iomd,
                                                    rm_flags,
                                                    index,
                                                    part,
                                                    &slice_row,
                                                    &sindex));
            iomd->pt_dyn_info->index = sindex;
        }

        slice_col += part;
        if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
            sid_array = ltid_info->hw_entry_info[mode].sid_data_only;
            sid = sid_array[slice_row][slice_col];
        } else {
            sid_array = ltid_info->hw_entry_info[mode].sid;
            sid = sid_array[slice_row][slice_col];
        }


        /* Insufficient buffer */
        if (words_size < bcmdrd_pt_entry_wsize(unit, sid)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cmdproc_read(unit,
                                 iomd->req_flags,
                                 iomd->trans_id,
                                 sid,
                                 iomd->pt_dyn_info,
                                 NULL,
                                 words_size,
                                 entry_words[part],
                                 &rsp_entry_cache_vbit,
                                 iomd->rsp_ltid,
                                 &rsp_dfield_format_id,
                                 iomd->rsp_flags));

        if ((ltid_info->rm_more_info->flags &
             BCMPTM_RM_TCAM_HALF_MODE) &&
            (ltid_info->pt_type != LT_PT_TYPE_FP) &&
            (rm_flags != BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW)) {

            /*
             * Update entry words to have only the correct
             * half of entry.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_half_mode_read(unit,
                                               iomd,
                                               index,
                                               entry_words[part]));
        }

        if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
            if (part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS &&
                entry_attrs->lookup_id == 1) {
                iomd->rsp_info->rsp_entry_data_sid[part] = sid;
                iomd->rsp_info->rsp_entry_data_index[part] =
                                iomd->pt_dyn_info->index;
                iomd->rsp_info->rsp_entry_data_index_count = num_parts;
            }
        } else {
            if (part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS &&
                entry_attrs->lookup_id == 1) {
                iomd->rsp_info->rsp_entry_sid[part] = sid;
                iomd->rsp_info->rsp_entry_index[part] =
                                iomd->pt_dyn_info->index;
                iomd->rsp_info->rsp_entry_index_count = num_parts;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_hw_write(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                        uint32_t rm_flags,
                        uint32_t index,
                        uint32_t **entry_words)

{
    uint32_t sindex = 0;
    uint8_t part = 0;
    uint8_t slice_row = 0;
    uint8_t slice_column = 0;
    bcmdrd_sid_t sid = 0;
    int num_parts = -1;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    uint32_t full_entry_words[BCMPTM_MAX_PT_FIELD_WORDS];
    bool rsp_entry_cache_vbit = 0;
    uint16_t rsp_dfield_format_id = 0;
    const bcmdrd_sid_t **sid_array = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;
    entry_attrs = iomd->entry_attrs;
    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
    } else {
        /* This includes AGGR_VIEW and TCAM view */
        num_parts = ltid_info->hw_entry_info[mode].num_key_rows;
        if (ltid_info->hw_entry_info->view_type) {
            num_parts =
                (num_parts / ltid_info->hw_entry_info->view_type);
        }
    }

    if (ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_parts = num_parts / 2;
    }

    for (part = 0; part < num_parts; part++) {
        slice_column = 0;
        if (ltid_info->pt_type != LT_PT_TYPE_FP) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_tindex_to_sindex(unit,
                                                  iomd,
                                                  rm_flags,
                                                  index,
                                                  &sindex,
                                                  &slice_row,
                                                  &slice_column));
             /* WIDTH expansion use same index for other parts. */
             if (ltid_info->rm_more_info->flags &
                 BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
                 iomd->pt_dyn_info->index = sindex + part;
             } else {
                 iomd->pt_dyn_info->index = sindex;
             }
        } else {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                     iomd,
                                                     rm_flags,
                                                     index,
                                                     part,
                                                     &slice_row,
                                                     &sindex));
             iomd->pt_dyn_info->index = sindex;
        }
        slice_column += part;
        if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
            sid_array = ltid_info->hw_entry_info[mode].sid_data_only;
            sid = sid_array[slice_row][slice_column];
        } else {
            sid_array = ltid_info->hw_entry_info[mode].sid;
            sid = sid_array[slice_row][slice_column];
        }

        if ((ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE)
            && (ltid_info->pt_type != LT_PT_TYPE_FP)
            && (rm_flags != BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW)) {

            sal_memset(full_entry_words, 0, sizeof(full_entry_words));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_read(unit,
                                     iomd->req_flags,
                                     iomd->trans_id,
                                     sid,
                                     iomd->pt_dyn_info,
                                     NULL,
                                     entry_words_size,
                                     full_entry_words,
                                     &rsp_entry_cache_vbit,
                                     iomd->rsp_ltid,
                                     &rsp_dfield_format_id,
                                     iomd->rsp_flags));

            /* Update FULL TCAM entry from PTCache. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_update_full_hw_entry(unit,
                                                     iomd,
                                                     index,
                                                     full_entry_words,
                                                     entry_words[part]));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                      iomd->req_flags,
                                      iomd->trans_id,
                                      sid,
                                      iomd->pt_dyn_info,
                                      NULL,
                                      full_entry_words,
                                      1,
                                      1,
                                      1,
                                      BCMPTM_RM_OP_NORMAL,
                                      NULL,
                                      iomd->ltid,
                                      0,
                                      iomd->rsp_flags));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cmdproc_write(unit,
                                      iomd->req_flags,
                                      iomd->trans_id,
                                      sid,
                                      iomd->pt_dyn_info,
                                      NULL,
                                      entry_words[part],
                                      1,
                                      1,
                                      1,
                                      BCMPTM_RM_OP_NORMAL,
                                      NULL,
                                      iomd->ltid,
                                      0,
                                      iomd->rsp_flags));
        }
        if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
            if (part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS &&
                entry_attrs->lookup_id == 1) {
                iomd->rsp_info->rsp_entry_data_sid[part] = sid;
                iomd->rsp_info->rsp_entry_data_index[part] =
                                iomd->pt_dyn_info->index;
                iomd->rsp_info->rsp_entry_data_index_count = num_parts;
            }
        } else {
            if (part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS &&
                entry_attrs->lookup_id == 1) {
                iomd->rsp_info->rsp_entry_sid[part] = sid;
                iomd->rsp_info->rsp_entry_index[part] =
                                iomd->pt_dyn_info->index;
                iomd->rsp_info->rsp_entry_index_count = num_parts;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the pointer that holds function pointers to basic
 * TCAM operations of a TCAM.
 */
int
bcmptm_rm_tcam_funct_get(int unit,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         void **funct)
{


    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(funct, SHR_E_PARAM);

    switch (ltid_info->pt_type) {
        case LT_PT_TYPE_TCAM_PRI_ONLY:
              *funct = &prio_only_funct_ptr;
             break;
        case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
             *funct = &prio_funct_ptr;
             break;
        case LT_PT_TYPE_FP:
             *funct = &fp_funct_ptr;
             break;
        default:
             break;
    }

    if ((*funct) == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * TCAM resource manager supports three basic operations(lookup, insert and
 * delete). This function is to request one of the opeartions for any TCAM.
 * This will dispatch the request to corresponding operations function pointer
 * of the TCAM.
 *
 * Mostly all priority based TCAMs have one set of function pointers.
 * Some TCAMs may have its own set of function pointers.
 */
int
bcmptm_rm_tcam_req(int unit,
                   uint64_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_tcam_req_t *req_info,
                   bcmptm_rm_tcam_rsp_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_funct_t *funct = NULL;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs1 = NULL;
    bcmptm_rm_tcam_trans_info_t *ltid_trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_entry_iomd_t *iomd = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      0,
                                      ((void *)&ltid_info),
                                      NULL,
                                      NULL));

    /* Fetch the function pointer to dipatch the PTM operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_funct_get(unit,
                                  ltid,
                                  &ltid_info,
                                  (void *)&funct));

    /* set the pipe_id in entry_attrs structure */
    if (req_info->entry_attrs == NULL) {
        /* For global mode, use pipe_id 0 */
        size = sizeof(bcmptm_rm_tcam_entry_attrs_t);
        SHR_ALLOC(entry_attrs,
                  size,
                  "bcmptmRmTcamReqInfoEntryAttrs");
        sal_memset(entry_attrs, 0, size);
        if (pt_dyn_info->tbl_inst == -1) {
            entry_attrs->pipe_id = 0;
        } else {
            entry_attrs->pipe_id = pt_dyn_info->tbl_inst;
        }

        entry_attrs->lookup_id = 1;

        req_info->entry_attrs = (void *)entry_attrs;
    } else {
        /* Set lookup_id to 1 */
        entry_attrs1 = (bcmptm_rm_tcam_entry_attrs_t *) req_info->entry_attrs;
        entry_attrs1->lookup_id = 1;
    }
    size = sizeof(bcmptm_rm_tcam_entry_iomd_t);
    SHR_ALLOC(iomd, size, "bcmptmRmTcamEntryIomd");
    sal_memset(iomd, 0, size);

    if (ltid_info.pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID ||
        ltid_info.pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_iomd_init(unit,
                                               req_flags,
                                               cseq_id,
                                               ltid,
                                               &ltid_info,
                                               pt_dyn_info,
                                               req_op,
                                               req_info,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags,
                                               iomd));
        if (ltid_info.pt_type == LT_PT_TYPE_FP) {
            if (iomd->group_ptr != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd));
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_iomd_init(unit,
                                               req_flags,
                                               cseq_id,
                                               ltid,
                                               &ltid_info,
                                               pt_dyn_info,
                                               req_op,
                                               req_info,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags,
                                               iomd));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_iomd_update(unit, iomd));
    }

    /* Dispatch the operation corresponding function pointer. */
    switch (req_op) {
        case BCMPTM_OP_LOOKUP:
            if (funct->entry_lookup != NULL) {
                rv = funct->entry_lookup(unit,
                                         req_flags,
                                         ltid,
                                         &ltid_info,
                                         pt_dyn_info,
                                         req_info,
                                         rsp_info,
                                         rsp_ltid,
                                         rsp_flags,
                                         NULL, iomd);
            }
            break;
        case BCMPTM_OP_INSERT:
            if (funct->entry_insert != NULL) {
                rv = funct->entry_insert(unit, iomd);
            }
            break;
        case BCMPTM_OP_DELETE:
            if (funct->entry_delete != NULL) {
                rv = funct->entry_delete(unit,
                                         req_flags,
                                         ltid,
                                         &ltid_info,
                                         pt_dyn_info,
                                         req_info,
                                         rsp_info,
                                         rsp_ltid,
                                         rsp_flags,
                                         iomd);
            }
            break;
        case BCMPTM_OP_GET_FIRST:
            if (funct->entry_get_first != NULL) {
                rv = funct->entry_get_first(unit,
                                            req_flags,
                                            ltid,
                                            &ltid_info,
                                            pt_dyn_info,
                                            req_info,
                                            rsp_info,
                                            rsp_ltid,
                                            rsp_flags,
                                            iomd);
            }
            break;
        case BCMPTM_OP_GET_NEXT:
            if (funct->entry_get_next != NULL) {
                rv = funct->entry_get_next(unit,
                                           req_flags,
                                           ltid,
                                           &ltid_info,
                                           pt_dyn_info,
                                           req_info,
                                           rsp_info,
                                           rsp_ltid,
                                           rsp_flags,
                                           iomd);
            }
            break;
         case BCMPTM_OP_GET_TABLE_INFO:
             if (funct->get_table_info != NULL) {
                 rv = funct->get_table_info(unit,
                                            req_flags,
                                            ltid,
                                            &ltid_info,
                                            pt_dyn_info,
                                            req_info,
                                            rsp_info,
                                            rsp_ltid,
                                            rsp_flags,
                                            iomd);
             }
             break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (req_op == BCMPTM_OP_INSERT ||
        req_op == BCMPTM_OP_DELETE) {
        /* Fetch the transaction state and update it. */
        if (iomd->req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_info_get(unit,
                                         ltid,
                                         -1,
                                         &ltid_info,
                                         (void *)&ltid_trans_info));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_info_get(unit,
                                         ltid,
                                         pt_dyn_info->tbl_inst,
                                         &ltid_info,
                                         (void *)&ltid_trans_info));
        }
        if (ltid_trans_info->trans_state ==
            BCMPTM_RM_TCAM_STATE_IDLE) {
            ltid_trans_info->trans_state =
                BCMPTM_RM_TCAM_STATE_UC_A;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_trans_info_global_get(unit,
                                           &trans_info_global));
            if (trans_info_global->trans_state ==
                BCMPTM_RM_TCAM_STATE_IDLE) {
                trans_info_global->trans_state =
                    BCMPTM_RM_TCAM_STATE_UC_A;
            }
        }
    }

exit:
    if (req_info->entry_attrs == (void *)entry_attrs) {
        req_info->entry_attrs = NULL;
    }
    SHR_FREE(entry_attrs);
    SHR_FREE(iomd);
    SHR_FUNC_EXIT();
}

/*
 * Move the TCAM entry from one TCAM index to the other TCAM index.
 * This function will disaptch the move request to TCAM specific
 * function pointer. Mostly all priority based TCAMs have one set of function
 * pointers.
 * Some TCAMs may have its own set of function pointers.
 */
int
bcmptm_rm_tcam_entry_move(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          uint8_t partition,
                          uint32_t from_idx,
                          uint32_t to_idx,
                          void *attrs,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_funct_t *funct = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_funct_get(unit,
                                  ltid,
                                  ltid_info,
                                  (void *)&funct));

    if (funct->entry_move != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (funct->entry_move(unit,
                               ltid,
                               ltid_info,
                               partition,
                               -1,
                               from_idx,
                               to_idx,
                               attrs,
                               pt_dyn_info,
                               iomd));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

