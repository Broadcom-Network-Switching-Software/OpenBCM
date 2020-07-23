/*! \file bcmecmp_lvl1_group_imm.c
 *
 * BCMECMP ECMP_LEVEL1t IMM handler.
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
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmecmp/bcmecmp_imm.h>


/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */
/*!
 * \brief ECMP_LEVEL1 LT entry fields parse routine.
 *
 * Parse ECMP_LEVEL1 logical table entry logical fields
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
bcmecmp_lvl1_group_lt_fields_parse(int unit,
                                   const bcmltd_field_t *in,
                                   bcmltd_sid_t sid,
                                   bcmecmp_flex_lt_entry_t *lt_entry)
{
    const bcmltd_field_t *gen_field;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(
                unit,
                "bcmecmp_lvl1_group_lt_fields_parse:LT_SID=%d.\n"),
                sid));

    gen_field = in;
    while (gen_field) {
        SHR_IF_ERR_EXIT
          (BCMECMP_FNCALL_EXEC(unit,
           grp_ul_lt_fields_fill)(unit, gen_field, lt_entry));
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
bcmecmp_lvl1_group_validate(int unit,
                            bcmltd_sid_t sid,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_flex_lt_entry_t *lt_entry = NULL; /* ECMP_LEVEL1 LT entry data. */
    bcmecmp_flex_lt_entry_t *cur_lt_entry = NULL; /* current LT entry.*/

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lvl1_group_validate:SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
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
    lt_entry = BCMECMP_FLEX_LT_ENT_PTR(unit);

    /* Initialize Logical Table Entry buffer. */
    SHR_IF_ERR_EXIT
        (bcmecmp_flex_lt_entry_t_init(unit, sid, lt_entry));

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_EXIT
        (bcmecmp_lvl1_group_lt_fields_parse(unit, key, sid, lt_entry));

    if (((event == BCMIMM_ENTRY_UPDATE) ||
        (event == BCMIMM_ENTRY_INSERT)) && (data != NULL)) {
        SHR_IF_ERR_EXIT
        (bcmecmp_lvl1_group_lt_fields_parse(unit, data, sid, lt_entry));
    }

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Get logical table entry buffer pointer. */
        cur_lt_entry = BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit);

        /* Initialize Logical Table Entry buffer. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_lt_entry_t_init(unit, sid, cur_lt_entry));

        cur_lt_entry->event = event;

        /* Parse the input fields and retrieve the field values. */
        SHR_IF_ERR_EXIT
            (bcmecmp_lvl1_group_lt_fields_parse(unit,
                                                 key,
                                                 sid,
                                                 cur_lt_entry));
        /* Get current fields */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_lt_fields_lookup(unit,
                                           cur_lt_entry,
                                           BCMECMP_FNCALL(unit,
                                           grp_ul_lt_fields_fill)));
    }

    /* Validate input parameter values. */
    SHR_IF_ERR_EXIT
        (bcmecmp_flex_lt_input_params_validate(unit, event, sid, lt_entry));

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
bcmecmp_lvl1_group_stage(int unit,
                         bcmltd_sid_t sid,
                         const bcmltd_op_arg_t *op_arg,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{

    bcmecmp_flex_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */
    uint32_t hw_ecmp_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lvl1_group_stage: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

        /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_FLEX_LT_ENT_PTR(unit);
    lt_entry->op_arg = op_arg;

    hw_ecmp_id = lt_entry->ecmp_id -
                 BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_FLEX_TBL_ENT_STAGED_SET
        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1),
         hw_ecmp_id);

    BCMECMP_FLEX_GRP_TRANS_ID(unit, lt_entry->ecmp_id) = op_arg->trans_id;

    /*
     * Check if atomic transactions support is enabled for this BCM unit i.e.
     * feature_conf->dis_atomic_trans == 0 and set staging flags if support is
     * enabled.
     */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_FLEX_TBL_BK_ENT_STAGED_SET
            (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1),
             hw_ecmp_id);
        /*
         * Save the Group's Trasanction ID for use during commit/abort
         * operation.
         */
        BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = op_arg->trans_id;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT(bcmecmp_flex_group_add(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT(bcmecmp_flex_group_update(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_delete(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, group,
                              BCMECMP_GRP_TYPE_LEVEL1)))) {
        BCMECMP_FLEX_TBL_ENT_STAGED_SET
            (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1), hw_ecmp_id);

        BCMECMP_FLEX_GRP_TRANS_ID(unit, lt_entry->ecmp_id) = op_arg->trans_id;
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
 * \brief ECMP_LEVEL1 group imm commit call back
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
bcmecmp_lvl1_group_commit(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    uint32_t global_ecmp_id;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lvl1_group_commit: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id++ ) {

        global_ecmp_id = ecmp_id +
                         BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        if (((!BCMECMP_FLEX_TBL_ENT_STAGED_GET(
                        BCMECMP_TBL_PTR(unit,
                                        group,
                                        BCMECMP_GRP_TYPE_LEVEL1),
                                        ecmp_id)) &&
            (!BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_GET(
                        BCMECMP_TBL_PTR(unit,
                                        group,
                                        BCMECMP_GRP_TYPE_LEVEL1),
                                        ecmp_id))) ||
            (trans_id != BCMECMP_FLEX_GRP_TRANS_ID(unit, global_ecmp_id))) {
            continue;
        }
        if (BCMECMP_FLEX_TBL_ENT_STAGED_GET(
                          BCMECMP_TBL_PTR(
                          unit,
                          group,
                          BCMECMP_GRP_TYPE_LEVEL1),
                          ecmp_id)) {
            BCMECMP_FLEX_TBL_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(
                                 unit,
                                 group,
                                 BCMECMP_GRP_TYPE_LEVEL1),
                                 ecmp_id);
        }
        if (BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_GET(
                          BCMECMP_TBL_PTR(
                          unit,
                          group,
                          BCMECMP_GRP_TYPE_LEVEL1),
                          ecmp_id)) {
            BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_CLEAR(BCMECMP_TBL_PTR(
                                 unit,
                                 group,
                                 BCMECMP_GRP_TYPE_LEVEL1),
                                 ecmp_id);
        }
        BCMECMP_FLEX_TBL_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(
                                          unit,
                                          group,
                                          BCMECMP_GRP_TYPE_LEVEL1),
                                          ecmp_id);
    }

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT not valid for ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id++ ) {

        global_ecmp_id = ecmp_id +
                         BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        if (((!BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                  BCMECMP_GRP_TYPE_LEVEL1),
                                                  ecmp_id)) &&
            (!BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL1),
                                                 ecmp_id)))
            || (trans_id !=
                BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id))) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id),
                     BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(
                                                     BCMECMP_TBL_PTR(unit,
                                                     group,
                                                     BCMECMP_GRP_TYPE_LEVEL1),
                                                     ecmp_id),
                                                     sid));
        BCMECMP_FLEX_GRP_TYPE_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_TYPE(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_LT_SID_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_LT_SID(unit,
                                            global_ecmp_id);
        BCMECMP_FLEX_GRP_LB_MODE_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_LB_MODE(unit,
                                             global_ecmp_id);
        BCMECMP_FLEX_GRP_MAX_PATHS_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_MAX_PATHS(unit,
                                               global_ecmp_id);
        BCMECMP_FLEX_GRP_NUM_PATHS_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_NUM_PATHS(unit,
                                               global_ecmp_id);

        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, global_ecmp_id);

       BCMECMP_FLEX_GRP_ITBM_USE_BK(unit, global_ecmp_id) =
                    BCMECMP_FLEX_GRP_ITBM_USE(unit, global_ecmp_id);

        tbl_size = (size_t)BCMECMP_TBL_SIZE(unit,
                                            member,
                                            BCMECMP_GRP_TYPE_LEVEL1)
                  + BCMECMP_TBL_SIZE(unit, member,
                                               BCMECMP_GRP_TYPE_LEVEL0);
        sal_memcpy(tbl_ptr->ent_bk_arr, tbl_ptr->ent_arr,
                   (sizeof(bcmecmp_flex_hw_entry_attr_t) * tbl_size));

        BCMECMP_FLEX_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                             BCMECMP_GRP_TYPE_LEVEL1),
                                             ecmp_id);
        BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(BCMECMP_TBL_PTR(unit,
                                            group,
                                            BCMECMP_GRP_TYPE_LEVEL1),
                                            ecmp_id);
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
 * \brief ECMP_LEVEL1 LT transaction abort
 *
 * Rollback the ECMP_LEVEL1t entry from backup state
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
bcmecmp_lvl1_group_abort(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;
    uint32_t global_ecmp_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lvl1_group_abort: LT_SID=%d.\n"),
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

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT not valid for ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id++ ) {
        global_ecmp_id = ecmp_id +
                         BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        if (((!BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(
                                BCMECMP_TBL_PTR(unit,
                                                group,
                                                BCMECMP_GRP_TYPE_LEVEL1),
                                                ecmp_id)) &&
            (!BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_GET(
                                BCMECMP_TBL_PTR(unit,
                                                group,
                                                BCMECMP_GRP_TYPE_LEVEL1),
                                                ecmp_id)))
            || (trans_id != BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id))
            ) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id),
                     BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET
                        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1),
                         ecmp_id),
                     sid));
       /*
         * Release the itbm resources for all the groups
         * 1. Moved as part of defragmentation.
         * 2. For insert or update operation
         *    release the resources in current transaction
         */
        if (BCMECMP_FLEX_GRP_ITBM_USE(unit, global_ecmp_id))  {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1)),
                    BCMECMP_FLEX_GRP_MAX_PATHS(unit, global_ecmp_id),
                    SHR_ITBM_INVALID,
                    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, global_ecmp_id)));
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1)),
                    1,
                    SHR_ITBM_INVALID,
                    global_ecmp_id));

        }
     }
     /*
      *  Add all the defragmentation groups itbm
      *  resources based on backup state.
      */
     for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL1);
         ecmp_id++ ) {
        global_ecmp_id = ecmp_id +
                         BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);

        if (((!BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(
                                BCMECMP_TBL_PTR(unit,
                                                group,
                                                BCMECMP_GRP_TYPE_LEVEL1),
                                                ecmp_id)) &&
            (!BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_GET(
                                BCMECMP_TBL_PTR(unit,
                                                group,
                                                BCMECMP_GRP_TYPE_LEVEL1),
                                                ecmp_id)))
            || (trans_id != BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id)))
        {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, global_ecmp_id),
                     BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET
                        (BCMECMP_TBL_PTR(
                         unit,
                         group, BCMECMP_GRP_TYPE_LEVEL1),
                         ecmp_id),
                     sid));
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
        if (BCMECMP_FLEX_GRP_ITBM_USE_BK(unit, global_ecmp_id))  {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_alloc_id(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1)),
                    BCMECMP_FLEX_GRP_MAX_PATHS_BK(unit, global_ecmp_id),
                    SHR_ITBM_INVALID,
                    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX_BK(unit,
                                                           global_ecmp_id)));
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_alloc_id(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1)),
                    1,
                    SHR_ITBM_INVALID,
                    global_ecmp_id));

        }
        BCMECMP_FLEX_GRP_TYPE(unit, global_ecmp_id) =
              BCMECMP_FLEX_GRP_TYPE_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_LT_SID(unit, global_ecmp_id) =
              BCMECMP_FLEX_GRP_LT_SID_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_LB_MODE(unit, global_ecmp_id) =
              BCMECMP_FLEX_GRP_LB_MODE_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_MAX_PATHS(unit, global_ecmp_id) =
              BCMECMP_FLEX_GRP_MAX_PATHS_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_NUM_PATHS(unit, global_ecmp_id) =
              BCMECMP_FLEX_GRP_NUM_PATHS_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_ITBM_USE(unit, global_ecmp_id) =
        BCMECMP_FLEX_GRP_ITBM_USE_BK(unit, global_ecmp_id);
        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, global_ecmp_id) =
        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX_BK(unit, global_ecmp_id);
        tbl_size = (size_t)BCMECMP_TBL_SIZE(unit, member,
                                            BCMECMP_GRP_TYPE_LEVEL1)
                        + BCMECMP_TBL_SIZE(unit, member,
                                               BCMECMP_GRP_TYPE_LEVEL0);
        sal_memcpy(tbl_ptr->ent_arr, tbl_ptr->ent_bk_arr,
                   (sizeof(bcmecmp_flex_hw_entry_attr_t) * tbl_size));
        BCMECMP_FLEX_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_LEVEL1),
                                            ecmp_id);
        BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(BCMECMP_TBL_PTR(unit,
                                            group,
                                            BCMECMP_GRP_TYPE_LEVEL1),
                                            ecmp_id);
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
 * \brief ECMP_UNDERLAY In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to ECMP_UNDERLAY logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = bcmecmp_lvl1_group_validate,

    /*! Extended staging function. */
    .op_stage = bcmecmp_lvl1_group_stage,

    /*! Commit function. */
    .commit = bcmecmp_lvl1_group_commit,

    /*! Abort function. */
    .abort = bcmecmp_lvl1_group_abort
};

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief ECMP_UNDERLAY IMM callback register
 *
 * regist ECMP_UNDERLAY LT callback in IMM module
 *
 * \param [in] unit This is device unit number.
 *
 * \retval None.
 */
int
bcmecmp_lvl1_group_imm_register(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for ECMP_LEVEL1t here.
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
