/*! \file bcmltx_oper_status.c
 *
 * TM Table operational status.
 *
 * This file contains field transform for operational state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_port_q_oper_state.h>
#include <bcmltx/bcmtm/bcmltx_port_oper_state.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

#define VALID 0
#define PORT_INFO_UNAVAIL 1
#define UC_Q_INVALID 2
#define MC_Q_INVALID 2

int
bcmltx_port_q_oper_state_rev_transform(int unit,
                                       const bcmltd_fields_t *in_key,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    uint32_t q_id;
    int num_q;
    uint64_t opcode = VALID;

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = (bcmtm_lport_t) (in_key->field[0]->data);

    /* Queue ID */
    q_id = (uint32_t)(in_key->field[1]->data);


    if (arg->value[0]) {
        if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, lport, &num_q))) {
            opcode = PORT_INFO_UNAVAIL;
        }
        else if (q_id >= (uint32_t)num_q) {
            opcode = MC_Q_INVALID;
        }
    } else{
        if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_q))) {
            opcode = PORT_INFO_UNAVAIL;
        }
        else if (q_id >= (uint32_t)num_q) {
            opcode = UC_Q_INVALID;
        }
    }
    /* updating operational status. */
    out->field[0]->data = opcode;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    SHR_FUNC_EXIT();
}

int
bcmltx_port_oper_state_rev_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int pport;
    uint64_t opcode = VALID;

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = (bcmtm_lport_t) (in_key->field[0]->data);
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        opcode = PORT_INFO_UNAVAIL;
    }

    /* updating operational status */
    out->field[0]->data = opcode;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

    SHR_FUNC_EXIT();
}
