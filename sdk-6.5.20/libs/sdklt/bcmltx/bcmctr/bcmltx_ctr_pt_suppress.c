/*! \file bcmltx_ctr_pt_suppress.c
 *
 * CTR PT Suppress.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcmctr/bcmltx_ctr_pt_suppress.h>
#include <bcmltx/bcmctr/bcmltx_ctr_port_q_pt_suppress.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_CTR

int
bcmltx_ctr_pt_suppress_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    uint32_t lport;
    int pport;
    uint32_t pt_suppress = 0;
    bcmdrd_pbmp_t lb_pbmp;

    SHR_FUNC_ENTER(unit);

    lport = in->field[0]->data;
    if (bcmdrd_dev_logic_port_pipe(unit, lport) == -1) {
        pt_suppress = 1;
    } else {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            pt_suppress = 1;
        } else {
          SHR_IF_ERR_EXIT(bcmdrd_dev_lb_pbmp(unit, &lb_pbmp));
          if (BCMDRD_PBMP_MEMBER(lb_pbmp, pport)) {
              pt_suppress = 1;
          }
        }

    }
    out->field[0]->data = pt_suppress;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ctr_port_q_pt_suppress_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t lport;
    uint32_t q_id;
    uint32_t pt_suppress = 0;
    int num_q = -1;

    SHR_FUNC_ENTER(unit);

    lport = in->field[0]->data;
    q_id = in->field[1]->data;

    if (bcmdrd_dev_logic_port_pipe(unit, lport) == -1) {
        pt_suppress = 1;
    } else {
        if (arg->value[0]) {
            /* Mutlicast queue ID */
            if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, lport, &num_q))) {
                pt_suppress = 1;
            }
        } else {
            /* Unicast queue ID */
            if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_q))) {
                pt_suppress = 1;
            }
        }

        if ((int)q_id >= num_q) {
            pt_suppress = 1;
        }
    }
    out->field[0]->data = pt_suppress;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->count = 1;

    SHR_FUNC_EXIT();
}
