/*! \file bcmcth_trunk_vp_common_imm.c
 *
 * TRUNK_VP imm handlers common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmcth/bcmcth_trunk_vp_common_imm.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TRUNK_VP LT field array free
 *
 * \param [in] unit This is device unit number.
 * \param [in] in Ltd fields array
 *
 * \retval None.
 */
static int
bcmcth_trunk_vp_group_lt_fields_free(int unit,
                             bcmltd_fields_t *in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        BCMCTH_TRUNK_VP_FREE(in->field);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP LT fields allocate
 *
 * \param [in] unit This is device unit number.
 * \param [in/out] in Ltd fields pointer.
 * \param [in] num_fields The num of fields element.
 *
 * \retval None.
 */
static int
bcmcth_trunk_vp_group_lt_fields_allocate(int unit,
                                 bcmltd_fields_t *in,
                                 size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    BCMCTH_TRUNK_VP_ALLOC(in->field, alloc_sz, "bcmctTrunkVpEntryFields");

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        BCMCTH_TRUNK_VP_ALLOC(in->field[i],
            sizeof(bcmltd_field_t), "bcmcthTrunkVpEntryField");
    }

    SHR_FUNC_EXIT();

exit:
    bcmcth_trunk_vp_group_lt_fields_free(unit, in);

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Initalize bcmcth_trunk_vp_lt_entry_t structure.
 *
 * Initializes bcmcth_trunk_vp_lt_entry_t structure member
 * variables to required initial values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LTD Source identifier.
 * \param [in] lt_entry Pointer to bcmcth_trunk_vp_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_trunk_vp_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmcth_trunk_vp_lt_entry_t_init.\n")));

    /* Get the TRUNK_VP driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    vp_drv_var = drv_var->trunk_vp_var;
    SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);

    if (lt_entry) {
        /* Initialize group type value. */
        if (sid == (uint32_t) vp_drv_var->ids->trunk_vp_ltid) {
            lt_entry->grp_type = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;
        } else if (sid == (uint32_t) vp_drv_var->ids->trunk_vp_weighted_ltid) {
            lt_entry->grp_type = BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        /* Initialize LT source ID value. */
        lt_entry->glt_sid = sid;

        /* Operational argument.. */
        lt_entry->op_arg = NULL;

        /* Initialize LT fields set bitmap to null. */
        lt_entry->fbmp = 0;

        /* Initialize TRUNK_VP group identifier value to invalid. */
        lt_entry->trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;

        if (!BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
            /*
             * Initialize TRUNK_VP group default load balancing
             * mode to Regular mode.
             */
            lt_entry->lb_mode = BCMCTH_TRUNK_VP_LB_MODE_REGULAR;
        } else {
            /*
             * For the TRUNK_VP_WEIGHTEDt LT lb_mode is equal to weighted_size.
             */
            lt_entry->lb_mode = BCMCTH_TRUNK_VP_LT_MIN_WEIGHTED_SIZE(unit,
                                                     group, lt_entry->grp_type);
        }

        /*
         * Initialize weighted hashing group size value to device
         * minimum weighted hashing group size value.
         */
        lt_entry->weighted_size = BCMCTH_TRUNK_VP_LT_MIN_WEIGHTED_SIZE
                                      (unit, group, lt_entry->grp_type);

        /* Initialize to device TRUNK_VP groups max_members value. */
        if (!BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
            lt_entry->max_members = BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
                                        (unit, group, lt_entry->grp_type);
        } else {
            lt_entry->max_members =
                bcmcth_trunk_vp_weighted_size_convert_to_member_cnt(
                 lt_entry->weighted_size);
        }

        /* Initialize configured paths to zero. */
        lt_entry->member_cnt = 0;
        /* Initialize Group Member table start index to invalid. */
        lt_entry->mstart_idx = BCMCTH_TRUNK_VP_INVALID;

        /* Initialize imm event to lookup. */
        lt_entry->event = BCMIMM_ENTRY_LOOKUP;

        if (vp_drv_var->member0_info) {
            for (i = 0; i < vp_drv_var->member0_info->flds_count; i++) {
                sal_memset(lt_entry->member0_field[i],
                           BCMCTH_TRUNK_INVALID,
                           sizeof(lt_entry->member0_field[i]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LT entry input parameters validate function.
 *
 * Validate LT entry input parameter values based on the LT operation type.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry event.
 * \param [in] sid Logical table source identifier.
 * \param [in] lt_entry TRUNK_VP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmcth_trunk_vp_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bool update_member_cnt_max_members = 0;
    bool in_use;
    uint32_t comp_id;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                 "\n bcmcth_trunk_vp_lt_input_params_validate %u %u.\n"),
                 sid, lt_entry->grp_type));

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /*
     * TRUNK_VP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET
            (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID field value not specified.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Verify TRUNK_VP_ID is in valid range. */
    if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
        BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID)
        && (lt_entry->trunk_vp_id <  BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
                                        (unit, group, gtype)
            || lt_entry->trunk_vp_id > BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
                                        (unit, group, gtype))) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d is invalid.\n"),
                     lt_entry->trunk_vp_id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (shr_itbm_list_elem_state_get
            (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
             SHR_ITBM_INVALID,
             lt_entry->trunk_vp_id,
             &in_use,
             &comp_id));
    /* Validate input fields based on the type of LT table operation. */
    switch (opcode) {
        case BCMIMM_ENTRY_INSERT:
            /* Verify specified TRUNK_VP group is not in use. */
            if (in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "TRUNK_VP_ID=%d exists.\n"),
                             lt_entry->trunk_vp_id));
                if (BCMCTH_TRUNK_VP_GRP_TYPE(unit, lt_entry->trunk_vp_id) ==
                   lt_entry->grp_type){
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }

            /* Verify specified LB_MODE is valid value. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_LB_MODE)
                && (lt_entry->lb_mode > BCMCTH_TRUNK_VP_LT_MAX_LB_MODE
                                                (unit, group, gtype)
                    || lt_entry->lb_mode < BCMCTH_TRUNK_VP_LT_MIN_LB_MODE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d is invalid.\n"),
                             lt_entry->lb_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_MEMBERS value is in valid range. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)
                && (lt_entry->max_members < BCMCTH_TRUNK_VP_LT_MIN_MEMBERS
                                                (unit, group, gtype)
                    || lt_entry->max_members > BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
                                                (unit, group, gtype))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_MEMBERS=%u is invalid.\n"),
                             lt_entry->max_members));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_MEMBERS value is a power-of-two. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)
                && BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_members & (lt_entry->max_members - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_MEMBERS=%u not a power-of-two.\n"),
                             lt_entry->max_members));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify MEMBER_CNT value is in valid range if it's non-zero. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && lt_entry->member_cnt
                && lt_entry->member_cnt > BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS
                                            (unit, group, gtype)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u is invalid "
                                        "(Grp MAX_MEMBERS=%u).\n"),
                            lt_entry->member_cnt,
                            BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS(unit, group, gtype)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified non-zero MEMBER_CNT value is a power-of-two. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->member_cnt
                && (lt_entry->member_cnt & (lt_entry->member_cnt - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u not a power-of-two.\n"),
                             lt_entry->member_cnt));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->member_cnt > lt_entry->max_members) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "MEMBER_CNT=%u exceeds MAX_MEMBERS=%u.\n"),
                            lt_entry->member_cnt,
                            lt_entry->max_members));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * MEMBER_CNT value must equal to the
             * WEIGHTED_SIZE members count value.
             */
            if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)
                && BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)) {
                if (lt_entry->member_cnt) {
                    if (lt_entry->weighted_size
                        != bcmcth_trunk_vp_member_cnt_convert_to_weighted_size(
                        lt_entry->member_cnt)) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u is invalid.\n"),
                             lt_entry->member_cnt));

                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                }
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            /*
             * TRUNK_VP_ID must be in use and logical table source identifier
             * value of the entry being updated must match the LT SID of the
             * entry stored during INSERT operation.
             */

            if ((BCMCTH_TRUNK_VP_GRP_TYPE(unit, lt_entry->trunk_vp_id)
                 != lt_entry->grp_type) || !in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "TRUNK_VP_ID=%d not in-use %d.\n"),
                   lt_entry->trunk_vp_id,
                   BCMCTH_TRUNK_VP_GRP_LT_SID(unit, lt_entry->trunk_vp_id)));

                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS) &&
                BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)) {
                if (lt_entry->max_members >= lt_entry->member_cnt) {
                    update_member_cnt_max_members = 1;
                }
            }

            /* Verify specified MAX_MEMBERS value is in valid range. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)
                && (lt_entry->max_members < BCMCTH_TRUNK_VP_LT_MIN_MEMBERS
                                                (unit, group, gtype)
                    || lt_entry->max_members > BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
                                                (unit, group, gtype)
                    || (lt_entry->max_members <  BCMCTH_TRUNK_VP_GRP_MEMBER_CNT
                                            (unit, lt_entry->trunk_vp_id)
                        && !update_member_cnt_max_members))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_MEMBERS=%u is invalid.\n"),
                             lt_entry->max_members));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_MEMBERS value is a power-of-two. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)
                && BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_members & (lt_entry->max_members - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_MEMBERS=%u not a power-of-two.\n"),
                             lt_entry->max_members));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify MEMBER_CNT value is in valid range if it's non-zero. */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && lt_entry->member_cnt
                && (lt_entry->member_cnt > BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS
                                            (unit, lt_entry->trunk_vp_id)
                    && !update_member_cnt_max_members)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u is invalid "
                                        "(Grp MAX_MEMBERS=%u).\n"),
                            lt_entry->member_cnt,
                            BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS
                                (unit, lt_entry->trunk_vp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified non-zero MEMBER_CNT value is a power-of-two
             * value for REGULAR and RANDOM LB_MODE groups.
             */
            if (BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->member_cnt
                && (lt_entry->member_cnt & (lt_entry->member_cnt - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u not a power-of-two.\n"),
                             lt_entry->member_cnt));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /*
             * MEMBER_CNT value must equal to the WEIGHTED_SIZE
             * members count value.
             */
            if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)
                && BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && lt_entry->member_cnt) {
                if (lt_entry->member_cnt != lt_entry->max_members) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u is invalid.\n"),
                             lt_entry->member_cnt));

                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            /*
             * MEMBER_CNT value must equal to the
             * WEIGHTED_SIZE members count value.
             */
            if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)
                && BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
                BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)
                && lt_entry->member_cnt) {
                if (lt_entry->weighted_size
                    != bcmcth_trunk_vp_member_cnt_convert_to_weighted_size(
                    lt_entry->member_cnt)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MEMBER_CNT=%u is invalid.\n"),
                             lt_entry->member_cnt));

                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->member_cnt > lt_entry->max_members) {
                if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                       "MEMBER_CNT=%u should be propotional to the weighted size.\n"),
                       lt_entry->member_cnt));
                } else {
                     LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                 "MEMBER_CNT=%u exceeds MAX_MEMBERS=%u.\n"),
                                lt_entry->member_cnt,
                                lt_entry->max_members));
                }
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMIMM_ENTRY_DELETE:
            /*
             * TRUNK_VP_ID must be in use and logical table source identifier
             * value of the entry being Deleted/Looked-up must match the LT SID
             * of the entry stored during INSERT operation.
             */
            if ((BCMCTH_TRUNK_VP_GRP_TYPE(unit, lt_entry->trunk_vp_id)
                 != lt_entry->grp_type) || !in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "TRUNK_VP_ID=%d not in-use.\n"),
                             lt_entry->trunk_vp_id));

                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Update sorted TRUNK_VP member fields.
 *
 * update TRUNK_VP member info in IMM with sorted member array.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry TRUNK_VP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
int
bcmcth_trunk_vp_lt_member_update(int unit,
                             bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields;
    bcmcth_trunk_vp_grp_ltd_fields_t *fids = NULL;
    bcmcth_trunk_vp_grp_type_t grp_type = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    num_fields = lt_entry->member_cnt + 1;

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_vp_group_lt_fields_allocate(unit,
                                        &imm_fields,
                                        num_fields));
    flist = &imm_fields;

    grp_type = lt_entry->grp_type;
    fids = (bcmcth_trunk_vp_grp_ltd_fields_t *)BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
                                            (unit, group, grp_type);
    data_field = flist->field[0];
    data_field->id = fids->trunk_vp_id_fid;
    data_field->idx = 0;
    data_field->data = lt_entry->trunk_vp_id;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_update(unit, 0, lt_entry->glt_sid, &imm_fields));

exit:
    bcmcth_trunk_vp_group_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm TRUNK_VP LT fields.
 *
 * Get the current fields from imm TRUNK_VP LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry TRUNK_VP LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
int
bcmcth_trunk_vp_lt_fields_lookup(int unit,
                             bcmcth_trunk_vp_lt_entry_t *lt_entry,
                             bcmcth_trunk_vp_lt_fields_fill_cb * cb)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields= 0, i;
    int rv = SHR_E_NONE;
    bcmcth_trunk_vp_grp_ltd_fields_t * fids = NULL;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_vp_group_lt_fields_allocate(unit,
                                          &key_fields,
                                          1));
    gtype = lt_entry->grp_type;
    fids = (bcmcth_trunk_vp_grp_ltd_fields_t *)BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
                                             (unit, group, gtype);
    SHR_NULL_CHECK(fids, SHR_E_INTERNAL);
    key_fields.field[0]->id   = fids->trunk_vp_id_fid;
    key_fields.field[0]->data = lt_entry->trunk_vp_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, lt_entry->glt_sid, &num_fields));

    num_fields += BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, lt_entry->trunk_vp_id);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_vp_group_lt_fields_allocate(unit,
                                        &imm_fields,
                                        num_fields));

    /* Lookup IMM table; error if entry not found */
    rv = bcmimm_entry_lookup(unit,
                             lt_entry->glt_sid,
                             &key_fields,
                             &imm_fields);
    if (rv == SHR_E_RESOURCE) {
        /* In the case that some array elements are reset to default value after
         * MAX paths is decreasing, but they will still be returned by IMM. So
         * use BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS to lookup again
         * when resource error.
         */
        /* reset imm fields count for freeing mem */
        imm_fields.count = num_fields;
        bcmcth_trunk_vp_group_lt_fields_free(unit, &imm_fields);

        num_fields -= BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit,
                      lt_entry->trunk_vp_id);
        if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) {
            num_fields += BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS(unit, group, gtype);
        }

        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_group_lt_fields_allocate(unit,
                                              &imm_fields,
                                              num_fields));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_lookup(unit,
                                 lt_entry->glt_sid,
                                 &key_fields,
                                 &imm_fields));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d contains %u fields.\n"),
                     lt_entry->trunk_vp_id,
                     (unsigned int)imm_fields.count));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        SHR_IF_ERR_EXIT(cb(unit, data_field, lt_entry));
    }

exit:
    bcmcth_trunk_vp_group_lt_fields_free(unit, &key_fields);
    /* reset imm fields count for freeing mem */
    imm_fields.count = num_fields;
    bcmcth_trunk_vp_group_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

