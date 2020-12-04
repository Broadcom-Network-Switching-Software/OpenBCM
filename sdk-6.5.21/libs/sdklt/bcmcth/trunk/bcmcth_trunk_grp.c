/*! \file bcmcth_trunk_grp.c
 *
 * Purpose:     handler implementation for TRUNK LT.
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
#include <bcmcth/bcmcth_trunk_grp.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_drv.h>


/*******************************************************************************
 * Private variables
 */
/*! TRUNK bookkeeping HA structure. */
static bcmcth_trunk_grp_bk_t *lth_grp_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK bookkeeping temporary structure. */
static bcmcth_trunk_grp_bk_t * lth_grp_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK group SW bitmap changed in one commit. */
SHR_BITDCL *bcmcth_trunk_grp_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk group state. */
#define GRP_HA(unit)                 lth_grp_bk[unit]

/*! Pointer to trunk group temporary state. */
#define GRP_TEMP(unit)               lth_grp_bk_temp[unit]

/*! Pointer to trunk group temporary state. */
#define GRP_DIFF(unit)               bcmcth_trunk_grp_bitmap_diff[unit]



#define BCMCTH_TRUNK_GRP_INFO(_u_) \
    (GRP_TEMP(unit)->grp)
#define BCMCTH_TRUNK_GRP(_u_, _idx_) \
    (&(GRP_TEMP(unit)->grp[_idx_]))
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


/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Public functions
 */
int
bcmcth_trunk_field_parse(int unit, const bcmltd_fields_t *in,
                         bcmcth_trunk_group_param_t **param)
{
    uint32_t idx = 0, id;
    uint32_t arr_idx, fval;
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    if ((in == NULL) || (param == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    (*param) = sal_alloc(sizeof(bcmcth_trunk_group_param_t),
                         "bcmcthTrunkGrpParam");
    SHR_NULL_CHECK((*param), SHR_E_MEMORY);
    sal_memset((*param), 0, sizeof(bcmcth_trunk_group_param_t));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmcth_trunk_field_parse:CNT=%d.\n"),
                    (uint32_t)(in->count)));

    for (idx = 0; idx < in->count; idx++) {
        id      = in->field[idx]->id;
        arr_idx = in->field[idx]->idx;
        fval    = in->field[idx]->data;
        if (in->field[idx]->flags & SHR_FMM_FIELD_DEL) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmlrd_field_default_get(unit, TRUNKt, id,
                                         1, &def_val, &num));
            fval = def_val;
        }
        switch (id) {
            case TRUNKt_TRUNK_IDf:
                (*param)->id = fval;
                (*param)->id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_TRUNK_IDf = %d.\n"), fval));
                break;
            case TRUNKt_LB_MODEf:
                (*param)->lb_mode = fval;
                (*param)->lb_mode_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_LB_MODEf = %d.\n"), fval));
                break;
            case TRUNKt_UC_RTAGf:
                (*param)->uc_rtag = fval;
                (*param)->uc_rtag_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_RTAGf = %d.\n"), fval));
                break;
            case TRUNKt_UC_MAX_MEMBERSf:
                if ((fval < BCMCTH_TRUNK_MIN_MEMBERS) ||
                    (fval > BCMCTH_TRUNK_MAX_MEMBERS)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_max_members = fval;
                (*param)->uc_max_members_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MAX_MEMBERSf = %d.\n"), fval));
                break;
            case TRUNKt_UC_MEMBER_CNTf:
                (*param)->uc_cnt = fval;
                (*param)->uc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MEMBER_CNTf = %d.\n"), fval));
                break;
            case TRUNKt_UC_MEMBER_MODIDf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODID[%d] index invalid.\n"),
                                    fval));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modid[arr_idx] = fval;
                (*param)->uc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MODID[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNKt_UC_MEMBER_MODPORTf:
                if (arr_idx >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modport[arr_idx] = fval;
                (*param)->uc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNKt_MON_AGM_POOLf:
                (*param)->uc_agm_pool = fval;
                (*param)->uc_agm_pool_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_MON_AGM_POOLf = %d.\n"), fval));
                break;
            case TRUNKt_MON_AGM_IDf:
                (*param)->uc_agm_id = fval;
                (*param)->uc_agm_id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_MON_AGM_IDf = %d.\n"), fval));
                break;
             case TRUNKt_NONUC_MEMBER_CNTf:
                if (fval > BCMCTH_TRUNK_MAX_MEMBERS) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_cnt = fval;
                (*param)->nonuc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_NONUC_MEMBER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNKt_NONUC_MEMBER_MODIDf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NONUC_MEMBER_MODID[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modid[arr_idx] = fval;
                (*param)->nonuc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_NONUC_MODIDf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNKt_NONUC_MEMBER_MODPORTf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "TRUNKt_NONUC_MODPORTf[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modport[arr_idx] = fval;
                (*param)->nonuc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_NONUC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
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

int
bcmcth_trunk_grp_to_api(int unit,
                        bcmcth_trunk_group_t *grp,
                        bcmltd_fields_t *out)
{
    uint32_t idx = 0, px;

    SHR_FUNC_ENTER(unit);

    if ((!grp) || (!out)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->field[idx]->id   = TRUNKt_LB_MODEf;
    out->field[idx]->data = grp->lb_mode;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNKt_UC_RTAGf;
    out->field[idx]->data = grp->uc_rtag;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNKt_UC_MAX_MEMBERSf;
    out->field[idx]->data = grp->uc_max_members;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNKt_UC_MEMBER_CNTf;
    out->field[idx]->data = grp->uc_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    /* All array member should be returned. */
    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNKt_UC_MEMBER_MODIDf;
        out->field[idx]->data = grp->uc_modid[px];
        out->field[idx]->idx  = px;
        idx++;
    }
    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNKt_UC_MEMBER_MODPORTf;
        out->field[idx]->data = grp->uc_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->field[idx]->id   = TRUNKt_MON_AGM_POOLf;
    out->field[idx]->data = grp->uc_agm_pool;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNKt_MON_AGM_IDf;
    out->field[idx]->data = grp->uc_agm_id;
    out->field[idx]->idx  = 0;
    idx++;

    /* nonuc member fields. */
    out->field[idx]->id   = TRUNKt_NONUC_MEMBER_CNTf;
    out->field[idx]->data = grp->nonuc_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    /* All array member should be returned. */
    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNKt_NONUC_MEMBER_MODIDf;
        out->field[idx]->data = grp->nonuc_modid[px];
        out->field[idx]->idx  = px;
        idx++;
    }
    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNKt_NONUC_MEMBER_MODPORTf;
        out->field[idx]->data = grp->nonuc_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->count = idx;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_sw_update(int unit, bcmcth_trunk_group_param_t *param,
                       bcmcth_trunk_group_t *grp)
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
    if (param->uc_max_members_valid) {
        grp->uc_max_members = param->uc_max_members;
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

    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        if (param->uc_modid_valid[px]) {
            grp->uc_modid[px] = param->uc_modid[px];
        }
        if (param->uc_modport_valid[px]) {
            grp->uc_modport[px] = param->uc_modport[px];
        }
    }
    for (px = 0; px < BCMCTH_TRUNK_MAX_MEMBERS; px++) {
        if (param->nonuc_modid_valid[px]) {
            grp->nonuc_modid[px] = param->nonuc_modid[px];
        }
        if (param->nonuc_modport_valid[px]) {
            grp->nonuc_modport[px] = param->nonuc_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_grp_validate(int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_group_param_t *param = NULL;
    int trunk_id, old_uc_cnt;
    int uc_cnt;
    int max_members;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!GRP_HA(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
       SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    if ((trunk_id < 0) || (trunk_id >= BCMCTH_TRUNK_MAX_TRUNK)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRUNK_ID=%d invalid.\n"), trunk_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            if (GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d exist.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            if (!param->uc_max_members_valid) {
                param->uc_max_members_valid = TRUE;
                param->uc_max_members = BCMCTH_TRUNK_MAX_MEMBERS;
            }
            if (param->uc_cnt > param->uc_max_members) {
                SHR_ERR_EXIT(SHR_E_PARAM);
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

            if (param->uc_max_members_valid) {
                max_members = param->uc_max_members;
            } else {
                max_members = GRP_MAX_MEMBERS(unit, trunk_id);
            }
            if (uc_cnt > max_members) {
                SHR_ERR_EXIT(SHR_E_PARAM);
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
bcmcth_trunk_grp_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_group_param_t *param = NULL;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_field_parse(unit, in, &param));
    trunk_id = param->id;

    if (!param->uc_max_members_valid) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_field_default_get(unit, TRUNKt,
                                     TRUNKt_UC_MAX_MEMBERSf,
                                     1, &def_val, &num_actual));
        param->uc_max_members = def_val;
        param->uc_max_members_valid = TRUE;
    }
    SHR_IF_ERR_EXIT(
        bcmcth_trunk_drv_grp_add(unit, op_arg, TRUNKt,
                                 GRP_TEMP(unit), param));

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_sw_update(unit, param, BCMCTH_TRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_grp_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_grp_to_api(unit, BCMCTH_TRUNK_GRP(unit, trunk_id), out));

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();

}

int
bcmcth_trunk_grp_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_group_param_t *param = NULL;
    int trunk_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(bcmcth_trunk_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_drv_grp_del(unit, op_arg,
                                 TRUNKt, GRP_TEMP(unit), param));

    sal_memset(BCMCTH_TRUNK_GRP(unit, trunk_id), 0,
               sizeof(bcmcth_trunk_group_t));

    SHR_BITSET(GRP_DIFF(unit), trunk_id);

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();
}


int
bcmcth_trunk_grp_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmcth_trunk_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_EXIT(bcmcth_trunk_field_parse(unit, in, &param));
    trunk_id = param->id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_grp_update(unit, op_arg, TRUNKt,
                                     GRP_TEMP(unit), param));

    SHR_IF_ERR_EXIT
       (bcmcth_trunk_sw_update(unit, param, BCMCTH_TRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_grp_first(int unit,
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

    if (!GRP_HA(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNKt, TRUNKt_TRUNK_IDf,
                                            &min, &max));

    for (trunk_id = min; trunk_id <= max; trunk_id++) {
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNKt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_grp_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNKt_TRUNK_IDf;
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
bcmcth_trunk_grp_next(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t trunk_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!GRP_HA(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNKt, TRUNKt_TRUNK_IDf,
                                            &min, &max));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_get(unit, in, TRUNKt_TRUNK_IDf, &trunk_id));

    trunk_id++;

    /* Stop traverse when exceeding the max trunk_id group entry. */
    for ( ; trunk_id <= max; trunk_id++) {
        /* Exit the current iteration once we get a valid entry. */
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNKt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_grp_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNKt_TRUNK_IDf;
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
bcmcth_trunk_grp_commit(int unit,
                        uint32_t trans_id,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_HA(unit)) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMCTH_TRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_DIFF(unit), idx)) {
            GRP_HA(unit)->grp[idx] = GRP_TEMP(unit)->grp[idx];
        }
    }
    sal_memcpy(GRP_HA(unit)->mbmp, GRP_TEMP(unit)->mbmp,
               SHR_BITALLOCSIZE(GRP_TEMP(unit)->member_size));

    sal_memset(GRP_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_grp_abort(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_HA(unit)) {
        SHR_EXIT();

    }

    for (idx = 0; idx < BCMCTH_TRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_DIFF(unit), idx)) {
            GRP_TEMP(unit)->grp[idx] = GRP_HA(unit)->grp[idx];
        }
    }

    sal_memcpy(GRP_TEMP(unit)->mbmp, GRP_HA(unit)->mbmp,
               SHR_BITALLOCSIZE(GRP_TEMP(unit)->member_size));
    sal_memset(GRP_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_grp_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;
    int trunk_member_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!GRP_HA(unit)) {
        alloc_size = sizeof(bcmcth_trunk_grp_bk_t);
        SHR_ALLOC(GRP_HA(unit), alloc_size, "bcmcthTrunkGrpBk");
        SHR_NULL_CHECK(GRP_HA(unit), SHR_E_MEMORY);
        sal_memset(GRP_HA(unit), 0, alloc_size);

        SHR_ALLOC(GRP_TEMP(unit), alloc_size, "bcmcthTrunkGrpBkTemp");
        SHR_NULL_CHECK(GRP_TEMP(unit), SHR_E_MEMORY);
        sal_memset(GRP_TEMP(unit), 0, alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_drv_grp_init(unit, GRP_TEMP(unit)));
        GRP_HA(unit)->member_size = GRP_TEMP(unit)->member_size;

        /* HA structure. */
        ha_req_size = BCMCTH_TRUNK_MAX_TRUNK * sizeof(bcmcth_trunk_group_t);
        ha_alloc_size = ha_req_size;
        GRP_HA(unit)->grp = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMCTH_TRUNK_GRP_SUB_COMP_ID,
                                             "bcmcthTrunkGroup",
                                             &ha_alloc_size);
        SHR_NULL_CHECK(GRP_HA(unit)->grp, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(GRP_HA(unit)->grp, 0, ha_alloc_size);
        }

        trunk_member_size = GRP_HA(unit)->member_size;
        ha_req_size = SHR_BITALLOCSIZE(trunk_member_size);
        ha_alloc_size = ha_req_size;
        GRP_HA(unit)->mbmp = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMCTH_TRUNK_MEMBER_BMP_SUB_COMP_ID,
                                             "bcmcthTrunkMemberBmp",
                                             &ha_alloc_size);
        SHR_NULL_CHECK(GRP_HA(unit)->mbmp, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(GRP_HA(unit)->mbmp, 0, ha_alloc_size);
            /*
            * Reserve entry 0 of TRUNK_MEMBERm, as invalid Higig trunk groups have
            * their TRUNK_GROUP table entry's BASE_PTR and TG_SIZE fields set to 0.
            * The default value of entry 0 should be null, so it is not set here.
            */
            SHR_BITSET(GRP_HA(unit)->mbmp, 0);
        }


        /* Temporary structure. */
        alloc_size = BCMCTH_TRUNK_MAX_TRUNK * sizeof(bcmcth_trunk_group_t);
        SHR_ALLOC(GRP_TEMP(unit)->grp, alloc_size, "bcmcthTrunkGrpInfoTemp");
        SHR_NULL_CHECK(GRP_TEMP(unit)->grp, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_TEMP(unit)->grp, 0, alloc_size);
        } else {
            sal_memcpy(GRP_TEMP(unit)->grp, GRP_HA(unit)->grp, alloc_size);
        }

        alloc_size = SHR_BITALLOCSIZE(trunk_member_size);
        SHR_ALLOC(GRP_TEMP(unit)->mbmp, alloc_size, "bcmcthTrunkMemBmpInfo");
        SHR_NULL_CHECK(GRP_TEMP(unit)->mbmp, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_TEMP(unit)->mbmp, 0, alloc_size);
            SHR_BITSET(GRP_TEMP(unit)->mbmp, 0);
        } else {
            sal_memcpy(GRP_TEMP(unit)->mbmp, GRP_HA(unit)->mbmp, alloc_size);
        }


        /* TRUNK bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_MAX_TRUNK);
        SHR_ALLOC(GRP_DIFF(unit), alloc_size, "bcmcthTrunkGrpBmpDiff");
        SHR_NULL_CHECK(GRP_DIFF(unit), SHR_E_MEMORY);
        sal_memset(GRP_DIFF(unit), 0, alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (GRP_HA(unit) != NULL) {
            if (GRP_HA(unit)->grp != NULL) {
                shr_ha_mem_free(unit, GRP_HA(unit)->grp);
                GRP_HA(unit)->grp = NULL;
            }
            if (GRP_HA(unit)->mbmp != NULL) {
                shr_ha_mem_free(unit, GRP_HA(unit)->mbmp);
                GRP_HA(unit)->mbmp = NULL;
            }
            SHR_FREE(GRP_HA(unit));
        }
        if (GRP_TEMP(unit) != NULL) {
            SHR_FREE(GRP_TEMP(unit)->grp);
            SHR_FREE(GRP_TEMP(unit)->mbmp);
        }
        SHR_FREE(GRP_TEMP(unit));
        SHR_FREE(GRP_DIFF(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_grp_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(GRP_HA(unit));
    if (GRP_TEMP(unit) != NULL) {
        SHR_FREE(GRP_TEMP(unit)->grp);
        SHR_FREE(GRP_TEMP(unit)->mbmp);
    }
    SHR_FREE(GRP_TEMP(unit));
    SHR_FREE(GRP_DIFF(unit));

    SHR_FUNC_EXIT();
}

