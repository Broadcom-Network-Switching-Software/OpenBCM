/*! \file bcmfp_entry.c
 *
 * APIs to operate on FP entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_cth_group.h>
#include <bcmfp/bcmfp_cth_entry.h>
#include <bcmfp/bcmfp_cth_policy.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmdrd/bcmdrd_field.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

typedef struct bcmfp_entry_buffer_s {
     uint32_t entry_key_words[BCMFP_ENTRY_WORDS_MAX];
     uint32_t entry_data_words[BCMFP_ENTRY_WORDS_MAX];
} bcmfp_entry_buffer_t;

static bcmfp_entry_buffer_t req_entry_buffer[BCMDRD_CONFIG_MAX_UNITS]
                                            [BCMFP_ENTRY_PARTS_MAX];
static bcmfp_entry_buffer_t rsp_entry_buffer[BCMDRD_CONFIG_MAX_UNITS]
                                            [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *req_ekw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *req_edw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *rsp_ekw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *rsp_edw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

int
bcmfp_em_entry_build_ekw(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_group_oper_info_t *opinfo,
                         bcmfp_entry_id_t entry_id,
                         bcmfp_group_config_t *group_config,
                         bcmfp_rule_config_t *rule_config,
                         uint32_t **ekw)
{
    uint32_t *bitmap = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_buffers_t buffers;
    bcmfp_key_ids_t key_ids;
    bcmfp_pdd_config_t *pdd_config = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(group_config, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (rule_config)  {
        qual_data = rule_config->qual_data;
        while (qual_data != NULL) {
            qual_bitmap = group_config->qual_bitmap;
            while (qual_bitmap != NULL) {
                if ((qual_bitmap->qual ==
                            qual_data->qual_id) &&
                        qual_bitmap->partial == TRUE) {
                    bitmap = qual_bitmap->bitmap;
                    break;
                }
                qual_bitmap = qual_bitmap->next;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_qual_set(unit,
                                stage_id,
                                group_id,
                                opinfo,
                                qual_data->flags,
                                qual_data->qual_id,
                                qual_data->fid,
                                qual_data->fidx,
                                bitmap,
                                qual_data->key,
                                qual_data->mask,
                                ekw));
            qual_data = qual_data->next;
            qual_bitmap = NULL;
        }
    }

    sal_memset(&key_ids, 0, sizeof(bcmfp_key_ids_t));
    sal_memset(&buffers, 0, sizeof(bcmfp_buffers_t));
    key_ids.group_id = group_id;
    key_ids.pdd_id = group_config->pdd_id;
    buffers.ekw = ekw;
    if (key_ids.pdd_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_remote_config_get(unit,
                                         op_arg->trans_id,
                                         stage_id,
                                         key_ids.pdd_id,
                                         &pdd_config));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_buffers_customize(unit,
                                 op_arg,
                                 BCMFP_BUFFERS_CUSTOMIZE_EKW,
                                 stage,
                                 opinfo,
                                 pdd_config,
                                 &key_ids,
                                 &buffers));
exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_hash_same_key_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_group_oper_info_t *opinfo,
                              uint64_t req_flags,
                              bcmltd_sid_t req_ltid,
                              void *pt_dyn_info,
                              bcmptm_keyed_lt_mreq_info_t *req_info,
                              bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags,
                              uint8_t *same_key)

{
    int rv;
    uint32_t **ekw = NULL;
    uint32_t wsize = 0;
    int i = 0;
    uint32_t group_id = 0;
    uint32_t rule_id = 0;
    uint8_t part = 0;
    uint8_t parts_count = 0;
    bool new_key_exists = FALSE;
    bool old_key_exists  = FALSE;
    uint32_t new_key_entry_index = 0;
    uint32_t old_key_entry_index = 0;
    uint32_t rsp_flags2 = 0;
    uint16_t *field_sbit = NULL, *field_width = NULL;
    bcmltd_sid_t  rsp_ltid2 = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmlrd_hw_field_list_t *hw_key_fields = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmptm_rm_hash_entry_attrs_t *hash_attrs = NULL;
    bcmptm_rm_hash_dynamic_entry_attrs_t *dyn_attrs = NULL;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    size = sizeof(bcmptm_rm_hash_entry_attrs_t);
    BCMFP_ALLOC(hash_attrs, size, "bcmfpRmHashEntryAttrs");
    size = sizeof(bcmptm_keyed_lt_mreq_info_t);
    BCMFP_ALLOC(lt_mreq_info, size, "bcmptmKeyedLtMreqInfo");
    size = sizeof(bcmptm_keyed_lt_mrsp_info_t);
    BCMFP_ALLOC(lt_mrsp_info, size, "bcmptmKeyedLtMrspInfo");
    size = sizeof(bcmptm_pt_op_info_t);
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                 sizeof(bcmptm_pt_op_info_t),
                 "bcmptmPtOpInfoInBcmFpEntryInsert");

    /* Allocate Memory for entry Key word and data word */
    size = (sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX);
    BCMFP_ALLOC(ekw, size, "bcmfpEkwPart");

    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
        BCMFP_ALLOC(ekw[i], size, "bcmfpEkwWord");
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        size = sizeof(bcmptm_rm_hash_dynamic_entry_attrs_t);
        BCMFP_ALLOC(dyn_attrs, size, "bcmfpRmHashDynEntryAttrs");
        hash_attrs->d_entry_attrs = dyn_attrs;

        size = sizeof(bcmlrd_hw_field_list_t);
        BCMFP_ALLOC(hw_key_fields, size, "bcmfpRmHashDynEntryHwKeyFields");
        dyn_attrs->hw_key_fields = hw_key_fields;
    }

    /*
     *  Lookup for the key with new ekw
     *  if found
     *  Lookup with old ekw/edw
     *  if not found
     *  return ERROR
     *  else
     *  if the entry_index of old and new keys are same
     *  continue with same_key=1
     *  else
     *  return entry_exist error
     *  else if not found
     *  Lookup for key with old ekw
     *  if not found
     *  insert with same key=0
     *  else if found
     *  delete the entry with the old ekw index
     */

    /* Lookup up for the new EKW/EDW */
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    rv = bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  req_info,
                                  op_arg->trans_id,
                                  rsp_info,
                                  rsp_ltid,
                                  rsp_flags);
    if (SHR_SUCCESS(rv)) {
        new_key_exists = TRUE;
        new_key_entry_index = rsp_info->pt_op_info->rsp_entry_index[0];
    }

    (void)bcmfp_entry_remote_config_get(unit,
                                        op_arg,
                                        stage_id,
                                        entry_id,
                                        &entry_config);
    if (entry_config != NULL) {
        group_id = entry_config->group_id;
        (void)bcmfp_group_remote_config_get(unit,
                                            op_arg,
                                            stage_id,
                                            group_id,
                                            &group_config);
        rule_id = entry_config->rule_id;
        (void)bcmfp_rule_remote_config_get(unit,
                                           op_arg,
                                           stage_id,
                                           rule_id,
                                           &rule_config);
    }
    if (group_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_em_entry_build_ekw(unit,
                                      op_arg,
                                      req_ltid,
                                      stage_id,
                                      group_id,
                                      opinfo,
                                      entry_id,
                                      group_config,
                                      rule_config,
                                      ekw));

        group_oper_info =  opinfo;
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE
            (group_oper_info, hash_attrs->entry_mode);

        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            ext_codes = &(group_oper_info->ext_codes[0]);
            dyn_attrs->bucket_mode = 0;
            dyn_attrs->entry_size = ext_codes->num_base_entries;
            dyn_attrs->hw_key_fields->num_fields = ext_codes->num_key_fields;

            size = (ext_codes->num_key_fields * sizeof(uint16_t));
            BCMFP_ALLOC(field_width, size, "bcmfpRmHashDynFldWidth");
            BCMFP_ALLOC(field_sbit, size, "bcmfpRmHashDynFldSbit");

            for (i = 0; i < ext_codes->num_key_fields; i++) {
                field_width[i] = ext_codes->key_field_width[i];
                field_sbit[i] = ext_codes->key_field_sbit[i];
            }

            hw_key_fields->field_width = field_width;
            hw_key_fields->field_start_bit = field_sbit;
        }

        lt_mreq_info->entry_attrs = hash_attrs;
        lt_mreq_info->entry_id = entry_id;
        lt_mreq_info->pdd_l_dtyp = NULL;
        lt_mreq_info->same_key = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_tcam_size_get(unit,
                                       FALSE,
                                       stage_id,
                                       group_id,
                                       group_oper_info,
                                       &wsize));
        lt_mreq_info->rsp_entry_key_buf_wsize = wsize;

        wsize = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_policy_size_get(unit,
                                         FALSE,
                                         stage_id,
                                         group_id,
                                         group_oper_info,
                                         &wsize));
        lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_parts_count(unit,
                                     FALSE,
                                     group_oper_info->flags,
                                     &parts_count));

        for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
            req_edw[unit][part] =
                req_entry_buffer[unit][part].entry_data_words;
            rsp_ekw[unit][part] =
                rsp_entry_buffer[unit][part].entry_key_words;
            rsp_edw[unit][part] =
                rsp_entry_buffer[unit][part].entry_data_words;
        }
        lt_mreq_info->entry_key_words = ekw;
        lt_mreq_info->entry_data_words = req_edw[unit];
        lt_mrsp_info->rsp_entry_key_words = rsp_ekw[unit];
        lt_mrsp_info->rsp_entry_data_words = rsp_edw[unit];
        lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
        lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

        rv = bcmptm_ltm_mreq_keyed_lt(unit,
                                      req_flags,
                                      req_ltid,
                                      pt_dyn_info,
                                      NULL,
                                      BCMPTM_OP_LOOKUP,
                                      lt_mreq_info,
                                      op_arg->trans_id,
                                      lt_mrsp_info,
                                      &rsp_ltid2,
                                      &rsp_flags2);
        if (SHR_SUCCESS(rv)) {
            old_key_exists = TRUE;
            if (lt_mrsp_info->pt_op_info != NULL)
            {
                old_key_entry_index = lt_mrsp_info->pt_op_info->rsp_entry_index[0];
            }
        }
    }
    if (old_key_exists) {
        if (new_key_exists) {
            if (new_key_entry_index == old_key_entry_index)  {
                /*
                 * On updating the data portion of the entry. Key calculated
                 * from the the new and old ekw is same.
                 */
                *same_key = TRUE;
            } else  {
                /*
                 * On updating the rule/group portion of the entry. The new key
                 * conflicts with some other entry's key.
                 */
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                          "BCMFP: EM hash key %d already exists "
                          "for the entry being inserted\n"),
                          new_key_entry_index));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
        } else {
            /*
             * Here we need to delete the entry from the older index and
             * insert in new index.
             */
            SHR_ERR_EXIT
                (bcmptm_ltm_mreq_keyed_lt(unit,
                                          req_flags,
                                          req_ltid,
                                          pt_dyn_info,
                                          NULL,
                                          BCMPTM_OP_DELETE,
                                          lt_mreq_info,
                                          op_arg->trans_id,
                                          lt_mrsp_info,
                                          &rsp_ltid2,
                                          &rsp_flags2));
        }
    } else {
        if (new_key_exists) {
            /* The Key already exists for the entry being inserted. */
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "BCMFP: EM hash key %d already exists "
                      "for the entry being inserted\n"),
                      new_key_entry_index));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FREE(hash_attrs);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    if (entry_config != NULL) {
        SHR_FREE(entry_config);
    }
    (void)(bcmfp_group_config_free(unit, group_config));
    (void)(bcmfp_rule_config_free(unit, rule_config));
    if (ekw != NULL) {
        for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            SHR_FREE(ekw[i]);
        }
        SHR_FREE(ekw);
    }
    if (stage != NULL &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        SHR_FREE(dyn_attrs);
        SHR_FREE(hw_key_fields);
        SHR_FREE(field_width);
        SHR_FREE(field_sbit);
    }

    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_entry_insert(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       int tbl_inst,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_entry_id_t entry_id,
                       uint32_t entry_prio,
                       uint32_t **ekw,
                       uint32_t **edw,
                       bcmfp_entry_profiles_t *ent_prof,
                       bool enable)
{
    bool is_presel = 0;
    int i = 0;
    uint8_t part = 0;
    uint8_t same_key = FALSE;
    uint8_t parts_count = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    bcmltd_sid_t rsp_ltid;
    bcmfp_stage_t *stage = NULL;
    uint16_t *field_sbit = NULL, *field_width = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmlrd_hw_field_list_t *hw_key_fields = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_hash_entry_attrs_t *hash_attrs = NULL;
    bcmptm_rm_hash_dynamic_entry_attrs_t *dyn_attrs = NULL;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpEntryInsert");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmptmPtOpInfoInBcmFpEntryInsert");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpEntryInsert");
    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmptmRmTcamEntryAttrsInBcmFpEntryInsert");
    BCMFP_ALLOC(hash_attrs,
                sizeof(bcmptm_rm_hash_entry_attrs_t),
                "bcmptmRmHashEntryAttrsInBcmFpEntryInsert");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_oper_info = opinfo;
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        BCMFP_ALLOC(dyn_attrs,
                    sizeof(bcmptm_rm_hash_dynamic_entry_attrs_t),
                    "bcmfpRmHashDynEntryAttrs");
        hash_attrs->d_entry_attrs = dyn_attrs;

        BCMFP_ALLOC(hw_key_fields,
                    sizeof(bcmlrd_hw_field_list_t),
                    "bcmfpRmHashDynEntryHwKeyFields");
        dyn_attrs->hw_key_fields = hw_key_fields;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 group_oper_info->flags,
                                 &parts_count));

    /* for EM hash entry, we need to use hash entry attrs */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))
        || (flags & BCMFP_ENTRY_PRESEL)) {
        if (flags & BCMFP_ENTRY_PRESEL) {
            is_presel = 1;
            SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
            if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
                SHR_BITSET(attrs->flags.w,
                           BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
            }
        }
        /*
         * In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM,
         * this range is mapped to 0-<MaxId-1>.
         */
        attrs->group_id = group_oper_info->group_id - 1;

        attrs->group_prio = group_oper_info->group_prio;

        attrs->group_ltid = group_oper_info->action_res_id;

        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(group_oper_info->group_mode,
                                              attrs->group_mode);

        attrs->num_presel_ids = group_oper_info->num_presel_ids;

        BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

       if (ent_prof != NULL && ent_prof->pdd != NULL &&
           BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD)) {

           if ((flags & BCMFP_ENTRY_FIRST_IN_GROUP &&
                group_oper_info->flags & BCMFP_GROUP_WITH_PDD_PROFILE)
                || (flags & BCMFP_ENTRY_WITH_PDD_PROFILE)) {

               for (part = 0; part < parts_count; part++) {
                   sal_memcpy(attrs->pdd_profile_info[part],
                              ent_prof->pdd[part],
                              BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
               }

               if (flags & BCMFP_ENTRY_WITH_PDD_PROFILE) {
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_PDD_PROF_PER_ENTRY);
               } else {
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_PDD_PROF_PER_GROUP);
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_FIRST_ENTRY_IN_GROUP);
               }
           }
       }

       if (ent_prof != NULL && ent_prof->sbr != NULL &&
           (BCMFP_STAGE_FLAGS_TEST(stage,
                  BCMFP_STAGE_ACTION_RESOLUTION_SBR))) {

           if ((flags & BCMFP_ENTRY_FIRST_IN_GROUP &&
               (group_oper_info->flags & BCMFP_GROUP_WITH_SBR_PROFILE))
                || (flags & BCMFP_ENTRY_WITH_SBR_PROFILE)) {
               uint8_t sbr_sid_columns = 0;
               sbr_sid_columns = stage->sbr_hw_info->sbr_profile_sid_columns;
               for (part = 0; part < (parts_count * sbr_sid_columns); part++) {
                   sal_memcpy(attrs->sbr_profile_info[part],
                              ent_prof->sbr[part],
                              BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
               }

               if (flags & BCMFP_ENTRY_WITH_SBR_PROFILE) {
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_SBR_PROF_PER_ENTRY);
               } else {
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_SBR_PROF_PER_GROUP);
                   SHR_BITSET(attrs->flags.w,
                              BCMPTM_FP_FLAGS_FIRST_ENTRY_IN_GROUP);
               }
           }
       }

       if (ent_prof != NULL && ent_prof->psg != NULL &&
           (BCMFP_STAGE_FLAGS_TEST(stage,
                  BCMFP_STAGE_PRESEL_KEY_DYNAMIC))) {
           if (flags & BCMFP_ENTRY_FIRST_IN_GROUP &&
               (group_oper_info->flags &
                BCMFP_GROUP_WITH_DYNAMIC_PRESEL)) {
               for (part = 0; part < parts_count; part++) {
                   sal_memcpy(attrs->presel_group_info[part],
                              ent_prof->psg[part],
                              BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
               }
               SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_GROUP);
           }
       }

       if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_SELCODE) &&
          ( flags & BCMFP_ENTRY_FIRST_IN_GROUP)) {
          SHR_IF_ERR_VERBOSE_EXIT
               (bcmfp_group_selcode_get(unit,
                                        stage,
                                        group_oper_info,
                                        attrs->selcode));
        }

        if (flags & BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL) {
            attrs->pipe_id = -1;
        } else {
            attrs->pipe_id = tbl_inst;
        }

        /* for exact match presel entry update the slice id in attrs */
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            attrs->slice_id = group_oper_info->slice_id;
        }
        attrs->trans_id = op_arg->trans_id;
        lt_mreq_info->entry_attrs = attrs;
    } else {
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE
            (group_oper_info, hash_attrs->entry_mode);

        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            ext_codes = &(group_oper_info->ext_codes[0]);
            dyn_attrs->bucket_mode = 0;
            dyn_attrs->entry_size = ext_codes->num_base_entries;
            dyn_attrs->hw_key_fields->num_fields = ext_codes->num_key_fields;

            BCMFP_ALLOC(field_width,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldWidth");
            BCMFP_ALLOC(field_sbit,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldSbit");

            for (i = 0; i < ext_codes->num_key_fields; i++) {
                field_width[i] = ext_codes->key_field_width[i];
                field_sbit[i] = ext_codes->key_field_sbit[i];
            }

            hw_key_fields->field_width = field_width;
            hw_key_fields->field_start_bit = field_sbit;
        }

        lt_mreq_info->entry_attrs = hash_attrs;
    }

    lt_mreq_info->entry_id = entry_id;
    lt_mreq_info->entry_pri = entry_prio;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    if (flags & BCMFP_ENTRY_SAME_KEY) {
        lt_mreq_info->same_key = 1;
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    if (flags & BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL) {
        req_flags = BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_valid_bit_set(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   group_oper_info,
                                   ekw,
                                   enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_tcam_size_get(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   group_oper_info,
                                   &wsize));
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    wsize = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_policy_size_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     group_oper_info,
                                     &wsize));
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        rsp_ekw[unit][part] = rsp_entry_buffer[unit][part].entry_key_words;
        rsp_edw[unit][part] = rsp_entry_buffer[unit][part].entry_data_words;
    }
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw[unit];
    lt_mrsp_info->rsp_entry_data_words = rsp_edw[unit];
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = tbl_inst;

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH) &&
        !(flags & BCMFP_ENTRY_PRESEL)) {

        /* Return with entry exists error
         * if key is conflciting with another
         * entry key.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_hash_same_key_get(unit,
                                           op_arg,
                                           stage_id,
                                           entry_id,
                                           group_oper_info,
                                           req_flags,
                                           req_ltid,
                                           (void *)&pt_dyn_info,
                                           lt_mreq_info,
                                           lt_mrsp_info,
                                           &rsp_ltid,
                                           &rsp_flags,
                                           &same_key));
        if (same_key == TRUE) {

            /* If the updated entry key conflicts
             * with same entry key, continue with
             * same_key set to 1
             */
            lt_mreq_info->same_key = 1;
        }
    }

    if (group_oper_info->group_slice_type == BCMFP_GROUP_SLICE_TYPE_SMALL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_USE_SMALL_SLICE);
    }
    if (group_oper_info->group_slice_type == BCMFP_GROUP_SLICE_TYPE_LARGE) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_USE_LARGE_SLICE);
    }
    if (group_oper_info->flags & BCMFP_GROUP_WITH_AUTO_EXPAND) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_AUTO_EXPAND_ENABLE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_INSERT,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));

    /* Update slice id in group oper info */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_FIXED) &&
       (flags & BCMFP_ENTRY_FIRST_IN_GROUP)) {
        group_oper_info->slice_id = attrs->slice_id;
    }

    /* update pdd profile info in group oper info */
    if ((flags & BCMFP_ENTRY_FIRST_IN_GROUP)
        && (group_oper_info->flags & BCMFP_GROUP_WITH_PDD_PROFILE)
        && !BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)
        && !BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PDD_PROFILES_GLOBAL)
        && (!(flags & BCMFP_ENTRY_SAME_KEY))) {

        for (part = 0; part < parts_count; part++) {
            group_oper_info->ext_codes[part].pdd_prof_index =
                       attrs->pdd_profile_indexes[part];
        }
    }

    /* update SBR profile info in group oper info */
    if ((flags & BCMFP_ENTRY_FIRST_IN_GROUP)
        && (group_oper_info->flags & BCMFP_GROUP_WITH_SBR_PROFILE)
        && !BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)
        && (!(flags & BCMFP_ENTRY_SAME_KEY))) {

        for (part = 0; part < parts_count; part++) {
            group_oper_info->ext_codes[part].sbr_prof_index =
                       attrs->sbr_profile_indexes[part];
        }
    }
    /* update Presel group info in group oper info */
    if ((flags & BCMFP_ENTRY_FIRST_IN_GROUP)
        && (group_oper_info->flags & BCMFP_GROUP_WITH_DYNAMIC_PRESEL)
        && !BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)
        && (!(flags & BCMFP_ENTRY_SAME_KEY))) {

        for (part = 0; part < parts_count; part++) {
            group_oper_info->ext_codes[part].psg_prof_index =
                       attrs->presel_group_indexes[part];
        }
    }

exit:
    SHR_FREE(attrs);
    SHR_FREE(hash_attrs);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FREE(lt_mrsp_info);
    if (stage != NULL &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        SHR_FREE(dyn_attrs);
        SHR_FREE(hw_key_fields);
        SHR_FREE(field_width);
        SHR_FREE(field_sbit);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_entry_delete(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_entry_id_t entry_id,
                       uint32_t **ekw,
                       bcmfp_entry_profiles_t *ent_prof)
{
    bool is_presel = 0;
    int i = 0;
    uint8_t part = 0;
    uint8_t parts_count = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint16_t *field_sbit = NULL, *field_width = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmlrd_hw_field_list_t *hw_key_fields = NULL;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_hash_entry_attrs_t *hash_attrs = NULL;
    bcmptm_rm_hash_dynamic_entry_attrs_t *dyn_attrs = NULL;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_flag_t stage_flag = 0;
    int start_bit = 0;
    int end_bit = 0;
    bcmfp_group_mode_t group_mode;
    uint32_t *temp_rsp_edw = NULL;
    uint32_t *temp_rsp_ekw = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpEntryDelete");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpEntryDelete");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmptmPtOpInfoInBcmFpEntryDelete");
    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmptmRmTcamEntryAttrsInBcmFpEntryDelete");
    BCMFP_ALLOC(hash_attrs,
                sizeof(bcmptm_rm_hash_entry_attrs_t),
                "bcmptmRmHashEntryAttrsInBcmFpEntryDelete");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    group_oper_info = opinfo;
    group_mode = opinfo->group_mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 group_oper_info->flags,
                                 &parts_count));

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        BCMFP_ALLOC(dyn_attrs,
                    sizeof(bcmptm_rm_hash_dynamic_entry_attrs_t),
                    "bcmfpRmHashDynEntryAttrs");
        hash_attrs->d_entry_attrs = dyn_attrs;

        BCMFP_ALLOC(hw_key_fields,
                    sizeof(bcmlrd_hw_field_list_t),
                    "bcmfpRmHashDynEntryHwKeyFields");
        dyn_attrs->hw_key_fields = hw_key_fields;
    }

    /* for EM hash entry, need to use hash entry attrs */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
        && !(flags & BCMFP_ENTRY_PRESEL))) {
        if (flags & BCMFP_ENTRY_PRESEL) {
            SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
            if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
                SHR_BITSET(attrs->flags.w,
                           BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
            }
        }

        if (flags & BCMFP_ENTRY_LAST_IN_GROUP) {

            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP);
            if (group_oper_info->flags & BCMFP_GROUP_WITH_PDD_PROFILE) {

                for (part = 0; part < parts_count; part++) {
                    attrs->pdd_profile_indexes[part] =
                            group_oper_info->ext_codes[part].pdd_prof_index;
                }
                SHR_BITSET(attrs->flags.w,
                           BCMPTM_FP_FLAGS_PDD_PROF_PER_GROUP);
            }
            if (group_oper_info->flags & BCMFP_GROUP_WITH_SBR_PROFILE) {

                for (part = 0; part < parts_count; part++) {
                    attrs->sbr_profile_indexes[part] =
                            group_oper_info->ext_codes[part].sbr_prof_index;
                }
                SHR_BITSET(attrs->flags.w,
                           BCMPTM_FP_FLAGS_SBR_PROF_PER_GROUP);
            }
            if (group_oper_info->flags & BCMFP_GROUP_WITH_DYNAMIC_PRESEL) {
                for (part = 0; part < parts_count; part++) {
                    attrs->presel_group_indexes[part] =
                            group_oper_info->ext_codes[part].psg_prof_index;
                }
                SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_GROUP);
            }
        }

        /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM, this range
         * is mapped to 0-<MaxId-1>  */
        attrs->group_id = group_oper_info->group_id - 1;

        attrs->group_prio = group_oper_info->group_prio;

        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(group_oper_info->group_mode,
                                              attrs->group_mode);

        attrs->num_presel_ids = group_oper_info->num_presel_ids;

        BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

        attrs->pipe_id = group_oper_info->tbl_inst;
        attrs->trans_id = op_arg->trans_id;
        lt_mreq_info->entry_attrs = attrs;

        if (flags & BCMFP_ENTRY_PRESEL) {
            is_presel = 1;
        }
    } else {
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE
            (group_oper_info, hash_attrs->entry_mode);

        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            ext_codes = &(group_oper_info->ext_codes[0]);
            dyn_attrs->bucket_mode = 0;
            dyn_attrs->entry_size = ext_codes->num_base_entries;
            dyn_attrs->hw_key_fields->num_fields = ext_codes->num_key_fields;

            BCMFP_ALLOC(field_width,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldWidth");
            BCMFP_ALLOC(field_sbit,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldSbit");

            for (i = 0; i < ext_codes->num_key_fields; i++) {
                field_width[i] = ext_codes->key_field_width[i];
                field_sbit[i] = ext_codes->key_field_sbit[i];
            }

            hw_key_fields->field_width = field_width;
            hw_key_fields->field_start_bit = field_sbit;
        }

        lt_mreq_info->entry_attrs = hash_attrs;
    }

    lt_mreq_info->entry_id = entry_id;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_tcam_size_get(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   group_oper_info,
                                   &wsize));
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_policy_size_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     group_oper_info,
                                     &wsize));
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        req_ekw[unit][part] = req_entry_buffer[unit][part].entry_key_words;
        req_edw[unit][part] = req_entry_buffer[unit][part].entry_data_words;
        rsp_ekw[unit][part] = rsp_entry_buffer[unit][part].entry_key_words;
        rsp_edw[unit][part] = rsp_entry_buffer[unit][part].entry_data_words;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
        && !(flags & BCMFP_ENTRY_PRESEL)) {
        lt_mreq_info->entry_key_words = ekw;
    } else {
        lt_mreq_info->entry_key_words = req_ekw[unit];
    }
    lt_mreq_info->entry_data_words = req_edw[unit];
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw[unit];
    lt_mrsp_info->rsp_entry_data_words = rsp_edw[unit];
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = group_oper_info->tbl_inst;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_DELETE,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));

    stage_flag = BCMFP_STAGE_ACTION_QOS_PROFILE;
    if (BCMFP_STAGE_FLAGS_TEST(stage, stage_flag) &&
        !(flags & BCMFP_ENTRY_PRESEL)) {
        bcmdrd_sid_t em_entry_sid = 0;
        bcmdrd_fid_t qos_prof_fid = 0;
        bcmfp_group_mode_t mode = 0;
        uint32_t index = 0;
        uint32_t pflags = 0;
        em_entry_sid =
            lt_mrsp_info->pt_op_info->rsp_entry_sid[0];
        mode = group_oper_info->group_mode;
        qos_prof_fid =
            stage->key_data_size_info->qos_prof_id_fid[mode];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_get(unit,
                                 em_entry_sid,
                                 rsp_edw[unit][0],
                                 qos_prof_fid,
                                 &index));
        group_oper_info->ext_codes[0].qos_prof_index = index;
        group_oper_info->ext_codes[1].qos_prof_index = index;
        pflags = BCMFP_GROUP_INSTALL_QOS_PROFILE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profiles_uninstall(unit,
                                      op_arg,
                                      pflags,
                                      stage,
                                      group_id,
                                      opinfo));
    }

    /*
     * Retrive the SBR index from response buffer as entry to SBR
     * index is not saved in anywhere. This is required to unistall
     * the SBR profile associated to the entry.
     */
    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) {
        stage_flag = BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE;
        if (BCMFP_STAGE_FLAGS_TEST(stage, stage_flag) &&
            !(flags & BCMFP_ENTRY_PRESEL)) {
            temp_rsp_edw =
                lt_mrsp_info->rsp_entry_data_words[parts_count -1];
            start_bit = (stage->pdd_hw_info->raw_policy_width -
                        stage->misc_info->per_entry_sbr_raw_data_offset);
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            bcmdrd_field_get(temp_rsp_edw,
                             start_bit,
                             end_bit,
                             &(ent_prof->sbr_index));
        }

        /* for HASH LTs the sbr_index is part of ekw */
        if (BCMFP_STAGE_FLAGS_TEST(stage,BCMFP_STAGE_ENTRY_TYPE_HASH) &&
            !(flags & BCMFP_ENTRY_PRESEL)) {
            temp_rsp_ekw =
                lt_mrsp_info->rsp_entry_data_words[0];
            start_bit = stage->key_data_size_info->data_start_offset[group_mode] + 1 -
                        stage->misc_info->per_entry_sbr_raw_data_offset;
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            bcmdrd_field_get(temp_rsp_ekw,
                             start_bit,
                             end_bit,
                             &(ent_prof->sbr_index));
        }
    }
exit:
    SHR_FREE(attrs);
    SHR_FREE(hash_attrs);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    if (stage!= NULL &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        SHR_FREE(dyn_attrs);
        SHR_FREE(hw_key_fields);
        SHR_FREE(field_width);
        SHR_FREE(field_sbit);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_entry_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_entry_id_t entry_id,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers)
{
    int i = 0;
    bool is_presel = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint16_t *field_sbit = NULL, *field_width = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmlrd_hw_field_list_t *hw_key_fields = NULL;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_hash_entry_attrs_t *hash_attrs = NULL;
    bcmptm_rm_hash_dynamic_entry_attrs_t *dyn_attrs = NULL;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpEntryLookup");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpEntryLookup");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmptmPtOpInfoInBcmFpEntryLookup");
    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmptmRmTcamEntryAttrsInBcmFpEntryLookup");
    BCMFP_ALLOC(hash_attrs,
                sizeof(bcmptm_rm_hash_entry_attrs_t),
                "bcmptmRmHashEntryAttrsInBcmFpEntryLookup");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        BCMFP_ALLOC(dyn_attrs,
                    sizeof(bcmptm_rm_hash_dynamic_entry_attrs_t),
                    "bcmfpRmHashDynEntryAttrs");
        hash_attrs->d_entry_attrs = dyn_attrs;

        BCMFP_ALLOC(hw_key_fields,
                    sizeof(bcmlrd_hw_field_list_t),
                    "bcmfpRmHashDynEntryHwKeyFields");
        dyn_attrs->hw_key_fields = hw_key_fields;
    }

    /* for EM hash entry, need to use hash entry attrs */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
        && !(flags & BCMFP_ENTRY_PRESEL))) {
        if (flags & BCMFP_ENTRY_PRESEL) {
            SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
            if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
                SHR_BITSET(attrs->flags.w,
                           BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
            }
        }

        /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM,
         * this range is mapped to 0-<MaxId-1>.
         */
        attrs->group_id = group_id - 1;
        attrs->group_prio = opinfo->group_prio;
        attrs->num_presel_ids = opinfo->num_presel_ids;

        BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);
        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(opinfo->group_mode,
                                              attrs->group_mode);

        attrs->pipe_id = opinfo->tbl_inst;
        attrs->trans_id = op_arg->trans_id;
        lt_mreq_info->entry_attrs = attrs;

        if (flags & BCMFP_ENTRY_PRESEL) {
            is_presel = 1;
        }
    } else {
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE
            (opinfo, hash_attrs->entry_mode);
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            ext_codes = &(opinfo->ext_codes[0]);
            dyn_attrs->bucket_mode = 0;
            dyn_attrs->entry_size = ext_codes->num_base_entries;
            dyn_attrs->hw_key_fields->num_fields = ext_codes->num_key_fields;

            BCMFP_ALLOC(field_width,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldWidth");
            BCMFP_ALLOC(field_sbit,
                        (ext_codes->num_key_fields * sizeof(uint16_t)),
                        "bcmfpRmHashDynFldSbit");

            for (i = 0; i < ext_codes->num_key_fields; i++) {
                field_width[i] = ext_codes->key_field_width[i];
                field_sbit[i] = ext_codes->key_field_sbit[i];
            }

            hw_key_fields->field_width = field_width;
            hw_key_fields->field_start_bit = field_sbit;
        }

        lt_mreq_info->entry_attrs = hash_attrs;
    }

    lt_mreq_info->entry_id = entry_id;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_tcam_size_get(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   opinfo,
                                   &wsize));
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_policy_size_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     opinfo,
                                     &wsize));
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = opinfo->tbl_inst;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    if (flags & BCMFP_ENTRY_READ_FROM_HW) {
        req_flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(attrs);
    SHR_FREE(hash_attrs);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    if (stage != NULL &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        SHR_FREE(dyn_attrs);
        SHR_FREE(hw_key_fields);
        SHR_FREE(field_width);
        SHR_FREE(field_sbit);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_group_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    bool is_presel = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_GROUP_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM,
     * this range is mapped to 0-<MaxId-1>.
     */
    attrs->group_id = group_id - 1;

    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

    attrs->pipe_id = opinfo->tbl_inst;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;

    lt_mreq_info->entry_id = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_tcam_size_get(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   opinfo,
                                   &wsize));
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_policy_size_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     opinfo,
                                     &wsize));
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = opinfo->tbl_inst;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_stage_entry_limit_get(int unit,
                       uint32_t trans_id,
                       bcmltd_sid_t req_ltid,
                       uint32_t *count)
{
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t size = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw1 = NULL;
    uint32_t **rsp_edw1 = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    uint32_t *rsp_ekw_ew = NULL;
    uint32_t *rsp_edw_ew = NULL;
    bcmltd_table_entry_limit_t *table_data = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmEntryLimitGet");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmEntryLimitGet");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpEntryLimitEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpEntryLimitEdw");
    BCMFP_ALLOC(rsp_ekw1, sizeof(uint32_t *), "bcmfpEntryLimitRspEkw");
    BCMFP_ALLOC(rsp_edw1, sizeof(uint32_t *), "bcmfpEntryLimitRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpEntryLimitEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpEntryLimitEdwEntryWords");
    BCMFP_ALLOC(rsp_ekw_ew, size, "bcmfpEntryLimitRspEkwEntryWords");
    BCMFP_ALLOC(rsp_edw_ew, size, "bcmfpEntryLimitRspEdwEntryWords");

    BCMFP_ALLOC(table_data,
                sizeof(bcmltd_table_entry_limit_t),
                "bcmfpEntryLimitTableData");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw1[0] = rsp_ekw_ew;
    rsp_edw1[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(table_data, 0, sizeof(bcmltd_table_entry_limit_t));

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw1;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw1;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->rsp_misc = table_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  &pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_GET_TABLE_INFO,
                                  lt_mreq_info,
                                  trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));

    *count = table_data->entry_limit;

exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw1);
    SHR_FREE(rsp_edw1);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(rsp_ekw_ew);
    SHR_FREE(rsp_edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(table_data);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_stage_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_STAGE_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    attrs->group_id = -1;
    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);
    attrs->pipe_id = -1;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;
    lt_mreq_info->entry_id = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = 0;
    lt_mreq_info->rsp_entry_data_buf_wsize = 0;
    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_slice_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t slice_id,
                       int pipe_id,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_SLICE_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    attrs->group_id = -1;
    attrs->slice_num = slice_id;

    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

    attrs->pipe_id = pipe_id;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;
    lt_mreq_info->entry_id = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = 0;
    lt_mreq_info->rsp_entry_data_buf_wsize = 0;
    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}
