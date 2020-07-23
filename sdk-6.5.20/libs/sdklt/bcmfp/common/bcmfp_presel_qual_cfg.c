/*! \file bcmfp_presel_qual_cfg.c
 *
 * Utility APIs to operate with FP preselection qualifers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_qual_presel_dynamic_key_set(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  bcmfp_qualifier_t qual,
                                  bcmltd_fid_t fid,
                                  uint16_t fidx,
                                  uint32_t *bitmap,
                                  uint32_t *data,
                                  uint32_t *mask,
                                  uint32_t **ekw)
{
    bool is_presel = TRUE;
    uint8_t part;
    uint8_t num_parts = 0;
    uint16_t idx;
    uint16_t start_bit;
    uint16_t end_bit;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    uint16_t chunk_size[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    uint16_t chunk_offset[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    bcmfp_data_t out_data1;
    bcmfp_data_t out_mask1;
    bcmfp_data_t actual_data;
    bcmfp_data_t actual_mask;
    int16_t get_start_bit = -1;
    uint16_t set_start_bit = 0;
    uint16_t width = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));
    BCMFP_ALLOC(q_offset,
                sizeof(bcmfp_qual_offset_info_t),
                "bcmfpPreselQualOffsetInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_qual_offset_info_get(unit,
                                          stage_id,
                                          group_id,
                                          opinfo,
                                          TRUE,
                                          0,
                                          fid,
                                          fidx,
                                          q_offset));
    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        if (q_offset->width[idx]) {
            chunk_size[idx] = q_offset->width[idx];
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

    BCMFP_QUAL_DATA_CLEAR(actual_data);
    BCMFP_QUAL_DATA_CLEAR(actual_mask);
    for (idx = 0; idx < BCMFP_QUAL_BITMAP_MAX_WORDS * 32; idx++) {
        if (SHR_BITGET(bitmap, idx)) {
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

    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        for (part = 0; part < num_parts; part++) {

            BCMFP_QUAL_DATA_CLEAR(out_data1);
            BCMFP_QUAL_DATA_CLEAR(out_mask1);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_key_info_get(unit,
                                         is_presel,
                                         stage_id,
                                         group_id,
                                         opinfo,
                                         &start_bit,
                                         &end_bit));

            bcmdrd_field_get(actual_data,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_data1);

            start_bit += q_offset->offset[idx];
            end_bit = start_bit + q_offset->width[idx] - 1;

            bcmdrd_field_set(ekw[part], start_bit, end_bit, out_data1);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_entry_mask_info_get(unit,
                                          is_presel,
                                          stage_id,
                                          group_id,
                                          opinfo,
                                          &start_bit,
                                          &end_bit));

            bcmdrd_field_get(actual_mask,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_mask1);

            start_bit += q_offset->offset[idx];
            end_bit = start_bit + q_offset->width[idx] - 1;
            bcmdrd_field_set(ekw[part], start_bit, end_bit, out_mask1);
        }
    }

exit:
    SHR_FREE(q_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_qual_cfg_t_init(int unit, bcmfp_qual_cfg_t *presel_qual_cfg)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_qual_cfg, SHR_E_PARAM);

    sal_memset(presel_qual_cfg, 0, sizeof(bcmfp_qual_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qualifier_t qual_id,
                      bcmfp_qual_cfg_t *new_presel_qual_cfg)
{
    bool qual_cfg_info_db_alloc = 0;
    bool qual_cfg_info_alloc = 0;
    uint8_t num_qual_cfg = 0;
    bcmfp_qual_cfg_t *qual_cfg = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_presel_qual_cfg, SHR_E_PARAM);

    qual_cfg_info_db = stage->presel_qual_cfg_info_db;

    if (qual_cfg_info_db == NULL) {
        BCMFP_ALLOC(qual_cfg_info_db,
                    sizeof(bcmfp_qual_cfg_info_db_t),
                    "bcmfpQualCfgDb");
        stage->presel_qual_cfg_info_db = qual_cfg_info_db;
        qual_cfg_info_db_alloc = 1;
    }

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_qual_cfg_info_t),
                    "bcmfpQualCfgDbInfo");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info_alloc = 1;
    }

    num_qual_cfg = qual_cfg_info->num_qual_cfg + 1;

    BCMFP_ALLOC(qual_cfg_arr,
                num_qual_cfg * sizeof(bcmfp_qual_cfg_t),
                "bcmfpQualCfg");

    sal_memcpy(qual_cfg_arr,
               qual_cfg_info->qual_cfg_arr,
               sizeof(bcmfp_qual_cfg_t) * (num_qual_cfg - 1));

    qual_cfg = &(qual_cfg_arr[num_qual_cfg - 1]);

    sal_memcpy(qual_cfg, new_presel_qual_cfg, sizeof(bcmfp_qual_cfg_t));

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
bcmfp_presel_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage)
{
    bcmfp_qualifier_t qual = 0;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

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
    }

    stage->presel_qual_cfg_info_db = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_presel_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_group_oper_info_t *opinfo,
                      bcmfp_qualifier_t qual,
                      bcmltd_fid_t fid,
                      uint16_t fidx,
                      uint32_t *bitmap,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw)
{
    bool is_presel = TRUE;
    uint8_t part;
    uint8_t entry_parts = 0;
    uint16_t idx;
    uint16_t start_bit;
    uint16_t end_bit;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_qual_presel_dynamic_key_set(unit,
                                              stage_id,
                                              group_id,
                                              opinfo,
                                              qual,
                                              fid,
                                              fidx,
                                              bitmap,
                                              data,
                                              mask,
                                              ekw));
        SHR_FUNC_EXIT();
    }

    qual_cfg =
        stage->presel_qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr;
    SHR_IF_ERR_EXIT((qual_cfg == NULL) ? SHR_E_NOT_FOUND : SHR_E_NONE);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &entry_parts));

    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {

        for (part = 0; part < entry_parts; part++) {

            SHR_IF_ERR_EXIT(
                bcmfp_entry_key_info_get(unit,
                                         is_presel,
                                         stage_id,
                                         group_id,
                                         opinfo,
                                         &start_bit,
                                         &end_bit));

            start_bit += qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_set(ekw[part], start_bit, end_bit, data);

            SHR_IF_ERR_EXIT(
                bcmfp_entry_mask_info_get(unit,
                                          is_presel,
                                          stage_id,
                                          group_id,
                                          opinfo,
                                          &start_bit,
                                          &end_bit));

            start_bit += qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_set(ekw[part], start_bit, end_bit, mask);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_presel_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_qualifier_t qual_id,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw)
{
    int start_bit;
    int end_bit;
    uint8_t part;
    uint8_t entry_parts = 0;
    uint16_t idx;
    uint32_t group_flags = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    /* Retrieve group entry parts */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 group_flags,
                                 &entry_parts));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit,
                         stage_id,
                         &stage));

    qual_cfg = stage->presel_qual_cfg_info_db->qual_cfg_info[qual_id]->qual_cfg_arr;
    SHR_IF_ERR_EXIT((qual_cfg == NULL) ? SHR_E_NOT_FOUND : SHR_E_NONE);

    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {

        for (part = 0; part < entry_parts; part++) {

            start_bit = qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_get(ekw[part], start_bit, end_bit, data);

            
        }
    }

exit:
    SHR_FUNC_EXIT();
}


