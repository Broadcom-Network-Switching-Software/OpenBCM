/*! \file bcmfp_lrd.c
 *
 * APIs to fetch FP related information from LRD .
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmlrd/bcmlrd_table.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_get_number_of_fields_in_table(int unit,
                                    bcmltd_sid_t tbl_id,
                                    uint32_t *num_fields)
{
    uint32_t idx;
    bcmlrd_fid_t *fid_list = NULL;
    size_t actual_fields;
    uint32_t num_to_add;
    uint32_t total_fields = 0;
    bcmlrd_field_def_t field_def;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, tbl_id, &num_fid));

    BCMFP_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fid,
        "bcmfpFieldList");

    /* Get list of fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               tbl_id,
                               num_fid,
                               fid_list,
                               &actual_fields));

    for (idx = 0; idx < actual_fields; idx++) {
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        fid_list[idx],
                                        &field_def));

        num_to_add = 0;
        if (field_def.width > 64) {
            num_to_add = field_def.width / 64;
            if ((field_def.width % 64)) {
                num_to_add += 1;
            }
        } else {
            num_to_add = 1;
        }
        if (field_def.depth > 0) {
            num_to_add = num_to_add * field_def.depth;
        }
        total_fields += num_to_add;
    }
    *num_fields = total_fields;
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}
int
bcmfp_group_mode_max_entries(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_mode_t group_mode,
                             uint32_t *max_entries)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    hw_entry_info = stage->hw_entry_info[group_mode];

    if (hw_entry_info) {
        *max_entries = hw_entry_info->max_entries;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}
int
bcmfp_max_non_conflicting_groups_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_mode_t group_mode,
                             uint32_t *max_groups)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_pt_banks_info_t banks_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(max_groups, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    hw_entry_info = stage->hw_entry_info[group_mode];

    if (hw_entry_info) {
        *max_groups = hw_entry_info->max_non_conflicting_groups;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_UFT_BANKS_DYNAMIC)) {
        sal_memset(&banks_info, 0, sizeof(bcmptm_pt_banks_info_t));
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_bank_info_get_from_lt(unit,
                                                  stage->tbls.entry_tbl->sid,
                                                  0,
                                                  &banks_info));
        *max_groups = banks_info.bank_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_tcam_size_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint32_t *size)
{
    bcmdrd_sid_t sid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    group_mode = opinfo->group_mode;
    hw_entry_info = stage->hw_entry_info[group_mode];

    if (is_presel) {
        sid = hw_entry_info->presel_sid;
    } else {
        sid = hw_entry_info->sid;
    }

    *size = bcmdrd_pt_entry_wsize(unit, sid);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_policy_size_get(int unit,
                            bool is_presel,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_group_oper_info_t *opinfo,
                            uint32_t *size)
{
    bcmdrd_sid_t sid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    group_mode = opinfo->group_mode;
    hw_entry_info = stage->hw_entry_info[group_mode];

    if (is_presel) {
        if (hw_entry_info->presel_aggr_view) {
            sid = hw_entry_info->presel_sid;
        } else {
            sid = hw_entry_info->presel_sid_data_only;
        }
    } else {
        sid = hw_entry_info->sid_data_only;
    }

    *size = bcmdrd_pt_entry_wsize(unit, sid);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_key_info_get(int unit,
                         bool is_presel,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_group_oper_info_t *opinfo,
                         uint16_t *start_bit,
                         uint16_t *end_bit)
{
    bcmdrd_sid_t sid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(end_bit, SHR_E_PARAM);
    SHR_NULL_CHECK(start_bit, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (is_presel == TRUE) {
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            group_mode = BCMFP_GROUP_MODE_SINGLE;
        } else if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            group_mode = BCMFP_GROUP_MODE_SINGLE_ASET_NARROW;
        } else {
            group_mode = BCMFP_GROUP_MODE_SINGLE;
        }
    } else {
        group_mode = opinfo->group_mode;
    }

    hw_entry_info = stage->hw_entry_info[group_mode];

    if (is_presel) {
        sid = hw_entry_info->presel_sid;
    } else {
        sid = hw_entry_info->sid;
    }
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
          && FALSE == is_presel)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_info_get(unit, sid,
                 hw_entry_info->key_fid, &finfo));

        *start_bit = finfo.minbit;
        *end_bit = finfo.maxbit;
    } else {
        if (stage->key_data_size_info != NULL) {
            *start_bit = 0;
            *end_bit = stage->key_data_size_info->key_size[group_mode] - 1;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_mask_info_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint16_t *start_bit,
                          uint16_t *end_bit)
{
    bcmdrd_sid_t sid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmdrd_sym_field_info_t finfo;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(end_bit, SHR_E_PARAM);
    SHR_NULL_CHECK(start_bit, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
        && is_presel == FALSE) {
        SHR_EXIT();
    }

    if (is_presel == TRUE) {
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            group_mode = BCMFP_GROUP_MODE_SINGLE;
        } else if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            group_mode = BCMFP_GROUP_MODE_SINGLE_ASET_NARROW;
        } else {
            group_mode = BCMFP_GROUP_MODE_SINGLE;
        }
    } else {
        group_mode = opinfo->group_mode;
    }

    hw_entry_info = stage->hw_entry_info[group_mode];

    if (is_presel) {
        sid = hw_entry_info->presel_sid;
    } else {
        sid = hw_entry_info->sid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_info_get(unit, sid,
             hw_entry_info->mask_fid, &finfo));

    *start_bit = finfo.minbit;
    *end_bit = finfo.maxbit;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_valid_bit_set(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint32_t **entry_words,
                          bool enable)
{
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t value = 0;
    bcmdrd_sid_t sid;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)
        && is_presel == FALSE) {
        SHR_EXIT();
    }

    group_mode = opinfo->group_mode;
    hw_entry_info = stage->hw_entry_info[group_mode];

    if (is_presel) {
        sid = hw_entry_info->presel_sid;
    } else {
        sid = hw_entry_info->sid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_info_get(unit, sid,
            hw_entry_info->valid_fid, &finfo));

    if (enable != 0) {
        value = ((enable << (finfo.maxbit - finfo.minbit + 1)) - 1);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    for (part = 0; part < num_parts; part++) {
        bcmdrd_pt_field_set(unit, sid, entry_words[part],
                            hw_entry_info->valid_fid, &value);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lrd_info_init(int unit,
                    bcmfp_stage_t *stage)
{
    bcmltd_sid_t tbl_id;
    uint32_t total_fields = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&(stage->imm_buffers), 0, sizeof(bcmfp_ltd_buffers_t));

    if (stage->tbls.group_tbl != NULL) {
        tbl_id = stage->tbls.group_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.group_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.group_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.group_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.rule_tbl != NULL) {
        tbl_id = stage->tbls.rule_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.rule_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.rule_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.rule_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.policy_tbl != NULL) {
        tbl_id = stage->tbls.policy_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.policy_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.policy_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.policy_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.entry_tbl != NULL) {
        tbl_id = stage->tbls.entry_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.entry_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.pdd_tbl != NULL) {
        tbl_id = stage->tbls.pdd_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.pdd_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.pdd_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.pdd_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.sbr_tbl != NULL) {
        tbl_id = stage->tbls.sbr_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.sbr_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.pse_tbl != NULL) {
        tbl_id = stage->tbls.pse_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.pse_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.pse_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.pse_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.psg_tbl != NULL) {
        tbl_id = stage->tbls.psg_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.psg_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.ctr_entry_tbl != NULL) {
        tbl_id = stage->tbls.ctr_entry_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.ctr_entry_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.meter_tbl != NULL) {
        tbl_id = stage->tbls.meter_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.meter_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.src_class_tbl != NULL) {
        tbl_id = stage->tbls.src_class_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.src_class_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.range_check_group_tbl != NULL) {
        tbl_id = stage->tbls.range_check_group_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.range_check_group_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.group_info_tbl != NULL) {
        tbl_id = stage->tbls.group_info_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.group_info_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.group_part_tbl != NULL) {
        tbl_id = stage->tbls.group_part_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.group_part_tbl->lrd_info.num_fid = total_fields;
        if (stage->tbls.group_part_tbl->lrd_info.num_fid >
            stage->imm_buffers.num_data_fields) {
            stage->imm_buffers.num_data_fields =
                   stage->tbls.group_part_tbl->lrd_info.num_fid;
        }
    }

    if (stage->tbls.pdd_info_tbl != NULL) {
        tbl_id = stage->tbls.pdd_info_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.pdd_info_tbl->lrd_info.num_fid = total_fields;
    }

    if (stage->tbls.psg_info_tbl != NULL) {
        tbl_id = stage->tbls.psg_info_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_get_number_of_fields_in_table(unit, tbl_id, &total_fields));
        stage->tbls.psg_info_tbl->lrd_info.num_fid = total_fields;
    }

exit:
    SHR_FUNC_EXIT();
}

