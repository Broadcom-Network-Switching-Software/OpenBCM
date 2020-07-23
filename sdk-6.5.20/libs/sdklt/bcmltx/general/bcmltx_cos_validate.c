/*! \file bcmltx_cos_validate.c
 *
 * Validates the number of unicast COS and multicast COS allowed for the
 * device based on TM_SCHEDULER_CONFIG.MC_Q_MODE.
 * This validates for non-cpu ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/bcmtm/bcmltx_mc_q_validate.h>
#include <bcmltx/bcmtm/bcmltx_uc_q_validate.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_uc_q_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_field_val_arg_t *arg)
{
    uint8_t num_ucq;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_num_ucq_get(unit, &num_ucq));

    if (in->count && in->field[0]) {
        if (in->field[0]-> data >= num_ucq) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mc_q_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_field_val_arg_t *arg)
{
    uint8_t num_mcq;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_num_mcq_get(unit, &num_mcq));
    if (in->count && in->field[0]) {
        if (in->field[0]-> data >= num_mcq) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}
