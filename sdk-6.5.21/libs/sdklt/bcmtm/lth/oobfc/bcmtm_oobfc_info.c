/*! \file bcmtm_oobfc_info.c
 *
 * Logical Table Custom Handlers for LT TM_OOBFC_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 INCLUDES
 */

#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/oobfc/bcmtm_lt_oobfc_info.h>
#include <bcmtm/bcmtm_drv.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CTH
/*******************************************************************************
* Public functions
 */

int
bcmtm_lt_oobfc_info_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    bcmlrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_lt_field_validate(unit, opcode, sid, in));

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            break;
        default:
            /* Insert, update, delete are not supported because of read-only. */
            SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_insert(int unit,
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

    SHR_ERR_EXIT(SHR_E_ACCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    bcmtm_drv_info_get(unit, &drv_info);
    out->field[TM_OOBFC_INFOt_CLOCK_CYCLE_DURATIONf]->id = 0;
    out->field[TM_OOBFC_INFOt_CLOCK_CYCLE_DURATIONf]->data =
                                    drv_info->oobfc_clock_ns;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_delete(int unit,
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

    SHR_ERR_EXIT(SHR_E_ACCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_update(int unit,
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

    SHR_ERR_EXIT(SHR_E_ACCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_first(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lt_oobfc_info_lookup(unit, op_arg, in, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_oobfc_info_next(int unit,
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

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

