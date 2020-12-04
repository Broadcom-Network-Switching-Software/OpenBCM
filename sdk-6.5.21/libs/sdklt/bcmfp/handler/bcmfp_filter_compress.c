/*! \file bcmfp_filter_compress.c
 *
 * This file contains functions managing ACL compression.
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_alpm_compress_internal.h>
#include <bcmfp/bcmfp_compression_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_filter_list_compress_cids_get(int unit,
                           bcmfp_list_compress_data_t *lc_data)
{
    int rv = SHR_E_NONE;
    size_t size = 0;
    bcmfp_basic_info_key_t *trie_key_info = NULL;
    bcmfp_rule_compress_key_t *ckeys = NULL;
    bcmfp_rule_compress_key_t *ckeys_in = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_rule_config_t *rconfig = NULL;
    uint8_t ctype = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lc_data, SHR_E_PARAM);

    /* Get the compression key(s) from rule config. */
    stage_id = lc_data->stage_id;
    group_id = lc_data->filter->entry_config->group_id;
    rconfig = lc_data->rule_config;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_compress_keys_get(unit,
                                      stage_id,
                                      group_id,
                                      rconfig,
                                      &ckeys));

    ckeys_in = ckeys;

    size = sizeof(bcmfp_basic_info_key_t);
    BCMFP_ALLOC(trie_key_info,
                size,
                "bcmfpFilterListCompressCidsGetTrieKeyInfo");

    while (ckeys != NULL) {
        /*
         * Do a lookup and get the CID and CID_MASK of
         * the ckey.
         */
        size = sizeof(bcmfp_key_t);
        trie_key_info->prefix = ckeys->prefix;
        sal_memcpy(trie_key_info->key.w, ckeys->key, size);
        rv = bcmfp_compress_key_lookup(unit,
                                       trie_key_info,
                                       ckeys->trie);
        size = sizeof(bcmfp_cid_t);
        ctype = ckeys->trie->trie_type;
        sal_memcpy((&lc_data->cid[ctype])->w,
                    trie_key_info->cid.w,
                    size);
        sal_memcpy((&lc_data->cid_mask[ctype])->w,
                    trie_key_info->cid_mask.w,
                    size);
        ckeys = ckeys->next;
    }

    if (rv == SHR_E_NOT_FOUND) {
        /* Cannot be list compressed */
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    bcmfp_rule_compress_keys_free(unit, ckeys_in);
    SHR_FREE(trie_key_info);
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_list_compress_criteria_check(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    bcmfp_entry_id_t entry_id,
                                    bcmfp_group_id_t group_id,
                                    void *user_data)
{
    uint8_t idx = 0;
    bcmfp_filter_t *filter = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_list_compress_data_t *lc_data = NULL;
    bool matched = FALSE;
    bool lc_status = FALSE;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    lc_data = user_data;
    if (lc_data->criteria_matched == TRUE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_status_get(unit,
                                              stage_id,
                                              entry_id,
                                              &lc_status));
    if (lc_status == TRUE) {
        SHR_EXIT();
    }

    filter = lc_data->filter;

    if (filter == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (filter->entry_config == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * This is entry updated case. Entry should not be
     * participated with itself for list compression
     * match criteria.
     */
    if (filter->entry_config->entry_id == entry_id) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       lc_data->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));

    /*
     * If entry state is not success, that means entry is
     * not installed in hardware.
     */
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        SHR_EXIT();
    }

    /* 1. Priority of the entries must be same. */
    if (entry_config->priority !=
        filter->entry_config->priority) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    /*
     * 2. Flex counter action is part of policy table. So both
     * entries must have same flex counter action id.
     */
    if (stage->tbls.entry_tbl->flex_ctr_action_fid != 0) {
        if (entry_config->flex_ctr_action !=
            filter->entry_config->flex_ctr_action) {
            lc_data->criteria_matched = FALSE;
            SHR_EXIT();
        }
    }

    /*
     * 3. Meter controls are part of policy table. So both
     * entries must have same meter id.
     */
    if (stage->tbls.entry_tbl->meter_id_fid != 0) {
        if (entry_config->meter_id !=
            filter->entry_config->meter_id) {
            lc_data->criteria_matched = FALSE;
            SHR_EXIT();
        }
    }

    /*
     * 4. Action data specified in policy of the entries
     * must be same.
     */

    /* Must have non zero policy ID. */
    if (entry_config->policy_id == 0 ||
        filter->entry_config->policy_id == 0) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_remote_config_get(unit,
                                        lc_data->op_arg,
                                        stage_id,
                                        entry_config->policy_id,
                                        &policy_config));
    if (filter->policy_config == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_remote_config_get(unit,
                                            lc_data->op_arg,
                                            stage_id,
                                            filter->entry_config->policy_id,
                                            &filter->policy_config));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_compare(unit,
                                     policy_config,
                                     filter->policy_config,
                                     &matched));
    if (matched == FALSE) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    /*
     * 5. Non compression match criteria must be same in
     *    both the entries.
     */
    /* Must have non zero rule ID. */
    if (entry_config->rule_id == 0 ||
        filter->entry_config->rule_id == 0) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_remote_config_get(unit,
                                      lc_data->op_arg,
                                      stage_id,
                                      entry_config->rule_id,
                                      &rule_config));
    if (filter->rule_config == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_remote_config_get(unit,
                                      lc_data->op_arg,
                                      stage_id,
                                      filter->entry_config->rule_id,
                                      &filter->rule_config));
    }

    /*
     * Compression types enabled in both the entries
     * must be same.
     */
    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (rule_config->compress_types[idx] !=
            filter->rule_config->compress_types[idx]) {
            lc_data->criteria_matched = FALSE;
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_compress_qualifiers_mark(unit,
                                             stage_id,
                                             rule_config));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_compress_qualifiers_mark(unit,
                                             stage_id,
                                             filter->rule_config));
    matched = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_non_compress_qualifiers_match(unit,
                                            rule_config,
                                            filter->rule_config,
                                            &matched));
    if (matched == FALSE) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    lc_data->criteria_matched = TRUE;
    lc_data->rule_config = rule_config;
    lc_data->entry_id = entry_id;
exit:
    bcmfp_entry_config_free(unit, entry_config);
    if (lc_data != NULL) {
        if (lc_data->criteria_matched == FALSE) {
            bcmfp_rule_config_free(unit, rule_config);
        }
    }
    bcmfp_policy_config_free(unit, policy_config);
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_list_compress_data_get(int unit,
                           bcmfp_list_compress_data_t *lc_data)
{
    bcmfp_group_id_t group_id = 0;
    bool group_not_mapped = FALSE;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = lc_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lc_data, SHR_E_PARAM);
    SHR_NULL_CHECK(lc_data->filter, SHR_E_PARAM);

    group_id = lc_data->filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     lc_data->stage_id,
                                     group_id,
                                     &group_not_mapped));
    if (group_not_mapped == TRUE) {
        lc_data->criteria_matched = FALSE;
        SHR_EXIT();
    }

    /*
     * Check if there are any entries that are mapped
     * to the same policy with same priority and non
     * compression match criteria.
     */
    cb_func = bcmfp_filter_list_compress_criteria_check;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        lc_data->stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));

    if (lc_data->criteria_matched == FALSE) {
        SHR_EXIT();
    }

    /* Get the CIDs to be used in list compression. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_list_compress_cids_get(unit, lc_data));

exit:
    if (lc_data != NULL) {
        bcmfp_rule_config_free(unit, lc_data->rule_config);
        lc_data->rule_config = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_list_compress_insert(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_entry_config_t *econfig = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_list_compress_data_t *lc_data = NULL;
    bcmfp_rule_compress_key_t *ckeys = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmltd_sid_t req_ltid = 0;
    uint8_t ctype = 0;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info1 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info2 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info3 = NULL;
    size_t size = 0;
    void *user_data = NULL;
    bcmfp_idp_info_t idp_info;
    bool list_compressed[BCMFP_COMPRESS_TYPES_MAX] = { FALSE };
    bcmfp_rule_config_t *rconfig = NULL;
    bcmfp_group_config_t *gconfig = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_data_t *temp_qual_data = NULL;
    bcmfp_tbl_compress_fid_info_t *compress_fid_info = NULL;
    uint8_t cid_size = 0;
    bcmfp_qualifier_t qual_id = 0;
    uint32_t fid = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint32_t *key = NULL;
    uint32_t *mask = NULL;
    bcmfp_group_id_t group_id = 0;
    uint8_t data_type = 0;
    bool ckey_present[BCMFP_COMPRESS_TYPES_MAX] = { FALSE };
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    filter->list_compressed = FALSE;

    econfig = filter->entry_config;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    size = sizeof(bcmfp_list_compress_data_t);
    BCMFP_ALLOC(lc_data,
                size,
                "bcmfpFilterListCompressInsertLcData");
    lc_data->filter = filter;
    lc_data->op_arg = (bcmltd_op_arg_t *)op_arg;
    lc_data->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_list_compress_data_get(unit, lc_data));

    /*
     * If the filter doesnot meet the criteria for list
     * compression, go back to width compression.
     */
    if (lc_data->criteria_matched == FALSE) {
        SHR_EXIT();
    }

    /*
     * Insert the keys in this filter with CID/CID_MASK
     * found in list compress data.
     */
    ckeys = filter->compression_keys;
    filter->list_compressed = TRUE;
    while (ckeys != NULL) {
        /*
         * Insert the compression key into ALPM tables
         * with algorithimically assigned CID.
         */
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        rv = bcmfp_alpm_compress_lookup_in_trie(unit,
                                                op_arg,
                                                req_ltid,
                                                stage_id,
                                                ckeys->key,
                                                ckeys->mask,
                                                ckeys->prefix,
                                                trie,
                                                FALSE,
                                                &(ckeys->cid),
                                                &(ckeys->cid_mask),
                                                &data_type);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        size = sizeof(bcmfp_cid_t);
        if (rv == SHR_E_NOT_FOUND) {
            ckey_present[idx] = FALSE;
        } else {
            if (sal_memcmp(&(ckeys->cid),
                           &lc_data->cid[ctype],
                           size) == 0 &&
                sal_memcmp(&(ckeys->cid_mask),
                           &lc_data->cid_mask[ctype],
                           size) == 0) {
                ckey_present[idx] = TRUE;
            } else {
                ckey_present[idx] = FALSE;
            }
        }
        size = sizeof(bcmfp_cid_t);
        sal_memcpy(&(ckeys->cid), &lc_data->cid[ctype], size);
        sal_memcpy(&(ckeys->cid_mask), &lc_data->cid_mask[ctype], size);
        idx++;
        ckeys = ckeys->next;
    }

    ckeys = filter->compression_keys;
    size = sizeof(bcmfp_cid_update_cb_info_t);
    while (ckeys != NULL) {
        BCMFP_ALLOC(cid_update_cb_info1,
                    size,
                    "bcmfpCidUpdateCbInfoInListCompressInsert");
        cid_update_cb_info1->stage_id = stage_id;
        cid_update_cb_info1->op_arg = (bcmltd_op_arg_t *)op_arg;
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        cid_update_cb_info1->compress_type = ctype;
        cid_update_cb_info1->trie = trie;
        cid_update_cb_info1->fp_alpm_sid = req_ltid;
        if (cid_update_cb_info2 == NULL) {
            cid_update_cb_info2 = cid_update_cb_info1;
            cid_update_cb_info3 = cid_update_cb_info1;
        } else {
            cid_update_cb_info2->next = cid_update_cb_info1;
            cid_update_cb_info2 = cid_update_cb_info1;
        }
        cid_update_cb_info1 = NULL;
        ckeys = ckeys->next;
    }
    user_data = cid_update_cb_info3;
    ckeys = filter->compression_keys;

    /*
     * Insert the compression key into trie with
     * algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_insert_to_trie(unit,
                                         TRUE,
                                         ckeys,
                                         user_data,
                                         list_compressed));

    while (idx != 0) {
        if (list_compressed[idx - 1] == FALSE &&
            ckey_present[idx - 1] == FALSE) {
            filter->list_compressed = FALSE;
            break;
        }
        idx--;
    }

    /*
     * Check enough resources are available in PTM if
     * multiple FP entries needs to be moved from list
     * to width.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_EXIT
        (bcmfp_compress_resource_check(unit,
                                       op_arg,
                                       stage_id,
                                       group_id));

    /*
     * Insert the compression key into ALPM tables
     * with algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_insert_to_hw(unit,
                                       op_arg,
                                       stage_id,
                                       ckeys));

    /*
     * Process the all cid update call backs fron trie
     * algorithm.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_cid_update_info_process(unit,
                                       stage_id,
                                       op_arg,
                                       NULL));

    qualifiers_fid_info =
        stage->tbls.rule_tbl->qualifiers_fid_info;
    while (ckeys != NULL) {
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        /* Add the entry ID to compress key to FP entry_id mapping. */
        sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
        idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        idp_info.ckey = ckeys->key;
        idp_info.prefix = ckeys->prefix;
        idp_info.compress_type = ctype;
        idp_info.tbl_id = req_ltid;
        idp_info.event_reason = BCMIMM_ENTRY_INSERT;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_process(unit,
                                          stage_id,
                                          econfig->entry_id,
                                          0,
                                          &idp_info));

        /* Build the CID qual data and add it to the list. */
        BCMFP_ALLOC(temp_qual_data,
                    sizeof(bcmfp_qual_data_t),
                    "bcmfpCidQualData");
        temp_qual_data->next = qual_data;
        qual_data = temp_qual_data;
        compress_fid_info = ckeys->cfid_info;
        cid_size = compress_fid_info->cid_size;
        fid = compress_fid_info->cid_key_fid;
        qual_id = qualifiers_fid_info[fid].qualifier;
        temp_qual_data->qual_id = qual_id;
        temp_qual_data->fid = qual_id;
        temp_qual_data->fidx = 0;
        key = temp_qual_data->key;
        SHR_BITCOPY_RANGE(key, 0, ckeys->cid.w, 0, cid_size);
        mask = temp_qual_data->mask;
        SHR_BITCOPY_RANGE(mask, 0, ckeys->cid_mask.w, 0, cid_size);
        temp_qual_data = NULL;
        ckeys = ckeys->next;
    }

    rconfig = filter->rule_config;
    gconfig = filter->group_config;
    group_id = gconfig->group_id;

    /* Compress the group cconfig */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_config_compress(unit,
                                     op_arg,
                                     stage_id,
                                     group_id,
                                     gconfig));

    /*
     * Replace compression eligible qualifiers key and
     * mask with CID and CID_MASK.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_compress(unit, qual_data, rconfig));

    filter->ckeys_inserted = TRUE;
    if (filter->list_compressed == FALSE) {
        SHR_EXIT();
    }

    /* Add entry Id to list compressed entry ID mapping. */
    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_idp_list_compress_process(unit,
                                               stage_id,
                                               econfig->entry_id,
                                               lc_data->entry_id,
                                               &idp_info));
exit:
    SHR_FREE(lc_data);
    while (cid_update_cb_info3 != NULL) {
        cid_update_cb_info2 = cid_update_cb_info3;
        cid_update_cb_info3 = cid_update_cb_info3->next;
        SHR_FREE(cid_update_cb_info2);
    }
    if (SHR_FUNC_ERR()) {
       while (qual_data != NULL) {
           temp_qual_data = qual_data;
           qual_data = qual_data->next;
           SHR_FREE(temp_qual_data);
       }
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_list_compress_delete(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_filter_t *filter)
{
    bcmfp_entry_config_t *econfig = NULL;
    bcmfp_rule_compress_key_t *ckeys = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmltd_sid_t req_ltid = 0;
    uint8_t ctype = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info1 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info2 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info3 = NULL;
    size_t size = 0;
    void *user_data = NULL;
    bcmfp_idp_info_t idp_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    econfig = filter->entry_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    ckeys = filter->compression_keys;
    size = sizeof(bcmfp_cid_update_cb_info_t);
    while (ckeys != NULL) {
        BCMFP_ALLOC(cid_update_cb_info1,
                    size,
                    "bcmfpCidUpdateCbInfoInListCompressDelete");
        cid_update_cb_info1->stage_id = stage_id;
        cid_update_cb_info1->op_arg = (bcmltd_op_arg_t *)op_arg;
        cid_update_cb_info1->event = BCMIMM_ENTRY_DELETE;
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        cid_update_cb_info1->compress_type = ctype;
        cid_update_cb_info1->trie = trie;
        cid_update_cb_info1->fp_alpm_sid = req_ltid;
        if (cid_update_cb_info2 == NULL) {
            cid_update_cb_info2 = cid_update_cb_info1;
            cid_update_cb_info3 = cid_update_cb_info1;
        } else {
            cid_update_cb_info2->next = cid_update_cb_info1;
            cid_update_cb_info2 = cid_update_cb_info1;
        }
        cid_update_cb_info1 = NULL;
        ckeys = ckeys->next;
    }
    user_data = cid_update_cb_info3;

    ckeys = filter->compression_keys;

    /*
     * Delete the compression key into trie with
     * algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_delete_from_trie(unit,
                                           ckeys,
                                           user_data));

    /*
     * Process the all cid update call backs fron trie
     * algorithm.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_cid_update_info_process(unit,
                                       stage_id,
                                       op_arg,
                                       NULL));

    /* Delete the compression key from ALPM table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_delete_from_hw(unit,
                                         op_arg,
                                         stage_id,
                                         ckeys,
                                         FALSE));
    while (ckeys != NULL) {
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        /*
         * Delete the entry ID from compress key to FP entry_id
         * mapping.
         */
        sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
        idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        idp_info.ckey = ckeys->key;
        idp_info.prefix = ckeys->prefix;
        idp_info.compress_type = ctype;
        idp_info.tbl_id = req_ltid;
        idp_info.event_reason = BCMIMM_ENTRY_DELETE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_process(unit,
                                          stage_id,
                                          econfig->entry_id,
                                          0,
                                          &idp_info));
        ckeys = ckeys->next;
    }

    /* Delete entry Id to list compressed entry ID mapping. */
    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.template_id = econfig->policy_id;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_idp_list_compress_process(unit,
                                               stage_id,
                                               econfig->entry_id,
                                               0,
                                               &idp_info));

exit:
    while (cid_update_cb_info3 != NULL) {
        cid_update_cb_info2 = cid_update_cb_info3;
        cid_update_cb_info3 = cid_update_cb_info3->next;
        SHR_FREE(cid_update_cb_info2);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_compress_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *filter)
{
    bcmfp_rule_config_t *rconfig = NULL;
    bcmfp_entry_config_t *econfig = NULL;
    bcmfp_group_config_t *gconfig = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_data_t *temp_qual_data = NULL;
    bcmfp_rule_compress_key_t *ckeys = NULL;
    bcmfp_rule_compress_key_t *temp_ckeys = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_tbl_compress_fid_info_t *compress_fid_info = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint8_t cid_size = 0;
    bcmfp_qualifier_t qual_id = 0;
    uint32_t fid = 0;
    bcmfp_stage_t *stage = NULL;
    uint32_t *key = NULL;
    uint32_t *mask = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmltd_sid_t req_ltid = 0;
    uint8_t ctype = 0;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info1 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info2 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info3 = NULL;
    size_t size = 0;
    void *user_data = NULL;
    bcmfp_idp_info_t idp_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    econfig = filter->entry_config;
    rconfig = filter->rule_config;
    gconfig = filter->group_config;
    group_id = econfig->group_id;

    /* Get the filter compression key(s). */
    ckeys = filter->compression_keys;

    if (ckeys == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    /*
     * Try list compression first and if succeded,
     * do nothing and return.
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_LIST_COMPRESSION)) {
        filter->compression_keys = ckeys;
        filter->list_compressed = FALSE;
        filter->ckeys_inserted = FALSE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_list_compress_insert(unit,
                                               op_arg,
                                               stage_id,
                                               filter));

        /* If filter is list compressed, do nothing else. */
        if (filter->list_compressed == TRUE) {
            SHR_EXIT();
        }

        /*
         * All other criterias are matched except that the
         * current filters keys have overlapped ckeys in
         * trie. So filter cannot be list compressed but
         * ckeys are inserted to trie and HW.
         */
        if (filter->ckeys_inserted == TRUE) {
            SHR_EXIT();
        }
    }

    /* Compress the group cconfig */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_config_compress(unit,
                                     op_arg,
                                     stage_id,
                                     group_id,
                                     gconfig));

    temp_ckeys = ckeys;
    size = sizeof(bcmfp_cid_update_cb_info_t);
    while (ckeys != NULL) {
        BCMFP_ALLOC(cid_update_cb_info1,
                    size,
                    "bcmfpCidUpdateCbInfoInInsert");
        cid_update_cb_info1->stage_id = stage_id;
        cid_update_cb_info1->op_arg = (bcmltd_op_arg_t *)op_arg;
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        cid_update_cb_info1->compress_type = ctype;
        cid_update_cb_info1->trie = trie;
        cid_update_cb_info1->fp_alpm_sid = req_ltid;
        if (cid_update_cb_info2 == NULL) {
            cid_update_cb_info2 = cid_update_cb_info1;
            cid_update_cb_info3 = cid_update_cb_info1;
        } else {
            cid_update_cb_info2->next = cid_update_cb_info1;
            cid_update_cb_info2 = cid_update_cb_info1;
        }
        cid_update_cb_info1 = NULL;
        ckeys = ckeys->next;
    }
    user_data = cid_update_cb_info3;
    ckeys = temp_ckeys;

    /*
     * Insert the compression key into trie with
     * algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_insert_to_trie(unit,
                                         FALSE,
                                         ckeys,
                                         user_data,
                                         NULL));

    /*
     * Check enough resources are available in PTM if
     * multiple FP entries needs to be moved from list
     * to width.
     */
    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_EXIT
        (bcmfp_compress_resource_check(unit,
                                       op_arg,
                                       stage_id,
                                       group_id));

    /*
     * Insert the compression key into ALPM tables
     * with algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_insert_to_hw(unit,
                                       op_arg,
                                       stage_id,
                                       ckeys));

    /*
     * Process the all cid update call backs fron trie
     * algorithm.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_cid_update_info_process(unit,
                                       stage_id,
                                       op_arg,
                                       NULL));

    qualifiers_fid_info =
        stage->tbls.rule_tbl->qualifiers_fid_info;
    temp_ckeys = ckeys;
    while (ckeys != NULL) {
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        /*
         * Add the entry ID to compress key to FP entry_id
         * mapping.
         */
        sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
        idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        idp_info.ckey = ckeys->key;
        idp_info.prefix = ckeys->prefix;
        idp_info.compress_type = ctype;
        idp_info.tbl_id = req_ltid;
        idp_info.event_reason = BCMIMM_ENTRY_INSERT;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_process(unit,
                                      stage_id,
                                      econfig->entry_id,
                                      0,
                                      &idp_info));

        /* Build the CID qual data and add it to the list. */
        BCMFP_ALLOC(temp_qual_data,
                    sizeof(bcmfp_qual_data_t),
                    "bcmfpCidQualData");
        temp_qual_data->next = qual_data;
        qual_data = temp_qual_data;
        compress_fid_info = ckeys->cfid_info;
        cid_size = compress_fid_info->cid_size;
        fid = compress_fid_info->cid_key_fid;
        qual_id = qualifiers_fid_info[fid].qualifier;
        temp_qual_data->qual_id = qual_id;
        temp_qual_data->fid = qual_id;
        temp_qual_data->fidx = 0;
        key = temp_qual_data->key;
        SHR_BITCOPY_RANGE(key, 0, ckeys->cid.w, 0, cid_size);
        mask = temp_qual_data->mask;
        SHR_BITCOPY_RANGE(mask, 0, ckeys->cid_mask.w, 0, cid_size);
        ckeys = ckeys->next;
        temp_qual_data = NULL;
    }
    ckeys = temp_ckeys;

    /*
     * Replace compression eligible qualifiers key and
     * mask with CID and CID_MASK.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_compress(unit,
                                    qual_data,
                                    rconfig));
exit:
    /*
     * Clear cid update call back info from trie algorithm
     * save in stage structure.
     */
    (void)bcmfp_cid_cb_info_clear(unit, stage_id);
    if (SHR_FUNC_ERR()) {
       (void)(bcmfp_alpm_ckeys_delete_from_hw(unit,
                                              op_arg,
                                              stage_id,
                                              ckeys,
                                              TRUE));
    }
    bcmfp_rule_compress_keys_free(unit, ckeys);
    while (cid_update_cb_info3 != NULL) {
        cid_update_cb_info2 = cid_update_cb_info3;
        cid_update_cb_info3 = cid_update_cb_info3->next;
        SHR_FREE(cid_update_cb_info2);
    }
    if (SHR_FUNC_ERR()) {
       while (qual_data != NULL) {
           temp_qual_data = qual_data;
           qual_data = qual_data->next;
           SHR_FREE(temp_qual_data);
       }
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_compress_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *filter)
{
    bcmfp_rule_config_t *rconfig = NULL;
    bcmfp_entry_config_t *econfig = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_rule_compress_key_t *ckeys = NULL;
    bcmfp_rule_compress_key_t *temp_ckeys = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmltd_sid_t req_ltid = 0;
    uint8_t ctype = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info1 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info2 = NULL;
    bcmfp_cid_update_cb_info_t *cid_update_cb_info3 = NULL;
    size_t size = 0;
    void *user_data = NULL;
    bcmfp_idp_info_t idp_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    econfig = filter->entry_config;
    group_id = econfig->group_id;
    rconfig = filter->rule_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the compression key(s) from rule config. */
    if (rconfig == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_rule_remote_config_get(unit,
                                         op_arg,
                                         stage_id,
                                         econfig->rule_id,
                                         &rconfig));
        filter->rule_config = rconfig;
    }

    /* Get the compression key(s) from rule config. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_compress_keys_get(unit,
                                      stage_id,
                                      group_id,
                                      rconfig,
                                      &ckeys));

    if (ckeys == NULL) {
        SHR_EXIT();
    }

    /*
     * If list compression is enabled, try to delete the
     * filter using list compress delete.
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_LIST_COMPRESSION)) {
        filter->compression_keys = ckeys;
        if (filter->list_compressed == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_list_compress_delete(unit,
                                                   op_arg,
                                                   stage_id,
                                                   filter));
            SHR_EXIT();
        }
    }

    temp_ckeys = ckeys;
    size = sizeof(bcmfp_cid_update_cb_info_t);
    while (ckeys != NULL) {
        BCMFP_ALLOC(cid_update_cb_info1,
                    size,
                    "bcmfpCidUpdateCbInfoInDelete");
        cid_update_cb_info1->stage_id = stage_id;
        cid_update_cb_info1->op_arg = (bcmltd_op_arg_t *)op_arg;
        cid_update_cb_info1->event = BCMIMM_ENTRY_DELETE;
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        cid_update_cb_info1->compress_type = ctype;
        cid_update_cb_info1->trie = trie;
        cid_update_cb_info1->fp_alpm_sid = req_ltid;
        if (cid_update_cb_info2 == NULL) {
            cid_update_cb_info2 = cid_update_cb_info1;
            cid_update_cb_info3 = cid_update_cb_info1;
        } else {
            cid_update_cb_info2->next = cid_update_cb_info1;
            cid_update_cb_info2 = cid_update_cb_info1;
        }
        cid_update_cb_info1 = NULL;
        ckeys = ckeys->next;
    }
    user_data = cid_update_cb_info3;
    ckeys = temp_ckeys;

    /*
     * Delete the compression key into trie with
     * algorithimically assigned CID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_delete_from_trie(unit,
                                           ckeys,
                                           user_data));

    /*
     * Process the all cid update call backs fron trie
     * algorithm.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_cid_update_info_process(unit,
                                       stage_id,
                                       op_arg,
                                       filter));

    /* Delete the compression key from ALPM table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_ckeys_delete_from_hw(unit,
                                         op_arg,
                                         stage_id,
                                         ckeys,
                                         FALSE));

    temp_ckeys = ckeys;
    while (ckeys != NULL) {
        trie = ckeys->trie;
        ctype = trie->trie_type;
        req_ltid =
            stage->tbls.compress_alpm_tbl[ctype]->sid;
        /*
         * Delete the entry ID from compress key to FP entry_id
         * mapping.
         */
        sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
        idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        idp_info.ckey = ckeys->key;
        idp_info.prefix = ckeys->prefix;
        idp_info.compress_type = ctype;
        idp_info.tbl_id = req_ltid;
        idp_info.event_reason = BCMIMM_ENTRY_DELETE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_idp_ckey_process(unit,
                                          stage_id,
                                          econfig->entry_id,
                                          0,
                                          &idp_info));
        ckeys = ckeys->next;
    }
    ckeys = temp_ckeys;

exit:
    /*
     * Clear cid update call back info from trie algorithm
     * save in stage structure.
     */
    (void)bcmfp_cid_cb_info_clear(unit, stage_id);
    bcmfp_rule_compress_keys_free(unit, ckeys);
    while (cid_update_cb_info3 != NULL) {
        cid_update_cb_info2 = cid_update_cb_info3;
        cid_update_cb_info3 = cid_update_cb_info3->next;
        SHR_FREE(cid_update_cb_info2);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_compress_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *prev_filter,
                             bool prev_compressed,
                             bcmfp_filter_t *curr_filter,
                             bool curr_compressed)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(curr_filter, SHR_E_PARAM);
    SHR_NULL_CHECK(prev_filter, SHR_E_PARAM);

    /* Nothing to do if there is no compression before */
    if (prev_compressed == FALSE) {
        SHR_EXIT();
    }

    /*
     * Previously compressed, delete the previous compression
     * installed.
     */
    if (prev_compressed == TRUE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_filter_compress_delete(unit,
                                         op_arg,
                                         stage_id,
                                         prev_filter));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_compress_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *filter)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_compress(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_opcode_t opcode,
                      bcmfp_filter_t *prev_filter,
                      bcmfp_filter_t *curr_filter)
{
    bool prev_compressed = FALSE;
    bool curr_compress = FALSE;
    bcmfp_group_id_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(curr_filter, SHR_E_PARAM);

    /*
     * Check if compression is enabled in current
     * filters group.
     */
    if (prev_filter != NULL) {
        group_id = prev_filter->entry_config->group_id;
        if (group_id != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_compress_enabled(unit,
                                          stage_id,
                                          group_id,
                                          &prev_compressed));
        }
    }
    /*
     * Check if compression is enabled in previous
     * filters group if prev_filter is non NULL.
     */
    group_id = curr_filter->entry_config->group_id;
    if (group_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_compress_enabled(unit,
                                      stage_id,
                                      group_id,
                                      &curr_compress));
    }

    /*
     * If in neither previous filter nor current filter
     * compression is enabled, return SHR_E_NONE.
     */
    if (prev_compressed == FALSE &&
        curr_compress == FALSE) {
        SHR_EXIT();
    }

    /* Dispatch the opcode specific function. */

    if (opcode == BCMFP_OPCODE_INSERT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_compress_insert(unit,
                                          op_arg,
                                          stage_id,
                                          curr_filter));
        /*
         * set the status to true if the first compression entry
         * is installed successfully.
         */
    } else if (opcode == BCMFP_OPCODE_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_compress_delete(unit,
                                          op_arg,
                                          stage_id,
                                          curr_filter));
    } else if (opcode == BCMFP_OPCODE_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_compress_update(unit,
                                          op_arg,
                                          stage_id,
                                          prev_filter,
                                          prev_compressed,
                                          curr_filter,
                                          curr_compress));
    } else if (opcode == BCMFP_OPCODE_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_compress_lookup(unit,
                                          op_arg,
                                          stage_id,
                                          curr_filter));
    } else {
       SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    /*
     * If the first entry insert fails free the memory
     * allocated during alpm_fids_init
     */
    SHR_FUNC_EXIT();
}
