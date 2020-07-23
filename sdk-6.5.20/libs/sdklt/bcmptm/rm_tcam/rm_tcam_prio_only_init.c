/*! \file rm_tcam_prio_only_init.c
 *
 * Functions required during all BCMPTM phases(Phase1, 2, and 3)
 * in module initialization for prio only TCAM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private Functions
 */

STATIC int
bcmptm_rm_tcam_prio_only_shr_info_init(int unit,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       uint8_t *start_ptr,
                                       uint32_t offset,
                                       uint16_t *shr_info_count,
                                       bool *expansion_disabled)
{
    uint8_t idx = 0;
    uint8_t depth = 0;
    uint8_t shr_depth = 0;
    bcmdrd_sid_t sid;
    bcmdrd_sid_t shr_sid;
    bcmltd_sid_t shr_ltid;
    bcmltd_sid_t cand_ltid = -1;
    uint16_t start_index = 0;
    uint16_t end_index = 0;
    uint16_t min_index = 0;
    uint16_t max_index = 0;
    uint16_t shr_start_index = 0;
    uint16_t shr_end_index = 0;
    bool shared = FALSE;
    uint16_t key_row = 0;
    uint16_t shr_key_row = 0;
    uint16_t num_depth_inst = 0;
    uint16_t shr_num_depth_inst = 0;
    uint16_t num_key_rows = 0;
    uint16_t shr_num_key_rows = 0;
    uint32_t shr_depth_bmp[4];
    const bcmltd_sid_t **shr_sid_array = NULL;
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bcmptm_rm_tcam_prio_only_shr_info_t *shr_info;
    bool sid_differ = FALSE;
    bool depth_differ = FALSE;
    int32_t num_lkup = 1;
    int32_t shr_num_lkup = 1;
    uint32_t slice_col = 0;
    uint32_t shr_slice_col = 0;
    uint32_t slice_size;
    uint32_t shr_slice_size;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(start_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(shr_info_count, SHR_E_PARAM);

    shr_info = (bcmptm_rm_tcam_prio_only_shr_info_t *)(start_ptr + offset);
    *shr_info_count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_shared_lt_depth_check(unit, ltid,ltid_info,&depth_differ));
    if ((ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) ||
         (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT)) {
        *expansion_disabled = TRUE;
    }
    for (idx = 0; idx < ltid_info->rm_more_info->shared_lt_count; idx++) {
        shr_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
        sal_memset(&shr_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shr_ltid_info),
                                          NULL,
                                          NULL));
        if ((shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) ||
             (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT)) {
            *expansion_disabled = TRUE;
        }
        start_index = 0;
        end_index = 0;
        shr_start_index = 0;
        shr_end_index = 0;
        num_lkup = 1;
        slice_col = 0;
        shr_slice_col = 0;
        shr_num_lkup = 1;
        sid_differ = FALSE;

        if (ltid_info->hw_entry_info[0].view_type) {
            if (shr_ltid_info.hw_entry_info[0].view_type == 1) {
                cand_ltid = shr_ltid;
            }
            if (cand_ltid != (bcmltd_sid_t)-1) {
                num_depth_inst = shr_ltid_info.hw_entry_info[0].num_depth_inst;
                for (depth = 0; depth < num_depth_inst; depth++) {
                    shr_sid = shr_ltid_info.hw_entry_info[0].sid[depth][0];
                    min_index = bcmptm_pt_index_min(unit, shr_sid);
                    max_index = bcmptm_pt_index_max(unit, shr_sid);
                    shr_slice_size = max_index - min_index + 1;
                    if (depth == 0) {
                        start_index = min_index;
                        end_index = max_index;
                    } else {
                        end_index += shr_slice_size;
                    }
                }
                for (idx = 0; idx <
                    ltid_info->rm_more_info->shared_lt_count; idx++) {
                    shr_ltid =
                       ltid_info->rm_more_info->shared_lt_info[idx];
                    if (shr_ltid == ltid) {
                        continue;
                    }
                    shr_info->shr_ltid = shr_ltid;
                    shr_info->start_index = start_index;
                    shr_info->end_index = end_index;
                    shr_info->shr_start_index = start_index;
                    shr_info->shr_end_index = end_index;
                    shr_info++;
                    (*shr_info_count)++;
                }
                SHR_EXIT();
            }
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ltid_tcam_info_shared_check(unit,
                                                        ltid_info,
                                                        &shr_ltid_info,
                                                        &shared,
                                                        &sid_differ));
        if (shared == TRUE) {
            if (sid_differ == TRUE && depth_differ == TRUE) {
                shared = FALSE;
            }
            else {
                continue;
            }
        }
        num_depth_inst = ltid_info->hw_entry_info[0].num_depth_inst;
        shr_num_depth_inst = shr_ltid_info.hw_entry_info[0].num_depth_inst;

        if (ltid_info->hw_entry_info[0].view_type)  {
            num_key_rows = ltid_info->hw_entry_info[0].num_key_rows
                / ltid_info->hw_entry_info[0].view_type;
        } else  {
            num_key_rows = ltid_info->hw_entry_info[0].num_key_rows;
        }
        if (shr_ltid_info.hw_entry_info[0].view_type)  {
            shr_num_key_rows = shr_ltid_info.hw_entry_info[0].num_key_rows
                / shr_ltid_info.hw_entry_info[0].view_type;
        } else  {
            shr_num_key_rows = shr_ltid_info.hw_entry_info[0].num_key_rows;
        }
        if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            num_key_rows /= 2;
            num_lkup = 2;
        }
        if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            shr_num_key_rows /= 2;
        }

        do {
            for (depth = 0; depth < num_depth_inst; depth++) {
                sid = ltid_info->hw_entry_info[0].sid[depth][slice_col];
                min_index = bcmptm_pt_index_min(unit, sid);
                max_index = bcmptm_pt_index_max(unit, sid);
                slice_size = (max_index - min_index + 1);
                if ((depth == 0) && (slice_col == 0)) {
                    start_index = min_index;
                    end_index = max_index;
                } else {
                    start_index += slice_size;
                    end_index += slice_size;
                }
                if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                    end_index += slice_size;
                }
                sal_memset(shr_depth_bmp, 0, sizeof(uint32_t)* 4);
                for (key_row = 0; key_row < num_key_rows; key_row++) {
                    sid = ltid_info->hw_entry_info[0].sid[depth][key_row + slice_col];
                    shr_slice_col = 0;
                    shr_num_lkup = 1;
                    if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
                        shr_num_lkup = 2;
                    }
                    do {
                        for (shr_depth = 0;
                            shr_depth < shr_num_depth_inst;
                            shr_depth++) {
                            shr_sid = shr_ltid_info.hw_entry_info[0].sid[shr_depth][shr_slice_col];
                            min_index = bcmptm_pt_index_min(unit, shr_sid);
                            max_index = bcmptm_pt_index_max(unit, shr_sid);
                            shr_slice_size = (max_index - min_index + 1);
                            if ((shr_depth == 0) && (shr_slice_col == 0)) {
                                shr_start_index = min_index;
                                shr_end_index = max_index;
                            } else {
                                shr_start_index += shr_slice_size;
                                shr_end_index += shr_slice_size;
                            }
                            if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                                shr_end_index += shr_slice_size;
                            }
                            for (shr_key_row = 0;
                                shr_key_row < shr_num_key_rows;
                                shr_key_row++) {
                                shr_sid_array = shr_ltid_info.hw_entry_info[0].sid;
                                shr_sid =
                                  shr_sid_array[shr_depth][shr_key_row + shr_slice_col];
                                if (sid == shr_sid) {
                                    if (SHR_BITGET(shr_depth_bmp, shr_depth * shr_num_lkup)) {
                                        break;
                                    }
                                    SHR_BITSET(shr_depth_bmp, shr_depth * shr_num_lkup);
                                    shr_info->shr_ltid = shr_ltid;
                                    shr_info->start_index = start_index;
                                    shr_info->end_index = end_index;
                                    shr_info->shr_start_index = shr_start_index;
                                    shr_info->shr_end_index = shr_end_index;
                                    shr_info->shr_flags = 0;
                                    if (key_row == 0) {
                                        shr_info->shr_flags |= BCMPTM_RM_TCAM_KEY0;
                                    }
                                    if (shr_key_row == 0) {
                                        shr_info->shr_flags |= BCMPTM_RM_TCAM_SHR_KEY0;
                                    }
                                    (*shr_info_count)++;
                                    shr_info++;
                                    break;
                                }
                            }
                            if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                                shr_start_index += shr_slice_size;
                                shr_depth++;
                            }
                        }
                        shr_num_lkup--;
                        shr_slice_col += shr_num_key_rows;
                    }while (shr_num_lkup > 0);
                }
                if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                    start_index += slice_size;
                    depth++;
                }
            }
            num_lkup--;
            slice_col += num_key_rows;
        }while (num_lkup > 0);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_prio_only_shared_lt_mem_required(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t *size)
{
    uint8_t idx = 0;
    bcmdrd_sid_t sid;
    uint8_t shr_depth = 0;
    uint16_t shr_info_count = 0;
    bool shared = FALSE;
    bcmltd_sid_t shr_ltid;
    bcmdrd_sid_t shr_sid;
    const bcmdrd_sid_t **shr_sid_array = NULL;
    uint16_t depth = 0;
    uint16_t key_row = 0;
    uint16_t shr_key_row = 0;
    uint16_t num_depth_inst = 0;
    uint16_t shr_num_depth_inst = 0;
    uint16_t num_key_rows = 0;
    uint16_t shr_num_key_rows = 0;
    uint32_t shr_depth_bmp[4];
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bool sid_differ = FALSE;
    bool depth_differ = FALSE;
    int32_t num_lkup = 1;
    int32_t shr_num_lkup = 1;
    uint32_t slice_col = 0;
    uint32_t shr_slice_col = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_shared_lt_depth_check(unit, ltid,ltid_info,&depth_differ));

    for (idx = 0; idx < ltid_info->rm_more_info->shared_lt_count; idx++) {
        shr_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
        if (shr_ltid == ltid) {
            continue;
        }
        if (ltid_info->hw_entry_info[0].view_type) {
            shr_info_count++;
            continue;
        }
        slice_col = 0;
        shr_slice_col = 0;
        num_lkup = 1;
        shr_num_lkup = 1;
        sid_differ = FALSE;
        sal_memset(&shr_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shr_ltid_info),
                                          NULL,
                                          NULL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ltid_tcam_info_shared_check(unit,
                                                        ltid_info,
                                                        &shr_ltid_info,
                                                        &shared,
                                                        &sid_differ));
        if (shared == TRUE) {
            if (sid_differ == TRUE && depth_differ == TRUE) {
                shared = FALSE;
            }
            else {
                continue;
            }
        }
        num_depth_inst = ltid_info->hw_entry_info[0].num_depth_inst;
        shr_num_depth_inst = shr_ltid_info.hw_entry_info[0].num_depth_inst;
        if (ltid_info->hw_entry_info[0].view_type)  {
            num_key_rows = ltid_info->hw_entry_info[0].num_key_rows
                / ltid_info->hw_entry_info[0].view_type;
        } else  {
            num_key_rows = ltid_info->hw_entry_info[0].num_key_rows;
        }
        if (shr_ltid_info.hw_entry_info[0].view_type)  {
            shr_num_key_rows = shr_ltid_info.hw_entry_info[0].num_key_rows
                / shr_ltid_info.hw_entry_info[0].view_type;
        } else  {
            shr_num_key_rows = shr_ltid_info.hw_entry_info[0].num_key_rows;
        }

        if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            num_key_rows /= 2;
            num_lkup = 2;
        }
        if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            shr_num_key_rows /= 2;
        }

        do {
            for (depth = 0; depth < num_depth_inst; depth++) {
                sal_memset(shr_depth_bmp, 0, sizeof(uint32_t)* 4);
                if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                    depth++;
                }
                for (key_row = 0; key_row < num_key_rows; key_row++) {
                    sid = ltid_info->hw_entry_info[0].sid[depth][slice_col + key_row];
                    shr_slice_col = 0;
                    shr_num_lkup = 1;
                    if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
                        shr_num_lkup = 2;
                    }
                    do {
                        for (shr_depth = 0;
                            shr_depth < shr_num_depth_inst;
                            shr_depth++) {
                            if (shr_ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                                shr_depth++;
                            }
                            for (shr_key_row = 0;
                                shr_key_row < shr_num_key_rows;
                                shr_key_row++) {
                                shr_sid_array = shr_ltid_info.hw_entry_info[0].sid;
                                shr_sid =
                                  shr_sid_array[shr_depth][shr_slice_col + shr_key_row];
                                if (sid == shr_sid) {
                                    if (!SHR_BITGET(shr_depth_bmp, shr_depth * shr_num_lkup)) {
                                        SHR_BITSET(shr_depth_bmp, shr_depth * shr_num_lkup);
                                        shr_info_count++;
                                        break;
                                    }
                                }
                            }
                        }
                        shr_num_lkup--;
                        shr_slice_col += shr_num_key_rows;
                    } while (shr_num_lkup > 0);
                }
            }
            num_lkup--;
            slice_col += num_key_rows;
        }while(num_lkup > 0);
    }
    (*size) += (sizeof(bcmptm_rm_tcam_prio_only_shr_info_t) * shr_info_count);
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
/*
 * Initialize the Max key rows needed for an entry in TCAM in
 * bcmptm_rm_tcam_prio_only_info_t format.
 */

/* Calculate memory size required for prio only type TCAMs */
int
bcmptm_rm_tcam_prio_only_calc_mem_required(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *user_data)
{
    uint32_t size = 0;
    uint32_t trans_req_size = 0;
    uint32_t num_entries = 0;
    uint32_t entry_hash_size = 0;
    uint32_t *total_size = user_data;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check. */
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    /* This should always be the first structure
     * memory allocation for every LTID
     */
    /* memory required to save Transaction related info */
    trans_req_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    size += sizeof(bcmptm_rm_tcam_prio_only_info_t);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));

    if (ltid_info->hw_entry_info->view_type) {
        num_entries *= ltid_info->hw_entry_info->view_type;
    }

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_entries = num_entries * 2;
    }
    /* Memory required for TCAM entry information consists of
     * (priority and offset)
     */
    size += (num_entries * sizeof(bcmptm_rm_tcam_prio_only_entry_info_t));

    if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        entry_hash_size = (num_entries / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
    } else {
        entry_hash_size = num_entries;
    }

    /* Memory space required for TCAM hash table. */
    size += (sizeof(int) * entry_hash_size);

    /* Memory required for shared LTs. */
    if (ltid_info->rm_more_info->shared_lt_count) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_shared_lt_mem_required(unit,
                                                             ltid,
                                                             ltid_info,
                                                             &size));
    }

    (*total_size) += trans_req_size + size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_metadata_cb_process(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint8_t *start_ptr,
                              uint8_t sub_component_id)
{
    int  *entry_hash = NULL;
    uint32_t num_entries = 0;
    uint32_t idx;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint32_t offset = 0;
    uint16_t shr_info_count = 0;
    bool shared_reorder_disabled = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);


    /* Report trans_info structure offset */
    offset = 0;
    bcmissu_struct_info_report(unit,
                               BCMMGMT_RM_TCAM_COMP_ID,
                               sub_component_id,
                               offset,
                               sizeof(bcmptm_rm_tcam_trans_info_t), 1,
                               BCMPTM_RM_TCAM_TRANS_INFO_T_ID);
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + offset);

    trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;

    /* Report prio_only_info structure offset */
    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_RM_TCAM_COMP_ID,
                               sub_component_id,
                               offset,
                               sizeof(bcmptm_rm_tcam_prio_only_info_t), 1,
                               BCMPTM_RM_TCAM_PRIO_ONLY_INFO_T_ID);
    tcam_info = (bcmptm_rm_tcam_prio_only_info_t *)(start_ptr + offset);
    tcam_info->ltid = ltid;
    /* Set tcam info flags according to LT_INFO flags */
    tcam_info->flags = 0;
    tcam_info->num_lt_sharing = 0;
    tcam_info->res_ent_key_type_val = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;

    if (ltid_info->non_aggr == 0) {
        tcam_info->flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
    }
    if (ltid_info->non_aggr == 1) {
        tcam_info->flags |= BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        tcam_info->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));

    if (ltid_info->hw_entry_info->view_type) {
        num_entries *= ltid_info->hw_entry_info->view_type;
    }

    tcam_info->num_entries_per_index = 1;
    tcam_info->num_lookups = 1;

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
        tcam_info->num_entries_per_index = 2;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_hw_key_info_init(unit,
                                               ltid,
                                               ltid_info,
                                               tcam_info->num_entries_per_index,
                                               tcam_info->hw_field_list));
    }

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_entries = num_entries * 2;
        tcam_info->num_lookups = 2;
    }

    /* Set the entry counts */
    tcam_info->num_entries = num_entries;
    tcam_info->num_entries_ltid = 0;
    /* Set the entry info and hash details */
    if (tcam_info->num_entries >
        BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        tcam_info->entry_hash_size =
            (num_entries / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
    } else {
        tcam_info->entry_hash_size = num_entries;
    }

    /* Report entry_info structure offset */
    tcam_info->entry_info_offset =
               sizeof(bcmptm_rm_tcam_prio_only_info_t);
    offset += sizeof(bcmptm_rm_tcam_prio_only_info_t);
    bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_prio_only_entry_info_t),
                              tcam_info->num_entries,
                              BCMPTM_RM_TCAM_PRIO_ONLY_ENTRY_INFO_T_ID);
    entry_info = (bcmptm_rm_tcam_prio_only_entry_info_t *)(start_ptr + offset);
    for (idx = 0; idx < tcam_info->num_entries; idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }
    tcam_info->entry_info_seg_size = (tcam_info->num_entries *
               sizeof(bcmptm_rm_tcam_prio_only_entry_info_t));
    tcam_info->entry_hash_offset =
               (tcam_info->entry_info_offset +
               (tcam_info->entry_info_seg_size));
    offset += (tcam_info->num_entries *
               sizeof(bcmptm_rm_tcam_prio_only_entry_info_t));
    entry_hash = (int *)(start_ptr + offset);
    for (idx = 0; idx < tcam_info->entry_hash_size; idx++) {
        entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }


    /* Report shr_info structure offset */
    tcam_info->entry_hash_seg_size= (tcam_info->entry_hash_size * sizeof(int));
    offset += (tcam_info->entry_hash_seg_size);
    tcam_info->shr_info_offset =
               tcam_info->entry_hash_offset +
               (tcam_info->entry_hash_seg_size);
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmptm_rm_tcam_prio_only_shr_info_init(unit,
                                               ltid,
                                               ltid_info,
                                               start_ptr,
                                               offset,
                                               &shr_info_count,
                                               &shared_reorder_disabled));
    if (shared_reorder_disabled == TRUE) {
        tcam_info->flags |= BCMPTM_RM_TCAM_SHARED_REORDER_DISABLED;
    }
    bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_prio_only_shr_info_t),
                              shr_info_count,
                              BCMPTM_RM_TCAM_PRIO_ONLY_SHR_INFO_T_ID);
    tcam_info->shr_info_count = shr_info_count;
    tcam_info->reserved_entry_index1 = BCMPTM_RM_TCAM_INDEX_INVALID;
    tcam_info->reserved_entry_index2 = BCMPTM_RM_TCAM_INDEX_INVALID;

    trans_info->state_size =
          (sizeof(bcmptm_rm_tcam_trans_info_t) +
          (sizeof(bcmptm_rm_tcam_prio_only_info_t)) +
          (sizeof(int) * tcam_info->entry_hash_size) +
          (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) * num_entries) +
          (sizeof(bcmptm_rm_tcam_prio_only_shr_info_t) * shr_info_count));
exit:
    SHR_FUNC_EXIT();
}

STATIC int
prio_only_init_metadata_cb(int unit,
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

    for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
        sub_comp_id = lt_md->active_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                           ltid,
                                           ltid_info,
                                           start_ptr,
                                           sub_comp_id));
        sub_comp_id = lt_md->backup_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                           ltid,
                                           ltid_info,
                                           start_ptr,
                                           sub_comp_id));
    }

exit:
     SHR_FUNC_EXIT();
}

STATIC int
prio_only_metadata_wb_process(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint8_t *start_ptr,
                              uint8_t sub_component_id)
{
    uint32_t num_entries = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint32_t offset = 0;
    uint32_t issu_offset = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_entries = num_entries * 2;
    }

    if (ltid_info->hw_entry_info->view_type) {
        num_entries *= ltid_info->hw_entry_info->view_type;
    }

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
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + offset);
    if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    /* Get prio_only_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    tcam_info = (bcmptm_rm_tcam_prio_only_info_t *)(start_ptr + offset);
    /*
     * Ensure that ltid and num_entries is not changed
     * during warmboot.
     */
    if (tcam_info->ltid != ltid ||
        tcam_info->num_entries != num_entries) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        if (tcam_info->entry_hash_size !=
           (num_entries / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (tcam_info->entry_hash_size != num_entries) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset += sizeof(bcmptm_rm_tcam_prio_only_info_t);
    /* Get prio_only_entry_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_ENTRY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    tcam_info->entry_info_offset =
               sizeof(bcmptm_rm_tcam_prio_only_info_t);

    offset += (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) *
               tcam_info->num_entries);
    tcam_info->entry_hash_offset =
                   (tcam_info->entry_info_offset +
                   (tcam_info->num_entries *
                   sizeof(bcmptm_rm_tcam_prio_only_entry_info_t)));

    offset += (sizeof(int) * tcam_info->entry_hash_size);
    /* Get prio_only_shr_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_SHR_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    tcam_info->shr_info_offset =
               tcam_info->entry_hash_offset +
               (tcam_info->entry_hash_size * sizeof(int));

    trans_info->state_size =
          (sizeof(bcmptm_rm_tcam_trans_info_t) +
          (sizeof(bcmptm_rm_tcam_prio_only_info_t)) +
          (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) * num_entries) +
          (sizeof(int) * tcam_info->entry_hash_size) +
          (sizeof(bcmptm_rm_tcam_prio_only_shr_info_t) *
           tcam_info->shr_info_count));

exit:
    SHR_FUNC_EXIT();
}

STATIC int
prio_only_init_metadata_wb(int unit,
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

    for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
        sub_comp_id = lt_md->active_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_only_metadata_wb_process(unit,
                                           ltid,
                                           ltid_info,
                                           start_ptr,
                                           sub_comp_id));
        sub_comp_id = lt_md->backup_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_only_metadata_wb_process(unit,
                                           ltid,
                                           ltid_info,
                                           start_ptr,
                                           sub_comp_id));
    }

exit:
     SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_init_metadata(int unit,
                                       bool warm,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       void *user_data)
{

    SHR_FUNC_ENTER(unit);

    if (warm == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_only_init_metadata_cb(unit, ltid, ltid_info));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_only_init_metadata_wb(unit, ltid, ltid_info));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_copy_metadata(int unit,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *old_ptr,
                                void *new_ptr)
{
    uint32_t offset = 0;
    uint8_t *start_ptr_old = (uint8_t*)old_ptr;
    uint8_t *start_ptr_new = (uint8_t*)new_ptr;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info_old = NULL;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info_new = NULL;
    void *to_ptr = NULL;
    void *from_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(old_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(new_ptr, SHR_E_PARAM);

    offset = 0;
    tcam_info_old = (bcmptm_rm_tcam_prio_only_info_t*)
        (start_ptr_old + offset + sizeof(bcmptm_rm_tcam_trans_info_t));
    tcam_info_new = (bcmptm_rm_tcam_prio_only_info_t*)
        (start_ptr_new + offset + sizeof(bcmptm_rm_tcam_trans_info_t));

    if (tcam_info_old->entry_info_seg_size == 0) {
        SHR_EXIT();
    }

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        /* Copy first half entries in to entry info data for Dst lookup. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_info_offset);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_info_offset);
        sal_memcpy(to_ptr,
                   from_ptr,
                   (tcam_info_old->entry_info_seg_size) / 2);

        /* Copy second half entries in to entry info data for src lookup. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_info_offset +
                          tcam_info_new->entry_info_seg_size / 2);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_info_offset +
                            tcam_info_old->entry_info_seg_size / 2);
        sal_memcpy(to_ptr,
                   from_ptr,
                   (tcam_info_old->entry_info_seg_size) / 2);

        /* Copy entry hash data for Dst lookup. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_hash_offset);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_hash_offset);
        sal_memcpy(to_ptr,
                   from_ptr,
                   tcam_info_old->entry_hash_seg_size / 2);

        /* Copy entry hash data for src lookup.. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_hash_offset +
                          tcam_info_new->entry_hash_seg_size / 2);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_hash_offset +
                            tcam_info_old->entry_hash_seg_size / 2);
        sal_memcpy(to_ptr,
                   from_ptr,
                   tcam_info_old->entry_hash_seg_size / 2);
    } else {
        /* Copy entry info data. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_info_offset);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_info_offset);
        sal_memcpy(to_ptr,
                   from_ptr,
                   tcam_info_old->entry_info_seg_size);

        /* Copy entry hash data. */
        to_ptr = (void *)((uint8_t *)tcam_info_new +
                          tcam_info_new->entry_hash_offset);
        from_ptr = (void *)((uint8_t *)tcam_info_old +
                            tcam_info_old->entry_hash_offset);
        sal_memcpy(to_ptr,
                   from_ptr,
                   tcam_info_old->entry_hash_seg_size);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_iomd_init(int unit,
                                  uint32_t req_flags,
                                  uint32_t cseq_id,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  bcmptm_op_type_t req_op,
                                  bcmptm_rm_tcam_req_t *req_info,
                                  bcmptm_rm_tcam_rsp_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    iomd->trans_id = cseq_id;
    iomd->req_flags = req_flags;
    iomd->rsp_flags = rsp_flags;
    iomd->ltid = ltid;
    iomd->ltid_info = ltid_info;
    iomd->pt_dyn_info = pt_dyn_info;
    iomd->req_op = req_op;
    iomd->req_info = req_info;
    iomd->rsp_info = rsp_info;
    iomd->rsp_ltid = rsp_ltid;
    iomd->entry_attrs = req_info->entry_attrs;
    iomd->pipe_id = pt_dyn_info->tbl_inst;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_iomd_update(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attrs = iomd->entry_attrs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                iomd->ltid,
                                                iomd->ltid_info,
                                                iomd->pipe_id,
                                                &tcam_info));
    iomd->tcam_info = (void *)tcam_info;

    if (tcam_info->num_entries == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id,
                                                 &entry_info));
    iomd->entry_info_arr.entry_info[0][0] = (void *)entry_info;
    iomd->entry_info_arr.num_depth_parts = 1;
    iomd->entry_info_arr.num_width_parts = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_get(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id,
                                                 entry_attrs->lookup_id,
                                                 &iomd->entry_hash_ptr));
    if (tcam_info->num_lookups == 2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_hash_get(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id,
                                                 2,
                                                 &iomd->entry_hash_ptr_2));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit,
                                                   iomd->ltid,
                                                   iomd->ltid_info,
                                                   iomd->req_info->ekw,
                                                   NULL,
                                                   &iomd->entry_hash_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_num_index_required_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               &iomd->num_index_required));
exit:
    SHR_FUNC_EXIT();
}

