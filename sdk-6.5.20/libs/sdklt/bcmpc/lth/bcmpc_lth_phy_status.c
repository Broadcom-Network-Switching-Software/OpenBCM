/*! \file bcmpc_lth_phy_status.c
 *
 * Logical Table Custom Handlers for PC_PHY_STATUS.
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
#include <bcmpc/bcmpc_lth_phy_status.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PHY_STATUSt

/*! Data structure to save the data of PC_PHY_STATUSt entry. */
typedef struct pc_phy_st_data_s {
    bcmpc_port_lane_t port_lane;
    bcmpc_pmgr_phy_status_t phy_st;
} pc_phy_st_data_t;


/*******************************************************************************
 * Private functions
 */

static void
pc_phy_st_data_t_init(pc_phy_st_data_t *phy_st_data)
{
    phy_st_data->port_lane.lport = BCMPC_INVALID_LPORT;
    phy_st_data->port_lane.lane_index = -1;
    bcmpc_pmgr_phy_status_t_init(&(phy_st_data->phy_st));
}

/*!
 * \brief LTA input fields parsing.
 *
 * Parse LTA input fields and save the data to pc_phy_st_data_t.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [out] lane_st_data PM lane status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to convert \c in to \c pc_phy_st_data.
 */
static int
lt_fields_parse(int unit, const bcmltd_fields_t *in,
                pc_phy_st_data_t *pc_phy_st_data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_port_lane_t *port_lane = &pc_phy_st_data->port_lane;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        fid = in->field[i]->id;
        fval = in->field[i]->data;

        switch (fid) {
            case PC_PHY_STATUSt_PORT_IDf:
                port_lane->lport = fval;
                break;
            case PC_PHY_STATUSt_LANE_INDEXf:
                port_lane->lane_index = fval;
                break;
            default:
                continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_lth_field_validate(unit, LTH_SID, in->field[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the input keys of LT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] phy_st_data Phy status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more keys are not given.
 * \retval SHR_E_PARAM One or more key values are invalid.
 */
static int
lt_key_fields_validate(int unit, pc_phy_st_data_t *phy_st_data)
{
    bcmpc_port_lane_t *port_lane = &phy_st_data->port_lane;
    bcmpc_lport_t lport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    if (port_lane->lport == BCMPC_INVALID_LPORT) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "PORT_ID is not specified.\n")));
    }

    if (port_lane->lane_index == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "LANE_INDEX is not specified.\n")));
    }

    lport = port_lane->lport;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));
    /* The range of lane_index is 0 to (num_lanes-1). */
    if (port_lane->lane_index >= pcfg.num_lanes) {
        return SHR_E_PARAM;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_phy_status_validate(int unit,
                              bcmlt_opcode_t opcode,
                              const bcmltd_fields_t *in,
                              const bcmltd_generic_arg_t *arg)
{
    pc_phy_st_data_t phy_st_data;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    pc_phy_st_data_t_init(&phy_st_data);
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &phy_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_key_fields_validate(unit, &phy_st_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phy_status_insert(int unit,
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
bcmpc_lth_phy_status_lookup(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0, idx;
    pc_phy_st_data_t phy_st_data;
    bcmpc_port_lane_t *port_lane = &phy_st_data.port_lane;
    bcmpc_pmgr_phy_status_t *phy_st = &phy_st_data.phy_st;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    pc_phy_st_data_t_init(&phy_st_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &phy_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_phy_status_get(unit, port_lane, phy_st));

    /* Key fields. */
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_PORT_IDf, 0,
                        port_lane->lport, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_LANE_INDEXf, 0,
                        port_lane->lane_index, fcnt);

    /* phy status. */
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_TX_SQUELCHf, 0,
                        phy_st->tx_squelch, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_SQUELCHf, 0,
                        phy_st->rx_squelch, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_VGAf, 0,
                        phy_st->rx_afe_vga, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_PEAKING_FILTERf, 0,
                        phy_st->rx_afe_pk_flt, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_LOW_FREQ_PEAKING_FILTERf, 0,
                        phy_st->rx_afe_low_fre_pk_flt, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_HIGH_FREQ_PEAKING_FILTERf, 0,
                        phy_st->rx_afe_hi_fre_pk_flt, fcnt);
    for (idx = 0; idx < BCMPC_NUM_RX_DFE_TAPS_MAX; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_DFE_TAP_SIGNf, idx,
                            phy_st->rx_afe_dfe_tap_sign[idx], fcnt);
    }
    for (idx = 0; idx < BCMPC_NUM_RX_DFE_TAPS_MAX; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_AFE_DFE_TAPf, idx,
                        phy_st->rx_afe_dfe_tap[idx], fcnt);
    }
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_RX_SIGNAL_DETECTf, 0,
                        phy_st->rx_signal_detect, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_PAM4_TX_PATTERNf, 0,
                        phy_st->pam4_tx_pattern, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_PAM4_TX_PRECODERf, 0,
                        phy_st->pam4_tx_precoder_enabled, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_PHY_ECC_INTR_ENABLEf, 0,
                        phy_st->phy_ecc_intr_enabled, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_TX_PI_FREQ_OVERRIDE_SIGNf, 0,
                        phy_st->tx_pi_sign, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_TX_PI_FREQ_OVERRIDEf, 0,
                        phy_st->tx_pi, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PHY_STATUSt_FEC_BYPASS_INDICATIONf, 0,
                        phy_st->phy_fec_bypass_ind, fcnt);

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phy_status_delete(int unit,
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
bcmpc_lth_phy_status_update(int unit,
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
bcmpc_lth_phy_status_first(int unit,
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
                                         PC_PHY_STATUSt_PORT_IDf,
                                         &min, &max));
    lport = min;
    while (lport <= max) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PHY_STATUSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PHY_STATUSt_LANE_INDEXf;
        flds.field[fcnt]->data = 0;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }
        rv = bcmpc_lth_phy_status_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phy_status_next(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    int rv, fcnt;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[2], fld[2];
    uint64_t lport, lane_index, count;
    uint64_t min, max;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHY_STATUSt_PORT_IDf,
                             &lport));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHY_STATUSt_LANE_INDEXf,
                             &lane_index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PHY_STATUSt_PORT_IDf,
                                         &min, &max));
    lane_index++;
    while (lport <= max) {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (SHR_SUCCESS(rv)) {
            count = pcfg.num_lanes;
        } else {
            count = 0;
        }
        if (lane_index >= count) {
            lport++;
            lane_index = 0;
        }
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PHY_STATUSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PHY_STATUSt_LANE_INDEXf;
        flds.field[fcnt]->data = lane_index;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_lth_phy_status_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

