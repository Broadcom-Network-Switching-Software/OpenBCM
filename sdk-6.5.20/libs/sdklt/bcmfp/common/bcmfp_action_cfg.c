/*! \file bcmfp_action_cfg.c
 *
 * Utility APIs to operate with FP actions.
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

int
bcmfp_action_cfg_t_init(int unit, bcmfp_action_cfg_t *action_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_cfg, SHR_E_PARAM);

    sal_memset(action_cfg, 0, sizeof(bcmfp_action_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cfg_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_action_t action,
                     bcmfp_action_cfg_t **action_cfg)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    *action_cfg = stage->action_cfg_db->action_cfg[action];
    if (action_cfg == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *new_action_cfg)
{
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_cfg_db_t *action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_action_cfg, SHR_E_PARAM);

    action_cfg_db = stage->action_cfg_db;

    if (action_cfg_db == NULL) {
        BCMFP_ALLOC(action_cfg_db,
                    sizeof(bcmfp_action_cfg_db_t),
                    "bcmfpActionCfgDb");
        stage->action_cfg_db = action_cfg_db;
    }

    BCMFP_ALLOC(action_cfg,
                sizeof(bcmfp_action_cfg_t),
                "bcmfpActionCfg");

    sal_memcpy(action_cfg, new_action_cfg, sizeof(bcmfp_action_cfg_t));

    if (action_cfg_db->action_cfg[action] == NULL) {
        action_cfg_db->num_actions++;
        action_cfg_db->action_cfg[action] = action_cfg;
    } else {
        action_cfg->next = action_cfg_db->action_cfg[action];
        action_cfg_db->action_cfg[action] = action_cfg;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage)
{
    bcmfp_action_t action = 0;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_cfg_db_t *action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    action_cfg_db = stage->action_cfg_db;

    if (action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTIONS_COUNT; action++) {

            while (action_cfg_db->action_cfg[action] != NULL) {
                action_cfg = action_cfg_db->action_cfg[action];
                action_cfg_db->action_cfg[action] = action_cfg->next;
                SHR_FREE(action_cfg);
            }

            action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(action_cfg_db);
    }

    stage->action_cfg_db = NULL;

    action_cfg_db = stage->presel_action_cfg_db;

    if (action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTIONS_COUNT; action++) {

            while (action_cfg_db->action_cfg[action] != NULL) {
                action_cfg = action_cfg_db->action_cfg[action];
                action_cfg_db->action_cfg[action] = action_cfg->next;
                SHR_FREE(action_cfg);
            }

            action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(action_cfg_db);
        stage->presel_action_cfg_db = NULL;
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_param_check(int unit,
                         uint32_t stage_id,
                         uint32_t group_id,
                         uint32_t action,
                         uint32_t param)
{
    uint8_t width = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->action_cfg_db->
            action_cfg[action]->offset_cfg.value[0] == -1) {
        BCMFP_ACTION_WIDTH_GET(width, action, stage);
        BCMFP_ACTION_CHECK(width, param);
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmfp_action_viewtype_set(int unit,
                          uint32_t stage_id,
                          uint32_t group_id,
                          uint32_t action,
                          uint32_t param,
                          uint32_t **edw,
                          bcmfp_action_viewtype_t viewtype,
                          uint8_t  part_id)
{
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_stage_t *stage = NULL;
    uint32_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    /*
     * Value zero for default value actions indicate
     * delete or don't configure
     */
    if (param == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ACTION_VIEWTYPE_OFFSET_GET(unit,
                                     stage,
                                     action,
                                     &a_offset,
                                     viewtype);
    for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
        if (-1 == a_offset.value[idx]) {
            a_offset.value[idx] = param;
        }
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE)) {
        BCMFP_ACTION_SET(unit, edw[part_id], &a_offset);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw,
                 uint8_t  part_id)
{
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_stage_t *stage = NULL;
    uint32_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    /*
     * Value zero for default value actions indicate
     * delete or don't configure
     */
    if (param == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ACTION_OFFSET_GET(unit, stage, action, &a_offset);
    for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
        if (-1 == a_offset.value[idx]) {
            a_offset.value[idx] = param;
        }
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE)) {
        BCMFP_ACTION_SET(unit, edw[part_id], &a_offset);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_action_viewtype_offset_get(int unit, bcmfp_stage_t *stage,
                                 bcmfp_action_t action,
                                 bcmfp_action_offset_cfg_t *a_cfg_ptr,
                                 bcmfp_action_viewtype_t viewtype)
{
    bcmfp_action_offset_cfg_t *a_cfg_ptr2 = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    action_cfg = stage->action_cfg_db->action_cfg[action];

    while (action_cfg != NULL &&
            !(action_cfg->flags & BCMFP_ACTION_NOT_VALID)) {
        if (action_cfg->viewtype == viewtype) {
            a_cfg_ptr2 = &(action_cfg->offset_cfg);
            break;
        }
        action_cfg = action_cfg->next;
    }
    if (a_cfg_ptr2 == NULL) {
        if (stage->action_cfg_db->action_cfg[action] != NULL &&
            (stage->action_cfg_db->action_cfg[action]->flags &
             BCMFP_ACTION_NOT_VALID)) {
            LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit, "BCMFP: Action is not"
                " mapped to physical container.Action: %s\n"),
                stage->action_cfg_db->action_cfg[action]->action_name));
        }
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(a_cfg_ptr, 0, sizeof(bcmfp_action_offset_cfg_t));
    sal_memcpy(a_cfg_ptr, a_cfg_ptr2, sizeof(bcmfp_action_offset_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_offset_get(int unit, bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    bcmfp_action_offset_cfg_t *a_cfg_ptr2;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    if (stage->action_cfg_db->action_cfg[action] != NULL &&
        !(stage->action_cfg_db->action_cfg[action]->flags & BCMFP_ACTION_NOT_VALID)) {
        a_cfg_ptr2 = &(stage->action_cfg_db->action_cfg[action]->offset_cfg);
    } else  {
        if (stage->action_cfg_db->action_cfg[action] != NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit, "BCMFP: Action is not"
                " mapped to physical container.Action: %s\n"),
                stage->action_cfg_db->action_cfg[action]->action_name));
        }
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    sal_memset(a_cfg_ptr, 0, sizeof(bcmfp_action_offset_cfg_t));
    sal_memcpy(a_cfg_ptr, a_cfg_ptr2, sizeof(bcmfp_action_offset_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_value_set(int unit,
                       uint32_t *entbuf,
                       bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    uint32_t idx;
    uint32_t offset;
    uint32_t width;
    uint32_t value;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
        if (a_cfg_ptr->width[idx] > 0 ) {
            offset = a_cfg_ptr->offset[idx];
            width  = a_cfg_ptr->width[idx];
            value  = a_cfg_ptr->value[idx];
            bcmdrd_field_set(entbuf, offset, ((offset + width) - 1), &value);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_data_size_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_action_t action_id,
                           uint32_t *size)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    action_cfg = stage->action_cfg_db->action_cfg[action_id];

    *size = 0;
    if (action_cfg != NULL) {
        if (action_cfg->offset_cfg.value[0] == -1) {
            *size = action_cfg->offset_cfg.width[0];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* New action set fucntions for PDD style policies */
int
bcmfp_pdd_action_set(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_group_id_t group_id,
                     bcmfp_pdd_oper_type_t pdd_type,
                     bcmfp_group_oper_info_t *opinfo,
                     bcmfp_action_t action,
                     uint32_t action_data,
                     uint32_t **edw)
{
    uint8_t    miss_count = 0;
    uint8_t    parts_cnt = 0;
    uint8_t    part_idx = 0;
    uint16_t   idx = 0;
    uint32_t   data[1] = { 0 };
    shr_error_t rv = SHR_E_NONE;
    bcmfp_action_oper_info_t a_offset[BCMFP_ENTRY_PARTS_MAX];
    bcmfp_action_offset_cfg_t act_offset_cfg;
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_oper_info_t *act_offset_ptr = NULL;
    uint8_t num_parts = 0;
    uint8_t resolved_part_idx = 0;
    uint16_t data_offset = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ACTION_OFFSET_GET(unit, stage, action, &act_offset_cfg);

    /* Retrieve group pdd entry parts */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_num_parts_get(unit,
                                 stage_id,
                                 pdd_id,
                                 pdd_type,
                                 &parts_cnt));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    /* Update each chunk size of the action. */
    for (part_idx = 0; part_idx < parts_cnt; part_idx++) {
        /*
         * Retrieve the action offsets for the particular
         * entry part.
         */
        rv = bcmfp_pdd_action_offset_info_get(unit,
                                              stage_id,
                                              pdd_id,
                                              part_idx,
                                              action,
                                              pdd_type,
                                              &a_offset[part_idx]);

        if (rv == SHR_E_NOT_FOUND) {
            miss_count++;
            continue;
        } else if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    /* if the action data not present in any part */
    if (miss_count == parts_cnt) {
        return SHR_E_NOT_FOUND;
    }

    /* Update the edw. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {

        act_offset_ptr = &a_offset[part_idx];

        /* Extract relevant data for this partion from user passed data. */
        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (0 == act_offset_ptr->width[idx]) {
                continue;
            }

            if (BCMFP_STAGE_FLAGS_TEST(stage,
                      BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
                resolved_part_idx = (num_parts - 1) - part_idx;
            } else {
                resolved_part_idx = part_idx;
            }

            if (act_offset_cfg.value[idx] != -1) {
                data[0] = act_offset_cfg.value[idx];
                SHR_BITCOPY_RANGE(edw[resolved_part_idx],
                                  act_offset_ptr->offset[idx],
                                  data,
                                  0,
                                  act_offset_ptr->width[idx]);
            } else {
                data[0] = action_data;
                SHR_BITCOPY_RANGE(edw[resolved_part_idx],
                                  act_offset_ptr->offset[idx],
                                  data,
                                  data_offset,
                                  act_offset_ptr->width[idx]);
                data_offset += act_offset_ptr->width[idx];
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_em_action_set(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_group_id_t group_id,
                    bcmfp_pdd_oper_type_t pdd_type,
                    bcmfp_group_oper_info_t *opinfo,
                    bcmfp_pdd_id_t pdd_id,
                    bcmfp_action_t action,
                    uint32_t param,
                    uint32_t **edw,
                    uint32_t **qos)
{
    uint32_t action_flag = 0;
    bcmfp_action_cfg_t *action_cfg = NULL;
    uint8_t part_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_INTERNAL);
    SHR_NULL_CHECK(opinfo, SHR_E_INTERNAL);

    /* Get Action Configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_cfg_get(unit,
                              stage_id,
                              action,
                              &action_cfg));

    /* QOS profile actions */
    action_flag = BCMFP_ACTIONS_IN_FLAT_POLICY;
    if (action_cfg->flags & action_flag) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_set(unit,
                              stage_id,
                              group_id,
                              action,
                              param,
                              qos,
                              part_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_action_set(unit,
                                  stage_id,
                                  pdd_id,
                                  group_id,
                                  pdd_type,
                                  opinfo,
                                  action,
                                  param,
                                  edw));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_colored_actions_enable(int unit,
                             uint32_t stage_id,
                             uint32_t **edw,
                             uint32_t value)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE)) {
        bcmdrd_field_set(edw[0],
            stage->misc_info->colored_actions_enable->offset[0],
            ((stage->misc_info->colored_actions_enable->offset[0] +
            stage->misc_info->colored_actions_enable->width[0]) - 1),
            &value);
    }

exit:
    SHR_FUNC_EXIT();
}
