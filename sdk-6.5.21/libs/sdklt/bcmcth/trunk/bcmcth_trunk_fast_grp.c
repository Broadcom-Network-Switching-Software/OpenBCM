/*! \file bcmcth_trunk_fast_grp.c
 *
 * Purpose:     handler implementation for TRUNK_FAST LT.
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
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_trunk_types.h>
#include <bcmcth/bcmcth_trunk_fast_grp.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_drv.h>

/*******************************************************************************
 * Private functions
 */
static bcmcth_trunk_fgrp_bk_t *lth_fgrp_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK bookkeeping temporary structure. */
static bcmcth_trunk_fgrp_bk_t * lth_fgrp_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK fast group SW bitmap changed in one commit. */
SHR_BITDCL *bcmcth_trunk_fgrp_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk fast group state. */
#define GRP_FAST_HA(unit)                 lth_fgrp_bk[unit]

/*! Pointer to trunk fast group temporary state. */
#define GRP_FAST_TEMP(unit)               lth_fgrp_bk_temp[unit]

/*! Pointer to trunk fast group temporary state. */
#define GRP_FAST_DIFF(unit)               bcmcth_trunk_fgrp_bitmap_diff[unit]


#define GRP_FAST_MAX_MEMBER(unit)         GRP_FAST_HA(unit)->max_members

#define BCMCTH_TRUNK_GRP_INFO(_u_) \
    (GRP_FAST_TEMP(unit)->grp)
#define BCMCTH_TRUNK_GRP(_u_, _idx_) \
    (&(GRP_FAST_TEMP(unit)->grp[_idx_]))
#define GRP_EXIST(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->inserted)
#define GRP_LB_MODE(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->lb_mode)
#define GRP_UC_RTAG(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_rtag)
#define GRP_MAX_MEMBERS(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_max_members)
#define GRP_BASE_PTR(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_base_ptr)
#define GRP_UC_AGM_POOL(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_agm_pool)
#define GRP_UC_AGM_ID(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_agm_id)
#define GRP_UC_MEMBER_CNT(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_cnt)
#define GRP_UC_MEMBER_MODID_PTR(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_modid)
#define GRP_UC_MEMBER_MODPORT_PTR(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_modport)
#define GRP_UC_MEMBER_MODID(_u_, _idx_, _px_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_modid[_px_])
#define GRP_UC_MEMBER_MODPORT(_u_, _idx_, _px_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->uc_modport[_px_])

#define GRP_NONUC_MEMBER_CNT(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->nonuc_cnt)
#define GRP_NONUC_MEMBER_MODID_PTR(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->nonuc_modid)
#define GRP_NONUC_MEMBER_MODPORT_PTR(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->nonuc_modport)
#define GRP_NONUC_MEMBER_MODID(_u_, _idx_, _px_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->nonuc_modid[_px_])
#define GRP_NONUC_MEMBER_MODPORT(_u_, _idx_, _px_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->nonuc_modport[_px_])

#define GRP_ING_EFLEX_ACTION_ID(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->ing_eflex_action_id)
#define GRP_ING_EFLEX_OBJECT(_u_, _idx_)  \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->ing_eflex_object)

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK


static int
bcmcth_trunk_fast_field_parse(int unit, const bcmltd_fields_t *in,
                              bcmcth_trunk_fast_group_param_t **param)
{
    uint32_t idx = 0, id;
    uint32_t arr_idx, fval;
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    if ((in == NULL) || (param == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    (*param) = sal_alloc(sizeof(bcmcth_trunk_fast_group_param_t),
                         "bcmcthTrunkFastGrpParam");
    SHR_NULL_CHECK((*param), SHR_E_MEMORY);
    sal_memset((*param), 0, sizeof(bcmcth_trunk_fast_group_param_t));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmcth_trunk_fast_field_parse:CNT=%d.\n"),
                    (uint32_t)(in->count)));

    for (idx = 0; idx < in->count; idx++) {
        id      = in->field[idx]->id;
        arr_idx = in->field[idx]->idx;
        fval    = in->field[idx]->data;
        if (in->field[idx]->flags & SHR_FMM_FIELD_DEL) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmlrd_field_default_get(unit, TRUNK_FASTt, id,
                                         1, &def_val, &num));
            fval = def_val;
        }
        switch (id) {
            case TRUNK_FASTt_TRUNK_IDf:
                (*param)->id = fval;
                (*param)->id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_TRUNK_IDf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_LB_MODEf:
                (*param)->lb_mode = fval;
                (*param)->lb_mode_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_LB_MODEf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_UC_RTAGf:
                (*param)->uc_rtag = fval;
                (*param)->uc_rtag_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_RTAGf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_CNTf:
                if (fval > GRP_FAST_MAX_MEMBER(unit)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_cnt = fval;
                (*param)->uc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MEMBER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_MODIDf:
                if (arr_idx >= BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modid[arr_idx] = fval;
                (*param)->uc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MODIDf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_MODPORTf:
                if (arr_idx >= BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modport[arr_idx] = fval;
                (*param)->uc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_MON_AGM_POOLf:
                (*param)->uc_agm_pool = fval;
                (*param)->uc_agm_pool_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_MON_AGM_POOLf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_MON_AGM_IDf:
                (*param)->uc_agm_id = fval;
                (*param)->uc_agm_id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_MON_AGM_IDf = %d.\n"), fval));
                break;
             case TRUNK_FASTt_NONUC_MEMBER_CNTf:
                if (fval > GRP_FAST_MAX_MEMBER(unit)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_cnt = fval;
                (*param)->nonuc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MEMBER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FASTt_NONUC_MEMBER_MODIDf:
                if (arr_idx >= BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "NONUC_MODID[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modid[arr_idx] = fval;
                (*param)->nonuc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MODIDf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_NONUC_MEMBER_MODPORTf:
                if (arr_idx >= BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "NONUC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modport[arr_idx] = fval;
                (*param)->nonuc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_TRUNK_CTR_ING_EFLEX_ACTION_IDf:
                (*param)->ing_eflex_action_id = fval;
                (*param)->ing_eflex_action_id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_TRUNK_CTR_ING_EFLEX_ACTION_IDf = %d.\n"),
                                fval));
                break;
            case TRUNK_FASTt_CTR_ING_EFLEX_OBJECTf:
                (*param)->ing_eflex_object = fval;
                (*param)->ing_eflex_object_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_CTR_ING_EFLEX_OBJECTf = %d.\n"),
                                fval));
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }


exit:
    if (SHR_FUNC_ERR()) {
        if ((param != NULL) && ((*param) != NULL)) {
            sal_free((*param));
            (*param) = NULL;
        }
    }
    SHR_FUNC_EXIT();
}


static int
bcmcth_trunk_fast_grp_to_api(int unit,
                             bcmcth_trunk_fast_group_t *grp,
                             bcmltd_fields_t *out)
{
    uint32_t idx = 0, px;
    uint32_t num_actual = 0;
    uint64_t def_val = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if ((!grp) || (!out)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Invalid map fields should not be returned. */
    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt, TRUNK_FASTt_LB_MODEf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_LB_MODEf;
        out->field[idx]->data = grp->lb_mode;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt, TRUNK_FASTt_UC_RTAGf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_UC_RTAGf;
        out->field[idx]->data = grp->uc_rtag;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_UC_MEMBER_CNTf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_CNTf;
        out->field[idx]->data = grp->uc_cnt;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_UC_MEMBER_MODIDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* All array member should be returned. */
        for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_MODIDf;
            out->field[idx]->data = grp->uc_modid[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_UC_MEMBER_MODPORTf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_MODPORTf;
            out->field[idx]->data = grp->uc_modport[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt, TRUNK_FASTt_MON_AGM_POOLf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_MON_AGM_POOLf;
        out->field[idx]->data = grp->uc_agm_pool;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt, TRUNK_FASTt_MON_AGM_IDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_MON_AGM_IDf;
        out->field[idx]->data = grp->uc_agm_id;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_NONUC_MEMBER_CNTf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* nonuc member fields. */
        out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_CNTf;
        out->field[idx]->data = grp->nonuc_cnt;
        out->field[idx]->idx  = 0;
        idx++;
    }

    /* Invalid map fields should not be returned. */
    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_NONUC_MEMBER_MODIDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* All array member should be returned. */
        for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_MODIDf;
            out->field[idx]->data = grp->nonuc_modid[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_NONUC_MEMBER_MODPORTf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_MODPORTf;
            out->field[idx]->data = grp->nonuc_modport[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_TRUNK_CTR_ING_EFLEX_ACTION_IDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_TRUNK_CTR_ING_EFLEX_ACTION_IDf;
        out->field[idx]->data = grp->ing_eflex_action_id;
        out->field[idx]->idx  = 0;
        idx++;
    }

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_CTR_ING_EFLEX_OBJECTf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_CTR_ING_EFLEX_OBJECTf;
        out->field[idx]->data = grp->ing_eflex_object;
        out->field[idx]->idx  = 0;
        idx++;
    }

    out->count = idx;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_fast_sw_update(int unit, bcmcth_trunk_fast_group_param_t *param,
                            bcmcth_trunk_fast_group_t *grp)
{
    int px = 0;

    SHR_FUNC_ENTER(unit);
    if (param == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    grp->inserted = TRUE;

    if (param->lb_mode_valid) {
        grp->lb_mode = param->lb_mode;
    }
    if (param->uc_rtag_valid) {
        grp->uc_rtag = param->uc_rtag;
    }
    if (param->uc_agm_pool_valid) {
        grp->uc_agm_pool = param->uc_agm_pool;
    }
    if (param->uc_agm_id_valid) {
        grp->uc_agm_id = param->uc_agm_id;
    }

    if (param->uc_cnt_valid) {
        grp->uc_cnt = param->uc_cnt;
    }

    if (param->nonuc_cnt_valid) {
        grp->nonuc_cnt = param->nonuc_cnt;
    }

    for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
        if (param->uc_modid_valid[px]) {
            grp->uc_modid[px] = param->uc_modid[px];
        }
        if (param->uc_modport_valid[px]) {
            grp->uc_modport[px] = param->uc_modport[px];
        }
    }
    for (px = 0; px < BCMCTH_TRUNK_FAST_MAX_MEMBERS; px++) {
        if (param->nonuc_modid_valid[px]) {
            grp->nonuc_modid[px] = param->nonuc_modid[px];
        }
        if (param->nonuc_modport_valid[px]) {
            grp->nonuc_modport[px] = param->nonuc_modport[px];
        }
    }

    if (param->ing_eflex_action_id_valid) {
        grp->ing_eflex_action_id = param->ing_eflex_action_id;
    }
    if (param->ing_eflex_object_valid) {
        grp->ing_eflex_object = param->ing_eflex_object;
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_fast_grp_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_fast_group_param_t *param = NULL;
    int trunk_id, old_uc_cnt;
    int uc_cnt;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!lth_fgrp_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
       bcmcth_trunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
       SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    if ((trunk_id < 0) || (trunk_id >= BCMCTH_TRUNK_MAX_TRUNK)) {
       SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            if (GRP_EXIST(unit, trunk_id)) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }

            /* The member cnt must be bigger than 0. */
            if (param->uc_cnt > BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
               return SHR_E_PARAM;
            }
            break;
        case BCMLT_OPCODE_LOOKUP:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_DELETE:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_UPDATE:
            if (!GRP_EXIST(unit, trunk_id)) {
               LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            old_uc_cnt = GRP_UC_MEMBER_CNT(unit, trunk_id);
            if (param->uc_cnt_valid) {
                uc_cnt = param->uc_cnt;
            } else {
                uc_cnt = old_uc_cnt;
            }

            if (uc_cnt > BCMCTH_TRUNK_FAST_MAX_MEMBERS) {
               return SHR_E_PARAM;
            }
            break;
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
   }

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();

}

int
bcmcth_trunk_fast_grp_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_fast_field_parse(unit, in, &param));
    trunk_id = param->id;

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_fast_grp_add(unit, op_arg,
                                       TRUNK_FASTt, param));

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_fast_sw_update(unit, param,
                                     BCMCTH_TRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_fast_grp_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
       bcmcth_trunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;

    SHR_IF_ERR_EXIT
       (bcmcth_trunk_fast_grp_to_api(unit,
                                    BCMCTH_TRUNK_GRP(unit, trunk_id), out));

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();

}

int
bcmcth_trunk_fast_grp_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_fast_group_param_t *param = NULL;
    int trunk_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(bcmcth_trunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_fast_grp_del(unit, op_arg,
                                       TRUNK_FASTt, param,
                                       BCMCTH_TRUNK_GRP(unit, trunk_id)));

    sal_memset(BCMCTH_TRUNK_GRP(unit, trunk_id), 0,
               sizeof(bcmcth_trunk_fast_group_t));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();
}


int
bcmcth_trunk_fast_grp_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_fast_field_parse(unit, in, &param));

    trunk_id = param->id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_fast_grp_update(unit, op_arg,
                                          TRUNK_FASTt, param,
                                          BCMCTH_TRUNK_GRP(unit, trunk_id)));

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_fast_sw_update(unit, param,
                                     BCMCTH_TRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_first(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t trunk_id, min, max;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(in);

    if (!lth_fgrp_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNK_FASTt,
                                            TRUNK_FASTt_TRUNK_IDf,
                                            &min, &max));

    for (trunk_id = min; trunk_id <= max; trunk_id++) {
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FASTt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_fast_grp_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FASTt_TRUNK_IDf;
            out->field[out->count]->data = trunk_id;
            out->count++;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_next(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t trunk_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!lth_fgrp_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNK_FASTt,
                                            TRUNK_FASTt_TRUNK_IDf,
                                            &min, &max));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_get(unit, in, TRUNK_FASTt_TRUNK_IDf, &trunk_id));

    trunk_id++;

    /* Stop traverse when exceeding the max trunk_id group entry. */
    for ( ; trunk_id <= max; trunk_id++) {
        /* Exit the current iteration once we get a valid entry. */
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FASTt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_fast_grp_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FASTt_TRUNK_IDf;
            out->field[out->count]->data = trunk_id;
            out->count++;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_commit(int unit,
                             uint32_t trans_id,
                             const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_FAST_HA(unit)) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMCTH_TRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_FAST_DIFF(unit), idx)) {
            GRP_FAST_HA(unit)->grp[idx] = GRP_FAST_TEMP(unit)->grp[idx];
        }
    }

    sal_memset(GRP_FAST_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_abort(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_FAST_HA(unit)) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMCTH_TRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_FAST_DIFF(unit), idx)) {
            GRP_FAST_TEMP(unit)->grp[idx] = GRP_FAST_HA(unit)->grp[idx];
        }
    }

    sal_memset(GRP_FAST_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!GRP_FAST_HA(unit)) {
        alloc_size = sizeof(bcmcth_trunk_fgrp_bk_t);
        SHR_ALLOC(GRP_FAST_HA(unit), alloc_size, "bcmcthTrunkFgrpBk");
        SHR_NULL_CHECK(GRP_FAST_HA(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_HA(unit), 0, alloc_size);

        SHR_ALLOC(GRP_FAST_TEMP(unit), alloc_size,
                  "bcmcthTrunkFgrpBkTemp");
        SHR_NULL_CHECK(GRP_FAST_TEMP(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_TEMP(unit), 0, alloc_size);


        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_drv_fast_grp_init(unit, GRP_FAST_TEMP(unit)));
        GRP_FAST_HA(unit)->max_members = GRP_FAST_TEMP(unit)->max_members;

        /* HA structure. */
        ha_req_size = BCMCTH_TRUNK_MAX_TRUNK * sizeof(bcmcth_trunk_fast_group_t);
        ha_alloc_size = ha_req_size;
        GRP_FAST_HA(unit)->grp = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMCTH_TRUNK_FAST_GRP_SUB_COMP_ID,
                                             "bcmcthTrunkFastGroup",
                                             &ha_alloc_size);
        SHR_NULL_CHECK(GRP_FAST_HA(unit)->grp, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(GRP_FAST_HA(unit)->grp, 0, ha_alloc_size);
        }

        /* Temporary structure. */
        alloc_size = BCMCTH_TRUNK_MAX_TRUNK * sizeof(bcmcth_trunk_fast_group_t);
        SHR_ALLOC(GRP_FAST_TEMP(unit)->grp, alloc_size,
                  "bcmcthTrunkFgrpInfoTemp");
        SHR_NULL_CHECK(GRP_FAST_TEMP(unit)->grp, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_FAST_TEMP(unit)->grp, 0, alloc_size);
        } else {
            sal_memcpy(GRP_FAST_TEMP(unit)->grp,
                       GRP_FAST_HA(unit)->grp,
                       alloc_size);
        }

        /* TRUNK_FAST bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);
        SHR_ALLOC(GRP_FAST_DIFF(unit), alloc_size, "bcmcthTrunkFgrpBmpDiff");
        SHR_NULL_CHECK(GRP_FAST_DIFF(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_DIFF(unit), 0, alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (GRP_FAST_HA(unit) != NULL) {
            if (GRP_FAST_HA(unit)->grp != NULL) {
                shr_ha_mem_free(unit, GRP_FAST_HA(unit)->grp);
                GRP_FAST_HA(unit)->grp = NULL;
            }
            SHR_FREE(GRP_FAST_HA(unit));
        }

        if (GRP_FAST_TEMP(unit) != NULL) {
            SHR_FREE(GRP_FAST_TEMP(unit)->grp);
        }
        SHR_FREE(GRP_FAST_TEMP(unit));
        SHR_FREE(GRP_FAST_DIFF(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_fast_grp_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(GRP_FAST_HA(unit));

    if (GRP_FAST_TEMP(unit) != NULL) {
        SHR_FREE(GRP_FAST_TEMP(unit)->grp);
    }
    SHR_FREE(GRP_FAST_TEMP(unit));
    SHR_FREE(GRP_FAST_DIFF(unit));

    SHR_FUNC_EXIT();
}
