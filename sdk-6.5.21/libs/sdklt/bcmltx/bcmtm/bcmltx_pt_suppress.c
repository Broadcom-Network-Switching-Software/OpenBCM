/*! \file bcmltx_pt_suppress.c
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
#include <bcmltx/bcmtm/bcmltx_port_pt_suppress.h>
#include <bcmltx/bcmtm/bcmltx_port_q_pt_suppress.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

int
bcmltx_port_q_pt_suppress_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    uint32_t q_id;
    int num_q;
    uint64_t suppress = 0; /*disable suppress */

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = (bcmtm_lport_t) (in->field[0]->data);
    q_id = (uint32_t)(in->field[1]->data);

    if (arg->value[0]) {
        /* Mutlicast queue ID */
        if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, lport, &num_q))) {
            suppress = 1;
        }
    } else {
        /* Unicast queue ID */
        if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_q))) {
            suppress = 1;
        }
    }

    if (q_id >= (uint32_t)num_q) {
        suppress = 1;
    }

    /* updating operational status. */
    out->field[0]->data = suppress;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->count = 1;

    SHR_FUNC_EXIT();
}

int
bcmltx_port_pt_suppress_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    uint64_t suppress = 0;  /* disable suppress */
    int pport;

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = (bcmtm_lport_t)(in->field[0]->data);

    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        suppress = 1;
    }

    /* updating operational status */
    out->field[0]->data = suppress;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->count = 1;

    SHR_FUNC_EXIT();
}
