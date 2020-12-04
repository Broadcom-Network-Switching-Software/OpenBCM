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
#include <bcmlrd/bcmlrd_table.h>

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
    bool is_unmapped = false;

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

    SHR_IF_ERR_EXIT
        (bcmlrd_field_is_unmapped(unit, PC_PORT_DIAG_STATSt,
                                  PC_PORT_DIAG_STATSt_FEC_CORRECTED_BLOCKSf,
                                  &is_unmapped));
    if (!is_unmapped) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_FEC_CORRECTED_BLOCKSf, 0,
                            diag_stats->fec_corrected_blocks, fcnt);
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_DIAG_STATSt_FEC_UNCORRECTED_BLOCKSf, 0,
                            diag_stats->fec_uncorrected_blocks, fcnt);
    }

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

/*!
 * \brief Logical table entries in-use get.
 *
 * This function returns the entry in-use count for the given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  sid             Table ID.
 * \param [in]  table_arg       Table arguments.
 * \param [out] table_data      Returning table entry in-use count.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_port_diag_stats_table_entry_inuse_get(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_entry_inuse_arg_t *table_arg,
                     bcmltd_table_entry_inuse_t *table_data,
                     const bcmltd_generic_arg_t *arg)
{
    uint64_t min, max;
    bcmpc_lport_t lport;
    bcmpc_port_diag_control_t diag_control;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(table_data, SHR_E_PARAM);

    table_data->entry_inuse = 0;
    /* Get min and max values for the logical port id. */
    SHR_IF_ERR_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_DIAG_STATSt_PORT_IDf,
                                         &min, &max));
    lport = (bcmpc_lport_t) min;

    for (; lport <= (bcmpc_lport_t) max; lport++) {
        int rv = SHR_E_NONE;

        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORT_DIAG_CONTROLt, P(&lport),
                                       P(&diag_control));
        if (SHR_FAILURE(rv)) {
            continue;
        }

        table_data->entry_inuse++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Table max entries set function pointer type.
 *
 * This function sets the max entries for the given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  sid             Table ID.
 * \param [in]  table_arg       Table arguments.
 * \param [in]  table_data      Table max entries to set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmpc_lth_port_diag_stats_table_max_entries_set(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_max_entries_arg_t *table_arg,
                     const bcmltd_table_max_entries_t *table_data,
                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}
