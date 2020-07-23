/*! \file bcm56780_a0_ecmp.c
 *
 * This file implements BCM56780_A0 device specific ECMP functions.
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
#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <shr/shr_ha.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_member_dest.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmecmp/bcmecmp_member_weight.h>
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP
#define MEMBER0_MAX_FIELD                    4
#define MEMBER1_MAX_FIELD                    3
#define MEMBER0                              0
#define MEMBER1                              1
#define BCM56780_MEMBER_NHOP_ARRAY_SIZE      16384
#define BCM56780_MEMBER_WEIGHT_QUANT_FACTOR  (1 << 16)
#define BCM56780_MEMBER_NHI_A_MIN_BIT        0
#define BCM56780_MEMBER_NHI_A_MIN_SIZE       16
#define BCM56780_MEMBER_NHI_B_MIN_BIT        16
#define BCM56780_MEMBER_NHI_B_MIN_SIZE       16
#define BCM56780_MEMBER_WEIGHT_MIN_BIT       32
#define BCM56780_MEMBER_WEIGHT_MIN_SIZE      16

static const uint32_t fbmp_mem0_id[MEMBER0_MAX_FIELD] = {
      BCMECMP_LT_FIELD_MEMBER0_FIELD0,
      BCMECMP_LT_FIELD_MEMBER0_FIELD1,
      BCMECMP_LT_FIELD_MEMBER0_FIELD2,
      BCMECMP_LT_FIELD_MEMBER0_FIELD3,
};

static const uint32_t fbmp_mem1_id[MEMBER1_MAX_FIELD] = {
      BCMECMP_LT_FIELD_MEMBER1_FIELD0,
      BCMECMP_LT_FIELD_MEMBER1_FIELD1,
      BCMECMP_LT_FIELD_MEMBER1_FIELD2,
};
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initalize a bcmecmp_pt_op_info_t structure memory.
 *
 * \param [in] pt_op_info Pointer to bcmecmp_pt_op_info_t structure.
 *
 * \return Nothing.
 */
static void
bcm56780_a0_pt_op_info_t_init(bcmecmp_pt_op_info_t *pt_op_info)
{
    /* Initialize structure member variables. */
    if (pt_op_info) {
        pt_op_info->drd_sid = INVALIDm;
        pt_op_info->req_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->rsp_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = BCMECMP_INVALID;
        pt_op_info->dyn_info.tbl_inst = BCMECMP_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
        pt_op_info->dma = FALSE;
        pt_op_info->ecount = 0;
        pt_op_info->op_arg = NULL;
    }

    return;
}

/*!
 * \brief ECMP_LEVEL0_WEIGHTED LT entry fields fill routine.
 *
 * Parse ECMP_LEVEL0_WEIGHTED logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_level0_weighted_group_lt_fields_fill(int unit,
                                             const bcmltd_field_t *in,
                                             bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    uint16_t i = 0;
    bool match = FALSE;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    member_info = drv_var->wmember0_info;

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (fid == ids->ecmp_wlevel0_ecmp_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
        lt_entry->ecmp_id = (bcmecmp_id_t) fval;
    } else if (fid == ids->ecmp_wlevel0_wt_size_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE);
        lt_entry->weighted_size = (uint32_t) fval;
        lt_entry->lb_mode = (uint32_t) fval;
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
        lt_entry->max_paths = bcmecmp_flex_weighted_size_convert_to_num_paths(fval);
    } else if (fid == ids->ecmp_wlevel0_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
        lt_entry->num_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel0_rh_mode_id) {
        lt_entry->rh_mode = ((uint32_t) fval)?TRUE:FALSE;
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE);
    } else if (fid == ids->ecmp_wlevel0_rh_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS);
        lt_entry->rh_num_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel0_rh_rand_seed_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED);
        lt_entry->rh_rand_seed = (uint32_t) fval;
    } else {
        for (i = 0; i < member_info->flds_count; i++) {
            match = FALSE;
            if (fid == drv_var->wflds_mem0[i].grp_fld) {
                lt_entry->member0_field[i][idx] = (int) fval;
                lt_entry->member_count[i] += 1;
                BCMECMP_LT_FIELD_SET
                        (lt_entry->fbmp, fbmp_mem0_id[i]);
                match = TRUE;
                break;
            }
        }
        if (match == FALSE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_MEMBER_DESTINATION LT entry fields fill routine.
 *
 * Parse ECMP_MEMBER_DESTINATION logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_flex_member_dest_lt_fields_fill(int unit,
                                    const void *in_fld,
                                    void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_flex_lt_entry_t *lt_entry = ltentry;
    uint16_t i = 0;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
        member_info = drv_var->dest_member0_info;
    } else {
        member_info = drv_var->dest_member1_info;
    }
    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (lt_entry->glt_sid == ids->ecmp_mlevel0_ltid) {
        if (fid == ids->ecmp_mlevel0_group_id) {
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (uint16_t) fval;
            lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL0;
        } else if (fid == ids->ecmp_mlevel0_num_entries_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            lt_entry->max_paths = (uint32_t) fval;
        } else if (fid == ids->ecmp_mlevel0_base_index_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX);
            lt_entry->mstart_idx = (uint32_t) fval;
        } else if (fid == ids->ecmp_mlevel0_base_index_auto_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX_AUTO);
                lt_entry->mstart_auto = (uint32_t)fval;
        } else {
            for (i = 0; i < member_info->flds_count; i++) {
                if (fid == drv_var->dest_flds_mem0[i].grp_fld) {
                    lt_entry->member0_field[i][idx] = (int) fval;
                    lt_entry->member_count[i] += 1;
                    BCMECMP_LT_FIELD_SET
                            (lt_entry->fbmp, fbmp_mem0_id[i]);
                    break;
                }
            }
        }
    } else {
        if (fid == ids->ecmp_mlevel1_group_id) {
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (uint16_t) fval;
            lt_entry->grp_type = BCMECMP_GRP_TYPE_LEVEL1;
        } else if (fid == ids->ecmp_mlevel1_num_entries_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            lt_entry->max_paths = (uint32_t) fval;
        } else if (fid == ids->ecmp_mlevel1_base_index_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX);
            lt_entry->mstart_idx = (uint32_t)fval;
        } else if (fid == ids->ecmp_mlevel1_base_index_auto_id) {
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_BASE_INDEX_AUTO);
                lt_entry->mstart_auto = (uint32_t)fval;
        } else {
            for (i = 0; i < member_info->flds_count; i++) {
                if (fid == drv_var->dest_flds_mem1[i].grp_fld) {
                    lt_entry->member1_field[i][idx] = (int) fval;
                    lt_entry->member_count[i] += 1;
                    BCMECMP_LT_FIELD_SET
                            (lt_entry->fbmp, fbmp_mem1_id[i]);
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief ECMP_LEVEL0 LT entry fields fill routine.
 *
 * Parse ECMP_LEVEL0 logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_level0_group_lt_fields_fill(int unit,
                                    const void *in_fld,
                                    void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_flex_lt_entry_t *lt_entry = ltentry;
    uint16_t i = 0;
    bool match = FALSE;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    member_info = drv_var->member0_info;

    if (lt_entry->glt_sid == ids->ecmp_wlevel0_ltid) {
        SHR_IF_ERR_EXIT(
            bcm56780_a0_level0_weighted_group_lt_fields_fill(unit, in_fld, ltentry));
        SHR_EXIT();
    }

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (fid == ids->ecmp_level0_ecmp_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
        lt_entry->ecmp_id = (bcmecmp_id_t) fval;
    } else if (fid == ids->ecmp_level0_lb_mode_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
        lt_entry->lb_mode = (uint32_t) fval;
    } else if (fid == ids->ecmp_level0_max_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
        lt_entry->max_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_level0_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
        lt_entry->num_paths = (uint32_t) fval;
    } else {
        for (i = 0; i < member_info->flds_count; i++) {
            match = FALSE;
            if (fid == drv_var->flds_mem0[i].grp_fld) {
                lt_entry->member0_field[i][idx] = (int) fval;
                lt_entry->member_count[i] += 1;
                match = TRUE;
                BCMECMP_LT_FIELD_SET
                        (lt_entry->fbmp, fbmp_mem0_id[i]);
                break;
            }
        }
        if (match == FALSE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_LEVEL1_WEIGHTED LT entry fields fill routine.
 *
 * Parse ECMP_LEVEL1_WEIGHTED logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_level1_weighted_group_lt_fields_fill(int unit,
                                             const bcmltd_field_t *in_fld,
                                             bcmecmp_flex_lt_entry_t *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_flex_lt_entry_t *lt_entry = ltentry;
    uint16_t i = 0;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    member_info = drv_var->wmember1_info;
    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (fid == ids->ecmp_wlevel1_ecmp_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
        lt_entry->ecmp_id = (bcmecmp_id_t)fval;
    } else if (fid == ids->ecmp_wlevel1_wt_size_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE);
        lt_entry->weighted_size = (uint32_t) fval;
        lt_entry->lb_mode = (uint32_t) fval;
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
        lt_entry->max_paths = bcmecmp_flex_weighted_size_convert_to_num_paths(fval);
    } else if (fid == ids->ecmp_wlevel1_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
        lt_entry->num_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel1_rh_mode_id) {
        lt_entry->rh_mode = ((uint32_t) fval)?TRUE:FALSE;
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE);
    } else if (fid == ids->ecmp_wlevel1_rh_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS);
        lt_entry->rh_num_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel1_rh_rand_seed_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED);
        lt_entry->rh_rand_seed = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel1_wt_mode_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE);
        lt_entry->weighted_mode = (uint32_t) fval;
    } else if (fid == ids->ecmp_wlevel1_wt_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT);
        lt_entry->weight[idx] = (uint32_t) fval;
        lt_entry->weight_count += 1;
    } else if (fid == ids->ecmp_wlevel1_wt_nhop_index_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_NHOP);
        lt_entry->weight_nhop_index[idx] = (uint32_t) fval;
        lt_entry->weight_nhop_index_count += 1;
    } else {
        for (i = 0; i < member_info->flds_count; i++) {
            if (fid == drv_var->wflds_mem1[i].grp_fld) {
                lt_entry->member1_field[i][idx] = (int) fval;
                lt_entry->member_count[i] += 1;
                BCMECMP_LT_FIELD_SET
                        (lt_entry->fbmp, fbmp_mem1_id[i]);
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_LEVEL1 LT entry fields fill routine.
 *
 * Parse ECMP_LEVEL1 logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_level1_group_lt_fields_fill(int unit,
                                    const void *in_fld,
                                    void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_flex_lt_entry_t *lt_entry = ltentry;
    uint16_t i = 0;
    bool match = FALSE;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    member_info = drv_var->member1_info;

    if (lt_entry->glt_sid == ids->ecmp_wlevel1_ltid) {
        SHR_IF_ERR_EXIT(
            bcm56780_a0_level1_weighted_group_lt_fields_fill(unit, in_fld, ltentry));
        SHR_EXIT();
    }

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (fid == ids->ecmp_level1_ecmp_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
        lt_entry->ecmp_id = (bcmecmp_id_t)fval;
    } else if (fid == ids->ecmp_level1_lb_mode_id) {
        BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
        lt_entry->lb_mode = (uint32_t) fval;
    } else if (fid == ids->ecmp_level1_max_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
        lt_entry->max_paths = (uint32_t) fval;
    } else if (fid == ids->ecmp_level1_num_paths_id) {
        BCMECMP_LT_FIELD_SET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
        lt_entry->num_paths = (uint32_t) fval;
    } else {
        for (i = 0; i < member_info->flds_count; i++) {
            match = FALSE;
            if (fid == drv_var->flds_mem1[i].grp_fld) {
                lt_entry->member1_field[i][idx] = (int) fval;
                /* Increment parsed NHOP_IDs count. */
                lt_entry->member_count[i] += 1;
                match = TRUE;
                BCMECMP_LT_FIELD_SET
                        (lt_entry->fbmp, fbmp_mem1_id[i]);
                break;
            }
        }
        if (match == FALSE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write an entry or entries to a table given.
 *
 * Write an entry or entries to a table given index or start index and number
 * of entries to write.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmecmp_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 * \retval SHR_E_INTERNAL Invalid entry count value when DMA is TRUE.
 */
static int
bcm56780_a0_ecmp_pt_write(int unit,
                          bcmecmp_pt_op_info_t *op_info,
                          uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read an entry or entries from a table given.
 *
 * Read an entry or entries from a table given index or start index and number
 * of entries to read.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmecmp_pt_op_info_t structure.
 * \param [out] buf Pointer to hardware table entry read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware table read operation unsuccessful.
 * \retval SHR_E_INTERNAL Invalid entry count value when DMA is TRUE.
 */
static int
bcm56780_a0_ecmp_pt_read(int unit,
                         bcmecmp_pt_op_info_t *op_info,
                         uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */
    uint32_t flags_0, flags_1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);
    flags_0 = (uint32_t) req_flags;
    flags_1 = (uint32_t) (req_flags >> 32);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));

    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initalize a bcmecmp_grp_drd_fields_t structure memory.
 *
 * \param [in] gdrd Pointer to bcmecmp_grp_drd_fields_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_grp_drd_fields_t_init(bcmecmp_grp_drd_fields_t *gdrd)
{
    if (gdrd) {
        sal_memset(gdrd, INVALIDf, sizeof(*gdrd));
    }
    return;
}

/*!
 * \brief Initalize a bcmecmp_lt_field_attrs_t structure memory.
 *
 * \param [in] fattrs Pointer to bcmecmp_lt_field_attrs_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_lt_field_attrs_t_init(bcmecmp_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMECMP_INVALID, sizeof(*fattrs));
    }
    return;
}

/*!
 * \brief Initalize a bcmecmp_lt_field_attrs_t structure memory.
 *
 * \param [in] fattrs Pointer to bcmecmp_lt_field_attrs_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_member_dest_lt_field_attrs_t_init(
                 bcmecmp_member_dest_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMECMP_INVALID, sizeof(*fattrs));
    }
    return;
}

/*!
 * \brief Initialize Trident4 ECMP tables DRD-SID and LTD-SID information.
 *
 * Initialize DRD-SID and LTD-SID details for ECMP Group, Member and Round-Robin
 * load balancing count tables.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value.
 */
static int
bcm56780_a0_ecmp_sids_init(int unit,
                           bcmecmp_drv_var_t *drv_var)
{
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member1_info;
    const bcmecmp_member_info_t *member0_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    member1_info = drv_var->member1_info;
    member0_info = drv_var->member0_info;

    BCMECMP_MODE(unit) = BCMECMP_MODE_HIER;

    /* Initialize device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = INVALIDr;

    /*
     * Pre-IFP ECMP stage is removed in TD4 initialize related Overlay
     * and Underlay tables DRD-SID values as invalid for this device.
     */
    BCMECMP_TBL_DRD_SID
        (unit, init_group,
         BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member,
         BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_group,
         BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member,
         BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;

    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, member,
         BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

    /* Initialize ECMP Level0 resolution tables DRD-SID, LTD_SID values. */
     BCMECMP_TBL_DRD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             member,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             member,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             member2,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             member2,
             BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             member2,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             member2,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = INVALIDm;



    /* Initialize ECMP Level1 resolution tables DRD-SID, LTD-SID values. */
    BCMECMP_TBL_DRD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;

    BCMECMP_TBL_DRD_SID
             (unit,
              member,
              BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_LTD_SID
             (unit,
              member,
              BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;

    BCMECMP_TBL_DRD_SID
             (unit,
              member2,
              BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_LTD_SID
             (unit,
              member2,
              BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;

    BCMECMP_TBL_DRD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
            (unit,
             group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;

    BCMECMP_TBL_DRD_SID
             (unit,
              member,
              BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
             (unit,
              member,
              BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;

    BCMECMP_TBL_DRD_SID
             (unit,
              member2,
              BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
             (unit,
              member2,
              BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = INVALIDm;

    if (ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) {
         /*
          * Initialize ECMP Level0 resolution tables
          * DRD-SID, LTD_SID values.
          */
         BCMECMP_TBL_DRD_SID
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0) =
             ECMP_GROUP_LEVEL0_GROUP_TABLEm;

         BCMECMP_TBL_LTD_SID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) =
            ids->ecmp_level0_ltid;
    }

    if (ids->ecmp_wlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
       /*
        * Initialize ECMP Level0 weighted resolution
        * tables DRD-SID, LTD-SID values.
        */
         BCMECMP_TBL_DRD_SID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) =
            ECMP_GROUP_LEVEL0_GROUP_TABLEm;
         BCMECMP_TBL_LTD_SID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) =
             ids->ecmp_wlevel0_ltid;
    }

    if (member0_info != NULL) {
        BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0)
        = member0_info->mbr_tbl[0];
        BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
        = member0_info->mbr_tbl[0];
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID) {
        /*
         * Initialize ECMP Level1 resolution tables
         * DRD-SID, LTD-SID values.
         */
        BCMECMP_TBL_DRD_SID
           (unit, group, BCMECMP_GRP_TYPE_LEVEL1) =
        ECMP_GROUP_LEVEL1_GROUP_TABLEm;
        BCMECMP_TBL_LTD_SID
           (unit, group, BCMECMP_GRP_TYPE_LEVEL1) =
           ids->ecmp_level1_ltid;
    }

    if (ids->ecmp_wlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        /*
         * Initialize ECMP Level1 weighted resolution
         * tables DRD-SID, LTD_SID values.
         */
        BCMECMP_TBL_DRD_SID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) =
        ECMP_GROUP_LEVEL1_GROUP_TABLEm;
        BCMECMP_TBL_LTD_SID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) =
            ids->ecmp_wlevel1_ltid;
    }

    if (member1_info != NULL) {
        BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1)
        = member1_info->mbr_tbl[0];
        BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
        = member1_info->mbr_tbl[0];
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP_LEVEL0 LT fields LRD and DRD IDs.
 *
 * Initialize ECMP_LEVEL0 single ECMP resolution LT fields LRD and DRD IDs
 * for use duing LT entry INSERT, LOOKUP, UPDATE and DELETE operations.
 *
 * \param [in] unit Unit number.
 * \param [in] map LT fields LRD mapping data.
 * \param [in] map Weighted LT fields LRD mapping data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Unexpected invalid input parameter.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_ecmp_olay_lt_flds_init(int unit,
                                   const bcmlrd_map_t *map, const bcmlrd_map_t *wmap)
{
    bcmecmp_grp_drd_fields_t *ogdrd_fields = NULL;  /* Group tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *olt_fids = NULL, *owlt_fids = NULL;   /* Overlay LT fields LTD ID. */
    bcmecmp_lt_field_attrs_t *ogrp_attrs = NULL, *owgrp_attrs = NULL; /* Overlay lt field attrs. */
    const bcmlrd_field_data_t *fdata = NULL;     /* LT field data map. */
    uint32_t idx;    /* Enum symbol array index. */
    uint32_t lb_val; /* LB mode value. */
    const char *const lb_name[BCMECMP_FLEX_LB_MODE_COUNT] =
                                               BCMECMP_FLEX_LB_MODE_NAME;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    if (drv_var == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "olay_fields_init")));

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL0) = NULL;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(ogdrd_fields, sizeof(*ogdrd_fields), "bcmecmpOlayGdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL0) = NULL;

    /* Initialize Group table entry fields DRD ID values. */
    bcmecmp_grp_drd_fields_t_init(ogdrd_fields);
    ogdrd_fields->lb_mode = LB_MODEf;
    ogdrd_fields->base_ptr = BASE_PTRf;
    ogdrd_fields->count = COUNTf;

    BCMECMP_TBL_DRD_FIELDS
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = NULL;
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = (void *)ogdrd_fields;
    /*
     * Allocate memory for storing ECMP_LEVEL0 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(ogrp_attrs, sizeof(*ogrp_attrs), "bcmecmpOlayGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = (void *)ogrp_attrs;
    bcmecmp_lt_field_attrs_t_init(ogrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(olt_fids, sizeof(*olt_fids), "bcmecmpOlayGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = (void *)olt_fids;

    /* Initialize Overlay ECMP Group LTD fields information. */
    olt_fids->ecmp_id_fid = ids->ecmp_level0_ecmp_id;
    olt_fids->lb_mode_fid = ids->ecmp_level0_lb_mode_id;
    olt_fids->max_paths_fid = ids->ecmp_level0_max_paths_id;
    olt_fids->num_paths_fid = ids->ecmp_level0_num_paths_id;
    olt_fids->ecmp_nhop_fid = INVALIDf;
    olt_fids->uecmp_id_fid = INVALIDf;
    olt_fids->nhop_id_fid = INVALIDf;

    /* Get ECMP ID min and max values from LRD field data. */
    if (olt_fids->ecmp_id_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[olt_fids->ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->max->u16;
    }
    /*
    * Get load balancing mode min and max values from LRD field
    * data.
    */
    if (olt_fids->lb_mode_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[olt_fids->lb_mode_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->max->u32;
       /*
        * Iterate over lb_mode enum symbols, get enum values and store in
        * SW database.
        */
        for (idx = 0; idx < fdata->edata->num_sym; idx++) {
            /* Get LB_MODE enum value. */
            lb_val = (uint32_t)fdata->edata->sym[idx].val;

            /*
             * Check and confirm LB_MODE enum name matches expected name
             * for this LB_MODE enum value. Return error and exit
             * initialization if invalid/unrecognized LB_MODE enum name is
             * found.
             */
            if (!sal_strcmp(lb_name[lb_val], fdata->edata->sym[idx].name)) {
                /* Initialize supported LB modes for Overlay ECMP LT. */
                BCMECMP_LT_LB_MODE
                    (unit, group, BCMECMP_GRP_TYPE_LEVEL0, lb_val) = lb_val;
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Unsupported supported LB mode %d"), lb_val));

                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }
    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be in
     * powers-of-two for entries in ECMP_LEVEL0 LT for this device.
     */
    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    if (olt_fids->max_paths_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[olt_fids->max_paths_fid]);
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->max->u16;
    }

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (olt_fids->num_paths_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[olt_fids->num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = fdata->max->u16;
    }

    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = (void *)ogdrd_fields;

    /*
     * Allocate memory for storing ECMP_LEVEL0 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(owgrp_attrs, sizeof(*owgrp_attrs), "bcmecmpWOlayGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = (void *)owgrp_attrs;
    bcmecmp_lt_field_attrs_t_init(owgrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(owlt_fids, sizeof(*owlt_fids), "bcmecmpWOlayGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = (void *)owlt_fids;

    /* Initialize Overlay ECMP Group LTD fields information. */
    owlt_fids->ecmp_id_fid = ids->ecmp_wlevel0_ecmp_id;
    owlt_fids->num_paths_fid = ids->ecmp_wlevel0_num_paths_id;
    owlt_fids->weighted_size_fid = ids->ecmp_wlevel0_wt_size_id;
    owlt_fids->ecmp_nhop_fid = INVALIDf;
    owlt_fids->uecmp_id_fid = INVALIDf;
    owlt_fids->nhop_id_fid = INVALIDf;

    /* Get ECMP ID min and max values from LRD field data. */
    if (owlt_fids->ecmp_id_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->max->u16;
    }

    /*
    * Get weighted size min and max values from LRD field
    * data.
    */
    if (owlt_fids->weighted_size_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->weighted_size_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->max->u32;
    }

    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = TRUE;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (owlt_fids->num_paths_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->max->u16;
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->max->u16;
    }


    /*
     * Get weighted ECMP group size min and max values from LRD field
     * data.
     */
    if (owlt_fids->weighted_size_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->weighted_size_fid]);
        BCMECMP_LT_MIN_WEIGHTED_SIZE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->min->u32;
        BCMECMP_LT_MAX_WEIGHTED_SIZE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = fdata->max->u32;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP_MEMBER_DESTINATIONLT fields LRD and DRD IDs.
 *
 * Initialize ECMP_MEMBER_DESTINATION LT fields LRD and DRD IDs
 * for use duing LT entry INSERT, LOOKUP, UPDATE and DELETE operations.
 *
 * \param [in] unit Unit number.
 * \param [in] map LT fields LRD mapping data.
 * \param [in] map Weighted LT fields LRD mapping data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Unexpected invalid input parameter.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_ecmp_member0_lt_flds_init(int unit,
                                   const bcmlrd_map_t *map)
{
    /* LT fields LTD ID. */
    bcmecmp_member_dest_ltd_fields_t *member0_lt_fids = NULL;
    /* Grp Field attrs. */
    bcmecmp_member_dest_lt_field_attrs_t *grp_attrs = NULL;
    /* LT field data map. */
    const bcmlrd_field_data_t *fdata = NULL;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    if (drv_var == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "member0_fields_init")));

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(member0_lt_fids, sizeof(*member0_lt_fids), \
                  "bcmecmpMemberDestLtdFids");
    BCMECMP_MEMBER0_INFO_FIDS(unit) = (void *)member0_lt_fids;

    /* Initialize Overlay ECMP Group LTD fields information. */
    member0_lt_fids->ecmp_id_fid = ids->ecmp_mlevel0_group_id;
    member0_lt_fids->num_paths_fid = ids->ecmp_mlevel0_num_entries_id;
    member0_lt_fids->base_index_fid = ids->ecmp_mlevel0_base_index_id;
    member0_lt_fids->base_index_auto_fid = ids->ecmp_mlevel0_base_index_auto_id;
    member0_lt_fids->base_index_oper_fid = ids->ecmp_mlevel0_base_index_oper_id;
    /*
     * Allocate memory for storing ECMP_LEVEL0 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpMemberDestAttrs");
    bcmecmp_member_dest_lt_field_attrs_t_init(grp_attrs);
    BCMECMP_MEMBER0_LT_FIELD_ATTRS_VOID_PTR(unit) = (void *)grp_attrs;


    /* Get ECMP ID min and max values from LRD field data. */
    if (member0_lt_fids->ecmp_id_fid !=  BCMECMP_INVALID) {
        fdata = &(map->field_data->field[member0_lt_fids->ecmp_id_fid]);
        BCMECMP_MEMBER0_LT_MIN_ECMP_ID(unit) = fdata->min->u16;
        BCMECMP_MEMBER0_LT_MAX_ECMP_ID(unit) = fdata->max->u16;
    }

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (member0_lt_fids->num_paths_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[member0_lt_fids->num_paths_fid]);
        BCMECMP_MEMBER0_LT_MIN_NPATHS(unit) = fdata->min->u16;
        BCMECMP_MEMBER0_LT_MAX_NPATHS(unit) = fdata->max->u16;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP_MEMBER_DESTINATIONLT fields LRD and DRD IDs.
 *
 * Initialize ECMP_MEMBER_DESTINATION LT fields LRD and DRD IDs
 * for use duing LT entry INSERT, LOOKUP, UPDATE and DELETE operations.
 *
 * \param [in] unit Unit number.
 * \param [in] map LT fields LRD mapping data.
 * \param [in] map Weighted LT fields LRD mapping data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Unexpected invalid input parameter.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_ecmp_member1_lt_flds_init(int unit,
                                   const bcmlrd_map_t *map)
{
    /* LT fields LTD ID. */
    bcmecmp_member_dest_ltd_fields_t *member1_lt_fids = NULL;
    /* Grp Field attrs. */
    bcmecmp_member_dest_lt_field_attrs_t *grp_attrs = NULL;
    /* LT field data map. */
    const bcmlrd_field_data_t *fdata = NULL;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    if (drv_var == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "member1_fields_init")));

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(member1_lt_fids, sizeof(*member1_lt_fids), \
                  "bcmecmpMemberDestLtdFids");
    BCMECMP_MEMBER1_INFO_FIDS(unit) = (void *)member1_lt_fids;

    /* Initialize Overlay ECMP Group LTD fields information. */
    member1_lt_fids->ecmp_id_fid = ids->ecmp_mlevel1_group_id;
    member1_lt_fids->num_paths_fid = ids->ecmp_mlevel1_num_entries_id;
    member1_lt_fids->base_index_fid = ids->ecmp_mlevel1_base_index_id;
    member1_lt_fids->base_index_auto_fid = ids->ecmp_mlevel1_base_index_auto_id;
    member1_lt_fids->base_index_oper_fid = ids->ecmp_mlevel1_base_index_oper_id;
    /*
     * Allocate memory for storing ECMP_LEVEL0 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpMemberDestAttrs");
    bcmecmp_member_dest_lt_field_attrs_t_init(grp_attrs);
    BCMECMP_MEMBER1_LT_FIELD_ATTRS_VOID_PTR(unit) = (void *)grp_attrs;


    /* Get ECMP ID min and max values from LRD field data. */
    if (member1_lt_fids->ecmp_id_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[member1_lt_fids->ecmp_id_fid]);
        BCMECMP_MEMBER1_LT_MIN_ECMP_ID(unit) = fdata->min->u16;
        BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit) = fdata->max->u16;
    }

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (member1_lt_fids->num_paths_fid !=  BCMECMP_INVALID) {
        fdata = &(map->field_data->field[member1_lt_fids->num_paths_fid]);
        BCMECMP_MEMBER1_LT_MIN_NPATHS(unit) = fdata->min->u16;
        BCMECMP_MEMBER1_LT_MAX_NPATHS(unit) = fdata->max->u16;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP_LEVEL1 LT fields LRD and DRD data.
 *
 * Initialize ECMP_LEVEL1 single ECMP resolution LT fields LRD and DRD data
 * for use duing LT entry INSERT, LOOKUP, UPDATE and DELETE operations.
 *
 * \param [in] unit Unit number.
 * \param [in] map LT fields LRD mapping data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Unexpected invalid input parameter.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_ecmp_ulay_lt_flds_init(int unit,
                                   const bcmlrd_map_t *map,
                                   const bcmlrd_map_t *wmap)
{
    bcmecmp_grp_drd_fields_t *ugdrd_fields = NULL;  /* Group tbl DRD info. */
    /* Group tbl LTD info. */
    bcmecmp_grp_ltd_fields_t *ult_fids = NULL, *uwlt_fids = NULL;
    /* Underlay Grp Field attrs. */
    bcmecmp_lt_field_attrs_t *ugrp_attrs = NULL, *uwgrp_attrs = NULL;
    const bcmlrd_field_data_t *fdata = NULL;      /* LT field data map.*/
    uint32_t idx;                                 /* Enum symbol array index.*/
    uint32_t lb_val;                              /* LB mode value.*/
    const char *const lb_name[BCMECMP_FLEX_LB_MODE_COUNT] =
                                               BCMECMP_FLEX_LB_MODE_NAME;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    if (drv_var == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values.*/
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL1) = NULL;

    /* Allocate memory for storing Group table entry fields DRD ID values.*/
    BCMECMP_ALLOC(ugdrd_fields, sizeof(*ugdrd_fields), "bcmecmpUlayGdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL1) = NULL;

    /* Initialize Group table entry fields DRD ID values. */
    bcmecmp_grp_drd_fields_t_init(ugdrd_fields);
    ugdrd_fields->lb_mode = LB_MODEf;
    ugdrd_fields->base_ptr = BASE_PTRf;
    ugdrd_fields->count = COUNTf;

    BCMECMP_TBL_DRD_FIELDS
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = NULL;
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = (void *)ugdrd_fields;

    /*
     * Allocate memory for storing ECMP_LEVEL1 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(ugrp_attrs, sizeof(*ugrp_attrs), "bcmecmpUlayGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = (void *)ugrp_attrs;
    bcmecmp_lt_field_attrs_t_init(ugrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(ult_fids, sizeof(*ult_fids), "bcmecmpUlayGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = (void *)ult_fids;

    /*
     * Initialize Underlay ECMP Group LTD fields info, the field ID
     * values are same as single level ECMP Group except that
     * RH_SIZE (last field in LT) is not supported for this LT.
     */
    ult_fids->ecmp_id_fid = ids->ecmp_level1_ecmp_id;
    ult_fids->lb_mode_fid = ids->ecmp_level1_lb_mode_id;
    ult_fids->max_paths_fid = ids->ecmp_level1_max_paths_id;
    ult_fids->num_paths_fid = ids->ecmp_level1_num_paths_id;
    ult_fids->nhop_id_fid = INVALIDf;

    /* Get ECMP ID min and max values from LRD field data. */
    if (ult_fids->ecmp_id_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[ult_fids->ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->max->u16;
    }

    /*
     * Get load balancing mode min and max values from LRD field
     * data.
     */
    if (ult_fids->lb_mode_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[ult_fids->lb_mode_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->max->u32;

        /*
         * Iterate over lb_mode enum symbols, get enum values and store in
         * SW database.
         */
        for (idx = 0; idx < fdata->edata->num_sym; idx++) {
            lb_val = (uint32_t)fdata->edata->sym[idx].val;
            if (!sal_strcmp(lb_name[lb_val], fdata->edata->sym[idx].name)) {
                /* Initialize supported LB modes for single level ECMP LT. */
                BCMECMP_LT_LB_MODE
                    (unit, group, BCMECMP_GRP_TYPE_LEVEL1, lb_val) = lb_val;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be in
     * powers-of-two for entries in ECMP_LEVEL0 LT for this device.
     */
    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    if (ult_fids->max_paths_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[ult_fids->max_paths_fid]);
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->max->u16;
    }

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (ult_fids->num_paths_fid != BCMECMP_INVALID) {
        fdata = &(map->field_data->field[ult_fids->num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = fdata->max->u16;
    }


    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(ugdrd_fields);
        BCMECMP_FREE(ult_fids);
        BCMECMP_FREE(ugrp_attrs);
    }
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = (void *)ugdrd_fields;

    /*
     * Allocate memory for storing ECMP_LEVEL0 LT logical fields Min, Max
     * attribute values.
     */
    BCMECMP_ALLOC(uwgrp_attrs, sizeof(*uwgrp_attrs), "bcmecmpWUlayGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = (void *)uwgrp_attrs;
    bcmecmp_lt_field_attrs_t_init(uwgrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(uwlt_fids, sizeof(*uwlt_fids), "bcmecmpWUlayGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = (void *)uwlt_fids;

    /* Initialize Overlay ECMP Group LTD fields information. */
    uwlt_fids->ecmp_id_fid = ids->ecmp_wlevel1_ecmp_id;
    uwlt_fids->num_paths_fid = ids->ecmp_wlevel1_num_paths_id;
    uwlt_fids->weighted_size_fid = ids->ecmp_wlevel1_wt_size_id;
    uwlt_fids->ecmp_nhop_fid = INVALIDf;
    uwlt_fids->uecmp_id_fid = INVALIDf;
    uwlt_fids->nhop_id_fid = INVALIDf;

    /* Get ECMP ID min and max values from LRD field data. */
    if (uwlt_fids->ecmp_id_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[uwlt_fids->ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->max->u16;
    }

    /*
    * Get weighted size min and max values from LRD field
    * data.
    */
    if (uwlt_fids->weighted_size_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[uwlt_fids->weighted_size_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->max->u32;
    }

    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = TRUE;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (uwlt_fids->num_paths_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[uwlt_fids->num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->max->u16;
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->max->u16;

    }

    /*
     * Get weighted ECMP group size min and max values from LRD field
     * data.
     */
    if (uwlt_fids->weighted_size_fid != BCMECMP_INVALID) {
        fdata = &(wmap->field_data->field[uwlt_fids->weighted_size_fid]);
        BCMECMP_LT_MIN_WEIGHTED_SIZE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->min->u32;
        BCMECMP_LT_MAX_WEIGHTED_SIZE
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = fdata->max->u32;

    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specific ECMP tables fields information.
 *
 * Initialize ECMP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid ECMP mode, invalid LB mode, null pointer.
 */
static int
bcm56780_a0_ecmp_fields_init(int unit,
                             bcmecmp_drv_var_t *drv_var)
{
    const bcmlrd_map_t *map = NULL, *wmap = NULL; /* LT map. */
    int rv = SHR_E_INTERNAL; /* Return value variable. */
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    /* Get LRD mapping information for ECMP_LEVEL0 LT. */
    rv = bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL0),
                        &map);
    if (SHR_SUCCESS(rv)) {
        /* Get LRD mapping information for ECMP_LEVEL0 LT. */
        rv = bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED),
                            &wmap);
    }

    if (SHR_SUCCESS(rv)) {
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);
        SHR_NULL_CHECK(wmap, SHR_E_INTERNAL);
        /*
         * Create ECMP_LEVEL0 LT related fields database info
         * required for programming LT entries in this LT.
         */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_olay_lt_flds_init(unit, map, wmap));

    }

    /* Get LRD mapping information for ECMP_LEVEL1 LT. */
    rv = bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_LEVEL1),
                        &map);
    if (SHR_SUCCESS(rv)) {
        /* Get LRD mapping information for ECMP_LEVEL0 LT. */
        rv = bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED),
                            &wmap);
    }

    if (SHR_SUCCESS(rv)) {
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);
        SHR_NULL_CHECK(wmap, SHR_E_INTERNAL);
        /*
         * Create ECMP_LEVEL1 LT related fields database info
         * required for programming LT entries in this LT.
         */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_ulay_lt_flds_init(unit, map, wmap));
    }

    /* Get LRD mapping information for ECMP_MEMBER0 LT. */
    rv = bcmlrd_map_get(unit, ids->ecmp_mlevel0_ltid,
                        &map);
    if (SHR_SUCCESS(rv)) {
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
          (bcm56780_a0_ecmp_member0_lt_flds_init(unit, map));
    }

    /* Get LRD mapping information for ECMP_MEMBER1 LT. */
    rv = bcmlrd_map_get(unit, ids->ecmp_mlevel1_ltid,
                        &map);
    if (SHR_SUCCESS(rv)) {
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
          (bcm56780_a0_ecmp_member1_lt_flds_init(unit, map));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_ecmp_itbm_groups_cleanup(int unit, int gtype)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID,
                 ecmp_id_temp = BCMECMP_INVALID; /* Group identifier. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;

    SHR_FUNC_ENTER(unit);
    if ((BCMECMP_INFO_PTR(unit) == NULL ) ||
        (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, group, gtype)) == NULL)) {
        SHR_EXIT();
    }

    for (ecmp_id_temp = BCMECMP_TBL_IDX_MIN(unit, group, gtype);
         ecmp_id_temp <= BCMECMP_TBL_IDX_MAX(unit, group, gtype);
         ecmp_id_temp++) {
        ecmp_id = ecmp_id_temp;
        if (gtype == BCMECMP_GRP_TYPE_LEVEL1) {
            ecmp_id +=
              BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        }
        SHR_IF_ERR_EXIT(
            bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use, &comp_id));
        if (in_use && (comp_id == BCMMGMT_ECMP_COMP_ID)) {
            if (BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) > 0) {
                SHR_IF_ERR_EXIT
                    (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST
                        (BCMECMP_TBL_PTR(unit, member, gtype)),
                        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id),
                        SHR_ITBM_INVALID,
                        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id)));
            }
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, group, gtype)),
                    1,
                    SHR_ITBM_INVALID,
                    ecmp_id));
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: RM cleaning up groups")));
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ecmp_itbm_lists_cleanup(int unit)
{
    shr_itbm_list_hdl_t list_hdl;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    list_hdl =
      BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL0)));
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        if (rv != SHR_E_NONE) {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    list_hdl =
      BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1)));

    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        if (rv != SHR_E_NONE) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL0))) = NULL;
    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))) = NULL;

    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))) = NULL;
    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))) = NULL;

    list_hdl =
      BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0)));
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        SHR_IF_ERR_EXIT(rv);
    }
    list_hdl =
      BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1)));
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        SHR_IF_ERR_EXIT(rv);
    }

    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0))) = NULL;
    BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))) = NULL;

    list_hdl = BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit));
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        SHR_IF_ERR_EXIT(rv);
    }

    BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)) = NULL;

    list_hdl = BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit));
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        SHR_IF_ERR_EXIT(rv);
    }

    BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)) = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        if (list_hdl) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Error: RM cleaning up list")));
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ecmp_member_dest_itbm_groups_cleanup(int unit, int level)
{
    bcmecmp_id_t grp_id_temp = 0; /* Group identifier. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;

    SHR_FUNC_ENTER(unit);

    if ((BCMECMP_INFO_PTR(unit) == NULL) ||
       ((level == 0) &&
        (BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)) == NULL)) ||
        ((level == 1) &&
        (BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit)) == NULL))) {
        SHR_EXIT();
    }

    for (grp_id_temp = 0;
         grp_id_temp < BCMECMP_MEMBER_DEST_GRP_TBL_SIZE;
         grp_id_temp++) {

        SHR_IF_ERR_EXIT
            (bcmecmp_grp_in_use(unit, grp_id_temp, TRUE, level, 0, &in_use, &comp_id));
        if (in_use) {
            if (level == 0) {
                SHR_IF_ERR_EXIT
                    ((shr_itbm_list_block_free(BCMECMP_MEMBER0_TBL_ITBM_LIST
                        (BCMECMP_INFO_PTR(unit)),
                        BCMECMP_MEMBER0_GRP_NUM_PATHS(unit,grp_id_temp),
                        SHR_ITBM_INVALID,
                        BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX(unit,
                                                               grp_id_temp))));
            } else {
                SHR_IF_ERR_EXIT
                    ((shr_itbm_list_block_free(BCMECMP_MEMBER1_TBL_ITBM_LIST
                        (BCMECMP_INFO_PTR(unit)),
                        BCMECMP_MEMBER1_GRP_NUM_PATHS(unit,grp_id_temp),
                        SHR_ITBM_INVALID,
                        BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit,
                                                               grp_id_temp))));
            }

        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: RM cleaning up groups")));
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ecmp_itbm_lists_init(int unit,
                                const bcmecmp_drv_var_t *drv_var)
{
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t list_hdl = NULL;
    int rv = SHR_E_NONE;
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member1_info;
    const bcmecmp_member_info_t *member0_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    member1_info = drv_var->member1_info;
    member0_info = drv_var->member0_info;

    if (ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, group,
                                    BCMECMP_GRP_TYPE_LEVEL0);
        params.min = BCMECMP_TBL_IDX_MIN
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) +
                     BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) + 1;
        params.first = BCMECMP_TBL_IDX_MIN
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        params.last = BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.block_stat = false;

        rv = shr_itbm_list_create(params, "bcmecmpGrp0List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL0)))
                 = list_hdl;
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)))
                 = list_hdl;
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpGrp0List %d\n"),
                                 rv));
            SHR_ERR_EXIT(rv);
        }
    } else {
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL0)))
                 = NULL;
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)))
                 = NULL;
    }

    if (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, group,
                                    BCMECMP_GRP_TYPE_LEVEL1);
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.min = BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) + 1;
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) +
                     BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1) + 1;
        params.first = BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) + 1;
        params.last = params.max;
        params.block_stat = false;

        rv = shr_itbm_list_create(params, "bcmecmpGrp1List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL1)))
                 = list_hdl;
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
                 BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)))
                 = list_hdl;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpGrp1List %d\n"),
                                 rv));
            SHR_ERR_EXIT(rv);
        }
    } else {
       BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1)))
             = NULL;

        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)))
             =  NULL;
    }

    if (member0_info != NULL) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, member,
                                    BCMECMP_GRP_TYPE_LEVEL0);
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.min = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.first = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.last = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.block_stat =true;

        rv = shr_itbm_list_create(params, "bcmecmpMem0List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
                 BCMECMP_GRP_TYPE_LEVEL0)))
                 = list_hdl;
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
                 BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)))
                 = list_hdl;
            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RM created bcmecmpMem0List %d\n"),
                                 rv));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpMem0List %d\n"),
                                 rv));
            SHR_ERR_EXIT(rv);
        }
    } else {
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0)))
             = NULL;
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)))
             = NULL;
    }

    if (ids->ecmp_mlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, member,
                                    BCMECMP_GRP_TYPE_LEVEL0);
        params.comp_id = BCMMGMT_ECMP_MEMBER_DEST_COMP_ID;
        params.min = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.first = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.last = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        params.block_stat =true;

        rv = shr_itbm_list_create(params, "bcmecmpMemDest0List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RM rv=%d list %p.\n"),
                                 rv, ((void *)list_hdl)));
            BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
             = list_hdl;

        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpMemDest0List %d\n"),
                                 rv));
            SHR_ERR_EXIT(rv);
        }
    } else {
        BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
         = NULL;
    }


    if (ids->ecmp_mlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        /* To create unique comp id for MemDest1 */
        params.comp_id = BCMMGMT_ECMP_MEMBER_DEST_COMP_ID +
                         BCMMGMT_ECMP_MEMBER_DEST_COMP_ID;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, member,
                                    BCMECMP_GRP_TYPE_LEVEL1);

        params.min = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.first = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.last = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.block_stat = true;

        rv = shr_itbm_list_create(params, "bcmecmpMemDest1List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "RM rv=%d list %p.\n"),
                                 rv, ((void *)list_hdl)));
             BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
                = list_hdl;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpMemDest1List %d\n"),
                                 rv));
            SHR_ERR_EXIT(rv);
        }
    } else {
        BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
        = NULL;
    }

    if (member1_info != NULL) {
        shr_itbm_list_params_t_init(&params);
        params.unit = unit;
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.table_id = BCMECMP_TBL_DRD_SID(unit, member,
                                    BCMECMP_GRP_TYPE_LEVEL1);
        params.min = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.max = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.first = BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.last = BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
        params.block_stat = true;

        rv = shr_itbm_list_create(params, "bcmecmpMem1List", &list_hdl);
        if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
                 BCMECMP_GRP_TYPE_LEVEL1)))
                 = list_hdl;
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
                 BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)))
                 = list_hdl;
        } else {
           LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Error: RM failed to create "\
                                            "bcmecmpMem1List %d\n"),
                                 rv));
           SHR_ERR_EXIT(rv);
        }
    } else {
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1)))
             = NULL;
        BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)))
             = NULL;
    }
exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RM failed to cleanup list rv=%d \n"),
                         rv));
        list_hdl = BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL0)));
        if (list_hdl) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL0))) = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl = BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0)));
        if (list_hdl) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL0))) = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl = BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1)));
        if (list_hdl) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))) = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl = BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1)));
        if (list_hdl) {
            BCMECMP_TBL_ITBM_LIST((BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))) = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl =
           BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit));
        if (list_hdl) {
            BCMECMP_MEMBER0_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
                                                               = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl =
           BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit));
        if (list_hdl) {
            BCMECMP_MEMBER1_TBL_ITBM_LIST(BCMECMP_INFO_PTR(unit))
                                                               = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specfic ECMP tables range information.
 *
 * Initialize ECMP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_var Variant driver.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value.
 */
static int
bcm56780_a0_ecmp_hw_tbls_range_init(int unit,
                                    const bcmecmp_drv_var_t *drv_var)
{
    uint32_t wsize; /* Hardware table entry size in words. */
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    int grp_imin;   /* ECMP Group hardware table Min index value. */
    int grp_imax;   /* ECMP Group hardware table Max index value. */
    int tbl_sz;     /* Local variable to store calculated table size. */
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    /*
     * Pre-IFP ECMP stage is removed in TD4 device, initalize related
     * Overlay and Underlay tables entry size as zero in SW database for
     * this device.
     */
    BCMECMP_TBL_ENT_SIZE
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL0) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL0) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL1) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL1) = 0;

    /*
     * Get Overlay Group table i.e. ECMP_LEVEL0 table entry
     * size in words.
     */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL0));
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID
                                        (unit, member,
                                         BCMECMP_GRP_TYPE_LEVEL0));
    BCMECMP_TBL_ENT_SIZE
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL1));

    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    BCMECMP_TBL_ENT_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_LEVEL0);

    BCMECMP_TBL_ENT_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_LEVEL1);

    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID
                                        (unit, member,
                                         BCMECMP_GRP_TYPE_LEVEL1));
    BCMECMP_TBL_ENT_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_LEVEL1) = wsize;

    BCMECMP_TBL_ENT_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                            (unit, member,
                                             BCMECMP_GRP_TYPE_LEVEL0);

    BCMECMP_TBL_ENT_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                            (unit, member,
                                             BCMECMP_GRP_TYPE_LEVEL1);


    /*
     * TD4 device does not require single ECMP resolution,
     * Overlay handles ECMP_LEVEL0 and underlay is for ECMP_LEVEL1.
     * initialize related member and group table
     * indices as invalid.
     */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_SIZE
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_SIZE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;


    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                        BCMECMP_GRP_TYPE_LEVEL0));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                        BCMECMP_GRP_TYPE_LEVEL0));

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min(unit,
                                   BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL0));
    grp_imax = bcmdrd_pt_index_max(unit,
                                   BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL0));

    /*
     * Initialize Overlay ECMP Member table Min & Max indicies, table
     * size and used indices status.
     * Max index value is half of the hardware table size. Member table
     * is split into two to support Overlay and Underlay ECMP lookups.
     */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = memb_imax;

    if (ids->ecmp_level0_ltid == (uint32_t) BCMECMP_INVALID) {
        tbl_sz = 0;
    } else {
        tbl_sz = (BCMECMP_TBL_IDX_MAX
                    (unit, member, BCMECMP_GRP_TYPE_LEVEL0)
                    - BCMECMP_TBL_IDX_MIN
                        (unit, member, BCMECMP_GRP_TYPE_LEVEL0)) + 1;
    }
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL0) = tbl_sz;
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = tbl_sz;

    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = memb_imin;

    /*
     * Overlay ECMP Group HW Table Min & Max indicies, Size and Used
     * indices status intialization.
     */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = grp_imax;

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = grp_imax;

    /* Calculate Overlay ECMP Group table size. */
    if (ids->ecmp_level0_ltid == (uint32_t) BCMECMP_INVALID) {
        tbl_sz = 0;
    } else {
        tbl_sz = (BCMECMP_TBL_IDX_MAX
                    (unit, group, BCMECMP_GRP_TYPE_LEVEL0)
                    - BCMECMP_TBL_IDX_MIN
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL0)) + 1;
    }
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0) = tbl_sz;
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = tbl_sz;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = grp_imin;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) = grp_imin;
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
            = BCMECMP_TBL_IDX_MIN
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
            = BCMECMP_TBL_IDX_MAX
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL0);

    /*
     * Initialize Underlay ECMP Member table Min & Max indices, Size
     * and Used indices status.
     */
    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                        BCMECMP_GRP_TYPE_LEVEL1));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                        BCMECMP_GRP_TYPE_LEVEL1));

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min(unit,
                                   BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL1));
    grp_imax = bcmdrd_pt_index_max(unit,
                                   BCMECMP_TBL_DRD_SID(unit, group,
                                        BCMECMP_GRP_TYPE_LEVEL1));

    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1)
            = memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = memb_imax;


    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
            = BCMECMP_TBL_IDX_MIN
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL1);
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
            = BCMECMP_TBL_IDX_MAX
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL1);

    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
            = BCMECMP_TBL_IDX_MIN
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
            = BCMECMP_TBL_IDX_MAX
                  (unit, member, BCMECMP_GRP_TYPE_LEVEL0);

    /* Calcuate Underlay ECMP Member table size. */
    if (ids->ecmp_level1_ltid == (uint32_t) BCMECMP_INVALID) {
        tbl_sz = 0;
    } else {
        tbl_sz = (BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_LEVEL1)
                - BCMECMP_TBL_IDX_MIN
                    (unit, member, BCMECMP_GRP_TYPE_LEVEL1)) + 1;
    }
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL1) = tbl_sz;
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = tbl_sz;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = memb_imin;

    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
        = BCMECMP_TBL_IDX_MAX_USED
              (unit, member, BCMECMP_GRP_TYPE_LEVEL1);

    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)
        = BCMECMP_TBL_IDX_MAX_USED
              (unit, member, BCMECMP_GRP_TYPE_LEVEL0);

    /*
     * Initialize Underlay ECMP Group HW Table Min & Max indicies, Size
     * and Used indices status intialization.
     */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1)
            = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = grp_imax;

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
            = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) = grp_imax;

    /* Calculate Underlay ECMP Group table size. */
    if (ids->ecmp_level1_ltid == (uint32_t) BCMECMP_INVALID) {
        tbl_sz = 0;
    } else {
        tbl_sz = (BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_LEVEL1)
                - BCMECMP_TBL_IDX_MIN
                    (unit, group, BCMECMP_GRP_TYPE_LEVEL1)) + 1;
    }
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL1) = tbl_sz;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = grp_imin;

    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
        = BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL1);
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)
        = BCMECMP_TBL_IDX_MAX_USED
              (unit, group, BCMECMP_GRP_TYPE_LEVEL1);


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specific ECMP tables information.
 *
 * Initialize ECMP Group and Member tables information by retrieving details
 * from LRD and DRD.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value, null pointer detected.
 */
static int
bcm56780_a0_ecmp_tables_init(int unit,
                             bcmecmp_drv_var_t *drv_var)
{

    SHR_FUNC_ENTER(unit);

    /* Initialize ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_sids_init(unit, drv_var));

    /* Initialize ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_hw_tbls_range_init(unit, drv_var));

    /* Initialize ECMP tables logical and physical field details. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_fields_init(unit, drv_var));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates the memory for storing ECMP software state.
 *
 * This function allocates memory to store ECMP Group tables, Member tables
 * and ECMP logical table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_ecmp_db_init(int unit,
                         bool warm,
                         const bcmecmp_drv_var_t *drv_var)
{
    bcmecmp_tbl_prop_t *tbl_ptr = NULL; /* ECMP hardware table info ptr. */
    size_t tbl_size;            /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0;   /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0;    /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56780000; /* ECMP HA block signature. */
    bcmecmp_flex_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_flex_grp_ha_blk_t *grp_array_ptr;
    bcmecmp_flex_grp_rh_ha_blk_t *rh_grp_array_ptr;
    bcmecmp_member_dest_grp_ha_blk_t *grp_member_dest_array_ptr;
    int grp_id, gstart_idx, gend_idx;
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    if ((ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) ||
        (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID)) {

        /* Allocate memory for Member table entries array. */
        tbl_size = (size_t)(BCMECMP_TBL_SIZE
                                (unit, member, BCMECMP_GRP_TYPE_LEVEL0)
                            + BCMECMP_TBL_SIZE
                                (unit, member,
                                 BCMECMP_GRP_TYPE_LEVEL1));

        /*
         * Allocate memory for active Member Table entries array
         * elements.
         */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_hw_entry_ha_alloc(
                                       unit,
                                       warm,
                                       tbl_size,
                                       BCMECMP_MEMB_TBL_SUB_COMP_ID,
                                       (blk_sig |
                                       BCMECMP_MEMB_TBL_SUB_COMP_ID),
                                       &hw_array_ptr,
                                       &ha_alloc_sz));
        tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        tbl_ptr->ent_arr = (bcmecmp_flex_hw_entry_attr_t *) hw_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
        /*
         * Initialize Underlay ECMP groups member table entries array
         * pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_arr =
                         ((bcmecmp_flex_hw_entry_attr_t *) tbl_ptr->ent_arr) +
                         BCMECMP_TBL_SIZE
                         (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        /*
         * Initialize level0 weighted ECMP groups member table entries array
         * pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;
        /*
         * Initialize level1 weighted ECMP groups member table entries array
         * pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_arr = (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1))->ent_arr;

        /* Allocate memory for Group Table entries arrary. */
        tbl_size = (size_t)(BCMECMP_TBL_SIZE
                                (unit, group, BCMECMP_GRP_TYPE_LEVEL0)
                            + BCMECMP_TBL_SIZE
                                (unit, group,
                                 BCMECMP_GRP_TYPE_LEVEL1));
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_hw_entry_ha_alloc(
                                       unit,
                                       warm,
                                       tbl_size,
                                       BCMECMP_GRP_TBL_SUB_COMP_ID,
                                       (blk_sig |
                                       BCMECMP_GRP_TBL_SUB_COMP_ID),
                                       &hw_array_ptr,
                                       &ha_alloc_sz));
        tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        tbl_ptr->ent_arr = (bcmecmp_flex_hw_entry_attr_t *) hw_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;

        /* Initialize Underlay ECMP groups array pointer. */
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_arr =
                         (bcmecmp_flex_hw_entry_attr_t *)tbl_ptr->ent_arr +
                         BCMECMP_TBL_SIZE
                         (unit, group, BCMECMP_GRP_TYPE_LEVEL0);

        /* Initialize weighted ECMP groups array pointer. */
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_arr =
                         ((bcmecmp_flex_hw_entry_attr_t *) tbl_ptr->ent_arr) +
                         BCMECMP_TBL_SIZE
                         (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        /* Allocate memory for active ECMP group LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_GRP_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_GRP_SUB_COMP_ID),
                                         &grp_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_FLEX_GRP_INFO_PTR(unit) = (void *) grp_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;

        SHR_IF_ERR_EXIT
            (bcmecmp_flex_grp_rh_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_RH_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_FLEX_RH_SUB_COMP_ID),
                                         &rh_grp_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_FLEX_GRP_RH_INFO_PTR(unit) = (void *) rh_grp_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
    } else {
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_arr = NULL;

        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_arr = NULL;
        BCMECMP_FLEX_GRP_INFO_PTR(unit) = NULL;
    }

    if (ids->ecmp_mlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
        tbl_size = BCMECMP_MEMBER_DEST_GRP_TBL_SIZE;

        /* Allocate memory for active ECMP member0 LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_dest_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_MEMB0_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_FLEX_MEMB0_SUB_COMP_ID),
                                         &grp_member_dest_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_MEMBER0_GRP_INFO_PTR(unit) = (void *)
                     grp_member_dest_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
        gstart_idx = 0;

        if (!warm) {
            gend_idx = BCMECMP_MEMBER0_LT_MAX_ECMP_ID(unit);
            for (grp_id = gstart_idx; grp_id <= gend_idx; grp_id++) {
                BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, grp_id) = 0;
            }
        }
    } else {
        BCMECMP_MEMBER0_GRP_INFO_PTR(unit) = NULL;
    }

    if (ids->ecmp_mlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        tbl_size = BCMECMP_MEMBER_DEST_GRP_TBL_SIZE;
        /* Allocate memory for active ECMP member1 LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_dest_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_MEMB1_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_FLEX_MEMB1_SUB_COMP_ID),
                                         &grp_member_dest_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_MEMBER1_GRP_INFO_PTR(unit) = (void *)
                     grp_member_dest_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
        gstart_idx = 0;

        if (!warm) {
            gend_idx = BCMECMP_MEMBER1_LT_MAX_ECMP_ID(unit);
            for (grp_id = gstart_idx; grp_id <= gend_idx; grp_id++) {
                BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, grp_id) = 0;
            }
        }
    } else {
        BCMECMP_MEMBER1_GRP_INFO_PTR(unit) = NULL;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total ECMP HA Mem = %uKB.\n"),
                tot_ha_mem / 1024));
exit:
    if (SHR_FUNC_ERR()) {
        if (BCMECMP_FLEX_GRP_INFO_PTR(unit)) {
            grp_array_ptr = (bcmecmp_flex_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_FLEX_GRP_INFO_PTR(unit)
                             - sizeof(bcmecmp_flex_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMECMP_FLEX_GRP_INFO_PTR(unit) = NULL;
        }

        if (BCMECMP_FLEX_GRP_RH_INFO_PTR(unit)) {
            rh_grp_array_ptr = (bcmecmp_flex_grp_rh_ha_blk_t *)
                            ((uint8_t *)BCMECMP_FLEX_GRP_RH_INFO_PTR(unit)
                             - sizeof(bcmecmp_flex_grp_rh_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp RH HA addr=0x%p\n"),
                         (void*)rh_grp_array_ptr));
            shr_ha_mem_free(unit, rh_grp_array_ptr);
            BCMECMP_FLEX_GRP_RH_INFO_PTR(unit) = NULL;
        }

        tbl_ptr = BCMECMP_TBL_PTR
                        (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        if (tbl_ptr->ent_arr) {
            hw_array_ptr = (bcmecmp_flex_hw_entry_ha_blk_t *)
                           ((uint8_t *)tbl_ptr->ent_arr
                            - sizeof(bcmecmp_flex_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free memb hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_arr = NULL;
        }

        tbl_ptr = BCMECMP_TBL_PTR
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        if (tbl_ptr->ent_arr) {
            hw_array_ptr = (bcmecmp_flex_hw_entry_ha_blk_t *)
                           ((uint8_t *)tbl_ptr->ent_arr
                            - sizeof(bcmecmp_flex_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_arr = NULL;
        }

        if (BCMECMP_MEMBER0_GRP_INFO_PTR(unit)) {
            grp_member_dest_array_ptr = (bcmecmp_member_dest_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_MEMBER0_GRP_INFO_PTR(unit)
                             - sizeof(bcmecmp_member_dest_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free mbr0 grp HA addr=0x%p\n"),
                         (void*)grp_member_dest_array_ptr));
            shr_ha_mem_free(unit, grp_member_dest_array_ptr);
            BCMECMP_MEMBER0_GRP_INFO_PTR(unit) = NULL;
        }

        if (BCMECMP_MEMBER1_GRP_INFO_PTR(unit)) {
            grp_member_dest_array_ptr = (bcmecmp_member_dest_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_MEMBER1_GRP_INFO_PTR(unit)
                             - sizeof(bcmecmp_member_dest_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free mbr1 grp HA addr=0x%p\n"),
                         (void*)grp_member_dest_array_ptr));
            shr_ha_mem_free(unit, grp_member_dest_array_ptr);
            BCMECMP_MEMBER1_GRP_INFO_PTR(unit) = NULL;
        }

    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates the memory for storing ECMP software state.
 *
 * This function allocates memory to store ECMP Group tables, Member tables
 * and ECMP logical table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_ecmp_abort_db_init(int unit,
                               bool warm,
                               const bcmecmp_drv_var_t *drv_var)
{
    bcmecmp_tbl_prop_t *tbl_ptr = NULL; /* ECMP hardware table info ptr. */
    size_t tbl_size; /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56780000; /* ECMP HA block signature. */
    bcmecmp_flex_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_flex_grp_ha_blk_t *grp_array_ptr;
    bcmecmp_flex_grp_rh_ha_blk_t *rh_grp_array_ptr;
    bcmecmp_member_dest_grp_ha_blk_t *grp_member_dest_array_ptr;
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    if ((ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) ||
        (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID)) {
        /* Allocate memory for Member table entries array. */
        tbl_size = (size_t)(BCMECMP_TBL_SIZE
                                (unit, member, BCMECMP_GRP_TYPE_LEVEL0)
                            + BCMECMP_TBL_SIZE
                                (unit, member,
                                 BCMECMP_GRP_TYPE_LEVEL1));


        SHR_IF_ERR_EXIT
            (bcmecmp_flex_hw_entry_ha_alloc(
                                       unit,
                                       warm,
                                       tbl_size,
                                       BCMECMP_MEMB_TBL_BK_SUB_COMP_ID,
                                       (blk_sig |
                                       BCMECMP_MEMB_TBL_BK_SUB_COMP_ID),
                                       &hw_array_ptr,
                                       &ha_alloc_sz));
        tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        tbl_ptr->ent_bk_arr =
           (bcmecmp_flex_hw_entry_attr_t *) hw_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;

        /*
         * Initialize Underlay ECMP groups member table entries array
         * pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr =
                    ((bcmecmp_flex_hw_entry_attr_t *) tbl_ptr->ent_bk_arr)  +
                      BCMECMP_TBL_SIZE
                     (unit, member,BCMECMP_GRP_TYPE_LEVEL0);

        /*
         * Initialize weighted ECMP groups member table entries array
         * pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_bk_arr =
             tbl_ptr->ent_bk_arr;

        (BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_bk_arr =
             (BCMECMP_TBL_PTR
                     (unit, member, BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr;
        /* Allocate memory for Group Table entries arrary. */
        tbl_size = (size_t)(BCMECMP_TBL_SIZE
                                (unit, group, BCMECMP_GRP_TYPE_LEVEL0)
                            + BCMECMP_TBL_SIZE
                                (unit, group,
                                 BCMECMP_GRP_TYPE_LEVEL1));

        SHR_IF_ERR_EXIT
            (bcmecmp_flex_hw_entry_ha_alloc(
                                       unit,
                                       warm,
                                       tbl_size,
                                       BCMECMP_GRP_TBL_BK_SUB_COMP_ID,
                                       (blk_sig |
                                       BCMECMP_GRP_TBL_BK_SUB_COMP_ID),
                                       &hw_array_ptr,
                                       &ha_alloc_sz));
        tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        tbl_ptr->ent_bk_arr = (bcmecmp_flex_hw_entry_attr_t *) hw_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;

        /* Initialize Underlay ECMP groups array pointer. */
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr =
                            ((bcmecmp_flex_hw_entry_attr_t *) tbl_ptr->ent_bk_arr) +
                                                       BCMECMP_TBL_SIZE
                                           (unit, group,BCMECMP_GRP_TYPE_LEVEL0);

        /* Initialize weighted ECMP groups array pointer. */
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_bk_arr = tbl_ptr->ent_bk_arr;

        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_bk_arr = BCMECMP_TBL_PTR
                       (unit, group, BCMECMP_GRP_TYPE_LEVEL1)->ent_bk_arr;
        /* Allocate memory for active ECMP group LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_GRP_BK_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_GRP_BK_SUB_COMP_ID),
                                         &grp_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_FLEX_GRP_INFO_BK_PTR(unit) = (bcmecmp_flex_grp_attr_t *) grp_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;

        /* Allocate memory for active ECMP RH group LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_grp_rh_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_RH_BK_SUB_COMP_ID,
                                         (blk_sig |
                                         BCMECMP_FLEX_RH_BK_SUB_COMP_ID),
                                         &rh_grp_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_FLEX_GRP_RH_INFO_BK_PTR(unit) = (bcmecmp_flex_grp_rh_attr_t *) rh_grp_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
    } else {
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0))->ent_bk_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, member,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_bk_arr =  NULL;
        (BCMECMP_TBL_PTR(unit, member,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_bk_arr = NULL;

        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0))->ent_bk_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED))->ent_bk_arr = NULL;
        (BCMECMP_TBL_PTR
            (unit, group,
             BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))->ent_bk_arr = NULL;
        BCMECMP_FLEX_GRP_INFO_BK_PTR(unit) = NULL;
    }

    if (ids->ecmp_mlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
        tbl_size = BCMECMP_MEMBER_DEST_GRP_TBL_SIZE;

        /* Allocate memory for active ECMP group LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_dest_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_MEMB0_BK_SUB_COMP_ID,
                                         (blk_sig | BCMECMP_FLEX_MEMB0_BK_SUB_COMP_ID),
                                         &grp_member_dest_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_MEMBER0_GRP_INFO_BK_PTR(unit) =
              (void *) grp_member_dest_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
    } else {
        BCMECMP_MEMBER0_GRP_INFO_BK_PTR(unit) = NULL;
    }

    if (ids->ecmp_mlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        tbl_size = BCMECMP_MEMBER_DEST_GRP_TBL_SIZE;
        /* Allocate memory for active ECMP group LT entries array elements. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_dest_grp_attr_ha_alloc(
                                         unit,
                                         warm,
                                         tbl_size,
                                         BCMECMP_FLEX_MEMB1_BK_SUB_COMP_ID,
                                         (blk_sig | BCMECMP_FLEX_MEMB1_BK_SUB_COMP_ID),
                                         &grp_member_dest_array_ptr,
                                         &ha_alloc_sz));
        BCMECMP_MEMBER1_GRP_INFO_BK_PTR(unit) =
              (void *) grp_member_dest_array_ptr->array;
        tot_ha_mem += ha_alloc_sz;
    } else {
        BCMECMP_MEMBER1_GRP_INFO_BK_PTR(unit) = NULL;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total ECMP HA BK Mem = %uKB.\n"),
                tot_ha_mem / 1024));
exit:
    if (SHR_FUNC_ERR()) {
        if (BCMECMP_FLEX_GRP_INFO_BK_PTR(unit)) {
            grp_array_ptr = (bcmecmp_flex_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_FLEX_GRP_INFO_BK_PTR(unit)
                             - sizeof(bcmecmp_flex_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free bk grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMECMP_FLEX_GRP_INFO_BK_PTR(unit) = NULL;
        }

        if (BCMECMP_FLEX_GRP_RH_INFO_BK_PTR(unit)) {
            rh_grp_array_ptr = (bcmecmp_flex_grp_rh_ha_blk_t *)
                            ((uint8_t *)BCMECMP_FLEX_GRP_RH_INFO_BK_PTR(unit)
                             - sizeof(bcmecmp_flex_grp_rh_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free bk grp HA addr=0x%p\n"),
                         (void*)rh_grp_array_ptr));
            shr_ha_mem_free(unit, rh_grp_array_ptr);
            BCMECMP_FLEX_GRP_RH_INFO_BK_PTR(unit) = NULL;
        }

        tbl_ptr = BCMECMP_TBL_PTR
                        (unit, member, BCMECMP_GRP_TYPE_LEVEL0);
        if (tbl_ptr->ent_bk_arr) {
            hw_array_ptr = (bcmecmp_flex_hw_entry_ha_blk_t *)
                           ((uint8_t *)tbl_ptr->ent_bk_arr
                            - sizeof(bcmecmp_flex_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\tfree bk memb hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_bk_arr = NULL;
        }
        (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr
                = NULL;


        tbl_ptr = BCMECMP_TBL_PTR
                        (unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        if (tbl_ptr->ent_bk_arr) {
            hw_array_ptr = (bcmecmp_flex_hw_entry_ha_blk_t *)
                           ((uint8_t *)tbl_ptr->ent_bk_arr
                            - sizeof(bcmecmp_flex_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\tfree bk grp hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_bk_arr = NULL;
        }
        (BCMECMP_TBL_PTR
            (unit, group, BCMECMP_GRP_TYPE_LEVEL1))->ent_bk_arr =
            NULL;
        if (BCMECMP_MEMBER0_GRP_INFO_BK_PTR(unit)) {
            grp_member_dest_array_ptr = (bcmecmp_member_dest_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_MEMBER0_GRP_INFO_BK_PTR(unit)
                             - sizeof(bcmecmp_member_dest_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free mbr0 grp HA addr=0x%p\n"),
                         (void*)grp_member_dest_array_ptr));
            shr_ha_mem_free(unit, grp_member_dest_array_ptr);
            BCMECMP_MEMBER0_GRP_INFO_BK_PTR(unit) = NULL;
        }

        if (BCMECMP_MEMBER1_GRP_INFO_BK_PTR(unit)) {
            grp_member_dest_array_ptr = (bcmecmp_member_dest_grp_ha_blk_t *)
                            ((uint8_t *)BCMECMP_MEMBER1_GRP_INFO_BK_PTR(unit)
                             - sizeof(bcmecmp_member_dest_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free mbr1 grp HA addr=0x%p\n"),
                         (void*)grp_member_dest_array_ptr));
            shr_ha_mem_free(unit, grp_member_dest_array_ptr);
            BCMECMP_MEMBER1_GRP_INFO_BK_PTR(unit) = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries initialization function.
 *
 * Allocates memory to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] drv_var Device variant driver info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_ecmp_entries_init(int unit,
                              bool warm,
                              const bcmecmp_drv_var_t *drv_var)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Allocate HA memory for managing ECMP group and member table
     * entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_db_init(unit, warm, drv_var));

    /* Initialize atomic transaction related SW database. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_ecmp_abort_db_init(unit, warm, drv_var));
    }
exit:
    SHR_FUNC_EXIT();
}

static int bcm56780_a0_ecmp_lt_entry_alloc(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate memory to store LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_FLEX_LT_ENT_PTR_VOID(unit), sizeof(bcmecmp_flex_lt_entry_t),
         "bcmecmpDevFlexLtEntInfo");

    /* Allocate memory to store current LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_CURRENT_LT_ENT_PTR_VOID(unit), sizeof(bcmecmp_flex_lt_entry_t),
         "bcmecmpDevCurrentLtEntInfo");
exit:
    SHR_FUNC_EXIT();
}

static void bcm56780_a0_ecmp_lt_entry_free(int unit)
{
    /* Free LT entry. */
    if (BCMECMP_FLEX_LT_ENT_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_FLEX_LT_ENT_PTR_VOID(unit));
    }

    /* Free memory allocated for current LT Entry buffer. */
    if (BCMECMP_CURRENT_LT_ENT_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_CURRENT_LT_ENT_PTR_VOID(unit));
    }
}

/*!
 * \brief Trident4 device ECMP hardware table details initialization
 * function.
 *
 * Initializes ECMP hardware Tables, Fields and Entries information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_info_init(int unit, bool warm)
{
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    if (ids == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Initialize ECMP tables information for Trident4 device. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_tables_init(unit, drv_var));

    /* Allocate itbm list handlers for the ecmp groups. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_itbm_lists_init(unit, drv_var));

    /*
     * Initialize ECMP Group and Member tables entries array for Single level,
     * Overlay and Underlay groups support.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_entries_init(unit, warm, drv_var));

    /* Allocate LT entry. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_lt_entry_alloc(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP_LEVEL0 LT fields LRD and DRD data.
 *
 * Cleanup ECMP_LEVEL0 single ECMP resolution LT fields LRD and DRD data.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_olay_grps_flds_cleanup(int unit)
{
    bcmecmp_grp_drd_fields_t *ogdrd_fields = NULL;  /* Group tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *olt_fids = NULL;   /* Group tbl LTD info. */
    bcmecmp_lt_field_attrs_t *ogrp_attrs = NULL; /* Single level grp attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing Group tables DRD fields data. */
    ogdrd_fields = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                    (unit, group,
                                                     BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_FREE(ogdrd_fields);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    olt_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_FREE(olt_fids);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    olt_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED);
    BCMECMP_FREE(olt_fids);

    /*
     * Free memory allocated for storing ECMP_LEVEL0 LT logical fields
     * attributes information.
     */
    ogrp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0);
    BCMECMP_FREE(ogrp_attrs);

    /*
     * Free memory allocated for storing ECMP_WEIGHTED_0 LT logical fields
     * attributes information.
     */
    ogrp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED);
    BCMECMP_FREE(ogrp_attrs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP_LEVEL1 LT fields LRD and DRD data.
 *
 * Cleanup ECMP_LEVEL1 single ECMP resolution LT fields LRD and DRD data.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_ulay_grps_flds_cleanup(int unit)
{
    bcmecmp_grp_drd_fields_t *ugdrd_fields = NULL;  /* Group tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *ult_fids = NULL;   /* Group tbl LTD info. */
    bcmecmp_lt_field_attrs_t *ugrp_attrs = NULL; /* Single level grp attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing Group tables DRD fields data. */
    ugdrd_fields = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                    (unit, group,
                                                     BCMECMP_GRP_TYPE_LEVEL1);
    BCMECMP_FREE(ugdrd_fields);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    ult_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL1);
    BCMECMP_FREE(ult_fids);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    ult_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED);
    BCMECMP_FREE(ult_fids);

    /*
     * Free memory allocated for storing ECMP_LEVEL1 LT logical fields
     * attributes information.
     */
    ugrp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL1);
    BCMECMP_FREE(ugrp_attrs);

    /*
     * Free memory allocated for storing ECMP_WEIGHTED_1 LT logical fields
     * attributes information.
     */
    ugrp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED);
    BCMECMP_FREE(ugrp_attrs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP_MEMBER0 LT fields LRD data.
 *
 * Cleanup ECMP_MEMBER0 LT fields LRD data.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_member0_grps_flds_cleanup(int unit)
{

    bcmecmp_member_dest_ltd_fields_t *lt_fids = NULL;   /* LTD info. */
    bcmecmp_member_dest_lt_field_attrs_t *grp_attrs = NULL; /* Single level grp attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing ECMP member0 LT logical fields data. */
    lt_fids = (bcmecmp_member_dest_ltd_fields_t *)
                  BCMECMP_MEMBER0_INFO_FIDS(unit);
    BCMECMP_FREE(lt_fids);

    /*
     * Free memory allocated for storing ECMP_MEMBER0 LT logical fields
     * attributes information.
     */
    grp_attrs = (bcmecmp_member_dest_lt_field_attrs_t *)
                    BCMECMP_MEMBER0_LT_FIELD_ATTRS_VOID_PTR(unit);
    BCMECMP_FREE(grp_attrs);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP_MEMBER1 LT fields LRD data.
 *
 * Cleanup ECMP_MEMBER1 LT fields LRD data.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_member1_grps_flds_cleanup(int unit)
{

    bcmecmp_member_dest_ltd_fields_t *lt_fids = NULL;   /* LTD info. */
    bcmecmp_member_dest_lt_field_attrs_t *grp_attrs = NULL; /* Single level grp attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing ECMP member1 LT logical fields data. */
    lt_fids = (bcmecmp_member_dest_ltd_fields_t *)
                  BCMECMP_MEMBER1_INFO_FIDS(unit);
    BCMECMP_FREE(lt_fids);

    /*
     * Free memory allocated for storing ECMP_MEMBER1 LT logical fields
     * attributes information.
     */
    grp_attrs = (bcmecmp_member_dest_lt_field_attrs_t *)
                    BCMECMP_MEMBER1_LT_FIELD_ATTRS_VOID_PTR(unit);
    BCMECMP_FREE(grp_attrs);

    SHR_FUNC_EXIT();
}
/*!
 * \brief Cleanup Trident4 device specific ECMP Table fields information.
 *
 * Cleanup ECMP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 * \param [in] ids  LT ID's for variant drivers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_fields_cleanup(int unit, const bcmecmp_drv_var_ids_t *ids)
{

    SHR_FUNC_ENTER(unit);

    if (ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) {
        /* Cleanup ECMP_LEVEL0 LT fields information. */
        bcm56780_a0_ecmp_olay_grps_flds_cleanup(unit);
    }


    if (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID) {
        /* Cleanup ECMP_LEVEL1 LT fields information. */
        bcm56780_a0_ecmp_ulay_grps_flds_cleanup(unit);
    }

        /* Cleanup ECMP_MEMBER0 LT fields information. */
    if (ids->ecmp_mlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
        bcm56780_a0_ecmp_member0_grps_flds_cleanup(unit);
    }

    if (ids->ecmp_mlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        /* Cleanup ECMP_MEMBER1 LT fields information. */
        bcm56780_a0_ecmp_member1_grps_flds_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries initialization function.
 *
 * Allocates memory to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_ecmp_db_clear(int unit)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /* Clear ECMP group logical table active array base pointers. */
    BCMECMP_FLEX_GRP_INFO_PTR(unit) = NULL;


    /* Clear Overlay Member table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL0);
    tbl_ptr->ent_arr = NULL;

    /* Clear Overlay Group table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    tbl_ptr->ent_arr = NULL;

    /* Clear Underlay Member table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1);
    tbl_ptr->ent_arr = NULL;

    /* Clear Underlay Group table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1);
    tbl_ptr->ent_arr = NULL;

    /* Clear weighted ECMP Member table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_WEIGHTED);
    tbl_ptr->ent_arr = NULL;

    /* Clear weighted ECMP Group table entries base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_WEIGHTED);
    tbl_ptr->ent_arr = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear device specfic ECMP tables range information .
 *
 * Clear ECMP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_hw_tbls_range_clear(int unit)
{

    SHR_FUNC_ENTER(unit);


    /* Clear device ECMP config register entry size in words value. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    /* Clear Max hardware tables entry size info. */
    BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

    /* Clear Overlay ECMP tables entry size value. */
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = 0;

    /* Clear Overlay ECMP Member table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = 0;

    /* Clear Overlay ECMP Group table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0) = 0;

    /* Clear Underlay ECMP tables entry size value. */
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = 0;

    /* Clear Underlay ECMP Member table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL1) = 0;

    /* Clear Underlay ECMP Group table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL1) = 0;

    /* Clear weighted ECMP tables entry size value. */
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = 0;

    /* Clear weighted ECMP Member table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = 0;

    /* Clear weighted ECMP Group table hardware data. */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = 0;

    /* TD4 device does not support Pre-IFP ECMP tables. */
    BCMECMP_TBL_ENT_SIZE
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL0) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL0) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_group, BCMECMP_GRP_TYPE_LEVEL1) = 0;
    BCMECMP_TBL_ENT_SIZE
        (unit, init_member, BCMECMP_GRP_TYPE_LEVEL1) = 0;

        SHR_FUNC_EXIT();
}

/*!
 * \brief Clear ECMP tables LRD-SID and DRD-SID information.
 *
 * Clear ECMP Group, Member and Round-Robin load balancing hardware tables
 * LRD-SID and DRD-SID information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_sids_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = INVALIDr;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    /*
     * Clear Post-IFP Overlay ECMP tables DRD-SID and LTD-SID
     * values.
     */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_LEVEL0) = BCMLTD_SID_INVALID;

    /*
     * Clear Post-IFP Underlay ECMP tables DRD-SID and LTD-SID
     * values.
     */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_LEVEL1) = BCMLTD_SID_INVALID;

    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMLTD_SID_INVALID;

    /* TD4 device does not support Pre-IFP ECMP tables. */
    BCMECMP_TBL_DRD_SID
        (unit, init_group,
         BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member,
         BCMECMP_GRP_TYPE_LEVEL0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_group,
         BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member,
         BCMECMP_GRP_TYPE_LEVEL1) = INVALIDm;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clears Trident4 device specific ECMP tables information.
 *
 * \param [in] unit Unit number.
 * \param [in] ids  LT ID's for variant drivers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_tables_clear(int unit, const bcmecmp_drv_var_ids_t *ids)
{

    SHR_FUNC_ENTER(unit);

    /* Clear ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_fields_cleanup(unit, ids));

    /* Clear ECMP hardware tables size and index range info. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_hw_tbls_range_clear(unit));

    /* Clear ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_sids_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries initialization function.
 *
 * Allocates memory to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_ecmp_abort_db_clear(int unit)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /* Clear ECMP group logical table active and backup arrays base pointers. */
    BCMECMP_FLEX_GRP_INFO_BK_PTR(unit) = NULL;

    /* Clear Overlay Member table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL0);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear Overlay Group table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear Underlay Member table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear Underlay Group table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted ECMP Member table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted ECMP Group table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted ECMP Member table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted ECMP Group table entries array base pointer. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED);
    tbl_ptr->ent_bk_arr = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries cleanup function.
 *
 * Cleanup memory allocated to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] ids  LT ID's for variant drivers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_ecmp_entries_cleanup(int unit, const bcmecmp_drv_var_ids_t *ids)
{
    SHR_FUNC_ENTER(unit);

    if (ids->ecmp_level0_ltid != (uint32_t) BCMECMP_INVALID) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_itbm_groups_cleanup(
                   unit, BCMECMP_GRP_TYPE_LEVEL0));
    }

    if (ids->ecmp_level1_ltid != (uint32_t) BCMECMP_INVALID) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_itbm_groups_cleanup(
                   unit, BCMECMP_GRP_TYPE_LEVEL1));
    }

    if (ids->ecmp_mlevel0_ltid != (uint32_t) BCMECMP_INVALID) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_member_dest_itbm_groups_cleanup(unit, 0));
    }

    if (ids->ecmp_mlevel1_ltid != (uint32_t) BCMECMP_INVALID) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_member_dest_itbm_groups_cleanup(unit, 1));
    }

    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_db_clear(unit));

    /* Clear atomic transaction related SW database info. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_abort_db_clear(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Trident4 device ECMP hardware table details cleanup function.
 *
 * Cleanup Trident4 ECMP hardware table information and free memory allocated
 * to store this information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_info_cleanup(int unit)
{
   bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    if (ids == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Cleanup memory allocated for ECMP Group and Member table entries info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_entries_cleanup(unit, ids));

    /* Clear ECMP tables information stored in SW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_tables_clear(unit, ids));

    /* Clean the ITBM lists handlers. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ecmp_itbm_lists_cleanup(unit));

    /* Free LT entry. */
    bcm56780_a0_ecmp_lt_entry_free(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Trident4 device ECMP resolution mode configuration function.
 *
 * Configures device ECMP resolution mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_mode_config(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Delete an ECMP Group from group and member tables using PIO mechanism.
 *
 * Deletes an ECMP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Post-IFP ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_grp_pio_del(int unit,
                             void *info)
{
    bcmecmp_flex_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    uint32_t arr_idx;             /* NHOP array index iterator. */
    int start_idx = BCMECMP_INVALID;   /* Member table start index. */
    uint32_t max_paths;                /* Group MAX_PATHS value. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    uint16_t ecmp_id = 0;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_ecmp_grp_pio_del.\n")));


    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_flex_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56780GrpPioDelPtBuf");

    if (lt_ent->glt_sid == ids->ecmp_mlevel0_ltid) {
        /* Get Group start index in ECMP Member table. */
        start_idx =  BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX(unit,
                                                    lt_ent->ecmp_id);

        /* Get MAX_PATHS value of the group. */
        max_paths = BCMECMP_MEMBER0_GRP_NUM_PATHS(unit, lt_ent->ecmp_id);
    } else if (lt_ent->glt_sid == ids->ecmp_mlevel1_ltid) {
        /* Get Group start index in ECMP Member table. */
        start_idx =  BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(unit,
                                                    lt_ent->ecmp_id);

        /* Get MAX_PATHS value of the group. */
        max_paths = BCMECMP_MEMBER1_GRP_NUM_PATHS(unit, lt_ent->ecmp_id);
    } else {
        /* Get Group start index in ECMP Member table. */
        start_idx =  BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);

        /* Get MAX_PATHS value of the group. */
        max_paths = BCMECMP_FLEX_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);
    }

    ecmp_id = lt_ent->ecmp_id;

    if (lt_ent->ecmp_id > (int)(BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0))) {
        ecmp_id -=
           BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.dyn_info.index = (start_idx);
    op_info.op_arg = lt_ent->op_arg;
    for (arr_idx = 0; arr_idx < max_paths; arr_idx++, op_info.dyn_info.index++) {
        /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        /* Write entry to Post-IFP Member table. */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    if ((lt_ent->glt_sid == ids->ecmp_mlevel0_ltid) ||
        (lt_ent->glt_sid == ids->ecmp_mlevel1_ltid)) {
        SHR_EXIT();
    }

    /* Delete Group entry in Post-IFP ECMP Group Table. */
    op_info.dyn_info.index = ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an ECMP Group into group and member tables using DMA mechanism.
 *
 * Installs an ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Post-IFP ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_grp_dma_ins(int unit,
                             void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_flex_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    uint32_t hw_npaths = 0;               /* HW (zero based) num_paths value. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t arr_idx;                     /* array index variable. */
    uint32_t val[1] = {0};                /* Temporary local variable. */
    uint32_t tmp_val = 0;                 /* Temporary local variable. */
    uint32_t ent_size = 0;                /* Table entry size. */
    uint32_t minbit = 0, maxbit = 0;
    int i;
    const bcmecmp_member_info_t *member_info;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    uint32_t num_paths = 0;
    bcmecmp_member_weight_grp_t *member_weight_grp = NULL;
    bool is_wecmp = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_flex_lt_entry_t *)info;
    member_info = drv_var->member0_info;
    if (lt_ent->ecmp_id > BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0)) {
        lt_ent->ecmp_id -=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        member_info = drv_var->member1_info;
    }

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56780GrpDmaInsPtEntBuf");

    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         lt_ent->max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56780GrpDmaInsPtDmaBuf");

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) &&
        (lt_ent->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)) {
        BCMECMP_ALLOC(member_weight_grp, sizeof(bcmecmp_member_weight_grp_t),
                      "bcmecmpBcm56780MemWtDist");

        BCMECMP_ALLOC(member_weight_grp->nhop_id_a,
                      (sizeof(bcmecmp_nhop_id_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistNhopA");

        BCMECMP_ALLOC(member_weight_grp->nhop_id_b,
                      (sizeof(bcmecmp_nhop_id_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistNhopB");

        BCMECMP_ALLOC(member_weight_grp->weight_a,
                      (sizeof(uint32_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistWtA");

        BCMECMP_ALLOC(member_weight_grp->weight_b,
                      (sizeof(uint32_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistWtB");

        /* Distribute weight. */
        SHR_IF_ERR_EXIT
            (bcmecmp_member_weight_distribute(unit,
                                              lt_ent->weight_nhop_index,
                                              lt_ent->weight,
                                              lt_ent->num_paths,
                                              BCM56780_MEMBER_WEIGHT_QUANT_FACTOR,
                                              member_weight_grp));
        is_wecmp = TRUE;
    }

    /*
     * Program the Member table entries in hardware.
     * Post-IFP Table.
     */

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    num_paths = lt_ent->num_paths;
    if (lt_ent->rh_mode) {
        /* In RH mode num_paths is the number of unique paths */
        num_paths = lt_ent->max_paths;
    }
    for (arr_idx = 0; arr_idx < num_paths; arr_idx++) {
        /* Set member field values. */
        for (i = 0; i < (member_info->flds_count); ++i) {
            minbit = member_info->flds[i].start_bit;
            maxbit = member_info->flds[i].end_bit;
            if (lt_ent->rh_mode) {
                val[0] = (uint32_t)lt_ent->rh_entries_arr[i][arr_idx];
            } else if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
                (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
                val[0] = (uint32_t)lt_ent->member1_field[i][arr_idx];
            } else {
                val[0] = (uint32_t)lt_ent->member0_field[i][arr_idx];
            }
            if (val[0] != (uint32_t)(BCMECMP_INVALID)) {
                shr_bitop_range_copy(&dma_buf[arr_idx * ent_size], minbit,
                             (SHR_BITDCL *) val, 0, maxbit - minbit + 1);
            }
        }

        if (is_wecmp) {
            /* NHI_A */
            val[0] = (uint32_t)member_weight_grp->nhop_id_a[arr_idx];
            shr_bitop_range_copy(&dma_buf[arr_idx * ent_size],
                                 BCM56780_MEMBER_NHI_A_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_NHI_A_MIN_SIZE);

            /* NHI_B */
            val[0] = (uint32_t)member_weight_grp->nhop_id_b[arr_idx];
            shr_bitop_range_copy(&dma_buf[arr_idx * ent_size],
                                 BCM56780_MEMBER_NHI_B_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_NHI_B_MIN_SIZE);

            /* WEIGHT */
            val[0] = (uint32_t)member_weight_grp->weight_a[arr_idx] - 1;
            shr_bitop_range_copy(&dma_buf[arr_idx * ent_size],
                                 BCM56780_MEMBER_WEIGHT_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_WEIGHT_MIN_SIZE);
        }
    }

    /* Prepare for PT entry write in Member table. */
    op_info.ecount = lt_ent->max_paths;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = mstart_idx;
    op_info.op_arg = lt_ent->op_arg;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    op_info.dma = TRUE;

    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, dma_buf));

    if ((lt_ent->glt_sid == ids->ecmp_mlevel0_ltid) ||
        (lt_ent->glt_sid == ids->ecmp_mlevel1_ltid)) {
        SHR_EXIT();
    }
    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Program ECMP Group table. */
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);

    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Clear entry buffer. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    if (lt_ent->num_paths != 0) {
        /* Set LB_MODE value. */
        if ((BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED != gtype) &&
            (BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED != gtype)) {
            tmp_val = lt_ent->lb_mode;
        } else {
            tmp_val = lt_ent->weighted_size;
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 grp_flds->lb_mode,
                                 &(tmp_val)));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\n inserted dma at %u %u.\n"), mstart_idx, lt_ent->mstart_idx));
        /*
         * If num_paths value is non-zero, decrement it by '1'
         * as hardware count field value is zero base.
         */
        if (lt_ent->num_paths && BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED != gtype
                              && BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED != gtype) {
            hw_npaths = (uint32_t) lt_ent->num_paths - 1;
        }
        /* Set members COUNT value. */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 grp_flds->count,
                                 &(hw_npaths)));

        /* Set Group's member table base pointer. */
        tmp_val = (uint32_t)mstart_idx;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 grp_flds->base_ptr,
                                 &(tmp_val)));

    }

    /* Write entry to ECMP GROUP table. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
        (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
        lt_ent->ecmp_id +=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

exit:
    if (member_weight_grp) {
        BCMECMP_FREE(member_weight_grp->nhop_id_a);
        BCMECMP_FREE(member_weight_grp->nhop_id_b);
        BCMECMP_FREE(member_weight_grp->weight_a);
        BCMECMP_FREE(member_weight_grp->weight_b);
    }
    BCMECMP_FREE(member_weight_grp);
    BCMECMP_FREE(pt_entry_buff);
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an ECMP Group into group and member tables using PIO mechanism.
 *
 * Installs an ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Post-IFP ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_grp_pio_ins(int unit,
                             void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_flex_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL, *entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t hw_npaths = 0;               /* HW (zero based) num_paths value. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t arr_idx;                /* NHOP array index variable. */
    uint32_t val[1] = {0};                     /* Temporary local variable. */
    uint32_t tmp_val = 0;                     /* Temporary local variable. */
    uint32_t minbit = 0, maxbit = 0;
    const bcmecmp_member_info_t *member_info;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    uint32_t num_paths = 0;
    bcmecmp_member_weight_grp_t *member_weight_grp = NULL;
    bool is_wecmp = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_flex_lt_entry_t *)info;
    member_info = drv_var->member0_info;
    if (lt_ent->ecmp_id > BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0)) {
        lt_ent->ecmp_id -=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        member_info = drv_var->member1_info;
    }

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56780GrpPioInsPtBuf");

    BCMECMP_ALLOC(entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56780GrpPioInsPtBuf");

    if (lt_ent->ecmp_id > BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0)) {
        lt_ent->ecmp_id -=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) &&
        (lt_ent->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)) {
        BCMECMP_ALLOC(member_weight_grp, sizeof(bcmecmp_member_weight_grp_t),
                      "bcmecmpBcm56780MemWtDist");

        BCMECMP_ALLOC(member_weight_grp->nhop_id_a,
                      (sizeof(bcmecmp_nhop_id_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistNhopA");

        BCMECMP_ALLOC(member_weight_grp->nhop_id_b,
                      (sizeof(bcmecmp_nhop_id_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistNhopB");

        BCMECMP_ALLOC(member_weight_grp->weight_a,
                      (sizeof(uint32_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistWtA");

        BCMECMP_ALLOC(member_weight_grp->weight_b,
                      (sizeof(uint32_t) * BCM56780_MEMBER_NHOP_ARRAY_SIZE),
                      "bcmecmpBcm56780MemWtDistWtB");

        /* Distribute weight. */
        SHR_IF_ERR_EXIT
            (bcmecmp_member_weight_distribute(unit,
                                              lt_ent->weight_nhop_index,
                                              lt_ent->weight,
                                              lt_ent->num_paths,
                                              BCM56780_MEMBER_WEIGHT_QUANT_FACTOR,
                                              member_weight_grp));
        is_wecmp = TRUE;
    }

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = lt_ent->op_arg;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.dyn_info.index = (mstart_idx);
    num_paths = lt_ent->num_paths;
    if (lt_ent->rh_mode) {
        /* In RH mode num_paths is the number of unique paths */
        num_paths = lt_ent->max_paths;
    }
    for (arr_idx = 0; arr_idx < num_paths; arr_idx++, op_info.dyn_info.index++) {
        int i;
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        sal_memset(entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));
        for (i = 0; i < (member_info->flds_count); ++i) {
            minbit = member_info->flds[i].start_bit;
            maxbit = member_info->flds[i].end_bit;
            if (lt_ent->rh_mode) {
                val[0] = (uint32_t)lt_ent->rh_entries_arr[i][arr_idx];
            } else if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
                       (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
                val[0] = (uint32_t)lt_ent->member1_field[i][arr_idx];
            } else {
                val[0] = (uint32_t)lt_ent->member0_field[i][arr_idx];
            }
            shr_bitop_range_copy(entry_buff, minbit, val, 0, maxbit - minbit + 1);
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\n pio_ins fld %d min %u max %u val %u.\n"), i, minbit, maxbit, val[0]));
        }

        if (is_wecmp) {
            /* NHI_A */
            val[0] = (uint32_t)member_weight_grp->nhop_id_a[arr_idx];
            shr_bitop_range_copy(entry_buff,
                                 BCM56780_MEMBER_NHI_A_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_NHI_A_MIN_SIZE);

            /* NHI_B */
            val[0] = (uint32_t)member_weight_grp->nhop_id_b[arr_idx];
            shr_bitop_range_copy(entry_buff,
                                 BCM56780_MEMBER_NHI_B_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_NHI_B_MIN_SIZE);

            /* WEIGHT */
            val[0] = (uint32_t)member_weight_grp->weight_a[arr_idx] - 1;
            shr_bitop_range_copy(entry_buff,
                                 BCM56780_MEMBER_WEIGHT_MIN_BIT,
                                 (SHR_BITDCL *) val, 0,
                                 BCM56780_MEMBER_WEIGHT_MIN_SIZE);
        }

        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "\nWriting %u at %d"), entry_buff[0], arr_idx));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 pt_entry_buff,
                                 DATAf,
                                 (entry_buff)));

        /* Prepare for PT entry write in Member table. */
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.op_arg = lt_ent->op_arg;
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        SHR_IF_ERR_EXIT
            (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }


    /* Program ECMP Group table. */
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Clear entry buffer. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    /* Set LB_MODE vlaue. */
    if ((BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED != gtype) && (BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED != gtype)) {
        tmp_val = lt_ent->lb_mode;
    } else {
        tmp_val = lt_ent->weighted_size;
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(tmp_val)));

    /*
     * If num_paths value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (lt_ent->num_paths && (BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED != gtype) &&
                             (BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED != gtype)) {
        hw_npaths = (uint32_t) lt_ent->num_paths - 1;
    }
    /* Set members COUNT value. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    /* Set Group's member table base pointer. */
    tmp_val = (uint32_t)mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(tmp_val)));

    /* Write entry to L3_ECMP_GROUP table. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
        (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
        lt_ent->ecmp_id +=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

exit:
    if (member_weight_grp) {
        BCMECMP_FREE(member_weight_grp->nhop_id_a);
        BCMECMP_FREE(member_weight_grp->nhop_id_b);
        BCMECMP_FREE(member_weight_grp->weight_a);
        BCMECMP_FREE(member_weight_grp->weight_b);
    }
    BCMECMP_FREE(member_weight_grp);
    BCMECMP_FREE(pt_entry_buff);
    BCMECMP_FREE(entry_buff);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert ECMP Group logical table entry into hardware tables.
 *
 * Installs ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_grp_ins(int unit,
                         void *info)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (BCMECMP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_grp_dma_ins(unit, info));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_grp_pio_ins(unit, info));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ecmp_grp_dma_get(int unit,
                             void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_flex_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t arr_idx;                     /* array index variable. */
    uint32_t val[1] = {0};                /* Temporary local variable. */
    uint32_t ent_size = 0;                /* Table entry size. */
    uint32_t minbit = 0, maxbit = 0;
    int i;
    const bcmecmp_member_info_t *member_info;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    uint32_t ecmp_id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_flex_lt_entry_t *)info;
    ecmp_id = lt_ent->ecmp_id;
    member_info = drv_var->member0_info;
    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;


    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56780GrpDmaGetPtEntBuf");

    lt_ent->grp_type = BCMECMP_FLEX_GRP_TYPE(unit, ecmp_id);
    lt_ent->num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);
    lt_ent->max_paths = BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id);
    lt_ent->rh_num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);
    lt_ent->rh_mode = BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id);
    lt_ent->rh_rand_seed = BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id);
    lt_ent->mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
    lt_ent->lb_mode = BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id);
    lt_ent->glt_sid = BCMECMP_FLEX_GRP_LT_SID(unit, ecmp_id);

    if (lt_ent->ecmp_id > BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0)) {
        lt_ent->ecmp_id -=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        member_info = drv_var->member1_info;
    }

    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         lt_ent->max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56780GrpDmaInsPtDmaBuf");


    /* member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    /*
     * Prepare for member table entries read operation
     * i.e. BCMPTM_OP_READ.
     */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_READ;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = lt_ent->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->max_paths;
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                    (unit, member, gtype) * op_info.ecount);
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_read(unit, &op_info, dma_buf));

    for (arr_idx = 0; arr_idx < lt_ent->max_paths; arr_idx++) {
        /* Set member field values. */
        for (i = 0; i < (member_info->flds_count); ++i) {
            minbit = member_info->flds[i].start_bit;
            maxbit = member_info->flds[i].end_bit;

            shr_bitop_range_copy((SHR_BITDCL *)val, 0, &dma_buf[arr_idx * ent_size], minbit,
                                 maxbit - minbit + 1);
            if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
                (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
                lt_ent->member1_field[i][arr_idx] = val[0];
            } else {
                lt_ent->member0_field[i][arr_idx] = val[0];
            }
            if (lt_ent->rh_mode) {
                lt_ent->rh_entries_arr[i][arr_idx] = val[0];
            }
        }
    }
exit:
    BCMECMP_FREE(pt_entry_buff);
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete ECMP Group logical table entry from hardware tables.
 *
 * Deletes ECMP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_grp_del(int unit,
                         void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_ecmp_grp_del.\n")));

    if (BCMECMP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_grp_pio_del(unit, info));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_grp_pio_del(unit, info));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to ECMP Logical Table entry data.
 * \param [in] sfrag Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_memb_dma_cpy(int unit,
                              const void *mdest,
                              const void *msrc)
{
    const bcmecmp_grp_defrag_t *sfrag = (bcmecmp_grp_defrag_t *)msrc;
                                        /* Source entries block. */
    const bcmecmp_grp_defrag_t *dfrag = (bcmecmp_grp_defrag_t *)mdest;
                                        /* Destination entries block. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_pt_op_info_t op_info;         /* PT operation info structure. */
    bcmecmp_wal_info_t *wali = NULL; /* ECMP WAL entries info. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get the group type. */
    gtype = sfrag->gtype;

    /* Allocate the DMA buffer for reading Member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         sfrag->gsize);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56780L3EcmpDmaCpy");

    /*
     * Prepare for member table entries read operation
     * i.e. BCMPTM_OP_READ.
     */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = sfrag->op_arg;
    op_info.req_lt_sid = sfrag->glt_sid;
    op_info.op = BCMPTM_OP_READ;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sfrag->gsize);
    op_info.dyn_info.index = sfrag->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = sfrag->gsize;
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                    (unit, member, gtype) * op_info.ecount);
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_read(unit, &op_info, dma_buf));

    /* Update WALI member table entries READ info. */
    if (wali->mwsize == 0) {
        wali->mwsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    }
    wali->mr_ecount += sfrag->gsize;

    /*
     * Prepare for L3_ECMP member table entries write operation
     * i.e. BCMPTM_OP_WRITE.
     */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = dfrag->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = dfrag->gsize;
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                    (unit, member, gtype) * op_info.ecount);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, dma_buf));

    BCMECMP_FREE(dma_buf);
    /* Update WALI member table entries write count. */
    wali->mw_ecount += dfrag->gsize;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to ECMP Logical Table entry data.
 * \param [in] sfrag Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_memb_pio_cpy(int unit,
                              const void *mdest,
                              const void *msrc)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * Copy ECMP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to ECMP Logical Table entry data.
 * \param [in] sfrag Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_memb_cpy(int unit,
                          const void *mdest,
                          const void *msrc)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mdest, SHR_E_PARAM);
    SHR_NULL_CHECK(msrc, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    if (BCMECMP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_memb_dma_cpy(unit, mdest, msrc));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_ecmp_memb_pio_cpy(unit, mdest, msrc));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear ECMP member tables.
 *
 * Clear ECMP member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_memb_dma_clr(int unit,
                              const void *mfrag)
{
    const bcmecmp_grp_defrag_t *mclr = (bcmecmp_grp_defrag_t *)mfrag;
    uint32_t *dma_buf = NULL;          /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;         /* Local DMA buffer alloc size. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_wal_info_t *wali = NULL; /* ECMP WAL entries info. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mclr, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcm56780_a0_ecmp_memb_dma_clr.\n")));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get ECMP Group type. */
    gtype = mclr->gtype;

    /* Allocate buffer for L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         mclr->gsize);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56780L3EcmpDmaClr");

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
    op_info.req_lt_sid = mclr->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = mclr->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = mclr->gsize;
    op_info.op_arg = mclr->op_arg;
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                    (unit, member, gtype) * op_info.ecount);

    /* Write entry to Post-IFP Member table. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, dma_buf));

    BCMECMP_FREE(dma_buf);
    /* Update WALI member table entries write count. */
    wali->mclr_ecount += mclr->gsize;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear given entries in member tables.
 *
 * Clear entries in member tables.
 *
 * \param [in] unit Unit number.
 * \param [in] mfrag Pointer to fragment which needs to be cleared.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_memb_clr(int unit,
                          const void *mfrag)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    SHR_ERR_EXIT
        (bcm56780_a0_ecmp_memb_dma_clr(unit, mfrag));

exit:
    SHR_FUNC_EXIT();
}

/* This function is not used. Can be removed. */
/*!
 * \brief Update Group's member table start index in HW.
 *
 * Update Group's member table start index in HW
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ecmp_gmstart_updt(int unit,
                              const void *ginfo)
{

    const bcmecmp_grp_defrag_t *gupdt = (bcmecmp_grp_defrag_t *)ginfo;
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t val = 0;                     /* Temporary local variable. */
    bcmecmp_wal_info_t *wali = NULL; /* ECMP WAL entries info. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ginfo, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get the ECMP group type and validate it. */
    gtype = gupdt->gtype;
    if (gtype < BCMECMP_GRP_TYPE_SINGLE || gtype > BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid GTYPE=%d.\n"), gtype));
    }

    /* Get the ECMP_ID and validate it. */
    ecmp_id = gupdt->ecmp_id;
    if (ecmp_id > BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_LEVEL0)) {
        ecmp_id -=
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
    }

    if (ecmp_id < BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype)
        || ecmp_id > BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid ECMP_ID=%d.\n"), ecmp_id));
    }

    /* Verify the group is in use. */

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, gupdt->ecmp_id, FALSE, 0, gtype, &in_use,
                            &comp_id));

    if (!in_use) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not in-use.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmp56780GrpMstartUpdt");

    /* Prepare for group table entry read operation. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = gupdt->op_arg;
    op_info.req_lt_sid = gupdt->glt_sid;
    op_info.dyn_info.index = ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.op = BCMPTM_OP_READ;

    /* Clear the entry buffer for storing the new group table entry. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_read(unit, &op_info, pt_entry_buff));
    if (wali->gwsize == 0) {
        wali->gwsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    }
    wali->gr_ecount += 1;

    /* Prepare for table entry write operation. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = gupdt->op_arg;
    op_info.req_lt_sid = gupdt->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    val = (uint32_t)gupdt->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    wali->gw_ecount += 1;

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_grp_member_fields_count_get(int unit, void *lt_entry)
{
    bcmecmp_flex_lt_entry_t *lt_ent = (bcmecmp_flex_lt_entry_t *)lt_entry;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;

    ids = drv_var->ids;
    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_flex_lt_entry_t *)lt_entry;
    return (((lt_ent->glt_sid == ids->ecmp_level0_ltid) ||
             (lt_ent->glt_sid == ids->ecmp_mlevel0_ltid) ||
             (lt_ent->glt_sid == ids->ecmp_wlevel0_ltid))?
             drv_var->member0_info->flds_count:
             drv_var->member1_info->flds_count);
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Attach device ECMP driver functions.
 *
 * Attach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56780_a0_ecmp_attach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_ecmp_attach.\n")));

    SHR_NULL_CHECK(ecmp_drv, SHR_E_FAIL);

    /* Initialize device ECMP feature function pointers. */
    BCMECMP_FNCALL_PTR(unit) = ecmp_drv;

    /* Setup device ECMP component initialization function. */
    BCMECMP_FNCALL(unit, info_init) = &bcm56780_a0_ecmp_info_init;

    /* Setup device ECMP component clean-up function. */
    BCMECMP_FNCALL(unit, info_cleanup) = &bcm56780_a0_ecmp_info_cleanup;

    /* Setup device ECMP component initialization function. */
    BCMECMP_FNCALL(unit, rh_pre_config) = &bcmecmp_flex_rh_groups_preconfig;

    /* Setup device ECMP resoultion mode hardware config function. */
    BCMECMP_FNCALL(unit, mode_config) = &bcm56780_a0_ecmp_mode_config;

    /* Initialize device ECMP group management function pointers. */
    BCMECMP_FNCALL(unit, grp_ins) = &bcm56780_a0_ecmp_grp_ins;
    BCMECMP_FNCALL(unit, grp_del) = &bcm56780_a0_ecmp_grp_del;
    BCMECMP_FNCALL(unit, grp_get) = &bcm56780_a0_ecmp_grp_dma_get;
    BCMECMP_FNCALL(unit, rh_grp_ins) = NULL;
    BCMECMP_FNCALL(unit, rh_grp_del) = NULL;
    BCMECMP_FNCALL(unit, rh_grp_get) = NULL;
    BCMECMP_FNCALL(unit, memb_cpy) = &bcm56780_a0_ecmp_memb_cpy;
    BCMECMP_FNCALL(unit, memb_clr) = &bcm56780_a0_ecmp_memb_clr;
    BCMECMP_FNCALL(unit, gmstart_updt) = &bcm56780_a0_ecmp_gmstart_updt;
    BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill) =
                            &bcm56780_a0_level0_group_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill) =
                            &bcm56780_a0_level1_group_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_member_dest_lt_fields_fill) =
                            &bcm56780_a0_flex_member_dest_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_member_fields_count_get) =
                            &bcm56780_a0_grp_member_fields_count_get;
    ecmp_drv->shr_grp_type_get = NULL;
    ecmp_drv->weight_size_to_max_paths = NULL;
    ecmp_drv->itbm_memb_lists_update = NULL;
    ecmp_drv->memb_bank_set= NULL;
    ecmp_drv->memb_bank_get = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deatch device ECMP driver functions.
 *
 * Detach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56780_a0_ecmp_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_ecmp_detach.\n")));

    if (BCMECMP_FNCALL_PTR(unit)) {
        /* Clear ECMP group management function pointers. */
        BCMECMP_FNCALL(unit, grp_get) = NULL;
        BCMECMP_FNCALL(unit, grp_del) = NULL;
        BCMECMP_FNCALL(unit, grp_ins) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_ins) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_del) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_get) = NULL;
        BCMECMP_FNCALL(unit, memb_cpy) = NULL;
        BCMECMP_FNCALL(unit, memb_clr) = NULL;
        BCMECMP_FNCALL(unit, gmstart_updt) = NULL;

        /* Clear device ECMP feature function pointers. */
        BCMECMP_FNCALL(unit, mode_config) = NULL;
        BCMECMP_FNCALL(unit, info_cleanup) = NULL;
        BCMECMP_FNCALL(unit, info_init) = NULL;
        BCMECMP_FNCALL(unit, rh_pre_config) = NULL;
        BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill) = NULL;
        BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill) = NULL;
        BCMECMP_FNCALL(unit, grp_member_dest_lt_fields_fill) = NULL;
        BCMECMP_FNCALL(unit, grp_member_fields_count_get) = NULL;
        BCMECMP_FNCALL_PTR(unit) = NULL;
    }

    SHR_FUNC_EXIT();
}
