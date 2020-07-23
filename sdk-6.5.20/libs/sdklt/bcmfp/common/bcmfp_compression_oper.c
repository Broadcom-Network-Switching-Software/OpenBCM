/*! \file bcmfp_compression_oper.c
 *
 * BCMFP operational information get and set APIs related to
 * FP compression.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_compression_internal.h>
#include <bcmissu/issu_api.h>
#include <shr/shr_pb.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_ALPMCOMPRESS

int
bcmfp_ckey_entry_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint8_t ctype,
                         uint32_t *key,
                         uint32_t prefix,
                         bcmfp_entry_id_t entry_id)
{
    bcmfp_compress_key_info_t *key_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint16_t num_words = 0;
    size_t size = 0;
    uint16_t array_size = 0;
    bcmfp_entry_id_t *entries = NULL;
    bool key_info_created = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_info = stage->compress_key_info[ctype];

    num_words = SHRi_BITDCLSIZE(BCMFP_MAX_KEY_LENGTH);
    size = (sizeof(uint32_t) * num_words);
    while (key_info != NULL) {
        if (!sal_memcmp(key, key_info->key.w, size) &&
            (prefix == key_info->prefix)) {
           break;
        }
        key_info = key_info->next;
    }

    if (key_info != NULL) {
        if (key_info->num_entries == key_info->entries_size) {
            array_size = key_info->num_entries +
                         BCMFP_COMRESSION_KEY_ENTRY_ARRAY_SIZE;
            size = (sizeof(bcmfp_entry_id_t) * array_size);
            BCMFP_ALLOC(entries, size, "bcmfpCompressKeyEntryArray");
            size = (sizeof(bcmfp_entry_id_t) * key_info->num_entries);
            sal_memcpy(entries, key_info->entries, size);
            SHR_FREE(key_info->entries);
            key_info->entries = entries;
            key_info->entries[key_info->num_entries] = entry_id;
            key_info->num_entries++;
            key_info->entries_size +=
                BCMFP_COMRESSION_KEY_ENTRY_ARRAY_SIZE;
        } else {
           key_info->entries[key_info->num_entries] = entry_id;
           key_info->num_entries++;
        }
    } else {
        size = sizeof(bcmfp_compress_key_info_t);
        BCMFP_ALLOC(key_info, size, "bcmfpCompressKeyInfo");
        key_info_created = TRUE;
        array_size = BCMFP_COMRESSION_KEY_ENTRY_ARRAY_SIZE;
        size = (sizeof(bcmfp_entry_id_t) * array_size);
        BCMFP_ALLOC(entries, size, "bcmfpCompressKeyEntryArray");
        key_info->entries = entries;
        key_info->entries[key_info->num_entries] = entry_id;
        key_info->num_entries++;
        key_info->entries_size = BCMFP_COMRESSION_KEY_ENTRY_ARRAY_SIZE;
        size = sizeof(bcmfp_key_t);
        sal_memcpy(key_info->key.w, key, size);
        key_info->prefix = prefix;
        key_info->next = stage->compress_key_info[ctype];
        stage->compress_key_info[ctype] = key_info;
    }
exit:
    if (entries == NULL && key_info_created == TRUE) {
        SHR_FREE(key_info);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ckey_entry_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint8_t ctype,
                            uint32_t *key,
                            uint32_t prefix,
                            bcmfp_entry_id_t entry_id)
{
    bcmfp_compress_key_info_t *key_info = NULL;
    bcmfp_compress_key_info_t *prev_key_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint16_t num_words = 0;
    size_t size = 0;
    uint16_t idx = 0;
    bool entry_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_info = stage->compress_key_info[ctype];

    num_words = SHRi_BITDCLSIZE(BCMFP_MAX_KEY_LENGTH);
    size = (sizeof(uint32_t) * num_words);
    while (key_info != NULL) {
        if (!sal_memcmp(key, key_info->key.w, size) &&
            (prefix == key_info->prefix)) {
           break;
        }
        prev_key_info = key_info;
        key_info = key_info->next;
    }

    if (key_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    for (idx = 0; idx < key_info->num_entries; idx++) {
        if (key_info->entries[idx] == entry_id) {
            key_info->entries[idx] =
                key_info->entries[key_info->num_entries - 1];
            key_info->num_entries--;
            entry_found = TRUE;
            break;
        }
    }

    if (entry_found == FALSE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key_info->num_entries != 0) {
        SHR_EXIT();
    }

    SHR_FREE(key_info->entries);
    if (prev_key_info == NULL) {
         stage->compress_key_info[ctype] = key_info->next;
    } else {
        prev_key_info->next = key_info->next;
    }
    SHR_FREE(key_info);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ckey_entry_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint8_t ctype,
                              uint32_t *key,
                              uint32_t prefix,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data)
{
    bcmfp_compress_key_info_t *key_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint16_t num_words = 0;
    size_t size = 0;
    uint16_t idx = 0;
    bcmfp_entry_id_t entry_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_info = stage->compress_key_info[ctype];

    num_words = SHRi_BITDCLSIZE(BCMFP_MAX_KEY_LENGTH);
    size = (sizeof(uint32_t) * num_words);
    while (key_info != NULL) {
        if (!sal_memcmp(key, key_info->key.w, size) &&
            (prefix == key_info->prefix)) {
           break;
        }
        key_info = key_info->next;
    }

    if (key_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    for (idx = 0; idx < key_info->num_entries; idx++) {
        entry_id = key_info->entries[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (cb(unit, stage_id, entry_id, 0, user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_oper_info_report(int unit,
                                bcmfp_stage_t *stage,
                                uint8_t blk_id)
{
    uint32_t offset = 0;
    uint16_t div_factor = 0;
    uint32_t max_entries = 0;
    size_t num_ha_blk_info = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);

    offset += sizeof(bcmfp_ha_blk_hdr_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               sizeof(bcmfp_compress_oper_info_t),
                               1,
                               BCMFP_COMPRESS_OPER_INFO_T_ID);
    offset += sizeof(bcmfp_compress_oper_info_t);

    div_factor = stage->idp_maps_modulo;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit, stage->stage_id, &max_entries));

    max_entries = (max_entries / div_factor);

    if (max_entries != 0) {
        num_ha_blk_info = ((max_entries + 1) * 2);
    }

    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               sizeof(bcmfp_ha_blk_info_t),
                               num_ha_blk_info,
                               BCMFP_HA_BLK_INFO_T_ID);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_oper_info_init(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_ha_blk_hdr_t *ha_mem)
{
    uint32_t max_entries = 0;
    uint8_t div_factor = 1;
    bcmfp_ha_blk_info_t *entry_child_entry_map_seg_info = NULL;
    bcmfp_ha_blk_info_t *entry_parent_entry_map_seg_info = NULL;
    uint32_t idx = 0;
    bcmfp_compress_oper_info_t *compress_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_mem, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    compress_oper_info = (bcmfp_compress_oper_info_t *)
                         ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage->stage_id, &stage));

    div_factor = stage->idp_maps_modulo;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit, stage->stage_id, &max_entries));

    max_entries = (max_entries / div_factor);

    compress_oper_info->entry_child_entry_map_seg =
                        (sizeof(bcmfp_ha_blk_hdr_t) +
                         sizeof(bcmfp_compress_oper_info_t));

    entry_child_entry_map_seg_info =
              (bcmfp_ha_blk_info_t *)((uint8_t *)ha_mem +
              compress_oper_info->entry_child_entry_map_seg);

    for (idx = 0; idx < (max_entries + 1); idx++) {
        entry_child_entry_map_seg_info[idx].blk_id =
                                            BCMFP_HA_BLK_ID_INVALID;
        entry_child_entry_map_seg_info[idx].blk_offset =
                                            BCMFP_HA_BLK_OFFSET_INVALID;
    }
    compress_oper_info->entry_child_entry_map_size = (max_entries + 1);

    compress_oper_info->entry_parent_entry_map_seg =
                        (sizeof(bcmfp_ha_blk_hdr_t) +
                         sizeof(bcmfp_compress_oper_info_t) +
                         (max_entries + 1) * sizeof(bcmfp_ha_blk_info_t));

    entry_parent_entry_map_seg_info =
              (bcmfp_ha_blk_info_t *)((uint8_t *)ha_mem +
              compress_oper_info->entry_parent_entry_map_seg);

    for (idx = 0; idx < (max_entries + 1); idx++) {
        entry_parent_entry_map_seg_info[idx].blk_id =
                                        BCMFP_HA_BLK_ID_INVALID;
        entry_parent_entry_map_seg_info[idx].blk_offset =
                                        BCMFP_HA_BLK_OFFSET_INVALID;
    }
    compress_oper_info->entry_parent_entry_map_size = (max_entries + 1);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_oper_info_size_get(int unit,
                                  bcmfp_stage_t *stage,
                                  uint32_t *size)
{
    uint32_t max_entries = 0;
    uint8_t div_factor = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    (*size) = 0;
    (*size) += sizeof(bcmfp_ha_blk_hdr_t);
    (*size) += sizeof(bcmfp_compress_oper_info_t);

    div_factor = stage->idp_maps_modulo;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit,
                                     stage->stage_id,
                                     &max_entries));

    max_entries = (max_entries / div_factor);

    if (max_entries != 0) {
        /*
         * Entry ID(parent entry ID) to its list compressed entry IDs
         * (child entry IDs) mapping.
         */
        (*size) += (sizeof(bcmfp_ha_blk_info_t) * (max_entries + 1));
        /*
         * Entry ID(Child entry ID) to entry ID with which its list
         * compressed(Parent entry ID).
         */
        (*size) += (sizeof(bcmfp_ha_blk_info_t) * (max_entries + 1));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_oper_info_create(int unit,
                                bcmfp_stage_id_t stage_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t bkup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t size = 0;
    uint32_t alloc_sz = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *bkup_blk_hdr = NULL;
    void *ha_mem = NULL;
    void *bkup_ha_mem = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ha_root_blk_t *root = NULL;
    bcmfp_stage_oper_info_t *stage_opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /*
     * Get the size of compress operational information required
     * for the stage.
     * .
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_oper_info_size_get(unit, stage, &size));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));

    /* Get one free HA block ID from dynamic block ID range. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_free_blk_id_get(unit, &blk_id));

    alloc_sz = size;
    ha_mem = shr_ha_mem_alloc(unit,
                              BCMMGMT_FP_COMP_ID,
                              blk_id,
                              "bcmfpHaCompressOperInfo",
                              &alloc_sz);
    SHR_NULL_CHECK(ha_mem, SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_ha_mem_free(unit, ha_mem));
        alloc_sz = size;
        ha_mem = shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  blk_id,
                                  "bcmfpHaCompressOperInfo",
                                  &alloc_sz);
        SHR_NULL_CHECK(ha_mem, SHR_E_MEMORY);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_set(unit, blk_id, ha_mem));
    sal_memset(ha_mem, 0, size);
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);

    /* Get the backup block ID corresponding to blk_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &bkup_blk_id));

    alloc_sz = size;
    bkup_ha_mem = shr_ha_mem_alloc(unit,
                                   BCMMGMT_FP_COMP_ID,
                                   bkup_blk_id,
                                   "bcmfpHaCompressOperInfo",
                                   &alloc_sz);
    SHR_NULL_CHECK(bkup_ha_mem, SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_ha_mem_free(unit, bkup_ha_mem));
        alloc_sz = size;
        bkup_ha_mem = shr_ha_mem_alloc(unit,
                                       BCMMGMT_FP_COMP_ID,
                                       bkup_blk_id,
                                       "bcmfpHaCompressOperInfo",
                                       &alloc_sz);
        SHR_NULL_CHECK(bkup_ha_mem, SHR_E_MEMORY);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_set(unit, bkup_blk_id, bkup_ha_mem));
    sal_memset(bkup_ha_mem, 0, size);
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, bkup_blk_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_opinfo));

    stage_opinfo->compress_oper_info.blk_id = blk_id;
    stage_opinfo->compress_oper_info.blk_offset = 0;

    blk_hdr = (bcmfp_ha_blk_hdr_t *)((uint8_t *)ha_mem);
    blk_hdr->backup_blk_id = bkup_blk_id;
    blk_hdr->blk_size = size;
    blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_COMPRESS_OPER_INFO;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
    blk_hdr->stage_id = stage_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_oper_info_init(unit, stage, ha_mem));

    sal_memcpy(bkup_ha_mem, ha_mem, size);
    bkup_blk_hdr = (bcmfp_ha_blk_hdr_t *)((uint8_t *)bkup_ha_mem);
    bkup_blk_hdr->backup_blk_id = 0;
    bkup_blk_hdr->blk_size = size;
    bkup_blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_COMPRESS_OPER_INFO;
    bkup_blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    bkup_blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + bkup_blk_id;
    bkup_blk_hdr->stage_id = stage_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_oper_info_report(unit, stage, blk_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_oper_info_report(unit, stage, bkup_blk_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_key_node_acquire(int unit,
                                uint8_t stage_id,
                                uint8_t ctype,
                                bcmfp_compress_key_t **ckey)
{
    bool found_blk_id = FALSE;
    void *ha_ptr = NULL;
    void *backup_ha_ptr = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t idx = 0;
    uint32_t num_ckeys = 0;
    uint8_t first_free_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_size = 0;
    uint32_t blk_size_required = 0;
    uint32_t ha_hdr_size = 0;
    uint32_t ckey_size = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;
    bcmfp_ha_root_blk_t *root = NULL;
    bcmfp_compress_key_t *ckeys = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ckey, SHR_E_PARAM);

    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = NULL;
         (void)bcmfp_ha_mem_get(unit, blk_id, &ha_ptr);
         if (ha_ptr == NULL) {
            first_free_blk_id = blk_id;
            found_blk_id = TRUE;
            break;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /* Skip HA blocks of different type. */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_DYNAMIC_CKEY) {
             continue;
         }
         /* Skip HA blocks of different stages. */
         if (blk_hdr->stage_id != stage_id) {
             continue;
         }
         /* Skip HA blocks of different compression type. */
         if (blk_hdr->ctype != ctype) {
             continue;
         }
         /*! No free HA elements in this HA block. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
             continue;
         }
         found_blk_id = TRUE;
         break;
    }

    /* There is no enough room and no block ids left over to use. */
    if ((found_blk_id == FALSE)) {
        
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* There is no enough room and free block ids are there then
     * create a new HA block.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));
    if (first_free_blk_id != BCMFP_HA_BLK_ID_INVALID) {
        num_ckeys = BCMFP_HA_CKEY_BLK_ELEMENT_COUNT;
        ha_hdr_size = sizeof(bcmfp_ha_blk_hdr_t);
        ckey_size = sizeof(bcmfp_compress_key_t);
        blk_size = (num_ckeys * ckey_size);
        blk_size += ha_hdr_size;
        blk_id = first_free_blk_id;
        blk_size_required = blk_size;
        /* Create the HA block */
        ha_ptr = shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  blk_id,
                                  "bcmfpCompressionBlock",
                                  &blk_size);
        SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        if (blk_size < blk_size_required) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_ha_mem_free(unit, ha_ptr));
            blk_size = blk_size_required;
            ha_ptr = shr_ha_mem_alloc(unit,
                                BCMMGMT_FP_COMP_ID,
                                blk_id,
                                "bcmfpCompressionBlock",
                                &blk_size);
            SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_set(unit, blk_id, ha_ptr));
        sal_memset(ha_ptr, 0, blk_size);
        SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);
        /* Initialize the block header. */
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
        blk_hdr->free_blk_offset = ha_hdr_size;
        blk_hdr->free_ha_element_count = num_ckeys;
        blk_hdr->blk_size = blk_size;
        blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->ctype = ctype;
        blk_hdr->stage_id = stage_id;
        blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
        blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_DYNAMIC_CKEY;
        ckeys = (bcmfp_compress_key_t *)
                ((uint8_t *)ha_ptr + ha_hdr_size);
        for (idx = 0; idx < num_ckeys; idx++) {
            ckeys[idx].blk_id =  blk_id;
            ckeys[idx].blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
            ckeys[idx].next_blk_offset =
                       (((idx + 1) * ckey_size) + ha_hdr_size);
        }
        ckeys[num_ckeys - 1].next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;

        /* Create the back up block Id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        blk_hdr->backup_blk_id = backup_blk_id;
        backup_ha_ptr = shr_ha_mem_alloc(unit,
                                         BCMMGMT_FP_COMP_ID,
                                         backup_blk_id,
                                         "bcmfpCompressionBkupBlock",
                                         &blk_size);
        SHR_NULL_CHECK(backup_ha_ptr, SHR_E_MEMORY);
        if (blk_size < blk_size_required) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_ha_mem_free(unit, backup_ha_ptr));
            blk_size = blk_size_required;
            backup_ha_ptr = shr_ha_mem_alloc(unit,
                                       BCMMGMT_FP_COMP_ID,
                                       backup_blk_id,
                                       "bcmfpCompressionBkupBlock",
                                       &blk_size);
            SHR_NULL_CHECK(backup_ha_ptr, SHR_E_MEMORY);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_set(unit, backup_blk_id, backup_ha_ptr));
        backup_blk_hdr =
            (bcmfp_ha_blk_hdr_t *)backup_ha_ptr;
        SHR_BITSET(root->in_use_ha_blk_id_bmp.w, backup_blk_id);
        /* Copy the Active block to backup block. */
        sal_memcpy(backup_ha_ptr, ha_ptr, blk_size);
        backup_blk_hdr->signature =
                        (BCMFP_HA_STRUCT_SIGN + backup_blk_id);
        backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    }

    (*ckey) = (bcmfp_compress_key_t *)
              ((uint8_t *)ha_ptr + blk_hdr->free_blk_offset);
    (*ckey)->blk_id = blk_id;
    (*ckey)->blk_offset = blk_hdr->free_blk_offset;
    blk_hdr->free_blk_offset = (*ckey)->next_blk_offset;
    (*ckey)->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count--;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_key_node_release(int unit,
                                uint8_t stage_id,
                                uint8_t ctype,
                                bcmfp_compress_key_t *ckey)
{
    void *ha_mem = NULL;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ckey, SHR_E_PARAM);

    blk_id = ckey->blk_id;
    blk_offset = ckey->blk_offset;

    sal_memset(ckey, 0, sizeof(bcmfp_compress_key_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));

    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    ckey->next_blk_offset = blk_hdr->free_blk_offset;
    ckey->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_blk_offset = blk_offset;
    blk_hdr->free_ha_element_count++;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_UC_A;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_trie_recreate(int unit, void *ha_mem)
{
    bcmfp_compress_key_t *ckeys = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_shr_trie_md_t *trie = NULL;
    uint16_t num_ha_elements = 0;
    uint16_t idx = 0;
    uint8_t ctype = 0;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_mem, SHR_E_PARAM);

    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    ctype = blk_hdr->ctype;
    stage_id = blk_hdr->stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    num_ha_elements = (blk_hdr->blk_size -
                       sizeof(bcmfp_ha_blk_hdr_t)) /
                       sizeof(bcmfp_compress_key_t);
    trie = stage->compress_tries[ctype];
    ckeys = (bcmfp_compress_key_t *)
            ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));

    for (idx = 0; idx < num_ha_elements; idx++) {
        if (ckeys[idx].next_blk_offset !=
            BCMFP_HA_BLK_OFFSET_INVALID &&
            ckeys[idx].blk_offset ==
            BCMFP_HA_BLK_OFFSET_INVALID)  {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_key_recover(unit,
                                        &ckeys[idx],
                                        trie));
    }
exit:
    SHR_FUNC_EXIT();
}
