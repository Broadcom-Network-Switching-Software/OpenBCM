/*! \file bcmecmp_overlay_group_imm.c
 *
 * BCMECMP ECMP_OVERLAYt IMM handler.
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
bcmecmp_overlay_group_lt_fields_parse(int unit,
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
          (BCMECMP_FNCALL_EXEC(unit, grp_ol_lt_fields_fill)(unit, gen_field, lt_entry));
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
bcmecmp_overlay_group_validate(int unit,
                               bcmltd_sid_t sid,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data. */
    bcmecmp_lt_entry_t *cur_lt_entry = NULL; /* current ECMP LT entry data. */
    uint32_t lb_mode_update = BCMECMP_LB_MODE_COUNT;
    uint32_t i = 0;
    bool nhop_id_reset = true;
    bool uecmp_id_reset = true;
    bool o_nhop_id_reset = true;
    bool u_ecmp_nhop = true;
    bcmecmp_lt_field_attrs_t * lt_fattr = NULL;
    bcmecmp_control_t * ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_group_validate:SID=%d.\n"),
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
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Initialize Logical Table Entry buffer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_entry_t_init(unit, sid, lt_entry));
    lt_entry->event = event;

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_overlay_group_lt_fields_parse(unit, key, sid, lt_entry));

    if (((event == BCMIMM_ENTRY_UPDATE) ||
        (event == BCMIMM_ENTRY_INSERT)) && (data != NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_overlay_group_lt_fields_parse(unit, data, sid, lt_entry));
    }

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Get logical table entry buffer pointer. */
        cur_lt_entry = BCMECMP_CURRENT_LT_ENT_PTR(unit);

        /* Initialize Logical Table Entry buffer. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_entry_t_init(unit, sid, cur_lt_entry));
        cur_lt_entry->event = event;

        /* Parse the input fields and retrieve the field values. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_overlay_group_lt_fields_parse(unit,
                                                   key,
                                                   sid,
                                                   cur_lt_entry));

        /* Get current fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_fields_lookup(unit,
                                      cur_lt_entry,
                                      BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill)));

        /*
         * verify releted fieds are reset before updating LB mode between
         * RH and NON-RH
         */
        if (BCMECMP_LT_FIELD_GET
                          (lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)) {
            if (lt_entry->lb_mode !=
                           BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) &&
               (lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT ||
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                        BCMECMP_LB_MODE_RESILIENT)) {
                lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl->ecmp_info.group[lt_entry->grp_type].lt_fattrs;
                for (i = 0;
                     i < BCMECMP_GRP_MAX_PATHS(unit, lt_entry->ecmp_id);
                     i++) {
                    if (cur_lt_entry->nhop_id[i] != 0 &&
                        cur_lt_entry->nhop_id[i] != BCMECMP_INVALID){
                        nhop_id_reset = false;
                        break;
                    }
                    if (cur_lt_entry->uecmp_id[i] != BCMECMP_LT_MIN_UECMP_ID
                                            (unit, group, lt_entry->grp_type) &&
                        cur_lt_entry->uecmp_id[i] != BCMECMP_INVALID){
                        uecmp_id_reset = false;
                        break;
                    }
                    if (bcmecmp_grp_is_overlay(lt_entry->grp_type) &&
                        ecmp_ctrl->ecmp_info.ovlay_memb_count) {
                        if (cur_lt_entry->o_nhop_id[i] !=
                                               lt_fattr->min_o_nhop_id &&
                            cur_lt_entry->o_nhop_id[i] != BCMECMP_INVALID){
                            o_nhop_id_reset = false;
                            break;
                        }
                        if (cur_lt_entry->u_ecmp_nhop[i] != 0 &&
                            cur_lt_entry->u_ecmp_nhop[i] != (uint8_t)BCMECMP_INVALID) {
                            u_ecmp_nhop = false;
                            break;
                        }
                    }
                }

                if (BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                        BCMECMP_LB_MODE_RESILIENT &&
                    (cur_lt_entry->rh_size_enc != BCMECMP_LT_MIN_RH_SIZE
                                            (unit, group, lt_entry->grp_type) ||
                     cur_lt_entry->rh_num_paths != 0 ||
                     !nhop_id_reset ||
                     !uecmp_id_reset ||
                     !o_nhop_id_reset ||
                     !u_ecmp_nhop)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RH fields are not reset.\n")));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }

                if (BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                        BCMECMP_LB_MODE_RESILIENT &&
                    (cur_lt_entry->max_paths != BCMECMP_LT_MAX_PATHS
                                            (unit, group, lt_entry->grp_type)||
                     cur_lt_entry->num_paths != 0 ||
                     cur_lt_entry->nhop_sorted != 0 ||
                     !nhop_id_reset ||
                     !uecmp_id_reset ||
                     !o_nhop_id_reset ||
                     !u_ecmp_nhop)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "NON RH fields aren't reset.\n")));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            }
        }

         /*
         * For Update operation if ECMP_NHOP is not set by user, use previously
         * stored value.
         */
        if (!(bcmecmp_grp_is_overlay(lt_entry->grp_type) &&
                        ecmp_ctrl->ecmp_info.ovlay_memb_count)) {
            if (!BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                && BCMECMP_TBL_REF_CNT
                        (BCMECMP_TBL_PTR(unit, group, lt_entry->grp_type),
                                         lt_entry->ecmp_id)
                && (BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id) == sid)) {
                lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit,
                                                            lt_entry->ecmp_id);
            }
        }
    }

    /*
     * ECMP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied or invalid.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        || (lt_entry->ecmp_id <  BCMECMP_LT_MIN_ECMP_ID
                                    (unit, group, BCMECMP_GRP_TYPE_OVERLAY))
        || (lt_entry->ecmp_id > BCMECMP_LT_MAX_ECMP_ID
                                    (unit, group, BCMECMP_GRP_TYPE_OVERLAY))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID field value error.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE) &&
          event == BCMIMM_ENTRY_UPDATE) {
        lb_mode_update = lt_entry->lb_mode;
    } else {
        lb_mode_update = BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id);
    }

    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID) ||
           BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID) ||
         BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID)||
         (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP) &&
          bcmecmp_grp_is_overlay(lt_entry->grp_type) &&
          ecmp_ctrl->ecmp_info.ovlay_memb_count)) {
        if (((event == BCMIMM_ENTRY_INSERT) &&
            (lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT)) ||
            ((event == BCMIMM_ENTRY_UPDATE) &&
            (lb_mode_update == BCMECMP_LB_MODE_RESILIENT))) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NHOP_ID or ECMP_UNDERLAY_ID or "
                                    "NHOP_OVERLAY_ID is set, LB_MODE is RH.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NHOP_ID) ||
          BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_UECMP_ID) ||
          BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_O_NHOP_ID)||
          BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_ECMP_NHOP)) {
        if (((event == BCMIMM_ENTRY_INSERT) &&
            (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)) ||
            ((event == BCMIMM_ENTRY_UPDATE) &&
            (lb_mode_update != BCMECMP_LB_MODE_RESILIENT))) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RH_NHOP_ID or RH_ECMP_UNDERLAY_ID is set, "
                                    "LB_MODE is not RH.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_RH_NHOP_ID)) {
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            } else if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                            BCMECMP_LT_FIELD_RH_UECMP_ID)){
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID);
            } else if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                            BCMECMP_LT_FIELD_RH_O_NHOP_ID)){
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID);
            } else {
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP);
            }
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
bcmecmp_overlay_group_stage(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_op_arg_t *op_arg,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{

    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */
    bcmecmp_id_t ecmp_id;
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_overlay_group_stage: LT_SID=%d.\n"),
              sid));
    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);
    lt_entry->op_arg = op_arg;
    ecmp_id = lt_entry->ecmp_id;
    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_add(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            if (((BCMECMP_LT_FIELD_GET
                          (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED) &&
               lt_entry->rh_rand_seed !=
                          BCMECMP_INIT_RH_RAND_SEED(unit, lt_entry->ecmp_id)) ||
               (BCMECMP_LT_FIELD_GET
                          (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE) &&
               lt_entry->rh_size_enc!=
                          BCMECMP_GRP_RH_SIZE(unit, lt_entry->ecmp_id))) &&
               !BCMECMP_LT_FIELD_GET
                         (lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_group_update_entry_set(unit, lt_entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_group_delete(unit, lt_entry));
                /* Delete commit. */
                if (BCMECMP_ATOMIC_TRANS(unit)) {
                    if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE
                            (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
                        if (ecmp_info->ovlay_memb_count) {
                            BCMECMP_FREE
                                (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr);
                            BCMECMP_FREE
                                (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr);
                        }
                        BCMECMP_FREE
                            (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                        BCMECMP_FREE
                            (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                        BCMECMP_FREE
                            (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                    }
                }
                SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_add(unit, lt_entry));
                break;
            }

            if (BCMECMP_LT_FIELD_GET
                          (lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)) {
                if (lt_entry->lb_mode !=
                               BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) &&
                   (lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT ||
                    BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                            BCMECMP_LB_MODE_RESILIENT)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_group_update_entry_set(unit, lt_entry));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_group_delete(unit, lt_entry));
                    /* Delete commit. */
                    if (BCMECMP_ATOMIC_TRANS(unit)) {
                        if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                            BCMECMP_FREE
                                (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
                            if (ecmp_info->ovlay_memb_count) {
                                BCMECMP_FREE
                                    (ecmp_info->rhg_bk[ecmp_id]
                                                            ->rh_o_entries_arr);
                                BCMECMP_FREE
                                    (ecmp_info->rhg_bk[ecmp_id]
                                                          ->rh_u_ecmp_nhop_arr);
                            }
                            BCMECMP_FREE
                                (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                            BCMECMP_FREE
                                (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                            BCMECMP_FREE
                                (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                        }
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_group_add(unit, lt_entry));
                    break;
                }
            }

            SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_update(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_group_delete(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMECMP_TBL_ENT_STAGED_SET
        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY),
         lt_entry->ecmp_id);
    BCMECMP_GRP_TRANS_ID(unit, lt_entry->ecmp_id) = op_arg->trans_id;
    /*
     * Check if atomic transactions support is enabled for this BCM unit i.e.
     * feature_conf->dis_atomic_trans == 0 and set staging flags if support is
     * enabled.
     */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_TBL_BK_ENT_STAGED_SET
            (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY),
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
bcmecmp_overlay_group_commit(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;
    bcmecmp_lt_entry_t *cur_entry = NULL;
    int *mem_array = NULL;
    int *cur_mem_arry =  0;
    uint32_t i = 0;
    bcmecmp_info_t *ecmp_info = NULL;
    shr_itbm_list_hdl_t grp_list_hdl;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    bcmecmp_grp_attr_t *grp_bk_attr, *grp_bk_attr_base;
    bcmecmp_hw_entry_attr_t *hw_ent_attr, *hw_ent_attr_base;
    bool in_use, in_use_bk;
    uint32_t comp_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_overlay_group_commit: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_list_hdl = ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].list_hdl;
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    grp_bk_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr);
    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                           (ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].ent_arr);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id++ ) {
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                        BCMECMP_GRP_TYPE_OVERLAY),
                                        ecmp_id) &&
             !(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED)) ||
           (trans_id != BCMECMP_GRP_TRANS_ID(unit, ecmp_id))) {
            continue;
        }

        if (hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED) {
            hw_ent_attr->flags &= (~BCMECMP_ENTRY_DEFRAG_STAGED);
        }

        if (BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) &&
            (sid == BCMECMP_GRP_LT_SID(unit, ecmp_id))) {
            if (!cur_entry) {
                /* Allocate memory to store current LT Entry information. */
                BCMECMP_ALLOC
                    (cur_entry, sizeof(bcmecmp_lt_entry_t),
                     "bcmecmpGroupCommitCurrentLtEntInfo");
            }

            SHR_IF_ERR_VERBOSE_EXIT
                   (bcmecmp_lt_entry_t_init(unit,
                                            BCMECMP_GRP_LT_SID(unit, ecmp_id),
                                            cur_entry));
            cur_entry->ecmp_id = ecmp_id;
            cur_entry->event = BCMIMM_ENTRY_UPDATE;

            /* Get current fields */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_lt_fields_lookup(unit,
                                      cur_entry,
                                      BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill)));
            if (cur_entry->num_paths) {
                if (cur_entry->ecmp_nhop) {
                    cur_mem_arry = cur_entry->uecmp_id;
                } else {
                    cur_mem_arry = cur_entry->nhop_id;
                }
                /* Initialize NHOP_IDs array elements to default */
                for (i=0 ; i < cur_entry->num_paths; i++) {
                    if (cur_mem_arry[i] == BCMECMP_INVALID) {
                        cur_mem_arry[i] = cur_entry->ecmp_nhop ?
                            BCMECMP_LT_MIN_UECMP_ID(unit, group,
                                                    BCMECMP_GRP_TYPE_OVERLAY) :
                            BCMECMP_LT_MIN_NHOP_ID(unit, group,
                                                   BCMECMP_GRP_TYPE_OVERLAY);
                    }
                }

                if (!mem_array) {
                    /* Allocate memory to store current LT Entry information. */
                    BCMECMP_ALLOC
                        (mem_array, BCMECMP_NHOP_ARRAY_SIZE * sizeof(int),
                         "bcmecmpGroupCommitMemArrayInfo");
                }
                sal_memcpy(mem_array, cur_mem_arry,
                           cur_entry->num_paths * sizeof(int));

                SHR_IF_ERR_EXIT
                        (bcmecmp_group_nhops_sort(unit, cur_entry));

                if (sal_memcmp(mem_array, cur_mem_arry,
                               cur_entry->num_paths * sizeof(int))) {
                     LOG_VERBOSE(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "update mems of group %d.\n"),
                                 cur_entry->ecmp_id));
                     SHR_IF_ERR_EXIT
                            (bcmecmp_lt_member_update(unit, cur_entry));
                }
            }
        }

        BCMECMP_TBL_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                                     BCMECMP_GRP_TYPE_OVERLAY),
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
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_OVERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                          (ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].ent_bk_arr);
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        grp_bk_attr = grp_bk_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                    BCMECMP_GRP_TYPE_OVERLAY),
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
                                                     BCMECMP_GRP_TYPE_OVERLAY),
                                                     ecmp_id),
                     sid));
        SHR_IF_ERR_EXIT
            (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0,
                                BCMECMP_GRP_TYPE_OVERLAY, &in_use, &comp_id));
        SHR_IF_ERR_EXIT
            (bcmecmp_grp_in_use_bk(unit, ecmp_id, FALSE, 0,
                                   BCMECMP_GRP_TYPE_OVERLAY, &in_use_bk,
                                   &comp_id));
        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
            && in_use) {
            /* Insert/Update commit. */
            sal_memcpy(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                       BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                       (sizeof(*BCMECMP_GRP_RH_ENTRIES_PTR(unit,
                               ecmp_id)) *
                               BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id)));
            if (ecmp_info->ovlay_memb_count) {
                /* Overlay member*/
                sal_memcpy(ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
                           ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                           grp_attr->rh_entries_cnt * sizeof(int));
                sal_memcpy(ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
                           ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                           grp_attr->rh_entries_cnt * sizeof(bool));
            }
            sal_memcpy(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                       BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                       (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit,
                               ecmp_id)) *
                               BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id)));
            sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                       BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                       (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                               ecmp_id)) *
                               BCMECMP_GRP_MAX_PATHS(unit, ecmp_id)));
        }

        if (BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
            && BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_REGULAR
            && (!in_use)
            && in_use_bk) {
            /* Delete commit. */
            if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
                if (ecmp_info->ovlay_memb_count) {
                    BCMECMP_FREE
                        (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr);
                    BCMECMP_FREE
                        (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr);
                }
                BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
            }
        }

        BCMECMP_GRP_TYPE_BK(unit, ecmp_id) = BCMECMP_GRP_TYPE(unit, ecmp_id);
        BCMECMP_GRP_LT_SID_BK(unit, ecmp_id) = BCMECMP_GRP_LT_SID(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id) = BCMECMP_GRP_LB_MODE(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NHOP_SORTED_BK(unit, ecmp_id) = BCMECMP_GRP_NHOP_SORTED
                                                            (unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_NUM_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id) =
                        BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);

        BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id) =
                        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
        grp_bk_attr->in_use_itbm = grp_attr->in_use_itbm;
        if (!grp_list_hdl) {
            tbl_size = (size_t)BCMECMP_TBL_SIZE(unit, member,
                                                BCMECMP_GRP_TYPE_OVERLAY)
                                           + BCMECMP_TBL_SIZE(unit, member,
                                                   BCMECMP_GRP_TYPE_UNDERLAY);
            sal_memcpy(tbl_ptr->ent_bk_arr, tbl_ptr->ent_arr,
                       (sizeof(bcmecmp_hw_entry_attr_t) * tbl_size));

            BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                        BCMECMP_GRP_TYPE_OVERLAY), ecmp_id)
                = BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                      BCMECMP_GRP_TYPE_OVERLAY),
                                                      ecmp_id);
        }
        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT) {
            BCMECMP_RH_RAND_SEED_BK(unit, ecmp_id) =
                                            BCMECMP_RH_RAND_SEED(unit, ecmp_id);
        }

        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_OVERLAY),
                                            ecmp_id);
    }

exit:
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "exit: trans_id=%u sid=%u.\n"),
                 trans_id, sid));
    /* Free memory to store current LT Entry information. */
    BCMECMP_FREE(cur_entry);
    /* Free memory to store member array information. */
    BCMECMP_FREE(mem_array);
    BCMECMP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmecmp_overlay_group_rh_revert(int unit,
                                bcmecmp_id_t ecmp_id,
                                bcmecmp_grp_attr_t *grp_bk_attr)
{
    bool in_use, in_use_bk;
    uint32_t comp_id;
    bcmecmp_info_t *ecmp_info;

    SHR_FUNC_ENTER(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0,
                            BCMECMP_GRP_TYPE_OVERLAY, &in_use, &comp_id));
    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use_bk(unit, ecmp_id, FALSE, 0,
                               BCMECMP_GRP_TYPE_OVERLAY, &in_use_bk,
                               &comp_id));

    /* Revert RH-ECMP group delete. */
    if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_REGULAR
        && BCMECMP_GRP_LB_MODE_BK(unit,
                                  ecmp_id) == BCMECMP_LB_MODE_RESILIENT
        && in_use_bk
        && (!in_use)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                      sizeof(*BCMECMP_GRP_RHG_PTR(unit, ecmp_id)),
                      "bcmecmpOlayRhGrpAbortRhgInfo");
        BCMECMP_ALLOC
            (BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
             BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id) *
             sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))),
             "bcmecmpOlayRhGrpAbortGrpRhEntArr");
        if (ecmp_info->ovlay_memb_count) {
            /* Allocate memory to store RH-ECMP group entries. */
            BCMECMP_ALLOC
                (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                 grp_bk_attr->rh_entries_cnt * sizeof(int),
                 "bcmecmpOlayRhGrpAddGrpRhOvlayEntArr");
            BCMECMP_ALLOC
                (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                 grp_bk_attr->rh_entries_cnt * sizeof(bool),
                 "bcmecmpOlayRhGrpAddGrpRhUnlayEntArr");
        }
        BCMECMP_ALLOC
            (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
             BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id) *
             sizeof(*(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id))),
             "bcmecmpOlayRhGrpAbortGrpRhMindexArr");
        BCMECMP_ALLOC
            (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
             BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id) *
             sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                     ecmp_id))),
             "bcmecmpOlayRhGrpAbortGrpRhMembEntCntArr");

        SHR_NULL_CHECK(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                                                     SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                          ecmp_id)) *
                    BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id)));
        if (ecmp_info->ovlay_memb_count) {
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                       ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
                       grp_bk_attr->rh_entries_cnt * sizeof(int));
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                       ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
                       grp_bk_attr->rh_entries_cnt * sizeof(bool));
        }
        SHR_NULL_CHECK(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                                                    SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                         ecmp_id)) *
                    BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id)));

        SHR_NULL_CHECK(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                                                         SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                              ecmp_id)) *
                    BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id)));
    }

    /* Revert RH-ECMP group insert. */
    if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
        && BCMECMP_GRP_LB_MODE_BK(unit,
                                  ecmp_id) == BCMECMP_LB_MODE_REGULAR
        && (!in_use_bk)
        && in_use) {
        SHR_NULL_CHECK(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                                           SHR_E_INTERNAL);
        if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
            BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
            if (ecmp_info->ovlay_memb_count) {
                BCMECMP_FREE
                    (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr);
                BCMECMP_FREE
                    (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr);
            }
            BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
            BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));
            BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
        }

        SHR_NULL_CHECK(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id),
                                              SHR_E_INTERNAL);
        if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
            BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
            if (ecmp_info->ovlay_memb_count) {
                BCMECMP_FREE
                    (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr);
                BCMECMP_FREE
                    (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr);
            }
            BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
            BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
            BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
        }
    }

    /* Revert RH-ECMP group update. */
    if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
        && in_use_bk
        && in_use) {
        SHR_NULL_CHECK(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                                                  SHR_E_INTERNAL);
        SHR_NULL_CHECK(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                                                     SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                          ecmp_id)) *
                    BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id)));
        if (ecmp_info->ovlay_memb_count) {
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                       ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
                       grp_bk_attr->rh_entries_cnt * sizeof(int));
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                       ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
                       grp_bk_attr->rh_entries_cnt * sizeof(bool));
        }

        SHR_NULL_CHECK(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                                                 SHR_E_INTERNAL);
        SHR_NULL_CHECK(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                                                    SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                         ecmp_id)) *
                    BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id)));

        SHR_NULL_CHECK(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                                                      SHR_E_INTERNAL);
        SHR_NULL_CHECK(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                                                         SHR_E_INTERNAL);
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                              ecmp_id)) *
                    BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id)));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP transaction abort
 *
 * Rollback the entry from backup state
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
bcmecmp_overlay_group_abort(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            void *context)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t grp_list_hdl;
    shr_itbm_list_hdl_t memb_list_hdl;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    bcmecmp_grp_attr_t *grp_bk_attr, *grp_bk_attr_base;
    bcmecmp_hw_entry_attr_t *hw_ent_attr, *hw_ent_attr_base;
    uint32_t bucket, first;
    uint32_t grp_size;

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
    grp_list_hdl = ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].list_hdl;
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    grp_bk_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr);

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT not valid for ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_EXIT();
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_OVERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    hw_ent_attr_base = (bcmecmp_hw_entry_attr_t *)
                          (ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].ent_bk_arr);
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(
                                         unit,
                                         group,
                                         BCMECMP_GRP_TYPE_OVERLAY),
                                         ecmp_id) &&
             !(hw_ent_attr->flags & BCMECMP_ENTRY_DEFRAG_STAGED))
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))
            ) {
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
                        (BCMECMP_TBL_PTR(
                         unit,
                         group, BCMECMP_GRP_TYPE_OVERLAY),
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
            if (grp_attr->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                grp_size = grp_attr->rh_entries_cnt;
            } else {
                grp_size = grp_attr->max_paths;
            }
            memb_list_hdl = ecmp_info->member[grp_attr->grp_type].list_hdl;
            if (bcmecmp_grp_is_member_replication(grp_attr->grp_type,
                                                  grp_attr->weighted_mode)) {
                memb_list_hdl =
                        ecmp_info->member[grp_attr->grp_type].list_hdl_1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free(memb_list_hdl,
                                          grp_size,
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
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id++ ) {
        grp_attr = grp_attr_base + ecmp_id;
        grp_bk_attr = grp_bk_attr_base + ecmp_id;
        hw_ent_attr = hw_ent_attr_base + ecmp_id;
        if ((!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                        BCMECMP_GRP_TYPE_OVERLAY),
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
            if (grp_bk_attr->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                grp_size = grp_bk_attr->rh_entries_cnt;
            } else {
                grp_size = grp_bk_attr->max_paths;
            }
            memb_list_hdl = ecmp_info->member[grp_bk_attr->grp_type].list_hdl;
            if (bcmecmp_grp_is_member_replication(grp_attr->grp_type,
                                                  grp_attr->weighted_mode)) {
                memb_list_hdl =
                        ecmp_info->member[grp_bk_attr->grp_type].list_hdl_1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id(memb_list_hdl,
                                              grp_size,
                                              bucket,
                                              first));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_alloc_id(grp_list_hdl,
                                              1,
                                              SHR_ITBM_INVALID,
                                              ecmp_id));
        }

        bcmecmp_overlay_group_rh_revert(unit, ecmp_id, grp_bk_attr);

        BCMECMP_GRP_TYPE(unit, ecmp_id) = BCMECMP_GRP_TYPE_BK(unit, ecmp_id);
        BCMECMP_GRP_LT_SID(unit, ecmp_id) = BCMECMP_GRP_LT_SID_BK(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_GRP_LB_MODE_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = BCMECMP_GRP_NHOP_SORTED_BK
                                                                (unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS_BK(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS_BK(unit,
                                                                       ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) =
        BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id);
        BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) =
                BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) =
                BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id);
        grp_attr->in_use_itbm = grp_bk_attr->in_use_itbm;
        if (!grp_list_hdl) {
            tbl_size = (size_t)BCMECMP_TBL_SIZE(unit, member,
                                                BCMECMP_GRP_TYPE_OVERLAY)
                                          + BCMECMP_TBL_SIZE(unit, member,
                                                       BCMECMP_GRP_TYPE_UNDERLAY);
            sal_memcpy(tbl_ptr->ent_arr, tbl_ptr->ent_bk_arr,
                       (sizeof(bcmecmp_hw_entry_attr_t) * tbl_size));

            BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                BCMECMP_GRP_TYPE_OVERLAY), ecmp_id)
                = BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                         BCMECMP_GRP_TYPE_OVERLAY),
                                                         ecmp_id);
        }
        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT) {
            BCMECMP_RH_RAND_SEED(unit, ecmp_id) =
                                            BCMECMP_RH_RAND_SEED_BK(unit, ecmp_id);
        }

        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_OVERLAY),
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
 * \brief ECMP_OVERLAY In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to ECMP_OVERLAY logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = bcmecmp_overlay_group_validate,

    /*! Extended staging function. */
    .op_stage = bcmecmp_overlay_group_stage,

    /*! Commit function. */
    .commit = bcmecmp_overlay_group_commit,

    /*! Abort function. */
    .abort = bcmecmp_overlay_group_abort
};

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief ECMP_OVERLAY IMM callback register
 *
 * regist ECMP_OVERLAY LT callback in IMM module
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid  Logic table ID.
 *
 * \retval None.
 */
int
bcmecmp_overlay_group_imm_register(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for ECMP_OVERLAYt here.
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
