/*! \file bcmfp_cth_rule.c
 *
 * APIs for FP rule template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate rule config provided
 * using rule template LTs.
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
bcmfp_rule_id_get(int unit,
                  bcmfp_stage_t *stage,
                  bcmltd_field_t *buffer,
                  uint32_t *rule_id)
{
    uint32_t rule_id_fid = 0;
    uint64_t rule_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(rule_id, SHR_E_PARAM);

    if (stage->tbls.rule_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    rule_id_fid = stage->tbls.rule_tbl->key_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             rule_id_fid,
                             (void *)buffer,
                             &rule_id_u64),
            SHR_E_NOT_FOUND);

    *rule_id = rule_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_rule_compress_params_get(int unit,
                               bcmfp_stage_t *stage,
                               bcmltd_field_t *buffer,
                               bool *compress_types)
{
    uint64_t compress_type_u64 = 0;
    uint32_t compress_type_fid = 0;
    uint64_t num_compress_type_u64 = 0;
    uint32_t num_compress_type_fid = 0;
    uint32_t num_compress_types = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(compress_types, SHR_E_PARAM);

    num_compress_type_fid =
        stage->tbls.rule_tbl->num_compression_type_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             num_compress_type_fid,
                             (void *)buffer,
                             &num_compress_type_u64),
            SHR_E_NOT_FOUND);
    num_compress_types = num_compress_type_u64;

    if (num_compress_types == 0) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        compress_types[idx] = FALSE;
    }
    compress_type_fid =
        stage->tbls.rule_tbl->compression_type_array_fid;
    for (idx = 0; idx < num_compress_types; idx++) {
        compress_type_u64 = 0;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_array_value_get(unit,
                                       compress_type_fid,
                                       idx,
                                       (void *)buffer,
                                       &compress_type_u64),
                SHR_E_NOT_FOUND);
        compress_types[compress_type_u64] = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_rule_qualifier_fid_get(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_qualifier_t qualifier,
                             bcmlrd_fid_t *key_fid,
                             bcmlrd_fid_t *mask_fid)
{
    uint16_t idx = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(mask_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *key_fid = 0;
    *mask_fid = 0;
    for (idx = 0; idx < stage->tbls.group_tbl->fid_count; idx++) {
        if (qualifiers_fid_info[idx].qualifier == qualifier &&
            SHR_BITGET(qualifiers_fid_info[idx].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_DATA)) {
            *key_fid = idx;
        }
        if (qualifiers_fid_info[idx].qualifier == qualifier &&
            SHR_BITGET(qualifiers_fid_info[idx].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_MASK)) {
            *mask_fid = idx;
        }
        if (*key_fid != 0 && *mask_fid != 0) {
            break;
        }
    }

    if (idx == stage->tbls.group_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_rule_config_dump(int unit,
                  bcmltd_sid_t tbl_id,
                  bcmfp_stage_t *stage,
                  bcmfp_rule_config_t *rule_config)
{
    bcmfp_qual_data_t *qual_data = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t key_fid = 0;
    bcmlrd_fid_t mask_fid = 0;
    bcmfp_qualifier_t qualifier;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(rule_config, SHR_E_PARAM);

    if (stage->tbls.rule_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Rule ID = %d\n"),
            rule_config->rule_id));
    }

    qual_data = rule_config->qual_data;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Qualifiers Data(KEY, MASK):\n")));
    while (qual_data != NULL) {

        qualifier = qual_data->qual_id;

        /* Get the key and mask field corresponding to the qualifier. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_qualifier_fid_get(unit,
                                          stage,
                                          qualifier,
                                          &key_fid,
                                          &mask_fid));

        /* Dump the key portion of the qualifier. */
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        key_fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s="),
                   field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_data->key[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

        /* Dump the mask portion of the qualifier. */
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        mask_fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "    %s="),
                   field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_data->mask[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

        qual_data = qual_data->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_rule_qualifiers_config_get(int unit,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_t *stage,
                                 bcmltd_field_t *buffer,
                                 bcmfp_qual_data_t **qd,
                                 bcmfp_rule_config_t *config)
{
    bcmlrd_field_def_t field_def;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint32_t *data_or_mask = NULL;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_data_t *temp_data = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qual_fid_info = NULL;
    int qual_enum = 0;
    bcmdrd_pbmp_t *pbmp = NULL;
    bcmlrd_fid_t inports_key_fid = 0;
    bcmlrd_fid_t inports_mask_fid = 0;
    bcmlrd_fid_t system_ports_key_fid = 0;
    bcmlrd_fid_t system_ports_mask_fid = 0;
    bcmlrd_fid_t device_ports_key_fid = 0;
    bcmlrd_fid_t device_ports_mask_fid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(qd, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    if (stage->tbls.rule_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    qualifiers_fid_info =
        stage->tbls.rule_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *qd = NULL;
    inports_key_fid = stage->tbls.rule_tbl->inports_key_fid;
    inports_mask_fid = stage->tbls.rule_tbl->inports_mask_fid;
    system_ports_key_fid = stage->tbls.rule_tbl->system_ports_key_fid;
    system_ports_mask_fid = stage->tbls.rule_tbl->system_ports_mask_fid;
    device_ports_key_fid = stage->tbls.rule_tbl->device_ports_key_fid;
    device_ports_mask_fid = stage->tbls.rule_tbl->device_ports_mask_fid;
    while (buffer != NULL) {
        qual_fid_info =
            &(qualifiers_fid_info[buffer->id]);
        if (qual_fid_info->qualifier == 0) {
            buffer = buffer->next;
            continue;
        }

        /* Only anyone of the ports bitmap can be supported in a rule_config */
        if ((buffer->id == inports_key_fid) ||
            (buffer->id == system_ports_key_fid) ||
            (buffer->id == device_ports_key_fid)) {
            pbmp = &(config->pbmp_data);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ports_bitmap_get(unit,
                                        buffer,
                                        pbmp));
            if (buffer->id == inports_key_fid)  {
                config->pbmp_type = BCMFP_INPORTS;
                config->pbmp_qual_id =
                        stage->tbls.group_tbl->qual_inports_fid;
            } else if (buffer->id == system_ports_key_fid)  {
                config->pbmp_type = BCMFP_SYSTEM_PORTS;
                config->pbmp_qual_id =
                        stage->tbls.group_tbl->qual_system_ports_fid;
            } else if (buffer->id == device_ports_key_fid)  {
                config->pbmp_type = BCMFP_DEVICE_PORTS;
                config->pbmp_qual_id =
                        stage->tbls.group_tbl->qual_device_ports_fid;
            }

            buffer = buffer->next;
            continue;
        }

        if ((buffer->id == inports_mask_fid) ||
            (buffer->id == system_ports_mask_fid) ||
            (buffer->id == device_ports_mask_fid)) {
            pbmp = &(config->pbmp_mask);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ports_bitmap_get(unit,
                                        buffer,
                                        pbmp));
            if (buffer->id == inports_mask_fid)  {
                config->pbmp_type = BCMFP_INPORTS;
            } else if (buffer->id == system_ports_mask_fid)  {
                config->pbmp_type = BCMFP_SYSTEM_PORTS;
            } else if (buffer->id == device_ports_mask_fid)  {
                config->pbmp_type = BCMFP_DEVICE_PORTS;
            }
            buffer = buffer->next;
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, tbl_id, buffer->id, &field_def));
        if (field_def.depth == 0) {
            qual_id = qual_fid_info->qualifier;
        } else {
            qual_id = qual_fid_info->qualifier + buffer->idx;
        }

        qual_data = *qd;
        while (qual_data != NULL) {
            if (qual_data->qual_id == qual_id) {
                break;
            }
            qual_data = qual_data->next;
        }
        if (qual_data == NULL) {
            BCMFP_ALLOC(qual_data,
                        sizeof(bcmfp_qual_data_t),
                        "bcmfpRuleQualifierData");
            /*
             * Add the new node to the linked list of
             * qualifier configurations.
             */
            qual_data->next = *qd;
            *qd = qual_data;

            /* Update the qualifier. */
            qual_data->qual_id = qual_id;
            if (field_def.depth == 0) {
                qual_data->fid = qual_fid_info->qualifier;
            } else {
                qual_data->fid = qual_fid_info->fid;
            }
            if (field_def.depth == 0 && field_def.width > 64) {
                qual_data->fidx = 0;
            } else {
                qual_data->fidx = buffer->idx;
            }

            /* Update flags. */
            if (SHR_BITGET(qual_fid_info->flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS)) {
                qual_data->flags = BCMFP_QUALIFIER_FLAGS_ALL_PARTS;
            }
        }

        if (field_def.symbol == TRUE) {
            qual_enum = buffer->data & 0xFFFFFFFF;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_qualifier_enum_set(unit,
                                          stage->stage_id,
                                          qual_id,
                                          FALSE,
                                          qual_enum,
                                          qual_data->key,
                                          qual_data->mask));
        } else {
            if (SHR_BITGET(qual_fid_info->flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA)) {
                data_or_mask = &(qual_data->key[0]);
                if (SHR_BITGET(qual_fid_info->flags.w,
                    BCMFP_QUALIFIER_FID_FLAGS_UPPER)) {
                    data_or_mask = &(qual_data->key[2]);
                }
            } else {
                data_or_mask = &(qual_data->mask[0]);
                if (SHR_BITGET(qual_fid_info->flags.w,
                    BCMFP_QUALIFIER_FID_FLAGS_UPPER)) {
                    data_or_mask = &(qual_data->mask[2]);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_tbl_field_value_get(unit,
                                           buffer->data,
                                           field_def.width,
                                           buffer->idx,
                                           data_or_mask));
        }
        buffer = buffer->next;
    }

exit:
    if (SHR_FUNC_ERR()) {
        while (qual_data != NULL) {
            temp_data = qual_data;
            qual_data = qual_data->next;
            SHR_FREE(temp_data);
        }
        *qd = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_idp_rule_count_get(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_rule_id_t rule_id,
                               void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info->entry_or_group_count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_config_get(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      bcmfp_rule_config_t **config)
{
    uint32_t rule_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bool *compress_types = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.rule_tbl == NULL ||
        stage->tbls.rule_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(rule_config,
                sizeof(bcmfp_rule_config_t),
                "bcmfpRuleConfig");
    *config = rule_config;

    /* Get the rule id */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_id_get(unit,
                           stage,
                           buffer,
                           &rule_id));
    rule_config->rule_id = rule_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get if the rule compression types. */
    if (stage->tbls.rule_tbl->compression_type_array_fid != 0 &&
        stage->tbls.rule_tbl->num_compression_type_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        compress_types = rule_config->compress_types;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_compress_params_get(unit,
                                            stage,
                                            buffer,
                                            compress_types));
    }

    /* Get the qualifiers config */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_qualifiers_config_get(unit,
                                          tbl_id,
                                          stage,
                                          buffer,
                                          &qual_data,
                                          rule_config));
    rule_config->qual_data = qual_data;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_remote_config_get(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_rule_id_t rule_id,
                             bcmfp_rule_config_t **config)
{
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.rule_tbl->key_fid;
    tbl_id = stage->tbls.rule_tbl->sid;
    num_fid = stage->tbls.rule_tbl->lrd_info.num_fid;

    /* Get the FP rule LT entry saved in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &rule_id,
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

    /* Get the FP rule LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage->stage_id,
                                key,
                                data,
                                &rule_config));
    *config = rule_config;
    rule_config = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_rule_config_free(unit, rule_config);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_config_free(int unit,
           bcmfp_rule_config_t *rule_config)
{
    bcmfp_qual_data_t *qual_data = NULL;

    SHR_FUNC_ENTER(unit);

    if (rule_config == NULL) {
        SHR_EXIT();
    }

    qual_data = rule_config->qual_data;
    while (qual_data != NULL) {
        rule_config->qual_data = qual_data->next;
        SHR_FREE(qual_data);
        qual_data = rule_config->qual_data;
    }

    SHR_FREE(rule_config);

exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_rule_compress_keys_free(int unit,
                         bcmfp_rule_compress_key_t *ckeys)
{
    bcmfp_rule_compress_key_t *ckey = NULL;

    ckey = ckeys;
    while (ckey != NULL) {
        ckeys = ckey->next;
        SHR_FREE(ckey);
        ckey = ckeys;
    }
}

int
bcmfp_rule_non_compress_qualifiers_match(int unit,
                                   bcmfp_rule_config_t *rconfig1,
                                   bcmfp_rule_config_t *rconfig2,
                                   bool *matched)
{
    bcmfp_qual_data_t *qual_data1 = NULL;
    bcmfp_qual_data_t *qual_data2 = NULL;
    bool match_found = TRUE;
    size_t size = 0;
    int16_t num_qual_data_matched = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rconfig1, SHR_E_PARAM);
    SHR_NULL_CHECK(rconfig2, SHR_E_PARAM);
    SHR_NULL_CHECK(matched, SHR_E_PARAM);

    *matched = TRUE;
    qual_data1 = rconfig1->qual_data;
    size = (sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    while (qual_data1 != NULL) {
        if (qual_data1->compress == TRUE) {
            qual_data1 = qual_data1->next;
            continue;
        }
        match_found = FALSE;
        qual_data2 = rconfig2->qual_data;
        while (qual_data2 != NULL) {
            if (qual_data1->qual_id !=
                qual_data2->qual_id) {
                qual_data2 = qual_data2->next;
                continue;
            }

            if (sal_memcmp(qual_data1->key,
                           qual_data2->key,
                           size)) {
                break;
            }
            if (sal_memcmp(qual_data1->mask,
                           qual_data2->mask,
                           size)) {
                break;
            }
            num_qual_data_matched++;
            match_found = TRUE;
            break;
        }
        if (match_found == FALSE) {
           break;
        }
        qual_data1 = qual_data1->next;
    }

    if (match_found == FALSE) {
        *matched = FALSE;
    }

    qual_data2 = rconfig2->qual_data;
    while (qual_data2 != NULL) {
        if (qual_data2->compress == FALSE) {
            num_qual_data_matched--;
        }
        if (num_qual_data_matched < 0) {
            *matched = FALSE;
            break;
        }
        qual_data2 = qual_data2->next;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_compress_qualifiers_mark(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    bcmfp_rule_config_t *rconfig)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_tbl_compress_fid_info_t *cfid_info = NULL;
    uint32_t fid = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    uint8_t idx1 = 0;
    uint8_t idx2 = 0;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bool *rcompress_types = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rconfig, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    qualifiers_fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;
    rcompress_types = rconfig->compress_types;
    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (rcompress_types[idx1] == FALSE) {
            continue;
        }
        cfid_info = stage->tbls.compress_fid_info[idx1];
        for (idx2 = 0; idx2 < cfid_info->num_fids; idx2++) {
            fid = cfid_info->key_fids[idx2];
            qual_id = qualifiers_fid_info[fid].qualifier;
            qual_data = rconfig->qual_data;
            while (qual_data != NULL) {
                if (qual_id == qual_data->qual_id) {
                    qual_data->compress = TRUE;
                    break;
                }
                qual_data = qual_data->next;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_compress_keys_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_rule_config_t *rconfig,
                             bcmfp_rule_compress_key_t **ckeys)
{
    bool *gcompress_types = NULL;
    bool *rcompress_types = NULL;
    bcmfp_tbl_compress_fid_info_t *cfid_info = NULL;
    bcmfp_rule_compress_key_t *ckey = NULL;
    bcmfp_rule_compress_key_t *prev_ckey = NULL;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;
    uint8_t idx1 = 0;
    uint8_t idx2 = 0;
    int dst_sbit = 0;
    int src_sbit = 0;
    int count = 0;
    int offset = 0;
    int width = 0;
    uint32_t fid = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rconfig, SHR_E_PARAM);
    SHR_NULL_CHECK(ckeys, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_compress_types_get(unit,
                                        stage_id,
                                        group_id,
                                        &gcompress_types));
    rcompress_types = rconfig->compress_types;
    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (rcompress_types[idx1] == TRUE) {
            if (gcompress_types[idx1] != TRUE) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    qualifiers_fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;
    size = sizeof(bcmfp_rule_compress_key_t);
    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (rcompress_types[idx1] == FALSE) {
            continue;
        }
        cfid_info = stage->tbls.compress_fid_info[idx1];
        dst_sbit = 0;
        BCMFP_ALLOC(ckey, size, "bcmfpRuleCompressKey");
        if (prev_ckey != NULL) {
            prev_ckey->next = ckey;
            prev_ckey = ckey;
        }
        if (*ckeys == NULL) {
            *ckeys = ckey;
            prev_ckey = ckey;
        }
        /*
         * Copy all compress FID key and mask values into
         * single key and mask buffers.
         */
        for (idx2 = 0; idx2 < cfid_info->num_fids; idx2++) {
            fid = cfid_info->key_fids[idx2];
            src_sbit = cfid_info->fid_offsets[idx2];
            width = cfid_info->fid_widths[idx2];
            qual_data = rconfig->qual_data;
            qual_id = qualifiers_fid_info[fid].qualifier;
            while (qual_data != NULL) {
                if (qual_id == qual_data->qual_id) {
                    qual_data->compress = TRUE;
                    break;
                }
                qual_data = qual_data->next;
            }
            if (qual_data != NULL) {
                SHR_BITCOPY_RANGE(ckey->key,
                                  dst_sbit,
                                  qual_data->key,
                                  src_sbit,
                                  width);
                SHR_BITCOPY_RANGE(ckey->mask,
                                  dst_sbit,
                                  qual_data->mask,
                                  src_sbit,
                                  width);
            }
            dst_sbit += width;
        }
        for (idx2 = 0; idx2 < dst_sbit; idx2++) {
            if (SHR_BITGET(ckey->mask, idx2)) {
                break;
            }
        }
        offset = idx2;
        width = (dst_sbit - idx2);
        SHR_BITCOUNT_RANGE(ckey->mask, count, offset, width);
        if (count != width) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        ckey->prefix = count;
        ckey->cfid_info = cfid_info;
        ckey->key_size = dst_sbit;
        ckey->trie = stage->compress_tries[idx1];
        ckey = NULL;
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_rule_compress_keys_free(unit, *ckeys);
        *ckeys = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_config_compress(int unit,
                           bcmfp_qual_data_t *cid_qual_data,
                           bcmfp_rule_config_t *rule_config)
{
    bcmfp_qual_data_t *temp_qual_data = NULL;
    bcmfp_qual_data_t *curr_qual_data = NULL;
    bcmfp_qual_data_t *prev_qual_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rule_config, SHR_E_PARAM);
    SHR_NULL_CHECK(cid_qual_data, SHR_E_PARAM);

    /*
     * Delete all the compress eligible qualifiers
     * from rule config.
     */
    curr_qual_data = rule_config->qual_data;
    while (curr_qual_data != NULL) {
        if (curr_qual_data->compress == TRUE) {
            if (prev_qual_data != NULL) {
                prev_qual_data->next =
                    curr_qual_data->next;
            } else {
                rule_config->qual_data =
                    curr_qual_data->next;
            }
            temp_qual_data = curr_qual_data;
            curr_qual_data = curr_qual_data->next;
            SHR_FREE(temp_qual_data);
            temp_qual_data = NULL;
        } else {
            prev_qual_data = curr_qual_data;
            curr_qual_data = curr_qual_data->next;
        }
    }

    /*
     * Add the all CID and CID_MASK pairs to rule
     * configuration.
     */
    prev_qual_data = NULL;
    curr_qual_data = cid_qual_data;
    while (curr_qual_data != NULL) {
        prev_qual_data = curr_qual_data;
        curr_qual_data = curr_qual_data->next;
    }

    if (prev_qual_data != NULL) {
        prev_qual_data->next = rule_config->qual_data;
        rule_config->qual_data = cid_qual_data;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
               RULE CONFIG VALIDATE FUNCTIONS
 */
int
bcmfp_rule_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_rule_id_t rule_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmltd_field_t *in_key = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_id_get(unit, stage, in_key, &rule_id));

    /* Process entries associated to this rule. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_check(unit,
                                    stage_id,
                                    rule_id,
                                    &not_mapped));

    /* Return if no entries are associated to the rule */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

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

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &rule_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_config_dump(unit,
                                    tbl_id,
                                    stage,
                                    rule_config));
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_RULE_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_rule_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));
    cb_func = bcmfp_entry_idp_rule_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));
exit:
    bcmfp_rule_config_free(unit, rule_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_rule_id_t rule_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_id_get(unit, stage, in_key, &rule_id));

    /* Process entries associated to this rule. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_check(unit,
                                    stage_id,
                                    rule_id,
                                    &not_mapped));

    /* Return if no entries are associated to the rule */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.rule_tbl->lrd_info.num_fid;
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
        (bcmfp_rule_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &rule_config));

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_RULE_TEMPLATE;
    idp_info.key = key;
    idp_info.data = updated_data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_UPDATE;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_rule_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));

    cb_func = bcmfp_entry_idp_rule_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));
exit:
    bcmfp_rule_config_free(unit, rule_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_rule_config_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_rule_id_t rule_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmltd_field_t *in_key = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_id_get(unit, stage, in_key, &rule_id));

    /* Process entries associated to this rule. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_check(unit,
                                    stage_id,
                                    rule_id,
                                    &not_mapped));

    /* Return if no entries are associated to the rule */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              data,
                              &rule_config));

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_RULE_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_rule_count_get;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));
    cb_func = bcmfp_entry_idp_rule_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_rule_entry_map_traverse(unit,
                                       stage_id,
                                       rule_id,
                                       cb_func,
                                       user_data));
exit:
    bcmfp_rule_config_free(unit, rule_config);
    SHR_FUNC_EXIT();
}
