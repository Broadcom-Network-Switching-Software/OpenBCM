/*! \file bcmptm_rm_hash_flex_hit_mgmt.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmevm/bcmevm_api.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */
/*! \brief Describe entry physical hit index information.
  - This structure describes entry physical hit index information. */
typedef struct rm_hash_hit_index_info_s {
    /*! \brief Number of entry instances. */
    uint8_t num;

    /*! \brief Entry hit index list. */
    uint32_t hit_index_list[RM_HASH_LOOKUP_MAX_CNT];
} rm_hash_hit_index_info_t;

/*******************************************************************************
 * Private variables
 */
/*! Per-unit entry hit index info. */
static rm_hash_hit_index_info_t hit_index_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Get the narrow mode hash LT entry width.
 *
 * \param [in] unit Unit number.
 * \param [in] e_nm Entry narrow mode.
 * \param [out] entry_width Entry width for the given narrow mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_narrow_entry_width_get(int unit,
                               rm_hash_entry_narrow_mode_t e_nm,
                               bcmptm_rm_hash_entry_width_t *entry_width)
{
    SHR_FUNC_ENTER(unit);

    switch (e_nm) {
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF:
        *entry_width = BCMPTM_RM_HASH_ENTRY_WIDTH_HALF;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD:
        *entry_width = BCMPTM_RM_HASH_ENTRY_WIDTH_THIRD;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE:
        *entry_width = BCMPTM_RM_HASH_ENTRY_WIDTH_SINGLE;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the hash LT entry width.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] entry_width Entry width for the given LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_entry_width_get(int unit,
                           rm_hash_lt_ctrl_t *lt_ctrl,
                           bcmptm_rm_hash_entry_width_t *entry_width)
{
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_map_info_t *map_info = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    uint8_t grp_idx, fmt_idx, num_views, entry_size, rbank;
    rm_hash_fmt_info_t *fmt_info = NULL;
    rm_hash_bm_t e_bm;
    rm_hash_entry_narrow_mode_t e_nm;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    if (lt_ctrl->num_maps == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    map_info = &lt_ctrl->map_info[0];
    grp_idx = map_info->grp_idx[0];
    fmt_idx = map_info->fmt_idx[0];
    fmt_info = &lt_ctrl->fmt_info[fmt_idx];
    rbank = fmt_info->en_rbank_list[0];

    bcmptm_rm_hash_key_format_num_view_get(unit,
                                           fmt_info->key_format,
                                           &num_views);
    if (num_views > 1) {
        /* Not support for the case where an LT has several entry widths. */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_base_view_info_get(unit,
                                           &lt_ctrl->lt_info,
                                           grp_idx,
                                           &view_info));
    entry_size = view_info->num_base_entries;
    switch (entry_size) {
    case 4:
        *entry_width = BCMPTM_RM_HASH_ENTRY_WIDTH_QUAD;
        break;
    case 2:
        *entry_width = BCMPTM_RM_HASH_ENTRY_WIDTH_DOUBLE;
        break;
    case 1:
        /* Still needs to figure out if the entry is in narow mode. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                             fmt_info->key_format,
                                             pt_info,
                                             rbank,
                                             &e_bm,
                                             &e_nm));
        SHR_IF_ERR_EXIT
            (rm_hash_narrow_entry_width_get(unit, e_nm, entry_width));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_hit_tbl_info_get(int unit,
                                bcmdrd_sid_t sid,
                                const bcmptm_rm_hash_hit_tbl_info_t **hit_tbl_info)
{
    const bcmptm_rm_hash_table_info_t *tbl_info = NULL;
    uint8_t num_tbl_info = 0, idx;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t grp_sid;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_hash_table_info_get(unit, &tbl_info, &num_tbl_info);
    if ((rv == SHR_E_UNAVAIL) || (num_tbl_info == 0)) {
        *hit_tbl_info = NULL;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_grp_sid_get(unit, sid, &grp_sid));
    for (idx = 0; idx < num_tbl_info; idx++, tbl_info++) {
        if (tbl_info->sid == grp_sid) {
            break;
        }
    }
    if (idx == num_tbl_info) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *hit_tbl_info = tbl_info->hit_tbl_info;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_hit_index_info_get(int unit,
                                      uint32_t lookup,
                                      uint32_t *hit_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hit_index, SHR_E_PARAM);

    if (lookup >= RM_HASH_LOOKUP_MAX_CNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (hit_index_info[unit].num == 1) {
        *hit_index = hit_index_info[unit].hit_index_list[0];
    } else {
        *hit_index = hit_index_info[unit].hit_index_list[lookup];
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_flex_ctr_offset_get(int unit,
                                   rm_hash_lt_ctrl_t *lt_ctrl,
                                   bcmdrd_sid_t sid,
                                   int tbl_inst,
                                   uint32_t ent_index,
                                   uint8_t ent_size,
                                   uint32_t *ctr_offset)
{
    uint8_t divisor;
    rm_hash_pt_info_t *pt_info = NULL;
    bcmptm_rm_hash_hit_context_t tmp_hit_context, *hit_context = NULL;
    bool context_exist = FALSE;
    rm_hash_lt_flex_ctr_info_t *fc_info = NULL;
    uint8_t rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    uint8_t rbank;
    bcmdrd_sid_t grp_sid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    /* Get the widest table view. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_grp_sid_get(unit, sid, &grp_sid));
    bcmptm_rm_hash_sid_rbank_list_get(unit,
                                      grp_sid,
                                      pt_info,
                                      rbank_list,
                                      &rbank_cnt);
    if (rbank_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    rbank = rbank_list[0];

    divisor = pt_info->base_bkt_size[rbank]/ent_size;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_hit_context_get(unit,
                                                lt_ctrl,
                                                grp_sid,
                                                &tmp_hit_context,
                                                &context_exist));
    if (context_exist == TRUE) {
        hit_context = &tmp_hit_context;
    }
    fc_info = &lt_ctrl->flex_ctr_info;
    if (fc_info->entry_index_cb == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (fc_info->entry_index_cb(unit,
                                 lt_ctrl->ltid,
                                 sid,
                                 hit_context,
                                 tbl_inst,
                                 ent_index / divisor,
                                 (ent_index % divisor) * ent_size,
                                 fc_info->context,
                                 ctr_offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_hit_index_info_set(int unit,
                                      rm_hash_lt_ctrl_t *lt_ctrl,
                                      int tbl_inst,
                                      rm_hash_map_srch_result_t *map_sr)
{
    uint8_t e_inst;
    uint32_t ctr_offset;
    rm_hash_inst_srch_result_t *inst_sr = NULL;
    uint32_t *hit_index = hit_index_info[unit].hit_index_list;

    SHR_FUNC_ENTER(unit);

    for (e_inst = 0; e_inst < map_sr->num_insts; e_inst++) {
        inst_sr = &map_sr->inst_srch_result[e_inst];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                lt_ctrl,
                                                inst_sr->sid,
                                                tbl_inst,
                                                inst_sr->e_idx,
                                                inst_sr->view_info->num_base_entries,
                                                &ctr_offset));
        hit_index[e_inst] = ctr_offset;
    }

    hit_index_info[unit].num = map_sr->num_insts;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_hit_index_callback_register(int unit,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_hash_entry_index_cb_t index_cb,
                                           bool report_for_insert,
                                           uint8_t num_lookups,
                                           bool *lookup_reporting_enable,
                                           void *context,
                                           bcmptm_rm_hash_entry_width_t *entry_width)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_lt_flex_ctr_info_t *fc_info = NULL;
    uint8_t idx;
    bool report_enable = FALSE;

    SHR_FUNC_ENTER(unit);

    /*
     * It is assumed that the function will be called in the same thread context,
     * i.e. modeling context.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    fc_info = &lt_ctrl->flex_ctr_info;
    fc_info->entry_index_cb = index_cb;
    fc_info->report_for_insert = report_for_insert;

    if (num_lookups > RM_HASH_LOOKUP_MAX_CNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < num_lookups; idx++) {
        fc_info->lookup_entry_move_reporting[idx] = lookup_reporting_enable[idx];
        report_enable = report_enable || lookup_reporting_enable[idx];
    }

    fc_info->num_lookups = num_lookups;
    fc_info->context = context;
    if (entry_width != NULL) {
        SHR_IF_ERR_EXIT
            (rm_hash_lt_entry_width_get(unit, lt_ctrl, entry_width));
    }

    if (fc_info->num_context == 0) {
        /* There is no hit context to configure. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_hit_context_configure(unit,
                                              report_enable,
                                              lt_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_move_event_notify(int unit,
                                 bcmltd_sid_t ltid,
                                 int tbl_inst,
                                 uint8_t num_lookups,
                                 void *context,
                                 uint32_t *dst_ctr_index,
                                 uint32_t *src_ctr_index)
{
    bcmevm_extend_ev_data_t event;
    uint64_t data[3 + 2 * RM_HASH_LOOKUP_MAX_CNT];
    uint8_t idx, src_start_idx;

    SHR_FUNC_ENTER(unit);
    /*
     * The entry move event data format is as follows:
     * LTID | PIPE BITMAP | CONTEXT | DST INDEX | SRC INDEX
     * The number of DST INDEX and SRC INDEX elements depends on
     * the number of lookups.
     */

    event.count = 3 + 2 * num_lookups;
    event.data = data;
    sal_memset(data, 0, sizeof(data));

    data[0] = ltid;

    if (tbl_inst == -1) {
        /* In global pipe operating mode, the bitmap will be all 1s. */
        data[1] = -1;
    } else if ((tbl_inst >= 0) && (tbl_inst < RM_HASH_MAX_TBL_INST_COUNT)) {
        data[1] = 0x1 << tbl_inst;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    data[2] = (uintptr_t)context;

    for (idx = 0; idx < num_lookups; idx++) {
        data[3 + idx] = dst_ctr_index[idx];
    }

    src_start_idx = num_lookups + 3;
    for (idx = 0; idx < num_lookups; idx++) {
        data[src_start_idx + idx] = src_ctr_index[idx];
    }

    bcmevm_publish_event_notify(unit, BCMPTM_LT_ENTRY_MOVE,
                                (uintptr_t)&event);

exit:
    SHR_FUNC_EXIT();
}
