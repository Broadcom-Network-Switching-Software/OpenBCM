/*! \file field_presel.c
 *
 * Field Module Presel Implementation above SDKLT.
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

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/*!
 * \brief bcmint_field_stage_from_presel_get
 *
 * Depending on the range of presel ID and validity of presel ID
 * determine the stage_id and return stage_info.
 *
 * \param [in] unit       - Unit Number.
 * \param [in] presel_id  - presel ID.
 * \param [in] stage_info - Pipeline stage info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_stage_from_presel_get(
    int unit,
    bcm_field_presel_t presel_id,
    bcmint_field_stage_info_t **stage_info)
{
    bool stage_found = FALSE;
    bool presel_found = FALSE;
    bcmi_ltsw_field_stage_t stage_id;
    bcm_field_presel_t act_presel_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given entry ID */
    act_presel_id = presel_id & BCM_FIELD_ID_MASK;

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_stage_info_get(unit, stage_id, stage_info));
        if ((presel_id > (*stage_info)->stage_base_id) &&
            (presel_id <=
            (*stage_info)->stage_base_id +
            (*stage_info)->presel_info->pid_max)) {

            stage_found = TRUE;
            /* Check whether the corresponding presel entry ID already exists. */
            if (SHR_BITGET((*stage_info)->presel_info->pid_bmp.w,
                           act_presel_id)) {
                presel_found = TRUE;
            }
            break;
        }
    }
    if (stage_found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FP,
            (BSL_META_U(unit,
            "Error: presel ID is not in valid range\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    if (presel_found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FP,
            (BSL_META_U(unit,
            "Error: presel ID is not existing\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_presel_default_group_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int *presel_group)
{
    int dunit = 0;
    int iter;
    int gid;
    bcm_field_qset_t qset;
    const bcm_field_qual_map_t *lt_map_db = NULL;
    bcmlt_entry_handle_t presel_grp_templ = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info->tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(stage_info->tbls_info->presel_group_sid, SHR_E_UNAVAIL);

    if (stage_info->presel_group_def != 0) {
        *presel_group = stage_info->presel_group_def;
        SHR_EXIT();
    }

    for (gid = 1; gid <= stage_info->presel_info->gid_max; gid++) {
        if (!SHR_BITGET(stage_info->presel_info->pgid_bmp.w, gid)) {
            break;
        }
    }

    if (gid > stage_info->presel_info->gid_max) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    BCM_FIELD_QSET_INIT(qset);
    /* Retreive Presel LT map and get the QSET */
    for (iter = 0; iter < stage_info->presel_qual_db_count; iter++) {
         lt_map_db = stage_info->presel_qual_lt_map_db + iter;
         BCM_FIELD_QSET_ADD(qset, lt_map_db->qual);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for Presel Group Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->presel_group_sid,
                             &presel_grp_templ));

    /* Add presel_grp_templ_id field to Presel grp LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(presel_grp_templ,
                              stage_info->tbls_info->presel_group_key_fid,
                              gid));
    /* Add presel QSET */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                   &qset,
                                   0,
                                   stage_info, 1, 1,
                                   &presel_grp_templ));

    /* Insert presel grp template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(presel_grp_templ, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    stage_info->presel_group_def = gid;

    /* Set group id in bmp */
    SHR_BITSET(stage_info->presel_info->pgid_bmp.w, gid);

    *presel_group = gid;

exit:
    if (presel_grp_templ != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(presel_grp_templ);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_presel_default_group_destroy(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    int group_id;
    int presel_gid;
    int presel_grp_cnt = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t presel_grp_templ = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info->tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(stage_info->tbls_info->presel_group_sid, SHR_E_UNAVAIL);

    if (stage_info->presel_group_def == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    presel_gid = stage_info->presel_group_def;

    /* Ensure the presel group is not assigned to more than one group */
    for (group_id = stage_info->stage_base_id;
         group_id <= (stage_info->stage_base_id +
                      stage_info->group_info->gid_max);
         group_id++) {

         /* Check whether the corresponding group ID exists. */
         if (SHR_BITGET(stage_info->group_info->grpid_bmp.w,
                        (group_id & BCM_FIELD_ID_MASK))) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmint_field_group_oper_lookup(unit, group_id,
                                                 stage_info,
                                                 &group_oper));
             if (group_oper->presel_group == stage_info->presel_group_def) {
                 presel_grp_cnt++;
             }
         }
    }

    if (presel_grp_cnt > 1) {
        SHR_EXIT();
    }

    /* Delete the presel group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &presel_grp_templ,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL,
                                  stage_info->tbls_info->presel_group_sid,
                                  stage_info->tbls_info->presel_group_key_fid,
                                  0, presel_gid, NULL,
                                  NULL,
                                  0,
                                  0, 0,
                                  1));

    /* Clear group id in bmp */
    SHR_BITCLR(stage_info->presel_info->pgid_bmp.w, presel_gid);
    stage_info->presel_group_def = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_presel_group_config_create(
    int unit,
    bcm_field_presel_group_config_t *group_config)
{
    int dunit = 0;
    int iter;
    int gid;
    bcmi_ltsw_field_stage_t stage;
    bcmint_field_stage_info_t *stage_info;
    bcmlt_entry_handle_t presel_grp_templ = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_stage_get(unit,
                                      &(group_config->qset),
                                      &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit,
                                     stage,
                                     &stage_info));

    SHR_NULL_CHECK(stage_info->tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(stage_info->tbls_info->presel_group_sid, SHR_E_UNAVAIL);

    /* Verify requested QSET is supported by the stage. */
    if (FALSE == bcmint_field_qset_is_subset(unit, &group_config->qset,
                                             stage_info, 1, NULL)) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "Error: Qualifier set is not"
               " supported by the device.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Check for presel group ID. if exists check if it is in range. */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_ID) {
        gid = group_config->presel_group;

        /* Verify whether the given ID is in group ID's range */
        if (!((gid > stage_info->stage_base_id) &&
              (gid <= (stage_info->stage_base_id +
                       stage_info->presel_info->gid_max)))) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: This Presel Group ID[0x%x] is not in valid range\n"),
                                                              gid));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Mask the stage bits from given group ID */
        gid = group_config->presel_group & BCM_FIELD_ID_MASK;

        /* Check whether it is already in use. */
        if (SHR_BITGET(stage_info->presel_info->pgid_bmp.w, gid)) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: This Presel Group ID is already in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    } else {
        bool gid_found = FALSE;

        /* Generate group ID */
        gid = stage_info->presel_info->last_allocated_pgid + 1;
        for (iter = 1; iter <= stage_info->presel_info->gid_max; iter++) {
            if (gid > stage_info->presel_info->gid_max) {
                gid = 1;
            }
            if (!SHR_BITGET(stage_info->presel_info->pgid_bmp.w, gid)) {
                gid_found = TRUE;
                break;
            }
            gid++;
        }

        if (gid_found == FALSE) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: All Group ID's are in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }

        group_config->presel_group = (stage_info->stage_base_id | gid);
        stage_info->presel_info->last_allocated_pgid = gid;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for Presel Group Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->presel_group_sid,
                             &presel_grp_templ));

    /* Add presel_grp_templ_id field to Presel grp LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(presel_grp_templ,
                              stage_info->tbls_info->presel_group_key_fid,
                              gid));
    /* Add presel QSET */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                   &group_config->qset,
                                   0,
                                   stage_info, 1, 1,
                                   &presel_grp_templ));

    /* Insert presel grp template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(presel_grp_templ, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    SHR_BITSET(stage_info->presel_info->pgid_bmp.w, gid);

exit:
    if (presel_grp_templ != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(presel_grp_templ);
    }

    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_stage_from_presel_group_get(
    int unit,
    bcm_field_group_t group,
    bcmint_field_stage_info_t **stage_info)
{
    int group_id = 0;
    int stage_base_id;
    bcmi_ltsw_field_stage_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_base_id = group & ~BCM_FIELD_ID_MASK;
    switch (stage_base_id) {
        case BCM_FIELD_IFP_ID_BASE:
             stage_id = bcmiFieldStageIngress;
             break;
        case BCM_FIELD_VFP_ID_BASE:
             stage_id = bcmiFieldStageVlan;
             break;
        case BCM_FIELD_EFP_ID_BASE:
             stage_id = bcmiFieldStageEgress;
             break;
        case BCM_FIELD_EM_ID_BASE:
             stage_id = bcmiFieldStageExactMatch;
             break;
        case BCM_FIELD_FT_ID_BASE:
             stage_id = bcmiFieldStageFlowTracker;
             break;
        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
             break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, stage_info));

    /* Mask the stage bits from given group ID */
    group_id = group & BCM_FIELD_ID_MASK;

    /* Check whether the corresponding group ID exists. */
    if (SHR_BITGET((*stage_info)->presel_info->pgid_bmp.w, group_id) == 0) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Error: Group ID not found\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmint_field_presel_group_destroy
 *
 * Destroy te given presel group.
 *
 * \param [in] unit  - BCM device number.
 * \param [in] presel_grp - Presel Group Identifier.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int
bcmint_field_presel_group_destroy(int unit,
                                  bcm_field_group_t presel_grp)
{
    int group_id = 0;
    int presel_gid = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t presel_grp_templ = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_stage_from_presel_group_get(unit, presel_grp, &stage_info));

    SHR_NULL_CHECK(stage_info->tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(stage_info->tbls_info->presel_group_sid, SHR_E_UNAVAIL);

    tbls_info = stage_info->tbls_info;
    presel_gid = presel_grp & BCM_FIELD_ID_MASK;

    /* Check whether presel group is in use by any field groups. */
    for (group_id = stage_info->stage_base_id;
         group_id <= (stage_info->stage_base_id +
                      stage_info->group_info->gid_max);
         group_id++) {

         /* Check whether the corresponding group ID exists. */
         if (SHR_BITGET(stage_info->group_info->grpid_bmp.w,
                        (group_id & BCM_FIELD_ID_MASK))) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmint_field_group_oper_lookup(unit, group_id,
                                                 stage_info,
                                                 &group_oper));
             if (group_oper->presel_group == presel_gid) {
                 SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
             }
         }
    }

    /* Delete the presel group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &presel_grp_templ,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->presel_group_sid,
                                      tbls_info->presel_group_key_fid,
                                      0, presel_gid, NULL,
                                      NULL, 0, 0, 0, 1));

    /* Clear group id in bmp */
    SHR_BITCLR(stage_info->presel_info->pgid_bmp.w, presel_gid);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Do search for given presel entry id and return success or failure.
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
bcmint_field_presel_oper_lookup(
    int unit,
    int presel_entry_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_presel_oper_t **presel_oper)
{
    uint32_t hash_offset = 0;
    bcmi_field_ha_presel_oper_t *oper = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given entry ID */
    presel_entry_id = presel_entry_id & BCM_FIELD_ID_MASK;

    /* Calculate the hash */
    hash_offset = presel_entry_id % stage_info->presel_info->hash_size;

    FP_HA_PRESEL_OPER_GET(unit, stage_info->stage_id,
                          hash_offset, oper);
    if (!oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    do {
        if (oper->presel_entry_id == presel_entry_id) {
            break;
        }
        FP_HA_PRESEL_OPER_NEXT(unit, oper, oper);
    } while (oper != NULL);

    /* Return not found if operational ptr is null */
    if (!oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (presel_oper != NULL) {
        *presel_oper = oper;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Insert the given entry ID at correct hash index
 *
 * Algorithm:
 *  1. Application provides required information about the entry
 *     to identify the presel_entry location in entry hash.
 *  2. Calculate the hash for the given entry Id.
 *  3. Check entry id is present in linked list of entry ids
 *     having the same hash value.
 *  4. If presel_entry_id exists, return error. code has some bug
 *  5. If presel_entry_id does not exist, create allocate a entry hash node and
 *     insert at proper location.
 */
int
ltsw_field_presel_oper_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int presel_entry_id,
    bcmi_field_ha_presel_oper_t **presel_oper)
{
    uint8_t ha_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_presel_oper_t *oper = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    presel_entry_id = presel_entry_id & BCM_FIELD_ID_MASK;

    /* Calculate the hash */
    hash_offset = presel_entry_id % stage_info->presel_info->hash_size;

    FP_HA_PRESEL_BASE_BLK_GET(unit,
                              stage_info->stage_id,
                              hash_offset,
                              entry_base);

    FP_STAGE_PRESEL_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
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

    /* Update presel operation structure */
    oper->presel_entry_id = presel_entry_id;

    if (presel_oper != NULL) {
        *presel_oper = oper;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Remove given presel operational db for a given presel entry
 */
int
ltsw_field_presel_oper_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int presel_entry_id)
{
    int blk_type;
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    bcmi_field_ha_presel_oper_t *oper = NULL;
    bcmi_field_ha_presel_oper_t *oper_head = NULL;
    bcmi_field_ha_presel_oper_t *oper_temp = NULL;
    bcmi_field_ha_presel_info_t *presel_info = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    presel_entry_id = presel_entry_id & BCM_FIELD_ID_MASK;

    presel_info = stage_info->presel_info;
    SHR_NULL_CHECK(presel_info, SHR_E_INTERNAL);

    /* Calculate the hash */
    hash_offset = presel_entry_id % presel_info->hash_size;

    FP_HA_PRESEL_BASE_BLK_GET(unit,
                              stage_info->stage_id,
                              0,
                              entry_base);
    FP_HA_PRESEL_BASE_OPER_GET(unit,
                               stage_info->stage_id,
                               entry_base,
                               hash_offset,
                               oper_head);
    entry_base = entry_base + hash_offset;
    if ((entry_base == NULL) || (oper_head == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    oper = oper_head;
    do {
        if (oper->presel_entry_id == presel_entry_id) {
            if (oper_temp == NULL) {
                ha_blk_id = entry_base->blk_id;
                ha_blk_offset = entry_base->blk_offset;
                entry_base->blk_id = oper->next.blk_id;
                entry_base->blk_offset = oper->next.blk_offset;
            } else {
                ha_blk_id = oper_temp->next.blk_id;
                ha_blk_offset = oper_temp->next.blk_offset;
                oper_temp->next.blk_id = oper->next.blk_id;
                oper_temp->next.blk_offset = oper->next.blk_offset;
            }
            break;
        }
        oper_temp = oper;
        FP_HA_PRESEL_OPER_NEXT(unit, oper, oper);
    } while (oper != NULL);

    /* Return not found if hash ptr is null */
    if (!oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    FP_STAGE_PRESEL_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_blk_element_release(unit, ha_blk_id,
                                             ha_blk_offset,
                                             blk_type));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief To get field module stage structure.
 *
 * \param [in] Unit Number.
 * \param [in] stage id.
 * \param [out] stage structure.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmint_field_stage_enum_info_get(int unit,
                                 bcm_field_stage_t stage,
                                 bcmint_field_stage_info_t **stage_info)
{
    bcmi_ltsw_field_stage_t stage_id = 0;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    switch(stage) {
        case bcmFieldStageIngress:
            stage_id = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage_id = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage_id = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
            stage_id = bcmiFieldStageExactMatch;
            break;
        case bcmFieldStageIngressFlowtracker:
            stage_id = bcmiFieldStageFlowTracker;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    *stage_info = &(fp_control->stage_info[stage_id]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_presel_entry_create_with_id
 *
 * Create a field presel entry in a given stage with given ID
 *
 * \param [in] unit     - BCM device number.
 * \param [in] stage    - stage ID
 * \param [in] presel_id - Presel entry ID
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *    !BCM_E_NONE  - Failure
 */
int
bcmint_field_presel_entry_create_with_id(int unit,
                                         bcmi_ltsw_field_stage_t stage,
                                         int presel_id)
{
    int dunit = 0;
    bcmlt_entry_handle_t presel_template = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));

    /* Entry handle allocate for Presel Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->presel_entry_sid,
                              &presel_template));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(presel_template,
                               stage_info->tbls_info->presel_entry_key_fid,
                               presel_id));

    /* Insert rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, presel_template,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != presel_template) {
        (void) bcmlt_entry_free(presel_template);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmint_field_presel_entry_destroy
 * Purpose:
 *     Delete entry and associated templates if any
 * Parameters:
 *     unit  - BCM device number
 *     entry - (IN) New entry
 * Returns:
 *     SHR_E_NONE        Success
 *     SHR_E_INIT        BCM unit not initialized
 *     SHR_E_NOT_FOUND   group not found in unit
 *     SHR_E_BUSY        presel_id is in use
 *     SHR_E_PARAM       param is invalid
 */
int
bcmint_field_presel_entry_destroy(int unit,
                                  bcm_field_presel_t presel_entry)
{
    int dunit = 0;
    bcm_field_presel_t presel_id = 0;
    int group_id = 0;
    int gid = 0;
    bool found;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bool presel_found = FALSE;
    bcmlt_entry_handle_t presel_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Mask the stage bits from given entry ID */
    presel_id = presel_entry & BCM_FIELD_ID_MASK;

    /* validate the entry range and retrieve the stage info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_presel_get(unit, presel_entry,
                                           &stage_info));

    /* go through the group list in the stage and find
     * if any group is using the presel
     */
    for (gid = stage_info->stage_base_id;
         gid <= (stage_info->stage_base_id +
                 stage_info->group_info->gid_max);
         gid++) {
        /* Mask the stage bits from given group ID */
        group_id = gid & BCM_FIELD_ID_MASK;

        /* Check whether the corresponding group ID exists. */
        if (SHR_BITGET(stage_info->group_info->grpid_bmp.w, group_id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_oper_lookup(unit, group_id,
                                                stage_info,
                                                &group_oper));
            FP_HA_GROUP_OPER_PRESEL_TEST(group_oper, presel_id, found);
            if (found == TRUE) {
                presel_found = TRUE;
                break;
            }
        }
    }
    if (presel_found == TRUE) {
        LOG_DEBUG(BSL_LS_BCM_FP,
        (BSL_META_U(unit,
        "Error: presel ID is in USE\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    /* Find provided entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_oper_lookup(unit, presel_id,
                                         stage_info,
                                         &presel_oper));

    /* Entry handle allocate for Presel Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->presel_entry_sid,
                              &presel_template));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(presel_template,
                               stage_info->tbls_info->presel_entry_key_fid,
                               presel_id));

    /* Delete presel template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, presel_template,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    /* Delete from entry hash */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_presel_oper_remove(unit,
                                       stage_info,
                                       presel_id));

    /* Clear entry id bitmap */
    SHR_BITCLR(stage_info->presel_info->pid_bmp.w, presel_id);

exit:
    if (BCMLT_INVALID_HDL != presel_template) {
        (void) bcmlt_entry_free(presel_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_presel_create_stage_id(int unit,
                                    bcm_field_stage_t stage,
                                    bcm_field_presel_t presel_id)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_presel_t  act_presel_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_enum_info_get(unit, stage, &stage_info));
    act_presel_id = presel_id & BCM_FIELD_ID_MASK;

    /* Verify whether the given ID is in Stage Entry ID's range */
    if ((presel_id > stage_info->stage_base_id) &&
        (presel_id <=
         stage_info->stage_base_id +
         stage_info->presel_info->pid_max)) {
        /* Check whether the corresponding presel entry ID already exists. */
        if (SHR_BITGET(stage_info->presel_info->pid_bmp.w, act_presel_id)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
              "Error: Presel entry ID is already in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP,
         (BSL_META_U(unit,
         "Error: Presel entry ID is not in valid range\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_entry_create_with_id(unit,
                                                  stage_info->stage_id,
                                                  act_presel_id));
    /* Add entry in hash list */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_presel_oper_insert(unit,
                                       stage_info,
                                       act_presel_id,
                                       NULL));

    /* Update the stage entry bitmap */
    SHR_BITSET(stage_info->presel_info->pid_bmp.w, act_presel_id);

exit:
    SHR_FUNC_EXIT();
}

/*
 * create a field presel ID with specified stage and group ID
 */
int
bcmint_field_presel_config_create(int unit,
                                   bcm_field_presel_config_t *presel_config)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_presel_t  pid = 0;
    bool is_free_presel_found = FALSE;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_config, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_enum_info_get(unit, presel_config->stage,
        &stage_info));

    if (presel_config->flags & BCM_FIELD_PRESEL_CREATE_WITH_ID) {
        if ((presel_config->presel_id & ~BCM_FIELD_ID_MASK) !=
            (stage_info->stage_base_id)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
             (BSL_META_U(unit,
             "Error: Presel entry ID is not valid for given stage\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        pid = (presel_config->presel_id) & BCM_FIELD_ID_MASK;
        /* Verify whether the given ID is in Stage Entry ID's range */
        if ((presel_config->presel_id > stage_info->stage_base_id) &&
            (presel_config->presel_id <=
            stage_info->stage_base_id +
             stage_info->presel_info->pid_max)) {
            /* Check whether the corresponding presel entry ID already exists.
             */
            if (SHR_BITGET(stage_info->presel_info->pid_bmp.w, pid)) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                  "Error: Presel entry ID is already in use\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
        } else {
            LOG_DEBUG(BSL_LS_BCM_FP,
             (BSL_META_U(unit,
             "Error: Presel entry ID is not in valid range\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        is_free_presel_found = FALSE;
        for (pid = 1; pid <= stage_info->presel_info->pid_max; pid++) {
            if (!(SHR_BITGET(stage_info->presel_info->pid_bmp.w, pid))) {
                is_free_presel_found = TRUE;
                break;
            }
        }

        if (!is_free_presel_found) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "Error: No free Presel entry ID is avaialable\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        presel_config->presel_id = pid | stage_info->stage_base_id;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_entry_create_with_id(unit,
                                                  stage_info->stage_id,
                                                  pid));
    /* Add entry in hash list */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_presel_oper_insert(unit,
                                       stage_info,
                                       pid,
                                       NULL));

    /* Update the stage entry bitmap */
    SHR_BITSET(stage_info->presel_info->pid_bmp.w, pid);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Remove All Presel Operational database
 */
int
bcmint_field_presel_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    uint32_t idx;
    uint8_t blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t blk_offset = 0;
    uint8_t next_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    uint32_t next_blk_offset = 0;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_presel_oper_t *oper = NULL;
    bcmi_field_ha_presel_oper_t *oper_next = NULL;
    bcmi_field_ha_blk_info_t *presel_base = NULL;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);

    FP_STAGE_PRESEL_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    FP_HA_PRESEL_BASE_BLK_GET(unit,
                              stage_info->stage_id,
                              0,
                              presel_base);
    for (idx = 0; idx < stage_info->presel_info->hash_size; idx++) {
        oper = NULL;
        FP_HA_PRESEL_BASE_OPER_GET(unit,
                                   stage_info->stage_id,
                                   presel_base,
                                   idx,
                                   oper);
        if (oper == NULL) {
            continue;
        }

        next_blk_id = presel_base->blk_id;
        next_blk_offset = presel_base->blk_offset;
        /* Search the list of entries */
        do {
            blk_id = next_blk_id;
            blk_offset = next_blk_offset;

            FP_HA_PRESEL_OPER_NEXT(unit, oper, oper_next);
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
bcmint_ltsw_field_source_class_mode_set(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t mode)
{
    int dunit = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    char *key_fid_symbol = NULL;
    uint8_t fp_pipe = 0xff;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_enum_info_get(unit,
                                          stage_id,
                                          &stage_info));
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    SHR_NULL_CHECK(tbls_info->src_class_mode_sid, SHR_E_UNAVAIL);

    /* Entry handle allocate for LT */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             tbls_info->src_class_mode_sid,
                             &lt_hdl));

    /* get source class mode LT symbol */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_src_class_enum_to_lt_symbol_get(unit, mode, &key_fid_symbol));

    /* Add src_class_mode_key_fid field to Src Class mode LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(lt_hdl,
                                      tbls_info->src_class_mode_key_fid,
                                      key_fid_symbol));

    /* Get FP Pipe */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_group_pipe_get(unit, stage_info, pbmp, &fp_pipe));

    if (0xff != fp_pipe) {
        /* Add PIPE to thr LT Hdl */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(lt_hdl, "PIPE", fp_pipe));
    }

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, lt_hdl,
                                  BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != lt_hdl) {
        (void) bcmlt_entry_free(lt_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_source_class_mode_from_stage_info_get(
        int unit,
        bcm_pbmp_t pbmp,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_src_class_mode_t *mode)
{
    int dunit = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    char *key_fid_symbol = NULL;
    uint8_t fp_pipe = 0xff;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    SHR_NULL_CHECK(tbls_info->src_class_mode_sid, SHR_E_UNAVAIL);

    /* Entry handle allocate for LT */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             tbls_info->src_class_mode_sid,
                             &lt_hdl));

    /* Get FP Pipe */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_group_pipe_get(unit, stage_info, pbmp, &fp_pipe));

    if (0xff != fp_pipe) {
        /* Add PIPE to thr LT Hdl */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(lt_hdl, "PIPE", fp_pipe));
    }

    /* Lookup Source Class Mode LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, lt_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(lt_hdl,
                                      tbls_info->src_class_mode_key_fid,
                                      (const char **)&key_fid_symbol));

    /* get source class mode LT symbol */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_src_class_lt_symbol_to_enum_get(unit, key_fid_symbol, mode));

exit:
    if (BCMLT_INVALID_HDL != lt_hdl) {
        (void) bcmlt_entry_free(lt_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_source_class_mode_get(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t *mode)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_enum_info_get(unit,
                                          stage_id,
                                          &stage_info));
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_source_class_mode_from_stage_info_get(unit, pbmp, stage_info, mode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_qualify_MixedSrcClassId(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t data,
        bcm_field_src_class_t mask)
{

    int dunit = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_src_class_mode_t mode;
    bcm_qual_field_t qual_info;
    uint32 hw_data = 0, hw_mask = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_entry_get(dunit, entry, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_source_class_mode_from_stage_info_get(unit,
                                                                 pbmp,
                                                                 stage_info,
                                                                 &mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_src_class_hw_fields_encode_set(unit,
                                                        mode,
                                                        data,
                                                        mask,
                                                        &hw_data,
                                                        &hw_mask));

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMixedSrcClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &hw_data;
    qual_info.qual_mask = (void *) &hw_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_qualify_MixedSrcClassId_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t *data,
        bcm_field_src_class_t *mask)
{

    int dunit = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_src_class_mode_t mode;
    bcm_qual_field_t qual_info;
    uint32 hw_data[1] = {0}, hw_mask[1] = {0};

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMixedSrcClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &hw_data;
    qual_info.qual_mask = (void *) &hw_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_entry_get(dunit, entry, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_source_class_mode_from_stage_info_get(unit,
                                                                 pbmp,
                                                                 stage_info,
                                                                 &mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_src_class_hw_fields_decode_get(unit,
                                                        mode,
                                                        hw_data[0],
                                                        hw_mask[0],
                                                        data,
                                                        mask));

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_presel_entry_prio_set(
    int unit,
    bcm_field_entry_t entry, int prio)
{
    uint64_t ent_lt_prio_min = 0;
    uint64_t ent_lt_prio_max = 0;
    bcm_field_entry_t entry_id = 0;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t entry_handle= BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

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
             stage_info->tbls_info->presel_entry_sid,
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
        (bcmint_field_presel_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &presel_oper));

    /* No change in prio value. return success */
    if (prio == presel_oper->priority) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &entry_handle,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->presel_entry_sid,
                                      stage_info->tbls_info->presel_entry_key_fid,
                                      0, entry_id, NULL,
                                      stage_info->tbls_info->presel_entry_prio_fid,
                                      prio, 0, 0, 1));

    /* update the s/w entry strucutre with new priority */
    presel_oper->priority = prio;

exit:
    if (BCMLT_INVALID_HDL != entry_handle) {
        (void) bcmlt_entry_free(entry_handle);
    }
    SHR_FUNC_EXIT();
}


int
bcmint_field_presel_entry_destroy_all(int unit,
                                      bcmint_field_stage_info_t *stage_info)
{
    int stage_id;
    uint32_t idx;
    bcm_field_presel_t presel_id = 0;
    bcmi_field_ha_presel_oper_t *oper = NULL;
    bcmi_field_ha_blk_info_t *presel_base = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = stage_info->stage_id;

    if (FP_HA_INFO(unit) == NULL) {
        SHR_EXIT();
    }

    /* Field Stage clear */
    FP_HA_PRESEL_BASE_BLK_GET(unit,
                              stage_info->stage_id,
                              0,
                              presel_base);
    if (presel_base == NULL) {
        SHR_EXIT();
    }
    for (idx = 0; idx < stage_info->presel_info->hash_size; idx++) {
        oper = NULL;
        FP_HA_PRESEL_BASE_OPER_GET(unit,
                                stage_id,
                                presel_base,
                                idx,
                                oper);
        if (oper == NULL) {
            continue;
        }
        do {
            presel_id = oper->presel_entry_id;
            FP_HA_PRESEL_OPER_NEXT(unit, oper, oper);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_entry_destroy(unit, presel_id));
        } while (oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_presel_qual_validate(int unit,
                            bcmint_field_stage_info_t *stage_info,
                            int presel_grp_id,
                            int presel_entry_id)
{
    bcm_field_qset_t qset, grp_qset, entry_qset;
    bcmi_field_qualify_t qual_idx;

    SHR_FUNC_ENTER(unit);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_INIT(grp_qset);
    BCM_FIELD_QSET_INIT(entry_qset);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_qset_get(unit,
                                  stage_info->stage_id,
                                  presel_grp_id,
                                  1,
                                  &grp_qset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_qset_get(unit,
                                  stage_info->stage_id,
                                  presel_entry_id,
                                  0,
                                  &entry_qset));

    /* Retreive Presel LT map and get the QSET */
    for (qual_idx = 0; qual_idx < BCM_FIELD_QUALIFY_MAX; qual_idx++) {
        if (BCM_FIELD_QSET_TEST(entry_qset, qual_idx)) {
            if (!BCM_FIELD_QSET_TEST(grp_qset, qual_idx)) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit, "Error: qualifier not part of"
                  " presel group qset.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}
