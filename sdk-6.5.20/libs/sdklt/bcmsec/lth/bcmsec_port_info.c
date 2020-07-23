/*! \file bcmsec_port_info.c
 *
 * SEC port information.
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
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_port_info.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMSEC_CTH


/*******************************************************************************
 * Public functions
 */
int
bcmsec_port_info_validate(int unit,
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
bcmsec_port_info_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    bcmlrd_fid_t fid;
    bcmsec_drv_t *drv;
    uint64_t val64;
    const bcmltd_field_t *in_fld;
    int lport, pport, opcode;
    const bcmlrd_field_data_t *field;
    sec_pt_port_info_t info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->port_info_get, SHR_E_UNAVAIL);

    in_fld = in->field[0];
    fid = SEC_PORT_INFOt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, in_fld, &val64));
    lport = (uint32_t) val64;

    opcode = PORT_INFO_UNAVAIL;
    sal_memset(&info, 0, sizeof(sec_pt_port_info_t));
    if (SHR_SUCCESS(bcmsec_port_phys_port_get(unit, lport, &pport))) {
        if (SHR_SUCCESS(drv->port_info_get(unit, pport, &info))) {
            opcode = info.oper;
        }
    }

    out->count = 0;

    while (fid < SEC_PORT_INFOt_FIELD_COUNT) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, arg->sid, fid, &field)) {
            fid++;
            continue;
        }
        switch (fid) {
            case SEC_PORT_INFOt_PORT_IDf:
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, out, fid, 0, lport));
                break;
            case SEC_PORT_INFOt_SECf:
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, out, fid, 0, info.valid));
                break;
            case SEC_PORT_INFOt_SEC_BLOCK_IDf:
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, out, fid, 0, info.sec_blk));
                break;
            case SEC_PORT_INFOt_SEC_PORT_NUMf:
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, out, fid, 0,
                                           info.sec_local_port));
                break;
            case SEC_PORT_INFOt_OPERATIONAL_STATEf:
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, out, fid, 0, opcode));
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
bcmsec_port_info_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_EXIT
        (bcmsec_port_info_next(unit, op_arg, NULL, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    bcmsec_lport_t lport;
    uint64_t lport_min, lport_max;
    bcmltd_sid_t ltid = arg->sid;
    bcmltd_fid_t fid = SEC_PORT_INFOt_PORT_IDf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmsec_field_value_range_get(unit, ltid, fid, &lport_min, &lport_max));

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
        (bcmsec_port_info_lookup(unit, op_arg, out, out, arg));
exit:
    SHR_FUNC_EXIT();
}
