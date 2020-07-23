/*! \file bcmecmp_weighted_underlay_group_imm.c
 *
 * BCMECMP ECMP_WEIGHTED_UNDERLAYt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_ts.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmecmp/bcmecmp_imm.h>
#include <bcmecmp/bcmecmp_group_util.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */
/*!
 * \brief ECMP/ECMP_FAST/ECMP_HIERARCHICAL LT entry fields parse routine.
 *
 * Parse ECMP/ECMP_FAST/ECMP_HIERARCHICAL logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [in] sid Logical table database source identifier.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcmecmp_weighted_underlay_group_lt_fields_parse(int unit,
                                       const bcmltd_field_t *in,
                                       bcmltd_sid_t sid,
                                       bcmecmp_lt_entry_t *lt_entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_fields_parse:LT_SID=%d.\n"),
                sid));

    gen_field = in;
    while (gen_field) {
        SHR_IF_ERR_VERBOSE_EXIT
            (BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill)(unit,
                                                         gen_field, lt_entry));
        gen_field = gen_field->next;
    }

    exit:
        SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmecmp_weighted_underlay_group_validate(int unit,
                                bcmltd_sid_t sid,
                                bcmimm_entry_event_t event,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data. */
    bcmecmp_lt_entry_t *cur_lt_entry = NULL; /* current ECMP LT entry data. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_group_validate:SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    /*
     * Validate Logical Table SID value. Verify if it matches Single ECMP
     * resolution group SID value initialized for the configured device ECMP
     * resolution mode.
     */
    if (sid != BCMECMP_TBL_LTD_SID(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unsupported logical table for "
                                "ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (event == BCMIMM_ENTRY_LOOKUP) {
        SHR_EXIT();
    }

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Initialize Logical Table Entry buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_entry_t_init(unit, sid, lt_entry));

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_weighted_underlay_group_lt_fields_parse
            (unit, key, sid, lt_entry));

    if (((event == BCMIMM_ENTRY_UPDATE) ||
        (event == BCMIMM_ENTRY_INSERT)) && (data != NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_weighted_underlay_group_lt_fields_parse
            (unit, data, sid, lt_entry));
    }

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Get logical table entry buffer pointer. */
        cur_lt_entry = BCMECMP_CURRENT_LT_ENT_PTR(unit);

        /* Initialize Logical Table Entry buffer. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_entry_t_init(unit, sid, cur_lt_entry));

        /* Parse the input fields and retrieve the field values. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_weighted_underlay_group_lt_fields_parse
                (unit, key, sid, cur_lt_entry));

        /* Get current fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_fields_lookup(unit, cur_lt_entry,
                                  BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill)));

        if (BCMECMP_LT_FIELD_GET(cur_lt_entry->fbmp,
                                               BCMECMP_LT_FIELD_WEIGHTED_SIZE)) {
            cur_lt_entry->max_paths =
                 BCMECMP_FNCALL(unit, weight_size_to_max_paths)
                     (cur_lt_entry->weighted_size);
        }
    }

    /* Make sure max_paths get correct value. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)) {
        lt_entry->max_paths =
            BCMECMP_FNCALL(unit, weight_size_to_max_paths)
                (lt_entry->weighted_size);
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
    } else {
        if (BCMIMM_ENTRY_INSERT != event) {
            lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_entry->ecmp_id);
        }
    }

    /* Validate input parameter values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_input_params_validate(unit, event, sid, lt_entry));

    exit:
        BCMECMP_UNLOCK(unit);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operational argument.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmecmp_weighted_underlay_group_stage(int unit,
                             bcmltd_sid_t sid,
                             const bcmltd_op_arg_t *op_arg,
                             bcmimm_entry_event_t event,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{

    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_weighted_underlay_group_stage: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);
    lt_entry->op_arg = op_arg;

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_group_add(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_WEIGHTED_MODE) &&
                lt_entry->weighted_mode !=
                BCMECMP_GRP_WEIGHTED_MODE(unit, lt_entry->ecmp_id)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_group_update_entry_set(unit, lt_entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_group_delete(unit, lt_entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_group_add(unit, lt_entry));
                break;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_group_update(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_group_delete(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Check if atomic transactions support is enabled for this BCM unit i.e.
     * feature_conf->dis_atomic_trans == 0 and set staging flags if support is
     * enabled.
     */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_TBL_BK_ENT_STAGED_SET
            (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
             lt_entry->ecmp_id);
        /*
         * Save the Group's Trasanction ID for use during commit/abort
         * operation.
         */
        BCMECMP_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = op_arg->trans_id;
    }

    /*No readonly field is added*/
    if (output_fields) {
        output_fields->count = 0;
    }

exit:
        BCMECMP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP group imm commit call back
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmecmp_weighted_underlay_group_commit(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    bcmecmp_info_t *ecmp_info;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    bcmecmp_grp_attr_t *grp_bk_attr, *grp_bk_attr_base;
    bcmecmp_hw_entry_attr_t *hw_ent_attr, *hw_ent_attr_base;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_weighted_underlay_group_commit: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    grp_bk_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr);
    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                           (ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ent_arr);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id++ ) {
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if (!(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED)
            || (trans_id != BCMECMP_GRP_TRANS_ID(unit, ecmp_id))) {
            continue;
        }
        if (hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED) {
            hw_ent_attr->flags &= (~BCMECMP_ENTRY_DEFRAG_STAGED);
        }
    }

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }

    if (sid != BCMECMP_TBL_LTD_SID(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT not valid for ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                        (ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ent_bk_arr);
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        grp_bk_attr = grp_bk_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                             ecmp_id) &&
             !(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED))
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id),
                     BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit,
                                                     group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                                     ecmp_id),
                     sid));

        BCMECMP_GRP_TYPE_BK(unit, ecmp_id) = BCMECMP_GRP_TYPE(unit, ecmp_id);
        BCMECMP_GRP_LT_SID_BK(unit, ecmp_id) = BCMECMP_GRP_LT_SID(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id) = BCMECMP_GRP_LB_MODE(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_WEIGHTED_MODE_BK(unit, ecmp_id) =
            BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_NUM_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id) =
                        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

        grp_bk_attr->in_use_itbm = grp_attr->in_use_itbm;

        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                            ecmp_id);
        hw_ent_attr->flags &= (~BCMECMP_ENTRY_DEFRAG_STAGED);
    }

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u.\n"),
                     trans_id, sid));
        BCMECMP_UNLOCK(unit);
        SHR_FUNC_EXIT();
}

/*!
 * \brief weighted group transaction abort
 *
 * Rollback the weighted group entry from backup state
 * as current transaction is getting aborted.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval None.
 */
static void
bcmecmp_weighted_underlay_group_abort(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t grp_list_hdl;
    shr_itbm_list_hdl_t memb_list_hdl;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    bcmecmp_grp_attr_t *grp_bk_attr, *grp_bk_attr_base;
    bcmecmp_hw_entry_attr_t *hw_ent_attr, *hw_ent_attr_base;
    uint32_t bucket = 0, first = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_group_abort: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_list_hdl = ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].list_hdl;
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    grp_bk_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr);

    if (sid != BCMECMP_TBL_LTD_SID(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT not valid for ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                        (ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ent_bk_arr);
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                                            ecmp_id) &&
             !(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED))
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id),
                     BCMECMP_TBL_BK_ENT_STAGED_GET
                        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                         ecmp_id),
                     sid));
        /*
         * Release the itbm resources for all the groups
         * 1. Moved as part of defragmentation.
         * 2. For insert or update operation
         *    release the resources in current transaction
         */
        if (grp_attr->in_use_itbm) {
            bcmecmp_itbm_bucket_first_get(unit, grp_attr->grp_type,
                                          grp_attr->weighted_mode,
                                          grp_attr->mstart_idx,
                                          &bucket, &first);
            memb_list_hdl = ecmp_info->member[grp_attr->grp_type].list_hdl;
            if (bcmecmp_grp_is_member_replication(grp_attr->grp_type,
                                                  grp_attr->weighted_mode)) {
                memb_list_hdl =
                        ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].list_hdl_1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free(memb_list_hdl,
                                          grp_attr->max_paths,
                                          bucket,
                                          first));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free(grp_list_hdl,
                                          1,
                                          SHR_ITBM_INVALID,
                                          ecmp_id));
        }
    }

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        grp_bk_attr = grp_bk_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                                            ecmp_id) &&
             !(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED))
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))) {
            continue;
        }

        /*
         * Add the itbm resources for all the groups.
         * 1. Moved as part of defragmentation based on backup state.
         * 2. Insert or update operation is being
         *    performed in current transaction,
         *    recover the itbm resources based on backup state.
         * 3. If there is a delete operation in current transaction
         *    recover the itbm rresource based on backup state, since
         *    the transaction has aborted.
         */
        if (grp_bk_attr->in_use_itbm) {
            bcmecmp_itbm_bucket_first_get(unit, grp_bk_attr->grp_type,
                                          grp_bk_attr->weighted_mode,
                                          grp_bk_attr->mstart_idx,
                                          &bucket, &first);
            memb_list_hdl = ecmp_info->member[grp_bk_attr->grp_type].list_hdl;
            if (bcmecmp_grp_is_member_replication(grp_bk_attr->grp_type,
                                                  grp_bk_attr->weighted_mode)) {
                memb_list_hdl =
                        ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].list_hdl_1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id(memb_list_hdl,
                                              grp_bk_attr->max_paths,
                                              bucket,
                                              first));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id(grp_list_hdl,
                                              1,
                                              SHR_ITBM_INVALID,
                                              ecmp_id));
        }
        BCMECMP_GRP_TYPE(unit, ecmp_id) = BCMECMP_GRP_TYPE_BK(unit, ecmp_id);
        BCMECMP_GRP_LT_SID(unit, ecmp_id) = BCMECMP_GRP_LT_SID_BK(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_GRP_LB_MODE_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id) =
            BCMECMP_GRP_WEIGHTED_MODE_BK(unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS_BK(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS_BK(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) =
                BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id);
        grp_attr->in_use_itbm = grp_bk_attr->in_use_itbm;
        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                                            ecmp_id);
        hw_ent_attr->flags &= (~BCMECMP_ENTRY_DEFRAG_STAGED);
    }

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u.\n"),
                     trans_id, sid));
        BCMECMP_UNLOCK(unit);
        return;
}

/*!
 * \brief ECMP_WEIGHTED In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to ECMP_WEIGHTED logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {
    /*! Validate function. */
    .validate = bcmecmp_weighted_underlay_group_validate,

    /*! Extended staging function. */
    .op_stage = bcmecmp_weighted_underlay_group_stage,

    /*! Commit function. */
    .commit = bcmecmp_weighted_underlay_group_commit,

    /*! Abort function. */
    .abort = bcmecmp_weighted_underlay_group_abort
};

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief ECMP WEIGHTED IMM callback register
 *
 * regist ECMP_WEIGHTED_UDERLAY LT callback in IMM module
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid  Logic table ID.
 *
 * \retval None.
 */
int
bcmecmp_weighted_underlay_group_imm_register(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for ECMP_WEIGHTED_UNDERLAYt here.
     */
    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 sid,
                                 &event_hdl,
                                 NULL));
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Registered IMM event handler for ECMP SID:%u.\n"),
                  sid));
    }

exit:
    SHR_FUNC_EXIT();
}
