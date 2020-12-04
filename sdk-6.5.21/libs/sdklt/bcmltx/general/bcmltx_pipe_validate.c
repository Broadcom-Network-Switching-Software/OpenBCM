/*! \file bcmltx_pipe_validate.c
 *
 * Built-in Field Validation functions -
 *
 * 1) Input pipe validation
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltx/general/bcmltx_pipe_validate.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_pipe_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_field_val_arg_t *arg)
{
    bcmlrd_sid_t sid;
    uint32_t pipe_idx;

    SHR_FUNC_ENTER(unit);

    /* Sanity check inputs,outputs,parameters */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((in->field[0] == NULL) ||
        (arg->values == 0) ||
        (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid validation information\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sid = arg->value[0];
    pipe_idx = in->field[0]->data;
    if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "invalid input pipe %d. \n"), pipe_idx));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}
