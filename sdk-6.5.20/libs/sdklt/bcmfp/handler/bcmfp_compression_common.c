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

int
bcmfp_cid_updates(int unit,
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
            (bcmfp_alpm_compress_update_internal(unit,
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
