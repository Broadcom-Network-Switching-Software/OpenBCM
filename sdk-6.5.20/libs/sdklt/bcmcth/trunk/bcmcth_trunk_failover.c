/*! \file bcmcth_trunk_failover.c
 *
 * Purpose:     Handler implementation for TRUNK_FAILOVER.
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
#include <bcmcth/bcmcth_trunk_failover.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_drv.h>

#include <bcmpc/bcmpc_lport.h>

/*******************************************************************************
 * Defines
 */
/* BSL Module. */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Private variables
 */
/*! Failover bookkeeping HA structure. */
static bcmcth_trunk_failover_bk_t *failover_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! Failover bookkeeping temporary structure. */
static bcmcth_trunk_failover_bk_t *failover_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK failover SW bitmap changed in one commit. */
SHR_BITDCL *bcmcth_trunk_failover_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk group state. */
#define FAILOVER_HA(unit)             failover_bk[unit]

/*! Pointer to trunk group temporary state. */
#define FAILOVER_TEMP(unit)           failover_bk_temp[unit]

/*! Pointer to trunk group temporary state. */
#define FAILOVER_DIFF(unit)           bcmcth_trunk_failover_bitmap_diff[unit]

#define BCMCTH_TRUNK_FAILOVER_INFO(_u_)  \
        (FAILOVER_TEMP(unit)->entry)
#define BCMCTH_TRUNK_FAILOVER_PTR(_u_, _idx_)  \
        (&(FAILOVER_TEMP(unit)->entry[_idx_]))
#define BCMCTH_TRUNK_FAILOVER_EXIST(_u_, _idx_)  \
        (BCMCTH_TRUNK_FAILOVER_PTR(_u_, _idx_)->set)
#define BCMCTH_TRUNK_FAILOVER_RTAG(_u_, _idx_)  \
        (BCMCTH_TRUNK_FAILOVER_PTR(_u_, _idx_)->rtag)
#define BCMCTH_TRUNK_FAILOVER_CNT(_u_, _idx_)  \
        (BCMCTH_TRUNK_FAILOVER_PTR(_u_, _idx_)->failover_cnt)


/*******************************************************************************
 * Private functions
 */
static int
bcmcth_trunk_failover_field_parse(int unit, const bcmltd_fields_t *in,
                                  bcmcth_trunk_failover_param_t **param)
{
    uint32_t idx = 0, id;
    uint32_t arr_idx;
    uint32_t fval;
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    if ((!in) || (!param)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmcth_trunk_failover_field_parse:CNT=%d.\n"),
                    (uint32_t)(in->count)));

    (*param) = sal_alloc(sizeof(bcmcth_trunk_failover_param_t),
                         "bcmcthTrunkGrpParam");
    SHR_NULL_CHECK((*param), SHR_E_MEMORY);
    sal_memset((*param), 0, sizeof(bcmcth_trunk_failover_param_t));

    for (idx = 0; idx < in->count; idx++) {
        id      = in->field[idx]->id;
        arr_idx = in->field[idx]->idx;
        fval    = in->field[idx]->data;
        if (in->field[idx]->flags & SHR_FMM_FIELD_DEL) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmlrd_field_default_get(unit, TRUNK_FAILOVERt, id,
                                         1, &def_val, &num));
            fval = def_val;
        }
        switch (id) {
            case TRUNK_FAILOVERt_PORT_IDf:
                if (fval > (failover_bk[unit]->max_ports)) {
                    SHR_ERR_EXIT(SHR_E_PORT);
                }
                (*param)->id = fval;
                (*param)->id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_PORT_IDf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_RTAGf:
                (*param)->rtag = fval;
                (*param)->rtag_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_RTAGf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_CNTf:
                if (fval > BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_CNTf = %d idx invalid.\n"),
                                    fval));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_cnt = fval;
                (*param)->failover_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_FAILOVER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_MODIDf:
                if (arr_idx >= BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_MODIDf[%d] idx invalid.\n"),
                                    fval));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_modid[arr_idx] = fval;
                (*param)->failover_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_FAILOVERt_FAILOVER_MODIDf[%d] = %d.\n"),
                     arr_idx, fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_MODPORTf:
                if (arr_idx >= BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_MODIDf[%d] idx invalid.\n"),
                                    fval));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_modport[arr_idx] = fval;
                (*param)->failover_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_FAILOVERt_FAILOVER_MODPORTf[%d] = %d.\n"),
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

static int
bcmcth_trunk_failover_sw_update(int unit, bcmcth_trunk_failover_param_t *param,
                                bcmcth_trunk_failover_t *entry)
{
    int px = 0;

    SHR_FUNC_ENTER(unit);

    if (param == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry->set = TRUE;

    if (param->rtag_valid) {
        entry->rtag = param->rtag;
    }
    if (param->failover_cnt_valid) {
        entry->failover_cnt = param->failover_cnt;
    }

    for (px = 0; px < BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS; px++) {
        if (param->failover_modid_valid[px]) {
            entry->failover_modid[px] = param->failover_modid[px];
        }
        if (param->failover_modport_valid[px]) {
            entry->failover_modport[px] = param->failover_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmcth_trunk_failover_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_failover_param_t *param = NULL;
    int port_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!failover_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
       bcmcth_trunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
       SHR_ERR_EXIT(SHR_E_PARAM);
    }
    port_id = param->id;

    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            if (BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            break;
       case BCMLT_OPCODE_LOOKUP:
            if (!BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
       case BCMLT_OPCODE_DELETE:
            if (!BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
       case BCMLT_OPCODE_UPDATE:
            if (!BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
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
bcmcth_trunk_failover_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_failover_param_t *param = NULL;
    int port_id;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_failover_field_parse(unit, in, &param));

    if (!param->failover_cnt_valid) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_field_default_get(unit, TRUNK_FAILOVERt,
                                     TRUNK_FAILOVERt_FAILOVER_CNTf,
                                     1, &def_val, &num_actual));
        param->failover_cnt = def_val;
        param->failover_cnt_valid = TRUE;
    }

    port_id = param->id;
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_failover_add(unit, op_arg,
                                       TRUNK_FAILOVERt, param));


    SHR_IF_ERR_EXIT
        (bcmcth_trunk_failover_sw_update
        (unit, param, BCMCTH_TRUNK_FAILOVER_PTR(unit, port_id)));
    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_failover_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int port_id;
    bcmcth_trunk_failover_param_t *param = NULL;
    int idx = 0, px;
    bcmcth_trunk_failover_t *entry = NULL;
    uint32_t num_actual = 0;
    uint64_t def_val = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    port_id = param->id;
    entry = BCMCTH_TRUNK_FAILOVER_PTR(unit, port_id);

    idx = 0;
    out->field[idx]->id   = TRUNK_FAILOVERt_RTAGf;
    out->field[idx]->data = entry->rtag;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_CNTf;
    out->field[idx]->data = entry->failover_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    /* Invalid map fields should not be returned. */
    rv = bcmlrd_field_default_get(unit, TRUNK_FAILOVERt,
                                  TRUNK_FAILOVERt_FAILOVER_MODIDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* All array member should be returned. */
        for (px = 0; px < BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_MODIDf;
            out->field[idx]->data = entry->failover_modid[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }

    for (px = 0; px < BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_MODPORTf;
        out->field[idx]->data = entry->failover_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->count = idx;

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_failover_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_trunk_failover_param_t *param = NULL;
    int port_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    port_id = param->id;

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_failover_del(unit, op_arg,
                                       TRUNK_FAILOVERt, param));

    sal_memset(BCMCTH_TRUNK_FAILOVER_PTR(unit, port_id),
               0, sizeof(bcmcth_trunk_failover_t));

    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int port_id;
    bcmcth_trunk_failover_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_EXIT(bcmcth_trunk_failover_field_parse(unit, in, &param));
    port_id = param->id;

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_failover_update(unit, op_arg,
                                          TRUNK_FAILOVERt,
                                          param,
                                          BCMCTH_TRUNK_FAILOVER_PTR(unit, port_id)));

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_failover_sw_update(unit, param,
                                         BCMCTH_TRUNK_FAILOVER_PTR(unit, port_id)));

    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_first(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t port_id, min, max;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(in);

    if (!failover_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNK_FAILOVERt,
                                            TRUNK_FAILOVERt_PORT_IDf,
                                            &min, &max));

    for (port_id = min; port_id <= max; port_id++) {
        if (BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FAILOVERt_PORT_IDf;
            flds.field[0]->data = port_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_failover_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FAILOVERt_PORT_IDf;
            out->field[out->count]->data = port_id;
            out->count++;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_next(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t port_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!failover_bk[unit]) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_value_range_get(unit, TRUNK_FAILOVERt,
                                            TRUNK_FAILOVERt_PORT_IDf,
                                            &min, &max));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_field_get(unit, in, TRUNK_FAILOVERt_PORT_IDf, &port_id));

    port_id++;

    /* Stop traverse when exceeding the max trunk_id group entry. */
    for ( ; port_id <= max; port_id++) {
        /* Exit the current iteration once we get a valid entry. */
        if (BCMCTH_TRUNK_FAILOVER_EXIST(unit, port_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FAILOVERt_PORT_IDf;
            flds.field[0]->data = port_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_failover_lookup(unit, op_arg, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FAILOVERt_PORT_IDf;
            out->field[out->count]->data = port_id;
            out->count++;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_commit(int unit,
                             uint32_t trans_id,
                             const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!FAILOVER_HA(unit)) {
        SHR_EXIT();
    }

    size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
    for (idx = 0; idx < FAILOVER_HA(unit)->max_ports; idx++) {
        if (SHR_BITGET(FAILOVER_DIFF(unit), idx)) {
            FAILOVER_HA(unit)->entry[idx] = FAILOVER_TEMP(unit)->entry[idx];
        }
    }

    sal_memset(FAILOVER_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_abort(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!FAILOVER_HA(unit)) {
        SHR_EXIT();
    }

    size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
    for (idx = 0; idx < FAILOVER_HA(unit)->max_ports; idx++) {
        if (SHR_BITGET(FAILOVER_DIFF(unit), idx)) {
            FAILOVER_TEMP(unit)->entry[idx] = FAILOVER_HA(unit)->entry[idx];
        }
    }

    sal_memset(FAILOVER_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!FAILOVER_HA(unit)) {
        alloc_size = sizeof(bcmcth_trunk_failover_bk_t);
        SHR_ALLOC(FAILOVER_HA(unit), alloc_size, "bcmcthTrunkFailoverBk");
        SHR_NULL_CHECK(FAILOVER_HA(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_HA(unit), 0, alloc_size);

        SHR_ALLOC(FAILOVER_TEMP(unit), alloc_size,
                  "bcmcthTrunkFailoverBkTemp");
        SHR_NULL_CHECK(FAILOVER_TEMP(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_TEMP(unit), 0, alloc_size);

        SHR_IF_ERR_EXIT
            (bcmcth_trunk_drv_failover_init(unit, FAILOVER_HA(unit)));
        FAILOVER_TEMP(unit)->max_ports = FAILOVER_HA(unit)->max_ports;

        /* HA structure. */
        ha_req_size = (FAILOVER_HA(unit)->max_ports) *
                       sizeof(bcmcth_trunk_failover_t);
        ha_alloc_size = ha_req_size;
        FAILOVER_HA(unit)->entry = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMCTH_TRUNK_FAILOVER_SUB_COMP_ID,
                                             "bcmcthTrunkFailover",
                                             &ha_alloc_size);
        SHR_NULL_CHECK(FAILOVER_HA(unit)->entry, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(FAILOVER_HA(unit)->entry, 0, ha_alloc_size);
        }

        /* Temporary structure. */
        alloc_size = (FAILOVER_TEMP(unit)->max_ports) *
                     sizeof(bcmcth_trunk_failover_t);
        SHR_ALLOC(FAILOVER_TEMP(unit)->entry, alloc_size,
                  "bcmcthTrunkFailoverInfo");
        SHR_NULL_CHECK(FAILOVER_TEMP(unit)->entry, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(FAILOVER_TEMP(unit)->entry, 0, alloc_size);
        } else {
            sal_memcpy(FAILOVER_TEMP(unit)->entry,
                       FAILOVER_HA(unit)->entry,
                       alloc_size);
        }

        /* TRUNK bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
        SHR_ALLOC(FAILOVER_DIFF(unit), alloc_size,
                  "bcmcthTrunkFailoverBmpDiff");
        SHR_NULL_CHECK(FAILOVER_DIFF(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_DIFF(unit), 0, alloc_size);

    }

exit:
    if (SHR_FUNC_ERR()) {
        if (FAILOVER_HA(unit) != NULL) {
            if (FAILOVER_HA(unit)->entry != NULL) {
                shr_ha_mem_free(unit, FAILOVER_HA(unit)->entry);
                FAILOVER_HA(unit)->entry = NULL;
            }
            SHR_FREE(FAILOVER_HA(unit));
        }

        if (FAILOVER_TEMP(unit) != NULL) {
            SHR_FREE(FAILOVER_TEMP(unit)->entry);
        }
        SHR_FREE(FAILOVER_TEMP(unit));
        SHR_FREE(FAILOVER_DIFF(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_failover_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(FAILOVER_HA(unit));
    if (FAILOVER_TEMP(unit) != NULL) {
        SHR_FREE(FAILOVER_TEMP(unit)->entry);
    }
    SHR_FREE(FAILOVER_TEMP(unit));
    SHR_FREE(FAILOVER_DIFF(unit));

    SHR_FUNC_EXIT();
}

