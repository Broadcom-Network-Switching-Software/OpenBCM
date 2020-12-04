/*! \file bcmptm_rm_hash_table_info.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement the basic operations of hash
 * resource manager. More specifically, it includes the routines for
 * GET_TABLE_INFO opcode.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_table_info.h"
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */
static int
rm_hash_ltid_eligible_entry_cnt_get(int unit,
                                    bcmltd_sid_t ltid,
                                    uint32_t *eligible_table_size)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_map_info_t *map_info = NULL;
    rm_hash_fmt_info_t * nfmt_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    const bcmptm_rm_hash_view_info_t *view_info_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {NULL};
    uint32_t ent_cnt = 0, base_bkt_cnt = 0;
    uint8_t grp_idx, map_idx, inst, view_idx, nvbe_in_map, fmt_nbe = 0;
    uint8_t view_cnt = 0, fmt_idx, base_bkt_size = 0;
    uint8_t num_insts = 1, bank_idx, rbank, num_en_rbanks, *p_en_rbank = NULL;
    bcmdrd_sid_t sid;
    bool num_retrieved = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, ltid, &lt_ctrl));

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_info = &lt_ctrl->map_info[map_idx];
        /* Use the 1st format as the default. */
        fmt_idx = map_info->fmt_idx[0];
        nfmt_info = &lt_ctrl->fmt_info[fmt_idx];
        nvbe_in_map = RM_HASH_MAX_ENT_SIZE;

        for (inst = 0; inst < map_info->num_insts; inst++) {
            fmt_idx = map_info->fmt_idx[inst];
            grp_idx = map_info->grp_idx[inst];
            /* Find the fmt which has the narrowest view in the group. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_base_view_info_get(unit,
                                                   &lt_ctrl->lt_info,
                                                   grp_idx,
                                                   &base_view_info));
            if ((lt_ctrl->is_per_pipe == TRUE) && (num_retrieved == FALSE)) {
                sid = base_view_info->sid[0];
                num_insts = bcmdrd_pt_num_tbl_inst(unit, sid);
                num_retrieved = TRUE;
            }
            key_format = lt_ctrl->fmt_info[fmt_idx].key_format;
            bcmptm_rm_hash_key_format_view_list_get(unit,
                                                    base_view_info,
                                                    key_format,
                                                    view_info_list,
                                                    &view_cnt);
            /* Find the minimum entry size for this key format. */
            fmt_nbe = view_info_list[0]->num_base_entries;
            for (view_idx = 0; view_idx < view_cnt; view_idx++) {
                if (view_info_list[view_idx]->num_base_entries < fmt_nbe) {
                    fmt_nbe = view_info_list[view_idx]->num_base_entries;
                }
            }

            /* The narrowest view for this key format has been determined. */
            if (fmt_nbe < nvbe_in_map) {
                nvbe_in_map = fmt_nbe;
                nfmt_info = &lt_ctrl->fmt_info[fmt_idx];
            }
        }
        /*
         * The narrowest view info has been found. Find which banks are enabled
         * for this view.
         */
        num_en_rbanks = nfmt_info->num_en_rbanks;
        p_en_rbank = nfmt_info->en_rbank_list;

        for (bank_idx = 0; bank_idx < num_en_rbanks; bank_idx++) {
            rbank = p_en_rbank[bank_idx];
            base_bkt_size = nfmt_info->pt_info->base_bkt_size[rbank];
            base_bkt_cnt  = nfmt_info->pt_info->base_bkt_cnt[rbank];
            ent_cnt = ent_cnt + (base_bkt_size * base_bkt_cnt) / nvbe_in_map;
        }
    }
    /* The total entry number for all the valid banks have been computed. */
    *eligible_table_size = ent_cnt * num_insts;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int bcmptm_rm_hash_table_info_get(int unit,
                                  bcmltd_sid_t req_ltid,
                                  uint32_t *table_size)
{
    uint32_t e_table_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_ltid_eligible_entry_cnt_get(unit,
                                             req_ltid,
                                             &e_table_size));
    *table_size = e_table_size;

exit:
    SHR_FUNC_EXIT();
}
