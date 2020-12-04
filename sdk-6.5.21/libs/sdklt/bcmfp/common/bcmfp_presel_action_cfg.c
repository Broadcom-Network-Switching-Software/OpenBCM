/*! \file bcmfp_presel_action_cfg.c
 *
 * Utility APIs to operate with FP preselection actions.
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/* Private Function Declarations. */
static int
presel_action_offset_get(int unit,
                         bcmfp_stage_t *stage,
                         bcmfp_action_t action,
                         bcmfp_action_offset_cfg_t *a_cfg_ptr);

#define PRESEL_ACTION_OFFSET_GET(_unit_,             \
                                 _stage_,            \
                                 _action_,           \
                                 _a_offset_)         \
    do {                                             \
        SHR_IF_ERR_EXIT(                             \
            presel_action_offset_get((_unit_),       \
                                     (_stage_),      \
                                     (_action_),     \
                                     (_a_offset_))); \
    } while (0)

/* Private Function Definitions */
static int
presel_action_offset_get(int unit,
                         bcmfp_stage_t *stage,
                         bcmfp_action_t action,
                         bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    bcmfp_action_offset_cfg_t *a_cfg_ptr2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT((action >= BCMFP_ACTIONS_COUNT) ? SHR_E_PARAM : SHR_E_NONE);

    a_cfg_ptr2 = &(stage->presel_action_cfg_db->action_cfg[action]->offset_cfg);
    sal_memset(a_cfg_ptr, 0, sizeof(bcmfp_action_offset_cfg_t));
    sal_memcpy(a_cfg_ptr, a_cfg_ptr2, sizeof(bcmfp_action_offset_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_t_init(int unit,
                               bcmfp_action_cfg_t *presel_action_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_action_cfg, SHR_E_PARAM);

    sal_memset(presel_action_cfg, 0, sizeof(bcmfp_action_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *new_action_cfg)
{
    bcmfp_action_cfg_t *presel_action_cfg = NULL;
    bcmfp_action_cfg_db_t *presel_action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_action_cfg, SHR_E_PARAM);

    presel_action_cfg_db = stage->presel_action_cfg_db;

    if (presel_action_cfg_db == NULL) {
        BCMFP_ALLOC(presel_action_cfg_db,
                    sizeof(bcmfp_action_cfg_db_t),
                    "bcmfpActionCfgDb");
        stage->presel_action_cfg_db = presel_action_cfg_db;
    }

    BCMFP_ALLOC(presel_action_cfg,
                sizeof(bcmfp_action_cfg_t),
                "bcmfpActionCfg");

    sal_memcpy(presel_action_cfg, new_action_cfg, sizeof(bcmfp_action_cfg_t));

    if (presel_action_cfg_db->action_cfg[action] == NULL) {
        presel_action_cfg_db->num_actions++;
        presel_action_cfg_db->action_cfg[action] = presel_action_cfg;
    } else {
        presel_action_cfg->next = presel_action_cfg_db->action_cfg[action];
        presel_action_cfg_db->action_cfg[action] = presel_action_cfg;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage)
{
    bcmfp_action_t action = 0;
    bcmfp_action_cfg_t *presel_action_cfg = NULL;
    bcmfp_action_cfg_db_t *presel_action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    presel_action_cfg_db = stage->presel_action_cfg_db;

    if (presel_action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTIONS_COUNT; action++) {

            while (presel_action_cfg_db->action_cfg[action] != NULL) {
                presel_action_cfg = presel_action_cfg_db->action_cfg[action];
                presel_action_cfg_db->action_cfg[action] = presel_action_cfg->next;
                SHR_FREE(presel_action_cfg);
            }

            presel_action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(presel_action_cfg_db);
    }

    stage->presel_action_cfg_db = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t action,
                 uint32_t param,
                 bcmfp_action_flags_t flags,
                 bcmfp_group_oper_info_t *opinfo,
                 uint32_t **edw)
{
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_stage_t *stage = NULL;
    uint8_t num_parts = 1;
    uint8_t part = 0;
    uint32_t group_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    PRESEL_ACTION_OFFSET_GET(unit, stage, action, &a_offset);

    if (flags == BCMFP_ACTION_FLAGS_ALL_PARTS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_mode_to_flags_get(unit,
                                           stage,
                                           opinfo->group_mode,
                                           &group_flags));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_parts_count(unit,
                                     TRUE,
                                     group_flags,
                                     &num_parts));
    }

    for (part = 0; part < num_parts; part++) {
        if (-1 == a_offset.value[0])
        {
            a_offset.value[0] = param;
        }
        BCMFP_ACTION_SET(unit, edw[part], &a_offset);
    }
exit:
    SHR_FUNC_EXIT();
}
