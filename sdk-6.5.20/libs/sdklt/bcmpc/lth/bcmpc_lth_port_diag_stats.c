/*! \file bcmpc_lth_port_diag_stats.c
 *
 * Logical Table Custom Handlers for PC_PORT_DIAG_STATS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_lth_port_diag_stats.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PORT_DIAG_STATSt

/*! Data structure to save the data of PC_PORT_DIAG_STATSt entry. */
typedef struct pc_tx_diag_stats_data_s {
    bcmpc_lport_t lport;
    bcmpc_pmgr_port_diag_stats_t diag_stats;
} pc_port_diag_stats_data_t;


/*******************************************************************************
 * Private functions
 */

static void
pc_port_diag_stats_data_t_init(pc_port_diag_stats_data_t *diag_stats_data)
{
    diag_stats_data->lport = BCMPC_INVALID_LPORT;
    bcmpc_pmgr_port_diag_stats_t_init(&(diag_stats_data->diag_stats));
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_port_diag_stats_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    size_t idx;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < in->count; idx++) {
        fid = in->field[idx]->id;

        if (fid == PC_PORT_DIAG_STATSt_PORT_IDf) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_lth_field_validate(unit, LTH_SID, in->field[idx]));
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_insert(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0;
    uint64_t fval;
    pc_port_diag_stats_data_t diag_stats_data;
    bcmpc_pmgr_port_diag_stats_t *diag_stats = &diag_stats_data.diag_stats;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    pc_port_diag_stats_data_t_init(&diag_stats_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_DIAG_STATSt_PORT_IDf, &fval));
    diag_stats_data.lport = fval;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_diag_stats_get(unit, diag_stats_data.lport, diag_stats));

    /* Key fields. */
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_PORT_IDf, 0,
                        diag_stats_data.lport, fcnt);

    /* FEC Stats */
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_FEC_CORRECTED_CODEWORDSf, 0,
                        diag_stats->fec_corrected_cws, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_FEC_UNCORRECTED_CODEWORDSf, 0,
                        diag_stats->fec_uncorrected_cws, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_FEC_SYMBOL_ERRORSf, 0,
                        diag_stats->fec_symbol_errors, fcnt);

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_delete(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    int rv, fcnt;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[2], fld[2];
    uint64_t lport;
    uint64_t min, max;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_DIAG_STATSt_PORT_IDf,
                                         &min, &max));
    lport = min;
    while (lport <= max) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_DIAG_STATSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }
        rv = bcmpc_lth_port_diag_stats_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_diag_stats_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    int rv, fcnt;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[2], fld[2];
    uint64_t lport;
    uint64_t min, max;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_DIAG_STATSt_PORT_IDf,
                             &lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_DIAG_STATSt_PORT_IDf,
                                         &min, &max));
    lport++;
    while (lport <= max) {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_DIAG_STATSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_lth_port_diag_stats_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

