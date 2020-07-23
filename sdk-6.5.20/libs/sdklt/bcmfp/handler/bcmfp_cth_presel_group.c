/*! \file bcmfp_cth_presel_group.c
 *
 * APIs for FP psg template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate psg config provided
 * using psg template LTs.
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
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_qual_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_psg_id_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   uint32_t *group_id)
{
    uint32_t group_id_fid = 0;
    uint64_t group_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(group_id, SHR_E_PARAM);

    if (stage->tbls.psg_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_id_fid = stage->tbls.psg_tbl->key_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_value_get(unit,
                             group_id_fid,
                             (void *)buffer,
                             &group_id_u64));

    *group_id = group_id_u64 & 0xFFFFFFFF;
exit:
    SHR_FUNC_EXIT();
}


static int
bcmfp_psg_qualifiers_bitmap_get(int unit,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        bcmfp_qual_bitmap_t **qb)
{
    bool is_partial = FALSE;
    uint32_t *bitmap = NULL;
    bcmfp_qual_bitmap_t *bitmap_one = NULL;
    uint8_t idx = 0;
    uint16_t width = 0;
    const bcmlrd_field_data_t *field = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(qb, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.psg_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *qb = NULL;
    while (buffer != NULL) {
        if (qualifiers_fid_info[buffer->id].qualifier == 0) {
            buffer = buffer->next;
            continue;
        }
        BCMFP_ALLOC(bitmap_one,
                    sizeof(bcmfp_qual_bitmap_t),
                    "bcmfpGroupQualBitmap");
        /*
         * Add the new node to the linked list of
         * qualifiers bitmap.
         */
        bitmap_one->next = *qb;
        *qb = bitmap_one;

        /* Update the qualifier. */
        bitmap_one->qual =
            qualifiers_fid_info[buffer->id].qualifier + buffer->idx;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_get(unit, tbl_id, buffer->id, &field));

        /* Is qualifier bitmap is partial. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_value_is_partial(unit,
                                        field->width,
                                        buffer->data,
                                        &is_partial));
        bitmap_one->partial = is_partial;

        bitmap = bitmap_one->bitmap;
        /* Get the qualifier bitmap if it is partial. */
        if (SHR_BITGET(qualifiers_fid_info[buffer->id].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_MATCH_ID)) {
            qual_cfg_info_db = stage->kgn_presel_qual_cfg_info_db;
            qual_cfg_info = qual_cfg_info_db->qual_cfg_info[buffer->id];
            qual_cfg = &(qual_cfg_info->qual_cfg_arr[0]);
            for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
                width += qual_cfg->e_params[idx].width;
            }
            SHR_BITSET_RANGE(bitmap, 0, width);
         } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_tbl_field_value_get(unit,
                                           buffer->data,
                                           field->width,
                                           buffer->idx,
                                           bitmap));
        }
        bitmap_one->fid = buffer->id;
        bitmap_one->fidx = buffer->idx;
        bitmap_one = NULL;
        buffer = buffer->next;
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_qual_bitmap_free(unit, *qb);
        *qb = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_psg_qualifier_fid_get(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_qualifier_t qualifier,
                              bcmlrd_fid_t *fid)
{
    uint16_t idx = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.psg_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < stage->tbls.psg_tbl->fid_count; idx++) {
        if (qualifiers_fid_info[idx].qualifier == qualifier) {
            *fid = idx;
            break;
        }
    }

    if (idx == stage->tbls.psg_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_psg_config_dump(int unit,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_psg_config_t *group_config)
{
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;
    int idx = 0;
    bcmfp_qualifier_t qualifier;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.psg_tbl == NULL ||
        stage->tbls.psg_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (stage->tbls.psg_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Presel group ID = %d\n"),
            group_config->psg_id));
    }

    qual_bitmap = group_config->qual_bitmap;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Qualifier bitmaps:\n")));
    while (qual_bitmap != NULL) {
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        qualifier = qual_bitmap->qual;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_qualifier_fid_get(unit,
                                           stage,
                                           qualifier,
                                           &fid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "%s = "), field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_bitmap->bitmap[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit, "\n")));
        qual_bitmap = qual_bitmap->next;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_psg_config_t **config)
{
    uint32_t group_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_psg_config_t *group_config = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.psg_tbl == NULL ||
        stage->tbls.psg_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(group_config,
                sizeof(bcmfp_psg_config_t),
                "bcmfpPreselGroupConfig");
    *config = group_config;

    /* Get the presel group id */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_id_get(unit,
                            stage,
                            buffer,
                            &group_id));
    group_config->psg_id = group_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the QSET and bitmaps of qualifiers  */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_qualifiers_bitmap_get(unit,
                                         tbl_id,
                                         stage,
                                         buffer,
                                         &qual_bitmap));
    group_config->qual_bitmap = qual_bitmap;
    qual_bitmap = NULL;
exit:
    if (SHR_FUNC_ERR()) {
        /* Handle case bcmfp_psg_qualifiers_bitmap_get return error */
        bcmfp_psg_config_free(unit,group_config);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_remote_config_get(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_psg_id_t psg_id,
                          bcmfp_psg_config_t **config)
{
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_psg_config_t *psg_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.psg_tbl->key_fid;
    tbl_id = stage->tbls.psg_tbl->sid;
    num_fid = stage->tbls.psg_tbl->lrd_info.num_fid;

    /* Get the FP psg LT entry saved in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &psg_id,
                                1,
                                num_fid,
                                &in,
                                &out,
                                &(stage->imm_buffers)));

    /*
     * If not found found in IMM return with no error but
     * config is set to NULL.
     */
    if (SHR_FUNC_ERR() && SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        SHR_EXIT();
    } else if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    /* Assign key and data from in and out. */
    key = in.field[0];
    if (out.count != 0) {
        data = out.field[0];
    }

    /* Get the FP psg LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage->stage_id,
                              key,
                              data,
                              &psg_config));
    *config = psg_config;
    psg_config = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_psg_config_free(unit, psg_config);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_psg_config_process(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_t *stage,
                         bcmfp_psg_config_t *psg_config)
{
    bcmfp_qualifier_t qual_id = 0;
    uint16_t idx = 0;
    size_t array_size = 0;
    bcmfp_keygen_cfg_t keygen_cfg;
    bcmfp_keygen_oper_t keygen_oper;
    bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&keygen_cfg, 0, sizeof(bcmfp_keygen_cfg_t));
    sal_memset(&keygen_oper, 0, sizeof(bcmfp_keygen_oper_t));

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(psg_config, SHR_E_PARAM);


    keygen_cfg.qual_cfg_info_db =
               stage->kgn_presel_qual_cfg_info_db;

    array_size = 0;
    qual_bitmap = psg_config->qual_bitmap;
    while (qual_bitmap != NULL) {
        array_size++;
        qual_bitmap = qual_bitmap->next;
    }

    if (array_size) {
        BCMFP_ALLOC(keygen_cfg.qual_info_arr,
                    (array_size *
                    sizeof(bcmfp_keygen_qual_info_t)),
                    "bcmfpPreselGroupQualInfoArray");
    }

    qual_bitmap = psg_config->qual_bitmap;
    qual_cfg_info_db = keygen_cfg.qual_cfg_info_db;
    idx = 0;
    while (qual_bitmap != NULL) {
        qual_id = qual_bitmap->qual;
        if (!qual_cfg_info_db->qual_cfg_info[qual_id]) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        keygen_cfg.qual_info_arr[idx].qual_id = qual_id;
        if (qual_bitmap->partial == TRUE) {
            sal_memcpy(keygen_cfg.qual_info_arr[idx].bitmap,
                       qual_bitmap->bitmap,
                       (sizeof(uint32_t) *
                        BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS));
            keygen_cfg.qual_info_arr[idx].partial = TRUE;
        } else {
            keygen_cfg.qual_info_arr[idx].partial = FALSE;
        }
        idx++;
        keygen_cfg.qual_info_count++;
        qual_bitmap = qual_bitmap->next;
    }

    keygen_cfg.mode = BCMFP_KEYGEN_MODE_SINGLE;
    keygen_cfg.num_parts = 1;
    keygen_cfg.ext_cfg_db =
        stage->kgn_presel_ext_cfg_db_arr[keygen_cfg.mode];

    if (keygen_cfg.qual_info_count) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_cfg_process(unit,
                                      &keygen_cfg,
                                      &keygen_oper));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_set(unit,
                                          stage->stage_id,
                                          psg_config->psg_id,
                                          psg_config,
                                          &keygen_oper));
exit:
    SHR_FREE(keygen_cfg.qual_info_arr);
    (void)bcmfp_keygen_oper_free(unit, 1, &keygen_oper);
    SHR_FUNC_EXIT();
}


int
bcmfp_psg_config_free(int unit,
                   bcmfp_psg_config_t *psg_config)
{
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    if (psg_config == NULL) {
        SHR_EXIT();
    }

    qual_bitmap = psg_config->qual_bitmap;
    bcmfp_qual_bitmap_free(unit, qual_bitmap);
    SHR_FREE(psg_config);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    int rv;
    bcmfp_stage_t *stage = NULL;
    bcmfp_psg_config_t *psg_config = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_psg_id_t psg_id = 0;
    bool not_mapped = FALSE;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    /*
     * Build list of fields with non default values from
     * the incoming data.
     */
    in_data = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_non_default_data_get(unit,
                                        tbl_id,
                                        in_data,
                                        &non_default_data,
                                        &(stage->imm_buffers)));

    /* Get the presel group configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &psg_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_config_dump(unit,
                                   tbl_id,
                                   stage_id,
                                   psg_config));
    }

    /*
     * Process presel group config. This involves allocating
     * mux controls for presel keygen and offsets/widths of
     * all qualifiersin presel group. And all this data will
     * be saved to HA area.
     */
    rv = bcmfp_psg_config_process(unit,
                                  op_arg,
                                  tbl_id,
                                  stage,
                                  psg_config);

    psg_id = psg_config->psg_id;

    if (rv == SHR_E_RESOURCE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_presel_group_oper_info_get(unit,
                                              stage_id,
                                              psg_id,
                                              &opinfo));
         opinfo->valid = FALSE;
         opinfo->qset_error = TRUE;
         SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /* Process group associated to this psg. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_check(unit,
                                   stage_id,
                                   psg_id,
                                   &not_mapped));

    /* Return if no groups are associated to the psg */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSG_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_traverse(unit,
                                      stage_id,
                                      psg_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));

exit:
    bcmfp_psg_config_free(unit, psg_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    int rv;
    bcmfp_stage_t *stage = NULL;
    bcmfp_psg_config_t *psg_config = NULL;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_idp_info_t group_idp_info;
    bcmfp_psg_id_t psg_id = 0;
    bool not_mapped = FALSE;
    bcmfp_group_oper_info_t *opinfo = NULL;


    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.psg_tbl->lrd_info.num_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ltd_updated_data_get(unit,
                                    tbl_id,
                                    num_fid,
                                    in_key,
                                    in_data,
                                    &updated_fields));

    updated_data =
        (const bcmltd_field_t *)(updated_fields.field[0]);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &psg_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_config_dump(unit,
                                   tbl_id,
                                   stage_id,
                                   psg_config));
    }

    /*
     * Delete the presel grp_oper_info as this will be set by
     * the psg_config_process api with the updated qualifiers
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_delete(unit,
                                             stage_id,
                                             psg_config->psg_id));
    /*
     * Process presel group config. This involves allocating
     * mux controls for presel keygen and offsets/widths of
     * all qualifiersin presel group. And all this data will
     * be saved to HA area.
     */
    rv = bcmfp_psg_config_process(unit,
                                  op_arg,
                                  tbl_id,
                                  stage,
                                  psg_config);

    psg_id = psg_config->psg_id;

    if (rv == SHR_E_RESOURCE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_presel_group_oper_info_get(unit,
                                              stage_id,
                                              psg_id,
                                              &opinfo));
         opinfo->valid = FALSE;
         opinfo->qset_error = TRUE;
         SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /* Process group associated to this psg. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_check(unit,
                                   stage_id,
                                   psg_id,
                                   &not_mapped));

    /* Return if no groups are associated to the psg */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSG_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_traverse(unit,
                                      stage_id,
                                      psg_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));
exit:
    bcmfp_psg_config_free(unit, psg_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_psg_config_t *psg_config = NULL;
    bcmfp_psg_id_t psg_id = 0;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_idp_info_t group_idp_info;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              data,
                              &psg_config));
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_config_dump(unit,
                                   tbl_id,
                                   stage_id,
                                   psg_config));
    }
    psg_id = psg_config->psg_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_delete(unit,
                                             stage_id,
                                             psg_id));
    /* Process group associated to this psg. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_check(unit,
                                   stage_id,
                                   psg_id,
                                   &not_mapped));
    /* Return if no groups are associated to the psg */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
    group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    group_idp_info.sid_type = BCMFP_SID_TYPE_PSG_TEMPLATE;
    group_idp_info.key = key;
    group_idp_info.data = data;
    group_idp_info.output_fields = output_fields;
    group_idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    group_idp_info.entry_state = state;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_traverse(unit,
                                      stage_id,
                                      psg_id,
                                      bcmfp_group_idp_process,
                                      (void *)(&group_idp_info)));

exit:
    bcmfp_psg_config_free(unit, psg_config);
    SHR_FUNC_EXIT();
}
