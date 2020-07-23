/*! \file bcmecmp_common_imm.c
 *
 * ECMP imm handlers common functions.
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
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_member_dest.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief LT entry next-hop ids input parameter validation function.
 *
 * Validates total NHOP_IDs count matches NUM_PATHS value and the NHOP_IDs have
 * been populated at correct indices in the passed NHOP_ID array.
 *
 * For example:
 * ===========
 * If NUM_PATHS=2 and NHOP_ID[0],NHOP_ID[1023] are supplied then it's invalid
 * configuration. NHOP_ID[0] and NHOP_ID[1] should be populated for this
 * configuration.
 *
 * This function will check if NHOP_ID[0] and NHOP_ID[1] have valid NHOP_ID
 * values at these indices. If they are equal to BCMECMP_INVALID, then
 * Invalid Parameter error is returned.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry operation code.
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
static int
bcmecmp_lt_nhops_validate(int unit,
                          bcmimm_entry_event_t opcode,
                          bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx = 0; /* NHOP_IDs iterator variable. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* ECMP group identifier. */
    uint32_t nh_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_nhops_validate.\n")));

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /* Get the group ECMP_ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Check and return error if ECMP_UNDERLAY_ID is also set. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_UNDERLAY_ID field cannot be set for this"
                                " entry.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate checkes based on the LT operation type. */
    switch (opcode) {
        case BCMIMM_ENTRY_UPDATE:
            /*
             * Verify specified ECMP_NHOP value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                && (lt_entry->ecmp_nhop
                    != BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_NHOP=%d expected.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, ecmp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_EXIT();
    }

    if (opcode == BCMIMM_ENTRY_UPDATE) {
        nh_count = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    }
    /*
     * Validate the NHOP_IDs have been passed at correct indices. Iterate over
     * count no. of NHOP_IDs and confirm all of them are valid.
     */
    for (idx = 0; (idx < nh_count) && (idx < BCMECMP_NHOP_ARRAY_SIZE); idx++) {
        /*
         * If a NHOP_ID was pouplated at wrong index, then it will be
         * equal to BCMECMP_INVALID and this condition will be TRUE.
         */
        if (BCMECMP_INVALID == lt_entry->nhop_id[idx]) {
            continue;
        }

        /*
         * Verify NHOP_ID values are within valid NHOP_ID range of this
         * device.
         */
        if (lt_entry->nhop_id[idx] < BCMECMP_LT_MIN_NHOP_ID
                                            (unit, group, gtype)
             || lt_entry->nhop_id[idx] > BCMECMP_LT_MAX_NHOP_ID
                                            (unit, group, gtype)) {

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NHOP_ID[%d]=%d is invalid.\n"),
                         idx,
                         lt_entry->nhop_id[idx]));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief LT entry Underlay ECMP IDs input parameter validation function.
 *
 * Verifies total ECMP_UNDERLAY_IDs count matches NUM_PATHS value and the
 * ECMP_UNDERLAY_IDs have been populated at correct indices in the supplied
 * ECMP_UNDERLAY_ID array.
 *
 * For example:
 * ===========
 * If NUM_PATHS=2 and ECMP_UNDERLAY_ID[0],ECMP_UNDERLAY_ID[1023] are supplied
 * then it's invalid configuration. ECMP_UNDERLAY_ID[0] and ECMP_UNDERLAY_ID[1]
 * should be populated for this configuration.
 *
 * This function will also validate supplied ECMP_UNDERLAY_IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry operation code.
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
static int
bcmecmp_lt_uecmp_ids_validate(int unit,
                              bcmimm_entry_event_t opcode,
                              bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx = 0; /* ECMP_UNDERLAY_ID array index iterator variable. */
    uint32_t max_paths = 0;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* ECMP group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    uint32_t nh_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_uecmp_ids_validate.\n")));

    /* Check and return error if NHOP_ID is also set. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Expected ECMP_UNDERLAY_ID to be set as "
                                "ECMP_NHOP=%d.\n"),
                     lt_entry->ecmp_nhop));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /* Get the group ECMP_ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Validate checkes based on the LT operation type. */
    switch (opcode) {
        case BCMIMM_ENTRY_INSERT:
            /* Check and return error if ECMP_NHOP is not TRUE. */
            if (!lt_entry->ecmp_nhop) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Expected ECMP_NHOP=(1)/(TRUE).\n")));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
                max_paths = lt_entry->max_paths;
            } else {
                max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
            }

            /*
             * Check if total number of ECMP_UNDERLAY_ID[] elements is less
             * than the group's MAX_PATHS value, return error if it exceeds
             * this limit.
             */
            if (lt_entry->uecmp_ids_count > max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) ECMP_UNDERLAY_ID[] fields "
                                        "exceeds MAX_PATHS=%u count.\n"),
                             lt_entry->uecmp_ids_count,
                             max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            /*
             * The count of UNDERLAY_ECMP_ID[] fields set during UPDATE cannot
             * exceed group MAX_PATHS value.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_MAX_PATHS)) {
                max_paths = lt_entry->max_paths;
            } else if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                            BCMECMP_LT_FIELD_LB_MODE) &&
                       lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
            } else {
                max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
            }

            if (lt_entry->uecmp_ids_count > max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) ECMP_UNDERLAY_ID[] fields "
                                        "exceed MAX_PATHS=%u count.\n"),
                            lt_entry->uecmp_ids_count,
                            max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified ECMP_NHOP value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                && (lt_entry->ecmp_nhop
                    != BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_NHOP=%d expected.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Check if ECMP_NHOP is TRUE for this entry as ECMP_UNDERLAY_ID
             * field has been set in this update call. If ECMP_NHOP is
             * false, return invalid parameter error.
             */
            if (!BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                    && !BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_UNDERLAY_ID cannot be set"
                                        " when ECMP_NHOP=%d.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_EXIT();
    }

    if (opcode == BCMIMM_ENTRY_UPDATE) {
        nh_count = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    } else {
        nh_count = lt_entry->uecmp_ids_count;
    }

    /*
     * Validate the ECMP_UNDERLAY_IDs have been passed at correct indices.
     * Iterate over count no. of ECMP_UNDERLAY_IDs and confirm all of them are
     * valid.
     */
    for (idx = 0; idx < nh_count; idx++) {
        /*
         * If a ECMP_UNDERLAY_ID was pouplated at wrong index, then it will be
         * equal to BCMECMP_INVALID and this condition will be TRUE.
         */
        if (BCMECMP_INVALID == lt_entry->uecmp_id[idx]) {
            if (opcode == BCMIMM_ENTRY_UPDATE) {
                continue;
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_UNDERLAY_ID[index=%d] value "
                                    "required.\n"),
                        idx));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Verify supplied Underly ECMP group ID is in valid range. */
        if (lt_entry->uecmp_id[idx] < BCMECMP_LT_MIN_ECMP_ID
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_UNDERLAY)
            || lt_entry->uecmp_id[idx] > BCMECMP_LT_MAX_ECMP_ID
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_UNDERLAY)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_UNDERLAY_ID[%d]=%d invalid.\n"),
                        idx,
                        lt_entry->uecmp_id[idx]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP LT field array free
 *
 * \param [in] unit This is device unit number.
 * \param [in] in Ltd fields array
 *
 * \retval None.
 */
static int
bcmecmp_group_lt_fields_free(int unit,
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
        BCMECMP_FREE(in->field);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP LT fields allocate
 *
 * \param [in] unit This is device unit number.
 * \param [in/out] in Ltd fields pointer.
 * \param [in] num_fields The num of fields element.
 *
 * \retval None.
 */
static int
bcmecmp_group_lt_fields_allocate(int unit,
                                 bcmltd_fields_t *in,
                                 size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    BCMECMP_ALLOC(in->field, alloc_sz, "bcmecmpEntryFields");

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        BCMECMP_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmecmpEntryField");
    }

    SHR_FUNC_EXIT();

exit:
    bcmecmp_group_lt_fields_free(unit, in);

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Initalize bcmecmp_lt_entry_t structure.
 *
 * Initializes bcmecmp_lt_entry_t structure member variables to required initial
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LTD Source identifier.
 * \param [in] lt_entry Pointer to bcmecmp_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmecmp_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    bcmecmp_control_t * ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    bcmecmp_lt_field_attrs_t * lt_fattr = NULL;
    bcmltd_sid_t ltd_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_entry_t_init.\n")));

    if (lt_entry) {

        /* Initialize group type value. */
        switch (sid) {
            case ECMP_OVERLAYt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_OVERLAY;
                break;
            case ECMP_UNDERLAYt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_UNDERLAY;
                break;
            case ECMP_WEIGHTEDt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_WEIGHTED;
                break;
            case ECMP_WEIGHTED_UNDERLAYt:
                ltd_sid = ecmp_ctrl->ecmp_info.
                              group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ltd_sid;
                if (ltd_sid == sid) {
                    lt_entry->grp_type = BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED;
                } else {
                    lt_entry->grp_type = BCMECMP_GRP_TYPE_WEIGHTED;
                }
                break;
            case ECMP_WEIGHTED_OVERLAYt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED;
                break;
            default:
                if (ids != NULL) {
                    if (sid == ids->ecmp_level0_ltid) {
                        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL0;
                    } else if (sid == ids->ecmp_level1_ltid) {
                        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL1;
                    } else if (sid == ids->ecmp_wlevel0_ltid) {
                        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED;
                    } else if ( sid == ids->ecmp_wlevel1_ltid) {
                        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED;
                    }  else {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                } else {
                    lt_entry->grp_type = BCMECMP_GRP_TYPE_SINGLE;
                }
                break;
        }

        /* Initialize LT source ID value. */
        lt_entry->glt_sid = sid;

        /* Operational argument. */
        lt_entry->op_arg = NULL;

        /* Initialize LT fields set bitmap to null. */
        SHR_BITCLR_RANGE(lt_entry->fbmp, 0, BCMECMP_FLAG_BMP_MAX_BIT);

        /* Initialize ECMP group identifier value to invalid. */
        lt_entry->ecmp_id = BCMECMP_INVALID;

        if (!BCMECMP_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
            /* Initialize ECMP group default load balancing mode to Regular mode. */
            lt_entry->lb_mode = BCMECMP_LB_MODE_REGULAR;
        } else {
            lt_entry->lb_mode = BCMECMP_LB_MODE_COUNT;
        }

        /* Set LT entry's default ECMP_NHOP field value. */
        if (bcmecmp_grp_is_overlay(lt_entry->grp_type)) {
            if (ecmp_ctrl->ecmp_info.ovlay_memb_count) {
                lt_entry->ecmp_nhop = FALSE;
            } else {
                /*
                 * For ECMP_OVERLAY LT, set the default value of this field
                 * to the value retrieved from device's LRD.
                 */
                lt_entry->ecmp_nhop = BCMECMP_LT_DEF_ECMP_NHOP
                                            (unit, group, lt_entry->grp_type);
            }
        } else {
            /* This field is not present in non-overlay LTs. */
            lt_entry->ecmp_nhop = FALSE;
        }

        sal_memset(lt_entry->u_ecmp_nhop, BCMECMP_INVALID,
                   sizeof(lt_entry->u_ecmp_nhop));

        lt_entry->u_ecmp_nhop_count = 0;

        /* Initialize ECMP_UNDERLAY_IDs array elements to invalid */
        sal_memset
            (lt_entry->uecmp_id, BCMECMP_INVALID, sizeof(lt_entry->uecmp_id));

        /* Initialize total Underlay ECMP IDs parsed count to zero. */
        lt_entry->uecmp_ids_count = 0;

        /* Initialize NHOP_IDs array elements to invalid */
        sal_memset
            (lt_entry->nhop_id, BCMECMP_INVALID, sizeof(lt_entry->nhop_id));

        sal_memset
            (lt_entry->rh_nhop_id, BCMECMP_INVALID, sizeof(lt_entry->rh_nhop_id));

        /* Initialize total NHOP_IDs parsed count to zero. */
        lt_entry->nhop_ids_count = 0;

        lt_entry->rh_nhop_ids_count = 0;

        sal_memset
            (lt_entry->o_nhop_id, BCMECMP_INVALID, sizeof(lt_entry->o_nhop_id));

        lt_entry->o_nhop_ids_count = 0;

        /* By default next-hops are not sorted. */
        lt_entry->nhop_sorted = FALSE;

        /*
         * Initialize weighted hashing group size value to device
         * minimum weighted hashing group size value.
         */
        lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl->ecmp_info.group[lt_entry->grp_type].lt_fattrs;
        if (lt_fattr &&
            lt_fattr->def_weighted_size != (uint32_t)BCMECMP_INVALID) {
            lt_entry->weighted_size = lt_fattr->def_weighted_size;
        } else {
            lt_entry->weighted_size = BCMECMP_WEIGHTED_SIZE_256;
        }

        /* Initialize to device ECMP groups max_paths value. */
        if (!BCMECMP_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
            lt_entry->max_paths = BCMECMP_LT_MAX_PATHS
                                        (unit, group, lt_entry->grp_type);
        } else {
            if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths) {
                lt_entry->max_paths =
                    ecmp_ctrl->ecmp_drv->weight_size_to_max_paths
                        (lt_entry->weighted_size);
            } else {
                lt_entry->max_paths =
                    bcmecmp_weighted_size_convert_to_num_paths
                        (lt_entry->weighted_size);
            }
        }

        lt_entry->weighted_mode = BCMECMP_LT_MIN_WEIGHTED_MODE
                                      (unit, group, lt_entry->grp_type);

        sal_memset(lt_entry->weight, BCMECMP_INVALID, sizeof(lt_entry->weight));
        lt_entry->weight_count = 0;

        /* Initialize configured paths to zero. */
        lt_entry->num_paths = 0;
        lt_entry->rh_num_paths = 0;

        /*
         * Initialize resilient hashing group size encoding value to device
         * minimum RH group size encoding value.
         */
        lt_entry->rh_size_enc = BCMECMP_LT_MIN_RH_SIZE
                                    (unit, group, lt_entry->grp_type);

        /*
         * Initialize resilient hashing group entries count value to device
         * minimum RH group size value.
         */
        lt_entry->rh_entries_cnt = (uint32_t)(1 << lt_entry->rh_size_enc);

        /* Initialize RH-ECMP group entries array base pointer to null. */
        lt_entry->rh_entries_arr = NULL;

        /*
         * Initialize RH group next-hop members ECMP member table entries
         * usage count array base pointer to null.
         */
        lt_entry->rh_memb_ecnt_arr = NULL;

        /*
         * Initialize RH group random seed to null.
         */
        lt_entry->rh_rand_seed = 0;

        /* Initialize Group Member table start index to invalid. */
        lt_entry->mstart_idx = BCMECMP_INVALID;

        /*
         * Initialize aggregation group monitor pool value to device
         * minimum aggregation group monitor pool value.
         */
        lt_entry->agm_pool = BCMECMP_LT_MIN_MON_AGM_POOL
                                    (unit, group, lt_entry->grp_type);

        /*
         * Initialize aggregation group monitor ID value to device
         * minimum aggregation group monitor ID value.
         */
        lt_entry->agm_id = BCMECMP_LT_MIN_MON_AGM_ID
                                    (unit, group, lt_entry->grp_type);

        lt_entry->ctr_ing_eflex_action_id =
            BCMECMP_LT_MIN_CTR_ING_EFLEX_ACTION_ID(unit, group,
                                                   lt_entry->grp_type);

        lt_entry->ctr_ing_eflex_object =
            BCMECMP_LT_MIN_CTR_ING_EFLEX_OBJECT(unit, group,
                                                lt_entry->grp_type);

        /* Initialize imm event to lookup. */
        lt_entry->event = BCMIMM_ENTRY_LOOKUP;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LTA out fields populate routine.
 *
 * Get ECMP LT field values from lt_entry and populate them in LTD out fields.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry ECMP LT entry data.
 * \param [in] include_key Include LT key field in the out fields list.
 * \param [out] out LTA out field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c lt_entry to \c out.
 */
int
bcmecmp_lt_fields_populate(int unit,
                           bcmecmp_lt_entry_t *lt_entry,
                           bool include_key,
                           bcmltd_fields_t *out)
{
    size_t fld_idx = 0; /* LT field index. */
    uint32_t path = 0, num_paths = 0, grp_mpaths = 0; /* Group paths. */
    bcmecmp_grp_ltd_fields_t *fids = NULL; /* Group LTD Field IDs. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Group NUM_PATHS field value. */
    num_paths = lt_entry->num_paths;

    /* Group MAX_PATHS field value. */
    grp_mpaths = lt_entry->max_paths;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_fields_populate[num_paths=%d].\n"),
                 num_paths));

    gtype = lt_entry->grp_type;

    fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                            (unit, group, gtype);
    SHR_NULL_CHECK(fids, SHR_E_INTERNAL);

    /* Check if LT key field must be populated. */
    if (include_key) {
        /* Set ECMP_ID logical field value. */
        out->field[fld_idx]->id = fids->ecmp_id_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t) lt_entry->ecmp_id;
    }

    /* Set LB_MODE logical field value. */
    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        out->field[fld_idx]->id = fids->lb_mode_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t) lt_entry->lb_mode;
    } else {
        out->field[fld_idx]->id = fids->weighted_size_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t) lt_entry->weighted_size;
    }

    if (BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        /* Set NHOP_SORTED logical field value. */
        out->field[fld_idx]->id = fids->nhop_sorted_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = lt_entry->nhop_sorted;

        /* Set MAX_PATHS logical field value. */
        out->field[fld_idx]->id = fids->max_paths_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t) lt_entry->max_paths;
    }

    /* Set NUM_PATHS logical field value. */
    out->field[fld_idx]->id = fids->num_paths_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->num_paths;

    /*
     * Set ECMP_NHOP logical field value for Overlay logical table entries if
     * this field is supported in this table for this device.
     */
    if (BCMECMP_GRP_TYPE_OVERLAY == gtype
        && (BCMLTD_SID_INVALID != (uint32_t) fids->nhop_id_fid)) {
        out->field[fld_idx]->id = fids->ecmp_nhop_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = lt_entry->ecmp_nhop;
    }

    /*
     * Check if it's an Overlay ECMP group LT and ECMP_NHOP is true. If this
     * condition is satisfied, then populate ECMP_UNDERLAY_ID field values for
     * this group.
     */
    if (BCMECMP_GRP_TYPE_OVERLAY == gtype &&  lt_entry->ecmp_nhop) {
        /* Populate retrieved ECMP_UNDERLAY_ID values. */
        for (path = 0; path < grp_mpaths; path++, fld_idx++) {
            out->field[fld_idx]->id = fids->uecmp_id_fid;
            out->field[fld_idx]->data = (uint64_t)lt_entry->uecmp_id[path];
            out->field[fld_idx]->idx = (uint32_t)path;
        }

        /* Set unused ECMP_UNDERLAY_ID fields to default value. */
        for (path = grp_mpaths;
             path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
             path++, fld_idx++) {
            out->field[fld_idx]->id = fids->uecmp_id_fid;
            out->field[fld_idx]->data = (uint64_t)BCMECMP_LT_MIN_UECMP_ID
                                                    (unit, group, gtype);
            out->field[fld_idx]->idx = (uint32_t)path;
        }

        /*
         * Initialize unused NHOP_ID logical fields to default value if this
         * field is supported in this logical table for this device.
         */
        if (BCMLTD_SID_INVALID != (uint32_t) fids->nhop_id_fid) {
            for (path = 0;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data =
                    (uint64_t)BCMECMP_LT_MIN_NHOP_ID(unit, group, gtype);
                out->field[fld_idx]->idx = (uint32_t)path;
            }
        }
    } else {
        if (BCMLTD_SID_INVALID != (uint32_t) fids->nhop_id_fid) {
            /* Populate retrieved NHOP_ID[] field values. */
            for (path = 0; path < grp_mpaths; path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data
                                    = (uint64_t)lt_entry->nhop_id[path];
                out->field[fld_idx]->idx = (uint32_t)path;
            }

            /* Set unused NHOP_ID[] fields to default value. */
            for (path = grp_mpaths;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data =
                        (uint64_t)BCMECMP_LT_MIN_NHOP_ID(unit, group, gtype);
                out->field[fld_idx]->idx = (uint32_t)path;
            }
        }

        /*
         * Initialize ECMP_UNDERLAY_ID fields to default value. When
         * ECMP_NHOP is FALSE, entry can support only regular next-hops as
         * group members.
         */
        if (BCMECMP_GRP_TYPE_OVERLAY == gtype && !lt_entry->ecmp_nhop) {
            for (path = 0;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->uecmp_id_fid;
                out->field[fld_idx]->data
                            = (uint64_t)BCMECMP_LT_MIN_UECMP_ID
                                            (unit, group, gtype);
                out->field[fld_idx]->idx = 0;
            }
        }
    }

    /*
     * Skip setting RH_SIZE field value for Underlay logical table
     * entries.
     */
    if (BCMECMP_GRP_TYPE_UNDERLAY != gtype
        && (!BCMECMP_GRP_IS_WEIGHTED(gtype))) {
        out->field[fld_idx]->id = fids->rh_size_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t)lt_entry->rh_size_enc;
    }

    /* Set MON_AGM_POOL logical field value. */
    out->field[fld_idx]->id = fids->agm_pool_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->agm_pool;

    /* Set MON_AGM_ID logical field value. */
    out->field[fld_idx]->id = fids->agm_id_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->agm_id;

    /* Set Out fields array legth. */
    out->count = fld_idx;

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "bcmecmp_lt_fields_populate[flds_cnt=%u] "
                                "- exit.\n"),
                    (uint32_t)fld_idx));
        SHR_FUNC_EXIT();
}

/*!
 * \brief reallocates resources during warm boot for a specific group type.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype group type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_wb_itbm_group_type_add(int unit, int gtype)
{
    int ecmp_id, gstart_idx, gend_idx;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    gstart_idx = BCMECMP_TBL_IDX_MIN(unit, group, gtype);
    if ((gtype != BCMECMP_GRP_TYPE_LEVEL0) &&
        (gtype != BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)) {
        gstart_idx += BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

    gend_idx = gstart_idx + BCMECMP_TBL_SIZE(unit, group, gtype) - 1;

    for (ecmp_id = gstart_idx; ecmp_id <= gend_idx; ecmp_id++) {
        /* check if the HA memory has the group configured. */
        if (BCMECMP_FLEX_GRP_ITBM_USE(unit, ecmp_id) == true) {
            gtype = BCMECMP_FLEX_GRP_TYPE(unit, ecmp_id);
        } else {
            continue;
        }

        mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
        max_members = BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id);
        rv = shr_itbm_list_block_alloc_id
                (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, group, gtype)),
                 1,
                 SHR_ITBM_INVALID,
                 ecmp_id);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n ECMP warmboot recovery for itbm list "
                                    "failed. Unable to allocare the itbm block "
                                    "for Group_id %d.\n"),
                         ecmp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        rv = shr_itbm_list_block_alloc_id
                (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, member, gtype)),
                 max_members,
                 SHR_ITBM_INVALID,
                 mstart_idx);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n ECMP warmboot recovery for itbm member "
                                    "list failed. Unable to allocare the itbm "
                                    "block for Group_id %d.\n"),
                         ecmp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief reallocates resources during warm boot for a specific group type.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype group type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_member0_wb_itbm_group_type_add(int unit)
{
    int grp_id, gstart_idx, gend_idx;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    gstart_idx = 0;

    gend_idx = BCMECMP_MEMBER0_LT_MAX_ECMP_ID(unit);

    for (grp_id = gstart_idx; grp_id <= gend_idx; grp_id++) {
        /* check if the HA memory has the group configured. */
        if (BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, grp_id) <= 0) {
            continue;
        }

        mstart_idx = BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX(unit, grp_id);
        max_members = BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, grp_id);

        rv = shr_itbm_list_block_alloc_id
                (BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)),
                 max_members,
                 SHR_ITBM_INVALID,
                 mstart_idx);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n ECMP MEMBER0 warmboot recovery for "
                                    "itbm member list failed. Unable to "
                                    "allocate the itbm block for ecmp_member0 id "
                                    "%d.\n"),
                         grp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief reallocates resources during warm boot for a specific group type.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype group type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_member1_wb_itbm_group_type_add(int unit)
{
    int grp_id, gstart_idx, gend_idx;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    gstart_idx = 0;

    gend_idx = BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit);

    for (grp_id = gstart_idx; grp_id <= gend_idx; grp_id++) {
        /* check if the HA memory has the group configured. */
        if (BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) <= 0) {
            continue;
        }

        mstart_idx = BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id);
        max_members = BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id);

        rv = shr_itbm_list_block_alloc_id
                (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)),
                 max_members,
                 SHR_ITBM_INVALID,
                 mstart_idx);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n ECMP MEMBER1 warmboot recovery for "
                                    "itbm member list failed. Unable to "
                                    "allocate the itbm block for ecmp_member1_id "
                                    "%d.\n"),
                         grp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief reallocates ECMP resources during warm boot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_wb_itbm_ecmp_group_add(int unit)
{
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    if (ids != NULL) {
        if (ids->ecmp_level0_ltid != (uint32_t)BCMECMP_INVALID) {
            SHR_IF_ERR_EXIT(bcmecmp_wb_itbm_group_type_add(unit, BCMECMP_GRP_TYPE_LEVEL0));
            SHR_IF_ERR_EXIT(bcmecmp_member0_wb_itbm_group_type_add(unit));
        }
        if (ids->ecmp_level1_ltid != (uint32_t)BCMECMP_INVALID) {
            SHR_IF_ERR_EXIT(bcmecmp_wb_itbm_group_type_add(unit, BCMECMP_GRP_TYPE_LEVEL1));
            SHR_IF_ERR_EXIT(bcmecmp_member1_wb_itbm_group_type_add(unit));
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
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmecmp_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use, update_num_paths_max_paths = 0;
    uint32_t lb_mode_update = BCMECMP_LB_MODE_COUNT;
    uint32_t rh_entry_cnt_update = 0;
    uint32_t comp_id;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    bcmecmp_control_t * ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    bcmecmp_lt_field_attrs_t * lt_fattr = NULL;
    bcmecmp_grp_attr_t * grp_attr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n bcmecmp_lt_input_params_validate %u %u.\n"), sid, lt_entry->grp_type));

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /*
     * ECMP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied.
     */
    if (!BCMECMP_LT_FIELD_GET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID field value not specified.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Verify ECMP_ID is in valid range. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        && (lt_entry->ecmp_id <  BCMECMP_LT_MIN_ECMP_ID
                                        (unit, group, gtype)
            || lt_entry->ecmp_id > BCMECMP_LT_MAX_ECMP_ID
                                        (unit, group, gtype))) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is invalid.\n"),
                     lt_entry->ecmp_id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ids != NULL) {
        if ((sid == ids->ecmp_level1_ltid) ||
            (sid == ids->ecmp_wlevel1_ltid)) {
            lt_entry->ecmp_id +=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
        }
    }

    if (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, group, gtype))) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_elem_state_get(BCMECMP_TBL_ITBM_LIST
                (BCMECMP_TBL_PTR(unit, group, gtype)),
                SHR_ITBM_INVALID,
                lt_entry->ecmp_id,
                &in_use,
                &comp_id));
    } else {
        in_use = BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), lt_entry->ecmp_id);
    }

    lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl->ecmp_info.group[lt_entry->grp_type].lt_fattrs;
    grp_attr = (bcmecmp_grp_attr_t *)(ecmp_ctrl->ecmp_info.grp_arr)
                                                            + lt_entry->ecmp_id;

    /* Validate input fields based on the type of LT table operation. */
    switch (opcode) {
        case BCMIMM_ENTRY_INSERT:
            /* Verify specified ECMP group is not in use. */
            if (in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d exists.\n"),
                             lt_entry->ecmp_id));
                if (lt_entry->glt_sid == BCMECMP_GRP_LT_SID(unit,
                                                          lt_entry->ecmp_id)) {
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }

            /* Verify specified LB_MODE is valid value. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
                && (lt_entry->lb_mode > BCMECMP_LT_MAX_LB_MODE
                                                (unit, group, gtype)
                    || lt_entry->lb_mode < BCMECMP_LT_MIN_LB_MODE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d is invalid.\n"),
                             lt_entry->lb_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* NHOP_SORTED must be FALSE/0 for RH LB_MODE. */
            if (lt_entry->nhop_sorted
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NHOP_SORTED=%u is invalid.\n"),
                             lt_entry->nhop_sorted));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * MAX_PATHS value must be equal to the default MAX_PATHS value if
             * it is set for RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->max_paths != BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid for "
                                        "RESILIENT LB_MODE.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is in valid range. */
            if (!BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && (lt_entry->max_paths < BCMECMP_LT_MIN_PATHS
                                                (unit, group, gtype)
                    || lt_entry->max_paths > BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_paths & (lt_entry->max_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u not a power-of-two.\n"),
                             lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * NUM_PATHS value must be equal to the default NUM_PATHS value if
             * it is set for RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT
                && lt_entry->num_paths != BCMECMP_LT_MIN_NPATHS
                                                   (unit, group, gtype)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid for "
                                        "RESILIENT LB_MODE.\n"),
                             lt_entry->num_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && lt_entry->num_paths > BCMECMP_LT_MAX_NPATHS
                                            (unit, group, gtype)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                             lt_entry->num_paths,
                             BCMECMP_LT_MAX_NPATHS(unit, group, gtype)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified non-zero NUM_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->num_paths > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u exceeds MAX_PATHS=%u.\n"),
                            lt_entry->num_paths,
                            lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }


            /*
             * Check if LT entry belongs to an Overlay Group and it's Underlay
             * ECMP_ID field array is set.
             */
            if (bcmecmp_grp_is_overlay(gtype) &&
                ecmp_ctrl->ecmp_info.ovlay_memb_count) {
                /*Not check member count and range, refer to SDKLT-14824. */
            }else if (gtype == BCMECMP_GRP_TYPE_OVERLAY
                      && (BCMECMP_LT_FIELD_GET
                              (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)
                      || lt_entry->ecmp_nhop)) {
                /*
                 * Validate ECMP_UNDERLAY_IDs and verify NUM_PATHS count
                 * matches total underlay ECMP IDs supplied.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_lt_uecmp_ids_validate(unit, opcode, lt_entry));
            } else if (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID)) {
                /*
                 * Validate NHOP_IDs and verify NUM_PATHS count matches total
                 * next-hop IDs supplied.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_lt_nhops_validate(unit, opcode, lt_entry));
            }

            /*
             * RH_SIZE field if set for non-resilient hashing LB_MODE, the value
             * must be equal to the default value i.e. RH_SIZE_64.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)
                && lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_SIZE cannot be set for "
                                        "LB_MODE=%d.\n"),
                            lt_entry->lb_mode));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify RH_SIZE value is in valid range. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)
                && (lt_entry->rh_size_enc < BCMECMP_LT_MIN_RH_SIZE
                                                (unit, group, gtype)
                    || lt_entry->rh_size_enc > BCMECMP_LT_MAX_RH_SIZE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_SIZE=%d is invalid.\n"),
                             lt_entry->rh_size_enc));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * RH_NUM_PATHS field if set for non-resilient hashing LB_MODE, the value
             * must be equal to the default value.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)
                && lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_NUM_PATHS cannot be set for "
                                        "LB_MODE=%d.\n"),
                             lt_entry->lb_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Number of next-hops for RH load balancing must be less than or
             * equal to the RH group size value when RH_SIZE is less than
             * MAX_PATHS value.
             */
            if (lt_entry->rh_num_paths > lt_entry->rh_entries_cnt
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u > RH_SIZE=%u for "
                                        "LB_MODE=%u.\n"),
                             lt_entry->rh_num_paths,
                             lt_entry->rh_entries_cnt,
                             lt_entry->lb_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);

            }

            /* NUM_PATHS value must equal to the WEIGHTED_SIZE members count value. */
            if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths &&
                BCMECMP_GRP_IS_WEIGHTED(gtype)) {
                if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                   && lt_entry->num_paths
                   && lt_entry->weighted_size != lt_fattr->min_weighted_size
                   && lt_entry->num_paths !=
                          ecmp_ctrl->ecmp_drv->weight_size_to_max_paths
                               (lt_entry->weighted_size)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid.\n"),
                             lt_entry->num_paths));

                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths) {
                if (lt_entry->weighted_size != BCMECMP_WEIGHTED_SIZE_0_127
                    && lt_entry->weighted_size !=
                       bcmecmp_num_paths_convert_to_weighted_size(lt_entry->num_paths)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid.\n"),
                             lt_entry->num_paths));

                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            /* WEIGHT only applied to MEMBER_WEIGHT mode. */
            if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT)
                && lt_entry->weighted_mode != BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "WEIGHT cannot be set for "
                                        "WEIGHTED_MODE=%d.\n"),
                             lt_entry->weighted_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* WEIGHTED_SIZE_0_127 only applied to MEMBER_WEIGHT mode. */
            if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths &&
                BCMECMP_GRP_IS_WEIGHTED(gtype)) {
                if (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
                    && lt_entry->weighted_size == lt_fattr->min_weighted_size
                    && lt_entry->weighted_mode !=
                                           BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT){
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "WEIGHTED_SIZE_0_127 cannot"
                                            " be set for WEIGHTED_MODE=%d.\n"),
                                 lt_entry->weighted_mode));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
                && lt_entry->weighted_size == BCMECMP_WEIGHTED_SIZE_0_127
                && lt_entry->weighted_mode != BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "WEIGHTED_SIZE_0_127 cannot be set for "
                                        "WEIGHTED_MODE=%d.\n"),
                             lt_entry->weighted_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being updated must match the LT SID of the entry
             * stored during INSERT operation.
             */
            if (!in_use
                || (BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id) != sid)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use %d.\n"),
                             lt_entry->ecmp_id, BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_LB_MODE)) {
                lb_mode_update = lt_entry->lb_mode;
            } else {
                lb_mode_update = BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id);
            }

            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_RH_SIZE)) {
                rh_entry_cnt_update = lt_entry->rh_entries_cnt;
            } else {
                rh_entry_cnt_update = BCMECMP_GRP_RH_ENTRIES_CNT
                                                (unit, lt_entry->ecmp_id);
            }

            /*
             * MAX_PATHS value must be equal to the default MAX_PATHS value if
             * it is set for RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && lt_entry->max_paths != BCMECMP_LT_MAX_PATHS
                                              (unit, group, gtype)
                && (lb_mode_update == BCMECMP_LB_MODE_RESILIENT)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid for "
                                        "RESILIENT LB_MODE.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->num_paths > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u exceeds MAX_PATHS=%u.\n"),
                            lt_entry->num_paths,
                            lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS) &&
                    BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)) {
                if (lt_entry->max_paths >= lt_entry->num_paths) {
                    update_num_paths_max_paths = 1;
                }
            }

            /* Verify specified MAX_PATHS value is in valid range. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && (lt_entry->max_paths < BCMECMP_LT_MIN_PATHS
                                                (unit, group, gtype)
                    || lt_entry->max_paths > BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype)
                    || (lt_entry->max_paths <  BCMECMP_GRP_NUM_PATHS
                                            (unit, lt_entry->ecmp_id)
                        && !update_num_paths_max_paths))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_paths & (lt_entry->max_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u not a power-of-two.\n"),
                             lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * NUM_PATHS value must be equal to the default NUM_PATHS value if
             * it is set for RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths != 0
                && (lb_mode_update == BCMECMP_LB_MODE_RESILIENT)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid for "
                                        "RESILIENT LB_MODE.\n"),
                             lt_entry->num_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && (lt_entry->num_paths > BCMECMP_GRP_MAX_PATHS
                                            (unit, lt_entry->ecmp_id)
                    && !update_num_paths_max_paths)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                            lt_entry->num_paths,
                            BCMECMP_GRP_MAX_PATHS
                                (unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified non-zero NUM_PATHS value is a power-of-two
             * value for REGULAR and RANDOM LB_MODE groups.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && (lb_mode_update != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Check if it's overlay ECMP group and ECMP_NHOP is TRUE. */
            if (bcmecmp_grp_is_overlay(gtype) &&
                ecmp_ctrl->ecmp_info.ovlay_memb_count) {
                /*Not check member count and range, refer to SDKLT-14824. */
            } else if (gtype == BCMECMP_GRP_TYPE_OVERLAY
                       && (BCMECMP_LT_FIELD_GET
                               (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)
                       || (BCMECMP_LT_FIELD_GET
                               (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                           && lt_entry->ecmp_nhop)
                       || (!BCMECMP_LT_FIELD_GET
                               (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                           && BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id)))) {
                /*
                 * Validate ECMP_UNDERLAY_IDs and also verify NUM_PATHS count
                 * matches total underlay ECMP IDs supplied.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_lt_uecmp_ids_validate(unit, opcode, lt_entry));
            } else {
                /*
                 * Validate NHOP_IDs and also verify NUM_PATHS count matches
                 * total next-hop IDs supplied.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_lt_nhops_validate(unit, opcode, lt_entry));
            }

            /*
             * RH_NUM_PATHS value must be equal to the default RH_NUM_PATHS value if
             * it is set for non-RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)
                && lt_entry->rh_num_paths != 0
                && (lb_mode_update != BCMECMP_LB_MODE_RESILIENT)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_NUM_PATHS=%u is invalid.\n"),
                             lt_entry->rh_num_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /*
             * Number of next-hops for RH load balancing must be less than or
             * equal to the RH group size value when RH_SIZE is less than
             * MAX_PATHS value.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                                     BCMECMP_LT_FIELD_RH_NUM_PATHS)
                && lt_entry->rh_num_paths > rh_entry_cnt_update
                && lb_mode_update == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u greater than RH_SIZE=%u "
                                        ".\n"),
                             lt_entry->rh_num_paths,
                             BCMECMP_GRP_RH_ENTRIES_CNT
                                (unit, lt_entry->ecmp_id)));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* NUM_PATHS value must equal to the WEIGHTED_SIZE members count value. */
            if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths) {
                if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths) {
                    if (grp_attr->lb_mode != lt_fattr->min_weighted_size) {
                        if (lt_entry->num_paths != lt_entry->max_paths) {
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                        (BSL_META_U(unit,
                                                    "weight_size=%u, "
                                                    "NUM_PATHS=%u is invalid.\n"),
                                         grp_attr->lb_mode,
                                         lt_entry->num_paths));

                            SHR_ERR_EXIT(SHR_E_PARAM);
                        }
                    }
                } else {
                    /* Weighted size is stored in LB_MODE for WECMP. */
                    if (BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id)
                        != BCMECMP_WEIGHTED_SIZE_0_127) {
                        if (lt_entry->num_paths != lt_entry->max_paths) {
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "NUM_PATHS=%u is invalid.\n"),
                                 lt_entry->num_paths));

                            SHR_ERR_EXIT(SHR_E_PARAM);
                        }
                    }
                }
            }

            /* WEIGHTED_SIZE_0_127 only applied to MEMBER_WEIGHT mode. */
            if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths &&
                BCMECMP_GRP_IS_WEIGHTED(gtype)) {
                if (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
                    && lt_entry->weighted_size == lt_fattr->min_weighted_size
                    && BCMECMP_GRP_WEIGHTED_MODE(unit, lt_entry->ecmp_id) !=
                                           BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT){
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "module based WEIGHTED_SIZE cannot"
                                            " be set for WEIGHTED_MODE=%d.\n"),
                                 BCMECMP_GRP_WEIGHTED_MODE
                                     (unit, lt_entry->ecmp_id)));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
                && lt_entry->weighted_size == BCMECMP_WEIGHTED_SIZE_0_127
                && BCMECMP_GRP_WEIGHTED_MODE(unit, lt_entry->ecmp_id)
                   != BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "WEIGHTED_SIZE_0_127 cannot be set for "
                                        "WEIGHTED_MODE=%d.\n"),
                             lt_entry->weighted_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* WEIGHTED_SIZE_0_127 only applied to MEMBER_WEIGHT mode. */
            if (ecmp_ctrl->ecmp_drv->weight_size_to_max_paths &&
                BCMECMP_GRP_IS_WEIGHTED(gtype)) {
                if (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE)
                    && lt_entry->weighted_mode ==
                                        BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION
                    && BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                           lt_fattr->min_weighted_size){
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Member replication cannot"
                                            "be set for WEIGHTED_SIZE=%d.\n"),
                                 lt_fattr->min_weighted_size));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (BCMECMP_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE)
                && lt_entry->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION
                && BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id)
                   == BCMECMP_WEIGHTED_SIZE_0_127) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "WEIGHTED_SIZE_0_127 cannot be set for "
                                        "WEIGHTED_MODE=%d.\n"),
                             lt_entry->weighted_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMIMM_ENTRY_DELETE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being Deleted/Looked-up must match the LT SID of the
             * entry stored during INSERT operation.
             */
            if (!in_use
                || (BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id) != sid)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use.\n"),
                             lt_entry->ecmp_id));

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
 * \brief Update sorted ECMP member fields.
 *
 * update ECMP member info in IMM with sorted member array.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
int
bcmecmp_lt_member_update(int unit,
                             bcmecmp_lt_entry_t *lt_entry)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields, i;
    bcmecmp_grp_ltd_fields_t *fids = NULL;
    bcmecmp_grp_type_t grp_type = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    num_fields = lt_entry->num_paths + 1;

    SHR_IF_ERR_EXIT
        (bcmecmp_group_lt_fields_allocate(unit,
                                        &imm_fields,
                                        num_fields));
    flist = &imm_fields;

    grp_type = lt_entry->grp_type;
    fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                            (unit, group, grp_type);
    data_field = flist->field[0];
    data_field->id = fids->ecmp_id_fid;
    data_field->idx = 0;
    data_field->data = lt_entry->ecmp_id;

    if (BCMECMP_GRP_TYPE_OVERLAY == grp_type && lt_entry->ecmp_nhop) {
        /* fill IMM table data fields */
        for (i = 0; i < lt_entry->num_paths; i++) {
            data_field = flist->field[i+1];
            data_field->id = fids->uecmp_id_fid;
            data_field->idx = i;
            data_field->data = lt_entry->uecmp_id[i];
        }
    } else {
        /* fill IMM table data fields */
        for (i = 0; i < lt_entry->num_paths; i++) {
            data_field = flist->field[i+1];
            data_field->id = fids->nhop_id_fid;
            data_field->idx = i;
            data_field->data = lt_entry->nhop_id[i];
        }
    }

    SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, 0, lt_entry->glt_sid, &imm_fields));

exit:
    bcmecmp_group_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm ECMP LT fields.
 *
 * Get the current fields from imm ECMP LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
int
bcmecmp_lt_fields_lookup(int unit,
                         bcmecmp_lt_entry_t *lt_entry,
                         fn_lt_fields_fill_t cb)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields= 0, i;
    int rv = SHR_E_NONE;
    bcmecmp_grp_ltd_fields_t * fids = NULL;
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    int member_count = 1;
    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmecmp_group_lt_fields_allocate(unit,
                                          &key_fields,
                                          1));
    gtype = lt_entry->grp_type;
    fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                             (unit, group, gtype);
    SHR_NULL_CHECK(fids, SHR_E_INTERNAL);
    key_fields.field[0]->id   = fids->ecmp_id_fid;
    key_fields.field[0]->data = lt_entry->ecmp_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, lt_entry->glt_sid, &num_fields));

    member_count = 1; /* Default member count */
    if (BCMECMP_FNCALL_CHECK(unit, grp_member_fields_count_get)) {
        member_count = (BCMECMP_FNCALL_EXEC(unit,
                       grp_member_fields_count_get)(unit, lt_entry));
    }
    num_fields += member_count *
                  BCMECMP_GRP_MAX_PATHS(unit, lt_entry->ecmp_id);

    if (BCMECMP_GRP_IS_WEIGHTED(gtype)){
        /* Count WEIGHT array field. */
        num_fields += BCMECMP_LT_MAX_PATHS(unit, group, gtype);
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_group_lt_fields_allocate(unit,
                                        &imm_fields,
                                        num_fields));

    /* Lookup IMM table; error if entry not found */
    rv = bcmimm_entry_lookup(unit,
                             lt_entry->glt_sid,
                             &key_fields,
                             &imm_fields);
    if (rv == SHR_E_RESOURCE) {
        /*
         * In the case that some array elements are reset to default value after
         * MAX paths is decreasing, but they will still be returned by IMM. So
         * use BCMECMP_LT_MAX_NPATHS to lookup again when resource error.
         */
        /* Reset imm fields count for freeing mem. */
        imm_fields.count = num_fields;
        bcmecmp_group_lt_fields_free(unit, &imm_fields);

        num_fields -=
                (member_count * BCMECMP_GRP_MAX_PATHS(unit, lt_entry->ecmp_id));

        num_fields += member_count *
                          BCMECMP_LT_MAX_NPATHS(unit, group, gtype);

        if (BCMECMP_GRP_IS_WEIGHTED(gtype)){
            /* Count WEIGHT array field. */
            num_fields -= BCMECMP_LT_MAX_PATHS(unit, group, gtype);
            num_fields += BCMECMP_LT_MAX_NPATHS(unit, group, gtype);
        }

        if (BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype)
                                    != (uint32_t)BCMECMP_INVALID) {
            num_fields += member_count *
                              BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
        }

        SHR_IF_ERR_EXIT
            (bcmecmp_group_lt_fields_allocate(unit,
                                              &imm_fields,
                                              num_fields));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_lookup(unit,
                                 lt_entry->glt_sid,
                                 &key_fields,
                                 &imm_fields));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d contains %u fields.\n"),
                     lt_entry->ecmp_id,
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
    bcmecmp_group_lt_fields_free(unit, &key_fields);
    /* reset imm fields count for freeing mem */
    imm_fields.count = num_fields;
    bcmecmp_group_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*
 * Update value of a field from IMM fields list.
 */
int
bcmecmp_lt_field_update(int unit,
                        bcmltd_fields_t *list,
                        uint32_t fid,
                        uint64_t data)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < list->count; i++) {
        if (list->field[i]->id == fid) {
            list->field[i]->data = data;
            break;
        }
    }

    if (i == list->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm ECMP FLEX LT fields.
 *
 * Get the current fields from imm ECMP FLEX LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP FLEX LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
int
bcmecmp_flex_lt_fields_lookup(int unit,
                         bcmecmp_flex_lt_entry_t *lt_entry,
                         fn_lt_fields_fill_t cb)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields= 0, i;
    int rv = SHR_E_NONE;
    bcmecmp_grp_ltd_fields_t * fids = NULL;
    bcmecmp_member_dest_ltd_fields_t *member_dest_fids = NULL;
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    int member_count = 1;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmecmp_group_lt_fields_allocate(unit,
                                          &key_fields,
                                          1));
    gtype = lt_entry->grp_type;
    ids = drv_var->ids;
    if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
        member_dest_fids = (bcmecmp_member_dest_ltd_fields_t *)
                           BCMECMP_MEMBER0_INFO_FIDS(unit);
        SHR_NULL_CHECK(member_dest_fids, SHR_E_INTERNAL);
        key_fields.field[0]->id   = member_dest_fids->ecmp_id_fid;
    } else if (lt_entry->glt_sid == ids->ecmp_mlevel1_ltid) {
        member_dest_fids = (bcmecmp_member_dest_ltd_fields_t *)
                           BCMECMP_MEMBER1_INFO_FIDS(unit);
        SHR_NULL_CHECK(member_dest_fids, SHR_E_INTERNAL);
        key_fields.field[0]->id   = member_dest_fids->ecmp_id_fid;
    } else {
        fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                             (unit, group, gtype);
        SHR_NULL_CHECK(fids, SHR_E_INTERNAL);
        key_fields.field[0]->id   = fids->ecmp_id_fid;
    }


    key_fields.field[0]->data = lt_entry->ecmp_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, lt_entry->glt_sid, &num_fields));

    member_count = 1; /* Default member count */
    if (BCMECMP_FNCALL_CHECK(unit, grp_member_fields_count_get)) {
        member_count = (BCMECMP_FNCALL_EXEC(unit,
                       grp_member_fields_count_get)(unit, lt_entry));
    }

    if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
        num_fields += member_count *
                     BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, lt_entry->ecmp_id);
    } else if (lt_entry->glt_sid == ids->ecmp_mlevel1_ltid) {
        num_fields += member_count *
                     BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, lt_entry->ecmp_id);
    } else {
        num_fields += member_count *
                     BCMECMP_FLEX_GRP_MAX_PATHS(unit, lt_entry->ecmp_id);
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_group_lt_fields_allocate(unit,
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
         * use BCMECMP_LT_MAX_NPATHS to lookup again when resource error.
         */
        /* reset imm fields count for freeing mem */
        imm_fields.count = num_fields;
        bcmecmp_group_lt_fields_free(unit, &imm_fields);

        if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
            num_fields -= member_count *
                          BCMECMP_MEMBER0_GRP_NUM_PATHS(unit,
                                                            lt_entry->ecmp_id);
        } else if (lt_entry->glt_sid == ids->ecmp_mlevel1_ltid) {
            num_fields -= member_count *
                          BCMECMP_MEMBER1_GRP_NUM_PATHS(unit,
                                                            lt_entry->ecmp_id);
        } else {
            num_fields -= member_count *
                          BCMECMP_FLEX_GRP_MAX_PATHS(unit, lt_entry->ecmp_id);
        }

        member_count = 1; /* Default member count */
        if (BCMECMP_FNCALL_CHECK(unit, grp_member_fields_count_get)) {
            member_count = (BCMECMP_FNCALL_EXEC(unit,
                           grp_member_fields_count_get)(unit, lt_entry));
        }

        if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
            num_fields += member_count *
                          BCMECMP_MEMBER0_LT_MAX_NPATHS(unit);
        } else if (lt_entry->glt_sid == ids->ecmp_mlevel1_ltid) {
            num_fields += member_count *
                          BCMECMP_MEMBER1_LT_MAX_NPATHS(unit);
        } else {
            num_fields += member_count *
                          BCMECMP_LT_MAX_NPATHS(unit, group, gtype);
        }
        SHR_IF_ERR_EXIT
            (bcmecmp_group_lt_fields_allocate(unit,
                                              &imm_fields,
                                              num_fields));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_lookup(unit,
                                 lt_entry->glt_sid,
                                 &key_fields,
                                 &imm_fields));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d contains %u fields.\n"),
                     lt_entry->ecmp_id,
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
    bcmecmp_group_lt_fields_free(unit, &key_fields);
    /* reset imm fields count for freeing mem */
    imm_fields.count = num_fields;
    bcmecmp_group_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initalize bcmecmp_flex_lt_entry_t structure.
 *
 * Initializes bcmecmp_flex_lt_entry_t structure member variables to required initial
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LTD Source identifier.
 * \param [in] lt_entry Pointer to bcmecmp_flex_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmecmp_flex_lt_entry_t_init(
                        int unit,
                        bcmltd_sid_t sid,
                        bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info = NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);


    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_entry_t_init.\n")));

    member_info = drv_var->member0_info;
    if (sid == ids->ecmp_level0_ltid) {
        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL0;
        member_info = drv_var->member0_info;
    } else if (sid == ids->ecmp_level1_ltid) {
        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL1;
        member_info = drv_var->member1_info;
    } else if (sid == ids->ecmp_wlevel0_ltid) {
        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED;
        member_info = drv_var->wmember0_info;
    } else if ( sid == ids->ecmp_wlevel1_ltid) {
        lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED;
        member_info = drv_var->wmember1_info;
        lt_entry->weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION;
    }  else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(member_info, SHR_E_INTERNAL);

    /* Initialize LT source ID value. */
    lt_entry->glt_sid = sid;

    /* Operational argument. */
    lt_entry->op_arg = NULL;

    /* Initialize LT fields set bitmap to null. */
    SHR_BITCLR_RANGE(lt_entry->fbmp, 0, BCMECMP_FLAG_BMP_MAX_BIT);

    /* Initialize ECMP group identifier value to invalid. */
    lt_entry->ecmp_id = BCMECMP_INVALID;

    lt_entry->rh_mode = false;

    if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
        /* Initialize ECMP group default load balancing mode to Regular mode. */
        lt_entry->lb_mode = BCMECMP_FLEX_LB_MODE_REGULAR;
    } else {
        lt_entry->lb_mode = BCMECMP_FLEX_LB_MODE_COUNT;
    }

    for (i = 0; i < member_info->flds_count; i++) {
        /* Initialize total member fields parsed count to zero. */
        lt_entry->member_count[i] = 0;

        /* Initialize member array elements to invalid */
        if ((sid == ids->ecmp_level0_ltid) || (sid == ids->ecmp_wlevel0_ltid)) {
            sal_memset
                (lt_entry->member0_field[i], BCMECMP_INVALID, sizeof(lt_entry->member0_field[i]));
        } else {
            sal_memset
                (lt_entry->member1_field[i], BCMECMP_INVALID, sizeof(lt_entry->member1_field[i]));
        }
    }
    /*
     * Initialize weighted hashing group size value to device
     * minimum weighted hashing group size value.
     */
    lt_entry->weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_256;

    /* Initialize to device ECMP groups max_paths value. */
    if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(lt_entry->grp_type)) {
        lt_entry->max_paths = BCMECMP_LT_MAX_PATHS
                                    (unit, group, lt_entry->grp_type);
    } else {
        lt_entry->max_paths =
            bcmecmp_flex_weighted_size_convert_to_num_paths(lt_entry->weighted_size);
    }

    /* Initialize configured paths to zero. */
    lt_entry->num_paths = 0;
    /* Initialize Group Member table start index to invalid. */
    lt_entry->mstart_idx = BCMECMP_INVALID;
    /* Initialize imm event to lookup. */
    lt_entry->event = BCMIMM_ENTRY_LOOKUP;

    /* Initialize weight parameters to 0. */
    sal_memset(lt_entry->weight, BCMECMP_INVALID, sizeof(lt_entry->weight));
    lt_entry->weight_count = 0;
    sal_memset(lt_entry->weight_nhop_index, BCMECMP_INVALID, sizeof(lt_entry->weight_nhop_index));
    lt_entry->weight_nhop_index_count = 0;

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
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmecmp_flex_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use, update_num_paths_max_paths = 0;
    uint32_t comp_id;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    LOG_VERBOSE(BSL_LOG_MODULE,
           (BSL_META_U(unit,
                       "\n bcmecmp_flex_lt_input_params_validate %u %u.\n"),
                       sid, lt_entry->grp_type));

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /*
     * ECMP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied.
     */
    if (!BCMECMP_FLEX_LT_FIELD_GET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID field value not specified.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Verify ECMP_ID is in valid range. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        && (lt_entry->ecmp_id <  BCMECMP_LT_MIN_ECMP_ID
                                        (unit, group, gtype)
            || lt_entry->ecmp_id > BCMECMP_LT_MAX_ECMP_ID
                                        (unit, group, gtype))) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is invalid.\n"),
                     lt_entry->ecmp_id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ids != NULL) {
        if ((sid == ids->ecmp_level1_ltid) ||
            (sid == ids->ecmp_wlevel1_ltid)) {
            lt_entry->ecmp_id +=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        }
    }

    SHR_IF_ERR_EXIT
        (shr_itbm_list_elem_state_get(BCMECMP_TBL_ITBM_LIST
            (BCMECMP_TBL_PTR(unit, group, gtype)),
            SHR_ITBM_INVALID,
            lt_entry->ecmp_id,
            &in_use,
            &comp_id));

    /* Validate input fields based on the type of LT table operation. */
    switch (opcode) {
        case BCMIMM_ENTRY_INSERT:
            /* Verify specified ECMP group is not in use. */
            if (in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d exists.\n"),
                             lt_entry->ecmp_id));
                if (lt_entry->glt_sid == BCMECMP_FLEX_GRP_LT_SID(unit,
                                                          lt_entry->ecmp_id)) {
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }

            /* Verify specified LB_MODE is valid value. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
                && (lt_entry->lb_mode > BCMECMP_LT_MAX_LB_MODE
                                                (unit, group, gtype)
                    || lt_entry->lb_mode < BCMECMP_LT_MIN_LB_MODE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d is invalid.\n"),
                             lt_entry->lb_mode));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is in valid range. */
            if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->max_paths < BCMECMP_LT_MIN_PATHS
                                                (unit, group, gtype)
                    || lt_entry->max_paths > BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_paths & (lt_entry->max_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u not a power-of-two.\n"),
                             lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_FLEX_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && lt_entry->num_paths > BCMECMP_LT_MAX_NPATHS
                                            (unit, group, gtype)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                             lt_entry->num_paths,
                             BCMECMP_LT_MAX_NPATHS(unit, group, gtype)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified non-zero NUM_PATHS value is a power-of-two. */
            if (BCMECMP_FLEX_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* NUM_PATHS value must equal to the WEIGHTED_SIZE members count value. */
            if (BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths) {
                if (lt_entry->weighted_size !=
                       bcmecmp_flex_num_paths_convert_to_weighted_size(lt_entry->num_paths)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid.\n"),
                             lt_entry->num_paths));

                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->num_paths > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u exceeds MAX_PATHS=%u.\n"),
                            lt_entry->num_paths,
                            lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Validate Weight configuration */
            if (BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE)
                && (lt_entry->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)) {
                if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE)
                    && (lt_entry->rh_mode == TRUE)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RH not supported in WEIGHT mode\n")));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have weight and weight nhop configured. */
                if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT)
                      && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_NHOP))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have same weight and weight nhop counts. */
                if (lt_entry->weight_count != lt_entry->weight_nhop_index_count) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have same num_paths, weight and weight nhop counts. */
                if (lt_entry->weight_count != lt_entry->num_paths) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            break;

        case BCMIMM_ENTRY_UPDATE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being updated must match the LT SID of the entry
             * stored during INSERT operation.
             */
            if (!in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use %d.\n"),
                             lt_entry->ecmp_id, BCMECMP_FLEX_GRP_LT_SID(unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->num_paths > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u exceeds MAX_PATHS=%u.\n"),
                            lt_entry->num_paths,
                            lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (BCMECMP_FLEX_LT_FIELD_GET(lt_entry->fbmp,
                                          BCMECMP_LT_FIELD_MAX_PATHS) &&
                BCMECMP_FLEX_LT_FIELD_GET(lt_entry->fbmp,
                                          BCMECMP_LT_FIELD_NUM_PATHS)) {
                if (lt_entry->max_paths >= lt_entry->num_paths) {
                    update_num_paths_max_paths = 1;
                }
            }

            /* Verify specified MAX_PATHS value is in valid range. */
            if (BCMECMP_FLEX_LT_FIELD_GET(lt_entry->fbmp,
                                          BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->max_paths < BCMECMP_LT_MIN_PATHS
                                                (unit, group, gtype)
                    || lt_entry->max_paths > BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype)
                    || (lt_entry->max_paths <  BCMECMP_FLEX_GRP_NUM_PATHS
                                            (unit, lt_entry->ecmp_id)
                        && !update_num_paths_max_paths))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid.\n"),
                             lt_entry->max_paths));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_paths & (lt_entry->max_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u not a power-of-two.\n"),
                             lt_entry->max_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && (lt_entry->num_paths > BCMECMP_FLEX_GRP_MAX_PATHS
                                            (unit, lt_entry->ecmp_id)
                    && !update_num_paths_max_paths)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                            lt_entry->num_paths,
                            BCMECMP_FLEX_GRP_MAX_PATHS
                                (unit, lt_entry->ecmp_id)));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* NUM_PATHS value must equal to the WEIGHTED_SIZE members count value. */
            if (BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths) {
                /* Weighted size is stored in LB_MODE for WECMP. */
                if (lt_entry->num_paths != lt_entry->max_paths) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NUM_PATHS=%u is invalid.\n"),
                         lt_entry->num_paths));

                        SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }


            /*
             * Verify specified non-zero NUM_PATHS value is a power-of-two
             * value for REGULAR and RANDOM LB_MODE groups.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Validate Weight configuration */
            if (BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)
                && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE)
                && (lt_entry->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)) {
                if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE)
                    && (lt_entry->rh_mode == TRUE)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RH not supported in WEIGHT mode\n")));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have weight and weight nhop configured. */
                if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT)
                      && BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_NHOP))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have same weight and weight nhop counts. */
                if (lt_entry->weight_count != lt_entry->weight_nhop_index_count) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                /* Should have same num_paths, weight and weight nhop counts. */
                if (lt_entry->weight_count != lt_entry->num_paths) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            break;

        case BCMIMM_ENTRY_DELETE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being Deleted/Looked-up must match the LT SID of the
             * entry stored during INSERT operation.
             */
            if (!in_use) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use.\n"),
                             lt_entry->ecmp_id));

                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    exit:
        SHR_FUNC_EXIT();
}
