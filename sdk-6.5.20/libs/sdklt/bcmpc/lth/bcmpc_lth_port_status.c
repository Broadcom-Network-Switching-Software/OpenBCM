/*! \file bcmpc_lth_port_status.c
 *
 * Logical Table Custom Handlers for PC_PORT_STATUS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_port_status.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_util_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PORT_STATUSt


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_port_status_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    size_t idx;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < in->count; idx++) {
        fid = in->field[idx]->id;

        if (fid == PC_PORT_STATUSt_PORT_IDf) {
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
bcmpc_lth_port_status_insert(int unit,
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
bcmpc_lth_port_status_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    uint64_t fval;
    int fcnt = 0;
    uint32_t failover = 0;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_port_cfg_t pcfg;
    bcmpc_pmgr_port_status_t pst;
    bcmpc_pmgr_link_status_t link_st = 0;
    bcmpc_pmgr_port_rlm_status_t rlm_status;
    bcmpc_mac_control_t mcfg;
    bcmpc_drv_t *pc_drv = NULL;
    bool is_internal = false;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    bcmpc_pmgr_port_status_t_init(&pst);
    rlm_status = BCMPC_PMGR_PORT_RLM_STATUS_DISABLED;
    sal_memset(&mcfg, 0, sizeof(mcfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_STATUSt_PORT_IDf, &fval));
    lport = fval;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));

    pport = bcmpc_lport_to_pport(unit, lport);

    /* Get the status from hardware when the port is valid. */
    if (pport != BCMPC_INVALID_PPORT && pcfg.speed != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_status_get(unit, pport, &pst));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_link_status_get(unit, pport, &link_st));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_failover_loopback_get(unit, pport, &failover));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_rlm_status_get(unit, pport, &rlm_status));
        /* Get MAC hardware configuration and update the DB. */
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_mac_control_get(unit, lport, pport, &mcfg));
    }

    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PORT_IDf, 0, lport, fcnt);
    fval = (pst.mac_enabled == 0) ? 1 : 0;
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_MAC_DISABLEDf, 0, fval, fcnt);

    fval = (pst.phy_enabled == 0) ? 1 : 0;
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PHY_DISABLEDf, 0, fval, fcnt);

    fval = BCMPC_BIT_GET(link_st, BCMPC_PMGR_LINK_EVENT_PMD_RX_LOCK);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PMD_RX_LOCKf, 0, fval, fcnt);

    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_FAILOVER_LOOPBACKf, 0,
                        failover, fcnt);

    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_FEC_MODEf, 0, pst.fec_mode, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LOOPBACKf, 0, pst.lpbk_mode, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_AUTONEGf, 0, pst.autoneg, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_AUTONEG_DONEf, 0,
                        pst.an_done, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LINK_TRAININGf, 0,
                        pst.link_training, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LINK_TRAINING_DONEf, 0,
                        pst.link_training_done, fcnt);
    /* For internal port, retrieve speed and num_lanes from PC_PORTt if available. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));
    if (is_internal == true) {
        pst.speed = pcfg.speed;
        pst.num_lanes = pcfg.num_lanes;
    }
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_SPEEDf, 0, pst.speed, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_NUM_LANESf, 0, pst.num_lanes,
                        fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_ENCAPf, 0, pst.encap_mode,
                        fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LOCAL_FAULTf, 0,
                        pst.local_fault, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_REMOTE_FAULTf, 0,
                        pst.remote_fault, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_RLM_STATUSf, 0, rlm_status, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PAUSE_TXf, 0,
                        (mcfg.pause_tx > 0)? 1: 0, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PAUSE_RXf, 0,
                        (mcfg.pause_rx > 0)? 1: 0, fcnt);


    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_delete(int unit,
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
bcmpc_lth_port_status_update(int unit,
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
bcmpc_lth_port_status_first(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t lport, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_STATUSt_PORT_IDf,
                                         &min, &max));

    lport = min;
    while (lport <= max) {
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PORT_STATUSt_PORT_IDf;
        flds.field[0]->data = lport;
        flds.count = 1;
        rv = bcmpc_lth_port_status_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_next(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t lport, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_PROPt_PC_PM_IDf,
                                         &min, &max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_STATUSt_PORT_IDf, &lport));

    lport++;

    /* Stop traverse when exceeding the max PM ID. */
    while (lport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PORT_STATUSt_PORT_IDf;
        flds.field[0]->data = lport;
        flds.count = 1;
        rv = bcmpc_lth_port_status_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
