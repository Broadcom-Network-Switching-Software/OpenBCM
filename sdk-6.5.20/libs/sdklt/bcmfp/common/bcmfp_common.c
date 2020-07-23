/*! \file bcmfp_common.c
 *
 * APIs to operate on s/w state of the FP stage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/bcmcfg_lt.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static bcmfp_control_t *bcmfp_control[BCMDRD_CONFIG_MAX_UNITS];

int
bcmfp_common_init(int unit)
{
   SHR_FUNC_ENTER(unit);

   BCMFP_ALLOC(bcmfp_control[unit],
               sizeof(bcmfp_control_t),
               "bcmfpFieldCtrl");

exit:
   SHR_FUNC_EXIT();
}

int
bcmfp_common_cleanup(int unit)
{
   SHR_FUNC_ENTER(unit);

   SHR_FREE(bcmfp_control[unit]);

   SHR_FUNC_EXIT();
}

int
bcmfp_control_get(int unit, bcmfp_control_t **fc)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fc, SHR_E_PARAM);

    if (bcmfp_control[unit] != NULL) {
        *fc = bcmfp_control[unit];
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stages_create(int unit)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_id_t stage_id = 0;

    SHR_FUNC_ENTER(unit);

    /* IFP Stage. */
    stage_id = BCMFP_STAGE_ID_INGRESS;
    BCMFP_ALLOC(stage,
                sizeof(bcmfp_stage_t),
                "bcmfpIngStage");
    stage->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_set(unit,
                         stage_id,
                         stage));
    stage = NULL;

    /* EFP Stage. */
    stage_id = BCMFP_STAGE_ID_EGRESS;
    BCMFP_ALLOC(stage,
                sizeof(bcmfp_stage_t),
                "bcmfpIngStage");
    stage->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_set(unit,
                         stage_id,
                         stage));
    stage = NULL;

    /* VFP Stage. */
    stage_id = BCMFP_STAGE_ID_LOOKUP;
    BCMFP_ALLOC(stage,
                sizeof(bcmfp_stage_t),
                "bcmfpIngStage");
    stage->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_set(unit,
                         stage_id,
                         stage));
    stage = NULL;

    /* EMFP Stage. */
    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    BCMFP_ALLOC(stage,
                sizeof(bcmfp_stage_t),
                "bcmfpIngStage");
    stage->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_set(unit,
                         stage_id,
                         stage));
    stage = NULL;

    /* EMFT Stage. */
    stage_id = BCMFP_STAGE_ID_FLOW_TRACKER;
    BCMFP_ALLOC(stage,
                sizeof(bcmfp_stage_t),
                "bcmfpFTStage");
    stage->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_set(unit,
                         stage_id,
                         stage));
    stage = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_stages_destroy(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_stages_destroy(int unit)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_id_t stage_id = 0;

    SHR_FUNC_ENTER(unit);

    for (stage_id = 0;
        stage_id < BCMFP_STAGE_ID_COUNT;
        stage_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit,
                             stage_id,
                             &stage));
        if (stage == NULL) {
            continue;
        }
        if (stage->tbls.entry_tbl == NULL) {
            SHR_FREE(stage);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_stage_set(unit,
                                 stage_id,
                                 NULL));
        }
        stage= NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_get(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t **stage)
{
    bcmfp_control_t *fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_control_get(unit, &fc));

    if (BCMFP_STAGE_ID_INGRESS == stage_id) {
        *stage = fc->ifp;
    } else if (BCMFP_STAGE_ID_LOOKUP == stage_id){
        *stage = fc->vfp;
    } else if (BCMFP_STAGE_ID_EGRESS == stage_id){
        *stage = fc->efp;
    } else if (BCMFP_STAGE_ID_EXACT_MATCH == stage_id){
        *stage = fc->emfp;
    } else if (BCMFP_STAGE_ID_FLOW_TRACKER == stage_id){
        *stage = fc->emft;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_set(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t *stage)
{
    bcmfp_control_t *fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_control_get(unit, &fc));

    if (BCMFP_STAGE_ID_INGRESS == stage_id) {
        fc->ifp = stage;
    } else if (BCMFP_STAGE_ID_LOOKUP == stage_id){
        fc->vfp = stage;
    } else if (BCMFP_STAGE_ID_EGRESS == stage_id){
        fc->efp = stage;
    } else if (BCMFP_STAGE_ID_EXACT_MATCH == stage_id){
        fc->emfp = stage;
    } else if (BCMFP_STAGE_ID_FLOW_TRACKER == stage_id){
        fc->emft = stage;
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmfp_stage_hw_entry_info_cleanup(int unit, bcmfp_stage_t *stage)
{
    bcmfp_group_mode_t group_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (group_mode = 0;
         group_mode < BCMFP_GROUP_MODE_COUNT;
         group_mode++) {
        SHR_FREE(stage->hw_entry_info[group_mode]);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmfp_stage_hw_field_info_cleanup(int unit, bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(stage->misc_info, SHR_E_PARAM);
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_SUPPORTS_COLORED_ACTIONS)) {
        SHR_FREE(stage->misc_info->colored_actions_enable->offset);
        SHR_FREE(stage->misc_info->colored_actions_enable->width);
        SHR_FREE(stage->misc_info->colored_actions_enable);
    }
exit:
    SHR_FUNC_EXIT();
}


int bcmfp_stage_misc_info_cleanup(int unit, bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_FREE(stage->misc_info);
    SHR_FREE(stage->key_data_size_info);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_pdd_hw_info_cleanup(int unit, bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->pdd_hw_info) {
        if (stage->pdd_hw_info->section_hw_info) {
            SHR_FREE(stage->pdd_hw_info->section_hw_info);
        }
        SHR_FREE(stage->pdd_hw_info);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_sbr_hw_info_cleanup(int unit, bcmfp_stage_t *stage)
{
    uint8_t idx = 0;
    uint8_t sbr_profile_sid_columns = 0;
    bcmfp_stage_hw_field_info_t *priority_field_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->sbr_hw_info) {
        if (stage->sbr_hw_info->priority_field_info) {
            priority_field_info =
                stage->sbr_hw_info->priority_field_info;
            sbr_profile_sid_columns =
                stage->sbr_hw_info->sbr_profile_sid_columns;
            for (idx = 0; idx < sbr_profile_sid_columns; idx++) {
               SHR_FREE(priority_field_info[idx].offset);
               SHR_FREE(priority_field_info[idx].width);
            }
            SHR_FREE(stage->sbr_hw_info->priority_field_info);
        }
        if (stage->sbr_hw_info->sbr_sid) {
            SHR_FREE(stage->sbr_hw_info->sbr_sid);
        }
        SHR_FREE(stage->sbr_hw_info);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_profiles_hw_info_cleanup(int unit,
                                     bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_FREE(stage->profiles_hw_info.sbr_profile_hw_info);
exit:
    SHR_FUNC_EXIT();
}

char *
bcmfp_ext_section_name(bcmfp_ext_section_t section)
{
    char *section_name = NULL;

    if (section <= BCMFP_EXT_SECTION_COUNT &&
        section >= BCMFP_EXT_SECTION_DISABLE) {
    }

    return section_name;
}

int
bcmfp_stage_max_entries_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t *max_entries)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    *max_entries = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.entry_tbl != NULL) {
        ltid = stage->tbls.entry_tbl->sid;
        fid = stage->tbls.entry_tbl->key_fid;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_entries));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_group_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_group_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_group_id, SHR_E_PARAM);

    *max_group_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.group_tbl != NULL) {
        ltid = stage->tbls.group_tbl->sid;
        fid = stage->tbls.group_tbl->key_fid;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_presel_id_get(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t *max_presel_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_presel_id, SHR_E_PARAM);

    *max_presel_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.pse_tbl != NULL) {
        ltid = stage->tbls.pse_tbl->sid;
        fid = stage->tbls.pse_tbl->key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_presel_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_meter_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_meter_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(max_meter_id, SHR_E_PARAM);

    *max_meter_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.meter_tbl != NULL) {
        ltid = stage->tbls.meter_tbl->sid;
        fid = stage->tbls.meter_tbl->key_fid;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_meter_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_pdd_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_pdd_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_pdd_id, SHR_E_PARAM);

    *max_pdd_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.pdd_tbl != NULL) {
        ltid = stage->tbls.pdd_tbl->sid;
        fid = stage->tbls.pdd_tbl->key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_pdd_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_ctr_id_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t *max_ctr_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_ctr_id, SHR_E_PARAM);

    *max_ctr_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.ctr_entry_tbl != NULL) {
        ltid = stage->tbls.ctr_entry_tbl->sid;
        fid = stage->tbls.ctr_entry_tbl->key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_ctr_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_ctr_pool_id_get(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t *max_ctr_pool_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_ctr_pool_id, SHR_E_PARAM);

    *max_ctr_pool_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.ctr_entry_tbl != NULL) {
        ltid = stage->tbls.ctr_entry_tbl->sid;
        fid = stage->tbls.ctr_entry_tbl->pool_id_fid;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_ctr_pool_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_idp_maps_modulo_set(int unit,
                                bcmfp_stage_t *stage)
{
    bcmfp_stage_oper_mode_t oper_mode = 0;
    uint32_t pipe_map = 0;
    int pipe_count = 1;
    bcmfp_stage_flag_t flag1 = 0;
    bcmfp_stage_flag_t flag2 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Update the idp_maps_modulo of the stage. */
    flag1 = BCMFP_STAGE_IDP_MAP_TYPE_HASH;
    flag2 = BCMFP_STAGE_IDP_MAP_TYPE_ARRAY;
    if (BCMFP_STAGE_FLAGS_TEST(stage, flag1)) {
        stage->idp_maps_modulo = BCMFP_IDP_MAPS_MODULO;
    } else if (BCMFP_STAGE_FLAGS_TEST(stage, flag2)) {
        stage->idp_maps_modulo = 1;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage->stage_id,
                                   &oper_mode));
    /*
     * If not per pipe mode total number of entries
     * allowed to create are less. So by increasing
     * the idp_maps_modulo, maps size will also be
     * reduced in stage operational information.
     */
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        /* Get the number of pipes */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dev_valid_pipes_get(unit,
                                       stage->stage_id,
                                       &pipe_map));
        pipe_count = shr_util_popcount(pipe_map);
    }

    if (pipe_count != 0) {
        stage->idp_maps_modulo *= pipe_count;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field ID from field name in an LT.
 *
 * \param [in] unit         Logical device id.
 * \param [in] table_name   FP LT name.
 * \param [in] field_name   FP LT field name.
 * \param [out] fid         FP LT field ID.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
int
bcmfp_field_name_to_id_get(int unit,
                           const char *table_name,
                           const char *field_name,
                           bcmltd_fid_t *fid)
{
    int rv;
    size_t actual_fields;
    bcmlrd_client_field_info_t *field_array = NULL;
    bcmlrd_table_attrib_t table_attrs;
    size_t num_fields = 0;
    size_t size = 0;
    size_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);
    SHR_NULL_CHECK(field_name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    sal_memset(&table_attrs, 0, sizeof(bcmlrd_table_attrib_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     table_name,
                                     &table_attrs));
    num_fields = table_attrs.number_of_fields;

    size = sizeof(bcmlrd_client_field_info_t) * num_fields;
    BCMFP_ALLOC(field_array, size, "bcmfpLrdClientFieldInfo");
    rv = bcmlrd_table_fields_def_get(unit,
                                     table_name,
                                     num_fields,
                                     field_array,
                                     &actual_fields);
    if (SHR_FAILURE(rv) || num_fields != actual_fields) {
        SHR_FREE(field_array);
        field_array = NULL;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < actual_fields; idx++) {
        if (!sal_strcmp(field_array[idx].name, field_name)) {
            *fid = field_array[idx].id;
            break;
        }
    }

    if (idx == actual_fields) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FREE(field_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field name from field ID in an LT.
 *
 * \param [in] unit         Logical device id.
 * \param [in] table_name   FP LT name.
 * \param [in] fid          FP LT field ID.
 * \param [out] field_name  FP LT field name.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
int
bcmfp_field_id_to_name_get(int unit,
                           const char *table_name,
                           bcmltd_fid_t fid,
                           char *field_name)
{
    int rv;
    size_t actual_fields;
    bcmlrd_client_field_info_t *field_array = NULL;
    bcmlrd_table_attrib_t table_attrs;
    size_t num_fields = 0;
    size_t size = 0;
    size_t idx = 0;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);
    SHR_NULL_CHECK(field_name, SHR_E_PARAM);

    sal_memset(&table_attrs, 0, sizeof(bcmlrd_table_attrib_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     table_name,
                                     &table_attrs));
    num_fields = table_attrs.number_of_fields;

    size = sizeof(bcmlrd_client_field_info_t) * num_fields;
    BCMFP_ALLOC(field_array, size, "bcmfpLrdClientFieldInfo");
    rv = bcmlrd_table_fields_def_get(unit,
                                     table_name,
                                     num_fields,
                                     field_array,
                                     &actual_fields);
    if (SHR_FAILURE(rv) || num_fields != actual_fields) {
        SHR_FREE(field_array);
        field_array = NULL;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < actual_fields; idx++) {
        if (field_array[idx].id == fid) {
            sal_strcpy(field_name, field_array[idx].name);
            break;
        }
    }

    if (idx == actual_fields) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FREE(field_array);
    SHR_FUNC_EXIT();
}
int
bcmfp_action_is_supported(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_sid_type_t sid_type,
                          const char *table_name,
                          bcmltd_fid_t fid,
                          const char *field_name,
                          bool *supported)
{
    bcmfp_tbl_action_fid_info_t *actions_fid_info = NULL;
    bcmfp_action_cfg_db_t *action_cfg_db = NULL;
    bcmfp_action_t action = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(supported, SHR_E_PARAM);

    *supported = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (field_name != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_field_name_to_id_get(unit,
                                        table_name,
                                        field_name,
                                        &fid));
    }
    if (sid_type == BCMFP_SID_TYPE_PDD_TEMPLATE) {
        actions_fid_info = stage->tbls.pdd_tbl->actions_fid_info;
    } else if (sid_type == BCMFP_SID_TYPE_SBR_TEMPLATE) {
        actions_fid_info = stage->tbls.sbr_tbl->actions_fid_info;
    } else if (sid_type == BCMFP_SID_TYPE_POLICY_TEMPLATE) {
        actions_fid_info = stage->tbls.policy_tbl->actions_fid_info;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    action = actions_fid_info[fid].action;
    action_cfg_db = stage->action_cfg_db;
    if (action_cfg_db == NULL)  {
        SHR_EXIT();
    }
    if (action_cfg_db->action_cfg[action] != NULL &&
        !(action_cfg_db->action_cfg[action]->flags & BCMFP_ACTION_NOT_VALID)) {
        *supported = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_psg_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_psg_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_psg_id, SHR_E_PARAM);

    *max_psg_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.psg_tbl != NULL) {
        ltid = stage->tbls.psg_tbl->sid;
        fid = stage->tbls.psg_tbl->key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_psg_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qualifier_is_supported(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_sid_type_t sid_type,
                             const char *table_name,
                             bcmltd_fid_t fid,
                             const char *field_name,
                             bool *supported)
{
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;
    bcmfp_keygen_qual_cfg_info_db_t *kgn_qual_cfg_info_db = NULL;
    bcmfp_qualifier_t qualifier = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(supported, SHR_E_PARAM);

    *supported = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (field_name != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_field_name_to_id_get(unit,
                                        table_name,
                                        field_name,
                                        &fid));
    }
    if (sid_type == BCMFP_SID_TYPE_GRP_TEMPLATE) {
        qualifiers_fid_info = stage->tbls.group_tbl->qualifiers_fid_info;
    } else if (sid_type == BCMFP_SID_TYPE_RULE_TEMPLATE) {
        qualifiers_fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;
    } else if (sid_type == BCMFP_SID_TYPE_PSE_TEMPLATE) {
        qualifiers_fid_info = stage->tbls.pse_tbl->qualifiers_fid_info;
    } else if (sid_type == BCMFP_SID_TYPE_PSG_TEMPLATE) {
        qualifiers_fid_info = stage->tbls.psg_tbl->qualifiers_fid_info;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    qualifier = qualifiers_fid_info[fid].qualifier;
    /* LT field is not a qualifier. */
    if (qualifier == 0) {
        SHR_EXIT();
    }
    if (sid_type == BCMFP_SID_TYPE_GRP_TEMPLATE ||
        sid_type == BCMFP_SID_TYPE_RULE_TEMPLATE) {
        if (stage->qual_cfg_info_db != NULL) {
            qual_cfg_info_db = stage->qual_cfg_info_db;
        } else {
            kgn_qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
        }
    } else {
        if (stage->presel_qual_cfg_info_db != NULL) {
            qual_cfg_info_db = stage->presel_qual_cfg_info_db;
        } else {
            kgn_qual_cfg_info_db = stage->kgn_presel_qual_cfg_info_db;
        }
    }

    if (kgn_qual_cfg_info_db != NULL) {
        if (kgn_qual_cfg_info_db->qual_cfg_info[qualifier] != NULL) {
            *supported = TRUE;
        }
    } else if (qual_cfg_info_db != NULL) {
        if (qual_cfg_info_db->qual_cfg_info[qualifier] != NULL) {
            *supported = TRUE;
        }
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_max_sbr_id_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t *max_sbr_id)
{
    bcmlrd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(max_sbr_id, SHR_E_PARAM);

    *max_sbr_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->tbls.sbr_tbl != NULL) {
        ltid = stage->tbls.sbr_tbl->sid;
        fid = stage->tbls.sbr_tbl->key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_max_value_get(unit, ltid, fid, max_sbr_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_misc_init(int unit)
{
    bcmltd_fid_t fid;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_tbl_qualifier_fid_info_t *fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    for (stage_id = 0; stage_id < BCMFP_STAGE_ID_COUNT; stage_id++) {
        stage = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit, stage_id, &stage));
        if (stage == NULL) {
            continue;
        }
        /* QUALIFIER in GROUP */
        fid = stage->tbls.group_tbl->key_type_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
        }
        /* DATA field in RULE */
        fid = stage->tbls.rule_tbl->key_type_fid;
        if (fid != 0) {
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
            = stage->tbls.group_tbl->key_type_fid;
            SHR_BITSET(stage->tbls.rule_tbl->qualifiers_fid_info[fid].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_DATA);
            SHR_BITSET(stage->tbls.rule_tbl->qualifiers_fid_info[fid].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS);
        }
        /* MASK field in RULE */
        fid = stage->tbls.rule_tbl->key_type_mask_fid;
        if (fid != 0) {
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
            = stage->tbls.group_tbl->key_type_fid;
            SHR_BITSET(stage->tbls.rule_tbl->qualifiers_fid_info[fid].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_MASK);
            SHR_BITSET(stage->tbls.rule_tbl->qualifiers_fid_info[fid].flags.w,
                       BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS);
        }
        /* PRESEL ACTION */
        if (NULL != stage->tbls.pse_tbl) {
            fid = stage->tbls.pse_tbl->key_type_fid;
            if (fid != 0) {
                stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier = fid;
                SHR_BITSET(stage->tbls.pse_tbl->qualifiers_fid_info[fid].flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_ACTION);
                SHR_BITSET(stage->tbls.pse_tbl->qualifiers_fid_info[fid].flags.w,
                           BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS);
            }
        }

        fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;

        fid = stage->tbls.group_tbl->hash_a0_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_a0_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_a0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_a0_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_a0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->hash_a1_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_a1_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_a1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_a1_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_a1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->hash_b0_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_b0_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_b0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_b0_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_b0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->hash_b1_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_b1_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_b1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_b1_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_b1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->hash_c0_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_c0_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_c0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_c0_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_c0_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->hash_c1_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->hash_c1_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_c1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->hash_c1_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->hash_c1_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->drop_data_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->drop_data_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->drop_data_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->drop_data_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->drop_data_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->drop_decision_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->drop_decision_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->drop_decision_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->drop_decision_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->drop_decision_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        fid = stage->tbls.group_tbl->trace_vector_fid;
        if (fid != 0) {
            stage->tbls.group_tbl->qualifiers_fid_info[fid].qualifier = fid;
            fid = stage->tbls.rule_tbl->trace_vector_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->trace_vector_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
            fid = stage->tbls.rule_tbl->trace_vector_mask_fid;
            stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier
                = stage->tbls.group_tbl->trace_vector_fid;
            SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
        }

        if (stage->tbls.psg_tbl != NULL && stage->tbls.pse_tbl != NULL) {
            fid_info = stage->tbls.pse_tbl->qualifiers_fid_info;
            fid = stage->tbls.psg_tbl->drop_decision_fid;
            if (fid != 0) {
                stage->tbls.psg_tbl->qualifiers_fid_info[fid].qualifier = fid;
                fid = stage->tbls.pse_tbl->drop_decision_fid;
                stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier
                    = stage->tbls.psg_tbl->drop_decision_fid;
                SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
                fid = stage->tbls.pse_tbl->drop_decision_mask_fid;
                stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier
                    = stage->tbls.psg_tbl->drop_decision_fid;
                SHR_BITSET(fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_maps_adjust(int unit)
{
    int rv;
    const bcmlrd_table_rep_t *tbl = NULL;
    bcmlrd_field_def_t field_def;
    const bcmlrd_map_t *map = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t sid = 0;
    bcmltd_fid_t fid = 0;
    uint16_t fid_count = 0;
    bcmfp_tbl_qualifier_fid_info_t *rqualifiers_fid_info = NULL;
    bcmfp_tbl_qualifier_fid_info_t *gqualifiers_fid_info = NULL;
    uint16_t total_array_elements = 0;
    bcmltd_fid_t *garray_fids = NULL;
    bcmltd_fid_t *temp_array_fids = NULL;
    uint32_t num_bunches = 0;
    uint16_t bunch_size = 10;
    size_t size= 0;
    uint32_t gtotal_array_fids = 0;
    uint16_t rtotal_array_fids = 0;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    for (stage_id = 0; stage_id < BCMFP_STAGE_ID_COUNT; stage_id++) {
        stage = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit, stage_id, &stage));
        if (stage == NULL) {
            continue;
        }
        sid = stage->tbls.group_tbl->sid;
        fid_count = stage->tbls.group_tbl->fid_count;
        gqualifiers_fid_info =
            stage->tbls.group_tbl->qualifiers_fid_info;
        gtotal_array_fids = 0;
        num_bunches = 0;
        total_array_elements = 0;
        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_map_get(unit, sid, &map));
        for (fid = 0; fid < fid_count; fid++) {
            rv = bcmlrd_field_def_get(unit, fid, tbl, map,  &field_def);
            if (rv == SHR_E_UNAVAIL) {
                continue;
            }
            SHR_IF_ERR_EXIT(rv);
            /* Non array qualifier FIDs. */
            if ((fid == stage->tbls.group_tbl->qual_inports_fid) ||
                (fid == stage->tbls.group_tbl->qual_system_ports_fid) ||
                (fid == stage->tbls.group_tbl->qual_device_ports_fid)) {
                continue;
            }
            if (field_def.depth == 0) {
                continue;
            }
            /* Non qualifier FIDs. */
            if (gqualifiers_fid_info[fid].qualifier == 0) {
                continue;
            }
            /*
             * Create memory to save array qualifier FIDs
             * in bunches of 10 FIDs.
             */
            if ((gtotal_array_fids + 1) > ((num_bunches * bunch_size))) {
                size = (((num_bunches + 1) * bunch_size) *
                       sizeof(bcmltd_fid_t));
                BCMFP_ALLOC(temp_array_fids,
                            size,
                            "bcmfpFidMapsAdjustArrayFids");
                size = ((num_bunches * bunch_size) * sizeof(bcmltd_fid_t));
                if (garray_fids != NULL) {
                    sal_memcpy(temp_array_fids, garray_fids, size);
                    SHR_FREE(garray_fids);
                }
                garray_fids = temp_array_fids;
                temp_array_fids = NULL;
                num_bunches++;
             }
             garray_fids[gtotal_array_fids] = fid;
             gtotal_array_fids++;
             gqualifiers_fid_info[fid].fid = fid;
             gqualifiers_fid_info[fid].qualifier =
                                fid_count + total_array_elements;
             total_array_elements += field_def.depth;
        }

        /* No array qualifier FIDs in this stage. */
        if (gtotal_array_fids == 0) {
            continue;
        }

        sid = stage->tbls.rule_tbl->sid;
        fid_count = stage->tbls.rule_tbl->fid_count;
        rqualifiers_fid_info =
            stage->tbls.rule_tbl->qualifiers_fid_info;
        rtotal_array_fids = 0;
        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_map_get(unit, sid, &map));
        for (fid = 0; fid < fid_count; fid++) {
            rv = bcmlrd_field_def_get(unit, fid, tbl, map,  &field_def);
            if (rv == SHR_E_UNAVAIL) {
                continue;
            }
            SHR_IF_ERR_EXIT(rv);
            /* Non array qualifier FIDs. */
            if ((fid == stage->tbls.rule_tbl->inports_key_fid) ||
                (fid == stage->tbls.rule_tbl->inports_mask_fid) ||
                (fid == stage->tbls.rule_tbl->system_ports_key_fid) ||
                (fid == stage->tbls.rule_tbl->system_ports_mask_fid) ||
                (fid == stage->tbls.rule_tbl->device_ports_key_fid) ||
                (fid == stage->tbls.rule_tbl->device_ports_mask_fid)) {
                continue;
            }
            if (field_def.depth == 0) {
                continue;
            }
            /* Non qualifier FIDs. */
            if (rqualifiers_fid_info[fid].qualifier == 0) {
                continue;
            }
            for (idx = 0; idx < gtotal_array_fids; idx++) {
                if (rqualifiers_fid_info[fid].qualifier ==
                    garray_fids[idx]) {
                    break;
                }
            }
            if (idx == gtotal_array_fids) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            rqualifiers_fid_info[fid].fid = garray_fids[idx];
            rqualifiers_fid_info[fid].qualifier =
                gqualifiers_fid_info[garray_fids[idx]].qualifier;
            rtotal_array_fids++;
            /*
             * For every qualifier in group template will have two
             * fields(data and mask) in rule LT. But in case of EM
             * one qualifier LT field in group template has only
             * one qualifier LT field in rule template
             */
            if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                if (rtotal_array_fids  == gtotal_array_fids) {
                    break;
                }
            } else {
                if (rtotal_array_fids == (gtotal_array_fids * 2)) {
                    break;
                }
            }
        }
        SHR_FREE(garray_fids);
        garray_fids = NULL;
    }

exit:
    SHR_FREE(garray_fids);
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_buffers_clear(int unit,
                        bcmfp_stage_id_t stage_id)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage == NULL) {
        SHR_EXIT();
    }

    if (stage->imm_buffers.non_def_data_fields_1_in_use == true) {
        stage->imm_buffers.non_def_data_fields_1_in_use  = false;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_buffers_cleanup(int unit,
                           bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    if (stage == NULL) {
        SHR_EXIT();
    }

    if (stage->imm_buffers.key_fields != NULL) {
        if (stage->imm_buffers.key_fields[0] != NULL) {
            SHR_FREE(stage->imm_buffers.key_fields[0]);
        }
        SHR_FREE(stage->imm_buffers.key_fields);
    }
    if (stage->imm_buffers.data_fields != NULL) {
        if (stage->imm_buffers.data_fields[0] != NULL) {
            SHR_FREE(stage->imm_buffers.data_fields[0]);
        }
        SHR_FREE(stage->imm_buffers.data_fields);
    }
    if (stage->imm_buffers.non_def_data_fields_1 != NULL) {
        if (stage->imm_buffers.non_def_data_fields_1[0] != NULL) {
            SHR_FREE(stage->imm_buffers.non_def_data_fields_1[0]);
        }
        SHR_FREE(stage->imm_buffers.non_def_data_fields_1);
    }
    if (stage->imm_buffers.non_def_data_fields_2 != NULL) {
        if (stage->imm_buffers.non_def_data_fields_2[0] != NULL) {
            SHR_FREE(stage->imm_buffers.non_def_data_fields_2[0]);
        }
        SHR_FREE(stage->imm_buffers.non_def_data_fields_2);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_buffers_init(int unit,
                       bcmfp_stage_t *stage)
{
    bcmltd_field_t *temp = NULL;
    size_t size = 0;
    uint16_t max_fields = 0;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->imm_buffers.num_key_fields = BCMFP_LTD_KEY_FIELDS_MAX;
    stage->imm_buffers.num_non_def_data_fields_1 =
           stage->imm_buffers.num_data_fields;
    stage->imm_buffers.num_non_def_data_fields_2 =
           stage->imm_buffers.num_data_fields;

    size = (sizeof(bcmltd_field_t *) *
            stage->imm_buffers.num_key_fields);
    BCMFP_ALLOC(stage->imm_buffers.key_fields,
                size,
                "bcmfpStageImmBuffersKeyFields");
    size = (sizeof(bcmltd_field_t *) *
            stage->imm_buffers.num_data_fields);
    BCMFP_ALLOC(stage->imm_buffers.data_fields,
                size,
                "bcmfpStageImmBuffersDataFields");
    size = (sizeof(bcmltd_field_t *) *
            stage->imm_buffers.num_non_def_data_fields_1);
    BCMFP_ALLOC(stage->imm_buffers.non_def_data_fields_1,
                size,
                "bcmfpStageImmBuffersNonDefDataFields1");
    size = (sizeof(bcmltd_field_t *) *
            stage->imm_buffers.num_non_def_data_fields_2);
    BCMFP_ALLOC(stage->imm_buffers.non_def_data_fields_2,
                size,
                "bcmfpStageImmBuffersNonDefDataFields2");

    size = (sizeof(bcmltd_field_t) *
            stage->imm_buffers.num_key_fields);
    if (size != 0) {
        BCMFP_ALLOC(temp,
                    size,
                    "bcmfpStageImmBuffersKeyFieldsArray");
        max_fields = stage->imm_buffers.num_key_fields;
        stage->imm_buffers.key_fields[0] = temp;
        for (idx = 1; idx < max_fields; idx++) {
            stage->imm_buffers.key_fields[idx] = &(temp[idx]);
        }
    }
    temp = NULL;
    size = (sizeof(bcmltd_field_t) *
            stage->imm_buffers.num_data_fields);
    if (size != 0) {
        BCMFP_ALLOC(temp,
                    size,
                    "bcmfpStageImmBuffersDataFieldsArray");
        max_fields = stage->imm_buffers.num_data_fields;
        stage->imm_buffers.data_fields[0] = temp;
        for (idx = 1; idx < max_fields; idx++) {
            stage->imm_buffers.data_fields[idx] = &(temp[idx]);
        }
    }
    temp = NULL;
    size = (sizeof(bcmltd_field_t) *
            stage->imm_buffers.num_non_def_data_fields_1);
    if (size != 0) {
        BCMFP_ALLOC(temp,
                    size,
                    "bcmfpStageImmBuffersNonDefDataFieldsArray1");
        max_fields = stage->imm_buffers.num_non_def_data_fields_1;
        stage->imm_buffers.non_def_data_fields_1[0] = temp;
        for (idx = 1; idx < max_fields; idx++) {
            stage->imm_buffers.non_def_data_fields_1[idx] = &(temp[idx]);
        }
    }
    temp = NULL;
    size = (sizeof(bcmltd_field_t) *
            stage->imm_buffers.num_non_def_data_fields_2);
    if (size != 0) {
        BCMFP_ALLOC(temp,
                    size,
                    "bcmfpStageImmBuffersNonDefDataFieldsArray2");
        max_fields = stage->imm_buffers.num_non_def_data_fields_2;
        for (idx = 0; idx < max_fields; idx++) {
            stage->imm_buffers.non_def_data_fields_2[idx] = &(temp[idx]);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_hit_context_init(int unit,
                       uint32_t trans_id,
                       bcmfp_stage_t *stage,
                       bool warm)
{
    int8_t botp_value;
    uint8_t hit_idx_profiles;
    uint8_t profile;
    uint32_t maxidx;
    uint32_t idx;
    uint32_t profile_data;
    bcmdrd_sid_t profile_tbl_sid;
    uint32_t rsp_flags = 0;


    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->hit_context == NULL) {
        SHR_EXIT();
    }

    botp_value = stage->hit_context->botp_value;
    hit_idx_profiles = stage->hit_context->hit_idx_profiles;
    /*
     * botp_value != -1 => the <stage>_hit_index is a valid logical field
     * For valid stages, all the entries in HIT_INDEX_PROFILE tbls are
     * updated with the BOTP_MUX_DATA
     */

    if (botp_value != -1) {
        profile_data = botp_value;
        for (profile = 0; profile < hit_idx_profiles; profile++) {
            profile_tbl_sid = stage->hit_context->hit_idx_profile_tbls[profile];
            maxidx = bcmdrd_pt_index_max(unit, profile_tbl_sid);
            for (idx = 0; idx <= maxidx; idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_insert(unit,
                                            trans_id,
                                            -1,
                                            0,
                                            profile_tbl_sid,
                                            &profile_data,
                                            idx));
            }
        }
    }
    if (hit_idx_profiles > 0 && (trans_id == BCMPTM_DIRECT_TRANS_ID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        BCMPTM_REQ_FLAGS_COMMIT,
                                        INVALIDm,
                                        NULL,
                                        NULL,
                                        NULL,
                                        BCMPTM_OP_NOP,
                                        NULL,
                                        0,
                                        0,
                                        BCMPTM_DIRECT_TRANS_ID,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmfp_hit_context_cleanup(int unit,
                          bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->hit_context == NULL) {
        SHR_EXIT();
    }

    SHR_FREE(stage->hit_context);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_dev_valid_pipes_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t *pipe_map)
{
    const char ipipe[] = "ipipe";
    const char epipe[] = "epipe";
    const bcmdrd_chip_info_t *cinfo;
    int blktype = 0;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
        case BCMFP_STAGE_ID_LOOKUP:
        case BCMFP_STAGE_ID_EXACT_MATCH:
        case BCMFP_STAGE_ID_FLOW_TRACKER:
            blktype = bcmdrd_chip_blktype_get_by_name(cinfo, ipipe);
            break;
        case BCMFP_STAGE_ID_EGRESS:
            blktype = bcmdrd_chip_blktype_get_by_name(cinfo, epipe);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit,
                                        0,
                                        blktype,
                                        pipe_map));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_fid_type_get(int unit,bcmlrd_sid_t sid, bcmlrd_fid_t fid,
                          bcmltd_field_data_tag_t *d_type)
{

    const bcmlrd_table_rep_t *tbl = NULL;
    bcmlrd_field_def_t field_def;
    const bcmlrd_map_t *map = NULL;
    SHR_FUNC_ENTER(unit);

    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_map_get(unit, sid, &map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_def_get(unit, fid,tbl, map,  &field_def));
    *d_type = field_def.dtag;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_prefix_length_get(int unit,
                        uint32_t *mask,
                        uint16_t max_prefix_length,
                        uint16_t *prefix_length)
{
    uint16_t idx = 0;
    uint32_t bit_set = 0;
    bool found_first_one = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(prefix_length, SHR_E_PARAM);

    *prefix_length = 0;

    /* Get the prefix length of the mask. */
    for (idx = 0; idx < max_prefix_length; idx++) {
        bit_set = SHR_BITGET(mask, idx);
        if (bit_set != 0 &&
            found_first_one == FALSE) {
            found_first_one = TRUE;
            *prefix_length += 1;
        } else if (found_first_one == TRUE &&
                   bit_set == 0) {
            SHR_ERR_EXIT(SHR_E_BADID);
        } else if (found_first_one == TRUE &&
                   bit_set != 0) {
            *prefix_length += 1;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_max_value_get(int unit,
                            bcmlrd_sid_t ltid,
                            bcmlrd_fid_t fid,
                            uint32_t *max_val)
{
    uint64_t min, max, def;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_min_max_default_get(unit,
                                       ltid,
                                       fid,
                                       &min,
                                       &max,
                                       &def));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        *max_val = (uint32_t)max;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_bitmap_free(int unit, bcmfp_qual_bitmap_t *qual_bitmap)
{
    bcmfp_qual_bitmap_t *qual_bitmap_temp;

    SHR_FUNC_ENTER(unit);
    while (qual_bitmap != NULL) {
        qual_bitmap_temp = qual_bitmap->next;
        SHR_FREE(qual_bitmap);
        qual_bitmap = qual_bitmap_temp;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_min_max_default_get(int unit,
                              bcmlrd_sid_t ltid,
                              bcmlrd_fid_t fid,
                              uint64_t *min_value,
                              uint64_t *max_value,
                              uint64_t *def_value)
{

    const bcmlrd_field_data_t *field_data = NULL;
    bcmltd_field_data_tag_t d_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, ltid, fid, &field_data));
    SHR_IF_ERR_EXIT
        (bcmfp_fid_type_get(unit, ltid, fid, &d_type));

    SHR_NULL_CHECK(min_value, SHR_E_PARAM);
    SHR_NULL_CHECK(max_value, SHR_E_PARAM);
    SHR_NULL_CHECK(def_value, SHR_E_PARAM);


    switch (d_type) {
        case BCMLT_FIELD_DATA_TAG_BOOL:
            *min_value = (uint64_t)field_data->min->is_true;
            *max_value = (uint64_t)field_data->max->is_true;
            *def_value = (uint64_t)field_data->def->is_true;
            break;
        case BCMLT_FIELD_DATA_TAG_U8:
            *min_value = (uint64_t)field_data->min->u8;
            *max_value = (uint64_t)field_data->max->u8;
            *def_value = (uint64_t)field_data->def->u8;
            break;
        case BCMLT_FIELD_DATA_TAG_U16:
            *min_value = (uint64_t)field_data->min->u16;
            *max_value = (uint64_t)field_data->max->u16;
            *def_value = (uint64_t)field_data->def->u16;
            break;
        case BCMLT_FIELD_DATA_TAG_U32:
            *min_value = (uint64_t)field_data->min->u32;
            *max_value = (uint64_t)field_data->max->u32;
            *def_value = (uint64_t)field_data->def->u32;
            break;
        case BCMLT_FIELD_DATA_TAG_U64:
            *min_value = (uint64_t)field_data->min->u64;
            *max_value = (uint64_t)field_data->max->u64;
            *def_value = (uint64_t)field_data->def->u64;
            break;
        default:
            *min_value = 0;
            *max_value = 0;
            *def_value = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_device_ipbm_to_pipe_ipbmp(int unit,
                                bcmdrd_pbmp_t *device_pbmp,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_local_pbmp)
{
    int base_port = -1;
    int pipe_no = tbl_inst;
    uint16_t port = 0;
    uint16_t pipe_port = 0;
    bcmdrd_pbmp_t valid_pbmp;
    int blktype;
    const bcmdrd_chip_info_t *cinfo;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_local_pbmp, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(valid_pbmp);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "ipipe");
    SHR_IF_ERR_EXIT(
        bcmdrd_dev_blk_pipe_pbmp(unit, blktype, pipe_no, &valid_pbmp));

    BCMDRD_PBMP_CLEAR(*pipe_local_pbmp);

    BCMDRD_PBMP_ITER(valid_pbmp, port) {
        if (base_port == -1) {
            base_port = port;
        }
        if (BCMDRD_PBMP_MEMBER((*device_pbmp), port)) {
            pipe_port = port - base_port;
            BCMDRD_PBMP_PORT_ADD(*pipe_local_pbmp, pipe_port);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pipe_ipbmp_to_device_pbmp(int unit,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_local_pbmp,
                                bcmdrd_pbmp_t *device_pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_local_pbmp, SHR_E_PARAM);
    SHR_NULL_CHECK(device_pbmp, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pbmp_validate(int unit,
                    int pipe_id,
                    bcmfp_stage_id_t stage_id,
                    bcmdrd_pbmp_t *in_pbmp)
{
    int pipe_min = -1;
    int pipe_max = -1;
    bcmdrd_pbmp_t dev_pbmp;
    bcmdrd_pbmp_t valid_pbmp;
    uint32_t pipe_map = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_pbmp, SHR_E_PARAM);

    if (pipe_id == -1) {
        pipe_min = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dev_valid_pipes_get(unit,
                                       stage_id,
                                       &pipe_map));
        pipe_max = shr_util_popcount(pipe_map) - 1;
    } else {
        pipe_min = pipe_id;
        pipe_max = pipe_id;
    }

    BCMDRD_PBMP_CLEAR(valid_pbmp);
    for (pipe_id = pipe_min;
         pipe_id <= pipe_max;
         pipe_id++) {
        BCMDRD_PBMP_CLEAR(dev_pbmp);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_dev_pipe_phys_pbmp(unit,
                                       pipe_id,
                                       &dev_pbmp));
        BCMDRD_PBMP_OR(valid_pbmp, dev_pbmp);
    }

    BCMDRD_PBMP_NEGATE(dev_pbmp, valid_pbmp);
    BCMDRD_PBMP_AND(dev_pbmp, (*in_pbmp));
    if (BCMDRD_PBMP_NOT_NULL(dev_pbmp)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
exit:
  SHR_FUNC_EXIT();
}

int
bcmfp_ports_bitmap_get(int unit,
                       bcmltd_field_t *field,
                       bcmdrd_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field, SHR_E_PARAM);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    if (field->data) {
        BCMDRD_PBMP_PORT_ADD(*pbmp, field->idx);
    }
exit:
    SHR_FUNC_EXIT();
}

bool
bcmfp_bits_ones(uint32_t *buffer,
                int offset,
                int width)
{
    int count = 0;

    if (buffer == NULL) {
        return FALSE;
    }

    SHR_BITCOUNT_RANGE(buffer,
                       count,
                       offset,
                       width);
   if (count == width) {
        return TRUE;
   } else {
        return FALSE;
   }
}

bool
bcmfp_bits_zeros(uint32_t *buffer,
                 int offset,
                 int width)
{
    int count = 0;

    if (buffer == NULL) {
        return FALSE;
    }

    SHR_BITCOUNT_RANGE(buffer,
                       count,
                       offset,
                       width);
   if (width == 0) {
        return TRUE;
   } else {
        return FALSE;
   }
}
static int
bcmfp_compress_tries_cleanup(int unit,
                             bcmfp_stage_t *stage)
{
    uint8_t idx = 0;
    bcmfp_compress_key_info_t *key_info1 = NULL;
    bcmfp_compress_key_info_t *key_info2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (stage->compress_tries[idx] == NULL) {
            continue;
        }
        /* Destroy the trie. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_trie_deinit(unit,
                                        stage->compress_tries[idx]));
        stage->compress_tries[idx] = NULL;
        if (stage->compress_key_info[idx] == NULL) {
            continue;
        }
        key_info1 = stage->compress_key_info[idx];
        while (key_info1 != NULL) {
            SHR_FREE(key_info1->entries);
            key_info2 = key_info1;
            key_info1 = key_info1->next;
            SHR_FREE(key_info2);
        }
        stage->compress_key_info[idx] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_tries_init(int unit,
                          bcmfp_stage_t *stage,
                          bcmfp_fn_cid_update_t cb_func)
{
    uint8_t idx = 0;
    bcmfp_shr_trie_md_t *trie = NULL;
    uint8_t fid_idx = 0;
    uint8_t num_fids = 0;
    uint16_t total_width = 0;
    bcmfp_tbl_compress_fid_info_t *compress_fid_info = NULL;
    uint8_t cid_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (stage->tbls.compress_fid_info[idx] == NULL) {
            continue;
        }
        compress_fid_info = stage->tbls.compress_fid_info[idx];
        num_fids = compress_fid_info->num_fids;
        for (fid_idx = 0; fid_idx < num_fids; fid_idx++) {
            total_width +=
                compress_fid_info->fid_widths[fid_idx];
        }
        /* Create the compression tries. */
        cid_size = stage->tbls.compress_fid_info[idx]->cid_size;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_trie_create(unit,
                                        total_width,
                                        cid_size,
                                        idx,
                                        cb_func,
                                        &trie));
        trie->ckey_acquire = bcmfp_compress_key_node_acquire;
        trie->ckey_release = bcmfp_compress_key_node_release;
        trie->stage_id = stage->stage_id;
        stage->compress_tries[idx] = trie;
        total_width = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_qualifier_fids_cleanup(int unit,
                                     bcmfp_stage_t *stage)
{
    uint16_t idx1 = 0;
    bcmfp_tbl_compress_fid_info_t *compress_qualifier_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (stage->tbls.compress_fid_info[idx1] == NULL) {
            continue;
        }
        compress_qualifier_info = stage->tbls.compress_fid_info[idx1];
        SHR_FREE(compress_qualifier_info->bitmap_fids);
        SHR_FREE(compress_qualifier_info->key_fids);
        SHR_FREE(compress_qualifier_info->mask_fids);
        SHR_FREE(compress_qualifier_info->fid_offsets);
        SHR_FREE(compress_qualifier_info->fid_widths);
        SHR_FREE(compress_qualifier_info);
        stage->tbls.compress_fid_info[idx1] = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}
static int
bcmfp_compress_qualifier_fids_init(int unit,
                                  bcmfp_stage_t *stage)
{
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;
    uint16_t idx3 = 0;
    uint16_t idx4 = 0;
    uint16_t num_compress_fid_remote_info = 0;
    size_t size = 0;
    const bcmfp_tbl_fid_remote_info_t *fid_remote_info = NULL;
    const bcmfp_tbl_fid_remote_maps_t *fid_remote_maps = NULL;
    bcmfp_tbl_compress_alpm_t *compress_alpm_tbl = NULL;
    bcmfp_tbl_compress_fid_info_t *compress_qualifier_info = NULL;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t cid_key_fid = 0;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_qualifier_t cid_qual_id = 0;
    uint16_t num_remote_map_fids = 0;
    bcmfp_tbl_qualifier_fid_info_t *rule_qualifiers_fid_info = NULL;
    bcmfp_tbl_qualifier_fid_info_t *group_qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.num_compress_fid_remote_info == 0) {
        SHR_EXIT();
    }

    num_compress_fid_remote_info =
        stage->tbls.num_compress_fid_remote_info;
    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (stage->tbls.compress_alpm_tbl[idx1] == NULL) {
            continue;
        }
        compress_alpm_tbl = stage->tbls.compress_alpm_tbl[idx1];
        /* Create memory for compress qualifier fid info */
        compress_qualifier_info = NULL;
        size = sizeof(bcmfp_tbl_compress_fid_info_t);
        BCMFP_ALLOC(compress_qualifier_info,
                    size,
                    "bcmfpCompressQualifierFidInfo");
        stage->tbls.compress_fid_info[idx1] =
                    compress_qualifier_info;

        compress_qualifier_info->type = idx1;
        /* Get the remote fid info for compress alpm LT fields */
        for (idx2 = 0; idx2 < num_compress_fid_remote_info; idx2++) {
            fid_remote_info =
                &(stage->tbls.compress_fid_remote_info[idx2]);
            if (compress_alpm_tbl->sid == fid_remote_info->sid ) {
                break;
            }
        }
        if (idx2 == num_compress_fid_remote_info) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        /*
         * Get the total number of qualifers mapped all key fields
         * together in compressl ALPM LT.
         */
        num_remote_map_fids = 0;
        for (idx2 = 0; idx2 < compress_alpm_tbl->num_key_fids; idx2++) {
            for (idx3 = 0; idx3 < fid_remote_info->num_remote_maps; idx3++) {
                 if (fid_remote_info->remote_maps[idx3].fid ==
                     compress_alpm_tbl->key_fids[idx2]) {
                     break;
                 }
            }
            if (idx3 == fid_remote_info->num_remote_maps) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            num_remote_map_fids +=
                fid_remote_info->remote_maps[idx3].num_remote_map;
        }

        /*
         * Allocate memory for bitmap, key and mask qualifiers FID
         * arrays. Also for qualifier offsets and width.
         */
        size = (sizeof(bcmltd_fid_t) * num_remote_map_fids);
        BCMFP_ALLOC(compress_qualifier_info->bitmap_fids,
                    size,
                    "bcmfpCompressQualifierBitmapFids");
        BCMFP_ALLOC(compress_qualifier_info->key_fids,
                    size,
                    "bcmfpCompressQualifierKeyFids");
        BCMFP_ALLOC(compress_qualifier_info->mask_fids,
                    size,
                    "bcmfpCompressQualifierMaskFids");
        size = (sizeof(uint16_t) * num_remote_map_fids);
        BCMFP_ALLOC(compress_qualifier_info->fid_offsets,
                    size,
                    "bcmfpCompressQualifierFidOffsets");
        BCMFP_ALLOC(compress_qualifier_info->fid_widths,
                    size,
                    "bcmfpCompressQualifierFidWidths");

        /*
         * Populate total number of qualifier FIDs mapped to all key
         * fields together in compress ALPM LT.
         */
        compress_qualifier_info->num_fids = num_remote_map_fids;

        /* Populate key_fids, offsets and widths of compress qualifiers. */
        num_remote_map_fids = 0;
        for (idx2 = 0; idx2 < compress_alpm_tbl->num_key_fids; idx2++) {
            for (idx3 = 0; idx3 < fid_remote_info->num_remote_maps; idx3++) {
                 if (fid_remote_info->remote_maps[idx3].fid ==
                     compress_alpm_tbl->key_fids[idx2]) {
                     break;
                 }
            }
            if (idx3 == fid_remote_info->num_remote_maps) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            fid_remote_maps = &(fid_remote_info->remote_maps[idx3]);
            for (idx4 = 0; idx4 < fid_remote_maps->num_remote_map; idx4++) {
                compress_qualifier_info->key_fids[num_remote_map_fids] =
                          fid_remote_maps->remote_map[idx4].fid;
                compress_qualifier_info->fid_offsets[num_remote_map_fids] =
                           fid_remote_maps->remote_map[idx4].offset;
                compress_qualifier_info->fid_widths[num_remote_map_fids] =
                           fid_remote_maps->remote_map[idx4].width;
                num_remote_map_fids++;
            }
        }

        /* Populate the cid_key_fid and cid_size. */
        for (idx3 = 0; idx3 < fid_remote_info->num_remote_maps; idx3++) {
            if (fid_remote_info->remote_maps[idx3].fid ==
                compress_alpm_tbl->cid_fid) {
                break;
            }
        }
        if (idx3 == fid_remote_info->num_remote_maps) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        compress_qualifier_info->cid_key_fid =
                 fid_remote_info->remote_maps[idx3].remote_map[0].fid;
        compress_qualifier_info->cid_size =
                 fid_remote_info->remote_maps[idx3].remote_map[0].width;

        rule_qualifiers_fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;
        group_qualifiers_fid_info = stage->tbls.group_tbl->qualifiers_fid_info;

        /* Populate bitmap FIDs for compress qualifiers. */
        for (idx2 = 0; idx2 < compress_qualifier_info->num_fids; idx2++) {
            key_fid = compress_qualifier_info->key_fids[idx2];
            qual_id = rule_qualifiers_fid_info[key_fid].qualifier;
            for (idx3 = 0; idx3 < stage->tbls.group_tbl->fid_count; idx3++) {
                if (group_qualifiers_fid_info[idx3].qualifier == qual_id) {
                    compress_qualifier_info->bitmap_fids[idx2] = idx3;
                }
            }
        }

        /* Populate mask FIDs for compress qualifiers. */
        for (idx2 = 0; idx2 < compress_qualifier_info->num_fids; idx2++) {
            key_fid = compress_qualifier_info->key_fids[idx2];
            qual_id = rule_qualifiers_fid_info[key_fid].qualifier;
            for (idx3 = 0; idx3 < stage->tbls.rule_tbl->fid_count; idx3++) {
                if ((rule_qualifiers_fid_info[idx3].qualifier == qual_id) &&
                    (key_fid != idx3)) {
                    compress_qualifier_info->mask_fids[idx2] = idx3;
                }
            }
        }

        cid_key_fid = compress_qualifier_info->cid_key_fid;
        cid_qual_id = rule_qualifiers_fid_info[cid_key_fid].qualifier;
        /* Populate the CID bitmap FID */
        for (idx3 = 0; idx3 < stage->tbls.group_tbl->fid_count; idx3++) {
            if (group_qualifiers_fid_info[idx3].qualifier == cid_qual_id) {
                compress_qualifier_info->cid_bitmap_fid = idx3;
            }
        }
        /* Populate the CID mask FID */
        for (idx3 = 0; idx3 < stage->tbls.rule_tbl->fid_count; idx3++) {
            if ((rule_qualifiers_fid_info[idx3].qualifier == cid_qual_id) &&
                (idx3 != cid_key_fid)) {
                compress_qualifier_info->cid_mask_fid = idx3;
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmfp_compress_qualifier_fids_cleanup(unit, stage);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_compress_alpm_fids_cleanup(int unit,
                         bcmfp_stage_t *stage)
{
    uint8_t idx = 0;
    bcmfp_tbl_compress_alpm_t *compress_alpm_tbl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (stage->tbls.compress_alpm_tbl[idx] == NULL) {
            continue;
        }
        compress_alpm_tbl =
                 stage->tbls.compress_alpm_tbl[idx];
        SHR_FREE(compress_alpm_tbl->key_fids);
        SHR_FREE(compress_alpm_tbl->mask_fids);
        SHR_FREE(compress_alpm_tbl);
        stage->tbls.compress_alpm_tbl[idx] = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_cleanup(int unit,
                       bcmfp_stage_t *stage)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.compress_fid_remote_info == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_alpm_fids_cleanup(unit, stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_qualifier_fids_cleanup(unit, stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_tries_cleanup(unit, stage));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compress_init(int unit,
                    bcmfp_stage_t *stage,
                    bcmfp_fn_cid_update_t cb_func)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.num_compress_fid_remote_info == 0) {
        SHR_EXIT();
    }

    /*
     * Consume the auto generated data by tools in
     * stage->tbls.compress_fid_remote_info and populate
     * stage->tbls.compress_fid_info
     *          and
     * stage->tbls.compress_alpm_tbl
     * data structures to use at run time. Using these
     * two sets of data create tries to which compression
     * keys will be added at run time.
     */

    /* Populate stage->tbls.compress_fid_info next. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_qualifier_fids_init(unit, stage));

    /*
     * Create the compression tries for poissble
     * compression types supported by the stage.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_compress_tries_init(unit, stage, cb_func));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_tile_l0_section_gran_get(int unit,
                               uint32_t tile_id,
                               uint8_t section_id,
                               uint8_t *gran)
{
    const bcmdrd_tile_mux_info_db_t *mux_info_db = NULL;
    const bcmdrd_tile_mux_info_t *mux_info = NULL;
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gran, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_tile_mux_info_db_get(unit,
                                     tile_id,
                                     &mux_info_db));

    mux_info = mux_info_db->info;
    for (idx1 = 0; idx1 < mux_info_db->count; idx1++) {
         for (idx2 = 0;
             idx2 < mux_info[idx1].num_in_sections;
             idx2++) {
             if (mux_info[idx1].in_sec_id[idx2] ==
                 section_id) {
                 *gran = mux_info[idx1].gran;
                 break;
             }
         }
         if (idx2 != mux_info[idx1].num_in_sections) {
             break;
         }
    }
    if (idx1 ==  mux_info_db->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}
