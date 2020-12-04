/*! \file bcmfp_cth_alpm_compress.c
 *
 * APIs to insert, delete or lookup FP ALPM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmfp/bcmfp_alpm_compress_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmptm/bcmptm_cth_alpm_be_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_alpm_compress_ewords_get(int unit,
                           uint8_t type,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_alpm_entry_info_t *einfo,
                           uint32_t *entry_words)
{
    bcmfp_stage_t *stage = NULL;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *map_group = NULL;
    const bcmlrd_map_entry_t *map_entry = NULL;
    int min_bit = 0;
    int width = 0;
    int t_width = 0;
    uint32_t value = 0;
    bcmltd_sid_t sid = 0;
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;
    uint16_t idx3 = 0;
    bcmltd_fid_t *fids = NULL;
    uint8_t num_fids = 0;
    int offset = 0;
    bcmltd_fid_t map_fid = 0;
    bcmltd_fid_t cid_fid = 0;
    bcmltd_fid_t dtype_fid = 0;
    uint32_t *src_value = NULL;
    const bcmlrd_field_xfrm_desc_t *desc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(einfo, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sid = stage->tbls.compress_alpm_tbl[type]->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* Fill all the fixed fields from map. */
    map_group = &(map->group[0]);
    for (idx1 = 0; idx1 < map_group->entries; idx1++) {
        map_entry = &(map_group->entry[idx1]);
        if (map_entry->entry_type !=
            BCMLRD_MAP_ENTRY_FIXED_KEY) {
            continue;
        }
        value = map_entry->u.fixed.value;
        min_bit = map_entry->desc.minbit;
        width = (map_entry->desc.maxbit -
                 map_entry->desc.minbit + 1);
        SHR_BITCOPY_RANGE(entry_words,
                          min_bit,
                          &value,
                          0,
                          width);
    }

    /* Fill the key fields with data coming from FP. */
    fids = stage->tbls.compress_alpm_tbl[type]->key_fids;
    num_fids =
        stage->tbls.compress_alpm_tbl[type]->num_key_fids;
    offset = 0;
    for (idx1 = 0; idx1 < num_fids; idx1++) {
        for (idx2 = 0; idx2 < map_group->entries; idx2++) {
            map_entry = &(map_group->entry[idx2]);
            if ((map_entry->entry_type !=
                 BCMLRD_MAP_ENTRY_MAPPED_KEY) &&
                (map_entry->entry_type !=
                 BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER)) {
                continue;
            }
            if (map_entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
                map_fid = map_entry->u.mapped.src.field_id;
                if (fids[idx1] != map_fid) {
                    continue;
                }
                min_bit = map_entry->desc.minbit;
                width = (map_entry->desc.maxbit -
                         map_entry->desc.minbit + 1);
                SHR_BITCOPY_RANGE(entry_words,
                                  min_bit,
                                  einfo->key,
                                  offset,
                                  width);
                offset += width;
            } else {
                desc = map_entry->u.xfrm.desc;
                if (desc->src[0].field_id != fids[idx1]) {
                    continue;
                }
                t_width = 0;
                for (idx3 = 0; idx3 < desc->src_fields; idx3++) {
                    min_bit = desc->dst[idx3].minbit;
                    width = (desc->dst[idx3].maxbit -
                             desc->dst[idx3].minbit + 1);
                    SHR_BITCOPY_RANGE(entry_words,
                                      min_bit,
                                      einfo->key,
                                      offset + desc->src[idx3].minbit,
                                      width);
                    t_width += width;
                }
                offset += t_width;
            }
            break;
        }
    }

    /* Fill the mask fields with data coming from FP. */
    fids = stage->tbls.compress_alpm_tbl[type]->mask_fids;
    num_fids =
        stage->tbls.compress_alpm_tbl[type]->num_mask_fids;
    offset = 0;
    for (idx1 = 0; idx1 < num_fids; idx1++) {
        for (idx2 = 0; idx2 < map_group->entries; idx2++) {
            map_entry = &(map_group->entry[idx2]);
            if ((map_entry->entry_type !=
                 BCMLRD_MAP_ENTRY_MAPPED_KEY) &&
                (map_entry->entry_type !=
                 BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER)) {
                continue;
            }
            if (map_entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
                map_fid = map_entry->u.mapped.src.field_id;
                if (fids[idx1] != map_fid) {
                    continue;
                }
                min_bit = map_entry->desc.minbit;
                width = (map_entry->desc.maxbit -
                         map_entry->desc.minbit + 1);
                SHR_BITCOPY_RANGE(entry_words,
                                  min_bit,
                                  einfo->mask,
                                  offset,
                                  width);
                offset += width;
            } else {
                desc = map_entry->u.xfrm.desc;
                if (desc->src[0].field_id != fids[idx1]) {
                    continue;
                }
                t_width = 0;
                for (idx3 = 0; idx3 < desc->src_fields; idx3++) {
                    min_bit = desc->dst[idx3].minbit;
                    width = (desc->dst[idx3].maxbit -
                             desc->dst[idx3].minbit + 1);
                    SHR_BITCOPY_RANGE(entry_words,
                                      min_bit,
                                      einfo->mask,
                                      offset + desc->src[idx3].minbit,
                                      width);
                    t_width += width;
                }
                offset += t_width;
            }
            break;
        }
    }

    /* Fill the polciy fields with data coming from FP. */
    cid_fid = stage->tbls.compress_alpm_tbl[type]->cid_fid;
    dtype_fid =
        stage->tbls.compress_alpm_tbl[type]->data_type_fid;
    offset = 0;
    for (idx2 = 0; idx2 < map_group->entries; idx2++) {
        map_entry = &(map_group->entry[idx2]);
        if (map_entry->entry_type !=
            BCMLRD_MAP_ENTRY_MAPPED_VALUE) {
            continue;
        }
        map_fid = map_entry->u.mapped.src.field_id;
        if (!(cid_fid == map_fid ||
            dtype_fid == map_fid)) {
            continue;
        }
        if (cid_fid == map_fid) {
            src_value = einfo->cid;
        } else if (dtype_fid == map_fid) {
            src_value = &(einfo->data_type);
        } else {
            continue;
        }
        if (src_value == NULL) {
            continue;
        }
        min_bit = map_entry->desc.minbit;
        width = (map_entry->desc.maxbit -
                 map_entry->desc.minbit + 1);
        SHR_BITCOPY_RANGE(entry_words,
                          min_bit,
                          src_value,
                          offset,
                          width);
        src_value = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_ckeys_insert_to_trie(int unit,
                                bool list_compress,
                                bcmfp_rule_compress_key_t *ckeys,
                                void *user_data,
                                bool *list_compressed)
{
    bcmfp_basic_info_key_t *nodes[BCMFP_COMPRESS_TYPES_MAX];
    bcmfp_shr_trie_md_t *tries[BCMFP_COMPRESS_TYPES_MAX];
    void *udata_arr[BCMFP_COMPRESS_TYPES_MAX];
    uint8_t num_tries = 0;
    uint8_t idx = 0;
    size_t size = 0;
    bcmfp_rule_compress_key_t *temp_ckey = NULL;
    bcmfp_cid_update_cb_info_t *cb_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(nodes, 0, sizeof(nodes));
    sal_memset(tries, 0, sizeof(tries));

    SHR_NULL_CHECK(ckeys, SHR_E_PARAM);

    if (list_compress == TRUE) {
        SHR_NULL_CHECK(list_compressed, SHR_E_PARAM);
    }

    temp_ckey = ckeys;
    cb_info = user_data;
    while (temp_ckey != NULL) {
        size = sizeof(bcmfp_basic_info_key_t);
        BCMFP_ALLOC(nodes[num_tries],
                    size,
                    "bcmfpAaclTrieBasicInfoKey");
        nodes[num_tries]->prefix = temp_ckey->prefix;
        sal_memcpy(&(nodes[num_tries]->key),
                   temp_ckey->key,
                   sizeof(bcmfp_key_t));
        tries[num_tries] = temp_ckey->trie;
        udata_arr[num_tries] = cb_info;
        if (list_compress == TRUE) {
            nodes[num_tries]->flags =
                      BCMFP_TRIE_NODE_LIST_COMPRESSED;
            size = sizeof(bcmfp_cid_t);
            sal_memcpy(&(nodes[num_tries]->cid),
                       &(temp_ckey->cid),
                       size);
            sal_memcpy(&(nodes[num_tries]->cid_mask),
                       &(temp_ckey->cid_mask),
                       size);
        }
        num_tries++;
        cb_info = cb_info->next;
        temp_ckey = temp_ckey->next;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_multi_key_insert(unit,
                                         nodes,
                                         tries,
                                         num_tries,
                                         udata_arr));

    /*
     * If ckeys are not list compressed even after node_info.flags
     * is set to BCMFP_TRIE_NODE_LIST_COMPRESSED, then trie algorithm
     * clears BCMFP_TRIE_NODE_LIST_COMPRESSED in node_info.flags.
     */
    if (list_compress == TRUE) {
        for (idx = 0; idx < num_tries; idx++) {
           if (!(nodes[idx]->flags &
               BCMFP_TRIE_NODE_LIST_COMPRESSED)) {
               list_compressed[idx] = FALSE;
           } else {
               list_compressed[idx] = TRUE;
           }
        }
    }

    size = sizeof(bcmfp_cid_t);
    temp_ckey = ckeys;
    for (idx = 0; idx < num_tries; idx++) {
        sal_memcpy(&(temp_ckey->cid),
                   &(nodes[idx]->cid),
                   size);
        sal_memcpy(&(temp_ckey->cid_mask),
                   &(nodes[idx]->cid_mask),
                   size);
        temp_ckey->ref_count = nodes[idx]->ref_count;
        temp_ckey = temp_ckey->next;
    }

exit:
    for (idx = 0; idx < num_tries; idx++) {
        SHR_FREE(nodes[idx]);
        nodes[idx] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_ckeys_insert_to_hw(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_rule_compress_key_t *ckeys)
{
    bcmfp_alpm_entry_info_t einfo;
    uint8_t trie_type = 0;
    size_t size = 0;
    uint32_t *entry_words = NULL;
    bcmfp_rule_compress_key_t *temp_ckey = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(ckeys, SHR_E_PARAM);

    temp_ckey = ckeys;
    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(entry_words,
                size,
                "bcmfpCompressAlpmInsertEntryWords");
    while (temp_ckey) {
       /*
        * Other FP entries are already using the same
        * key so dont need to add the same ALPM entry
        * again in HW.
        */
        if (temp_ckey->ref_count > 1) {
            temp_ckey = temp_ckey->next;
            continue;
        }

        size = sizeof(bcmfp_alpm_entry_info_t);
        sal_memset(&einfo, 0, size);
        einfo.key = temp_ckey->key;
        einfo.mask = temp_ckey->umask;
        einfo.cid = temp_ckey->cid.w;
        einfo.data_type = 1;
        trie_type = temp_ckey->trie->trie_type;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_alpm_compress_ewords_get(unit,
                                            trie_type,
                                            stage_id,
                                            &einfo,
                                            entry_words));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_alpm_insert(unit,
                                   op_arg,
                                   temp_ckey->req_ltid,
                                   entry_words));

        size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
        sal_memset(entry_words, 0, size);
        temp_ckey->inserted = TRUE;
        temp_ckey = temp_ckey->next;
    }

exit:
    SHR_FREE(entry_words);
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_compress_insert_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    uint32_t prefix,
                                    bcmfp_shr_trie_md_t *trie,
                                    void *user_data,
                                    bool list_compress,
                                    bcmfp_cid_t *cid,
                                    bcmfp_cid_t *cid_mask,
                                    bool *list_compressed)
{
    bcmfp_basic_info_key_t node_info;
    bcmfp_alpm_entry_info_t einfo;
    uint8_t trie_type = 0;
    size_t size = 0;
    uint32_t *entry_words = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cid, SHR_E_PARAM);
    SHR_NULL_CHECK(cid_mask, SHR_E_PARAM);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(trie, SHR_E_PARAM);
    if (list_compress == TRUE) {
        SHR_NULL_CHECK(list_compressed, SHR_E_PARAM);
    }

    size = sizeof(bcmfp_basic_info_key_t);
    sal_memset(&node_info, 0, size);
    node_info.prefix = prefix;
    sal_memcpy(&node_info.key, key, sizeof(bcmfp_key_t));
    if (list_compress == TRUE) {
        node_info.flags = BCMFP_TRIE_NODE_LIST_COMPRESSED;
        size = sizeof(bcmfp_cid_t);
        sal_memcpy(&node_info.cid, cid, size);
        sal_memcpy(&node_info.cid_mask, cid_mask, size);
        *list_compressed = FALSE;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_key_insert(unit,
                                   &node_info,
                                   trie,
                                   user_data));

    /*
     * If ckeys are not list compressed even after node_info.flags
     * is set to BCMFP_TRIE_NODE_LIST_COMPRESSED, then trie algorithm
     * clears BCMFP_TRIE_NODE_LIST_COMPRESSED in node_info.flags.
     */
    if (list_compress == TRUE &&
        (node_info.flags & BCMFP_TRIE_NODE_LIST_COMPRESSED)) {
        *list_compressed = TRUE;
    }

    size = sizeof(bcmfp_cid_t);
    sal_memcpy(cid, &node_info.cid, size);
    sal_memcpy(cid_mask, &node_info.cid_mask, size);

    /*
     * Other FP entries are already using the same
     * key so dont need to add the same ALPM entry
     * again in HW.
     */
    if (node_info.ref_count > 1) {
        SHR_EXIT();
    }

    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(entry_words,
                size,
                "bcmfpCompressAlpmInsertEntryWords");
    size = sizeof(bcmfp_alpm_entry_info_t);
    sal_memset(&einfo, 0, size);
    einfo.key = key;
    einfo.mask = mask;
    einfo.cid = cid->w;
    einfo.data_type = 1;
    trie_type = trie->trie_type;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_compress_ewords_get(unit,
                                        trie_type,
                                        stage_id,
                                        &einfo,
                                        entry_words));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_alpm_insert(unit,
                               op_arg,
                               req_ltid,
                               entry_words));
exit:
    SHR_FREE(entry_words);
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_ckeys_delete_from_trie(int unit,
                                  bcmfp_rule_compress_key_t *ckeys,
                                  void *user_data)
{
    bcmfp_basic_info_key_t *nodes[BCMFP_COMPRESS_TYPES_MAX];
    bcmfp_shr_trie_md_t *tries[BCMFP_COMPRESS_TYPES_MAX];
    void *udata_arr[BCMFP_COMPRESS_TYPES_MAX];
    uint8_t num_tries = 0;
    uint8_t idx = 0;
    size_t size = 0;
    bcmfp_rule_compress_key_t *temp_ckey = NULL;
    bcmfp_cid_update_cb_info_t *cb_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(nodes, 0, sizeof(nodes));
    sal_memset(tries, 0, sizeof(tries));
    sal_memset(udata_arr, 0, sizeof(udata_arr));

    SHR_NULL_CHECK(ckeys, SHR_E_PARAM);

    temp_ckey = ckeys;
    cb_info = user_data;
    while (temp_ckey != NULL) {
        size = sizeof(bcmfp_basic_info_key_t);
        BCMFP_ALLOC(nodes[num_tries],
                    size,
                    "bcmfpAaclTrieBasicInfoKey");
        nodes[num_tries]->prefix = temp_ckey->prefix;
        sal_memcpy(&(nodes[num_tries]->key),
                   temp_ckey->key,
                   sizeof(bcmfp_key_t));
        tries[num_tries] = temp_ckey->trie;
        udata_arr[num_tries] = cb_info;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_key_lookup(unit,
                                       nodes[num_tries],
                                       tries[num_tries]));
        num_tries++;
        cb_info = cb_info->next;
        temp_ckey = temp_ckey->next;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_multi_key_delete(unit,
                                         nodes,
                                         tries,
                                         num_tries,
                                         udata_arr));


    size = sizeof(bcmfp_cid_t);
    temp_ckey = ckeys;
    for (idx = 0; idx < num_tries; idx++) {
        sal_memcpy(&(temp_ckey->cid),
                   &(nodes[idx]->cid),
                   size);
        sal_memcpy(&(temp_ckey->cid_mask),
                   &(nodes[idx]->cid_mask),
                   size);
        temp_ckey->ref_count = nodes[idx]->ref_count;
        temp_ckey = temp_ckey->next;
    }

exit:
    for (idx = 0; idx < num_tries; idx++) {
        SHR_FREE(nodes[idx]);
        nodes[idx] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_ckeys_delete_from_hw(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_rule_compress_key_t *ckeys,
                                bool revert)
{
    bcmfp_alpm_entry_info_t einfo;
    uint8_t trie_type = 0;
    size_t size = 0;
    uint32_t *entry_words = NULL;
    bcmfp_rule_compress_key_t *temp_ckey = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(ckeys, SHR_E_PARAM);

    temp_ckey = ckeys;
    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(entry_words,
                size,
                "bcmfpCompressAlpmInsertEntryWords");
    while (temp_ckey) {
       /*
        * Other FP entries are already using the same
        * key so dont need to add the same ALPM entry
        * again in HW.
        */
        if (temp_ckey->ref_count > 0) {
            temp_ckey = temp_ckey->next;
            continue;
        }

        /*
         * If it is a revert case(failed during insert),
         * then delete ckeys that were inserted.
         */
        if (revert == TRUE && ckeys->inserted != TRUE) {
            continue;
        }

        size = sizeof(bcmfp_alpm_entry_info_t);
        sal_memset(&einfo, 0, size);
        einfo.key = temp_ckey->key;
        einfo.mask = temp_ckey->umask;
        trie_type = temp_ckey->trie->trie_type;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_alpm_compress_ewords_get(unit,
                                            trie_type,
                                            stage_id,
                                            &einfo,
                                            entry_words));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_alpm_delete(unit,
                                   op_arg,
                                   temp_ckey->req_ltid,
                                   entry_words));

        size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
        sal_memset(entry_words, 0, size);
        temp_ckey = temp_ckey->next;
    }

exit:
    SHR_FREE(entry_words);
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_compress_delete_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    uint32_t prefix,
                                    bcmfp_shr_trie_md_t *trie,
                                    void *user_data,
                                    bcmfp_cid_t *cid,
                                    bcmfp_cid_t *cid_mask)
{
    bcmfp_basic_info_key_t node_info;
    bcmfp_alpm_entry_info_t einfo;
    uint8_t trie_type = 0;
    size_t size = 0;
    uint32_t *entry_words = NULL;

    SHR_FUNC_ENTER(unit);

    /* Delete the node from the trie. */
    size = sizeof(bcmfp_basic_info_key_t);
    sal_memset(&node_info, 0, size);
    node_info.prefix = prefix;
    size = sizeof(bcmfp_key_t);
    sal_memcpy(&node_info.key, key, size);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_key_lookup(unit,
                                   &node_info,
                                   trie));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_key_delete(unit,
                                   &node_info,
                                   trie,
                                   user_data));

    size = sizeof(bcmfp_cid_t);
    sal_memcpy(cid, &node_info.cid, size);
    sal_memcpy(cid_mask, &node_info.cid_mask, size);

    /*
     * Other FP entries are using the same key so
     * dont remove the ALPM entry from HW.
     */
    if (node_info.ref_count > 0) {
        SHR_EXIT();
    }

    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(entry_words,
                size,
                "bcmfpCompressAlpmDeleteEntryWords");
    size = sizeof(bcmfp_alpm_entry_info_t);
    sal_memset(&einfo, 0, size);
    einfo.key = key;
    einfo.mask = mask;
    trie_type = trie->trie_type;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_compress_ewords_get(unit,
                                        trie_type,
                                        stage_id,
                                        &einfo,
                                        entry_words));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_alpm_delete(unit,
                               op_arg,
                               req_ltid,
                               entry_words));
exit:
    SHR_FREE(entry_words);
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_compress_update_hw(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmlrd_sid_t req_ltid,
                              bcmfp_stage_id_t stage_id,
                              uint32_t *key,
                              uint32_t *mask,
                              bcmfp_shr_trie_md_t *trie,
                              bcmfp_cid_t *cid)
{
    bcmfp_alpm_entry_info_t einfo;
    uint8_t trie_type = 0;
    size_t size = 0;
    uint32_t *entry_words = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(trie, SHR_E_PARAM);
    SHR_NULL_CHECK(cid, SHR_E_PARAM);

    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    BCMFP_ALLOC(entry_words,
                size,
                "bcmfpCompressAlpmInsertEntryWords");
    size = sizeof(bcmfp_alpm_entry_info_t);
    sal_memset(&einfo, 0, size);
    einfo.key = key;
    einfo.mask = mask;
    einfo.cid = cid->w;
    einfo.data_type = 1;
    trie_type = trie->trie_type;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_alpm_compress_ewords_get(unit,
                                        trie_type,
                                        stage_id,
                                        &einfo,
                                        entry_words));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_alpm_insert(unit,
                               op_arg,
                               req_ltid,
                               entry_words));
exit:
    SHR_FREE(entry_words);
    SHR_FUNC_EXIT();
}

int
bcmfp_alpm_compress_lookup_in_trie(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmlrd_sid_t req_ltid,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t *key,
                                   uint32_t *mask,
                                   uint16_t prefix,
                                   bcmfp_shr_trie_md_t *trie,
                                   bool trie_only,
                                   bcmfp_cid_t *cid,
                                   bcmfp_cid_t *cid_mask,
                                   uint8_t *data_type)
{
    bcmfp_basic_info_key_t node_info;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(trie, SHR_E_PARAM);
    SHR_NULL_CHECK(cid, SHR_E_PARAM);
    SHR_NULL_CHECK(cid_mask, SHR_E_PARAM);
    SHR_NULL_CHECK(data_type, SHR_E_PARAM);

    /* Lookup the node from the trie. */
    size = sizeof(bcmfp_basic_info_key_t);
    sal_memset(&node_info, 0, size);
    node_info.prefix = prefix;
    size = sizeof(bcmfp_key_t);
    sal_memcpy(&node_info.key, key, size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_key_lookup(unit,
                                   &node_info,
                                   trie));

    /* Copy required fields from trie data base. */
    size = sizeof(bcmfp_cid_t);
    sal_memcpy(cid, &node_info.cid, size);
    sal_memcpy(cid_mask, &node_info.cid_mask, size);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_alpm_compress_event_process(int unit,
                                  const char *event,
                                  uint64_t ev_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_INGRESS;
    bcmltd_op_arg_t op_arg;
    bcmevm_extend_ev_data_t *extended_ev_data = NULL;
    bcmptm_cth_alpm_control_t *alpm_ctrl = NULL;
    uint32_t trans_id = 9;

    SHR_FUNC_ENTER(unit);

    extended_ev_data =
         (bcmevm_extend_ev_data_t *)(unsigned long)ev_data;
    if (extended_ev_data == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (stage == NULL) {
        SHR_EXIT();
    }

    alpm_ctrl = (bcmptm_cth_alpm_control_t *)(unsigned long) ev_data;

    sal_memset(&op_arg, 0, sizeof(bcmltd_op_arg_t));
    op_arg.trans_id = trans_id;

    /* clean up the memory allocated for compress */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_cleanup(unit, stage));

    /* Reinitialize the compress fid info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_init(unit,
                             stage,
                             bcmfp_cid_updates,
                             alpm_ctrl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_fid_hw_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_alpm_compress_event_cb(int unit,
                             const char *event,
                             uint64_t ev_data)
{
    (void)bcmfp_alpm_compress_event_process(unit, event, ev_data);
}
