/*! \file bcmfp_compression_common.c
 *
 * APIs common to all FP compression related activities.
 *
 * This file contains callback function called when there
 * is a change in CID/CID_MASK of existing keys in a trie.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmfp/bcmfp_compression_internal.h>
#include <bcmfp/bcmfp_alpm_compress_internal.h>
#include <bcmfp/bcmfp_internal.h>
#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>
#include <bcmfp/bcmfp_strings_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_ALPMCOMPRESS

static int
bcmfp_cid_updates_dump(int unit,
                       bcmfp_cid_update_list_t *cid_update_info,
                       bcmfp_cid_update_cb_info_t *cb_info)
{
    uint32_t *u32_arr = NULL;
    bcmfp_cid_update_list_t *temp_cid_update_info = NULL;
    shr_pb_t *pb = NULL;
    int num_words = 0;
    bcmfp_key_t mask;
    uint16_t max_prefix_mask = 0;
    uint16_t prefix = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cid_update_info, SHR_E_PARAM);
    SHR_NULL_CHECK(cb_info, SHR_E_PARAM);

    pb = shr_pb_create();

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "----------START OF FP ALPM CID UPDATES DUMP----------\n")));
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Stage ID:%s\n"),
                    bcmfp_stage_string(cb_info->stage_id)));
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Compress Type:%d\n\n"), cb_info->compress_type));
    if (cb_info->keys_moved_from_list_to_trie == TRUE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Some existing ckeys are moved from"
                              " depth to width compression.\n")));
    }

    temp_cid_update_info = cid_update_info;
    max_prefix_mask = cb_info->trie->max_prefix_mask;
    while (temp_cid_update_info != NULL) {
        num_words = ((BCMFP_MAX_KEY_LENGTH + 31) / 32);
        shr_pb_reset(pb);
        shr_pb_printf(pb, "Compress Key Data:");
        u32_arr = temp_cid_update_info->key.w;
        shr_pb_format_uint32(pb, NULL, u32_arr, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));

        shr_pb_reset(pb);
        shr_pb_printf(pb, "Compress Key Mask:");
        prefix = temp_cid_update_info->prefix;
        SHR_BITCLR_RANGE(mask.w, 0, (32 * num_words));
        SHR_BITSET_RANGE(mask.w, (max_prefix_mask - prefix), prefix);
        shr_pb_format_uint32(pb, NULL, mask.w, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));

        num_words = ((BCMFP_MAX_CID_SIZE + 31) / 32);
        shr_pb_reset(pb);
        shr_pb_printf(pb, "Old Cid Data:");
        u32_arr = temp_cid_update_info->old_cid.w;
        shr_pb_format_uint32(pb, NULL, u32_arr, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));

        shr_pb_reset(pb);
        shr_pb_printf(pb, "Old Cid Mask:");
        u32_arr = temp_cid_update_info->old_cid_mask.w;
        shr_pb_format_uint32(pb, NULL, u32_arr, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));

        shr_pb_reset(pb);
        shr_pb_printf(pb, "New Cid Data:");
        u32_arr = temp_cid_update_info->new_cid.w;
        shr_pb_format_uint32(pb, NULL, u32_arr, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));

        shr_pb_reset(pb);
        shr_pb_printf(pb, "New Cid Mask:");
        u32_arr = temp_cid_update_info->new_cid_mask.w;
        shr_pb_format_uint32(pb, NULL, u32_arr, num_words, 0);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s\n\n"), shr_pb_str(pb)));

        temp_cid_update_info = temp_cid_update_info->next;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "----------END OF FP ALPM CID UPDATES DUMP----------\n")));
exit:
    shr_pb_destroy(pb);
    SHR_FUNC_EXIT();
}

static int
bcmfp_cid_updates_internal(int unit,
                  bcmfp_cid_update_list_t *cid_update_info,
                  void *user_data)
{
    bcmfp_cid_update_cb_info_t *cb_info = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    uint8_t compress_type = 0;
    bcmfp_idp_info_t idp_info;
    void *idp_user_data = NULL;
    uint8_t num_words = 0;
    uint8_t idx = 0;
    bcmfp_entry_id_traverse_cb cb_func;
    uint32_t *alpm_key = NULL;
    uint32_t *alpm_mask = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmltd_sid_t req_ltid = 0;
    bcmfp_cid_t *cid = NULL;
    size_t size = 0;
    uint16_t prefix = 0;
    uint16_t start_bit = 0;
    bcmfp_cid_update_list_t *temp_cid_update_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bool cid_is_zero = TRUE;
    uint32_t *entry_id_bmp = NULL;
    uint32_t max_entries = 0;

    SHR_FUNC_ENTER(unit);

    if (cid_update_info == NULL) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    cb_info = user_data;
    stage_id = cb_info->stage_id;
    compress_type = cb_info->compress_type;
    req_ltid = cb_info->fp_alpm_sid;
    trie = cb_info->trie;

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_cid_updates_dump(unit,
                                    cid_update_info,
                                    cb_info));
    }

    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(alpm_mask, size, "bcmfpAlpmEntryCidUpdate");
    /* Update the ALPM entries. */
    temp_cid_update_info = cid_update_info;
    num_words = SHRi_BITDCLSIZE(BCMFP_MAX_CID_SIZE);
    while (temp_cid_update_info != NULL) {
        cid_is_zero = TRUE;
        cid = &(temp_cid_update_info->old_cid);
        for (idx = 0; idx < num_words; idx++) {
            if (cid->w[idx] != 0) {
                cid_is_zero = FALSE;
                break;
            }
        }
        /*
         * Old cid is zero means, this is the first time this
         * ALPM entry is inserted. So no need to update it.
         */
        if (cid_is_zero == TRUE) {
            temp_cid_update_info = temp_cid_update_info->next;
            continue;
        }
        alpm_key = temp_cid_update_info->key.w;
        prefix = temp_cid_update_info->prefix;
        start_bit = trie->max_prefix_mask - prefix;
        sal_memset(alpm_mask, 0, size);
        SHR_BITSET_RANGE(alpm_mask, start_bit, prefix);
        cid = &(temp_cid_update_info->new_cid);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_alpm_compress_update_hw(unit,
                                           cb_info->op_arg,
                                           req_ltid,
                                           stage_id,
                                           alpm_key,
                                           alpm_mask,
                                           trie,
                                           cid));
        temp_cid_update_info = temp_cid_update_info->next;
    }

    /*
     * Traverse through FP entries using CID/CID_MASK
     * pairs in the cid_update_info list and update
     * the FP entries with new CID and CID_MASK.
     */
    temp_cid_update_info = cid_update_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    req_ltid = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit, stage_id, &max_entries));
    size = (((max_entries + 31) / 32) * sizeof(uint32_t));
    BCMFP_ALLOC(entry_id_bmp, size, "bcmfpEntryIdBmp");
    while (temp_cid_update_info != NULL) {
        cid_is_zero = TRUE;
        cid = &(temp_cid_update_info->old_cid);
        for (idx = 0; idx < num_words; idx++) {
            if (cid->w[idx] != 0) {
                cid_is_zero = FALSE;
                break;
            }
        }
        /*
         * Old cid is zero means, this is the first time this
         * ALPM entry is inserted. So no need to update it.
         */
        if (cid_is_zero == TRUE) {
            temp_cid_update_info = temp_cid_update_info->next;
            continue;
        }
        sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
        idp_info.op_arg = cb_info->op_arg;
        idp_info.tbl_id = req_ltid;
        idp_info.cid_curr= &(temp_cid_update_info->old_cid);
        idp_info.cid_mask_curr = &(temp_cid_update_info->old_cid_mask);
        idp_info.cid_next = &(temp_cid_update_info->new_cid);
        idp_info.cid_mask_next = &(temp_cid_update_info->new_cid_mask);
        idp_info.compress_type = compress_type;
        idp_info.ckey = temp_cid_update_info->key.w;
        idp_info.prefix = temp_cid_update_info->prefix;
        idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
        idp_info.entry_id_bmp = entry_id_bmp;
        idp_info.generic_data = cb_info;
        idp_info.cid_update_flags = temp_cid_update_info->flags;
        idp_user_data = (void *)(&idp_info);
        if (temp_cid_update_info->flags == BCMFP_TRIE_NODE_WIDTH_COMPRESSED) {
            cb_info->keys_moved_from_list_to_trie = TRUE;
        }
        cb_func = bcmfp_entry_idp_ckey_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ckey_entry_map_traverse(unit,
                                           stage_id,
                                           compress_type,
                                           temp_cid_update_info->key.w,
                                           temp_cid_update_info->prefix,
                                           cb_func,
                                           idp_user_data));
        temp_cid_update_info = temp_cid_update_info->next;
    }

exit:
    SHR_FREE(alpm_mask);
    SHR_FREE(entry_id_bmp);
    SHR_FUNC_EXIT();
}

int
bcmfp_cid_cb_info_clear(int unit, bcmfp_stage_id_t stage_id)
{
    bcmfp_cid_update_list_t *temp_cid_update_info = NULL;
    uint8_t ctype = 0;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    for (ctype = 0; ctype < BCMFP_COMPRESS_TYPES_MAX; ctype++) {
        SHR_FREE(stage->cb_info[ctype]);
        stage->cb_info[ctype] = NULL;
        if (stage->cid_update_info[ctype] != NULL) {
            temp_cid_update_info = stage->cid_update_info[ctype];
            while (temp_cid_update_info != NULL) {
                stage->cid_update_info[ctype] =
                       stage->cid_update_info[ctype]->next;
                SHR_FREE(temp_cid_update_info);
                temp_cid_update_info =
                                stage->cid_update_info[ctype];
            }
            stage->cid_update_info[ctype] = NULL;
        }
    }

    size = (stage->compress_md.entry_id_bmp_size * sizeof(uint32_t));
    sal_memset(stage->compress_md.entry_id_bmp, 0, size);
    size = (stage->compress_md.group_id_bmp_size * sizeof(uint32_t));
    sal_memset(stage->compress_md.group_id_bmp, 0, size);
    size = (stage->compress_md.entry_count_in_group_size * sizeof(uint32_t));
    sal_memset(stage->compress_md.entry_count_in_group, 0, size);
    stage->compress_md.req_slots = 0;
    stage->compress_md.num_groups = 0;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_entries_bmp_update(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             uint32_t ckey,
                             void *user_data)
{
    bool lc_status = FALSE;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_config_t *econfig = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_status_get(unit,
                                          stage_id,
                                          entry_id,
                                          &lc_status));
    if (lc_status == TRUE) {
        if (!SHR_BITGET(stage->compress_md.entry_id_bmp, entry_id)) {
            SHR_IF_ERR_EXIT
                (bcmfp_entry_remote_config_get(unit,
                                               NULL,
                                               stage_id,
                                               entry_id,
                                               &econfig));
            group_id = econfig->group_id;
            SHR_BITSET(stage->compress_md.entry_id_bmp, entry_id);
            if (!SHR_BITGET(stage->compress_md.group_id_bmp, group_id)) {
                stage->compress_md.num_groups++;
            }
            SHR_BITSET(stage->compress_md.group_id_bmp, group_id);
            stage->compress_md.entry_count_in_group[group_id] += 1;
            stage->compress_md.req_slots += 1;
            bcmfp_entry_config_free(unit, econfig);
            econfig = NULL;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_cid_updates(int unit,
                  bcmfp_cid_update_list_t *cid_update_info,
                  void *user_data)
{
    bcmfp_cid_update_list_t *temp_cid_update_info = NULL;
    uint8_t ctype = 0;
    bcmfp_cid_update_cb_info_t *cb_info = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;
    bcmfp_entry_id_traverse_cb cb_func;

    SHR_FUNC_ENTER(unit);

    if (cid_update_info == NULL) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    cb_info = user_data;
    ctype = cb_info->compress_type;
    stage_id = cb_info->stage_id;
    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_FREE(stage->cb_info[ctype]);
    size = sizeof(bcmfp_cid_update_cb_info_t);
    BCMFP_ALLOC(stage->cb_info[ctype],
                size,
                "bcmfpCidUpdateCbInfo");
    sal_memcpy(stage->cb_info[ctype], cb_info, size);

    if (stage->cid_update_info[ctype] != NULL) {
        temp_cid_update_info = stage->cid_update_info[ctype];
        while (temp_cid_update_info != NULL) {
            stage->cid_update_info[ctype] =
                   stage->cid_update_info[ctype]->next;
            SHR_FREE(temp_cid_update_info);
            temp_cid_update_info =
                            stage->cid_update_info[ctype];
        }
        stage->cid_update_info[ctype] = NULL;
    }

    cb_func = bcmfp_compress_entries_bmp_update;
    stage->cid_update_info[ctype] = cid_update_info;
    while (cid_update_info != NULL) {
        if (cid_update_info->flags ==
                BCMFP_TRIE_NODE_WIDTH_COMPRESSED) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ckey_entry_map_traverse(unit,
                                           stage_id,
                                           ctype,
                                           cid_update_info->key.w,
                                           cid_update_info->prefix,
                                           cb_func,
                                           NULL));
        }
        cid_update_info = cid_update_info->next;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_cid_update_info_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_filter_t *filter)
{
    uint8_t ctype = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_id_t child_eid = 0;
    bcmfp_entry_id_t entry_id = 0;
    bool list_compressed = FALSE;
    bool cb_info_found = FALSE;
    bool map_not_found = FALSE;
    bcmltd_sid_t req_ltid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    for (ctype = 0; ctype < BCMFP_COMPRESS_TYPES_MAX; ctype++) {
        if (stage->cid_update_info[ctype] == NULL ||
            stage->cb_info[ctype] == NULL) {
            continue;
        }
        cb_info_found = TRUE;
        SHR_IF_ERR_EXIT
            (bcmfp_cid_updates_internal(unit,
                                        stage->cid_update_info[ctype],
                                        stage->cb_info[ctype]));
    }

    /*
     * If multiple similar entries(actions, priority, non compress match
     * criteria and compress match critreria( are list compressed, there
     * will not be any callback from trie algorithim when compression
     * keys in parent entry are removed. In such one of the child entry
     * must be made parent.
     */
    if (cb_info_found == FALSE && filter != NULL) {
        entry_id = filter->entry_config->entry_id;
        SHR_IF_ERR_EXIT
            (bcmfp_entry_list_compress_status_get(unit,
                                                  stage_id,
                                                  entry_id,
                                                  &list_compressed));
        if (list_compressed == FALSE) {
            /*
             * Check if the parent entry has list compressed child
             * nodes attached to it.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_list_compress_child_entry_map_check(unit,
                                                           stage_id,
                                                           entry_id,
                                                           &map_not_found));
            /*
             * If map is found, move child nodes to current entry,
             * as the parent entry is going to be deleted.
             */
            if (map_not_found == FALSE) {
                SHR_IF_ERR_EXIT
                    (bcmfp_entry_list_compress_first_child_entry_get(unit,
                                                                 stage_id,
                                                                 entry_id,
                                                                 &child_eid));

                /*
                 * Delete all the list compressed child entries from
                 * parent entry id and add them(except first child entry)
                 * to the first child entry as child entries.
                 */
                SHR_IF_ERR_EXIT
                    (bcmfp_entry_list_compress_child_entry_map_move(unit,
                                                               stage_id,
                                                               entry_id,
                                                               &child_eid));

                /* Copy filter from parent eid to child eid. */
                req_ltid = stage->tbls.entry_tbl->sid;
                SHR_IF_ERR_EXIT
                    (bcmfp_filter_copy(unit,
                                       op_arg,
                                       stage_id,
                                       req_ltid,
                                       child_eid,
                                       filter));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_resource_check(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    size_t size = 0;
    uint32_t req_flags = 0;
    uint32_t bflags = 0;
    bcmptm_rm_tcam_fp_resources_req_t *req_res = NULL;
    uint16_t req_res_count = 0;
    bool resources_available = FALSE;
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;
    bcmptm_rm_tcam_group_mode_t ptm_mode = 0;
    uint16_t num_groups = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->compress_md.req_slots == 0) {
        SHR_EXIT();
    }

    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size, "bcmfpGroupInfoReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size, "bcmfpGroupInfoRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_filter_buffers_allocate(unit,
                                      bflags,
                                      stage->tbls.entry_tbl->sid,
                                      stage,
                                      rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       stage->tbls.entry_tbl->sid,
                                       stage,
                                       req_buffers));

    size = (sizeof(bcmptm_rm_tcam_fp_resources_req_t) *
            stage->compress_md.num_groups);
    BCMFP_ALLOC(req_res, size, "bcmfpRmTcamResourcesRequired");
    for (idx1 = 0; idx1 < stage->compress_md.num_groups; idx1++) {
         req_res[idx1].priority = 0;
         req_res[idx1].pipe_id = -1;
    }
    for (idx1 = 0;
         idx1 < stage->compress_md.entry_count_in_group_size;
         idx1++) {
        if (num_groups == stage->compress_md.num_groups) {
            break;
        }
        if (stage->compress_md.entry_count_in_group[idx1] == 0) {
            continue;
        }
        num_groups++;
        SHR_IF_ERR_EXIT
            (bcmfp_group_oper_info_get(unit,
                                       stage_id,
                                       idx1,
                                       &opinfo));
        for (idx2 = 0; idx2 < idx1; idx2++) {
            if (opinfo->group_prio == req_res[idx2].priority &&
                opinfo->tbl_inst == req_res[idx2].pipe_id) {
                break;
            }
        }
        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE
                         (opinfo->group_mode, ptm_mode);
        if (idx2 == idx1) {
            req_res[req_res_count].priority = opinfo->group_prio;
            req_res[req_res_count].group_id[ptm_mode] = (idx1 - 1);
            req_res[req_res_count].entries_req[ptm_mode] =
                   stage->compress_md.entry_count_in_group[idx1];
            req_res[req_res_count].pipe_id = opinfo->tbl_inst;
            if (idx1 == group_id) {
                req_res[req_res_count].entries_req[ptm_mode] += 1;
            }
            req_res_count++;
        } else {
            req_res[idx2].entries_req[ptm_mode] +=
                    stage->compress_md.entry_count_in_group[idx1];
            if (idx2 == group_id) {
                req_res[idx2].entries_req[ptm_mode] += 1;
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_resource_scope(unit,
                                  req_flags,
                                  op_arg,
                                  stage_id,
                                  stage->tbls.entry_tbl->sid,
                                  req_buffers,
                                  rsp_buffers,
                                  req_res,
                                  req_res_count,
                                  &resources_available));
    if (resources_available == FALSE) {
        SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    bcmfp_filter_buffers_free(unit, req_buffers);
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FREE(attrs);
    SHR_FREE(req_res);
    SHR_FUNC_EXIT();
}
