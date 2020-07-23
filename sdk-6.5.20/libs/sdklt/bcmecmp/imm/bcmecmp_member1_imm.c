/*! \file bcmecmp_member1_imm.c
 *
 * BCMECMP ECMP_MEMBER1t IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_member_dest.h>
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */

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
bcmecmp_member1_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmecmp_flex_lt_entry_t *lt_entry)
{
    const bcmecmp_drv_var_ids_t *ids = NULL;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_member_info_t *member_info = NULL;
    int i;
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_member1_lt_entry_t_init.\n")));


    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    if (ids != NULL) {
        member_info = drv_var->member1_info;
    }
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    SHR_NULL_CHECK(member_info, SHR_E_INTERNAL);

    if (lt_entry) {

        /* Initialize LT source ID value. */
        lt_entry->glt_sid = sid;

        /* Operational argument.. */
        lt_entry->op_arg = NULL;

        /* Initialize LT fields set bitmap to null. */
        SHR_BITSET_RANGE(lt_entry->fbmp, 0, BCMECMP_FLAG_BMP_MAX_BIT);

        /* Initialize ECMP group identifier value to invalid. */
        lt_entry->ecmp_id = BCMECMP_INVALID;

        /* Initialize num_paths value to minimum. */
        lt_entry->max_paths = BCMECMP_MEMBER1_LT_MIN_NPATHS(unit);
        lt_entry->num_paths = BCMECMP_MEMBER1_LT_MIN_NPATHS(unit);

        /* Initialize total members parsed count to zero. */
        lt_entry->member_count[0] = 0;
        lt_entry->member_count[1] = 0;
        lt_entry->member_count[2] = 0;
        lt_entry->member_count[3] = 0;

        lt_entry->rh_mode = false;

        /* Initialize Group Member table start index to invalid. */
        lt_entry->mstart_idx = BCMECMP_INVALID;

        /* Initialize Auto assignment of Group Member table start index. */
        lt_entry->mstart_auto = true;

        /* Initialize imm event to lookup. */
        lt_entry->event = BCMIMM_ENTRY_LOOKUP;

        for (i = 0; i < member_info->flds_count; i++) {
            /* Initialize total member fields parsed count to zero. */
            lt_entry->member_count[i] = 0;

            /* Initialize array elements to invalid */
            sal_memset
                (lt_entry->member1_field[i], BCMECMP_INVALID,
                 sizeof(lt_entry->member1_field[i]));
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
static int
bcmecmp_member1_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_flex_lt_entry_t *lt_entry)
{
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n input_params_validate %u.\n"), sid));

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /*
     * GRP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied.
     */
    if (!BCMECMP_LT_FIELD_GET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "GRP_ID field value not \
                                        specified.\n")));
    }

    /* Verify GRP_ID is in valid range. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        && (lt_entry->ecmp_id <  BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit)
            || lt_entry->ecmp_id > BCMECMP_MEMBER1_LT_MAX_ECMP_ID (unit))) {

        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "GRP_ID=%d is invalid.\n"),
                            lt_entry->ecmp_id));
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, lt_entry->ecmp_id, TRUE, 1, (bcmecmp_grp_type_t)0, &in_use, &comp_id));

    /* Validate input fields based on the type of LT table operation. */
    switch (opcode) {
        case BCMIMM_ENTRY_INSERT:
            /* Verify specified ECMP group is not in use. */
            if (in_use) {
                if (lt_entry->glt_sid == BCMECMP_MEMBER1_GRP_LT_SID(unit,
                                                          lt_entry->ecmp_id)) {
                    SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                                        (BSL_META_U(unit,
                                                    "GRP_ID=%d exists.\n"),
                                        lt_entry->ecmp_id));
                } else {
                    SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                        (BSL_META_U(unit,
                                                    "GRP_ID=%d exists.\n"),
                                        lt_entry->ecmp_id));
                }
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && lt_entry->num_paths > BCMECMP_MEMBER1_LT_MAX_NPATHS(unit)) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                    (BSL_META_U(unit,
                                                "NUM_PATHS=%u is invalid "
                                                "(Grp MAX_PATHS=%u).\n"),
                                    lt_entry->num_paths,
                                    BCMECMP_MEMBER1_LT_MAX_NPATHS(unit)));
            }

            break;

        case BCMIMM_ENTRY_UPDATE:
            /*
             * GRP_ID must be in use and logical table source identifier value
             * of the entry being updated must match the LT SID of the entry
             * stored during INSERT operation.
             */
            if (!in_use) {

                SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                    (BSL_META_U(unit,
                                                "GRP_ID=%d not in-use %d.\n"),
                                    lt_entry->ecmp_id,
                                    BCMECMP_MEMBER1_GRP_LT_SID(unit,
                                    lt_entry->ecmp_id)));
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && !lt_entry->num_paths) {

                SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                    (BSL_META_U(unit,
                                                "NUM_PATHS=%u is invalid "),
                                    lt_entry->num_paths));
            }

            break;

        case BCMIMM_ENTRY_DELETE:
            /*
             * GRP_ID must be in use and logical table source identifier value
             * of the entry being Deleted/Looked-up must match the LT SID of the
             * entry stored during INSERT operation.
             */
            if (!in_use) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                    (BSL_META_U(unit,
                                                "GRP_ID=%d not in-use.\n"),
                                    lt_entry->ecmp_id));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief update the num_paths of ecmp group.
 *
 * Update the ref-count of reserved entries in member table and member start
 * idx in group:
 * If free entries are available in member table, use them;
 * If free entries are not enough, defragment the groups and try again.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_num_paths_update(int unit,
                               bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;            /* Group ECMP_ID. */
    uint32_t new_num_paths;
    uint32_t old_num_paths;
    int old_mstart_idx;
    int rv = SHR_E_NONE;                  /* Return value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_num_paths_update.\n")));

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    new_num_paths = lt_entry->num_paths;
    old_num_paths = BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, ecmp_id);
    old_mstart_idx = BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

    if (new_num_paths < old_num_paths) {
        if (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))) {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_resize(BCMECMP_MEMBER1_TBL_ITBM_LIST
                    (BCMECMP_INFO_PTR(unit)),
                    old_num_paths,
                    SHR_ITBM_INVALID,
                    old_mstart_idx,
                    new_num_paths));
        }
        BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, ecmp_id) = new_num_paths;
        SHR_EXIT();
    }

    if (new_num_paths > old_num_paths) {
         /* Check if it is possible to increase max grp size in-place. */
        if (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))) {

            if (old_mstart_idx + (int)new_num_paths
                > BCMECMP_MEMBER1_TBL_SIZE(unit)) {
                rv = SHR_E_RESOURCE;
            } else {
                rv = shr_itbm_list_block_resize
                        (BCMECMP_MEMBER1_TBL_ITBM_LIST
                            (BCMECMP_INFO_PTR(unit)),
                         old_num_paths,
                         SHR_ITBM_INVALID,
                         old_mstart_idx,
                         new_num_paths);
            }
        }

        if (rv == SHR_E_FULL) {
            /*
             * Error code should be E_RESOURCE if there is insufficient
             * resource to resize.
             */
            rv = SHR_E_RESOURCE;
        }
        SHR_IF_ERR_EXIT(rv);

        BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, ecmp_id) = new_num_paths;
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_MEMBER1_DESTINATION LT entry fields parse routine.
 *
 * Parse ECMP_MEMBER1_DESTINATION logical table entry logical fields
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
bcmecmp_member1_lt_fields_parse(int unit,
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
                (BSL_META_U(unit,
                            "bcmecmp_lt_fields_parse:LT_SID=%d.\n"),
                sid));

    gen_field = in;
    while (gen_field) {
        SHR_IF_ERR_EXIT
          (BCMECMP_FNCALL_EXEC(unit, grp_member_dest_lt_fields_fill)(unit, \
                               gen_field, lt_entry));
        gen_field = gen_field->next;
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add an ECMP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs ECMP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
int
bcmecmp_member1_add(int unit,
                  bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint16_t grp_id = BCMECMP_MEMBER_DEST_INVALID; /* Group identifier. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    uint32_t num_paths = 0;
    int rv = SHR_E_NONE;
    uint32_t bucket = SHR_ITBM_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_add.\n")));
    /* Get ECMP group identifier value. */
    grp_id = lt_entry->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, grp_id, TRUE, 1, 0, &in_use, &comp_id));

    if (in_use) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                            (BSL_META_U(unit,
                                        "GRP_ID=%d already Exists.\n"),
                                        grp_id));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "GRP_ID=%d is New.\n"), grp_id));

        /*
         * Initialize number of entries to be reserved in member table
         */
        num_paths = lt_entry->num_paths;


        /* Get free base index from member table. */
        if (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR((unit)))) {
            uint32_t free_idx;
            if (lt_entry->mstart_auto) {
                rv = shr_itbm_list_block_alloc
                        (BCMECMP_MEMBER1_TBL_ITBM_LIST
                            (BCMECMP_INFO_PTR((unit))),
                         lt_entry->num_paths,
                         &bucket,
                         &free_idx);
                lt_entry->mstart_idx = free_idx;
            } else {
                if (lt_entry->mstart_idx + lt_entry->num_paths - 1 >
                    bcmecmp_flex_mbr_tbl_max(unit, BCMECMP_GRP_TYPE_LEVEL1)) {
                    rv = SHR_E_RESOURCE;
                    SHR_ERR_EXIT(rv);
                }

                rv = shr_itbm_list_block_alloc_id
                        (BCMECMP_MEMBER1_TBL_ITBM_LIST
                            (BCMECMP_INFO_PTR((unit))),
                         lt_entry->num_paths,
                         bucket,
                         lt_entry->mstart_idx);
                if (SHR_E_EXISTS == rv) {
                    /* When the requested block is in use,
                       ITBM returns E_EXISTS */
                    rv = SHR_E_RESOURCE;
                }
            }

            if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {
                SHR_ERR_EXIT(rv);
            }
        }

        if (SHR_E_NONE != rv) {
            SHR_ERR_EXIT(rv);
        }

        /* Update allocated ECMP Group member table Base Index information. */


        /* Install ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));

        /* Store ECMP Group information in Per-Group Info Array. */
        BCMECMP_MEMBER1_GRP_LT_SID(unit, grp_id) = lt_entry->glt_sid;
        BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) = num_paths;
        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id) =
                                                       lt_entry->mstart_idx;
        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO(unit, grp_id) =
                                                       lt_entry->mstart_auto;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "[GRP_ID=%d]: mstart_idx=%d.\n"),
                    grp_id,
                    BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id)));

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add: GRP_ID=%d "
                              "SID[%d] != [%d].\n"),
                   grp_id,
                   BCMECMP_MEMBER1_GRP_LT_SID(unit, grp_id),
                   lt_entry->glt_sid));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Update installed ECMP logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
int
bcmecmp_member1_update(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint16_t grp_id = BCMECMP_MEMBER_DEST_INVALID; /* Group identifier. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    uint32_t num_paths = 0, new_num_paths = 0;
    bcmecmp_flex_lt_entry_t *cur_lt_entry;
    uint32_t new_mstart_idx = 0;
    uint32_t bucket = SHR_ITBM_INVALID;
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_member1_update.\n")));

    /* Get ECMP group identifier value. */
    grp_id = lt_entry->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, grp_id, TRUE, 1, 0, &in_use, &comp_id));

    /* Verify the group is in use. */
    if (!in_use) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                            (BSL_META_U(unit,
                                        "GRP_ID=%d not found.\n"), grp_id));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "GRP_ID=%d in use.\n"), grp_id));

    /* Get logical table entry buffer pointer. */
    cur_lt_entry = BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit);

    SHR_IF_ERR_EXIT
        (bcmecmp_flex_member_fields_fill(unit, cur_lt_entry, 1,
                                          lt_entry, TRUE));

    /*
     * Use stored group mstart_idx value for update operation if it has not
     * been supplied.
     */
    if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX))) {
        lt_entry->mstart_idx = cur_lt_entry->mstart_idx;
    }

    /*
     * Use stored group mstart_auto value for update operation if it has not
     * been supplied.
     */
    if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX_AUTO))) {
        lt_entry->mstart_auto = cur_lt_entry->mstart_auto;
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id);
        lt_entry->max_paths = lt_entry->num_paths;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    if (lt_entry->mstart_auto == false) {
        if (lt_entry->mstart_idx + lt_entry->num_paths - 1 >
            bcmecmp_flex_mbr_tbl_max(unit, BCMECMP_GRP_TYPE_LEVEL1)) {
            rv = SHR_E_RESOURCE;
            SHR_ERR_EXIT(rv);
        }
    }

    if ((lt_entry->mstart_auto == false) &&
         (lt_entry->mstart_idx != BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX
                                  (unit, grp_id))) {
            /*Move the ECMP group*/
            rv = shr_itbm_list_block_alloc_id
                    (BCMECMP_MEMBER1_TBL_ITBM_LIST
                        (BCMECMP_INFO_PTR((unit))),
                     lt_entry->num_paths,
                     bucket,
                     lt_entry->mstart_idx);

            if (SHR_E_EXISTS == rv) {
                /* When the requested block is in use,
                   ITBM returns E_EXISTS */
                rv = SHR_E_RESOURCE;
            }
            SHR_IF_ERR_EXIT(rv);
            new_num_paths = lt_entry->num_paths;
            new_mstart_idx = lt_entry->mstart_idx;
            lt_entry->mstart_idx =
                      BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id);
            lt_entry->num_paths =
                      BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id);

            /* Delete ECMP Group from hardware tables. */
            SHR_IF_ERR_EXIT
                (BCMECMP_FNCALL_EXEC(unit, grp_del)(unit, lt_entry));

           if (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))) {
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_block_free(BCMECMP_MEMBER1_TBL_ITBM_LIST
                        (BCMECMP_INFO_PTR(unit)),
                        lt_entry->num_paths,
                        SHR_ITBM_INVALID,
                        lt_entry->mstart_idx));
           }
           lt_entry->mstart_idx =
                BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id) =
                                                               new_mstart_idx;
           lt_entry->num_paths =
                BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) = new_num_paths;
    }

    if (lt_entry->num_paths != BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id)) {
            /*resize the ECMP group*/
            SHR_IF_ERR_EXIT(bcmecmp_group_num_paths_update(unit, lt_entry));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX
                                (unit, grp_id);

    num_paths = lt_entry->num_paths;

    /* Install ECMP Group in hardware tables. */
    SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));

    BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) = num_paths;

    BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO(unit, grp_id) =
                                             lt_entry->mstart_auto;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[GID=%d]: Num_Paths=%u.\n"),
                grp_id,
                num_paths));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP Group logical table entry from hardware tables.
 *
 * Delete an ECMP Group logical table entry from hardware tables. ECMP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_member1_delete(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint16_t grp_id = BCMECMP_INVALID; /* Group identifier. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_memb0_delete.\n")));

    /* Get ECMP ID value. */
    grp_id = lt_entry->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, grp_id, TRUE, 1, 0, &in_use, &comp_id));

    /* Check and confirm ECMP_ID exists prior to delete operation. */
    if (!in_use) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                            (BSL_META_U(unit,
                                        "GRP_ID=%d not found.\n"), grp_id));
    }

    /* Delete ECMP Group from hardware tables. */
    SHR_IF_ERR_EXIT
        (BCMECMP_FNCALL_EXEC(unit, grp_del)(unit, lt_entry));

    if (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free(BCMECMP_MEMBER1_TBL_ITBM_LIST
                (BCMECMP_INFO_PTR(unit)),
                BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id),
                SHR_ITBM_INVALID,
                BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id)));
    }

    /* Clear/initialize ECMP Group information in Per-Group Info Array. */
    BCMECMP_MEMBER1_GRP_LT_SID(unit, grp_id) = BCMLTD_SID_INVALID;
    BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id) = BCMECMP_INVALID;
    BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO(unit, grp_id) = 1; /*Default */
    BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) = 0;

    exit:
        SHR_FUNC_EXIT();
}

static int
bcmecmp_member1_field_list_get(int unit,
                     const bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint64_t *val)
{
    size_t count;
    for (count = 0; count < flist->count; count++) {
        if (flist->field[count]->id == fid) {
            *val = flist->field[count]->data;
            return SHR_E_NONE;
        }
    }
    return SHR_E_NOT_FOUND;
}

static int
bcmecmp_member1_field_list_set(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t val)
{
    size_t count;

    SHR_FUNC_ENTER(unit);

    for (count = 0; count < flist->count; count++) {
        if ((flist->field[count]->id == fid) &&
            (flist->field[count]->idx == idx)) {
            flist->field[count]->data = val;
            SHR_EXIT();
        }
    }
    if (!flist->field[count]) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
    flist->field[count]->id = fid;
    flist->field[count]->data = val;
    flist->field[count]->idx = idx;
    flist->count++;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an ECMP Group logical table entry data from hardware tables.
 *
 * Get an ECMP Group logical table entry data from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_member1_get(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out)
{
    bcmecmp_member_dest_ltd_fields_t * fids = NULL;
    uint64_t grp_id = 0, val;
    const bcmltd_fields_t *key_fields = in;

    SHR_FUNC_ENTER(unit);

    fids = (bcmecmp_member_dest_ltd_fields_t *)BCMECMP_MEMBER1_INFO_FIDS
                                             (unit);
    SHR_NULL_CHECK(fids, SHR_E_INTERNAL);

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    bcmecmp_member1_field_list_get(unit, key_fields,  fids->ecmp_id_fid, &(grp_id));

    val = BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id);

    SHR_IF_ERR_EXIT
        (bcmecmp_member1_field_list_set(unit, out, fids->base_index_oper_fid, 0, val));

    val = BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO(unit, grp_id);

    SHR_IF_ERR_EXIT
        (bcmecmp_member1_field_list_set(unit, out, fids->base_index_auto_fid, 0, val));

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
bcmecmp_member1_validate(int unit,
                               bcmltd_sid_t sid,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context)
{
    bcmecmp_flex_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data. */
    bcmecmp_flex_lt_entry_t *cur_lt_entry;

    /* current ECMP LT entry data. */
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_member1_validate:SID=%d.\n"),
              sid));
    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    if (event == BCMIMM_ENTRY_LOOKUP) {
        SHR_EXIT();
    }

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_FLEX_LT_ENT_PTR(unit);

    /* Initialize Logical Table Entry buffer. */
    SHR_IF_ERR_EXIT
        (bcmecmp_member1_lt_entry_t_init(unit, sid, lt_entry));
    lt_entry->event = event;

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_EXIT
        (bcmecmp_member1_lt_fields_parse(unit, key, sid, lt_entry));

    if (((event == BCMIMM_ENTRY_UPDATE) ||
        (event == BCMIMM_ENTRY_INSERT)) && (data != NULL)) {
        SHR_IF_ERR_EXIT
        (bcmecmp_member1_lt_fields_parse(unit, data, sid, lt_entry));
    }

    /*
     * GRP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied or invalid.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        || (lt_entry->ecmp_id <  BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit))
        || (lt_entry->ecmp_id > BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit))) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "GRP_ID field value error.\n")));
    }

    /* Validate input parameter values. */
    SHR_IF_ERR_EXIT
        (bcmecmp_member1_lt_input_params_validate(unit, event, sid, lt_entry));

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Get logical table entry buffer pointer. */
        cur_lt_entry = BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit);

        /* Initialize Logical Table Entry buffer. */
        SHR_IF_ERR_EXIT
            (bcmecmp_member1_lt_entry_t_init(unit, sid, cur_lt_entry));

        cur_lt_entry->event = event;

        /* Parse the input fields and retrieve the field values. */
        SHR_IF_ERR_EXIT
            (bcmecmp_member1_lt_fields_parse(unit,
                                                 key,
                                                 sid,
                                                 cur_lt_entry));
        /* Get current fields */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_lt_fields_lookup(unit,
                                           cur_lt_entry,
                                           BCMECMP_FNCALL(unit,
                                           grp_member_dest_lt_fields_fill)));
    }

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
bcmecmp_member1_stage(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_op_arg_t *op_arg,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{

    bcmecmp_flex_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp__member1_stage: LT_SID=%d.\n"),
              sid));
    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);


    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_FLEX_LT_ENT_PTR(unit);
    lt_entry->op_arg = op_arg;

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT(bcmecmp_member1_add(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT(bcmecmp_member1_update(unit, lt_entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmecmp_member1_delete(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMECMP_MEMBER1_GRP_TRANS_ID(unit, lt_entry->ecmp_id) = op_arg->trans_id;

    /*
     * Check if atomic transactions support is enabled for this BCM unit i.e.
     * feature_conf->dis_atomic_trans == 0 and set staging flags if support is
     * enabled.
     */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        /*
         * Save the Group's Trasanction ID for use during commit/abort
         * operation.
         */
        BCMECMP_MEMBER1_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = op_arg->trans_id;
    }

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
bcmecmp_member1_commit(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context)
{
    uint16_t grp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_member1_commit: LT_SID=%d.\n"),
              sid));

    BCMECMP_INIT(unit);
    BCMECMP_LOCK(unit);

    for (grp_id = BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit);
         grp_id <= BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit);
         grp_id++ ) {
        if (!BCMECMP_MEMBER1_TBL_ENT_STAGED_GET(BCMECMP_INFO_PTR(unit),
                                        grp_id)
            || (trans_id != BCMECMP_MEMBER1_GRP_TRANS_ID(unit, grp_id))) {
            continue;
        }

        BCMECMP_MEMBER1_TBL_ENT_STAGED_CLEAR(BCMECMP_INFO_PTR( unit), grp_id);
    }

    /*
     * If atomic transaction support is disabled for this BCM unit, nothing to
     * do in this function return success.
     */
    if (!BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_EXIT();
    }


    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_MEMBER1_TBL_PTR(unit, member);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (grp_id = BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit);
         grp_id <= BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit);
         grp_id++ ) {

        if (!BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_GET(BCMECMP_INFO_PTR(unit),
                                                    grp_id)
            || (trans_id != BCMECMP_MEMBER1_GRP_TRANS_ID_BK(unit, grp_id))) {
            continue;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: grp_id=%d trans_id/bk_id=%u/%u"
                                " sid=%u.\n"),
                     grp_id,
                     trans_id,
                     BCMECMP_MEMBER1_GRP_TRANS_ID_BK(unit, grp_id),
                                                               sid));


        BCMECMP_MEMBER1_GRP_LT_SID_BK(unit, grp_id) = BCMECMP_MEMBER1_GRP_LT_SID(unit,
                                                                  grp_id);
        BCMECMP_MEMBER1_GRP_NUM_PATHS_BK(unit, grp_id) = BCMECMP_MEMBER1_GRP_NUM_PATHS(unit,
                                                                       grp_id);

        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX_BK(unit, grp_id) =
                        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id);

        tbl_size = (size_t)BCMECMP_MEMBER1_TBL_SIZE(unit);
        sal_memcpy(tbl_ptr->ent_bk_arr, tbl_ptr->ent_arr,
                   (sizeof(bcmecmp_flex_hw_entry_attr_t) * tbl_size));

        BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_INFO_PTR(unit),
                                                    grp_id);
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
 * \brief transaction abort
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
bcmecmp_member1_abort(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            void *context)
{
    uint16_t grp_id = BCMECMP_MEMBER_DEST_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_member1_abort: LT_SID=%d.\n"),
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

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_MEMBER1_TBL_PTR(unit, member);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (grp_id = BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit);
         grp_id <= BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit);
         grp_id++ ) {

        if (!BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_GET(BCMECMP_INFO_PTR(unit),
                                                       grp_id)
            || (trans_id != BCMECMP_MEMBER1_GRP_TRANS_ID_BK(unit, grp_id))
            ) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: grp_id=%d trans_id/bk_id=%u/%u"
                                " sid=%u.\n"),
                     grp_id,
                     trans_id,
                     BCMECMP_GRP_TRANS_ID_BK(unit, grp_id),
                     sid));


        BCMECMP_MEMBER1_GRP_LT_SID(unit, grp_id) = BCMECMP_MEMBER1_GRP_LT_SID_BK(
                                                             unit, grp_id);
        BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) =
                                      BCMECMP_MEMBER1_GRP_NUM_PATHS_BK(unit, grp_id);
        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit, grp_id) =
                BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX_BK(unit, grp_id);

        tbl_size = (size_t)BCMECMP_MEMBER1_TBL_SIZE(unit);
        sal_memcpy(tbl_ptr->ent_arr, tbl_ptr->ent_bk_arr,
                   (sizeof(bcmecmp_flex_hw_entry_attr_t) * tbl_size));

        BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_INFO_PTR(unit), grp_id);
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
 * \brief ECMP_MEMBER1 In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to ECMP_OVERLAY logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = bcmecmp_member1_validate,

    /*! Extended staging function. */
    .op_stage = bcmecmp_member1_stage,

    /*! Commit function. */
    .commit = bcmecmp_member1_commit,

    /*! Abort function. */
    .abort = bcmecmp_member1_abort,

    .lookup = bcmecmp_member1_get
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
bcmecmp_member1_imm_register(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for ECMP_MEMBER1t here.
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
