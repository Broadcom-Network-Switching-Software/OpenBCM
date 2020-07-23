/*! \file field_entry.c
 *
 * Field Module Entry Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bsl/bsl.h>
#include <bcm/field.h>

#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/mbcm/field.h>

#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/policer.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

STATIC int
ltsw_field_entry_flexctr_action_prof_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    uint32_t flexctr_action_id,
    uint32_t counter_idx,
    uint64_t *flex_ctr_action,
    char **opq_obj,
    uint32_t *opq_obj_val,
    char **flexctr_cont_obj,
    uint64_t *flexctr_cont_id,
    bcm_color_t *color);

/*!
 * \brief bcmint_field_stage_from_entry_get
 *
 * Depending on the range of Entry ID and validity of entry ID
 * determine the stage_id.
 *
 * \param [in]   unit        - Unit Number.
 * \param [in]   entry_id    - Entry ID.
 * \param [out]  stage_info  - Pipeline stage info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_stage_from_entry_get(int unit,
                                  bcm_field_entry_t entry,
                                  bcmint_field_stage_info_t **stage_info)
{
    int entry_id = 0;
    bool stage_found = FALSE;
    bcmi_ltsw_field_stage_t stage_id;

    SHR_FUNC_ENTER(unit);

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_stage_info_get(unit, stage_id, stage_info));

        if (((*stage_info)->flags & BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL)
            && (BCMINT_FIELD_IS_PRESEL_ENTRY(entry))) {
            /* Verify wheter the given ID is in Entry ID's range */
            if ((entry >
                (BCM_FIELD_QUALIFY_PRESEL | (*stage_info)->stage_base_id)) &&
                (entry <=
               ((BCM_FIELD_QUALIFY_PRESEL | (*stage_info)->stage_base_id) +
                (*stage_info)->presel_info->pid_max))) {

                /* Mask the stage bits from given entry ID */
                entry_id = entry & BCM_FIELD_ID_MASK;

                /* Check whether the corresponding entry ID exists. */
                if (SHR_BITGET
                     ((*stage_info)->presel_info->pid_bmp.w, entry_id)) {
                    stage_found = TRUE;
                    break;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                        "Error: Entry does not exists\n")));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
                }
            }
        } else {
            /* Verify wheter the given ID is in Entry ID's range */
            if ((entry > (*stage_info)->stage_base_id) &&
                (entry <=
               ((*stage_info)->stage_base_id +
                                     (*stage_info)->entry_info->eid_max))) {

                /* Mask the stage bits from given entry ID */
                entry_id = entry & BCM_FIELD_ID_MASK;

                /* Check whether the corresponding entry ID exists. */
                if (SHR_BITGET
                         ((*stage_info)->entry_info->eid_bmp.w, entry_id)) {
                    stage_found = TRUE;
                    break;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                       (BSL_META_U(unit,
                       "Error: Entry does not exists\n")));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
                }
            }
        }
    }

    if (stage_found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit,
           "Error: Entry ID is not in valid range\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Do search for given entry id and return success or failure.
 *
 * Algorithm:
 *  1. Application provides required information about the entry
 *     to identify the entry location in entry hash.
 *  2. Calculate the hash for the given entry Id.
 *  3. Check entry id is present in linked list of entry ids
 *     having the same hash value.
 *  4. If entry_id exists, return corresponding entry hash pointer.
 *  5. If entry_id does not exist, return error code "Not Found".
 */
int
bcmint_field_entry_oper_lookup(
    int unit,
    int entry_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_entry_oper_t **entry_oper)
{
    uint32_t hash_offset = 0;
    bcmi_field_ha_entry_oper_t *ent_oper = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given entry ID */
    entry_id = entry_id & BCM_FIELD_ID_MASK;

    /* Calculate the hash */
    hash_offset = entry_id % stage_info->entry_info->hash_size;
    FP_HA_ENT_OPER_GET(unit, stage_info->stage_id,
                       hash_offset, ent_oper);

    if (!ent_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    do {
        if (ent_oper->entry_id == entry_id) {
            break;
        }
        FP_HA_ENT_OPER_NEXT(unit, ent_oper, ent_oper);
    } while (ent_oper != NULL);

    /* Return not found if hash ptr is null */
    if (!ent_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (entry_oper != NULL) {
        *entry_oper = ent_oper;
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Insert the given entry ID at correct hash index
 *
 * Algorithm:
 *  1. Application provides required information about the entry
 *     to identify the entry location in entry hash
 *  2. Calculate the hash for the given entry Id.
 *  3. Check entry id is present in linked list of entry ids
 *     having the same hash value.
 *  4. If entry_id exists, return error. code has some bug
 *  5. If entry_id does not exist, create allocate a entry hash node and
 *     insert at proper location.
 */
static int
ltsw_field_entry_oper_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry_id,
    int group_id,
    bcmi_field_ha_entry_oper_t **entry_oper)
{
    uint8_t ha_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_entry_oper_t *oper = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    entry_id = entry_id & BCM_FIELD_ID_MASK;

    /* Calculate the hash */
    hash_offset = entry_id % stage_info->entry_info->hash_size;

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           hash_offset,
                           entry_base);

    FP_STAGE_ENTRY_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_ha_blk_element_acquire(unit, blk_type,
                                            (void *)&oper,
                                            &ha_blk_id,
                                            &ha_blk_offset));

    if (entry_base->blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
        entry_base->blk_id = ha_blk_id;
        entry_base->blk_offset = ha_blk_offset;
        oper->next.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;;
        oper->next.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
    } else {
        oper->next.blk_id = entry_base->blk_id;
        oper->next.blk_offset = entry_base->blk_offset;
        entry_base->blk_id = ha_blk_id;
        entry_base->blk_offset = ha_blk_offset;
    }

    /* Update group operation structure */
    oper->entry_id = entry_id;
    oper->group_id = group_id;
    oper->entry_flags = 0;
    oper->color_action_id = -1;
    oper->policer_id = 0;
    oper->strength_idx = 0;
    oper->color_strength_idx = -1;

    if (entry_oper != NULL) {
        *entry_oper = oper;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Remove given entry ID at correct hash index
 */
int
bcmint_field_entry_oper_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry_id)
{
    int blk_type;
    uint8_t ha_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    bcmi_field_ha_entry_oper_t *ent_oper = NULL;
    bcmi_field_ha_entry_oper_t *ent_oper_head = NULL;
    bcmi_field_ha_entry_oper_t *ent_oper_temp = NULL;
    bcmi_field_ha_entry_info_t *entry_info = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    entry_id = entry_id & BCM_FIELD_ID_MASK;

    entry_info = stage_info->entry_info;
    SHR_NULL_CHECK(entry_info, SHR_E_INTERNAL);

    /* Calculate the hash */
    hash_offset = entry_id % entry_info->hash_size;

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);

    FP_HA_ENT_BASE_OPER_GET(unit,
                            stage_info->stage_id,
                            entry_base,
                            hash_offset,
                            ent_oper_head);
    entry_base = entry_base + hash_offset;
    if ((entry_base == NULL) || (ent_oper_head == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    ent_oper = ent_oper_head;
    do {
        if (ent_oper->entry_id == entry_id) {
            if (ent_oper_temp == NULL) {
                ha_blk_id = entry_base->blk_id;
                ha_blk_offset = entry_base->blk_offset;
                entry_base->blk_id = ent_oper->next.blk_id;
                entry_base->blk_offset = ent_oper->next.blk_offset;
            } else {
                ha_blk_id = ent_oper_temp->next.blk_id;
                ha_blk_offset = ent_oper_temp->next.blk_offset;
                ent_oper_temp->next.blk_id = ent_oper->next.blk_id;
                ent_oper_temp->next.blk_offset = ent_oper->next.blk_offset;
            }
            break;
        }
        ent_oper_temp = ent_oper;
        FP_HA_ENT_OPER_NEXT(unit, ent_oper, ent_oper);
    } while (ent_oper != NULL);

    /* Return not found if hash ptr is null */
    if (!ent_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    FP_STAGE_ENTRY_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_blk_element_release(unit, ha_blk_id,
                                             ha_blk_offset,
                                             blk_type));


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_default_entry_lt_create
 *
 * Create a field default entry in a given stage with given ID which
 * belongs to given group ID.
 *
 * \param [in] unit         - BCM device number.
 * \param [in] stage_info   - stage info
 * \param [in] entry_config - Entry configuration
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *    !BCM_E_NONE      - Failure
 */
static int
bcmint_field_default_entry_lt_create(int unit,
                                  bcmint_field_stage_info_t *stage_info,
                                  bcm_field_entry_config_t *entry_config)

{
    int dunit = 0;
    int act_pipe = -1;
    bool pdd_template_created = FALSE;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t pdd_template = BCMLT_INVALID_HDL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    int color_pdd_idx = -1;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        /* create a pdd for default entry */
        /* default entry pdd will be allocated after max group Id and
         * they are allocated on one to one mapping with their grop id
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                stage_info->tbls_info->pdd_sid, &pdd_template));

        /* Add pdd_template_id field to PDD LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(pdd_template,
                stage_info->tbls_info->pdd_key_fid,
                (((stage_info->group_info->gid_max) +
                ((entry_config->group_id) & BCM_FIELD_ID_MASK)))));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit, entry_config->group_id,
                stage_info, &group_oper));

        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                    entry_config->group_id,
                                                    &act_pipe));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_process_group_aset(unit,
                stage_info, 1,
                act_pipe,
                &entry_config->aset,
                group_oper->hintid,
                &pdd_template, group_oper->policer_pool_id,
                &color_pdd_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, pdd_template,
                BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        pdd_template_created = TRUE;
    }

    /* Entry handle allocate for Policy Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
           stage_info->tbls_info->policy_sid, &policy_template));

    /* Add policy_template_id field to Policy LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(policy_template,
           stage_info->tbls_info->policy_key_fid,
           (entry_config->entry_id) & BCM_FIELD_ID_MASK));

    /* Insert policy template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, policy_template,
            BCMLT_OPCODE_INSERT,BCMLT_PRIORITY_NORMAL));

exit:
    if (SHR_FUNC_ERR()) {
        if (TRUE == pdd_template_created) {
            (void) bcmi_lt_entry_commit(unit, pdd_template,
                                    BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL);
        }
    }
    if (BCMLT_INVALID_HDL != policy_template) {
        (void) bcmlt_entry_free(policy_template);
    }

    if (BCMLT_INVALID_HDL != pdd_template) {
        (void) bcmlt_entry_free(pdd_template);
    }

    SHR_FUNC_EXIT();
}

static int
bcmint_field_entry_lt_create(int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_config_t *entry_config)
{
    int dunit = 0;
    bcm_field_entry_t entry_id = 0;
    bool rule_template_created = FALSE;
    bcmlt_entry_handle_t rule_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    entry_id = (entry_config->entry_id) & BCM_FIELD_ID_MASK;

    /* Entry handle allocate for Rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
           stage_info->tbls_info->rule_sid, &rule_template));
    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(rule_template,
           stage_info->tbls_info->rule_key_fid, entry_id));

    /* Insert rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, rule_template,
             BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

    rule_template_created = TRUE;

    /* Entry handle allocate for Policy Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
           stage_info->tbls_info->policy_sid, &policy_template));

    /* Add policy_template_id field to Policy LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(policy_template,
           stage_info->tbls_info->policy_key_fid, entry_id));

    /* Insert policy template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, policy_template,
            BCMLT_OPCODE_INSERT,BCMLT_PRIORITY_NORMAL));

exit:
    if (SHR_FUNC_ERR()) {
        if (TRUE == rule_template_created) {
        (void) bcmi_lt_entry_commit(unit, rule_template,
                                    BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL);
        }
    }
    if (BCMLT_INVALID_HDL != rule_template) {
        (void) bcmlt_entry_free(rule_template);
    }
    if (BCMLT_INVALID_HDL != policy_template) {
        (void) bcmlt_entry_free(policy_template);
    }
    SHR_FUNC_EXIT();
}
/*
 * Function: bcmint_field_entry_destroy
 * Purpose:
 *     Delete entry and associated templates if any
 * Parameters:
 *     unit  - BCM device number
 *     entry - (IN) New entry
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 */
int
bcmint_field_entry_destroy(int unit,
                           bcm_field_entry_t entry,
                           bcmi_field_ha_entry_oper_t *entry_oper)
{
    int dunit = 0;
    bcm_field_entry_t entry_id = 0;
    bcmint_field_tbls_info_t *tbls_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t rule_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t pdd_template = BCMLT_INVALID_HDL;
    bcm_field_entry_config_t entry_config;
    bool slice_reserve_entry_enable = FALSE;
    bcm_field_group_t group_id = 0;
    uint64_t active_entries_count = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    tbls_info = stage_info->tbls_info;

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    if (entry_oper == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &entry_oper));
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, tbls_info->group_sid,
                                  &grp_template));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template, tbls_info->group_key_fid,
                                   entry_oper->group_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, grp_template, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                                   tbls_info->def_pdd_key_fid, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                                   tbls_info->def_sbr_key_fid, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                                   tbls_info->def_policy_key_fid, 0));
        /* remove the read only field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_remove(grp_template,
                                      stage_info->tbls_info->grp_oper_mode_fid));
         /* update group template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, grp_template,
                                      BCMLT_PRIORITY_NORMAL));
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_stage_based_id_get(unit,
                                                   stage_info->stage_id,
                                                   entry_oper->group_id,
                                                   &group_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_group_active_entries_get(unit,
                                                 group_id,
                                                 &active_entries_count));
        if (active_entries_count == 1)  {
            /* Not a slice reserve and not a invalidated entry. */
            if (!(entry_oper->entry_flags &
                        BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO) &&
                    !(entry_oper->entry_flags &
                        BCMINT_FIELD_ENTRY_HW_INVALIDATED)) {
                /* Install slice_reserve_entry  during active entry deletion */
                slice_reserve_entry_enable = TRUE;
            }
            /* Allow deletion of slice_reserve_entry */
        }
    }

    /* Detach the stat */
    if (entry_oper->flexctr_action_id) {
        bcm_field_flexctr_config_t flexctr_cfg;

        flexctr_cfg.flexctr_action_id = entry_oper->flexctr_action_id;
        if (entry_oper->counter_color == 0) {
            flexctr_cfg.color = 0;
            flexctr_cfg.counter_index = entry_oper->counter_idx;
        } else {
            flexctr_cfg.color = entry_oper->counter_color;
            flexctr_cfg.counter_index = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_field_entry_flexctr_detach(unit, entry,
                                                &flexctr_cfg));
    }

    /* Detach the policer */
    if (entry_oper->policer_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_policer_detach(unit, entry, 0));
    }

    /* Free all the Action's associated to the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_delete_all(unit, entry));

    /* Entry handle allocate for Entry Template */
    if ((entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) ||
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_HW_INVALIDATED)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->entry_sid,
                                  &entry_template));
        /* Add entry_id field to Entry LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_template,
                                   tbls_info->entry_key_fid,
                                   entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_template,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    /* Entry handle allocate for Rule Template */
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->pdd_sid,
                                  &pdd_template));
         /* Add policy_template_id field to Policy LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(pdd_template,
                tbls_info->pdd_key_fid,
                (((stage_info->group_info->gid_max) +
                ((entry_oper->group_id) & BCM_FIELD_ID_MASK)))));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, pdd_template,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));

        /* Entry handle allocate for Policy Template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->policy_sid,
                                  &policy_template));
        /* Add policy_template_id field to Policy LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(policy_template,
                                   tbls_info->policy_key_fid,
                                   entry_id));
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, policy_template,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DEFAULT_ONLY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->rule_sid, &rule_template));
        /* Add rule_template_id field to Rule LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(rule_template,
                                   tbls_info->rule_key_fid, entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, rule_template,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

        /* Entry handle allocate for Policy Template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->policy_sid, &policy_template));
        /* Add policy_template_id field to Policy LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(policy_template,
                                   tbls_info->policy_key_fid, entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, policy_template,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    }

    /* Delete from entry hash */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_remove(unit,
                                        stage_info,
                                        entry_id));
    /* Clear entry id bitmap */
    SHR_BITCLR(stage_info->entry_info->eid_bmp.w, entry_id);

    /* Skip installing slice_reserve_entry, during slice_reserve_entry delete. */
    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) &&
            (slice_reserve_entry_enable)) {
        bcm_field_entry_config_t_init(&entry_config);
        entry_config.group_id = group_id;
        entry_config.entry_id = entry;
        entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_ID;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_config_create(unit,
                                              &entry_config));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit,
                                            entry_id,
                                            stage_info,
                                            &entry_oper));
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_install(unit,
                                        entry));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_template) {
        (void) bcmlt_entry_free(entry_template);
    }
    if (BCMLT_INVALID_HDL != rule_template) {
        (void) bcmlt_entry_free(rule_template);
    }
    if (BCMLT_INVALID_HDL != policy_template) {
        (void) bcmlt_entry_free(policy_template);
    }
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    if (BCMLT_INVALID_HDL != pdd_template) {
        (void) bcmlt_entry_free(pdd_template);
    }
    SHR_FUNC_EXIT();
}


/*
 * Function: bcmint_field_entry_destroy_all
 * Purpose:
 *     Delete all entries present in system or for a given group ID.
 * Parameters:
 *     unit  - BCM device number
 *     group - (IN) Field Group ID or BCMINT_FIELD_ID_INVALID
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *
 * Note: If (group == BCMINT_FIELD_ID_INVALID), delete all entries.
 */
int
bcmint_field_entry_destroy_all(int unit, bcm_field_group_t group)
{
    int idx, rv;
    int group_id = 0;
    bcm_field_entry_t entry = 0;
    bcmi_ltsw_field_stage_t stage_id;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *oper = NULL;
    bcmi_field_ha_entry_oper_t *oper_next = NULL;

    SHR_FUNC_ENTER(unit);

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount;
         stage_id++) {
        rv = bcmint_field_stage_info_get(unit,
                                         stage_id,
                                         &stage_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        if (group != BCMINT_FIELD_ID_INVALID) {
            if ((stage_info->stage_base_id & group) !=
                 stage_info->stage_base_id) {
                continue;
            }
            group_id = group & BCM_FIELD_ID_MASK;
        }

        FP_HA_ENT_BASE_BLK_GET(unit,
                               stage_info->stage_id,
                               0,
                               entry_base);
        for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
            oper = NULL;
            FP_HA_ENT_BASE_OPER_GET(unit,
                                    stage_info->stage_id,
                                    entry_base,
                                    idx,
                                    oper);
            if (oper == NULL) {
                continue;
            }

            do {
                FP_HA_ENT_OPER_NEXT(unit, oper, oper_next);
                if ((group_id == 0) ||
                    (oper->group_id == group_id)) {
                    entry = stage_info->stage_base_id | oper->entry_id;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_destroy(unit, entry, oper));
                }
                oper = oper_next;
            } while (oper != NULL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_entry_create
 * Purpose:
 *   Create a blank entry based on a group.
 *   Automatically generate an entry
 *   ID. Create Rule and Policy templates.
 * Parameters:
 *   unit  - BCM device number
 *   entry_config - (OUT) configuration for new entry creation
 * Returns:
 *   BCM_E_INIT        BCM unit not initialized
 *   BCM_E_NOT_FOUND   group not found in unit
 *   BCM_E_PARAM       *entry was NULL
 *   BCM_E_RESOURCE    No unused entries available
 *   BCM_E_XXX         Error if any
 */

int
bcmint_field_entry_config_create(int unit,
                                 bcm_field_entry_config_t *entry_config)
{
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int idx = 0;
    int entry_id = 0;
    int group_id = 0;
    bool entry_id_found = FALSE;
    uint64_t ent_lt_prio_min = 0;
    uint64_t ent_lt_prio_max = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(entry_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_group_get(unit,
       entry_config->group_id, &stage_info));
    group_id = (entry_config->group_id) & BCM_FIELD_ID_MASK;

    /* check if aset passed is supported for the stage */
    if (entry_config->flags & BCM_FIELD_ENTRY_CREATE_AS_DEFAULT) {
        if ((((entry_config->group_id) & ~BCM_FIELD_ID_MASK) !=
              BCM_FIELD_EM_ID_BASE) &&
            (((entry_config->group_id) & ~BCM_FIELD_ID_MASK) !=
              BCM_FIELD_FT_ID_BASE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        for (idx = 0; idx < bcmFieldActionCount; idx++) {
            if (SHR_BITGET(entry_config->aset.w, idx)) {
                if (bcmint_field_action_map_find
                    (unit, idx, stage_info, &lt_map) != SHR_E_NONE) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit, group_id,
            stage_info, &group_oper));
    }

    /* get the entry id based on flag passed*/
    if (entry_config->flags & BCM_FIELD_ENTRY_CREATE_WITH_ID) {
        if (((entry_config->group_id) & ~BCM_FIELD_ID_MASK) !=
            ((entry_config->entry_id) & ~BCM_FIELD_ID_MASK)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

    }

    /* Do not create if its for a reserved group (the first reserve grp ID) */
    if (entry_config->group_id ==
        (((stage_info->stage_base_id) | (stage_info->group_info->gid_max)) -
        BCMINT_FIELD_GROUP_RESERVE_GID_COUNT + 1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    /* find the entry Id*/
    if (entry_config->flags & BCM_FIELD_ENTRY_CREATE_WITH_ID) {
        /* Verify whether the given ID is in Stage Entry ID's range */
        if ((entry_config->entry_id > stage_info->stage_base_id) &&
            (entry_config->entry_id <=
            (stage_info->stage_base_id + stage_info->entry_info->eid_max))) {

            /* Mask the stage bits from given entry ID */
            entry_id = (entry_config->entry_id) & BCM_FIELD_ID_MASK;

            /* Check whether the corresponding Entry ID already exists. */
            if (SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry_id)) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                    "Error: Entry ID is already in use\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
        } else {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit,
               "Error: Entry ID is not in valid range\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        /* Generate Entry ID */
        entry_id = stage_info->entry_info->last_allocated_eid + 1;
        for (idx = 1; idx <= stage_info->entry_info->eid_max; idx++) {
            if (entry_id > stage_info->entry_info->eid_max) {
                entry_id = 1;
            }
            if (!SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry_id)) {
                entry_id_found = TRUE;
                break;
            }
            entry_id++;
        }

        if ((entry_id_found == FALSE) &&
                (idx > stage_info->entry_info->eid_max)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit,
               "Error: All Entry ID's are in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Add the stage bits to entry Id */
    entry_config->entry_id = ((stage_info->stage_base_id) | entry_id);

    /* Add entry in hash list */
    SHR_IF_ERR_VERBOSE_EXIT(ltsw_field_entry_oper_insert(unit,
        stage_info, entry_id, group_id, &entry_oper));

    if (entry_config->flags & BCM_FIELD_ENTRY_CREATE_WITH_PRIORITY) {
        if (stage_info->tbls_info->entry_priority_fid == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        /* check the range of entry priority */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit,
             stage_info->tbls_info->entry_sid,
             stage_info->tbls_info->entry_priority_fid,
             &ent_lt_prio_min, &ent_lt_prio_max));

        if ((entry_config->priority < ent_lt_prio_min) ||
            (entry_config->priority > ent_lt_prio_max)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        entry_oper->priority = entry_config->priority;
    }

    if (entry_config->flags & BCM_FIELD_ENTRY_CREATE_AS_DEFAULT) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_default_entry_lt_create(
            unit, stage_info, entry_config));
        FP_HA_GROUP_OPER_DEF_ENTRY_ASET_ARR_SET(unit,
            group_oper, entry_config->aset);
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_DEFAULT_ONLY;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_entry_lt_create(
            unit, stage_info, entry_config));
    }

    if (!(entry_config->flags & BCM_FIELD_ENTRY_CREATE_WITH_ID)) {
        /* Update last allocated entry ID */
        stage_info->entry_info->last_allocated_eid = entry_id;
    }

    /* Update the stage entry bitmap */
    SHR_BITSET(stage_info->entry_info->eid_bmp.w, entry_id);

exit:
    if ((SHR_FUNC_ERR()) && (entry_oper != NULL)) {
        (void)bcmint_field_entry_oper_remove(unit,
            stage_info, entry_id);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_entry_get
 * Purpose:
 *   Retrieve the configuration of
 *   the requested entry
 * Parameters:
 *   unit  - BCM device number
 *   entry_config - (INOUT) configuration of the requested entry
 * Returns:
 *   BCM_E_XXX         Error if any
 */

int
bcmint_field_entry_config_get(int unit,
                                 bcm_field_entry_config_t *entry_config)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    int entry_id = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    uint64_t gp_counter_id = 0;
    uint64_t rp_counter_id = 0;
    uint64_t yp_counter_id = 0;
    uint64_t counter_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(entry_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_entry_get(unit,
       entry_config->entry_id, &stage_info));
    entry_id = (entry_config->entry_id) & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &entry_oper));
    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    entry_config->flags = 0;
    entry_config->status_flags = 0;
    BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                            entry_oper->group_id,
                                            entry_config->group_id);
    entry_config->priority = entry_oper->priority;

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_INSTALLED;
        if (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED)) {
            entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_ENABLED;
        }
    }
    entry_config->priority = entry_oper->priority;

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_INSTALLED;
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_ENABLED;
    }

    if (entry_oper->flexctr_action_id) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_FLEXCTR_ATTACHED;
        entry_config->flex_counter_action_id = entry_oper->flexctr_action_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_flex_counter_info_get(unit,
                                              entry_oper,
                                              group_oper,
                                              stage_info,
                                              &counter_id,
                                              &gp_counter_id,
                                              &rp_counter_id,
                                              &yp_counter_id));
        entry_config->counter_id = counter_id;
        entry_config->gp_counter_id = COMPILER_64_LO(gp_counter_id);
        entry_config->rp_counter_id = COMPILER_64_LO(rp_counter_id);
        entry_config->yp_counter_id = COMPILER_64_LO(yp_counter_id);
    }

    if (entry_oper->policer_id) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_POLICER_ATTACHED;
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        entry_config->status_flags |= BCM_FIELD_ENTRY_STATUS_DEFAULT_ENTRY;
        FP_HA_GROUP_OPER_DEF_ENTRY_ASET_ARR_GET(unit,
            group_oper, entry_config->aset);
    } else {
        FP_HA_GROUP_OPER_ASET_ARR_GET(unit,
            group_oper, entry_config->aset);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_install(int unit,
                           bcm_field_entry_t entry)
{
    int pipe = -1;
    int rv = 0;
    int dunit = 0;
    bcm_field_group_t group_id = 0;
    const char *oper_stat;
    bcm_field_entry_t entry_id = 0;
    bcm_field_action_params_t params;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    uint64_t active_entries_count = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given group ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Get the entry hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        SHR_EXIT();
    }

    /* Entry handle allocate for Entry Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->entry_sid,
                             &entry_template));

    /* Add entry_id field to Entry LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(entry_template,
                              stage_info->tbls_info->entry_key_fid,
                              entry_id));

    /* Add group_template_id field to Entry LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(entry_template,
                              stage_info->tbls_info->group_key_fid,
                              entry_oper->group_id));

    if (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DEFAULT_ONLY)) {
        /* Add rule_template_id field to Entry LT. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_template,
                                  stage_info->tbls_info->entry_rule_key_fid,
                                  entry_id));

        /* Add policy_template_id field to Entry LT. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_template,
                                  stage_info->tbls_info->entry_policy_key_fid,
                                  entry_id));
    }

    /* Update entry priority in entry LT */
    if (stage_info->tbls_info->entry_priority_fid != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_template,
                                   stage_info->tbls_info->entry_priority_fid,
                                   entry_oper->priority));
    }
    /*update grp template if its a default entry */
    if (entry_oper->entry_flags &
        BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                stage_info->tbls_info->group_sid, &grp_template));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                stage_info->tbls_info->group_key_fid, entry_oper->group_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, grp_template,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                stage_info->tbls_info->def_pdd_key_fid,
                (((stage_info->group_info->gid_max) +
                entry_oper->group_id))));

         SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                stage_info->tbls_info->def_sbr_key_fid,
                (stage_info->strength_id_max) + 1));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_add(grp_template,
                 stage_info->tbls_info->def_policy_key_fid,
                 entry_oper->entry_id));
        /* remove the readonly field before commit*/
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_remove(grp_template,
                stage_info->tbls_info->grp_oper_mode_fid));

         /* update group template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, grp_template,
                BCMLT_PRIORITY_NORMAL));
    }

    /* If entry has stats attached, update in entry LT */
    if (entry_oper->flexctr_action_id) {
        char *opaque_object = NULL;
        char *flexctr_cont_obj = NULL;
        uint64_t flex_ctr_action = 0;
        uint64_t flexctr_cont_id = 0;
        uint32_t opaque_obj_val = 0;
        bcm_color_t color = bcmColorCount;
        bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

        /* Validate flexctr action and retrive the action profile index */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_entry_flexctr_action_prof_get(
                    unit, stage_info,
                    group_oper,
                    entry_oper->flexctr_action_id,
                    entry_oper->counter_idx,
                    &flex_ctr_action,
                    &opaque_object,
                    &opaque_obj_val,
                    &flexctr_cont_obj,
                    &flexctr_cont_id,
                    &color));

        if (NULL != flexctr_cont_obj) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(
                        unit, &policy_template,
                        BCMLT_OPCODE_UPDATE,
                        BCMLT_PRIORITY_NORMAL,
                        stage_info->tbls_info->policy_sid,
                        stage_info->tbls_info->policy_key_fid,
                        0, entry_id, NULL,
                        flexctr_cont_obj,
                        flexctr_cont_id,
                        0, 0,
                        1));
        }

        /* set counter index in the opaque object */
        if (opaque_object != NULL) {
            if (entry_oper->counter_color == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_lt_entry_commit(
                            unit, &policy_template,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL,
                            stage_info->tbls_info->policy_sid,
                            stage_info->tbls_info->policy_key_fid,
                            0, entry_id, NULL,
                            opaque_object,
                            opaque_obj_val,
                            0, 0,
                            1));
                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_action_entry_update(unit,
                                                       entry_oper,
                                                       bcmFieldActionStatGroup,
                                                       true));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                        entry_oper->group_id,
                                                        &pipe));
                if (entry_oper->counter_color & BCM_FIELD_COUNT_GREEN) {
                    params.param0 = entry_oper->g_counter_idx;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionGpStatGroup,
                                &params));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionGpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(unit,
                                                          entry_oper,
                                                          bcmFieldActionGpStatGroup,
                                                          true));
                }
                if (entry_oper->counter_color & BCM_FIELD_COUNT_YELLOW) {
                    params.param0 = entry_oper->y_counter_idx;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionYpStatGroup,
                                &params));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionYpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(unit,
                                                          entry_oper,
                                                          bcmFieldActionYpStatGroup,
                                                          true));
                }
                if (entry_oper->counter_color & BCM_FIELD_COUNT_RED) {
                    params.param0 = entry_oper->r_counter_idx;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionRpStatGroup,
                                &params));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionRpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(unit,
                                                          entry_oper,
                                                          bcmFieldActionRpStatGroup,
                                                          true));
                }
            }
        }

        /* Add flex action profile index to the counter key field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_template,
                                   stage_info->tbls_info->ctr_key_fid,
                                   flex_ctr_action));
    }

    if (entry_oper->policer_id) {
        bcm_policer_t policer_id = entry_oper->policer_id;
        if (stage_info->flags & BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_template,
                                       stage_info->tbls_info->meter_key_fid,
                                       (policer_id & BCM_FIELD_ID_MASK)));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_entry_policer_attach(unit,
                                                      stage_info,
                                                      entry_oper,
                                                      policer_id));
        }
    }

    if ((entry_oper->num_actions) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) &&
        (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE)) {
        uint64_t s_idx = 0;

        if (entry_oper->num_actions) {
            /* Create SBR strength profile */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_entry_strength_aset_create(
                        unit, stage_info, group_oper,
                        entry_oper, NULL, 0));

            s_idx = entry_oper->strength_idx;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_template,
                                       stage_info->tbls_info->sbr_key_fid,
                                       s_idx));

            if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
                (entry_oper->color_strength_idx != -1)) {
                int a;
                bcm_field_action_t action;

                for (a = 0; a < entry_oper->num_actions; a++) {
                    action = entry_oper->action_arr[a];
                    if (BCMINT_FIELD_IS_COLORED_ACTION(action)) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                                entry_oper->group_id,
                                                                &pipe));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_field_action_color_pdd_sbr_set(unit,
                                                                   stage_info,
                                                                   group_oper,
                                                                   entry_oper,
                                                                   pipe,
                                                                   action));
                        break;
                    }
                }
            }
        }
    }

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmi_lt_entry_set_commit(unit, entry_template,
                                   BCMLT_PRIORITY_NORMAL));

    /*
     * Retrieve the operation status for the installed entry.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_template,
                                      "OPERATIONAL_STATE",
                                      &oper_stat));

    if (sal_strcmp(oper_stat, "SUCCESS") != 0) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "Entry[%d] install failed: %s\n"), entry_id, oper_stat));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    if ((entry_oper->flexctr_action_id) &&
        !(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_STAT_ATTACHED)) {
        /* Increment the flexctr reference counter */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_flexctr_attach_counter_id_status_update(
                                         unit,
                                         entry_oper->flexctr_action_id));
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_STAT_ATTACHED;
    }

    /* Set entry install status */
    entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_INSTALLED;
    entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_DISABLED;
    entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_HW_INVALIDATED;

    if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) {
        /* Delete the slice_reserve_entry if any */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_stage_based_id_get(unit,
                                                   stage_info->stage_id,
                                                   entry_oper->group_id,
                                                   &group_id));
        rv = bcmi_field_group_active_entries_get(unit,
                                                 group_id,
                                                 &active_entries_count);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        /* slice_reserve_entry + first valid entry */
        if (rv != SHR_E_NOT_FOUND) {
            if ((active_entries_count == 2) &&
                    !(entry_oper->entry_flags &
                        BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO)) {
                SHR_IF_ERR_EXIT_EXCEPT_IF
                    (bcmi_field_group_delete_slice_reserve_entry(unit,
                                                                 group_id),
                     SHR_E_NOT_FOUND);
            }
        }
    }

    /* Set group install status */
    group_oper->group_flags |= BCMINT_FIELD_GROUP_INSTALLED;

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    if (BCMLT_INVALID_HDL != entry_template) {
        (void) bcmlt_entry_free(entry_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_prio_get(int unit,
                              bcm_field_entry_t entry, int *prio)
{
    bcm_field_entry_t entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit,
                                           entry,
                                           &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = (BCM_FIELD_ID_MASK) & entry;

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_presel_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &presel_oper));
        *prio = presel_oper->priority;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &entry_oper));
        *prio = entry_oper->priority;
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_prio_set(int unit,
                            bcm_field_entry_t entry, int prio)
{
    int dunit = 0;
    uint64_t ent_lt_prio_min = 0;
    uint64_t ent_lt_prio_max = 0;
    bcm_field_entry_t entry_id = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t entry_handle= BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_presel_entry_prio_set(unit, entry,
                                                prio));
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit,
                                           entry,
                                           &stage_info));

    if (stage_info->tbls_info->entry_priority_fid == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }


    /* check the range of entry priority */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmi_lt_field_value_range_get(unit,
             stage_info->tbls_info->entry_sid,
             stage_info->tbls_info->entry_priority_fid,
             &ent_lt_prio_min, &ent_lt_prio_max));

    if ((prio < ent_lt_prio_min) ||
        (prio > ent_lt_prio_max)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Mask the stage bits from given entry ID */
    entry_id = (BCM_FIELD_ID_MASK) & entry;

    /* Verify whether entry exists in the given stage */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* No change in prio value. return success */
    if (prio == entry_oper->priority) {
        SHR_EXIT();
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        /* Entry handle allocate for Entry Template */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit,
                                 stage_info->tbls_info->entry_sid,
                                 &entry_handle));

        /* Add entry_id field to Entry LT. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_handle,
                                  stage_info->tbls_info->entry_key_fid,
                                  entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_handle,
                              stage_info->tbls_info->entry_priority_fid,
                              prio));

        /* Insert entry template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, entry_handle,
                                  BCMLT_PRIORITY_NORMAL));
    }

    /* update the s/w entry strucutre with new priority */
    entry_oper->priority = prio;

exit:
    if (BCMLT_INVALID_HDL != entry_handle) {
        (void) bcmlt_entry_free(entry_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_enable_get(int unit,
                              bcm_field_entry_t entry,
                              int *enable_flag)
{
    bcm_field_entry_t entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    if (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *enable_flag =
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_enable_set(int unit,
                              bcm_field_entry_t entry,
                              int enable)
{
    int dunit = 0;
    int group_id = 0;
    int entry_flags = 0;
    bcm_field_entry_t entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;
    bool slice_reserve_entry_enable = FALSE;
    uint64_t active_entries_count = 0;
    bcm_field_entry_config_t entry_config;

    SHR_FUNC_ENTER(unit);

    if ((enable < 0) || (enable > 2)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* if entry is not installed, we cant enable/disable the entry */
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_HW_INVALIDATED) {
        if (enable == 2) {
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* check if entry is already diabled. if yes do nothing */
    if ((enable == 0) &&
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED)) {
         /*Do Nothing */
         SHR_EXIT();
    }

    /* check if entry is already enabled. if yes do nothing */
    if ((enable == 1) &&
        (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED))) {
         /*Do Nothing */
         SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for Entry Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->entry_sid,
                             &entry_template));

    /* Add entry_id field to Entry LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(entry_template,
                              stage_info->tbls_info->entry_key_fid,
                              entry_id));

    entry_flags = entry_oper->entry_flags;
    group_id = entry_oper->group_id;
    if (enable == 2) {
        /*
         * set the group ID to 0 in Lt entry update.
         * change the entry_oper install status to False.
         */
         /* Add group_template_id field to Entry LT. */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_add(entry_template,
                                    stage_info->tbls_info->group_key_fid,
                                    0));

         /* Remove the entry install status */
         entry_flags &= ~BCMINT_FIELD_ENTRY_INSTALLED;
         entry_flags |= BCMINT_FIELD_ENTRY_HW_INVALIDATED;

         /* Install slice_reserve_entry with entry remove of only active entry */
         if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO)  {
             SHR_IF_ERR_EXIT_EXCEPT_IF
                 (bcmi_field_group_active_entries_get(unit,
                                                      group_id,
                                                      &active_entries_count),
                  SHR_E_NOT_FOUND);
             if (active_entries_count == 1)  {
                 /* Not a slice reserve. */
                 if (!(entry_oper->entry_flags &
                             BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO) &&
                         (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED)) {
                     slice_reserve_entry_enable =TRUE;
                 }
             }
         }
    } else {
        if (0 == enable) {  /* entry disable */
            entry_flags |= BCMINT_FIELD_ENTRY_DISABLED;
        } else if (1 == enable) { /* entry enable */
            entry_flags &= ~BCMINT_FIELD_ENTRY_DISABLED;
        }

        /* Add group_template_id field to Entry LT. */
        SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(entry_template,
                                stage_info->tbls_info->entry_enable_fid,
                                enable));
    }

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmi_lt_entry_set_commit(unit, entry_template,
                                   BCMLT_PRIORITY_NORMAL));

    /* Update entry flags */
    entry_oper->entry_flags = entry_flags;

    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) &&
            (slice_reserve_entry_enable)) {
        entry = stage_info->entry_info->eid_max;
        for (; entry > 0; entry--) {
            if (SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry)) {
                continue;
            }
            break;
        }

        if (entry == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        entry_id = entry;
        entry = stage_info->stage_base_id | entry;

        bcm_field_entry_config_t_init(&entry_config);
        entry_config.group_id = group_id;
        entry_config.entry_id = entry;
        entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_ID;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_config_create(unit,
                                              &entry_config));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit,
                                            entry_id,
                                            stage_info,
                                            &entry_oper));
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_install(unit,
                                        entry));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_template) {
        (void) bcmlt_entry_free(entry_template);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function to validate the given flex counter index
 * and return the flexctr action profile index.
 */
STATIC int
ltsw_field_entry_flexctr_action_prof_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    uint32_t flexctr_action_id,
    uint32_t counter_idx,
    uint64_t *flex_ctr_action,
    char **opq_obj,
    uint32_t *opq_obj_val,
    char **flexctr_cont_obj,
    uint64_t *flexctr_cont_id,
    bcm_color_t *color)
{
    char *opq_obj_fid = NULL;
    bool stat_without_obj = false;
    bcm_flexctr_object_t object, object1;
    bcmi_ltsw_flexctr_counter_info_t ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opq_obj, SHR_E_INTERNAL);
    SHR_NULL_CHECK(flexctr_cont_obj, SHR_E_INTERNAL);
    SHR_NULL_CHECK(flexctr_cont_id, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               flexctr_action_id,
                                               &ctr_info));

    if (ctr_info.group == TRUE) {
        bcm_flexctr_group_action_t group_action;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_group_action_get(unit, flexctr_action_id,
                                                &group_action));
        if (group_action.action_num == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        sal_memset(&ctr_info, 0x0, sizeof(ctr_info));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_counter_id_info_get(
                      unit,
                      group_action.stat_counter_id[0],
                      &ctr_info));
    }

    if (ctr_info.act_cfg.hint_type == bcmFlexctrHintFieldGroupId) {
        bcm_field_group_t group_id, hgid;

        if (ctr_info.group == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Hint validation */
        hgid = ctr_info.act_cfg.hint;
        if (hgid == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Hint group should belongs to same stage */
        if ((hgid & stage_info->stage_base_id) != stage_info->stage_base_id) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        group_id = (group_oper->group_id | stage_info->stage_base_id);
        if (hgid != group_id) {
            bcmi_field_ha_group_oper_t *g_oper = NULL;

            /*
             * If the flexctr action belongs to different group, ensure the
             * hint group exists and has same priority.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_oper_lookup(unit,
                                                hgid,
                                                stage_info,
                                                &g_oper));
            if (g_oper->priority != group_oper->priority) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    } else if (ctr_info.act_cfg.hint_type == bcmFlexctrHintPool) {
        /*
         * Nothing to validate, the flexctr pool to field
         * group mapping is managed by the user.
         */
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate counter index */
    if (counter_idx > ctr_info.act_cfg.index_num) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *opq_obj_val = counter_idx;
    /* Validate Flex counter index operation object field */
    object = ctr_info.act_cfg.index_operation.object[0];
    object1 = ctr_info.act_cfg.index_operation.object[1];

    /* Validate stage id and respective counter index */
    if ((bcmFlexctrObjectIngIfpOpaqueObj0 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj1 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj2 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj3 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj4 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj5 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj6 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj7 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj8 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj9 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj1_0 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj1_1 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj1_2 == object) ||
        (bcmFlexctrObjectIngIfpOpaqueObj1_3 == object) ||
        (bcmFlexctrObjectStaticIngFieldStageIngress == object)) {
        if (bcmiFieldStageIngress != stage_info->stage_id) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        switch (object) {
            case bcmFlexctrObjectStaticIngFieldStageIngress:
            case bcmFlexctrObjectIngIfpOpaqueObj0:
                /* Fall through */
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx0_fid;
                if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXCTR_CONT_OBJ_SUPPORT) {
                    *flexctr_cont_obj = stage_info->tbls_info->flex_ctr_cont_fid;
                    *flexctr_cont_id = (uint64_t)ctr_info.act_cfg.index_operation.object_id[0];
                }
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx1_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj2:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx2_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj3:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx3_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj4:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx4_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj5:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx5_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj6:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx6_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj7:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx7_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj8:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx8_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj9:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx9_fid;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_0:
            case bcmFlexctrObjectIngIfpOpaqueObj1_1:
            case bcmFlexctrObjectIngIfpOpaqueObj1_2:
            case bcmFlexctrObjectIngIfpOpaqueObj1_3:
                /* Fall through */
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx1_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_flexctr_field_opaque_object_value_get(
                                 unit, flexctr_action_id,
                                 counter_idx, 16,
                                 opq_obj_val));
                break;
            default:
                opq_obj_fid = NULL;
                break;
        }

        if ((color != NULL) &&
            (object1 == bcmFlexctrObjectStaticIngPktAttribute)) {
            int e, sel, ct = 0;
            int profile_idx;
            bcmi_ltsw_flexctr_packet_attribute_info_t info;
            bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst_info;
            profile_idx = ctr_info.act_cfg.index_operation.object_id[1];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_packet_attribute_id_info_get(
                          unit, profile_idx, &info));
            for (e = 0; e < info.obj_inst_max; e++) {
                obj_inst_info = &info.obj_inst[e];
                for (sel = 0; sel < obj_inst_info->num_entries; sel++) {
                    if (obj_inst_info->map_entry_type[sel] ==
                        bcmFlexctrPacketAttributeTypeIngFieldIngressColor) {
                        if (ct >= 3) {
                            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                        }
                        color[ct++] = obj_inst_info->map_entry_value[sel];
                    }
                }
            }
        }
    } else if ((bcmFlexctrObjectEgrEfpOpaqueObj1 == object) ||
               (bcmFlexctrObjectEgrEfpOpaqueObj2 == object) ||
               (bcmFlexctrObjectStaticEgrFieldStageEgress == object)) {
        if (stage_info->stage_id != bcmiFieldStageEgress) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        switch (object) {
            case bcmFlexctrObjectStaticEgrFieldStageEgress:
            case bcmFlexctrObjectEgrEfpOpaqueObj1:
                /* Fall through */
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx0_fid;
                break;
            default:
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx1_fid;
                break;
        }
    } else if ((bcmFlexctrObjectIngVfpOpaqueObj0 == object) ||
               (bcmFlexctrObjectStaticIngFieldStageLookup == object)) {
        if (stage_info->stage_id != bcmiFieldStageVlan) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        switch (object) {
            case bcmFlexctrObjectIngVfpOpaqueObj0:
            case bcmFlexctrObjectStaticIngFieldStageLookup:
                /* Fall through */
                opq_obj_fid = stage_info->tbls_info->flex_ctr_idx0_fid;
                break;
            default:
                opq_obj_fid = NULL;
                break;
        }
    } else if (bcmFlexctrObjectStaticIngExactMatch == object) {
        if (stage_info->stage_id != bcmiFieldStageExactMatch) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        opq_obj_fid = stage_info->tbls_info->flex_ctr_idx0_fid;
        if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXCTR_CONT_OBJ_SUPPORT) {
            *flexctr_cont_obj = stage_info->tbls_info->flex_ctr_cont_fid;
            *flexctr_cont_id = (uint64_t)ctr_info.act_cfg.index_operation.object_id[0];
        }
    } else {
        stat_without_obj = true;
    }

    if (stat_without_obj == false) {
        if (opq_obj_fid == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        if (opq_obj) {
            *opq_obj = opq_obj_fid;
        }
    }

    /* Validate Flex counter table name */
    if (sal_strcmp(stage_info->tbls_info->entry_sid, ctr_info.table_name)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *flex_ctr_action = (uint64_t)ctr_info.action_index;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_flexctr_attach(int unit,
                                  bcm_field_entry_t entry,
                                  bcm_field_flexctr_config_t *flexctr_cfg)
{
    int pipe = -1;
    int iter = 0;
    int entry_id = 0;
    bool stat_group_found = false, stat_without_obj_found = false;
    bool stat_gp = false, stat_yp = false, stat_rp = false;
    bool policer_found = false;
    int dunit = 0;
    bcm_color_t color[3] = {bcmColorCount, bcmColorCount, bcmColorCount};
    uint64_t flex_ctr_action = 0;
    char *opaque_object = NULL;
    uint32_t opaque_obj_val = 0;
    char *flexctr_cont_obj = NULL;
    uint64_t flexctr_cont_id = 0;
    bcm_field_action_params_t params;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    const bcm_field_action_map_t *lt_map_ref = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));
    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    /* cannot attach flex counters to default entry*/
    if ((entry_oper->entry_flags) &
         BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    /* Check if stat already attached to the entry */
    if (entry_oper->flexctr_action_id) {
        if (flexctr_cfg->flexctr_action_id != entry_oper->flexctr_action_id) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if (flexctr_cfg->color == entry_oper->counter_color) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }

        if (flexctr_cfg->color == 0) {
            if (entry_oper->counter_color != 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
        } else {
            if (entry_oper->counter_color == 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
        }
    }

    /* Check whether StatGroup action is part of Group's ASET or not */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                   bcmFieldActionGpStatGroup,
                                   stat_gp);
        FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                   bcmFieldActionYpStatGroup,
                                   stat_yp);
        FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                   bcmFieldActionRpStatGroup,
                                   stat_rp);
        FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                   bcmFieldActionStatGroup,
                                   stat_group_found);
        FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                   bcmFieldActionStatGroupWithoutCounterIndex,
                                   stat_without_obj_found);
        if (stat_without_obj_found == FALSE &&
            stat_group_found == false &&
            stat_rp == false &&
            stat_yp == false &&
            stat_gp == false) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

    if (flexctr_cfg->color != 0) {
        if (!(stage_info->flags &
              BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
            FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                       bcmFieldActionPolicerGroup,
                                       policer_found);
            if (policer_found == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
        }
    }

    /* Validate flexctr action and retrive the action profile index */
    SHR_IF_ERR_VERBOSE_EXIT
       (ltsw_field_entry_flexctr_action_prof_get(
             unit, stage_info,
             group_oper,
             flexctr_cfg->flexctr_action_id,
             flexctr_cfg->counter_index,
             &flex_ctr_action,
             &opaque_object,
             &opaque_obj_val,
             &flexctr_cont_obj,
             &flexctr_cont_id,
             color));

    /*
     * If FLEX_CTR object is not OPAQUE_OBJ and the group is not
     * configured to use action "bcmFieldActionStatGroupWithoutCounterIndex"
     * return E_CONFIG.
     */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        if (opaque_object == NULL) {
            if ((stat_without_obj_found == FALSE) ||
                (stat_group_found == TRUE) ||
                (stat_gp == TRUE) ||
                (stat_rp == TRUE) ||
                (stat_yp == TRUE)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
        } else {
            if (((stat_group_found == FALSE) &&
                 (stat_gp == FALSE) &&
                 (stat_yp == FALSE) &&
                 (stat_rp == FALSE)) ||
                (stat_without_obj_found == TRUE)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
        }
        if (((flexctr_cfg->color & BCM_FIELD_COUNT_GREEN) &&
             (stat_gp == false)) ||
            ((flexctr_cfg->color & BCM_FIELD_COUNT_YELLOW) &&
             (stat_yp == false)) ||
            ((flexctr_cfg->color & BCM_FIELD_COUNT_RED) &&
             (stat_rp == false)) ||
            ((flexctr_cfg->color == 0) && (stat_group_found == false) &&
             (stat_without_obj_found == false))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    } else {
        if (opaque_object == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

    /* Update the stat if the entry is already installed */
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

        /* set flex counter action */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_field_lt_entry_commit(unit, &entry_template,
                                         BCMLT_OPCODE_UPDATE,
                                         BCMLT_PRIORITY_NORMAL,
                                         stage_info->tbls_info->entry_sid,
                                         stage_info->tbls_info->entry_key_fid,
                                         0, entry_id, NULL,
                                         stage_info->tbls_info->ctr_key_fid,
                                         flex_ctr_action,
                                         0, 0,
                                         1));

        if (NULL != flexctr_cont_obj) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(
                        unit, &policy_template,
                        BCMLT_OPCODE_UPDATE,
                        BCMLT_PRIORITY_NORMAL,
                        stage_info->tbls_info->policy_sid,
                        stage_info->tbls_info->policy_key_fid,
                        0, entry_id, NULL,
                        flexctr_cont_obj,
                        flexctr_cont_id,
                        0, 0,
                        1));
        }

        if (opaque_object != NULL) {
            if (flexctr_cfg->color == 0) {
                /* set counter index in the opaque object */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_lt_entry_commit(
                            unit, &policy_template,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL,
                            stage_info->tbls_info->policy_sid,
                            stage_info->tbls_info->policy_key_fid,
                            0, entry_id, NULL,
                            opaque_object,
                            opaque_obj_val,
                            0, 0,
                            1));

                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_entry_strength_action_update(
                            unit,
                            stage_info, group_oper,
                            entry_oper, bcmFieldActionStatGroup,
                            NULL, true));
                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_action_entry_update(
                            unit,
                            entry_oper,
                            bcmFieldActionStatGroup,
                            true));
            } else {
                uint8_t iter;
                bcm_field_action_t action;

                params.param0 = flexctr_cfg->counter_index;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                        entry_oper->group_id,
                                                        &pipe));
                for (iter = 0; iter < 3; iter++) {
                    action = bcmFieldActionCount;
                    if ((iter == 0) &&
                        (flexctr_cfg->color & BCM_FIELD_COUNT_GREEN)) {
                        action = bcmFieldActionGpStatGroup;
                    } else if ((iter == 1) &&
                               (flexctr_cfg->color & BCM_FIELD_COUNT_YELLOW)) {
                        action = bcmFieldActionYpStatGroup;
                    } else if ((iter == 2) &&
                               (flexctr_cfg->color & BCM_FIELD_COUNT_RED)) {
                        action = bcmFieldActionRpStatGroup;
                    }

                    if (action == bcmFieldActionCount) {
                        continue;
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                action,
                                &params));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_set(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                action));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_strength_action_update(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                action,
                                NULL, true));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(
                                unit,
                                entry_oper,
                                action,
                                true));
                }
            }
        }

        /* Increment the flexctr reference counter */
        if ((entry_oper->flexctr_action_id != flexctr_cfg->flexctr_action_id) &&
            !(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_STAT_ATTACHED)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_attach_counter_id_status_update(
                        unit,
                        flexctr_cfg->flexctr_action_id));
            entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_STAT_ATTACHED;
        }
    }

    entry_oper->flexctr_action_id = flexctr_cfg->flexctr_action_id;
    entry_oper->counter_color |= flexctr_cfg->color;
    if (flexctr_cfg->color == 0) {
        entry_oper->counter_idx = flexctr_cfg->counter_index;
    } else {
        if (flexctr_cfg->color & BCM_FIELD_COUNT_GREEN) {
            entry_oper->g_counter_idx = flexctr_cfg->counter_index;
        }
        if (flexctr_cfg->color & BCM_FIELD_COUNT_YELLOW) {
            entry_oper->y_counter_idx = flexctr_cfg->counter_index;
        }
        if (flexctr_cfg->color & BCM_FIELD_COUNT_RED) {
            entry_oper->r_counter_idx = flexctr_cfg->counter_index;
        }
    }

    /* Find lt_map for provided qualifier from db. */
    if (opaque_object != NULL) {
        bool map_set = false;
        uint32_t c, color_val[3] = {0};
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_map_find(unit,
                                          bcmFieldActionStatGroup,
                                          stage_info,
                                          &lt_map_ref));

        /* Set internal maps associated if any. */
        if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
            map_set = false;
            sal_memset(&color_val, 0, sizeof(color_val));
            for (c = 0; c < 3; c++) {
                if (color[c] != bcmColorCount) {
                    if (color[c] == bcmColorRed) {
                        color_val[0] = 1;
                    } else if (color[c] == bcmColorYellow) {
                        color_val[1] = 1;
                    } else if (color[c] == bcmColorGreen) {
                        color_val[2] = 1;
                    }
                    map_set = true;
                }
            }

            dunit = bcmi_ltsw_dev_dunit(unit);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit,
                                      stage_info->tbls_info->policy_sid,
                                      &action_template_hdl));

            /* Add policy_template_id field to Policy LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(action_template_hdl,
                                       stage_info->tbls_info->policy_key_fid,
                                       entry & BCM_FIELD_ID_MASK));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, action_template_hdl,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_internal_map_set(unit,
                                                      1,
                                                      bcmFieldActionStatGroup,
                                                      action_template_hdl,
                                                      action_template_hdl,
                                                      stage_info,
                                                      &(lt_map_ref->map[iter]),
                                                      0, 0,
                                                      map_set, color_val));
            if (stage_info->tbls_info->action_ctr_enable) {
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmlt_entry_field_add(action_template_hdl,
                                       stage_info->tbls_info->action_ctr_enable,
                                       1));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, action_template_hdl,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
        }
    }

exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
       (void) bcmlt_entry_free(action_template_hdl);
       action_template_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_flexctr_detach(int unit,
                                  bcm_field_entry_t entry,
                                  bcm_field_flexctr_config_t *flexctr_cfg)
{
    int iter = 0;
    int dunit = 0;
    int entry_id = 0;
    int pipe = -1;
    char *opaque_object = NULL;
    uint32_t opq_obj_val = 0;
    char *flexctr_cont_obj = NULL;
    uint64_t flex_ctr_action = 0;
    uint64_t flexctr_cont_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    const bcm_field_action_map_t *lt_map_ref = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Check if stat already attached to the entry */
    if (entry_oper->flexctr_action_id != flexctr_cfg->flexctr_action_id) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (flexctr_cfg->color == 0) {
        if (entry_oper->counter_color != 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        if (!(stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        if (entry_oper->counter_color == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    /* Validate flexctr action and retrive the action profile index */
    SHR_IF_ERR_VERBOSE_EXIT
       (ltsw_field_entry_flexctr_action_prof_get(
             unit, stage_info,
             group_oper,
             flexctr_cfg->flexctr_action_id,
             flexctr_cfg->counter_index,
             &flex_ctr_action, &opaque_object,
             &opq_obj_val,
             &flexctr_cont_obj, &flexctr_cont_id,
             NULL));

    /* Update the stat if the entry is already installed */
    if ((entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) ||
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_HW_INVALIDATED)) {
        bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
        if (flexctr_cfg->color == entry_oper->counter_color) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &entry_template,
                                              BCMLT_OPCODE_UPDATE,
                                              BCMLT_PRIORITY_NORMAL,
                                              stage_info->tbls_info->entry_sid,
                                              stage_info->tbls_info->entry_key_fid,
                                              0, entry_id, NULL,
                                              stage_info->tbls_info->ctr_key_fid,
                                              BCMI_LTSW_FLEXCTR_ACTION_INVALID,
                                              0, 0,
                                              1));
        }

        if (NULL != flexctr_cont_obj) {
            /* Reset counter container iD */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &policy_template,
                                              BCMLT_OPCODE_UPDATE,
                                              BCMLT_PRIORITY_NORMAL,
                                              stage_info->tbls_info->policy_sid,
                                              stage_info->tbls_info->policy_key_fid,
                                              0, entry_id, NULL,
                                              flexctr_cont_obj,
                                              0,
                                              0, 0,
                                              1));
        }

        if (opaque_object != NULL) {
            if (flexctr_cfg->color == 0) {
                /* Reset counter index in the opaque object */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_lt_entry_commit(unit, &policy_template,
                                                  BCMLT_OPCODE_UPDATE,
                                                  BCMLT_PRIORITY_NORMAL,
                                                  stage_info->tbls_info->policy_sid,
                                                  stage_info->tbls_info->policy_key_fid,
                                                  0, entry_id, NULL,
                                                  opaque_object,
                                                  0,
                                                  0, 0,
                                                  1));
                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_action_entry_update(unit,
                                                       entry_oper,
                                                       bcmFieldActionStatGroup,
                                                       false));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                        entry_oper->group_id,
                                                        &pipe));
                if (flexctr_cfg->color & BCM_FIELD_COUNT_GREEN) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionGpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionGpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(
                                unit,
                                entry_oper,
                                bcmFieldActionGpStatGroup,
                                false));
                }
                if (flexctr_cfg->color & BCM_FIELD_COUNT_YELLOW) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionYpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionYpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(
                                unit,
                                entry_oper,
                                bcmFieldActionYpStatGroup,
                                false));
                }
                if (flexctr_cfg->color & BCM_FIELD_COUNT_RED) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionRpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_remove(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                pipe,
                                bcmFieldActionRpStatGroup));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_entry_update(
                                unit,
                                entry_oper,
                                bcmFieldActionRpStatGroup,
                                false));
                }
            }
        }

        /* Decrement the flexctr reference counter */
        if ((flexctr_cfg->color == entry_oper->counter_color) &&
            (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_STAT_ATTACHED)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_detach_counter_id_status_update(
                        unit,
                        flexctr_cfg->flexctr_action_id));
            entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_STAT_ATTACHED;
        }
    }

    if (flexctr_cfg->color == 0) {
        entry_oper->counter_idx = 0;
        entry_oper->counter_color = 0;
    } else {
        if (flexctr_cfg->color & BCM_FIELD_COUNT_GREEN) {
            entry_oper->g_counter_idx = 0;
        }
        if (flexctr_cfg->color & BCM_FIELD_COUNT_YELLOW) {
            entry_oper->y_counter_idx = 0;
        }
        if (flexctr_cfg->color & BCM_FIELD_COUNT_RED) {
            entry_oper->r_counter_idx = 0;
        }
        entry_oper->counter_color &= ~(flexctr_cfg->color);
    }

    if (entry_oper->counter_color == 0) {
        if (opaque_object != NULL) {
            /* Find lt_map for provided qualifier from db. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_map_find(unit, bcmFieldActionStatGroup,
                                              stage_info, &lt_map_ref));

            /* Set internal maps associated if any. */
            if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                dunit = bcmi_ltsw_dev_dunit(unit);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit,
                                          stage_info->tbls_info->policy_sid,
                                          &action_template_hdl));
                /* Add policy_template_id field to Policy LT. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(action_template_hdl,
                                           stage_info->tbls_info->policy_key_fid,
                                           entry & BCM_FIELD_ID_MASK));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, action_template_hdl,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL));


                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_action_internal_map_set(
                            unit,
                            0,
                            bcmFieldActionStatGroup,
                            action_template_hdl,
                            action_template_hdl,
                            stage_info, &(lt_map_ref->map[iter]),
                            0, 0, 0, NULL));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, action_template_hdl,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL));
            }
        }

        entry_oper->flexctr_action_id = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    }

exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
       (void) bcmlt_entry_free(action_template_hdl);
       action_template_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_policer_attach(int unit,
                                  bcm_field_entry_t entry,
                                  int level,
                                  bcm_policer_t policer_id)
{
    int entry_id = 0, group_id = 0;
    int pipe = 0, pool;
    bool found = false;
    bcmi_ltsw_policer_stage_t policer_stage;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (level != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        policer_stage = bcmiPolicerStageEgress;
    } else {
        policer_stage = bcmiPolicerStageIngress;
    }

    if (((stage_info->stage_id == bcmiFieldStageIngress) ||
         (stage_info->stage_id == bcmiFieldStageExactMatch)) &&
        (policer_stage != bcmiPolicerStageIngress)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (stage_info->stage_id == bcmiFieldStageEgress &&
        policer_stage != bcmiPolicerStageEgress) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));
    /* cannot attach policers to default entry*/
    if ((entry_oper->entry_flags) &
         BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    /* Check if policer already attached to the entry */
    if (entry_oper->policer_id) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
    }

    group_id = entry_oper->group_id;

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        /* Check whether PolicerGroup action is part of Group's ASET or not */
        FP_HA_GROUP_OPER_ASET_TEST(group_oper, bcmFieldActionPolicerGroup, found);
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit,
                                     stage_info,
                                     group_oper,
                                     &pipe));

    if (group_oper->policer_pool_id == -1) {
        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        /*
         * This is the first entry to attach with policers.
         * And Policer Pool is not allocated during group create.
         * So allocate one pool now.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_policer_pool_alloc(unit,
                                                   stage_info,
                                                   pipe,
                                                   group_oper->priority,
                                                   &group_oper->policer_pool_id));
    }

    pool = group_oper->policer_pool_id;

    /* Notify policer about FP entry attach. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_policer_attach
         (unit, policer_id, pipe, pool));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        /* Attach Policer to FP entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_entry_policer_attach(unit,
                                                  stage_info,
                                                  entry_oper,
                                                  policer_id));
    }

    entry_oper->policer_id = policer_id;

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcmint_field_policer_found_entries_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper_orig,
    int entry_size,
    bcm_field_entry_t *entry_array,
    int *entry_count)
{
    int ct = 0;
    int eid = 0;
    int idx = 0;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Null check */
    if ((entry_size) && (entry_array == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if (entry_oper->policer_id) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_group_oper_lookup(unit,
                                                    entry_oper->group_id,
                                                    stage_info,
                                                    &group_oper));
                if (group_oper->policer_pool_id ==
                        group_oper_orig->policer_pool_id) {
                    BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                                    entry_oper->entry_id,
                                                    eid);
                    if ((entry_size) && (ct < entry_size)) {
                        entry_array[ct] = eid;
                    }
                    ct++;
                }
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

    *entry_count = ct;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_policer_detach(int unit,
                                  bcm_field_entry_t entry,
                                  int level)
{
    int pipe = 0;
    int entry_id = 0;
    int entry_count = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    if (level != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    entry_id = entry & BCM_FIELD_ID_MASK;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Check if policer already attached to the entry */
    if (entry_oper->policer_id == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Notify policer about FP entry detach. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_policer_detach(unit, entry_oper->policer_id));

    /* Delete policer from FP entry. */
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_entry_policer_detach(unit,
                                                  entry));
    }

    entry_oper->policer_id = 0;

    if (0 == (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD)) {
        /* Retreive group hash entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit,
                                    (BCM_FIELD_ID_MASK) & entry_oper->group_id,
                                    stage_info,
                                    &group_oper));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_policer_found_entries_get(unit, stage_info, group_oper,
                                                    0, NULL, &entry_count));

        if (0 == entry_count) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_pipe_get(unit,
                                             stage_info,
                                             group_oper,
                                             &pipe));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_policer_pool_free(unit, stage_info, pipe,
                                                      group_oper->policer_pool_id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_policer_detach_all(int unit,
                                      bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_detach(unit,
                                           entry,
                                           0));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_policer_get(int unit,
                                 bcm_field_entry_t entry,
                                 int level,
                                 bcm_policer_t *policer_id)
{
    int entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    if (level != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Check if policer is attached to the entry */
    if (entry_oper->policer_id == 0) {
        *policer_id = 0;
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *policer_id = entry_oper->policer_id;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_policer_update_all(int unit,
                                      bcm_policer_t policer_id,
                                      uint8_t in_use)
{
    int idx;
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;

    SHR_FUNC_ENTER(unit);

    if ((policer_id & ~(BCM_FIELD_ID_MASK)) == BCM_FIELD_EFP_ID_BASE) {
        stage_id = bcmiFieldStageEgress;
    } else {
        stage_id = bcmiFieldStageIngress;
        if (in_use == BCMI_LTSW_POLICER_POOL_USED_BY_EM) {
        stage_id = bcmiFieldStageExactMatch;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit,
                                     stage_id,
                                     &stage_info));

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        entry_oper = NULL;
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if (entry_oper->policer_id == policer_id) {
                if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mbcm_ltsw_field_entry_policer_attach(unit,
                                                              stage_info,
                                                              entry_oper,
                                                              policer_id));
                }
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmint_field_entry_multi_get
 *
 * Purpose:
 *     Gets an array of a group's entry IDs
 *
 * Parameters:
 *     unit -  (IN) BCM device number.
 *     group - (IN) Field group ID.
 *     entry_size - (IN) Maximum number of entries to return.  Set to 0
 *                       to get the number of entries available
 *     entry_array - (OUT) Pointer to a buffer to fill with the array of
 *                         entry IDs.  Ignored if entry_size is 0
 *     entry_count - (OUT) Returns the number of entries returned in the
 *                         array, or if entry_size was 0, the number of
 *                         entries available

 * Returns:
 *     BCM_E_INIT      - unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found in unit
 *     BCM_E_NONE      - Success
 */

int
bcmint_field_entry_multi_get(
    int unit,
    bcm_field_group_t group,
    int entry_size,
    bcm_field_entry_t *entry_array,
    int *entry_count)
{
    int ct = 0;
    int eid = 0;
    int idx = 0;
    bcm_field_group_t group_id = 0;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Null check */
    if ((entry_size) && (entry_array == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        group_id,
                                        stage_info,
                                        NULL));

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if (entry_oper->group_id == group_id) {
                BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                                entry_oper->entry_id,
                                                eid);
                if ((entry_size) && (ct < entry_size)) {
                    entry_array[ct] = eid;
                }
                ct++;
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

    *entry_count = ct;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmint_field_entry_copy
 * Purpose:
 *   Create an entry in the group of the src entry and
 *   copy the contents from src_entry to dst_entry.
 *   If the destination entry id is not euqal to zero,
 *   then an entry is created with given destination entry ID else
 *   the an entry is created with next available entry Id.
 * Parameters:
 *   unit      - BCM device number
 *   src_entry - Source entry IdD
 *   dst_entry - (IN/OUT) Destination Entry Id
 * Returns:
 *   BCM_E_SUCCESS    : If entry opy succeed
 *   BCM_E_XXX        : Error if entry copy failed
 */
int
bcmint_field_entry_copy(int unit,
                        bcm_field_entry_t src_entry,
                        bcm_field_entry_t *dst_entry,
                        uint32 flags)
{
    int ct;
    int src_ent_id;
    int dst_ent_id;
    bool is_entry_created = FALSE;
    bcm_field_entry_config_t entry_config;
    bcm_field_qset_t qset;
    char     *lt_field_name = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmlt_entry_handle_t src_rule_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t dst_rule_templ = BCMLT_INVALID_HDL;
    bcm_field_qual_map_t *lt_qual_map = NULL;

    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL;

    int iter = 0;
    char *qual_field = NULL;
    char *qual_mask_field = NULL;
    uint64_t *qual_mask_op_buf = NULL;
    uint64_t *qual_value_op_buf = NULL;
    uint32_t num_element = 0;
    char *lt_sym_str_op = NULL;
    /* action related */
    bcm_field_action_t action = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcm_field_action_params_t params;
    bcm_field_action_match_config_t match_config;
    uint32 mirror_gport[4] = {0};
    int num_gports = 0;
    int indx =0;
    int map_count= 0;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, src_entry,
                                           &stage_info));

    /* Mask the stage bits from given entry ID */
    src_ent_id = src_entry & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, src_ent_id,
                                        stage_info,
                                        &entry_oper));
    bcm_field_entry_config_t_init(&entry_config);
    entry_config.group_id =
        (entry_oper->group_id | stage_info->stage_base_id) ;

    if ( *dst_entry != 0) {
        entry_config.flags |=  BCM_FIELD_ENTRY_CREATE_WITH_ID;
        entry_config.entry_id = *dst_entry;
    }

    if (entry_oper->priority != 0) {
        entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_PRIORITY;
        entry_config.priority = entry_oper->priority;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_config_create(unit, &entry_config));
    is_entry_created = TRUE;
    *dst_entry = entry_config.entry_id;
    dst_ent_id = (*dst_entry) & BCM_FIELD_ID_MASK;

    /*set the priority of the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_prio_set(unit, *dst_entry,
                                    entry_oper->priority));
    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    /* Allocate and Read rule template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &src_rule_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->rule_sid,
                                      stage_info->tbls_info->rule_key_fid,
                                      0, src_ent_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    /* Allocate and Read rule template of dst entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &dst_rule_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->rule_sid,
                                      stage_info->tbls_info->rule_key_fid,
                                      0, dst_ent_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    BCM_FIELD_QSET_INIT(qset);
    for (ct = 0; ct < group_oper->qual_cnt; ct++) {
        SHR_BITSET(qset.w, (bcmi_field_qualify_t)group_oper->qset_arr[ct]);
    }

    for (ct = 0; ct < group_oper->qual_cnt; ct++) {
        if (BCMINT_FIELD_IS_PSEUDO_QUAL(group_oper->qset_arr[ct])) {
            continue;
        }

        /* Find lt map from db for given qualifier in group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, group_oper->qset_arr[ct],
                                        stage_info, 0, 0, &map_count,
                                        &lt_qual_map));

        BCMINT_FIELD_MEM_ALLOC
            (fld_defs_ref,
            sizeof(bcmlt_field_def_t) * (lt_qual_map->num_maps),
            "field definitions");
        fld_defs_iter = fld_defs_ref;
        for (iter = 0; iter < lt_qual_map->num_maps; iter++) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                (lt_qual_map->map[iter]))) {
                fld_defs_iter++;
                continue;
            }

            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, &qset, &(lt_qual_map->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    stage_info->tbls_info->rule_sid,
                    lt_field_name, fld_defs_iter));

            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_op_buf,
                sizeof(uint64_t) * (fld_defs_iter->elements),
                "qualifier value");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_op_buf,
                sizeof(uint64_t) * (fld_defs_iter->elements),
                "qualifier mask value");
            BCMINT_FIELD_MEM_ALLOC
                (qual_field,
                (strlen(lt_field_name) + 1),
                "qualifier field");
            BCMINT_FIELD_MEM_ALLOC
               (qual_mask_field,
               (strlen(lt_field_name) + 6),
               "qualifier mask field");

            sal_strcpy(qual_field, lt_field_name);
            sal_strcpy(qual_mask_field, qual_field);
            sal_strcat(qual_mask_field,"_MASK");
            num_element = fld_defs_iter->elements;

            if (fld_defs_iter->elements > 1) {
                if (fld_defs_iter->symbol) {
                    /* Not handled till now as no such case is present
                     * in db till date */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
               } else {
                /* Retreive from src qualifier & mask value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(src_rule_templ,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element,
                                                 &num_element));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(src_rule_templ,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element,
                                                 &num_element));
                    /* Add to dst qualifier & mask value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(dst_rule_templ,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(dst_rule_templ,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element));
                }
            } else {
                if (fld_defs_iter->symbol) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_get(src_rule_templ,
                                                  qual_field,
                                      (const char **)&lt_sym_str_op));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_add(src_rule_templ,
                                                  qual_field,
                                                  lt_sym_str_op));
                } else {
                    /* Retreive from src qualifier & mask value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(src_rule_templ,
                                           qual_field,
                                           qual_value_op_buf));
                    SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(src_rule_templ,
                                           qual_mask_field,
                                           qual_mask_op_buf));

                    /* Add to dst qualifier & mask value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(dst_rule_templ,
                                           qual_field,
                                           *qual_value_op_buf));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(dst_rule_templ,
                                           qual_mask_field,
                                           *qual_mask_op_buf));
                }
             }
            BCMINT_FIELD_MEM_FREE(qual_field);
            BCMINT_FIELD_MEM_FREE(qual_mask_field);
            BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
            BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
            fld_defs_iter++;
        } /* end of for */
        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
        /* Update rule template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(dst_rule_templ, BCMLT_OPCODE_UPDATE,
                    BCMLT_PRIORITY_NORMAL));
     }

    /* copy the actions from source to destination entry */

    /* loop through all actions and see if action is configured in entry*/
    for (ct = 0; ct < group_oper->action_cnt; ct++) {
        sal_memset(&params, 0, sizeof(bcm_field_action_params_t));
        action = group_oper->aset_arr[ct];
        if ((bcmFieldActionStatGroup == action) ||
            (bcmFieldActionPolicerGroup == action)) {
            /* will be handled later */
            continue;
        }
        if ((bcmFieldActionMirrorIngress == action) ||
            (bcmFieldActionMirrorEgress == action)) {
            num_gports = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_mirror_gport_get(unit, src_entry,
                                            stage_info,
                                            action, mirror_gport,
                                            &num_gports));
            for (indx=0; indx < num_gports; indx++) {
                params.param1 =  mirror_gport[indx];
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmint_field_action_mirror_set(unit,
                                *dst_entry, stage_info, action, &params));
            }
        }
        if (bcmint_field_action_config_info_get(unit,
                                            src_entry,
                                            action, NULL,
                                            &params,
                                            &match_config) != SHR_E_NONE) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_config_info_add(unit,
                                                 *dst_entry,
                                                 action,
                                                 &params,
                                                 &match_config));
    }
    /* check if source entry has flex counters attached */
    if (!(flags & BCM_FIELD_ENTRY_COPY_WITHOUT_STATS)) {
        if (entry_oper->flexctr_action_id) {
            bcm_field_flexctr_config_t flexctr_cfg;

            flexctr_cfg.flexctr_action_id = entry_oper->flexctr_action_id;
            if (entry_oper->counter_color == 0) {
                flexctr_cfg.counter_index = entry_oper->counter_idx;
                flexctr_cfg.color = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_entry_flexctr_attach(unit, *dst_entry,
                                                       &flexctr_cfg));
            } else {
                if (entry_oper->counter_color & BCM_FIELD_COUNT_GREEN) {
                    flexctr_cfg.counter_index = entry_oper->g_counter_idx;
                    flexctr_cfg.color = BCM_FIELD_COUNT_GREEN;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_flexctr_attach(unit, *dst_entry,
                                                           &flexctr_cfg));
                }
                if (entry_oper->counter_color & BCM_FIELD_COUNT_YELLOW) {
                    flexctr_cfg.counter_index = entry_oper->y_counter_idx;
                    flexctr_cfg.color = BCM_FIELD_COUNT_YELLOW;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_flexctr_attach(unit, *dst_entry,
                                                           &flexctr_cfg));
                }
                if (entry_oper->counter_color & BCM_FIELD_COUNT_RED) {
                    flexctr_cfg.counter_index = entry_oper->r_counter_idx;
                    flexctr_cfg.color = BCM_FIELD_COUNT_RED;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_flexctr_attach(unit, *dst_entry,
                                                           &flexctr_cfg));
                }
            }
        }
    }
    /* attach policers if any */
    if (entry_oper->policer_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_policer_attach(unit, *dst_entry, 0,
                                               entry_oper->policer_id));
    }

exit:
    if (BCMLT_INVALID_HDL != src_rule_templ) {
        (void) bcmlt_entry_free(src_rule_templ);
    }

    if (BCMLT_INVALID_HDL != dst_rule_templ) {
        (void) bcmlt_entry_free(dst_rule_templ);
    }

    if ( (SHR_FUNC_ERR()) && (is_entry_created)) {
        (void)bcmint_field_entry_destroy(unit,
                                         *dst_entry, NULL);
    }

    BCMINT_FIELD_MEM_FREE(qual_field);
    BCMINT_FIELD_MEM_FREE(qual_mask_field);
    BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    SHR_FUNC_EXIT();
}

/*
 * Remove All Entry Operational database
 */
int
bcmint_field_entry_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    uint32_t idx;
    uint8_t blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t blk_offset = 0;
    uint8_t next_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t next_blk_offset = 0;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_entry_oper_t *oper = NULL;
    bcmi_field_ha_entry_oper_t *oper_next = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);

    FP_STAGE_ENTRY_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        oper = NULL;
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                oper);
        if (oper == NULL) {
            continue;
        }

        next_blk_id = entry_base->blk_id;
        next_blk_offset = entry_base->blk_offset;
        /* Search the list of entries */
        do {
            blk_id = next_blk_id;
            blk_offset = next_blk_offset;

            FP_HA_ENT_OPER_NEXT(unit, oper, oper_next);
            if (oper_next != NULL) {
                next_blk_id = oper->next.blk_id;
                next_blk_offset = oper->next.blk_offset;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_ha_blk_element_release(unit, blk_id,
                                                     blk_offset,
                                                     blk_type));
        } while (oper_next != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_entry_strength_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    int curr_grp_prio)
{
    int idx;
    int pipe = 0;
    uint64_t s_idx = 0;
    int color_s_idx = -1;
    bcm_field_aset_t aset;
    bcmi_field_ha_entry_oper_t *oper = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmlt_entry_handle_t entry_templ = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmint_field_group_pipe_get(unit, stage_info,
                                      group_oper,
                                      &pipe));
    pipe = (pipe == -1) ? 0 : pipe;

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        oper = NULL;
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                oper);

        while (oper != NULL) {
            if (oper->group_id == group_oper->group_id) {
                /* Destroy old profile first associated to group. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_group_strength_prof_destroy(
                           unit,
                           stage_info,
                           pipe,
                           group_oper->priority,
                           oper->strength_idx,
                           oper->color_strength_idx));

                oper->strength_idx = 0;
                oper->color_strength_idx = -1;
            }
            FP_HA_ENT_OPER_NEXT(unit, oper, oper);
        }
    }

    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        oper = NULL;
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                oper);

        while (oper != NULL) {
            if (oper->group_id != group_oper->group_id) {
                FP_HA_ENT_OPER_NEXT(unit, oper, oper);
                continue;
            }

            BCM_FIELD_ASET_INIT(aset);
            BCMINT_FIELD_ACTION_ARR_TO_ASET(oper->num_actions,
                                            oper->action_arr,
                                            aset);


            s_idx = 0;
            color_s_idx = -1;
            /* Create SBR strength profile */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_create(
                        unit, stage_info, pipe,
                        group_oper->priority,
                        group_oper->hintid,
                        group_oper->policer_pool_id,
                        &aset,
                        &s_idx, &color_s_idx));

            oper->strength_idx = s_idx;
            oper->color_strength_idx = color_s_idx;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &entry_templ,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->entry_sid,
                                          stage_info->tbls_info->entry_key_fid,
                                          0, oper->entry_id, NULL,
                                          stage_info->tbls_info->sbr_key_fid,
                                          s_idx,
                                          0, 0,
                                          1));

            /* update color strength */

            FP_HA_ENT_OPER_NEXT(unit, oper, oper);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_strength_aset_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_field_aset_t *new_aset,
    bool install)
{
    int pipe = -1;
    uint64_t s_idx = 0;
    int color_s_idx = -1;
    uint32_t del_s_idx = 0;
    int del_color_s_idx = -1;
    bcm_field_aset_t aset;
    SHR_FUNC_ENTER(unit);

    BCM_FIELD_ASET_INIT(aset);
    BCMINT_FIELD_ACTION_ARR_TO_ASET(entry_oper->num_actions,
                                    entry_oper->action_arr,
                                    aset);

    if (new_aset) {
        BCMI_FIELD_ASET_OR(aset, (*new_aset), bcmFieldActionCount);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info,
                                     group_oper,
                                     &pipe));
    pipe = (pipe == -1) ? 0 : pipe;

    if (entry_oper->num_actions || (new_aset)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_strength_prof_create(unit, stage_info,
                                                     pipe,
                                                     group_oper->priority,
                                                     group_oper->hintid,
                                                     group_oper->policer_pool_id,
                                                     &aset,
                                                     &s_idx,
                                                     &color_s_idx));
    }

    if ((entry_oper->strength_idx != 0) &&
        (entry_oper->strength_idx != stage_info->sbr_max_limit)) {
        if (entry_oper->strength_idx == s_idx) {
            del_s_idx = 0;
        } else {
            del_s_idx = entry_oper->strength_idx;
        }
    }

    if (entry_oper->color_strength_idx != -1) {
        if (entry_oper->color_strength_idx == color_s_idx) {
            del_color_s_idx = -1;
        } else {
            del_color_s_idx = entry_oper->color_strength_idx;
        }
    }

    /* Delete the old entry */
    if ((del_s_idx != 0) ||
        (del_color_s_idx != -1)) {
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_field_group_strength_prof_destroy(unit,
                         stage_info, pipe,
                         group_oper->priority,
                         del_s_idx,
                         del_color_s_idx));
    }

    entry_oper->strength_idx = s_idx;
    entry_oper->color_strength_idx = color_s_idx;

    if (install) {
        bcmlt_entry_handle_t entry_templ = BCMLT_INVALID_HDL;
        bcm_field_action_t action;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &entry_templ,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->entry_sid,
                                          stage_info->tbls_info->entry_key_fid,
                                          0, entry_oper->entry_id, NULL,
                                          stage_info->tbls_info->sbr_key_fid,
                                          s_idx,
                                          0, 0,
                                          1));

        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) {
            SHR_BIT_ITER(aset.w, bcmFieldActionCount, action) {
                if (BCMINT_FIELD_IS_COLORED_ACTION(action)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                            entry_oper->group_id,
                                                            &pipe));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_color_pdd_sbr_set(unit,
                                                               stage_info,
                                                               group_oper,
                                                               entry_oper,
                                                               pipe,
                                                               action));
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_field_group_is_empty(int unit,
                         bcm_field_group_t group,
                         bool *entry_present)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    int gid = 0;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);

    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if ((entry_oper->group_id == gid) &&
               (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED)) {
                *entry_present = TRUE;
                break;
            }
                FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);

        if (*entry_present) {
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_field_group_delete_slice_reserve_entry(int unit,
                                            bcm_field_group_t group)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    int gid = 0;
    int idx = 0;
    int entry_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);

    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }
        /* Search the list of entries */
        do {
            if ((entry_oper->group_id == gid))  {
                if (entry_oper->entry_flags &
                        BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_stage_based_id_get(unit,
                                                           stage_info->stage_id,
                                                           entry_oper->entry_id,
                                                           &entry_id));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_destroy(unit,
                                                    entry_id,
                                                    entry_oper));
                    SHR_EXIT();
                }
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_entry_stage_based_id_get(int unit,
                                      bcmi_ltsw_field_stage_t stage_id,
                                      bcm_field_entry_t entry_id,
                                      bcm_field_entry_t *stage_base_entry_id)
{
    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case bcmiFieldStageIngress:
            *stage_base_entry_id = entry_id | BCM_FIELD_IFP_ID_BASE;
            break;
        case bcmiFieldStageEgress:
            *stage_base_entry_id = entry_id | BCM_FIELD_EFP_ID_BASE;
            break;
        case bcmiFieldStageVlan:
            *stage_base_entry_id = entry_id | BCM_FIELD_VFP_ID_BASE;
            break;
        case bcmiFieldStageExactMatch:
            *stage_base_entry_id = entry_id | BCM_FIELD_EM_ID_BASE;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
