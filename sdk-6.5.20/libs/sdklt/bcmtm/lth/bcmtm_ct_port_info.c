/*! \file bcmtm_ct_port_info.c
 *
 * TM device information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmtm/ct/bcmtm_ct_port_info.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_CTH


/*******************************************************************************
 * Public functions
 */
int
bcmtm_ct_port_info_validate(int unit,
                        bcmlt_opcode_t opcode,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if (!(opcode == BCMLT_OPCODE_LOOKUP ||
          opcode == BCMLT_OPCODE_TRAVERSE)){
        SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    size_t fid = 0;
    bcmtm_drv_t *drv;
    bcmtm_ct_drv_t ct_drv;
    const bcmlrd_field_data_t *field;
    uint64_t fval = 0, flags = 0;
    int lport, pport, idx;
    uint8_t opcode = VALID;
    bcmtm_ct_port_info_t ct_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    sal_memset(&ct_info, 0, sizeof(bcmtm_ct_port_info_t));
    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ct_drv_get, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (drv->ct_drv_get(unit, &ct_drv));

    lport = in->field[TM_CUT_THROUGH_PORT_INFOt_PORT_IDf]->data;

    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        opcode = PORT_INFO_UNAVAIL;
    } else {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
        if (bcmtm_lport_is_fp (unit, lport) && ct_drv.port_info_get) {
            SHR_IF_ERR_EXIT
                (ct_drv.port_info_get(unit, arg->sid, flags, lport, &ct_info));
        }
    }
    out->count = 0;

    while (fid < TM_CUT_THROUGH_PORT_INFOt_FIELD_COUNT) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, arg->sid, fid, &field)) {
            fid++;
            continue;
        }
        switch (fid) {
            case TM_CUT_THROUGH_PORT_INFOt_PORT_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, lport));
                break;
            case TM_CUT_THROUGH_PORT_INFOt_SRC_PORT_MAX_SPEEDf:
                fval = ct_info.src_port_max_speed;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, fval));
                break;
            case TM_CUT_THROUGH_PORT_INFOt_CUT_THROUGH_CLASSf:
                fval = ct_info.ct_class;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, fval));
                break;
            case TM_CUT_THROUGH_PORT_INFOt_MAX_CREDIT_CELLSf:
                fval = ct_info.max_credit_cells;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, fval));
                break;
            case TM_CUT_THROUGH_PORT_INFOt_EGR_XMIT_START_COUNT_BYTESf:
                for (idx = 0; idx < MAX_TM_CUT_THROUGH_CLASS; idx++) {
                    fval = ct_info.xmit_start_cnt[idx];
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, out, fid, idx, fval));
                }
                break;
            case TM_CUT_THROUGH_PORT_INFOt_FIFO_THD_CELLSf:
                fval = ct_info.fifo_thd_cells;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, fval));
                break;
            case TM_CUT_THROUGH_PORT_INFOt_OPERATIONAL_STATEf:
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, opcode));
                break;
            default:
                break;
        }
        fid++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_info_next(unit, op_arg, NULL, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_info_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    bcmtm_lport_t lport;
    uint64_t lport_min, lport_max;
    bcmltd_sid_t ltid = arg->sid;
    bcmltd_fid_t fid = TM_CUT_THROUGH_PORT_INFOt_PORT_IDf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &lport_min, &lport_max));

    if (in) {
        lport = ++(in->field[fid]->data);
    } else {
        lport = lport_min;
    }
    if (lport > lport_max) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    out->field[0]->data = lport;
    out->field[0]->idx = 0;
    out->field[0]->id = fid;

    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_info_lookup(unit, op_arg, out, out, arg));
exit:
    SHR_FUNC_EXIT();
}
