/*! \file bcmpc_lth_pmd_firmware_status.c
 *
 * Logical Table Custom Handlers for PC_PMD_FIRMWARE_STATUS.
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
#include <bcmpc/bcmpc_lth_pmd_firmware_status.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PMD_FIRMWARE_STATUSt

/*! Data structure to save the data of PC_PMD_FIRMWARE_STATUSt entry. */
typedef struct pc_pmd_firmware_st_data_s {
    bcmpc_lport_t lport;
    bcmpc_pmgr_pmd_firmware_status_t pmdf_st;
} pc_pmd_firmware_st_data_t;


/*******************************************************************************
 * Private functions
 */

static void
pc_pmd_firmware_st_data_t_init(pc_pmd_firmware_st_data_t *pmdf_st_data)
{
    pmdf_st_data->lport = BCMPC_INVALID_LPORT;
    bcmpc_pmgr_pmd_firmware_status_t_init(&(pmdf_st_data->pmdf_st));
}

/*!
 * \brief LTA input fields parsing.
 *
 * Parse LTA input fields and save the data to pc_pm_lane_st_data_t.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [out] lane_st_data PM lane status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c lane_st_data.
 */
static int
lt_fields_parse(int unit, const bcmltd_fields_t *in,
                pc_pmd_firmware_st_data_t *pmdf_st_data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        fid = in->field[i]->id;
        fval = in->field[i]->data;

        switch (fid) {
            case PC_PMD_FIRMWARE_STATUSt_PORT_IDf:
                pmdf_st_data->lport = (bcmpc_lport_t) fval;
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
 * \param [in] lane_st_data Lane status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more keys are not given.
 * \retval SHR_E_PARAM One or more key values are invalid.
 */
static int
lt_key_fields_validate(int unit, pc_pmd_firmware_st_data_t *pmdf_st_data)
{
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    if (pmdf_st_data->lport == BCMPC_INVALID_LPORT) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "PORT_ID is not specified.\n")));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&(pmdf_st_data->lport)),
                                   P(&pcfg)));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_pmd_firmware_status_validate(int unit,
                                       bcmlt_opcode_t opcode,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_generic_arg_t *arg)
{
    pc_pmd_firmware_st_data_t pmdf_st_data;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    pc_pmd_firmware_st_data_t_init(&pmdf_st_data);
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &pmdf_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_key_fields_validate(unit, &pmdf_st_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pmd_firmware_status_insert(int unit,
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
bcmpc_lth_pmd_firmware_status_lookup(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0;
    pc_pmd_firmware_st_data_t pmdf_st_data;
    bcmpc_pmgr_pmd_firmware_status_t *pmdf_st = &pmdf_st_data.pmdf_st;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    pc_pmd_firmware_st_data_t_init(&pmdf_st_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &pmdf_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_firmware_status_get(unit, pmdf_st_data.lport,
                                            pmdf_st));

    /* Key fields. */
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_PORT_IDf, 0,
                        pmdf_st_data.lport, fcnt);

    /* Tx lane status. */
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_DFEf, 0,
                        pmdf_st->dfe, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_LP_DFEf, 0,
                        pmdf_st->lp_dfe, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_MEDIUM_TYPEf, 0,
                        pmdf_st->medium, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_CL72_RESTART_TIMEOUT_ENf,
                         0, pmdf_st->cl72_res_to, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_LP_TX_PRECODER_ONf, 0,
                        pmdf_st->lp_tx_precoder_on, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_UNRELIABLE_LOSf, 0,
                        pmdf_st->unreliable_los, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_SCRAMBLING_ENABLEf, 0,
                        pmdf_st->scrambling_en, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_NORMAL_REACH_PAM4f, 0,
                        pmdf_st->norm_reach_pam4_bmask, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PMD_FIRMWARE_STATUSt_EXTENDED_REACH_PAM4f, 0,
                        pmdf_st->ext_reach_pam4_bmask, fcnt);

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pmd_firmware_status_delete(int unit,
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
bcmpc_lth_pmd_firmware_status_update(int unit,
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
bcmpc_lth_pmd_firmware_status_first(int unit,
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
                                         PC_PMD_FIRMWARE_STATUSt_PORT_IDf,
                                         &min, &max));
    lport = min;
    while (lport <= max) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PMD_FIRMWARE_STATUSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }
        rv = bcmpc_lth_pmd_firmware_status_lookup(unit, op_arg,
                                                  &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pmd_firmware_status_next(int unit,
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
        (bcmpc_lth_field_get(unit, in, PC_PMD_FIRMWARE_STATUSt_PORT_IDf,
                             &lport));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PMD_FIRMWARE_STATUSt_PORT_IDf,
                                         &min, &max));
    lport++;
    while (lport <= max) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PMD_FIRMWARE_STATUSt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.count = fcnt;

        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            lport++;
            continue;
        }

        rv = bcmpc_lth_pmd_firmware_status_lookup(unit, op_arg,
                                                  &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
