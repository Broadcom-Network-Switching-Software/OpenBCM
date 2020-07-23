/*! \file bcmfp_qual_cfg.c
 *
 * Utility APIs to operate with FP qualifers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_qual_cfg_t_init(int unit, bcmfp_qual_cfg_t *qual_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_cfg, SHR_E_PARAM);

    sal_memset(qual_cfg, 0, sizeof(bcmfp_qual_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qualifier_t qual_id,
                      bcmfp_qual_cfg_t *new_qual_cfg)
{
    bool qual_cfg_info_db_alloc = 0;
    bool qual_cfg_info_alloc = 0;
    uint8_t chunk = 0;
    uint8_t num_qual_cfg = 0;
    bcmfp_qual_cfg_t *qual_cfg = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_qual_cfg, SHR_E_PARAM);

    qual_cfg_info_db = stage->qual_cfg_info_db;

    if (qual_cfg_info_db == NULL) {
        BCMFP_ALLOC(qual_cfg_info_db,
                    sizeof(bcmfp_qual_cfg_info_db_t),
                    "bcmfpQualCfgDb");
        stage->qual_cfg_info_db = qual_cfg_info_db;
        qual_cfg_info_db_alloc = 1;
    }

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_qual_cfg_info_t),
                    "bcmfpQualCfgInfo");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info->qual = qual_id;
        qual_cfg_info_alloc = 1;
    }

    num_qual_cfg = qual_cfg_info->num_qual_cfg + 1;

    BCMFP_ALLOC(qual_cfg_arr,
                num_qual_cfg * sizeof(bcmfp_qual_cfg_t),
                "bcmfpQualCfgArray");

    sal_memcpy(qual_cfg_arr,
               qual_cfg_info->qual_cfg_arr,
               sizeof(bcmfp_qual_cfg_t) * (num_qual_cfg - 1));

    qual_cfg = &(qual_cfg_arr[num_qual_cfg - 1]);

    for (chunk = 0; chunk < new_qual_cfg->num_chunks; chunk++) {
        if (BCMFP_EXT_SECTION_L1E32 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 32;
        } else if (BCMFP_EXT_SECTION_L1E16 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 16;
        } else if (BCMFP_EXT_SECTION_L1E8 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 8;
        } else if (BCMFP_EXT_SECTION_L1E4 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 4;
        } else if (BCMFP_EXT_SECTION_L1E2 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 2;
        }
    }

    sal_memcpy(qual_cfg, new_qual_cfg, sizeof(bcmfp_qual_cfg_t));

    SHR_FREE(qual_cfg_info->qual_cfg_arr);

    qual_cfg_info->qual_cfg_arr = qual_cfg_arr;
    qual_cfg_info->num_qual_cfg++;

    SHR_FUNC_EXIT();

exit:
    SHR_FREE(qual_cfg_arr);
    if (qual_cfg_info_db_alloc) {
        SHR_FREE(qual_cfg_info_db);
    }
    if (qual_cfg_info_alloc) {
        SHR_FREE(qual_cfg_info);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage)
{
    bcmfp_qualifier_t qual = 0;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;
    bcmfp_keygen_qual_cfg_info_t *kgn_qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_info_db_t *kgn_qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qual_cfg_info_db = stage->qual_cfg_info_db;
    if (qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUALIFIERS_COUNT; qual++) {

            qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual];
            if (qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(qual_cfg_info->qual_cfg_arr);
            SHR_FREE(qual_cfg_info);
            qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(qual_cfg_info_db);
    }
    stage->qual_cfg_info_db = NULL;

    qual_cfg_info_db = stage->presel_qual_cfg_info_db;
    if (qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUALIFIERS_COUNT; qual++) {

            qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual];
            if (qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(qual_cfg_info->qual_cfg_arr);
            SHR_FREE(qual_cfg_info);
            qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(qual_cfg_info_db);
        stage->presel_qual_cfg_info_db = NULL;
    }
    stage->presel_qual_cfg_info_db = NULL;

    kgn_qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
    if (kgn_qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUALIFIERS_COUNT; qual++) {

            kgn_qual_cfg_info = kgn_qual_cfg_info_db->qual_cfg_info[qual];
            if (kgn_qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(kgn_qual_cfg_info->qual_cfg_arr);
            SHR_FREE(kgn_qual_cfg_info);
            kgn_qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(kgn_qual_cfg_info_db);
    }
    stage->kgn_qual_cfg_info_db = NULL;

    kgn_qual_cfg_info_db = stage->kgn_presel_qual_cfg_info_db;
    if (kgn_qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUALIFIERS_COUNT; qual++) {

            kgn_qual_cfg_info = kgn_qual_cfg_info_db->qual_cfg_info[qual];
            if (kgn_qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(kgn_qual_cfg_info->qual_cfg_arr);
            SHR_FREE(kgn_qual_cfg_info);
            kgn_qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(kgn_qual_cfg_info_db);
    }
    stage->kgn_presel_qual_cfg_info_db = NULL;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_set(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_group_id_t group_id,
               bcmfp_group_oper_info_t *opinfo,
               bcmfp_qualifier_flags_t flags,
               bcmfp_qualifier_t qual,
               bcmltd_fid_t fid,
               uint16_t fidx,
               uint32_t *bitmap,
               uint32_t *data,
               uint32_t *mask,
               uint32_t **ekw)
{
    bool       is_presel = 0;
    uint8_t    miss_count = 0;
    uint8_t    parts_cnt = 0;
    uint8_t    part_idx = 0;
    int16_t    get_start_bit = -1;
    uint16_t   set_start_bit = 0;
    uint16_t   key_start_bit = 0;
    uint16_t   mask_start_bit = 0;
    uint16_t   key_end_bit = 0;
    uint16_t   mask_end_bit = 0;
    uint16_t   idx = 0;
    uint16_t   width = 0;
    uint16_t   chunk_size[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    uint16_t   chunk_offset[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    uint32_t   start_offset = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_data_t out_data1;
    bcmfp_data_t out_mask1;
    bcmfp_data_t actual_data;
    bcmfp_data_t actual_mask;
    bcmfp_data_t qual_bitmap;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    /* Retrieve group entry parts */
    rv = bcmfp_group_parts_count(unit, FALSE, opinfo->flags, &parts_cnt);
    SHR_IF_ERR_EXIT(rv);

    miss_count = 0;
    get_start_bit = -1;
    set_start_bit = 0;
    width = 0;
    BCMFP_ALLOC(q_offset, sizeof(bcmfp_qual_offset_info_t),
                "bcmfpQualOffsetInfo");
    /* Update each chunk size of the qualifier. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));
        /* Retrieve the Qualifier offsets for the particular
         * entry part.
         */
        rv = bcmfp_group_qual_offset_info_get(unit,
                                              stage_id,
                                              group_id,
                                              opinfo,
                                              FALSE,
                                              part_idx,
                                              fid,
                                              fidx,
                                              q_offset);

        if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            return rv;
        }

        for (idx = 0; idx < q_offset->num_offsets; idx++) {
            if (q_offset->width[idx]) {
                chunk_size[idx] = q_offset->width[idx];
            }
        }
    }
    /* Update each chunk offset of the qualifier. offsets in q_offset
     * are local to the partition but chunk_offset array will have
     * offsets across the partitions.
     */
    for (idx = 0; idx < BCMFP_QUAL_OFFSETS_MAX; idx++) {
        if (idx == 0) {
            chunk_offset[idx] = 0;
            continue;
        }
        if (chunk_size[idx]) {
           chunk_offset[idx] = chunk_offset[idx-1] + chunk_size[idx - 1];
        } else {
           break;
        }
    }

    /* Get the actual dat and mask after removing the
     * unwanted bits.
     */
    BCMFP_QUAL_DATA_CLEAR(actual_data);
    BCMFP_QUAL_DATA_CLEAR(actual_mask);
    BCMFP_QUAL_DATA_CLEAR(qual_bitmap);
    if (bitmap == NULL) {
        SHR_IF_ERR_EXIT(
            bcmfp_group_qual_bitmap_get(unit,
                                        stage_id,
                                        group_id,
                                        qual,
                                        qual_bitmap));
    } else {
        sal_memcpy(qual_bitmap, bitmap,
            sizeof(uint32_t) * BCMFP_QUAL_BITMAP_MAX_WORDS);
    }
    for (idx = 0; idx < BCMFP_QUAL_BITMAP_MAX_WORDS * 32; idx++) {
        if (SHR_BITGET(qual_bitmap, idx)) {
           if (-1 == get_start_bit) {
              get_start_bit = idx;
           }
           width++;
        } else {
           if (-1 != get_start_bit) {

              BCMFP_QUAL_DATA_CLEAR(out_data1);
              BCMFP_QUAL_DATA_CLEAR(out_mask1);

              bcmdrd_field_get(data,
                               get_start_bit,
                               get_start_bit + width - 1,
                               out_data1);

              bcmdrd_field_set(actual_data,
                               set_start_bit,
                               set_start_bit + width - 1,
                               out_data1);

              bcmdrd_field_get(mask,
                               get_start_bit,
                               get_start_bit + width - 1,
                               out_mask1);

              bcmdrd_field_set(actual_mask,
                               set_start_bit,
                               set_start_bit + width - 1,
                               out_mask1);

              set_start_bit += width;
              get_start_bit = -1;
              width = 0;
           }
        }
    }

    /* Update the ekw. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));

        if (flags == BCMFP_QUALIFIER_FLAGS_ALL_PARTS) {
            rv = bcmfp_group_qual_offset_info_get(unit,
                                                  stage_id,
                                                  group_id,
                                                  opinfo,
                                                  FALSE,
                                                  0,
                                                  fid,
                                                  fidx,
                                                  q_offset);
        } else {
            rv = bcmfp_group_qual_offset_info_get(unit,
                                                  stage_id,
                                                  group_id,
                                                  opinfo,
                                                  FALSE,
                                                  part_idx,
                                                  fid,
                                                  fidx,
                                                  q_offset);
        }
        if (rv == SHR_E_NOT_FOUND) {
            miss_count += 1;
            continue;
        } else if (SHR_FAILURE(rv)) {
           return rv;
        }
        start_offset = 0;
        SHR_IF_ERR_EXIT(
            bcmfp_entry_key_info_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     opinfo,
                                     &key_start_bit,
                                     &key_end_bit));
        SHR_IF_ERR_EXIT(
            bcmfp_entry_mask_info_get(unit,
                                      is_presel,
                                      stage_id,
                                      group_id,
                                      opinfo,
                                      &mask_start_bit,
                                      &mask_end_bit));
        /* Extract relevent data for this partion from
         * user passed data.
         */
        for (idx = 0; idx < q_offset->num_offsets; idx++) {
            if (0 == q_offset->width[idx]) {
                continue;
            }
            BCMFP_QUAL_DATA_CLEAR(out_data1);
            BCMFP_QUAL_DATA_CLEAR(out_mask1);

            bcmdrd_field_get(actual_data,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_data1);

            bcmdrd_field_set(ekw[part_idx],
                             (key_start_bit +
                              q_offset->offset[idx]),
                             (key_start_bit +
                              q_offset->offset[idx] +
                              q_offset->width[idx] - 1),
                             out_data1);

            bcmdrd_field_get(actual_mask,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_mask1);
            if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {
                bcmdrd_field_set(ekw[part_idx],
                             (mask_start_bit +
                              q_offset->offset[idx]),
                             (mask_start_bit +
                              q_offset->offset[idx] +
                              q_offset->width[idx] - 1),
                             out_mask1);
            }
            start_offset += q_offset->width[idx];
        }
    }

    if (miss_count == parts_cnt) {
        return SHR_E_NOT_FOUND;
    }

exit:
    SHR_FREE(q_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_data_size_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual_id,
                         uint32_t *size)
{
    uint16_t qual_cfg_idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_t *kgn_qual_cfg_arr = NULL;
    bcmfp_keygen_qual_cfg_info_t *kgn_qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        kgn_qual_cfg_info = stage->kgn_qual_cfg_info_db->qual_cfg_info[qual_id];
        kgn_qual_cfg_arr = kgn_qual_cfg_info->qual_cfg_arr;
        *size = 0;
        for (qual_cfg_idx = 0;
            qual_cfg_idx < kgn_qual_cfg_info->num_qual_cfg;
            qual_cfg_idx++) {
            if (*size < kgn_qual_cfg_arr[qual_cfg_idx].size) {
                *size = kgn_qual_cfg_arr[qual_cfg_idx].size;
            }
        }
    } else {
        qual_cfg_info = stage->qual_cfg_info_db->qual_cfg_info[qual_id];
        qual_cfg_arr = qual_cfg_info->qual_cfg_arr;
        *size = 0;
        for (qual_cfg_idx = 0;
            qual_cfg_idx < qual_cfg_info->num_qual_cfg;
            qual_cfg_idx++) {
            if (*size < qual_cfg_arr[qual_cfg_idx].size) {
                *size = qual_cfg_arr[qual_cfg_idx].size;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
