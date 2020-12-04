/*! \file bcmltx_ctr_oper_state.c
 *
 * CTR Operational State.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcmctr/bcmltx_ctr_oper_state.h>
#include <bcmltx/bcmctr/bcmltx_ctr_port_q_oper_state.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_CTR

typedef enum {
    CTR_PORT_STATE_VALID = 0,
    CTR_PORT_STATE_PORT_INFO_INVALID = 1,
    CTR_PORT_STATE_COUNTER_COLLECT_DISABLED = 2
} ctr_port_oper_state_t;

typedef enum {
    CTR_PORT_Q_STATE_VALID = 0,
    CTR_PORT_Q_STATE_PORT_INFO_INVALID = 1,
    CTR_PORT_Q_STATE_QUEUE_INVALID = 2,
    CTR_PORT_Q_STATE_COUNTER_COLLECT_DISABLED = 3
} ctr_port_q_oper_state_t;

int
bcmltx_ctr_oper_state_rev_transform(int unit,
                                    const bcmltd_fields_t *in_key,
                                    const bcmltd_fields_t *in_value,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t lport;
    int pport;
    ctr_port_oper_state_t oper_state = 0;
    bool port_map_check, port_col_check;
    bcmdrd_pbmp_t lb_pbmp;

    SHR_FUNC_ENTER(unit);

    lport = in_key->field[0]->data;
    port_map_check = arg->value[0];
    port_col_check = TRUE;

    if (bcmdrd_dev_logic_port_pipe(unit, lport) == -1) {
        oper_state = CTR_PORT_STATE_PORT_INFO_INVALID;
        port_col_check = FALSE;
        port_map_check = FALSE;
    }
    if (port_map_check) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            oper_state = CTR_PORT_STATE_PORT_INFO_INVALID;
            port_col_check = FALSE;
        } else {
            SHR_IF_ERR_EXIT(bcmdrd_dev_lb_pbmp(unit, &lb_pbmp));
            if (BCMDRD_PBMP_MEMBER(lb_pbmp, pport)) {
                oper_state = CTR_PORT_STATE_PORT_INFO_INVALID;
                port_col_check = FALSE;
            }
        }
    }
    if (port_col_check) {
       if (!bcmptm_cci_port_collect_enabled(unit, lport)) {
           oper_state = CTR_PORT_STATE_COUNTER_COLLECT_DISABLED;
       }
    }

    out->field[0]->data = oper_state;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->count = 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_ctr_port_q_oper_state_rev_transform(int unit,
                                           const bcmltd_fields_t *in_key,
                                           const bcmltd_fields_t *in_value,
                                           bcmltd_fields_t *out,
                                           const bcmltd_transform_arg_t *arg)
{
    uint32_t lport;
    uint32_t q_id;
    ctr_port_q_oper_state_t oper_state = 0;
    int num_q = -1;
    bool q_check = TRUE, port_col_check = TRUE;

    SHR_FUNC_ENTER(unit);

    lport = in_key->field[0]->data;
    q_id = in_key->field[1]->data;

    if (bcmdrd_dev_logic_port_pipe(unit, lport) == -1) {
        oper_state = CTR_PORT_Q_STATE_PORT_INFO_INVALID;
        q_check = FALSE;
        port_col_check = FALSE;
    } else {
        if (arg->value[0]) {
            /* Mutlicast queue ID */
            if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, lport, &num_q))) {
                oper_state = CTR_PORT_Q_STATE_PORT_INFO_INVALID;
                q_check = FALSE;
                port_col_check = FALSE;
            }
        } else {
            /* Unicast queue ID */
            if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_q))) {
                oper_state = CTR_PORT_Q_STATE_PORT_INFO_INVALID;
                q_check = FALSE;
                port_col_check = FALSE;
            }
        }
    }

    if (q_check) {
        if ((int)q_id >= num_q) {
            oper_state = CTR_PORT_Q_STATE_QUEUE_INVALID;
            port_col_check = FALSE;
        }
    }

    if (port_col_check) {
       if (!bcmptm_cci_evict_collect_enabled(unit)) {
           oper_state = CTR_PORT_Q_STATE_COUNTER_COLLECT_DISABLED;
       }
    }

    out->field[0]->data = oper_state;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->count = 1;

    SHR_FUNC_EXIT();
}
