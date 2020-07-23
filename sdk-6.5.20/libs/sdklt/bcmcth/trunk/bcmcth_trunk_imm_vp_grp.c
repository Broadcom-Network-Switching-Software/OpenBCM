/*! \file bcmcth_trunk_imm_vp_grp.c
 *
 * BCMCTH  TRUNK_VP IMM handler.
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
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmcth/bcmcth_trunk_vp_common_imm.h>
#include <bcmcth/bcmcth_trunk_vp_grpmgr.h>
#include <bcmcth/bcmcth_trunk_vp_imm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK
/*******************************************************************************
 * Local definitions
 */
/*!
 * \brief TRUNK_VP LT entry fields parse routine.
 *
 * Parse TRUNK_VP logical table entry logical fields
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
bcmcth_trunk_vp_group_lt_fields_parse(int unit,
                                      const bcmltd_field_t *in,
                                      bcmltd_sid_t sid,
                                      bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Get the TRUNK device variant driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);
    vp_drv_var = drv_var->trunk_vp_var;
    SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmcth_trunk_vp_lt_fields_parse:LT_SID=%d.\n"),
                sid));

    gen_field = in;
    while (gen_field) {
        if (sid == (uint32_t) vp_drv_var->ids->trunk_vp_ltid) {
            lt_entry->grp_type = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;
        } else if (sid == (uint32_t) vp_drv_var->ids->trunk_vp_weighted_ltid) {
            lt_entry->grp_type = BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
              (bcmcth_trunk_drv_vp_lt_fields_fill(
                     unit,
                     gen_field,
                     lt_entry));
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
bcmcth_trunk_vp_group_validate(int unit,
                               bcmltd_sid_t sid,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context)
{
    bcmcth_trunk_vp_lt_entry_t *lt_entry = NULL; /* TRUNK_VP LT entry data. */

    SHR_FUNC_ENTER(unit);


    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_lth_group_validate:SID=%d.\n"),
              sid));
    BCMCTH_TRUNK_VP_INIT(unit);
    BCMCTH_TRUNK_VP_LOCK(unit);

    if (event == BCMIMM_ENTRY_LOOKUP) {
        SHR_EXIT();
    }

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMCTH_TRUNK_VP_LT_ENT_PTR(unit);

    /* Initialize Logical Table Entry buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_lt_entry_t_init(unit, sid, lt_entry));
    lt_entry->event = event;

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_group_lt_fields_parse(unit, key, sid, lt_entry));

    if (((event == BCMIMM_ENTRY_UPDATE) ||
        (event == BCMIMM_ENTRY_INSERT)) && (data != NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_group_lt_fields_parse(unit, data, sid, lt_entry));
    }

   if (event == BCMIMM_ENTRY_UPDATE) {
       if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
            BCMCTH_TRUNK_VP_LT_FIELD_WEIGHTED_SIZE)
            && BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(lt_entry->grp_type)) {

            /* Retrive and use group lb_mode value. */
            lt_entry->weighted_size =
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, lt_entry->trunk_vp_id);

            lt_entry->max_members =
                    bcmcth_trunk_vp_weighted_size_convert_to_member_cnt(
                     lt_entry->weighted_size);

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "WEIGHTED_SIZE=%d.\n"), lt_entry->weighted_size));
        }
    }

    /*
     * TRUNK_VP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied or invalid.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                                      BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID)
        || (lt_entry->trunk_vp_id <  BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                             (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))
        || (lt_entry->trunk_vp_id > BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                             (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID field value error.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate input parameter values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_lt_input_params_validate(unit, event, sid, lt_entry));

exit:
        BCMCTH_TRUNK_VP_UNLOCK(unit);
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
bcmcth_trunk_vp_group_stage(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_op_arg_t *op_arg,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{

    bcmcth_trunk_vp_lt_entry_t *lt_entry = NULL; /* TRUNK_VP entry data. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_stage: LT_SID=%d.\n"),
              sid));
    BCMCTH_TRUNK_VP_INIT(unit);
    BCMCTH_TRUNK_VP_LOCK(unit);

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMCTH_TRUNK_VP_LT_ENT_PTR(unit);
    lt_entry->op_arg = op_arg;

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_vp_group_add(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT(
               bcmcth_trunk_vp_group_update(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_group_delete(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMCTH_TRUNK_VP_TBL_ENT_STAGED_SET
        (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
         lt_entry->trunk_vp_id);

    BCMCTH_TRUNK_VP_GRP_TRANS_ID(unit, lt_entry->trunk_vp_id) = op_arg->trans_id;

    /*
     * Check if atomic transactions support is enabled for this BCM unit i.e.
     * feature_conf->dis_atomic_trans == 0 and set staging flags if support is
     * enabled.
     */
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_SET
            (BCMCTH_TRUNK_VP_TBL_PTR(
             unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
             lt_entry->trunk_vp_id);
        /*
         * Save the Group's Trasanction ID for use during commit/abort
         * operation.
         */
        BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, lt_entry->trunk_vp_id) = op_arg->trans_id;
    }

    /*No readonly field is added*/
    if (output_fields) {
        output_fields->count = 0;
    }

exit:
    BCMCTH_TRUNK_VP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP group imm commit call back
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
bcmcth_trunk_vp_group_commit(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_commit: LT_SID=%d.\n"),
              sid));

    BCMCTH_TRUNK_VP_INIT(unit);
    BCMCTH_TRUNK_VP_LOCK(unit);
    for (trunk_vp_id = BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id <= BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id++ ) {
        if (trans_id != BCMCTH_TRUNK_VP_GRP_TRANS_ID(unit, trunk_vp_id))  {
            continue;
        }
        if (BCMCTH_TRUNK_VP_TBL_ENT_STAGED_GET(
                          BCMCTH_TRUNK_VP_TBL_PTR(
                          unit,
                          group,
                          BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                          trunk_vp_id)) {
            BCMCTH_TRUNK_VP_TBL_ENT_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(
                                 unit,
                                 group,
                                 BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                 trunk_vp_id);
        }
        if (BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_GET(
                          BCMCTH_TRUNK_VP_TBL_PTR(
                          unit,
                          group,
                          BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                          trunk_vp_id)) {
            BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(
                                 unit,
                                 group,
                                 BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                 trunk_vp_id);
        }
    }

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit,
              member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (trunk_vp_id = BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id <= BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id++ ) {

        if (((!BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                   unit, group,
                                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                   trunk_vp_id))
           && (!BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                   unit, group,
                                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                   trunk_vp_id)))
           || (trans_id != BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(
               unit, trunk_vp_id))) {
            continue;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: trunk_vp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     trunk_vp_id,
                     trans_id,
                     BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, trunk_vp_id),
                     BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET(
                          BCMCTH_TRUNK_VP_TBL_PTR(
                                          unit,
                                          group,
                                          BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                          trunk_vp_id),
                                          sid));

        BCMCTH_TRUNK_VP_GRP_TYPE_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_LT_SID_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_LB_MODE_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_ITBM_USE_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id);

        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX_BK(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id);

        tbl_size = (size_t)BCMCTH_TRUNK_VP_TBL_SIZE(unit, member,
                                            BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        sal_memcpy(tbl_ptr->ent_bk_arr, tbl_ptr->ent_arr,
                   (sizeof(*tbl_ptr->ent_bk_arr) * tbl_size));

        BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                            group,
                                            BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                            trunk_vp_id);
        BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                            group,
                                            BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                            trunk_vp_id);

    }

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u.\n"),
                     trans_id, sid));
        BCMCTH_TRUNK_VP_UNLOCK(unit);
        SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP transaction abort
 *
 * Rollback the TRUNK_VP entry from backup state
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
bcmcth_trunk_vp_group_abort(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            void *context)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;
    int gtype = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_lth_group_abort: LT_SID=%d.\n"),
              sid));

    BCMCTH_TRUNK_VP_INIT(unit);
    BCMCTH_TRUNK_VP_LOCK(unit);

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit,
              member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (trunk_vp_id = BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id <= BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id++ ) {

        if (((!BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                         unit,
                                         group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                         trunk_vp_id))
            && (!BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                         unit,
                                         group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                         trunk_vp_id)))
            || (trans_id != BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, trunk_vp_id))
            ) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: trunk_vp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     trunk_vp_id,
                     trans_id,
                     BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, trunk_vp_id),
                     BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET
                        (BCMCTH_TRUNK_VP_TBL_PTR(
                         unit,
                         group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                         trunk_vp_id),
                     sid));
        /*
         * Release the itbm resources for all the groups
         * 1. Moved as part of defragmentation.
         * 2. For insert or update operation
         *    release the resources in current transaction
         */
        if (BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id))  {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id),
                     SHR_ITBM_INVALID,
                     BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX
                        (unit, trunk_vp_id)));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                     1,
                     SHR_ITBM_INVALID,
                     trunk_vp_id));

        }
     }
     /* Add all the defragmentation groups itbm resources based on backup state. */
     for (trunk_vp_id = BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id <= BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
         trunk_vp_id++ ) {

        if (((!BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                         unit,
                                         group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                         trunk_vp_id))
            && (!BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_GET(BCMCTH_TRUNK_VP_TBL_PTR(
                                         unit,
                                         group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                                         trunk_vp_id)))
            || (trans_id != BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, trunk_vp_id))
            ) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: trunk_vp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     trunk_vp_id,
                     trans_id,
                     BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, trunk_vp_id),
                     BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET
                        (BCMCTH_TRUNK_VP_TBL_PTR(
                         unit,
                         group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                         trunk_vp_id),
                     sid));
        /*
         * Add the itbm resources for all the groups.
         * 1. Moved as part of defragmentation based on backup state.
         * 2. Insert or update operation is being performed in current transaction,
         *    recover the itbm resources based on backup state.
         * 3. If there is a delete operation in current transaction
         *    recover the itbm rresource based on backup state, since
         *    the transaction has aborted.
         */
        if (BCMCTH_TRUNK_VP_GRP_ITBM_USE_BK(unit, trunk_vp_id))  {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS_BK(unit, trunk_vp_id),
                     SHR_ITBM_INVALID,
                     BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX_BK
                        (unit, trunk_vp_id)));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                     1,
                     SHR_ITBM_INVALID,
                     trunk_vp_id));

        }
        BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_TYPE_BK(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_LT_SID_BK(unit,
                                      trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_LB_MODE_BK(unit,
                                      trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS_BK(unit,
                                           trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT_BK(unit,
                                          trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_ITBM_USE_BK(unit, trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX_BK(unit, trunk_vp_id);
        tbl_size = (size_t)BCMCTH_TRUNK_VP_TBL_SIZE(unit, member,
                                            BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        sal_memcpy(tbl_ptr->ent_arr, tbl_ptr->ent_bk_arr,
                   (sizeof(*tbl_ptr->ent_arr) * tbl_size));

        BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(
                               unit, group,
                               BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                               trunk_vp_id);
        BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(BCMCTH_TRUNK_VP_TBL_PTR(
                               unit, group,
                               BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                               trunk_vp_id);
    }

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u error=%d.\n"),
                     trans_id, sid, _func_rv));
        BCMCTH_TRUNK_VP_UNLOCK(unit);
        return;

}

/*!
 * \brief TRUNK_VP In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to TRUNK_VP logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = bcmcth_trunk_vp_group_validate,

    /*! Staging function. */
    .op_stage = bcmcth_trunk_vp_group_stage,

    /*! Commit function. */
    .commit = bcmcth_trunk_vp_group_commit,

    /*! Abort function. */
    .abort = bcmcth_trunk_vp_group_abort
};

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief TRUNK_VP IMM callback register.
 *
 * regist TRUNK_VP LT callback in IMM module.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid  Logic table ID.
 *
 * \retval None.
 */
int
bcmcth_trunk_vp_overlay_group_imm_register(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TRUNK_VPt here.
     */
    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_lt_event_reg(unit,
                                 sid,
                                 &event_hdl,
                                 NULL));
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                 "Registered IMM event handler for TRUNK_VP SID:%u.\n"),
                 sid));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate itbm group resources on warmboot
 *        initalization of TRUNK_VP subcomponent.
 *
 * \param [in] unit unit number.

 * Allocate itbm group used by the TRUNK_VP component
 * calling the itbm resource manager.
 *
 * \retval SHR_E_NONE on success.
 *         !SHR_E_NONE on failure.
 */
int
bcmcth_trunk_vp_wb_itbm_group_alloc(int unit)
{
    int trunk_vp_id, gstart_idx, gend_idx;
    int gtype = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the TRUNK_VP driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    vp_drv_var = drv_var->trunk_vp_var;
    SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);

    gstart_idx = BCMCTH_TRUNK_VP_TBL_IDX_MIN(unit, group, gtype);

    gend_idx = gstart_idx + BCMCTH_TRUNK_VP_TBL_SIZE(unit, group, gtype) - 1;

    for (trunk_vp_id = gstart_idx; trunk_vp_id <= gend_idx; trunk_vp_id++) {
         /* check if the HA memory has the group configured. */
         if (BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id)) {
             gtype = BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id);
         } else {
             continue;
         }

         mstart_idx =
              BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id);
         max_members = BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);
         rv = shr_itbm_list_block_alloc_id
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                 1,
                 SHR_ITBM_INVALID,
                 trunk_vp_id);
         if (rv) {
             LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n TRUNK_VP warmboot recovery for itbm list failed."\
                "Unable to allocare the itbm block for Group_id %d.\n")
                , trunk_vp_id));
              SHR_ERR_EXIT(SHR_E_INTERNAL);
         }
         rv = shr_itbm_list_block_alloc_id
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                 max_members,
                 SHR_ITBM_INVALID,
                 mstart_idx);
         if (rv) {
             LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n TRUNK_VP warmboot recovery for itbm member list failed."\
                "Unable to allocare the itbm block for Group_id %d.\n")
                , trunk_vp_id));
              SHR_ERR_EXIT(SHR_E_INTERNAL);
         }
    }
    exit:
        SHR_FUNC_EXIT();
}


